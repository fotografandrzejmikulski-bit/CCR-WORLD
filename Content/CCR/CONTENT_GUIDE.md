# CCR WORLD - Content Guide

This document describes the complete `Content/CCR/` folder structure and explains
what UE5 assets belong in each folder, with naming conventions and creation steps.

---

## Folder Tree

```
Content/CCR/
+-- Maps/                          <- Level maps (.umap)
+-- Story/                         <- UCCRStoryChunk data assets
|   +-- Prologue/
|   +-- Chapter1/
|   +-- Chapter2/
|   +-- Chapter3/
+-- Characters/                    <- UCCRCharacterDefinition data assets
+-- Items/                         <- UCCRInventoryItemDefinition data assets
+-- Blueprints/
|   +-- Widgets/                   <- UMG Widget Blueprints (WBP_*)
|   +-- GameFramework/             <- BP_CCRGameInstance, BP_CCRGameMode, etc.
+-- Audio/
|   +-- Music/                     <- USoundWave / USoundCue for background music
|   +-- VO/
|   |   +-- ANDRZEJ/               <- Voice-over lines for Andrzej
|   |   +-- ZUZIA/                 <- Voice-over lines for Zuzia
|   |   +-- EVA/                   <- Voice-over lines for Ewy
|   |   +-- CIEN/                  <- Voice-over lines for Cieńa
|   |   +-- EIMSTAIN/              <- Voice-over lines for Eimstaina
|   |   +-- BATISTA/               <- Voice-over lines for Batisty
|   |   +-- WERONIKA/              <- Voice-over lines for Weroniki
|   |   +-- DAVID/                 <- Voice-over lines for Davida
|   |   +-- KAFKA/                 <- Voice-over lines for Kafki
|   +-- SFX/                       <- Sound effects
|   +-- Ambient/                   <- Ambient sound loops
+-- Textures/
|   +-- Portraits/
|   |   +-- ANDRZEJ/               <- T_ANDRZEJ_Neutral.png, T_ANDRZEJ_Sad.png, ...
|   |   +-- ZUZIA/
|   |   +-- EVA/
|   |   +-- CIEN/
|   |   +-- EIMSTAIN/
|   |   +-- BATISTA/
|   |   +-- WERONIKA/
|   |   +-- DAVID/
|   |   +-- KAFKA/
|   +-- UI/                        <- UI icons, backgrounds, buttons
|   +-- Items/                     <- Item icon textures (T_Item_<ItemId>.png)
+-- Cinematics/
    +-- Prologue/                  <- Level Sequences for prologue cutscenes
    +-- Chapter3/                  <- Level Sequences for finale cutscenes
```

---

## Maps

| Asset Name | Path | Description |
|---|---|---|
| `MainMenu` | `Maps/MainMenu.umap` | Main menu level |
| `Prologue` | `Maps/Prologue.umap` | Prologue chapter environment |
| `Chapter1` | `Maps/Chapter1.umap` | Chapter 1 playable level |
| `Chapter2` | `Maps/Chapter2.umap` | Chapter 2 playable level |
| `Chapter3` | `Maps/Chapter3.umap` | Chapter 3 finale level |
| `Loading`  | `Maps/Loading.umap`  | Empty transition level shown during async loads |

**Setup for each level map:**
1. Add `BP_CCRGameMode` to WorldSettings -> GameMode Override.
2. Add `ACCRSpawnPoint` actors with unique `SpawnTag` names.
3. Add `ACCREventTriggerActor` actors at narrative trigger zones.
4. Add `ACCRAmbientSoundZoneActor` actors with ambient sounds.
5. Set `GameDefaultMap` in `DefaultEngine.ini` to `MainMenu`.

---

## Story Assets (UCCRStoryChunk)

Create one `UCCRStoryChunk` data asset per narrative chunk.

**To create in UE5 Editor:**
1. Right-click in the target `Content/CCR/Story/<Chapter>/` folder.
2. **Miscellaneous -> Data Asset -> CCRStoryChunk**.
3. Name it `DA_Story_<ChunkId>` (e.g. `DA_Story_PROLOGUE_01`).
4. Fill in `ChunkId`, `AxisId`, `EntryNodeId`, `ChapterTitle`, `ChapterSubtitle`.
5. Add `FCCRNode` entries to the `Nodes` array.

See `Story/Prologue/PROLOGUE_STORY.json` for the Prologue manifest.
See `Story/Chapter1/CHAPTER1_STORY.json` for Chapter 1 manifest.
See `Story/Chapter2/CHAPTER2_STORY.json` for Chapter 2 manifest.
See `Story/Chapter3/CHAPTER3_STORY.json` for Chapter 3 (Finale) manifest.

