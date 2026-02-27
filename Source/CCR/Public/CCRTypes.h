#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CCRTypes.generated.h"

// ---------------------------------------------------------------------------
// Enumerations
// ---------------------------------------------------------------------------

UENUM(BlueprintType)
enum class ECCRNodeType : uint8
{
	Dialogue       UMETA(DisplayName = "Dialogue"),
	Choice         UMETA(DisplayName = "Choice"),
	Condition      UMETA(DisplayName = "Condition"),
	SetValues      UMETA(DisplayName = "SetValues"),
	QTE            UMETA(DisplayName = "QTE"),
	Cinematic      UMETA(DisplayName = "Cinematic"),
	Jump           UMETA(DisplayName = "Jump"),
	End            UMETA(DisplayName = "End"),
};

UENUM(BlueprintType)
enum class ECCRCompareOp : uint8
{
	Equals         UMETA(DisplayName = "=="),
	NotEquals      UMETA(DisplayName = "!="),
	Less           UMETA(DisplayName = "<"),
	LessOrEqual    UMETA(DisplayName = "<="),
	Greater        UMETA(DisplayName = ">"),
	GreaterOrEqual UMETA(DisplayName = ">="),
};

UENUM(BlueprintType)
enum class ECCRGestureType : uint8
{
	LongPress      UMETA(DisplayName = "LongPress"),
	Tap            UMETA(DisplayName = "Tap"),
	Swipe          UMETA(DisplayName = "Swipe"),
};

UENUM(BlueprintType)
enum class ECCRSwipeDirection : uint8
{
	Any   UMETA(DisplayName = "Any"),
	Up    UMETA(DisplayName = "Up"),
	Down  UMETA(DisplayName = "Down"),
	Left  UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
};

UENUM(BlueprintType)
enum class ECCRStateValueType : uint8
{
	Flag  UMETA(DisplayName = "Flag"),
	Float UMETA(DisplayName = "Float"),
	Int   UMETA(DisplayName = "Int"),
};

// ---------------------------------------------------------------------------
// Condition
// ---------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct FCCRCondition
{
	GENERATED_BODY()

	/** World-state variable name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECCRStateValueType ValueType = ECCRStateValueType::Flag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECCRCompareOp CompareOp = ECCRCompareOp::Equals;

	/** Compared value (cast to matching type) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CompareValue = 0.f;
};

// ---------------------------------------------------------------------------
// SetOp – single "write" instruction
// ---------------------------------------------------------------------------

/**
 * How the value is applied to the existing world-state variable.
 * Set:      variable = Value
 * Add:      variable += Value   (Flag: treated as Set)
 * Multiply: variable *= Value   (Flag: treated as Set)
 */
UENUM(BlueprintType)
enum class ECCRSetOpMode : uint8
{
	Set      UMETA(DisplayName = "Set"),
	Add      UMETA(DisplayName = "Add"),
	Multiply UMETA(DisplayName = "Multiply"),
};

USTRUCT(BlueprintType)
struct FCCRSetOp
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECCRStateValueType ValueType = ECCRStateValueType::Flag;

	/** How to apply the value: Set (default), Add, or Multiply */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECCRSetOpMode Mode = ECCRSetOpMode::Set;

	/** Operand value (cast to matching type) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0.f;
};

// ---------------------------------------------------------------------------
// Choice option
// ---------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct FCCRChoiceOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Label;

	/** Node to jump to when this option is picked */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetNodeId;

	/** Optional conditions that must all be true to show this option */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCCRCondition> ShowConditions;
};

// ---------------------------------------------------------------------------
// Narrative Node
// ---------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct FCCRNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECCRNodeType NodeType = ECCRNodeType::Dialogue;

	// ---- Dialogue / narration ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DialogueText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SpeakerTag;

	/** Next node after dialogue line plays */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NextAfterDialogue;

	/**
	 * Portrait expression state to display for this dialogue node.
	 * Must match a FCCRPortraitEntry::StateName registered for the speaker
	 * in UCCRCharacterDataAsset.  Leave None to use the Neutral portrait.
	 * Common values: "Neutral", "Happy", "Sad", "Angry", "Surprised", "Fear".
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ExpressionTag;

	// ---- Choice ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCCRChoiceOption> Choices;

	// ---- Condition ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCCRCondition> Conditions;

	/** Node taken when all conditions are true */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ConditionTrueNodeId;

	/** Node taken when any condition fails */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ConditionFalseNodeId;

	// ---- SetValues ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCCRSetOp> SetOps;

	/** Node to continue to after applying set ops */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NextAfterSet;

	// ---- QTE ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECCRGestureType GestureType = ECCRGestureType::LongPress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeWindowSec = 2.f;

	/** Required swipe direction (only relevant when GestureType == Swipe) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECCRSwipeDirection RequiredSwipeDir = ECCRSwipeDirection::Any;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName QTESuccessNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName QTEFailNodeId;

	// ---- Cinematic ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<class ULevelSequence> CinematicSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NextAfterCinematic;

	/** Allow the player to skip this cinematic with a tap/touch. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSkippable = true;

	// ---- Jump ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetChunkId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EntryNodeInTarget;

	// ---- Checkpoint flag ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCheckpoint = false;
};

// ---------------------------------------------------------------------------
// Speaker data
// ---------------------------------------------------------------------------

/**
 * Data describing a named speaker referenced by FCCRNode::SpeakerTag.
 * Registered at startup via UCCRSpeakerRegistrySubsystem::RegisterSpeaker().
 */
USTRUCT(BlueprintType)
struct FCCRSpeakerData
{
	GENERATED_BODY()

	/** Localised display name shown in the dialogue UI (e.g. above the speech bubble). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	/**
	 * Optional portrait texture displayed alongside the dialogue text.
	 * Soft-referenced so only the active speaker's portrait is in memory.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Portrait;

	/**
	 * Optional VO key prefix used by UCCRAudioSubsystem.
	 * When set, UCCRAudioSubsystem auto-plays a cue registered under
	 * "<VOKeyPrefix>_<NodeId>" for each dialogue node of this speaker.
	 * Leave None to derive VO keys solely from NodeId.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName VOKeyPrefix;
};

// ---------------------------------------------------------------------------
// Player spatial save data
// ---------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct FCCRPlayerSpatialSave
{
	GENERATED_BODY()

	UPROPERTY()
	FString LevelName;

	UPROPERTY()
	FTransform PlayerTransform;

	UPROPERTY()
	float CameraYaw = 0.f;

	UPROPERTY()
	float CameraPitch = 0.f;

	UPROPERTY()
	FName SpawnTag;

	UPROPERTY()
	bool bHasSpatial = false;
};
