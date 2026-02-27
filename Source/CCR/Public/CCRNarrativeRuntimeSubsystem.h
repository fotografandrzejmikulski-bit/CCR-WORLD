#pragma once

#include "CoreMinimal.h"
#include "CCRTypes.h"
#include "CCRStoryChunk.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRNarrativeRuntimeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCCRNodeChanged,  FName, NodeId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCCRChoicePresented, const TArray<FCCRChoiceOption>&, Choices);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCCRCheckpointRequested, FName, NodeId);

/**
 * UCCRNarrativeRuntimeSubsystem
 *
 * Executes the narrative node graph.
 * Integrates with UCCRWorldStateSubsystemV2 for condition evaluation and SetOps.
 * Emits events: OnNodeChanged, OnChoicePresented, OnCheckpointRequested.
 * Predictively preloads referenced chunks.
 */
UCLASS()
class CCR_API UCCRNarrativeRuntimeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// ---- Events ----
	UPROPERTY(BlueprintAssignable, Category = "CCR|Narrative")
	FOnCCRNodeChanged OnNodeChanged;

	UPROPERTY(BlueprintAssignable, Category = "CCR|Narrative")
	FOnCCRChoicePresented OnChoicePresented;

	UPROPERTY(BlueprintAssignable, Category = "CCR|Narrative")
	FOnCCRCheckpointRequested OnCheckpointRequested;

	// ---- Public API ----

	/** Begin narrative from an already-loaded chunk */
	UFUNCTION(BlueprintCallable, Category = "CCR|Narrative")
	void StartChunk(UCCRStoryChunk* Chunk, FName OverrideEntryNodeId = NAME_None);

	/** Player selects a choice at index */
	UFUNCTION(BlueprintCallable, Category = "CCR|Narrative")
	void SelectChoice(int32 ChoiceIndex);

	/** QTE result callback */
	UFUNCTION(BlueprintCallable, Category = "CCR|Narrative")
	void ResolveQTE(bool bSuccess);

	/** Advance from a Dialogue node to the next node (tap-to-continue) */
	UFUNCTION(BlueprintCallable, Category = "CCR|Narrative")
	void AdvanceDialogue();

	/**
	 * Called by UCCRCinematicSubsystem when the Level Sequence finishes.
	 * Moves the narrative to the node referenced by NextAfterCinematic.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Narrative")
	void FinishCinematic();

	UFUNCTION(BlueprintPure, Category = "CCR|Narrative")
	FName GetCurrentNodeId() const { return CurrentNodeId; }

	UFUNCTION(BlueprintPure, Category = "CCR|Narrative")
	FName GetCurrentChunkId() const { return CurrentChunk ? CurrentChunk->ChunkId : NAME_None; }

	/**
	 * Returns a copy of the current node data so that UI can read dialogue text,
	 * speaker tag, etc. Returns false if no node is active.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Narrative")
	bool GetCurrentNode(FCCRNode& OutNode) const;

	/** Predictive preloading: follow node graph and enqueue referenced chunks */
	UFUNCTION(BlueprintCallable, Category = "CCR|Narrative")
	void PreloadPredictedFromNode(FName NodeId);

private:
	UPROPERTY()
	UCCRStoryChunk* CurrentChunk = nullptr;

	FName CurrentNodeId;

	/** Cache: NodeId -> index in CurrentChunk->Nodes */
	TMap<FName, int32> NodeIndexCache;

	void ExecuteNode(FName NodeId);
	const FCCRNode* FindNode(FName NodeId) const;
	void RebuildCache();
	bool EvaluateConditions(const TArray<FCCRCondition>& Conditions) const;
	void ApplySetOps(const TArray<FCCRSetOp>& SetOps);

	/** Recursively trace targets for predictive loading (loop-safe) */
	void TraceToJumpTargets(FName NodeId, TSet<FName>& OutChunkIds, TSet<FName>& Visited, int32 Depth) const;
};
