#include "CCRSpawnPoint.h"
#include "CCRSpawnSubsystem.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITORONLY_DATA
#include "Components/BillboardComponent.h"
#endif

ACCRSpawnPoint::ACCRSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

#if WITH_EDITORONLY_DATA
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (SpriteComponent)
	{
		SpriteComponent->bHiddenInGame = true;
		RootComponent = SpriteComponent;
	}
#endif
}

void ACCRSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnTag.IsNone()) return;

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!GI) return;

	UCCRSpawnSubsystem* SpawnSys = GI->GetSubsystem<UCCRSpawnSubsystem>();
	if (!SpawnSys) return;

	// Warn if a point with the same tag is already registered (designer error).
	if (SpawnSys->HasSpawnPoint(SpawnTag))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ACCRSpawnPoint: tag '%s' is already registered — last registered location will be used. "
				 "Check your level for duplicate SpawnPoint actors."),
			*SpawnTag.ToString());
	}

	SpawnSys->RegisterSpawnPoint(SpawnTag, GetActorLocation(), GetActorRotation());
}

void ACCRSpawnPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!SpawnTag.IsNone())
	{
		if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
		{
			if (UCCRSpawnSubsystem* SpawnSys = GI->GetSubsystem<UCCRSpawnSubsystem>())
			{
				SpawnSys->UnregisterSpawnPoint(SpawnTag);
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}
