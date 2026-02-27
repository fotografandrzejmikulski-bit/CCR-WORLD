#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRInputMappingSubsystem.generated.h"

/**
 * CCR input action category.
 * Used to map named logical actions to physical keys / buttons.
 */
UENUM(BlueprintType)
enum class ECCRInputAction : uint8
{
	Advance        UMETA(DisplayName = "Advance"),         // tap-to-continue / confirm
	Skip           UMETA(DisplayName = "Skip"),            // skip cinematic
	Pause          UMETA(DisplayName = "Pause"),           // open pause menu
	QTE_Confirm    UMETA(DisplayName = "QTE_Confirm"),     // confirm QTE (non-touch)
	QTE_Cancel     UMETA(DisplayName = "QTE_Cancel"),      // cancel QTE
	DebugDump      UMETA(DisplayName = "DebugDump"),       // dump world state (debug)
};

/**
 * UCCRInputMappingSubsystem
 *
 * Provides a single Blueprint-callable API for keyboard / gamepad input mapping
 * as a non-touch fallback for desktop/console testing and accessibility.
 *
 * Complements ACCRTouchController which handles mobile touch gestures.
 * Registers console / keyboard bindings so that testers can run the game
 * without a touchscreen.
 *
 * Usage (from UCCRGameInstance::OnCCRInit()):
 * @code
 *   InputMapping->SimulateAction(ECCRInputAction::Advance);
 * @endcode
 *
 * ACCRTouchController calls SimulateAction() from its keyboard binding callbacks
 * if bound via UCCRInputMappingSubsystem.
 *
 * Note: On mobile shipping builds this subsystem registers no bindings and
 * SimulateAction() is a no-op, so it is safe to leave active.
 */
UCLASS(BlueprintType)
class CCR_API UCCRInputMappingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Simulate pressing a named logical action.
	 * Delegates to the appropriate subsystem API:
	 *   Advance       → UCCRNarrativeRuntimeSubsystem::AdvanceDialogue()
	 *   Skip          → UCCRCinematicSubsystem::SkipCinematic()
	 *   Pause         → ACCRGameHUD::TogglePause()
	 *   QTE_Confirm   → ACCRTouchController::EndQTE(true)  (via NRS::ResolveQTE)
	 *   QTE_Cancel    → ACCRTouchController::EndQTE(false) (via NRS::ResolveQTE)
	 *   DebugDump     → UCCRDebugSubsystem::DumpWorldState() + DumpNarrativeState()
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Input")
	void SimulateAction(ECCRInputAction Action);

	/**
	 * Returns the default keyboard key name for the given action.
	 * Useful for displaying key hints in the UI.
	 * Returns FString() if no default is defined.
	 */
	UFUNCTION(BlueprintPure, Category = "CCR|Input")
	FString GetDefaultKeyName(ECCRInputAction Action) const;
};
