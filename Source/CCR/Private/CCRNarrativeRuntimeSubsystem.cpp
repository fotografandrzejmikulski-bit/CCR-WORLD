#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRWorldStateSubsystemV2.h"
#include "CCRAsyncNarrativeLoaderSubsystem.h"
#include "CCRPerformanceGovernorSubsystem.h"
#include "CCRStoryRegistrySubsystem.h"
#include "CCRStoryChunk.h"
#include "Engine/AssetManager.h"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

void UCCRNarrativeRuntimeSubsystem::RebuildCache()
{
	NodeIndexCache.Reset();
	if (!CurrentChunk) return;

	const TArray<FCCRNode>& Nodes = CurrentChunk->Nodes;
	NodeIndexCache.Reserve(Nodes.Num());
	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		NodeIndexCache.Add(Nodes[i].NodeId, i);
	}
}

const FCCRNode* UCCRNarrativeRuntimeSubsystem::FindNode(FName NodeId) const
{
	if (!CurrentChunk) return nullptr;
	const int32* Idx = NodeIndexCache.Find(NodeId);
	if (!Idx) return nullptr;
	if (!CurrentChunk->Nodes.IsValidIndex(*Idx)) return nullptr;
	return &CurrentChunk->Nodes[*Idx];
}

bool UCCRNarrativeRuntimeSubsystem::EvaluateConditions(const TArray<FCCRCondition>& Conditions) const
{
	UCCRWorldStateSubsystemV2* WSM = GetGameInstance()->GetSubsystem<UCCRWorldStateSubsystemV2>();
	if (!WSM) return true;

	for (const FCCRCondition& Cond : Conditions)
	{
		float ActualValue = 0.f;
		switch (Cond.ValueType)
		{
		case ECCRStateValueType::Flag:  ActualValue = WSM->GetFlag(Cond.Key) ? 1.f : 0.f; break;
		case ECCRStateValueType::Float: ActualValue = WSM->GetFloat(Cond.Key); break;
		case ECCRStateValueType::Int:   ActualValue = static_cast<float>(WSM->GetInt(Cond.Key)); break;
		}

		bool bPass = false;
		switch (Cond.CompareOp)
		{
		case ECCRCompareOp::Equals:         bPass = FMath::IsNearlyEqual(ActualValue, Cond.CompareValue); break;
		case ECCRCompareOp::NotEquals:      bPass = !FMath::IsNearlyEqual(ActualValue, Cond.CompareValue); break;
		case ECCRCompareOp::Less:           bPass = ActualValue <  Cond.CompareValue; break;
		case ECCRCompareOp::LessOrEqual:    bPass = ActualValue <= Cond.CompareValue; break;
		case ECCRCompareOp::Greater:        bPass = ActualValue >  Cond.CompareValue; break;
		case ECCRCompareOp::GreaterOrEqual: bPass = ActualValue >= Cond.CompareValue; break;
		}
		if (!bPass) return false;
	}
	return true;
}

