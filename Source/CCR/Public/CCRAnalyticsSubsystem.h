#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRAnalyticsSubsystem.generated.h"

/**
 * Standard event categories for CCR analytics.
 * Add project-specific categories in a Blueprint subclass.
 */
UENUM(BlueprintType)
enum class ECCRAnalyticsCategory : uint8
{
	Narrative  UMETA(DisplayName = "Narrative"),  // story node / choice
	QTE        UMETA(DisplayName = "QTE"),        // gesture result
	Session    UMETA(DisplayName = "Session"),    // start/resume/quit
	Settings   UMETA(DisplayName = "Settings"),   // player changed a setting
	Error      UMETA(DisplayName = "Error"),      // non-fatal error
};

/**
 * UCCRAnalyticsSubsystem
 *
 * Lightweight event telemetry hub for CCR WORLD.
 *
 * All recorded events are accumulated in memory and can be flushed to any
 * backend by binding to OnFlushRequested (e.g. a Blueprint HTTP request node
 * or a third-party analytics SDK).  The subsystem never transmits data itself,
 * keeping the C++ foundation backend-agnostic.
 *
 * Automatic events:
 *   - Session Start/End (Initialize/Deinitialize)
 *   - Narrative node changes (subscribes to UCCRNarrativeRuntimeSubsystem::OnNodeChanged)
 *   - QTE results (subscribes to OnNodeChanged; resolved by detecting QTE nodes)
 *
 * Manual events (call from Blueprint):
 *   RecordEvent(Category, EventName, Params)
 */
UCLASS(BlueprintType)
class CCR_API UCCRAnalyticsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ---- Manual event recording ----

	/**
	 * Record a named analytics event.
	 * @param Category   Broad category for filtering.
	 * @param EventName  Specific event identifier (e.g. "ChoiceMade", "ChunkEntered").
	 * @param Params     Optional key=value string payload (e.g. "nodeId=N001,choice=0").
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Analytics")
	void RecordEvent(ECCRAnalyticsCategory Category, const FString& EventName,
	                 const FString& Params = TEXT(""));

	/**
	 * Flush all buffered events.
	 * Fires OnFlushRequested with the accumulated event list, then clears the buffer.
	 * Call periodically (e.g. every 60 s or on session end) or let Deinitialize handle it.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Analytics")
	void Flush();

	/** Returns the number of buffered events waiting to be flushed. */
	UFUNCTION(BlueprintPure, Category = "CCR|Analytics")
	int32 GetBufferedEventCount() const { return Events.Num(); }

	// ---- Delegate ----

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCCRAnalyticsFlush, const TArray<FString>&, EventLines);

	/**
	 * Fired by Flush().  Each FString in EventLines is a tab-separated line:
	 *   "[ISO8601_Timestamp]  [Category]  [EventName]  [Params]"
	 * Bind to this delegate in Blueprint to forward data to your analytics backend.
	 */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Analytics")
	FOnCCRAnalyticsFlush OnFlushRequested;

	/** Maximum number of events to buffer before an automatic flush. Default: 200. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Analytics",
	          meta = (ClampMin = "10"))
	int32 MaxBufferSize = 200;

private:
	TArray<FString> Events;

	UFUNCTION()
	void OnNodeChanged(FName NodeId);

	FString CategoryToString(ECCRAnalyticsCategory Category) const;
	FString CurrentTimestamp() const;
};
