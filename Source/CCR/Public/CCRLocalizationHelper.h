#pragma once

#include "CoreMinimal.h"
#include "Internationalization/Culture.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CCRLocalizationHelper.generated.h"

/**
 * UCCRLocalizationHelper
 *
 * Blueprint Function Library providing a Blueprint-accessible API for culture /
 * language switching in CCR WORLD.  All functions are static and can be called
 * from Blueprint without instantiating any object.
 *
 * Usage:
 *   - Call GetAvailableCultureNames() to populate a language-selection dropdown.
 *   - Call SetCurrentCulture(CultureName) when the player selects a language.
 *   - The chosen culture is persisted via UCCRSettingsSubsystem (key stored in
 *     CCRSettingsSaveGame::CultureName) and restored on game startup.
 *
 * Text localisation itself is handled by the standard Unreal
 * FText / LOCTEXT system; this class only manages which culture is active.
 */
UCLASS()
class CCR_API UCCRLocalizationHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Return the list of culture names available in this build.
	 * Names follow IETF BCP 47 format (e.g. "en", "pl", "de", "fr").
	 * Only cultures present in the Localization directory are returned.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Localization",
	          meta = (WorldContext = "WorldContextObject"))
	static TArray<FString> GetAvailableCultureNames(const UObject* WorldContextObject);

	/**
	 * Return the display name of a culture suitable for showing in a UI list.
	 * Example: GetCultureDisplayName("pl") → "Polski"
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Localization")
	static FText GetCultureDisplayName(const FString& CultureName);

	/**
	 * Return the currently active culture name.
	 * Example: "pl" or "en"
	 */
	UFUNCTION(BlueprintPure, Category = "CCR|Localization")
	static FString GetCurrentCultureName();

	/**
	 * Switch the game language to the given culture and persist it via
	 * UCCRSettingsSubsystem.
	 * @param CultureName   IETF BCP 47 culture identifier, e.g. "pl".
	 * @param WorldContext  Used to locate UCCRSettingsSubsystem.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Localization",
	          meta = (WorldContext = "WorldContextObject"))
	static void SetCurrentCulture(const FString& CultureName, const UObject* WorldContextObject);

	/**
	 * Restore the last saved culture preference from UCCRSettingsSubsystem.
	 * Call this from UCCRGameInstance::OnCCRInit() so the correct language is
	 * active from the first frame.
	 * @param WorldContext  Used to locate UCCRSettingsSubsystem.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Localization",
	          meta = (WorldContext = "WorldContextObject"))
	static void RestoreSavedCulture(const UObject* WorldContextObject);
};
