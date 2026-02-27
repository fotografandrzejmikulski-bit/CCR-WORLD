#pragma once

#include "CoreMinimal.h"
#include "CCRTypes.h"
#include "Blueprint/UserWidget.h"
#include "CCRDialogueWidget.generated.h"

/**
 * UCCRDialogueWidget
 *
 * C++ base class for the UMG dialogue/choice widget.
 * Blueprint WBP_CCRDialogue should derive from this class.
 *
 * Listens to UCCRNarrativeRuntimeSubsystem events and exposes
 * Blueprint-implementable handlers to drive the UI.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCR_API UCCRDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ---- Blueprint-implementable event handlers ----

	/** Called when a dialogue node is active. Override in Blueprint to display text. */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|UI")
	void OnDialogueNode(const FText& Text, FName SpeakerTag);

	/** Called when a choice node is active. Override in Blueprint to show buttons. */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|UI")
	void OnChoicePresented(const TArray<FCCRChoiceOption>& Choices);

	/** Called when the narrative ends in this chunk. */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|UI")
	void OnNarrativeEnd();

	// ---- Called by Blueprint choice buttons ----
	UFUNCTION(BlueprintCallable, Category = "CCR|UI")
	void SelectChoice(int32 ChoiceIndex);

	/** Advance dialogue (tap to continue) */
	UFUNCTION(BlueprintCallable, Category = "CCR|UI")
	void AdvanceDialogue();

private:
	UFUNCTION()
	void HandleNodeChanged(FName NodeId);

	UFUNCTION()
	void HandleChoicePresented(const TArray<FCCRChoiceOption>& Choices);
};
