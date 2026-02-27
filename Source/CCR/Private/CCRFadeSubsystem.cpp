#include "CCRFadeSubsystem.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

APlayerCameraManager* UCCRFadeSubsystem::GetCameraManager() const
{
	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			return PC->PlayerCameraManager;
		}
	}
	return nullptr;
}

// ---------------------------------------------------------------------------
// FadeTo
// ---------------------------------------------------------------------------

void UCCRFadeSubsystem::FadeTo(float DurationSec, FLinearColor Color)
{
	APlayerCameraManager* Cam = GetCameraManager();
	if (!Cam) return;

	bFading = true;

	// StartCameraFade: from alpha 0 to 1 = fade TO opaque color (covers screen)
	Cam->StartCameraFade(/*FromAlpha=*/0.f, /*ToAlpha=*/1.f, DurationSec,
	                     Color, /*bShouldFadeAudio=*/false, /*bHoldWhenFinished=*/true);

	if (DurationSec <= 0.f)
	{
		bFading = false;
		OnFadedIn.Broadcast();
		return;
	}

	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		World->GetTimerManager().SetTimer(FadeTimer,
			FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				bFading = false;
				OnFadedIn.Broadcast();
			}),
			DurationSec, /*bLoop=*/false);
	}
}

// ---------------------------------------------------------------------------
// FadeFrom
// ---------------------------------------------------------------------------

void UCCRFadeSubsystem::FadeFrom(float DurationSec, FLinearColor Color)
{
	APlayerCameraManager* Cam = GetCameraManager();
	if (!Cam) return;

	bFading = true;

	// StartCameraFade: from alpha 1 to 0 = reveal (fade FROM opaque color)
	Cam->StartCameraFade(/*FromAlpha=*/1.f, /*ToAlpha=*/0.f, DurationSec,
	                     Color, /*bShouldFadeAudio=*/false, /*bHoldWhenFinished=*/false);

	if (DurationSec <= 0.f)
	{
		bFading = false;
		OnFadedOut.Broadcast();
		return;
	}

	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		World->GetTimerManager().SetTimer(FadeTimer,
			FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				bFading = false;
				OnFadedOut.Broadcast();
			}),
			DurationSec, /*bLoop=*/false);
	}
}

// ---------------------------------------------------------------------------
// FadeToAndFrom
// ---------------------------------------------------------------------------

void UCCRFadeSubsystem::FadeToAndFrom(float FadeInSec, float HoldSec, float FadeOutSec,
                                      FLinearColor Color)
{
	APlayerCameraManager* Cam = GetCameraManager();
	if (!Cam) return;

	bFading = true;

	// Fade to opaque (don't call FadeTo() to avoid the delegate-and-timer conflict)
	Cam->StartCameraFade(0.f, 1.f, FMath::Max(FadeInSec, 0.f),
	                     Color, false, /*bHoldWhenFinished=*/true);

	const float TotalDelay    = FadeInSec + HoldSec;
	const float FadeOutSecCap = FadeOutSec;
	const FLinearColor ColorCap = Color;

	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		// After FadeIn + Hold: fire OnCutReady then begin fade-out
		World->GetTimerManager().SetTimer(FadeTimer,
			FTimerDelegate::CreateWeakLambda(this, [this, FadeOutSecCap, ColorCap]()
			{
				OnCutReady.Broadcast();
				FadeFrom(FadeOutSecCap, ColorCap);
			}),
			FMath::Max(TotalDelay, KINDA_SMALL_NUMBER), false);
	}
}

// ---------------------------------------------------------------------------
// CancelFade
// ---------------------------------------------------------------------------

void UCCRFadeSubsystem::CancelFade()
{
	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		World->GetTimerManager().ClearTimer(FadeTimer);
	}

	if (APlayerCameraManager* Cam = GetCameraManager())
	{
		Cam->StopCameraFade();
	}

	bFading = false;
}
