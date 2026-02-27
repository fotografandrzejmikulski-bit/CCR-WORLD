#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCRInteractableActor.generated.h"

/**
 * ACCRInteractableActor
 *
 * A level-placed actor that the player can tap / touch to trigger a narrative
 * event, start a dialogue chunk, or set a world-state flag.
 *
 * Features:
 *   - UCapsuleComponent (or custom collision) for tap hit detection.
 *   - UCCRNarrativeRuntimeSubsystem integration: StartChunk() on interaction.
 *   - World-state condition: only interactive when all Conditions are true.
 *   - World-state write: applies SetOps when interacted with.
 *   - OnInteracted() Blueprint-implementable event for custom logic.
 *   - bConsumedAfterUse: the actor disables itself after the first interaction.
 *
 * Touch detection:
 *   The actor registers an overlap with ACCRTouchController and detects taps
 *   via the camera hit-test. For a simple implementation, designers can
 *   add the trace logic in Blueprint; the C++ base provides the data.
 *
 * Visibility control:
 *   The actor's StaticMesh visibility is driven by UCCRWorldStateSubsystemV2
 *   flag "VISIBLE_<ActorTag>" if bUseWorldStateVisibility is true.
 */
UCLASS(BlueprintType, Blueprintable)
class CCR_API ACCRInteractableActor : public AActor
{
	GENERATED_BODY()

public:
	ACCRInteractableActor();

	virtual void BeginPlay() override;

	// ---- Narrative integration ----

	/**
	 * Chunk to start when this actor is interacted with.
	 * Leave None to skip narrative and only apply SetOps / fire OnInteracted.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Interactable")
	FName InteractionChunkId;

	/** Entry node within InteractionChunkId (uses chunk entry node if None). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Interactable")
	FName InteractionEntryNodeId;

	// ---- Conditions ----

	/** All conditions must be true for the interaction to be available. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Interactable")
	TArray<FCCRCondition> Conditions;

	// ---- World-state writes ----

	/** These operations are applied when the actor is interacted with. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Interactable")
	TArray<FCCRSetOp> InteractionSetOps;

	// ---- Behaviour ----

	/**
	 * When true, the actor disables further interactions after the first successful one.
	 * The world-state flag "USED_<InteractionTag>" is set to track consumption across saves.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Interactable")
	bool bConsumedAfterUse = false;

	/**
	 * Unique tag identifying this interactable in world state.
	 * Used for the "USED_<InteractionTag>" consumed flag.
	 * Should be unique per level.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Interactable")
	FName InteractionTag;

	/**
	 * Interact with this actor.
	 * Called by Blueprint (e.g. from a touch trace) or by C++ when a tap is detected.
	 * Evaluates conditions, applies SetOps, starts narrative chunk, fires OnInteracted.
	 * Returns true when the interaction was processed successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Interactable")
	bool Interact();

	/**
	 * Returns true when this actor can currently be interacted with.
	 * Checks Conditions and the "consumed" flag if bConsumedAfterUse is true.
	 */
	UFUNCTION(BlueprintPure, Category = "CCR|Interactable")
	bool CanInteract() const;

	/**
	 * Called when the interaction is triggered and all conditions are met.
	 * Override in Blueprint for custom visual / audio feedback.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Interactable")
	void OnInteracted();

private:
	bool EvaluateConditions() const;
	void ApplySetOps();
};
