#include "CCRChapterSelectWidget.h"
#include "CCRWorldStateSubsystemV2.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRStoryRegistrySubsystem.h"
#include "CCRGameMode.h"
#include "CCRStoryChunk.h"
#include "Engine/AssetManager.h"

void UCCRChapterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	const TArray<FCCRChapterSelectEntry> Entries = BuildEntryList();
	OnChapterListReady(Entries);
}

TArray<FCCRChapterSelectEntry> UCCRChapterSelectWidget::BuildEntryList() const
{
	UGameInstance* GI = GetGameInstance();
	UCCRWorldStateSubsystemV2* WSM = GI ? GI->GetSubsystem<UCCRWorldStateSubsystemV2>() : nullptr;

	TArray<FCCRChapterSelectEntry> Result = ChapterEntries;

	for (FCCRChapterSelectEntry& Entry : Result)
	{
		if (Entry.ChunkId.IsNone()) continue;

		// A chapter is unlocked when the player has visited its entry node.
		// The flag is set by UCCRNarrativeRuntimeSubsystem::StartChunk().
		if (WSM)
		{
			Entry.bLocked = !WSM->GetFlag(UCCRChapterSelectWidget::ChapterReachedFlag(Entry.ChunkId));
		}
	}

	return Result;
}

void UCCRChapterSelectWidget::SelectChapter(FName ChunkId, FName NodeId)
{
	if (ChunkId.IsNone()) return;

	// Check whether the chapter is locked
	UGameInstance* GI = GetGameInstance();
	if (UCCRWorldStateSubsystemV2* WSM = GI ? GI->GetSubsystem<UCCRWorldStateSubsystemV2>() : nullptr)
	{
		if (!WSM->GetFlag(ChapterReachedFlag(ChunkId)))
		{
			OnChapterLocked(ChunkId);
			return;
		}
	}

	// Load the chunk and start narrative
	UCCRStoryRegistrySubsystem* Registry = GI ? GI->GetSubsystem<UCCRStoryRegistrySubsystem>() : nullptr;
	UCCRNarrativeRuntimeSubsystem* NRS   = GI ? GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>() : nullptr;
	if (!Registry || !NRS) return;

	const FPrimaryAssetId AssetId = Registry->GetAssetIdForChunk(ChunkId);
	if (!AssetId.IsValid()) return;

	const FSoftObjectPath AssetPath = UAssetManager::Get().GetPrimaryAssetPath(AssetId);
	const FName CapturedNodeId = NodeId;

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		AssetPath,
		FStreamableDelegate::CreateWeakLambda(NRS, [NRS, AssetId, CapturedNodeId]()
		{
			UCCRStoryChunk* Chunk = Cast<UCCRStoryChunk>(
				UAssetManager::Get().GetPrimaryAssetObject(AssetId));
			if (Chunk)
			{
				NRS->StartChunk(Chunk, CapturedNodeId);
			}
		}));
}

void UCCRChapterSelectWidget::Close()
{
	OnClosed.Broadcast();
	SetVisibility(ESlateVisibility::Collapsed);
}

// ---------------------------------------------------------------------------
// BlueprintNativeEvent default implementations
// ---------------------------------------------------------------------------

void UCCRChapterSelectWidget::OnChapterListReady_Implementation(const TArray<FCCRChapterSelectEntry>& Entries)
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRChapterSelectWidget::OnChapterListReady has no C++ or Blueprint implementation."))
#endif
}

void UCCRChapterSelectWidget::OnChapterLocked_Implementation(FName ChunkId)
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRChapterSelectWidget::OnChapterLocked has no C++ or Blueprint implementation."))
#endif
}
