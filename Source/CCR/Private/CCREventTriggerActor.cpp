#include "CCREventTriggerActor.h"
#include "CCRWorldStateSubsystemV2.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRStoryRegistrySubsystem.h"
#include "CCRStoryChunk.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

ACCREventTriggerActor::ACCREventTriggerActor()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetSphereRadius(150.f);
	TriggerSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	TriggerSphere->SetGenerateOverlapEvents(true);
	RootComponent = TriggerSphere;
}

void ACCREventTriggerActor::BeginPlay()
{
	Super::BeginPlay();

	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ACCREventTriggerActor::HandleBeginOverlap);

	if (bTriggerOnOverlapEnd)
	{
		TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &ACCREventTriggerActor::HandleEndOverlap);
	}
}

bool ACCREventTriggerActor::HasFired() const
{
	if (TriggerTag.IsNone()) return false;

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	UCCRWorldStateSubsystemV2* WSM = GI ? GI->GetSubsystem<UCCRWorldStateSubsystemV2>() : nullptr;
	if (!WSM) return false;

	return WSM->GetFlag(FName(*FString::Printf(TEXT("TRIG_%s_FIRED"), *TriggerTag.ToString())));
}

void ACCREventTriggerActor::HandleBeginOverlap(
	UPrimitiveComponent* /*OverlappedComp*/,
	AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/,
	bool /*bFromSweep*/,
	const FHitResult& /*SweepResult*/)
{
	// Only react to pawn overlaps
	if (!Cast<APawn>(OtherActor)) return;

	if (bTriggerOnce && HasFired()) return;
	if (!EvaluateConditions()) return;

	Fire(OtherActor);
}

void ACCREventTriggerActor::HandleEndOverlap(
	UPrimitiveComponent* /*OverlappedComp*/,
	AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/)
{
	if (!Cast<APawn>(OtherActor)) return;
	OnTriggerExited(OtherActor);
}

void ACCREventTriggerActor::Fire(AActor* TriggeringActor)
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);

	// Mark as fired
	if (!TriggerTag.IsNone())
	{
		if (UCCRWorldStateSubsystemV2* WSM = GI ? GI->GetSubsystem<UCCRWorldStateSubsystemV2>() : nullptr)
		{
			WSM->SetFlag(FName(*FString::Printf(TEXT("TRIG_%s_FIRED"), *TriggerTag.ToString())), true);
		}
	}

	// Apply world-state ops
	ApplySetOps();

	// Notify Blueprint
	OnTriggerFired(TriggeringActor);

	// Start narrative chunk
	if (!TriggerChunkId.IsNone() && GI)
	{
		UCCRStoryRegistrySubsystem* Registry = GI->GetSubsystem<UCCRStoryRegistrySubsystem>();
		UCCRNarrativeRuntimeSubsystem* NRS   = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();

		if (Registry && NRS)
		{
			const FPrimaryAssetId AssetId = Registry->GetAssetIdForChunk(TriggerChunkId);
			if (AssetId.IsValid())
			{
				const FSoftObjectPath AssetPath = UAssetManager::Get().GetPrimaryAssetPath(AssetId);
				const FName EntryNode = TriggerEntryNodeId;

				UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
					AssetPath,
					FStreamableDelegate::CreateWeakLambda(NRS, [NRS, AssetId, EntryNode]()
					{
						UCCRStoryChunk* Chunk = Cast<UCCRStoryChunk>(
							UAssetManager::Get().GetPrimaryAssetObject(AssetId));
						if (Chunk)
						{
							NRS->StartChunk(Chunk, EntryNode);
						}
					}));
			}
		}
	}
}

bool ACCREventTriggerActor::EvaluateConditions() const
{
	if (Conditions.IsEmpty()) return true;

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	UCCRWorldStateSubsystemV2* WSM = GI ? GI->GetSubsystem<UCCRWorldStateSubsystemV2>() : nullptr;
	return WSM ? WSM->EvaluateConditions(Conditions) : true;
}

void ACCREventTriggerActor::ApplySetOps()
{
	if (TriggerSetOps.IsEmpty()) return;

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	UCCRWorldStateSubsystemV2* WSM = GI ? GI->GetSubsystem<UCCRWorldStateSubsystemV2>() : nullptr;
	if (!WSM) return;

	for (const FCCRSetOp& Op : TriggerSetOps)
	{
		switch (Op.ValueType)
		{
		case ECCRStateValueType::Flag:
			WSM->SetFlag(Op.Key, Op.Value != 0.f);
			break;
		case ECCRStateValueType::Float:
			switch (Op.Mode)
			{
			case ECCRSetOpMode::Add:      WSM->SetFloat(Op.Key, WSM->GetFloat(Op.Key) + Op.Value); break;
			case ECCRSetOpMode::Multiply: WSM->SetFloat(Op.Key, WSM->GetFloat(Op.Key) * Op.Value); break;
			default:                      WSM->SetFloat(Op.Key, Op.Value); break;
			}
			break;
		case ECCRStateValueType::Int:
			switch (Op.Mode)
			{
			case ECCRSetOpMode::Add:
				WSM->SetInt(Op.Key, WSM->GetInt(Op.Key) + static_cast<int32>(Op.Value));
				break;
			case ECCRSetOpMode::Multiply:
				WSM->SetInt(Op.Key, FMath::RoundToInt32(WSM->GetInt(Op.Key) * Op.Value));
				break;
			default:
				WSM->SetInt(Op.Key, static_cast<int32>(Op.Value));
				break;
			}
			break;
		}
	}
}

void ACCREventTriggerActor::ResetTrigger()
{
	if (TriggerTag.IsNone()) return;

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (UCCRWorldStateSubsystemV2* WSM = GI ? GI->GetSubsystem<UCCRWorldStateSubsystemV2>() : nullptr)
	{
		WSM->SetFlag(FName(*FString::Printf(TEXT("TRIG_%s_FIRED"), *TriggerTag.ToString())), false);
	}
}
