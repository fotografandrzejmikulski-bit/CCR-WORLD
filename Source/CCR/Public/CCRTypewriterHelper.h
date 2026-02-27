#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CCRTypewriterHelper.generated.h"

/**
 * UCCRTypewriterHelper
 *
 * A lightweight UObject that drives character-by-character text reveal for
 * dialogue widgets (typewriter effect).
 *
 * Usage from a Blueprint or C++ widget:
 * @code
 *   TypewriterHelper = NewObject<UCCRTypewriterHelper>(this);
 *   TypewriterHelper->OnCharRevealed.AddDynamic(this, &UMyWidget::HandleCharRevealed);
 *   TypewriterHelper->OnComplete.AddDynamic(this, &UMyWidget::HandleTypewriteComplete);
 *   TypewriterHelper->Start(FullText, CharsPerSecond);
 * @endcode
 *
 * Call Skip() to instantly reveal the full text and fire OnComplete.
 * The helper uses a world timer internally and is safe to be GC'd at any
 * time (the timer is cleared in the destructor).
 *
 * Recommended construction: `NewObject<UCCRTypewriterHelper>(OwningWidget)` so
 * the helper's lifetime is tied to the widget.  Assign it to a UPROPERTY on
 * the widget to prevent premature GC.
 */
UCLASS(BlueprintType)
class CCR_API UCCRTypewriterHelper : public UObject
{
	GENERATED_BODY()

public:
	virtual void BeginDestroy() override;

	// ---- API ----

	/**
	 * Begin revealing FullText one character at a time.
	 * Any in-progress typewrite is stopped first.
	 * @param FullText        The complete text to reveal.
	 * @param CharsPerSecond  Reveal speed (default: 30 characters/second).
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Typewriter")
	void Start(const FText& FullText, float CharsPerSecond = 30.f);

	/**
	 * Instantly reveal all remaining characters and fire OnComplete.
	 * Safe to call even when not active.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Typewriter")
	void Skip();

	/** Stop and reset without firing OnComplete. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Typewriter")
	void Stop();

	/** Returns true while characters are still being revealed. */
	UFUNCTION(BlueprintPure, Category = "CCR|Typewriter")
	bool IsActive() const { return bActive; }

	/** Returns the number of characters revealed so far. */
	UFUNCTION(BlueprintPure, Category = "CCR|Typewriter")
	int32 GetRevealedCount() const { return RevealedCount; }

	/** Returns the full target text. */
	UFUNCTION(BlueprintPure, Category = "CCR|Typewriter")
	FText GetFullText() const { return TargetText; }

	// ---- Delegates ----

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCCRCharRevealed,
	                                               const FText&, PartialText,
	                                               int32, RevealedCount,
	                                               int32, TotalCount);

	/**
	 * Fired each time a character is revealed.
	 * PartialText is a substring of the full text up to RevealedCount characters.
	 */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Typewriter")
	FOnCCRCharRevealed OnCharRevealed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCCRTypewriteComplete);

	/** Fired when all characters have been revealed (naturally or via Skip()). */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Typewriter")
	FOnCCRTypewriteComplete OnComplete;

private:
	FText TargetText;
	FString TargetString; // FString cache for efficient substring ops
	int32 TotalCount  = 0;
	int32 RevealedCount = 0;
	bool  bActive     = false;

	FTimerHandle TimerHandle;

	void RevealNextChar();
	void ClearTimer();
	UWorld* GetWorld() const override;
};
