#include "CCRCharacterDataAsset.h"

const FPrimaryAssetType UCCRCharacterDataAsset::CharacterAssetType = TEXT("CCRCharacterData");

FPrimaryAssetId UCCRCharacterDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(CharacterAssetType, CharacterId);
}

TSoftObjectPtr<UTexture2D> UCCRCharacterDataAsset::GetPortraitForState(FName StateName) const
{
	// First try the exact state match
	for (const FCCRPortraitEntry& Entry : Portraits)
	{
		if (Entry.StateName == StateName)
		{
			return Entry.Portrait;
		}
	}

	// Fall back to "Neutral"
	static const FName NeutralState(TEXT("Neutral"));
	if (StateName != NeutralState)
	{
		for (const FCCRPortraitEntry& Entry : Portraits)
		{
			if (Entry.StateName == NeutralState)
			{
				return Entry.Portrait;
			}
		}
	}

	// Last resort: return the first portrait in the list
	if (Portraits.Num() > 0)
	{
		return Portraits[0].Portrait;
	}

	return TSoftObjectPtr<UTexture2D>();
}

FCCRSpeakerData UCCRCharacterDataAsset::ToSpeakerData() const
{
	FCCRSpeakerData Data;
	Data.DisplayName  = DisplayName;
	Data.VOKeyPrefix  = VOKeyPrefix;
	Data.Portrait     = GetPortraitForState(FName(TEXT("Neutral")));
	return Data;
}
