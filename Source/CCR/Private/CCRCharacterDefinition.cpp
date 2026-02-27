#include "CCRCharacterDefinition.h"

const FPrimaryAssetType UCCRCharacterDefinition::CharacterAssetType = TEXT("CCRCharacterDefinition");

FPrimaryAssetId UCCRCharacterDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(CharacterAssetType, CharacterId);
}

TSoftObjectPtr<UTexture2D> UCCRCharacterDefinition::GetPortraitForExpression(FName ExpressionTag) const
{
	if (Portraits.IsEmpty())
	{
		return TSoftObjectPtr<UTexture2D>();
	}

	// First pass: exact match
	for (const FCCRPortraitEntry& Entry : Portraits)
	{
		if (Entry.StateName == ExpressionTag)
		{
			return Entry.Portrait;
		}
	}

	// Fallback: return the "Neutral" portrait
	for (const FCCRPortraitEntry& Entry : Portraits)
	{
		if (Entry.StateName == NAME_None || Entry.StateName == FName("Neutral"))
		{
			return Entry.Portrait;
		}
	}

	// Last resort: first entry
	return Portraits[0].Portrait;
}
