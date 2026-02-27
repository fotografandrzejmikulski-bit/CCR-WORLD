#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRPhotoModeSubsystem.generated.h"

/**
 * UCCRPhotoModeSubsystem
 *
 * Minimal screenshot capture subsystem for CCR WORLD.
 *
 * Provides Blueprint-callable screenshot capture using Unreal's built-in
 * FScreenshotRequest API.  On mobile, screenshots are saved to the default
 * screenshots directory (inside the app's Documents folder).
 *
 * Features:
 *   - CaptureScreenshot() — fire-and-forget capture with an auto-generated filename.
 *   - CaptureScreenshotWithPath() — capture to a specified path.
 *   - OnScreenshotCaptured delegate — notifies Blueprint when the file is ready.
 *   - HideHUD() / ShowHUD() wrappers for clean "no-UI" screenshots.
 *
 * For a full "photo mode" with frozen time and camera controls, extend this
 * subsystem in Blueprint and call PauseGame() before capturing.
 */
UCLASS(BlueprintType)
class CCR_API UCCRPhotoModeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Request a screenshot with an auto-generated filename.
	 * The filename is "CCR_<ISO8601_timestamp>.png".
	 * The screenshot is saved asynchronously; OnScreenshotCaptured fires when done.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Photo")
	void CaptureScreenshot();

	/**
	 * Request a screenshot with an explicit filename (no extension needed;
	 * Unreal appends ".png" automatically).
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Photo")
	void CaptureScreenshotWithFilename(const FString& Filename);

	/**
	 * Hide the HUD before capturing.  Call ShowHUD() to restore.
	 * Delegates to APlayerController::SetShowHUD(false).
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Photo")
	void HideHUD();

	/** Restore HUD visibility after a screenshot. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Photo")
	void ShowHUD();

	// ---- Delegate ----

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCCRScreenshotCaptured, const FString&, FilePath);

	/** Fired after the screenshot PNG has been written to disk. */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Photo")
	FOnCCRScreenshotCaptured OnScreenshotCaptured;

private:
	FDelegateHandle ScreenshotHandle;

	void HandleScreenshotCaptured(int32 Width, int32 Height, const TArray<FColor>& Colors);
	FString PendingFilename;
};
