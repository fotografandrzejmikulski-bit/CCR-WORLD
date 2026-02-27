#include "CCRCharacterDefinition.h"

const FPrimaryAssetType UCCRCharacterDefinition::CharacterAssetType = TEXT("CCRCharacterDefinition");

FPrimaryAssetId UCCRCharacterDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(CharacterAssetType, CharacterId);
}
