#include "CCRAnalyticsSubsystem.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRTypes.h"

// ---------------------------------------------------------------------------
// Initialize / Deinitialize
// ---------------------------------------------------------------------------

void UCCRAnalyticsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	RecordEvent(ECCRAnalyticsCategory::Session, TEXT("SessionStart"));

	if (UCCRNarrativeRuntimeSubsystem* NRS = GetGameInstance()->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
	{
		NRS->OnNodeChanged.AddDynamic(this, &UCCRAnalyticsSubsystem::OnNodeChanged);
	}
}

void UCCRAnalyticsSubsystem::Deinitialize()
{
	RecordEvent(ECCRAnalyticsCategory::Session, TEXT("SessionEnd"));
	Flush();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
		{
			NRS->OnNodeChanged.RemoveDynamic(this, &UCCRAnalyticsSubsystem::OnNodeChanged);
		}
	}

	Super::Deinitialize();
}

// ---------------------------------------------------------------------------
// OnNodeChanged
// ---------------------------------------------------------------------------

void UCCRAnalyticsSubsystem::OnNodeChanged(FName NodeId)
{
	UCCRNarrativeRuntimeSubsystem* NRS = GetGameInstance()->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!NRS) return;

	FCCRNode Node;
	if (!NRS->GetCurrentNode(Node)) return;

	const FString ChunkId = NRS->GetCurrentChunkId().ToString();
	const FString Params  = FString::Printf(TEXT("chunk=%s,node=%s,type=%d"),
		*ChunkId, *NodeId.ToString(), static_cast<int32>(Node.NodeType));

	RecordEvent(ECCRAnalyticsCategory::Narrative, TEXT("NodeEntered"), Params);
}

// ---------------------------------------------------------------------------
// RecordEvent
// ---------------------------------------------------------------------------

void UCCRAnalyticsSubsystem::RecordEvent(
	ECCRAnalyticsCategory Category,
	const FString& EventName,
	const FString& Params)
{
	const FString Line = FString::Printf(TEXT("%s\t%s\t%s\t%s"),
		*CurrentTimestamp(),
		*CategoryToString(Category),
		*EventName,
		*Params);

	Events.Add(Line);

	if (Events.Num() >= MaxBufferSize)
	{
		Flush();
	}
}

// ---------------------------------------------------------------------------
// Flush
// ---------------------------------------------------------------------------

void UCCRAnalyticsSubsystem::Flush()
{
	if (Events.Num() == 0) return;

	OnFlushRequested.Broadcast(Events);
	Events.Reset();
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

FString UCCRAnalyticsSubsystem::CategoryToString(ECCRAnalyticsCategory Category) const
{
	switch (Category)
	{
	case ECCRAnalyticsCategory::Narrative: return TEXT("Narrative");
	case ECCRAnalyticsCategory::QTE:       return TEXT("QTE");
	case ECCRAnalyticsCategory::Session:   return TEXT("Session");
	case ECCRAnalyticsCategory::Settings:  return TEXT("Settings");
	case ECCRAnalyticsCategory::Error:     return TEXT("Error");
	default:                               return TEXT("Unknown");
	}
}

FString UCCRAnalyticsSubsystem::CurrentTimestamp() const
{
	return FDateTime::UtcNow().ToIso8601();
}
