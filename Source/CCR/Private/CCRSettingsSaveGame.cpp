#include "CCRSettingsSaveGame.h"

UCCRSettingsSaveGame::UCCRSettingsSaveGame()
{
	// Must match CCRSettingsPrivate::SlotName in CCRSettingsSubsystem.cpp
	SaveSlotName = TEXT("CCRSettings");
	UserIndex    = 0;
}
