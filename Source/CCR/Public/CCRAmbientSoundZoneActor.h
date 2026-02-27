#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCRAmbientSoundZoneActor.generated.h"

/**
 * UCCAmbientSoundZoneActor
 *
 * A sphere-trigger actor that changes the ambient background music or
 * ambient sound loop when the player enters/exits the zone.
 *
 * Uses UCCRAudioSubsystem::PlayMusic() for seamless crossfades so that
 * entering a dark cave triggers the eerie cave music track without a hard cut.
 *
 * Each level can contain multiple overlapping zones.  Zones are prioritised
 * by Priority value; the highest priority active zone wins.
 *
 * When the player exits all zones, the DefaultMusicAssetId is played
 * (set on UCCRAudioSubsystem).
 */
UCLASS(BlueprintType, Blueprintable)
class CCR_API ACCRAmbientSoundZoneActor : public AActor
{
	GENERATED_BODY()

public:
	ACCRAmbientSoundZoneActor();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Music asset ID to play when the player enters this zone. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Sound")
	FName MusicAssetId;

	/** Crossfade duration in seconds (passed to UCCRAudioSubsystem::PlayMusic). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Sound",
	          meta = (ClampMin = "0.0"))
	float CrossfadeSec = 1.5f;

	/**
	 * Priority for zone stacking.  Higher value = higher priority.
	 * When multiple zones overlap, the zone with the highest Priority determines
	 * the active music track.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Sound")
	int32 Priority = 0;

	/**
	 * When true, restores the previous music track (or the game default) when
	 * the player exits the zone.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Sound")
	bool bRestoreOnExit = true;

	/** Sphere radius. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Sound",
	          meta = (ClampMin = "1.0"))
	float ZoneRadius = 500.f;

protected:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                        bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CCR|Sound",
	          meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> ZoneSphere;

	FName PreviousMusicId;
	bool bPlayerInside = false;
};
