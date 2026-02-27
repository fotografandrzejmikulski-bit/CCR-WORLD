#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/ForceFeedbackEffect.h"
#include "CCRHapticLibrarySubsystem.generated.h"

/**
 * Named haptic feedback pattern.
 * Registered at startup via UCCRHapticLibrarySubsystem::RegisterPattern().
 */
USTRUCT(BlueprintType)
struct FCCRHapticPattern
{
	GENERATED_BODY()

	/**
	 * Duration of the effect in seconds.
	 * For a simple pulse pattern this is typically 0.1–0.3 s.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
	float Duration = 0.15f;

	/** Intensity multiplier [0..1]. Scaled against the device haptic API. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Intensity = 0.5f;

	/**
	 * Optional UE force-feedback asset. When set, it overrides Duration/Intensity
	 * and is played via PlayHapticEffect on the player controller.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UForceFeedbackEffect> ForceFeedbackEffect;

	/**
	 * Which controller hand receives the haptic effect when using a UForceFeedbackEffect.
	 * Has no effect on the PlayDynamicForceFeedback path (which affects all motors).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EControllerHand> Hand = EControllerHand::Right;
};

/**
 * UCCRHapticLibrarySubsystem
 *
 * Centralised registry and playback controller for named haptic / force-feedback
 * patterns.  Separates pattern data from playback call-sites so that designers
 * can tune intensities and durations from a single place without changing BP logic.
 *
 * Patterns are registered at game startup (from UCCRGameInstance::OnCCRInit).
 *
 * Usage:
 * @code
 *   HapticLib->RegisterPattern("QTE_Success",  { 0.2f, 1.0f });
 *   HapticLib->RegisterPattern("QTE_Failure",  { 0.4f, 0.3f });
 *   HapticLib->RegisterPattern("Checkpoint",   { 0.1f, 0.6f });
 *   HapticLib->PlayPattern("QTE_Success");
 * @endcode
 */
UCLASS(BlueprintType)
class CCR_API UCCRHapticLibrarySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Register a named pattern.
	 * If a pattern with PatternId already exists it is overwritten.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Haptic")
	void RegisterPattern(FName PatternId, const FCCRHapticPattern& Pattern);

	/** Unregister a pattern. Safe to call when the pattern doesn't exist. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Haptic")
	void UnregisterPattern(FName PatternId);

	/**
	 * Play the named haptic pattern on the first local player controller.
	 * No-op when:
	 *   - PatternId is None or not registered.
	 *   - No valid player controller is available.
	 *   - The device does not support force feedback.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Haptic")
	void PlayPattern(FName PatternId);

	/**
	 * Play the named pattern on a specific controller.
	 * Useful for split-screen scenarios (not typical on mobile but included for completeness).
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Haptic")
	void PlayPatternOnController(FName PatternId, APlayerController* PlayerController);

	/** Returns true when the pattern is registered. */
	UFUNCTION(BlueprintPure, Category = "CCR|Haptic")
	bool HasPattern(FName PatternId) const;

	/** Returns all registered pattern IDs. */
	UFUNCTION(BlueprintPure, Category = "CCR|Haptic")
	TArray<FName> GetAllPatternIds() const;

private:
	TMap<FName, FCCRHapticPattern> Patterns;

	void PlayPatternInternal(const FCCRHapticPattern& Pattern, APlayerController* PC);
};
