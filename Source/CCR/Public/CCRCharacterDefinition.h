#pragma once

#include "CoreMinimal.h"
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
	 * Portrait texture displayed alongside dialogue text.
	 * Soft-referenced so only the active character's portrait is loaded.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	TSoftObjectPtr<UTexture2D> Portrait;

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
	 * Item IDs associated with this character (e.g. key items, gifts, weapons).
	 * Each entry should correspond to a CCRInventoryItem PrimaryAssetName so
	 * that UCCRInventorySubsystem can resolve the UCCRInventoryItemDefinition.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Character")
	TArray<FName> AssociatedItemIds;

	// ---- UPrimaryDataAsset ----
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	static const FPrimaryAssetType CharacterAssetType;
};