### Complete story flow

```
PROLOGUE_01 -> PROLOGUE_02 -> CH1_01 -> CH1_02 -> CH2_01 -> CH2_02 -> CH3_01 -> CH3_02 -> End
```

| Chunk | AxisId | Characters appearing |
|---|---|---|
| `PROLOGUE_01` | `PROLOGUE` | ANDRZEJ, ZUZIA |
| `PROLOGUE_02` | `PROLOGUE` | ANDRZEJ, ZUZIA |
| `CH1_01` | `CHAPTER1` | ANDRZEJ, EVA |
| `CH1_02` | `CHAPTER1` | ANDRZEJ, CIEN |
| `CH2_01` | `CHAPTER2` | ANDRZEJ, EIMSTAIN |
| `CH2_02` | `CHAPTER2` | ANDRZEJ, BATISTA, WERONIKA |
| `CH3_01` | `CHAPTER3` | ANDRZEJ, DAVID, KAFKA |
| `CH3_02` | `CHAPTER3` | ANDRZEJ (finale cinematic + End) |

### Node ID naming convention

```
<Axis>_<Chunk>_<Sequence>
e.g.:  PRO_01_010   (Prologue, chunk 01, node 010)
       CH1_02_030   (Chapter 1, chunk 02, node 030)
```

---

## Character Assets (UCCRCharacterDefinition)

Create one `UCCRCharacterDefinition` per named character.

**To create in UE5 Editor:**
1. Right-click in `Content/CCR/Characters/`.
2. **Miscellaneous -> Data Asset -> CCRCharacterDefinition**.
3. Name it `DA_Char_<CharacterId>` (e.g. `DA_Char_ANDRZEJ`).
4. Set `CharacterId` to match the filename suffix (e.g. `ANDRZEJ`).
5. Fill `DisplayName`, `ShortName`, `VOKeyPrefix`, `SpeakerTag`, `MetFlag`.
6. Toggle `bIsKeyCharacter` for story-critical characters.
7. Add portrait entries to `Portraits[]` -- one `FCCRPortraitEntry` per expression state.

See `Characters/CHARACTERS_MANIFEST.json` for all character definitions.

**Characters:**

| CharacterId | Display Name | MetFlag |
|---|---|---|
| `ANDRZEJ`  | Andrzej  | -- (protagonist) |
| `ZUZIA`    | Zuzia    | `MET_ZUZIA` |
| `EVA`      | Eva      | `MET_EVA` |
| `CIEN`     | Cien     | `MET_CIEN` |
| `EIMSTAIN` | Eimstain | `MET_EIMSTAIN` |
| `BATISTA`  | Batista  | `MET_BATISTA` |
| `WERONIKA` | Weronika | `MET_WERONIKA` |
| `DAVID`    | David    | `MET_DAVID` |
| `KAFKA`    | Kafka    | `MET_KAFKA` |

### Portrait naming convention

```
T_<CharacterId>_<StateName>
e.g.: T_ANDRZEJ_Neutral.png
      T_ANDRZEJ_Happy.png
      T_ZUZIA_Sad.png
      T_CIEN_Fear.png
```

Import portrait textures to `Content/CCR/Textures/Portraits/<CharacterId>/`.

`UCCRCharacterDefinition::GetPortraitForExpression(ExpressionTag)` resolves
the correct portrait at runtime, falling back to "Neutral" when the tag is
not found.

---

## Item Assets (UCCRInventoryItemDefinition)

Create one `UCCRInventoryItemDefinition` per collectible item.

**To create in UE5 Editor:**
1. Right-click in `Content/CCR/Items/`.
2. **Miscellaneous -> Data Asset -> CCRInventoryItemDefinition**.
3. Name it `DA_Item_<ItemId>` (e.g. `DA_Item_KEY_01`).
4. Set `ItemId` to match the FName used in `UCCRInventorySubsystem::AddItem()`.
5. Fill `DisplayName`, `Description`, assign `Icon`.
6. Set `bCanUse`, `bConsumedOnUse`, `UseWorldStateFlag` if applicable.

See `Items/ITEMS_MANIFEST.json` for all item definitions.

### Item icon naming convention

```
T_Item_<ItemId>
e.g.: T_Item_KEY_01.png
```

Import icons to `Content/CCR/Textures/Items/`.

---

## Blueprint Widgets

Create one Blueprint Widget subclass per C++ base class.

