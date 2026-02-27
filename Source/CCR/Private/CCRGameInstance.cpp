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

// ---------------------------------------------------------------------------
// BlueprintNativeEvent default implementations
// ---------------------------------------------------------------------------

void UCCRGameInstance::OnCCRInit_Implementation()
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRGameInstance::OnCCRInit has no C++ or Blueprint implementation."))
#endif
}

void UCCRGameInstance::OnCCRShutdown_Implementation()
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRGameInstance::OnCCRShutdown has no C++ or Blueprint implementation."))
#endif
}
