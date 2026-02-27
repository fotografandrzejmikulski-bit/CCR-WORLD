#include "CCRHapticLibrarySubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UCCRHapticLibrarySubsystem::RegisterPattern(FName PatternId, const FCCRHapticPattern& Pattern)
{
	if (PatternId.IsNone()) return;
	Patterns.Emplace(PatternId, Pattern);
}

void UCCRHapticLibrarySubsystem::UnregisterPattern(FName PatternId)
{
	Patterns.Remove(PatternId);
}

void UCCRHapticLibrarySubsystem::PlayPattern(FName PatternId)
{
	if (PatternId.IsNone()) return;

	UWorld* World = GetGameInstance()->GetWorld();
	if (!World) return;

	APlayerController* PC = World->GetFirstPlayerController();
	PlayPatternOnController(PatternId, PC);
}

void UCCRHapticLibrarySubsystem::PlayPatternOnController(FName PatternId, APlayerController* PlayerController)
{
	if (PatternId.IsNone() || !PlayerController) return;

	const FCCRHapticPattern* Pattern = Patterns.Find(PatternId);
	if (!Pattern) return;

	PlayPatternInternal(*Pattern, PlayerController);
}

bool UCCRHapticLibrarySubsystem::HasPattern(FName PatternId) const
{
	if (PatternId.IsNone()) return false;
	return Patterns.Contains(PatternId);
}

TArray<FName> UCCRHapticLibrarySubsystem::GetAllPatternIds() const
{
	TArray<FName> Keys;
	Patterns.GetKeys(Keys);
	return Keys;
}

void UCCRHapticLibrarySubsystem::PlayPatternInternal(const FCCRHapticPattern& Pattern, APlayerController* PC)
{
	if (!PC) return;

	// If a UE force-feedback asset is provided, use it
	if (Pattern.ForceFeedbackEffect.IsValid())
	{
		UForceFeedbackEffect* Effect = Pattern.ForceFeedbackEffect.LoadSynchronous();
		if (Effect)
		{
			PC->PlayHapticEffect(Effect, Pattern.Hand, Pattern.Intensity);
			return;
		}
	}

	// Otherwise use the dynamic force-feedback API (available on mobile)
	PC->PlayDynamicForceFeedback(
		Pattern.Intensity,
		Pattern.Duration,
		/*bLooping=*/false,
		/*bAffectsLeftLarge=*/true,
		/*bAffectsLeftSmall=*/true,
		/*bAffectsRightLarge=*/true,
		/*bAffectsRightSmall=*/true);
}
