#include "CCRConditionalActorComponent.h"
#include "CCRWorldStateSubsystemV2.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UCCRConditionalActorComponent::UCCRConditionalActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCCRConditionalActorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
		{
			NRS->OnNodeChanged.AddDynamic(this, &UCCRConditionalActorComponent::HandleNodeChanged);
		}
	}

	// Apply initial visibility
	Evaluate();
}

void UCCRConditionalActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
		{
			NRS->OnNodeChanged.RemoveDynamic(this, &UCCRConditionalActorComponent::HandleNodeChanged);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void UCCRConditionalActorComponent::HandleNodeChanged(FName NodeId)
{
	Evaluate();
}

void UCCRConditionalActorComponent::Evaluate()
{
	bool bVisible = EvaluateConditions();
	if (bInvertResult) bVisible = !bVisible;
	ApplyVisibility(bVisible);
}

bool UCCRConditionalActorComponent::EvaluateConditions() const
{
	if (Conditions.IsEmpty()) return true;

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	UCCRWorldStateSubsystemV2* WSM = GI ? GI->GetSubsystem<UCCRWorldStateSubsystemV2>() : nullptr;
	return WSM ? WSM->EvaluateConditions(Conditions) : true;
}

void UCCRConditionalActorComponent::ApplyVisibility(bool bVisible)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	Owner->SetActorHiddenInGame(!bVisible);
	if (bToggleCollision)
	{
		Owner->SetActorEnableCollision(bVisible);
	}
}