void UCCRNarrativeRuntimeSubsystem::ApplySetOps(const TArray<FCCRSetOp>& SetOps)
{
	UCCRWorldStateSubsystemV2* WSM = GetGameInstance()->GetSubsystem<UCCRWorldStateSubsystemV2>();
	if (!WSM) return;

	for (const FCCRSetOp& Op : SetOps)
	{
		switch (Op.ValueType)
		{
		case ECCRStateValueType::Flag:  WSM->SetFlag(Op.Key, Op.Value != 0.f); break;
		case ECCRStateValueType::Float: WSM->SetFloat(Op.Key, Op.Value); break;
		case ECCRStateValueType::Int:   WSM->SetInt(Op.Key, static_cast<int32>(Op.Value)); break;
		}
	}
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void UCCRNarrativeRuntimeSubsystem::StartChunk(UCCRStoryChunk* Chunk, FName OverrideEntryNodeId)
{
	if (!Chunk) return;
	CurrentChunk = Chunk;
	RebuildCache();

	const FName EntryNode = OverrideEntryNodeId.IsNone() ? Chunk->EntryNodeId : OverrideEntryNodeId;
	ExecuteNode(EntryNode);
}

void UCCRNarrativeRuntimeSubsystem::SelectChoice(int32 ChoiceIndex)
{
	const FCCRNode* Node = FindNode(CurrentNodeId);
	if (!Node || Node->NodeType != ECCRNodeType::Choice) return;
	if (!Node->Choices.IsValidIndex(ChoiceIndex)) return;

	ExecuteNode(Node->Choices[ChoiceIndex].TargetNodeId);
}

void UCCRNarrativeRuntimeSubsystem::ResolveQTE(bool bSuccess)
{
	const FCCRNode* Node = FindNode(CurrentNodeId);
	if (!Node || Node->NodeType != ECCRNodeType::QTE) return;

	ExecuteNode(bSuccess ? Node->QTESuccessNodeId : Node->QTEFailNodeId);
}

void UCCRNarrativeRuntimeSubsystem::AdvanceDialogue()
{
	const FCCRNode* Node = FindNode(CurrentNodeId);
	if (!Node || Node->NodeType != ECCRNodeType::Dialogue) return;

	if (!Node->NextAfterDialogue.IsNone())
	{
		ExecuteNode(Node->NextAfterDialogue);
	}
}

void UCCRNarrativeRuntimeSubsystem::FinishCinematic()
{
	const FCCRNode* Node = FindNode(CurrentNodeId);
	if (!Node || Node->NodeType != ECCRNodeType::Cinematic) return;

	if (!Node->NextAfterCinematic.IsNone())
	{
		ExecuteNode(Node->NextAfterCinematic);
	}
}

bool UCCRNarrativeRuntimeSubsystem::GetCurrentNode(FCCRNode& OutNode) const
{
	const FCCRNode* Node = FindNode(CurrentNodeId);
	if (!Node) return false;
	OutNode = *Node;
	return true;
}

// ---------------------------------------------------------------------------
// Node execution
// ---------------------------------------------------------------------------

void UCCRNarrativeRuntimeSubsystem::ExecuteNode(FName NodeId)
{
	if (NodeId.IsNone()) return;

	const FCCRNode* Node = FindNode(NodeId);
	if (!Node)
	{
		UE_LOG(LogTemp, Warning, TEXT("UCCRNarrativeRuntimeSubsystem: node '%s' not found in chunk '%s'"),
			*NodeId.ToString(), CurrentChunk ? *CurrentChunk->ChunkId.ToString() : TEXT("null"));
		return;
	}

	CurrentNodeId = NodeId;
	OnNodeChanged.Broadcast(NodeId);

	if (Node->bCheckpoint)
	{
		OnCheckpointRequested.Broadcast(NodeId);
	}

	// Predictive preload
	PreloadPredictedFromNode(NodeId);

	switch (Node->NodeType)
	{
	case ECCRNodeType::Dialogue:
		// Dialogue text is consumed by UI; runtime just holds position.
		break;

	case ECCRNodeType::Choice:
	{
		// Filter choices by conditions
		TArray<FCCRChoiceOption> Visible;
		for (const FCCRChoiceOption& Option : Node->Choices)
		{
			if (EvaluateConditions(Option.ShowConditions))
			{
				Visible.Add(Option);
			}
		}
		OnChoicePresented.Broadcast(Visible);
		break;
	}

	case ECCRNodeType::Condition:
		if (EvaluateConditions(Node->Conditions))
			ExecuteNode(Node->ConditionTrueNodeId);
		else
			ExecuteNode(Node->ConditionFalseNodeId);
		break;

	case ECCRNodeType::SetValues:
		ApplySetOps(Node->SetOps);
		ExecuteNode(Node->NextAfterSet);
		break;

	case ECCRNodeType::QTE:
		// Execution paused; resolved via ResolveQTE()
		break;

	case ECCRNodeType::Cinematic:
		// Cinematic playback handled externally; runtime waits.
		break;

	case ECCRNodeType::Jump:
	{
		// Cross-chunk jump: async-load the target chunk then start it.
		const FName TargetChunkId   = Node->TargetChunkId;
		const FName EntryNodeId     = Node->EntryNodeInTarget;

		UCCRStoryRegistrySubsystem* Registry =
			GetGameInstance()->GetSubsystem<UCCRStoryRegistrySubsystem>();
		if (!Registry) break;

		const FPrimaryAssetId AssetId = Registry->GetAssetIdForChunk(TargetChunkId);
		if (!AssetId.IsValid())
		{
			UE_LOG(LogTemp, Warning,
				TEXT("UCCRNarrativeRuntimeSubsystem: Jump target chunk '%s' not found in registry."),
				*TargetChunkId.ToString());
			break;
		}

		// PreloadChunks stores a FStreamableHandle in UCCRAsyncNarrativeLoaderSubsystem
		// (the "Required" bucket) so the asset is kept alive for the duration of the
		// new chunk's use. RequestAsyncLoad below is the trigger that fires our
		// completion callback; the Loader's handle prevents the chunk from being
		// GC'd between the callback and StartChunk returning.
		if (UCCRAsyncNarrativeLoaderSubsystem* Loader =
			GetGameInstance()->GetSubsystem<UCCRAsyncNarrativeLoaderSubsystem>())
		{
			TArray<FPrimaryAssetId> Required;
			Required.Add(AssetId);
			Loader->PreloadChunks(Required, {});
		}

		const FSoftObjectPath AssetPath = UAssetManager::Get().GetPrimaryAssetPath(AssetId);

		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			AssetPath,
			FStreamableDelegate::CreateWeakLambda(this,
				[this, AssetId, EntryNodeId]()
				{
					UCCRStoryChunk* Chunk = Cast<UCCRStoryChunk>(
						UAssetManager::Get().GetPrimaryAssetObject(AssetId));
					if (Chunk)
					{
						StartChunk(Chunk, EntryNodeId);
					}
					else
					{
						UE_LOG(LogTemp, Warning,
							TEXT("UCCRNarrativeRuntimeSubsystem: Jump — failed to get chunk object after async load."));
					}
				}));
		break;
	}

	case ECCRNodeType::End:
		break;

	default:
		break;
	}
}

