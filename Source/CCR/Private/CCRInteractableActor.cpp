#include "CCRInteractableActor.h"
#include "CCRWorldStateSubsystemV2.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRStoryRegistrySubsystem.h"
#include "CCRStoryChunk.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

ACCRInteractableActor::ACCRInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ACCRInteractableActor::BeginPlay()
{
	Super::BeginPlay();
}

bool ACCRInteractableActor::CanInteract() const
{
	// If consumed, check the world-state flag
	if (bConsumedAfterUse && !InteractionTag.IsNone())
	{
		UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
		if (UCCRWorldStateSubsystemV2* WSM = GI ? GI->GetSubsystem<UCCRWorldStateSubsystemV2>() : nullptr)
		{
			const FName ConsumedKey = FName(
				*FString::Printf(TEXT("USED_%s"), *InteractionTag.ToString()));
			if (WSM->GetFlag(ConsumedKey))
			{
				return false;
			}
		}
	}

	return EvaluateConditions();
}

bool ACCRInteractableActor::Interact()
{
	if (!CanInteract()) return false;

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!GI) return false;

	// Apply world-state writes
	ApplySetOps();

	// Mark as consumed
	if (bConsumedAfterUse && !InteractionTag.IsNone())
	{
		if (UCCRWorldStateSubsystemV2* WSM = GI->GetSubsystem<UCCRWorldStateSubsystemV2>())
		{
			WSM->SetFlag(FName(*FString::Printf(TEXT("USED_%s"), *InteractionTag.ToString())), true);
		}
	}

	// Notify Blueprint
	OnInteracted();

	// Start narrative chunk (if set)
	if (!InteractionChunkId.IsNone())
	{
		UCCRStoryRegistrySubsystem* Registry = GI->GetSubsystem<UCCRStoryRegistrySubsystem>();
		UCCRNarrativeRuntimeSubsystem* NRS   = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();

		if (Registry && NRS)
		{
			const FPrimaryAssetId AssetId = Registry->GetAssetIdForChunk(InteractionChunkId);
			if (AssetId.IsValid())
			{
				const FSoftObjectPath AssetPath = UAssetManager::Get().GetPrimaryAssetPath(AssetId);
				const FName EntryNode = InteractionEntryNodeId;

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

	return true;
}

bool ACCRInteractableActor::EvaluateConditions() const
{
	if (Conditions.IsEmpty()) return true;

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	UCCRWorldStateSubsystemV2* WSM = GI ? GI->GetSubsystem<UCCRWorldStateSubsystemV2>() : nullptr;
	return WSM ? WSM->EvaluateConditions(Conditions) : true;
}

void ACCRInteractableActor::ApplySetOps()
{
	if (InteractionSetOps.IsEmpty()) return;

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	UCCRWorldStateSubsystemV2* WSM = GI ? GI->GetSubsystem<UCCRWorldStateSubsystemV2>() : nullptr;
	if (!WSM) return;

	for (const FCCRSetOp& Op : InteractionSetOps)
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
