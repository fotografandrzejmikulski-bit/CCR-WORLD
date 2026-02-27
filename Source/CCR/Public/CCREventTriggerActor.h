#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCRTypes.h"
#include "CCREventTriggerActor.generated.h"

/**
 * ACCREventTriggerActor
 *
 * A level-placed trigger volume that fires world-state events when
 * a pawn enters (or optionally leaves) the trigger area.
 *
 * Features:
 *   - UCapsuleComponent / USphereComponent trigger volume (Blueprint-assignable).
 *   - Optional conditions: only triggers when all Conditions are true.
 *   - World-state writes: applies SetOps on trigger.
 *   - Optional narrative chunk: starts a chunk on first entry.
 *   - bTriggerOnce: auto-disables after first activation.
 *   - bTriggerOnOverlapEnd: also fires on pawn exit.
 *   - World-state flag tracking: "TRIG_<TriggerTag>_FIRED" persists whether
 *     the trigger has fired so it doesn't re-fire after save/load.
 */
UCLASS(BlueprintType, Blueprintable)
class CCR_API ACCREventTriggerActor : public AActor
{
	GENERATED_BODY()

public:
	ACCREventTriggerActor();

	virtual void BeginPlay() override;

	// ---- Identification ----

	/** Unique tag for this trigger (used for "TRIG_<Tag>_FIRED" world-state flag). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Trigger")
	FName TriggerTag;

	// ---- Conditions ----

	/** All conditions must be true for the trigger to fire. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Trigger")
	TArray<FCCRCondition> Conditions;

	// ---- World-state writes ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Trigger")
	TArray<FCCRSetOp> TriggerSetOps;

	// ---- Narrative chunk ----

	/** Chunk to start when trigger fires (leave None to skip). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Trigger")
	FName TriggerChunkId;

	/** Entry node within TriggerChunkId. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Trigger")
	FName TriggerEntryNodeId;

	// ---- Behaviour ----

	/** When true, the trigger fires only once per save slot. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Trigger")
	bool bTriggerOnce = true;

	/** When true, also fires when the pawn exits the trigger volume. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Trigger")
	bool bTriggerOnOverlapEnd = false;

	// ---- Events ----

	/** Called when the trigger fires (pawn enters and conditions met). */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Trigger")
	void OnTriggerFired(AActor* TriggeringActor);

	/** Called when the pawn exits the trigger (only when bTriggerOnOverlapEnd = true). */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Trigger")
	void OnTriggerExited(AActor* ExitingActor);

	/** Manually reset the trigger so it can fire again (clears the FIRED flag). */
	UFUNCTION(BlueprintCallable, Category = "CCR|Trigger")
	void ResetTrigger();

protected:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                        bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Root collision component – blueprint can replace with any shape. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CCR|Trigger")
	TObjectPtr<class USphereComponent> TriggerSphere;

private:
	bool HasFired() const;
	void Fire(AActor* TriggeringActor);
	bool EvaluateConditions() const;
	void ApplySetOps();
};
