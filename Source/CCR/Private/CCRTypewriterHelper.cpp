#include "CCRTypewriterHelper.h"
#include "Engine/World.h"

// ---------------------------------------------------------------------------
// UObject overrides
// ---------------------------------------------------------------------------

void UCCRTypewriterHelper::BeginDestroy()
{
	ClearTimer();
	Super::BeginDestroy();
}

// ---------------------------------------------------------------------------
// GetWorld — walk the outer chain to find a world context
// ---------------------------------------------------------------------------

UWorld* UCCRTypewriterHelper::GetWorld() const
{
	// Walk the outer chain looking for something with a world
	const UObject* Outer = GetOuter();
	while (Outer)
	{
		if (UWorld* World = Outer->GetWorld())
		{
			return World;
		}
		Outer = Outer->GetOuter();
	}
	return nullptr;
}

// ---------------------------------------------------------------------------
// Start
// ---------------------------------------------------------------------------

void UCCRTypewriterHelper::Start(const FText& FullText, float CharsPerSecond)
{
	ClearTimer();

	TargetText   = FullText;
	TargetString = FullText.ToString();
	TotalCount   = TargetString.Len();
	RevealedCount = 0;
	bActive      = true;

	if (TotalCount == 0 || CharsPerSecond <= 0.f)
	{
		// Nothing to reveal; finish immediately
		Skip();
		return;
	}

	const float Interval = FMath::Max(1.f / CharsPerSecond, KINDA_SMALL_NUMBER);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TimerHandle,
			this,
			&UCCRTypewriterHelper::RevealNextChar,
			Interval,
			/*bLoop=*/true);
	}
	else
	{
		// No world context — reveal instantly
		Skip();
	}
}

// ---------------------------------------------------------------------------
// Skip
// ---------------------------------------------------------------------------

void UCCRTypewriterHelper::Skip()
{
	if (!bActive && RevealedCount == TotalCount) return;

	ClearTimer();
	RevealedCount = TotalCount;
	bActive = false;

	if (TotalCount > 0)
	{
		OnCharRevealed.Broadcast(TargetText, RevealedCount, TotalCount);
	}
	OnComplete.Broadcast();
}

// ---------------------------------------------------------------------------
// Stop
// ---------------------------------------------------------------------------

void UCCRTypewriterHelper::Stop()
{
	ClearTimer();
	bActive = false;
}

// ---------------------------------------------------------------------------
// RevealNextChar  (timer callback)
// ---------------------------------------------------------------------------

void UCCRTypewriterHelper::RevealNextChar()
{
	if (RevealedCount >= TotalCount)
	{
		ClearTimer();
		bActive = false;
		OnComplete.Broadcast();
		return;
	}

	++RevealedCount;

	// Build partial FText from the substring up to RevealedCount characters.
	// FText::FromString is safe here because the source came from FText::ToString.
	const FString Partial = TargetString.Left(RevealedCount);
	OnCharRevealed.Broadcast(FText::FromString(Partial), RevealedCount, TotalCount);

	if (RevealedCount >= TotalCount)
	{
		ClearTimer();
		bActive = false;
		OnComplete.Broadcast();
	}
}

// ---------------------------------------------------------------------------
// ClearTimer
// ---------------------------------------------------------------------------

void UCCRTypewriterHelper::ClearTimer()
{
	if (TimerHandle.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(TimerHandle);
		}
		TimerHandle.Invalidate();
	}
}
