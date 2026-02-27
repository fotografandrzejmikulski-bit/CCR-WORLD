#include "CCRGameInstance.h"

void UCCRGameInstance::Init()
{
	Super::Init();
	// All GameInstanceSubsystems (audio, narrative, world state, etc.) are
	// initialised automatically by the engine before Init() returns, so it
	// is safe to call OnCCRInit() here.
	OnCCRInit();
}

void UCCRGameInstance::Shutdown()
{
	OnCCRShutdown();
	Super::Shutdown();
}

UCCRGameInstance* UCCRGameInstance::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;
	return Cast<UCCRGameInstance>(World->GetGameInstance());
}
