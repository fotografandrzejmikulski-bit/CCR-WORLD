#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CCRGameHUD.generated.h"

class UCCRDialogueWidget;
class UCCRQTEWidget;

/**
 * ACCRGameHUD
 *
 * Manages the in-game widget stack.
 * Creates the dialogue widget and QTE widget on BeginPlay and
 * adds them to the viewport. Widgets are hidden/shown by game phase
 * or by Blueprint logic deriving from the C++ base widget classes.
 *
 * Widget class references are soft-pointed so Blueprint sub-classes
 * (WBP_CCRDialogue, WBP_CCRQTE) can be assigned in the editor without
 * requiring hard C++ dependencies.
 */
UCLASS()
class CCR_API ACCRGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	// ---- Widget class references (assign Blueprint subclasses in editor) ----

	/** Blueprint subclass of UCCRDialogueWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRDialogueWidget> DialogueWidgetClass;

	/** Blueprint subclass of UCCRQTEWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRQTEWidget> QTEWidgetClass;

	// ---- Live widget instances ----

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRDialogueWidget* DialogueWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRQTEWidget* QTEWidget = nullptr;

	/** Show or hide the dialogue panel */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void SetDialogueVisible(bool bVisible);

	/** Show or hide the QTE overlay */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void SetQTEVisible(bool bVisible);
};
