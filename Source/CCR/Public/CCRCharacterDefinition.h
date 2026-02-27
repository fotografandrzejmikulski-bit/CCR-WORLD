#pragma once

#include "CoreMinimal.h"
#include "CCRTypes.h"
#include "Engine/DataAsset.h"
#include "CCRCharacterDefinition.generated.h"

/**
 * UCCRCharacterDefinition
 *
 * Primary data asset describing a named character in CCR WORLD.
 *
 * Register character assets in the Asset Manager under PrimaryAssetType
 * "CCRCharacterDefinition".  The Asset Manager scans /Game/CCR/Characters.
 *
 * The SpeakerTag links this asset to UCCRSpeakerRegistrySubsystem so that
 * UCCRDialogueWidget can display the correct portrait and display name for
 * every dialogue node without duplicating data.
 *
 * UCCRAudioSubsystem uses VOKeyPrefix to build per-node VO cue keys in the
 * form "<VOKeyPrefix>_<NodeId>".
 *
 * Usage from Blueprint / C++:
 * @code
 *   FPrimaryAssetId AssetId("CCRCharacterDefinition", CharacterId);
 *   UAssetManager::Get().LoadPrimaryAsset(AssetId, {}, Callback);
 * @endcode
 */
UCLASS(BlueprintType)
class CCR_API UCCRCharacterDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Unique identifier — used as the PrimaryAssetName and in audio / state lookups. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	FName CharacterId;

	/** Localised display name shown in dialogue UI (above the speech bubble). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	FText DisplayName;

	/**
	 * Short / first name used in compact UI labels (e.g. in objectives).
	 * May be left empty to fall back to DisplayName.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	FText ShortName;

	/**
	 * Per-expression portrait textures.
	 * Each entry maps an expression state name (matching FCCRNode::ExpressionTag)
	 * to a soft-referenced texture.  Use "Neutral" as the default/fallback.
	 * Common state names: "Neutral", "Happy", "Sad", "Angry", "Surprised", "Fear".
	 * Import textures to Content/CCR/Textures/Portraits/<CharacterId>/.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	TArray<FCCRPortraitEntry> Portraits;

	/**
	 * VO key prefix used by UCCRAudioSubsystem.
	 * A cue registered as "<VOKeyPrefix>_<NodeId>" is auto-played for each
	 * dialogue node belonging to this character.
	 * Leave None to derive VO keys solely from NodeId.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	FName VOKeyPrefix;

	/**
	 * Speaker tag that links this character to UCCRSpeakerRegistrySubsystem.
	 * Must match the FName used in FCCRNode::SpeakerTag for this character's
	 * dialogue nodes.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	FName SpeakerTag;

	/**
	 * World-state flag set to 1 when the player first encounters this character.
	 * Leave None if no met-tracking is needed (e.g. NARRATOR).
	 * Example: "MET_MARTA", "MET_KOZLOWSKI".
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	FName MetFlag;

	/** True for story-critical characters that drive the main narrative arc. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	bool bIsKeyCharacter = false;

	/**
	 * Item IDs associated with this character (e.g. key items, gifts, weapons).
	 * Each entry should correspond to a CCRInventoryItem PrimaryAssetName so
	 * that UCCRInventorySubsystem can resolve the UCCRInventoryItemDefinition.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	TArray<FName> AssociatedItemIds;

	// ---- Helpers ----

	/**
	 * Returns the soft portrait reference for the given expression state.
	 * Falls back to the "Neutral" portrait when ExpressionTag is not found.
	 * Returns a null TSoftObjectPtr when Portraits is empty.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Character")
	TSoftObjectPtr<UTexture2D> GetPortraitForExpression(FName ExpressionTag) const;

	// ---- UPrimaryDataAsset ----
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	static const FPrimaryAssetType CharacterAssetType;
};