// ---------------------------------------------------------------------------
// Predictive preloading
// ---------------------------------------------------------------------------

void UCCRNarrativeRuntimeSubsystem::PreloadPredictedFromNode(FName NodeId)
{
	UCCRAsyncNarrativeLoaderSubsystem* Loader =
		GetGameInstance() ? GetGameInstance()->GetSubsystem<UCCRAsyncNarrativeLoaderSubsystem>() : nullptr;
	UCCRPerformanceGovernorSubsystem* Governor =
		GetGameInstance() ? GetGameInstance()->GetSubsystem<UCCRPerformanceGovernorSubsystem>() : nullptr;
	if (!Loader || !Governor) return;

	TSet<FName> ChunkIds;
	TSet<FName> Visited;
	TraceToJumpTargets(NodeId, ChunkIds, Visited, /*Depth=*/0);

	TArray<FPrimaryAssetId> Predicted;
	Predicted.Reserve(ChunkIds.Num());

	const int32 MaxPredicted = Governor->GetMaxPredictedChunks();
	int32 Count = 0;
	for (const FName& ChId : ChunkIds)
	{
		if (Count >= MaxPredicted) break;
		Predicted.Add(FPrimaryAssetId(TEXT("CCRStoryChunk"), ChId));
		++Count;
	}

	Loader->PreloadChunks({}, Predicted);
}

void UCCRNarrativeRuntimeSubsystem::TraceToJumpTargets(
	FName NodeId,
	TSet<FName>& OutChunkIds,
	TSet<FName>& Visited,
	int32 Depth) const
{
	static constexpr int32 MaxDepth = 8;
	if (Depth >= MaxDepth) return;
	if (Visited.Contains(NodeId)) return;
	Visited.Add(NodeId);

	const FCCRNode* Node = FindNode(NodeId);
	if (!Node) return;

	switch (Node->NodeType)
	{
	case ECCRNodeType::Jump:
		if (!Node->TargetChunkId.IsNone())
			OutChunkIds.Add(Node->TargetChunkId);
		break;

	case ECCRNodeType::Choice:
		for (const FCCRChoiceOption& Opt : Node->Choices)
			TraceToJumpTargets(Opt.TargetNodeId, OutChunkIds, Visited, Depth + 1);
		break;

	case ECCRNodeType::QTE:
		TraceToJumpTargets(Node->QTESuccessNodeId, OutChunkIds, Visited, Depth + 1);
		TraceToJumpTargets(Node->QTEFailNodeId,    OutChunkIds, Visited, Depth + 1);
		break;

	case ECCRNodeType::Condition:
		TraceToJumpTargets(Node->ConditionTrueNodeId,  OutChunkIds, Visited, Depth + 1);
		TraceToJumpTargets(Node->ConditionFalseNodeId, OutChunkIds, Visited, Depth + 1);
		break;

	case ECCRNodeType::Dialogue:
		TraceToJumpTargets(Node->NextAfterDialogue, OutChunkIds, Visited, Depth + 1);
		break;

	case ECCRNodeType::SetValues:
		TraceToJumpTargets(Node->NextAfterSet, OutChunkIds, Visited, Depth + 1);
		break;

	case ECCRNodeType::Cinematic:
		TraceToJumpTargets(Node->NextAfterCinematic, OutChunkIds, Visited, Depth + 1);
		break;

	default:
		break;
	}
}
