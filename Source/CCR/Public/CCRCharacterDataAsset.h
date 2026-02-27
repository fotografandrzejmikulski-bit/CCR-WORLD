#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CCRTypes.h"
#include "CCRCharacterDataAsset.generated.h"

/**
 * FCCRPortraitEntry
 *
 * One portrait texture associated with a named emotional state.
 * Common state names: "Neutral", "Happy", "Sad", "Angry", "Surprised", "Fear".
 * UCCRDialogueWidget can request a specific state via ResolveSpeakerPortrait().
 */
USTRUCT(BlueprintType)
struct FCCRPortraitEntry
{
	GENERATED_BODY()

	/** Emotional / expression state name (e.g. "Neutral", "Happy", "Sad"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	FName StateName = NAME_None;

	/**
	 * Portrait texture for this state.
	 * Soft-referenced so only the active portrait is kept in memory.
	 * Place portrait textures in Content/CCR/Textures/Portraits/<CharacterId>/.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	TSoftObjectPtr<UTexture2D> Portrait;
};

/**
 * UCCRCharacterDataAsset
 *
 * Primary Data Asset that centralises all data for a single character in CCR WORLD:
 *   - SpeakerTag: links to UCCRSpeakerRegistrySubsystem
 *   - Portraits:  per-expression textures (soft-refs, lazy-loaded)
 *   - VO prefix:  key prefix for UCCRAudioSubsystem cue lookup
 *   - Lore:       optional world-state flag set when the player "meets" this character
 *   - Items:      item IDs whose lore/descriptions reference this character
 *
 * Register this type in DefaultGame.ini:
 *   +PrimaryAssetTypesToScan=(PrimaryAssetType="CCRCharacterData",
 *     AssetBaseClass=/Script/CCR.CCRCharacterDataAsset, ...)
 *
 * Naming convention: DA_Char_<CharacterId>  (e.g. DA_Char_ALEKSY)
 *
 * At game startup UCCRGameInstance's Blueprint subclass should iterate
 * all loaded CCRCharacterDataAssets and call:
 *   UCCRSpeakerRegistrySubsystem::RegisterSpeaker(CharacterId, {DisplayName, Portrait, VOKeyPrefix})
 * using the Neutral portrait as the default.
 */
UCLASS(BlueprintType)
class CCR_API UCCRCharacterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ---- Identity ----

	/**
	 * Unique character identifier – used as the PrimaryAssetLabel and as the
	 * SpeakerTag in FCCRNode.  Must be unique across all characters.
	 * Example: "ALEKSY", "MARTA", "NARRATOR"
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	FName CharacterId;

	/**
	 * Localised full display name shown above dialogue lines in the HUD.
	 * Example: NSLOCTEXT("CCR", "Char_Aleksy", "Aleksy")
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	FText DisplayName;

	/**
	 * Short name used in UI space-constrained contexts (e.g. chapter-select thumbnails).
	 * Leave empty to fall back to DisplayName.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	FText ShortName;

	// ---- Portraits ----

	/**
	 * Per-expression portrait list.
	 * Add one FCCRPortraitEntry per emotion state.
	 * UCCRDialogueWidget calls ResolveSpeakerPortrait() to pick the right portrait
	 * based on the current dialogue node's ExpressionTag.
	 *
	 * Place portrait textures in:
	 *   Content/CCR/Textures/Portraits/<CharacterId>/T_<CharacterId>_<StateName>.png
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	TArray<FCCRPortraitEntry> Portraits;

	// ---- Audio ----

	/**
	 * VO key prefix used by UCCRAudioSubsystem.
	 * VO cues must be registered under "<VOKeyPrefix>_<NodeId>".
	 * Example: if VOKeyPrefix = "ALEKSY" and NodeId = "A_010",
	 * the audio cue should be registered as "ALEKSY_A_010".
	 *
	 * Place VO files in: Content/CCR/Audio/VO/<CharacterId>/
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	FName VOKeyPrefix;

	// ---- Narrative flags ----

	/**
	 * World-state flag set to true the first time the player encounters this character.
	 * Leave None to disable automatic met-tracking.
	 * Convention: "MET_<CharacterId>"   (e.g. "MET_ALEKSY")
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	FName MetFlag;

	/**
	 * When true, this character appears in the in-game character gallery / codex.
	 * Non-important NPCs (guards, crowds) should set this to false.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	bool bIsKeyCharacter = false;

	// ---- Associated items ----

	/**
	 * Item IDs whose discovery/acquisition is associated with this character.
	 * Used by UCCRInventoryWidget to show a character thumbnail alongside
	 * the item description.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	TArray<FName> AssociatedItemIds;

	// ---- UPrimaryDataAsset ----

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	static const FPrimaryAssetType CharacterAssetType;

	// ---- Helpers ----

	/**
	 * Returns the portrait for the given state name, or the Neutral portrait
	 * if the requested state is not found, or a null soft-ref if Portraits is empty.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Character")
	TSoftObjectPtr<UTexture2D> GetPortraitForState(FName StateName) const;

	/**
	 * Converts this asset to FCCRSpeakerData using the Neutral portrait.
	 * Convenience method for UCCRSpeakerRegistrySubsystem::RegisterSpeaker().
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Character")
	FCCRSpeakerData ToSpeakerData() const;
};
