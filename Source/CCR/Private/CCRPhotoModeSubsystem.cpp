#include "CCRPhotoModeSubsystem.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "HighResScreenshot.h"
#include "UnrealEngine.h"

void UCCRPhotoModeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Subscribe to the engine screenshot delegate so we know when the file is ready.
	ScreenshotHandle = UGameViewportClient::OnScreenshotCaptured().AddWeakLambda(this,
		[this](int32 Width, int32 Height, const TArray<FColor>& Colors)
		{
			HandleScreenshotCaptured(Width, Height, Colors);
		});
}

void UCCRPhotoModeSubsystem::Deinitialize()
{
	UGameViewportClient::OnScreenshotCaptured().Remove(ScreenshotHandle);
	ScreenshotHandle.Reset();
	Super::Deinitialize();
}

void UCCRPhotoModeSubsystem::CaptureScreenshot()
{
	const FString Filename = FString::Printf(TEXT("CCR_%s"),
		*FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
	CaptureScreenshotWithFilename(Filename);
}

void UCCRPhotoModeSubsystem::CaptureScreenshotWithFilename(const FString& Filename)
{
	PendingFilename = Filename;
	// RequestScreenshot queues an async capture that fires OnScreenshotCaptured.
	// bAddFilenameSuffix = false keeps the filename as given.
	FScreenshotRequest::RequestScreenshot(Filename, /*bShowUI=*/false, /*bAddFilenameSuffix=*/false);
}

void UCCRPhotoModeSubsystem::HideHUD()
{
	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			PC->SetShowHUD(false);
		}
	}
}

void UCCRPhotoModeSubsystem::ShowHUD()
{
	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			PC->SetShowHUD(true);
		}
	}
}

void UCCRPhotoModeSubsystem::HandleScreenshotCaptured(int32 Width, int32 Height, const TArray<FColor>& Colors)
{
	// Build the expected file path (Unreal saves to the Screenshots folder)
	const FString SavedDir = FPaths::ScreenShotDir();
	const FString FilePath = FPaths::Combine(SavedDir, PendingFilename + TEXT(".png"));

	OnScreenshotCaptured.Broadcast(FilePath);
	PendingFilename.Empty();
}
