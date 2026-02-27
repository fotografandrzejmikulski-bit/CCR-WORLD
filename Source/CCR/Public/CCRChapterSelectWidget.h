#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCRChapterSelectWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCCRChapterSelectWidgetClosed);

/**
 * FCCRChapterSelectEntry
 *
 * Data for a single row in the chapter select UI.
 */
USTRUCT(BlueprintType)
struct FCCRChapterSelectEntry
{
	GENERATED_BODY()

	/** Chunk to load when the player selects this chapter. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ChunkId;

	/** Human-readable chapter title displayed in the list. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	/** Optional subtitle or location description. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Subtitle;

	/**
	 * When true, this chapter is locked.
	 * A chapter becomes unlocked when the player has reached its entry node
	 * (i.e. the world-state flag "CHAPTER_<ChunkId>_REACHED" is true).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLocked = true;
};

/**
 * UCCRChapterSelectWidget
 *
 * C++ base class for the chapter-select / chapter-replay screen.
 * Blueprint WBP_CCRChapterSelect should derive from this class.
 *
 * NativeConstruct assembles the list of chapters from the registered entries
 * and fires OnChapterListReady(Entries) so Blueprint can populate a scroll box.
 * Chapters are locked unless the player has previously visited them
 * (checked via UCCRWorldStateSubsystemV2 flag "CHAPTER_<ChunkId>_REACHED").
 *
 * The widget is typically accessible from the main menu or pause menu.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCR_API UCCRChapterSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/**
	 * The list of chapters to display.
	 * Populate this in the Blueprint defaults or assign before adding to viewport.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|ChapterSelect")
	TArray<FCCRChapterSelectEntry> ChapterEntries;

	/**
	 * Called when the chapter list has been assembled and unlock states resolved.
	 * Override in Blueprint to populate the scroll box.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|ChapterSelect")
	void OnChapterListReady(const TArray<FCCRChapterSelectEntry>& Entries);

	/**
	 * Select and load a chapter.
	 * If the chapter is locked, this is a no-op and OnChapterLocked is fired instead.
	 * @param ChunkId   The chunk to load.
	 * @param NodeId    The entry node (use the chunk's default entry if None).
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|ChapterSelect")
	void SelectChapter(FName ChunkId, FName NodeId = NAME_None);

	/**
	 * Called when the player attempts to select a locked chapter.
	 * Override in Blueprint to show a "locked" animation or message.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|ChapterSelect")
	void OnChapterLocked(FName ChunkId);

	/** Close (collapse) the chapter select widget and notify listeners. */
	UFUNCTION(BlueprintCallable, Category = "CCR|ChapterSelect")
	void Close();

	/** Fired when Close() is called; owning menus subscribe to this. */
	UPROPERTY(BlueprintAssignable, Category = "CCR|ChapterSelect")
	FOnCCRChapterSelectWidgetClosed OnClosed;

	/** World-state flag prefix used to mark visited chapters. */
	static FName ChapterReachedFlag(FName ChunkId)
	{
		return FName(*FString::Printf(TEXT("CHAPTER_%s_REACHED"), *ChunkId.ToString()));
	}

private:
	TArray<FCCRChapterSelectEntry> BuildEntryList() const;
};
