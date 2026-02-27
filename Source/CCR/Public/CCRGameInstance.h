#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CCRGameInstance.generated.h"

/**
 * UCCRGameInstance
 *
 * Custom Game Instance for CCR WORLD.
 *
 * Responsibilities:
 *   - Serves as the root UObject owner for all UCCRXxxSubsystem instances
 *     (UE5 UGameInstanceSubsystem is registered here automatically by the engine).
 *   - Provides typed GetCCRSubsystem<T>() as a convenience shorthand.
 *   - Exposes a Blueprint-callable version for runtime use in Blueprint graphs.
 *   - Persists across level transitions so that all game-instance subsystems
 *     (narrative state, audio, world state, etc.) remain alive between levels.
 *
 * Usage:
 *   1. Open Project Settings → Maps & Modes → Game Instance Class → UCCRGameInstance
 *      (or assign a Blueprint subclass of UCCRGameInstance).
 *   2. Call UCCRGameInstance::Get(WorldContextObject) from anywhere to access
 *      the typed instance without casting.
 *
 * Blueprint subclasses can override Init() / Shutdown() to register speakers,
 * inventory items, music cues, etc. on game startup.
 */
UCLASS(BlueprintType, Blueprintable)
class CCR_API UCCRGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// ---- UGameInstance overrides ----
	virtual void Init() override;
	virtual void Shutdown() override;

	// ---- Static convenience accessor ----

	/**
	 * Returns the UCCRGameInstance for the given world context, or null if
	 * the running game instance is not a UCCRGameInstance.
	 */
	UFUNCTION(BlueprintPure, Category = "CCR", meta = (WorldContext = "WorldContextObject"))
	static UCCRGameInstance* Get(const UObject* WorldContextObject);

	// ---- Typed subsystem accessor (C++ only) ----

	/**
	 * Typed shorthand for GetSubsystem<T>().
	 * Returns null when the subsystem type is not registered.
	 */
	template<typename T>
	T* GetCCRSubsystem() const
	{
		return GetSubsystem<T>();
	}

	// ---- Blueprint-implementable events ----

	/**
	 * Called during Init() after all subsystems are initialised.
	 * Override in Blueprint to register speakers, music cues, VO cues,
	 * inventory item definitions, achievement definitions, etc.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR")
	void OnCCRInit();

	/**
	 * Called during Shutdown() before subsystems are torn down.
	 * Override in Blueprint for any cleanup the project requires.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR")
	void OnCCRShutdown();
};
