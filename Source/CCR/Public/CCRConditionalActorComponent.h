#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CCRTypes.h"
#include "CCRConditionalActorComponent.generated.h"

/**
 * UCCRConditionalActorComponent
 *
 * A scene component that shows or hides its owner actor based on
 * UCCRWorldStateSubsystemV2 condition evaluation.
 *
 * Attach to any actor in the level to make it appear/disappear according to
 * narrative world state (e.g. "show the key prop only after flag DOOR_LOCKED is true").
 *
 * Evaluation is performed:
 *   - On BeginPlay
 *   - Every time UCCRNarrativeRuntimeSubsystem::OnNodeChanged fires (narrative advances)
 *   - Manually via Evaluate()
 *
 * When all Conditions are met the actor is made visible and has collision enabled.
 * When any condition fails the actor is hidden and collision is disabled.
 *
 * If bInvertResult is true the visibility logic is flipped (actor visible when
 * conditions are NOT met — useful for "show this object only before the player
 * picks it up").
 */
UCLASS(ClassGroup = "CCR", meta = (BlueprintSpawnableComponent))
class CCR_API UCCRConditionalActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCCRConditionalActorComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Conditions that must ALL be true for the actor to be visible. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Conditional")
	TArray<FCCRCondition> Conditions;

	/**
	 * When true, inverts the result: actor is visible when conditions are NOT met.
	 * Useful for "consumed / picked up" actors that should disappear after use.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Conditional")
	bool bInvertResult = false;

	/**
	 * When true, also toggle collision (no collision when hidden).
	 * Disable if the actor must block movement even when invisible.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Conditional")
	bool bToggleCollision = true;

	/** Manually re-evaluate conditions and update visibility. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Conditional")
	void Evaluate();

private:
	UFUNCTION()
	void HandleNodeChanged(FName NodeId);

	bool EvaluateConditions() const;
	void ApplyVisibility(bool bVisible);
};
