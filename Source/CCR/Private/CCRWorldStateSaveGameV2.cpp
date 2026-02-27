#include "CCRWorldStateSaveGameV2.h"
#include "CCR.h"

UCCRWorldStateSaveGameV2::UCCRWorldStateSaveGameV2()
{
	SaveSlotName = CCRSaveSlots::WorldState;
	UserIndex    = 0;
}