| Blueprint Name | C++ Base | Folder |
|---|---|---|
| `WBP_CCRDialogue` | `UCCRDialogueWidget` | `Blueprints/Widgets/` |
| `WBP_CCRQTE` | `UCCRQTEWidget` | `Blueprints/Widgets/` |
| `WBP_CCRPause` | `UCCRPauseWidget` | `Blueprints/Widgets/` |
| `WBP_CCRLoading` | `UCCRLoadingWidget` | `Blueprints/Widgets/` |
| `WBP_CCRMainMenu` | `UCCRMainMenuWidget` | `Blueprints/Widgets/` |
| `WBP_CCRSettings` | `UCCRSettingsWidget` | `Blueprints/Widgets/` |
| `WBP_CCRNotification` | `UCCRNotificationWidget` | `Blueprints/Widgets/` |
| `WBP_CCRChapterTransition` | `UCCRChapterTransitionWidget` | `Blueprints/Widgets/` |
| `WBP_CCRCredits` | `UCCRCreditsWidget` | `Blueprints/Widgets/` |
| `WBP_CCRCutsceneSkip` | `UCCRCutsceneSkipWidget` | `Blueprints/Widgets/` |
| `WBP_CCRObjective` | `UCCRObjectiveWidget` | `Blueprints/Widgets/` |
| `WBP_CCRInventory` | `UCCRInventoryWidget` | `Blueprints/Widgets/` |
| `WBP_CCRChapterSelect` | `UCCRChapterSelectWidget` | `Blueprints/Widgets/` |

**Assign all widget class references in `ACCRGameHUD` (Blueprint Defaults).**

---

## Game Framework Blueprints

| Blueprint Name | C++ Base | Notes |
|---|---|---|
| `BP_CCRGameInstance` | `UCCRGameInstance` | Register speakers in `Event Init`; assign widget classes to `ACCRGameHUD` |
| `BP_CCRGameMode` | `ACCRGameMode` | Set `PlayerControllerClass = BP_CCRTouchController` |
| `BP_CCRGameHUD` | `ACCRGameHUD` | Assign all `WBP_*` widget class properties |
| `BP_CCRTouchController` | `ACCRTouchController` | Mobile touch controller |

---

## Audio

### Music cues

Place music `USoundWave` assets in `Content/CCR/Audio/Music/`.
Register cues at startup via `UCCRAudioSubsystem::RegisterMusicCue(Key, Cue)`.

| Key | File | Description |
|---|---|---|
| `MENU` | `MUS_MainMenu.wav` | Main menu ambient theme |
| `PROLOGUE` | `MUS_Prologue.wav` | Prologue background music |
| `CHAPTER1` | `MUS_Chapter1.wav` | Chapter 1 theme |
| `CHAPTER2` | `MUS_Chapter2.wav` | Chapter 2 theme |
| `CHAPTER3` | `MUS_Chapter3.wav` | Chapter 3 finale theme |
| `TENSION` | `MUS_Tension.wav` | Tension / QTE music |

### Voice-over cues

File naming: `VO_<CharacterId>_<NodeId>.wav`
Place in `Content/CCR/Audio/VO/<CharacterId>/`.

### SFX

Place in `Content/CCR/Audio/SFX/`.

| Asset Name | Description |
|---|---|
| `SFX_QTE_Success` | QTE success feedback |
| `SFX_QTE_Fail` | QTE failure feedback |
| `SFX_CheckpointSaved` | Auto-save confirmation |
| `SFX_ItemPickup` | Item acquired notification |
| `SFX_UIConfirm` | UI confirm button |
| `SFX_UIBack` | UI back button |

---

## Cinematics (Level Sequences)

Create `ULevelSequence` assets in `Content/CCR/Cinematics/<Chapter>/`.
Assign them to `FCCRNode.CinematicSequence` (soft object ptr) on Cinematic nodes.

| Asset Name | Chunk | Node | Description |
|---|---|---|---|
| `LS_Prologue_Intro` | `PROLOGUE_01` | `PRO_01_CIN_001` | Opening cutscene |
| `LS_Prologue_Outro` | `PROLOGUE_02` | `PRO_02_CIN_001` | Prologue ending cutscene |
| `LS_Chapter3_Finale` | `CH3_02` | `CH3_02_010` | Finale cutscene |
| `LS_Chapter3_Epilogue` | `CH3_02` | `CH3_02_050` | Epilogue montage (skippable) |

---

## `.gitignore` for Content

Binary `.uasset` and `.umap` files should **not** be tracked in Git (use Perforce or Git LFS).

The folder structure (tracked via `.gitkeep` files) provides the scaffold for the team.

If you use Git LFS, add to `.gitattributes`:
```
Content/**/*.uasset filter=lfs diff=lfs merge=lfs -text
Content/**/*.umap    filter=lfs diff=lfs merge=lfs -text
Content/**/*.uexp    filter=lfs diff=lfs merge=lfs -text
Content/**/*.ubulk   filter=lfs diff=lfs merge=lfs -text
```
