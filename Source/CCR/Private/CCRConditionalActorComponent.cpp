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
	if (!WSM) return true;

	for (const FCCRCondition& Cond : Conditions)
	{
		float ActualValue = 0.f;
		switch (Cond.ValueType)
		{
		case ECCRStateValueType::Flag:  ActualValue = WSM->GetFlag(Cond.Key) ? 1.f : 0.f; break;
		case ECCRStateValueType::Float: ActualValue = WSM->GetFloat(Cond.Key); break;
		case ECCRStateValueType::Int:   ActualValue = static_cast<float>(WSM->GetInt(Cond.Key)); break;
		}

		bool bPass = false;
		switch (Cond.CompareOp)
		{
		case ECCRCompareOp::Equals:         bPass = FMath::IsNearlyEqual(ActualValue, Cond.CompareValue); break;
		case ECCRCompareOp::NotEquals:      bPass = !FMath::IsNearlyEqual(ActualValue, Cond.CompareValue); break;
		case ECCRCompareOp::Less:           bPass = ActualValue <  Cond.CompareValue; break;
		case ECCRCompareOp::LessOrEqual:    bPass = ActualValue <= Cond.CompareValue; break;
		case ECCRCompareOp::Greater:        bPass = ActualValue >  Cond.CompareValue; break;
		case ECCRCompareOp::GreaterOrEqual: bPass = ActualValue >= Cond.CompareValue; break;
		}
		if (!bPass) return false;
	}
	return true;
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
