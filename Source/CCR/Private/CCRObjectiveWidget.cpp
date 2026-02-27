#include "CCRObjectiveWidget.h"
#include "CCRObjectiveSubsystem.h"

void UCCRObjectiveWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRObjectiveSubsystem* ObjSys = GI->GetSubsystem<UCCRObjectiveSubsystem>())
		{
			ObjSys->OnObjectiveChanged.AddDynamic(this, &UCCRObjectiveWidget::HandleObjectiveChanged);
		}
	}

	Refresh();
}

void UCCRObjectiveWidget::NativeDestruct()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRObjectiveSubsystem* ObjSys = GI->GetSubsystem<UCCRObjectiveSubsystem>())
		{
			ObjSys->OnObjectiveChanged.RemoveDynamic(this, &UCCRObjectiveWidget::HandleObjectiveChanged);
		}
	}
	Super::NativeDestruct();
}

void UCCRObjectiveWidget::Refresh()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRObjectiveSubsystem* ObjSys = GI->GetSubsystem<UCCRObjectiveSubsystem>())
		{
			OnFullRefresh(ObjSys->GetAllObjectives());
		}
	}
}

void UCCRObjectiveWidget::HandleObjectiveChanged(const FCCRObjective& Objective, ECCRObjectiveState NewState)
{
	OnObjectiveUpdated(Objective, NewState);
}

// ---------------------------------------------------------------------------
// BlueprintNativeEvent default implementations
// ---------------------------------------------------------------------------

void UCCRObjectiveWidget::OnObjectiveUpdated_Implementation(const FCCRObjective& Objective, ECCRObjectiveState NewState)
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRObjectiveWidget::OnObjectiveUpdated has no C++ or Blueprint implementation."))
#endif
}

void UCCRObjectiveWidget::OnFullRefresh_Implementation(const TArray<FCCRObjective>& AllObjectives)
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRObjectiveWidget::OnFullRefresh has no C++ or Blueprint implementation."))
#endif
}
