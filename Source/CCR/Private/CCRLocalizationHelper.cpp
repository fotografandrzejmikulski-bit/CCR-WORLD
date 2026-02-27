#include "CCRLocalizationHelper.h"
#include "CCRSettingsSubsystem.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h"
#include "Engine/World.h"

TArray<FString> UCCRLocalizationHelper::GetAvailableCultureNames(const UObject* WorldContextObject)
{
	// FInternationalization returns all cultures that have been registered /
	// discovered in the active build's Localization folder.
	TArray<FCultureRef> Cultures;
	FInternationalization::Get().GetCulturesWithAvailableLocalization(
		FPaths::ProjectContentDir() / TEXT("Localization"),
		Cultures,
		/*bIncludeDerivedCultures=*/false);

	TArray<FString> Names;
	Names.Reserve(Cultures.Num());
	for (const FCultureRef& C : Cultures)
	{
		Names.Add(C->GetName());
	}

	// Always include English as a fallback even if not discovered
	if (Names.Num() == 0)
	{
		Names.Add(TEXT("en"));
	}

	return Names;
}

FText UCCRLocalizationHelper::GetCultureDisplayName(const FString& CultureName)
{
	FCulturePtr Culture = FInternationalization::Get().GetCulture(CultureName);
	if (Culture.IsValid())
	{
		return FText::FromString(Culture->GetNativeName());
	}
	return FText::FromString(CultureName);
}

FString UCCRLocalizationHelper::GetCurrentCultureName()
{
	FCulturePtr Current = FInternationalization::Get().GetCurrentCulture();
	return Current.IsValid() ? Current->GetName() : TEXT("en");
}

void UCCRLocalizationHelper::SetCurrentCulture(const FString& CultureName, const UObject* WorldContextObject)
{
	if (CultureName.IsEmpty()) return;

	// Persist first so the setting survives a crash / force-quit mid-session.
	if (WorldContextObject)
	{
		if (UWorld* World = WorldContextObject->GetWorld())
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				if (UCCRSettingsSubsystem* Settings = GI->GetSubsystem<UCCRSettingsSubsystem>())
				{
					Settings->SetCultureName(CultureName);
				}
			}
		}
	}

	// Apply immediately
	FInternationalization::Get().SetCurrentCulture(CultureName);
}

void UCCRLocalizationHelper::RestoreSavedCulture(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return;
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return;
	UGameInstance* GI = World->GetGameInstance();
	if (!GI) return;

	UCCRSettingsSubsystem* Settings = GI->GetSubsystem<UCCRSettingsSubsystem>();
	if (!Settings) return;

	const FString SavedCulture = Settings->GetCultureName();
	if (!SavedCulture.IsEmpty())
	{
		FInternationalization::Get().SetCurrentCulture(SavedCulture);
	}
}
