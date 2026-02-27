#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCRObjectiveSubsystem.h"
#include "CCRObjectiveWidget.generated.h"

/**
 * UCCRObjectiveWidget
 *
 * C++ base class for the in-game objective / quest tracker overlay.
 * Blueprint WBP_CCRObjective should derive from this class.
 *
 * NativeConstruct subscribes to UCCRObjectiveSubsystem::OnObjectiveChanged.
 * Changes fire OnObjectiveUpdated() so Blueprint can animate the list in/out,
 * update item labels, and play completion sounds.
 *
 * Additionally, NativeConstruct fires OnFullRefresh() so Blueprint can
 * populate the initial list from GetActiveObjectives().
 *
 * The widget is created by ACCRGameHUD at CCRZOrder::Objectives (= 4) and
 * remains always-present but collapsed when no objectives are active.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCR_API UCCRObjectiveWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/**
	 * Called when any objective changes state.
	 * Override in Blueprint to update the objective list item.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|Objective")
	void OnObjectiveUpdated(const FCCRObjective& Objective, ECCRObjectiveState NewState);

	/**
	 * Called once on NativeConstruct and whenever Refresh() is called.
	 * Override in Blueprint to rebuild the complete objectives list from
	 * GetActiveObjectives().
	 * @param AllObjectives  All registered objectives (including hidden ones)
	 *                       so Blueprint can show the full history if desired.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|Objective")
	void OnFullRefresh(const TArray<FCCRObjective>& AllObjectives);

	/**
	 * Manually request a full refresh.  Useful after loading a save when all
	 * objectives are synced but OnObjectiveChanged fires per item.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Objective")
	void Refresh();

private:
	UFUNCTION()
	void HandleObjectiveChanged(const FCCRObjective& Objective, ECCRObjectiveState NewState);
};
