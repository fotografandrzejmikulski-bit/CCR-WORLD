#include "CCRGameState.h"

void ACCRGameState::SetGamePhase(ECCRGamePhase NewPhase)
{
	if (CurrentPhase == NewPhase) return;
	CurrentPhase = NewPhase;
	OnGamePhaseChanged.Broadcast(NewPhase);
}
