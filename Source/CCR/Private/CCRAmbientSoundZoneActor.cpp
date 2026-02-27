#include "CCRAmbientSoundZoneActor.h"
#include "CCRAudioSubsystem.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

ACCRAmbientSoundZoneActor::ACCRAmbientSoundZoneActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneSphere"));
	ZoneSphere->SetSphereRadius(ZoneRadius);
	ZoneSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ZoneSphere->SetGenerateOverlapEvents(true);
	RootComponent = ZoneSphere;
}

void ACCRAmbientSoundZoneActor::BeginPlay()
{
	Super::BeginPlay();

	// Apply the zone radius in case it was changed in the editor
	ZoneSphere->SetSphereRadius(ZoneRadius);

	ZoneSphere->OnComponentBeginOverlap.AddDynamic(this, &ACCRAmbientSoundZoneActor::HandleBeginOverlap);
	ZoneSphere->OnComponentEndOverlap.AddDynamic(this,  &ACCRAmbientSoundZoneActor::HandleEndOverlap);
}

void ACCRAmbientSoundZoneActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ZoneSphere->OnComponentBeginOverlap.RemoveDynamic(this, &ACCRAmbientSoundZoneActor::HandleBeginOverlap);
	ZoneSphere->OnComponentEndOverlap.RemoveDynamic(this,  &ACCRAmbientSoundZoneActor::HandleEndOverlap);
	Super::EndPlay(EndPlayReason);
}

void ACCRAmbientSoundZoneActor::HandleBeginOverlap(
	UPrimitiveComponent* /*OverlappedComp*/,
	AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/,
	bool /*bFromSweep*/,
	const FHitResult& /*SweepResult*/)
{
	if (!Cast<APawn>(OtherActor)) return;
	if (bPlayerInside) return; // already inside

	bPlayerInside = true;

	if (MusicAssetId.IsNone()) return;

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (UCCRAudioSubsystem* Audio = GI ? GI->GetSubsystem<UCCRAudioSubsystem>() : nullptr)
	{
		// Remember what was playing so we can restore on exit
		PreviousMusicId = Audio->GetCurrentMusicId();
		Audio->PlayMusic(MusicAssetId, CrossfadeSec);
	}
}

void ACCRAmbientSoundZoneActor::HandleEndOverlap(
	UPrimitiveComponent* /*OverlappedComp*/,
	AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/)
{
	if (!Cast<APawn>(OtherActor)) return;
	if (!bPlayerInside) return;

	bPlayerInside = false;

	if (!bRestoreOnExit) return;

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (UCCRAudioSubsystem* Audio = GI ? GI->GetSubsystem<UCCRAudioSubsystem>() : nullptr)
	{
		// Restore the previous track (or stop if there was none)
		if (!PreviousMusicId.IsNone())
		{
			Audio->PlayMusic(PreviousMusicId, CrossfadeSec);
		}
		else
		{
			Audio->StopMusic(CrossfadeSec);
		}
	}
}
