#pragma once

#include "CoreMinimal.h"

// Save slot names shared across subsystems and widgets
namespace CCRSaveSlots
{
	inline const FString WorldState = TEXT("CCRSaveSlot");
	inline const FString Settings   = TEXT("CCRSettings");
}

// Widget z-order constants shared across HUD and widget classes
namespace CCRZOrder
{
	constexpr int32 Dialogue          = 0;
	constexpr int32 QTE               = 1;
	constexpr int32 Cinematic         = 2;
	constexpr int32 Notification      = 3;
	constexpr int32 MainMenu          = 5;
	constexpr int32 Pause             = 10;
	constexpr int32 Settings          = 11;
	constexpr int32 Credits           = 15;
	constexpr int32 ChapterTransition = 18;
	constexpr int32 Loading           = 20;
}
