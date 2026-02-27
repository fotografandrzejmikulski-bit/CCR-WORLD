# CCR WORLD

**CCR WORLD** is an AAA narrative adventure game built with Unreal Engine 5.3, targeting Android (Vulkan).

---

## Overview

CCR WORLD is a story-driven, choice-based game featuring:
- **Chunk-based narrative graph** – stories are broken into `UCCRStoryChunk` data assets with fully typed nodes (Dialogue, Choice, Condition, SetValues, QTE, Cinematic, Jump, End).
- **World State Manager** – a fast bitset-backed system (`UCCRWorldStateSubsystemV2`) that tracks flags, floats and ints and evaluates branching conditions.
- **Persistent save system** – checkpoint auto-saves, a 4-slot manual save manager (`UCCRSaveSlotManagerSubsystem`), and full resume flow including player spatial restore.
- **Mobile-first input** – `ACCRTouchController` handles LongPress, Tap and Swipe QTE gestures with haptic feedback.
- **Full HUD stack** – dialogue, QTE progress, pause menu, loading overlay, chapter transitions, credits, cutscene-skip prompt, notifications, inventory and objectives.
- **Accessibility** – subtitles, font-scale, reduced-motion and high-contrast settings persisted via `UCCRSettingsSubsystem`.
- **Photo mode** – in-game screenshot capture with HUD hide/show.
- **Analytics** – session-scoped event tracking via `UCCRAnalyticsSubsystem`.
- **Fade / scene-transition system** – `UCCRFadeSubsystem` drives camera fades with optional hold and cut-ready delegate.
- **Debug tooling** – `UCCRDebugSubsystem` (stripped in Shipping) for world-state dumps, node jumps and inventory/achievement cheats.

---

## Project Structure

```
CCR-WORLD/
├── .gitignore                    # Excludes binary Content, build output
├── CCR.uproject                  # UE5 project descriptor
├── Config/
│   ├── DefaultEngine.ini         # Renderer, streaming, Android/Vulkan settings
│   ├── DefaultGame.ini           # GameMode, GameInstance, AssetManager config
│   ├── DefaultInput.ini          # Action/axis mappings (touch + gamepad fallback)
│   └── DefaultDeviceProfiles.ini # Mobile device performance profiles
└── Source/CCR/
    ├── Public/                   # 59 header files
    └── Private/                  # 59 implementation files
```

### Key Subsystems

| Subsystem | Role |
|---|---|
| `UCCRNarrativeRuntimeSubsystem` | Executes the node graph, fires events, predictively preloads chunks |
| `UCCRWorldStateSubsystemV2` | Fast flag/float/int store; evaluates branching conditions |
| `UCCRCheckpointSubsystem` | Auto-saves on safe windows (dialogue, choice, QTE start) |
| `UCCRSaveSlotManagerSubsystem` | 4-slot manual saves with metadata (time, chapter, datetime) |
| `UCCRResumeSubsystem` | Loads a save, restores world state + player spatial, starts narrative |
| `UCCRAudioSubsystem` | Music (fade), VO (per-node auto-play), SFX, cinematic ducking |
| `UCCRSubtitleSubsystem` | Auto-subtitles driven by narrative nodes; respects settings |
| `UCCRSettingsSubsystem` | Persisted audio volumes, subtitles, font scale, accessibility flags |
| `UCCRObjectiveSubsystem` | Quest/task tracking backed by world-state flags |
| `UCCRInventorySubsystem` | Item counts as world-state integers (`INV_<ItemId>`) |
| `UCCRAchievementSubsystem` | Named achievements; auto-mirrors to world state and notifications |
| `UCCRFadeSubsystem` | Camera fade-in / fade-out / fade-to-and-from with delegates |
| `UCCRCinematicSubsystem` | Level Sequence playback; fires `FinishCinematic` on NRS |
| `UCCRSceneDirectorSubsystem` | Full-level travel and sublevel streaming with loading overlay |
| `UCCRSpawnSubsystem` | Resolves `FName` spawn tags to `ACCRSpawnPoint` transforms |
| `UCCRStoryRegistrySubsystem` | Maps `ChunkId → FPrimaryAssetId` for the Asset Manager |
| `UCCRAsyncNarrativeLoaderSubsystem` | Required + predicted chunk preloading via FStreamableHandle |
| `UCCRPerformanceGovernorSubsystem` | Detects device tier; caps predicted-chunk count on low-end devices |
| `UCCRNotificationSubsystem` | Timed pop-up notifications (checkpoint saved, item picked up, achievement) |
| `UCCRHapticLibrarySubsystem` | Named haptic patterns (QTE_Success, QTE_Failure, …) |
| `UCCRAnalyticsSubsystem` | Session-scoped analytics events |
| `UCCRPlayTimeSubsystem` | Cumulative play time (excludes paused time); persisted in saves |
| `UCCRPhotoModeSubsystem` | Screenshot capture with HUD toggle |
| `UCCRDebugSubsystem` | Dev-only: world-state dump, node jump, inventory/achievement cheats |
| `UCCRAccessibilitySubsystem` | Runtime application of font scale and reduced-motion settings |
| `UCCRLocalizationHelper` | Culture switching via UCCRSettingsSubsystem |
| `UCCRInputMappingSubsystem` | Runtime input mapping context management |
| `UCCRSpeakerRegistrySubsystem` | Maps `SpeakerTag → FCCRSpeakerData` (name, portrait, VO prefix) |

### Actors / Components

| Class | Role |
|---|---|
| `ACCRGameMode` | Sets up Controller, HUD, GameState; starts MainMenu phase on PostLogin |
| `ACCRGameState` | Tracks `ECCRGamePhase`; broadcasts `OnGamePhaseChanged` |
| `UCCRGameInstance` | Owner of all subsystems; fires `OnCCRInit` / `OnCCRShutdown` Blueprint events |
| `ACCRGameHUD` | Creates and manages all widget instances; reacts to game-phase changes |
| `ACCRTouchController` | Mobile PlayerController; handles LongPress / Tap / Swipe QTE gestures |
| `ACCRSpawnPoint` | Tagged spawn location actor for respawn/resume positioning |
| `ACCRInteractableActor` | World actor that fires a narrative event trigger on player interaction |
| `ACCREventTriggerActor` | Overlap-based trigger that sets world-state flags or jumps narrative nodes |
| `ACCRAmbientSoundZoneActor` | Proximity-based ambient audio zone |
| `UCCRConditionalActorComponent` | Shows/hides or enables/disables its owning actor based on world-state conditions |

### Data Assets

| Class | Asset Type | Content Path |
|---|---|---|
| `UCCRStoryChunk` | `CCRStoryChunk` | `Content/CCR/Story/` |
| `UCCRCharacterDefinition` | `CCRCharacterDefinition` | `Content/CCR/Characters/` |
| `UCCRInventoryItemDefinition` | `CCRInventoryItem` | `Content/CCR/Items/` |

---

## Getting Started

### Prerequisites

- Unreal Engine **5.3** (installed via Epic Games Launcher or source build)
- Android SDK / NDK configured in UE Editor (for device builds)
- Vulkan-capable Android device (API 26+, ARM64)

### Opening the Project

1. Double-click `CCR.uproject` to open in UE5 Editor.
2. On first open the engine will ask to compile C++ modules — click **Yes**.
3. Assign Blueprint subclasses to the `ACCRGameHUD` widget class properties (e.g. `WBP_CCRDialogue` → `DialogueWidgetClass`).
4. Create `UCCRStoryChunk` data assets under `Content/CCR/Story/` and author narrative content.

### Blueprint Subclasses Required

| C++ Base | Suggested Blueprint Name | Where to assign |
|---|---|---|
| `UCCRDialogueWidget` | `WBP_CCRDialogue` | `ACCRGameHUD.DialogueWidgetClass` |
| `UCCRQTEWidget` | `WBP_CCRQTE` | `ACCRGameHUD.QTEWidgetClass` |
| `UCCRPauseWidget` | `WBP_CCRPause` | `ACCRGameHUD.PauseWidgetClass` |
| `UCCRLoadingWidget` | `WBP_CCRLoading` | `ACCRGameHUD.LoadingWidgetClass` |
| `UCCRMainMenuWidget` | `WBP_CCRMainMenu` | `ACCRGameHUD.MainMenuWidgetClass` |
| `UCCRSettingsWidget` | `WBP_CCRSettings` | `ACCRGameHUD.SettingsWidgetClass` |
| `UCCRNotificationWidget` | `WBP_CCRNotification` | `ACCRGameHUD.NotificationWidgetClass` |
| `UCCRChapterTransitionWidget` | `WBP_CCRChapterTransition` | `ACCRGameHUD.ChapterTransitionWidgetClass` |
| `UCCRCreditsWidget` | `WBP_CCRCredits` | `ACCRGameHUD.CreditsWidgetClass` |
| `UCCRCutsceneSkipWidget` | `WBP_CCRCutsceneSkip` | `ACCRGameHUD.CutsceneSkipWidgetClass` |
| `UCCRObjectiveWidget` | `WBP_CCRObjective` | `ACCRGameHUD.ObjectiveWidgetClass` |
| `UCCRGameInstance` | `BP_CCRGameInstance` | Project Settings → Game Instance Class |

### Content Folder Structure

```
Content/
├── CCR/
│   ├── Maps/          # UE5 level files (.umap) — one per chapter/area
│   ├── Blueprints/    # Blueprint widgets (WBP_*) and actor BPs (BP_*)
│   ├── Story/
│   │   ├── Prologue/  # UCCRStoryChunk assets: PROLOGUE_01, PROLOGUE_02
│   │   ├── Chapter1/  # UCCRStoryChunk assets: CH1_01, CH1_02
│   │   ├── Chapter2/  # UCCRStoryChunk assets: CH2_01, CH2_02
│   │   └── Chapter3/  # UCCRStoryChunk assets: CH3_01 (Finale → End → Credits)
│   ├── Characters/    # UCCRCharacterDefinition data assets (.uasset)
│   ├── Items/         # UCCRInventoryItemDefinition data assets (.uasset)
│   ├── Textures/      # Portrait and UI textures
│   ├── Audio/         # VO cues, music, and SFX
│   └── Cinematics/    # Level Sequences for cutscenes
```

> **Note:** Binary `.umap` and `.uasset` files are excluded from Git by `.gitignore`.
> Use Perforce, Git LFS, or an asset server to manage binary Content files.
> JSON manifest files in each Story / Characters / Items subfolder describe the full
> data to enter in the UE5 Editor when creating the `.uasset` instances.

### Zasady narracyjne

| Zasada | Opis |
|---|---|
| **ANDRZEJ = ojciec ZUZI** | Protagonista podróżuje przez WSZYSTKIE 19 osi czasu, aby odnaleźć córkę |
| **ZUZIA jest izolowana** | David i Weronika trzymają Zuzię z dala od Andrzeja. NIE dochodzi do fizycznego spotkania w Tomie 3. |
| **WERONIKA tylko z DAVIDEM** | Weronika NIGDY nie spotkała Evy. Sceny Weroniki i Evy NIE krzyżują się |
| **CIEŃ = alter ego ANDRZEJA** | Odkrycie w osi T17: Cień to wersja Andrzeja, który poddał się szukaniu |
| **T19 – Rezonator, nie spotkanie** | Andrzej wysyła wiadomość do Zuzi przez Kosmiczny Rezonator Świadomości. Brak fizycznego spotkania. |
| **Tom 4 – ZUZIA protagonistka** | Zuzia idzie śladami ojca przez 19 osi czasu – fundament Tomu 4 |
| **Otwarcie gry** | Po tytule: ciepły szept dziecka — *„Czas nie płynie… czas oddycha."* |

### Complete Story Flow

📋 **Pełny przegląd wszystkich 19 osi czasu:** [`Content/CCR/Story/AXES_OVERVIEW.md`](Content/CCR/Story/AXES_OVERVIEW.md)

**Przepływ główny — 19 osi czasu:**

```
PROLOGUE → szept dziecka: "Czas nie płynie… czas oddycha."
  → T01: POCZĄTEK ŚWIATŁA 1679
  → T02: WARSZAWA 1983
  → T03: ŁĘCZNA–LUBLIN 2010–2018
  → T04: BEŁŻYCE 2016
  → T05: NOVAKY (SŁOWACJA) 2018–2023
  → T06: CSM SEVER (CZECHY) 2024
  → T07: CIESZYN 2025
  → T08: CIEŃ SYSTEMU 2035
  → T09: FOTONY PAMIĘCI 2045
  → T10: ARCHIWUM CISZY 2077
  → T11: TERMINAL ŚWIATŁA 2100
  → T12: CHICAGO 2135
  → T13: TRANSMISJA WSPOMNIEŃ 2150
  → T14: ŚWIATŁO W PRÓŻNI 2222
  → T15: NEO PARIS 2345
  → T16: KOD PAMIĘCI 2378
  → T17: POWROTY DO CIENIA 2378
  → T18: DEVIL MIND 2777
  → T19: ŚWIATŁO KTÓRE TRWA ∞  ← FINAŁ TOMU 3:
       Andrzej WYSYŁA OSTATNIĄ WIADOMOŚĆ do Zuzi przez Kosmiczny Rezonator Świadomości
       NIE dochodzi do fizycznego spotkania
       Andrzej rozpływa się w wieczności
       ↓ PROLOG TOMU 4: Zuzia słyszy wiadomość ojca i idzie jego śladami →
```

| Chunk | Oś | Lokacja | Postacie (skrócone) |
|---|---|---|---|
| `PROLOGUE_01` + `PROLOGUE_02` | PROLOGUE | Otwarcie | DZIECKO (szept), ANDRZEJ, ZUZIA |
| `T01_01` | T01 | POCZĄTEK ŚWIATŁA 1679 | ANDRZEJ, CIEŃ, EIMSTAIN |
| `T02_01` | T02 | WARSZAWA 1983 | ANDRZEJ, KAFKA, BATISTA |
| `T03_01` | T03 | ŁĘCZNA–LUBLIN 2010–2018 | ANDRZEJ, DAVID, WERONIKA |
| `T04_01` | T04 | BEŁŻYCE 2016 | ANDRZEJ, KAFKA, EVA |
| `T05_01` | T05 | NOVAKY (SŁOWACJA) 2018–2023 | ANDRZEJ, BATISTA, CIEŃ |
| `T06_01` | T06 | CSM SEVER (CZECHY) 2024 | ANDRZEJ, EIMSTAIN, KAFKA |
| `T07_01` | T07 | CIESZYN 2025 | ANDRZEJ, DAVID, WERONIKA |
| `T08_01` | T08 | CIEŃ SYSTEMU 2035 | ANDRZEJ, CIEŃ, EVA |
| `T09_01` | T09 | FOTONY PAMIĘCI 2045 | ANDRZEJ, EIMSTAIN |
| `T10_01` | T10 | ARCHIWUM CISZY 2077 | ANDRZEJ, BATISTA, KAFKA |
| `T11_01` | T11 | TERMINAL ŚWIATŁA 2100 | ANDRZEJ, CIEŃ, DAVID, WERONIKA |
| `T12_01` | T12 | CHICAGO 2135 | ANDRZEJ, EVA, BATISTA |
| `T13_01` | T13 | TRANSMISJA WSPOMNIEŃ 2150 | ANDRZEJ, KAFKA, CIEŃ |
| `T14_01` | T14 | ŚWIATŁO W PRÓŻNI 2222 | ANDRZEJ, EIMSTAIN |
| `T15_01` | T15 | NEO PARIS 2345 | ANDRZEJ, EVA, KAFKA |
| `T16_01` | T16 | KOD PAMIĘCI 2378 | ANDRZEJ, DAVID, WERONIKA |
| `T17_01` | T17 | POWROTY DO CIENIA 2378 | ANDRZEJ, CIEŃ, EIMSTAIN |
| `T18_01` | T18 | DEVIL MIND 2777 | ANDRZEJ, CIEŃ, BATISTA |
| `T19_01` | T19 | ŚWIATŁO KTÓRE TRWA ∞ | ANDRZEJ + EIMSTAIN (echo) + REZONATOR. **Brak fizycznego spotkania.** Andrzej wysyła wiadomość do Zuzi przez Kosmiczny Rezonator Świadomości. Prolog Tomu 4. |

### Authoring Story Content

1. Create a `UCCRStoryChunk` data asset in `Content/CCR/Story/`.
2. Set `ChunkId`, `AxisId`, `EntryNodeId`.
3. Add `FCCRNode` entries to `Nodes[]`:
   - **Dialogue** – fill `DialogueText`, `SpeakerTag`, `NextAfterDialogue`.
   - **Choice** – fill `Choices[]` (each with `Label`, `TargetNodeId`, optional `ShowConditions`).
   - **Condition** – fill `Conditions[]`, `ConditionTrueNodeId`, `ConditionFalseNodeId`.
   - **SetValues** – fill `SetOps[]` and `NextAfterSet`.
   - **QTE** – fill `GestureType`, `TimeWindowSec`, `QTESuccessNodeId`, `QTEFailNodeId`.
   - **Cinematic** – assign `CinematicSequence`, set `NextAfterCinematic`, toggle `bSkippable`.
   - **Jump** – set `TargetChunkId` and `EntryNodeInTarget`.
   - **End** – fires `OnNarrativeEnded` → triggers credits.

### Authoring Character Data

1. Create a `UCCRCharacterDefinition` data asset in `Content/CCR/Characters/`.
2. Set `CharacterId` (must be unique; used as the `PrimaryAssetName`).
3. Set `DisplayName` (localised text shown above the dialogue bubble) and `ShortName`.
4. Set `SpeakerTag` to match `FCCRNode::SpeakerTag` in all dialogue nodes for this character.
5. Set `VOKeyPrefix` — `UCCRAudioSubsystem` will auto-play `<VOKeyPrefix>_<NodeId>` cues.
6. Set `MetFlag` (e.g. `MET_ZUZIA`) — the world-state flag written when the player first meets this character.
7. Add `FCCRPortraitEntry` items to `Portraits[]` — one per expression state (Neutral, Happy, Sad, Angry, Surprised, Fear).
   - `StateName` must match values used in `FCCRNode::ExpressionTag` in the story nodes.
   - `UCCRCharacterDefinition::GetPortraitForExpression(ExpressionTag)` resolves the correct portrait, falling back to "Neutral".
8. List any `AssociatedItemIds` referencing `CCRInventoryItem` assets in `Content/CCR/Items/`.

### Authoring Inventory Items

1. Create a `UCCRInventoryItemDefinition` data asset in `Content/CCR/Items/`.
2. Set `ItemId` (must match the `FName` passed to `UCCRInventorySubsystem::AddItem()`).
3. Set `DisplayName` and `Description`.
4. Assign an `Icon` texture (soft reference).
5. Toggle `bCanUse` / `bConsumedOnUse` and optionally set `UseWorldStateFlag`.

---

## Save System

| Slot | Name | Written by |
|---|---|---|
| Auto-save | `CCRSaveSlot` | `UCCRCheckpointSubsystem` on narrative safe windows |
| Manual 0–3 | `CCRSave_0` … `CCRSave_3` | `UCCRSaveSlotManagerSubsystem::SaveToSlot(index)` |
| Settings | `CCRSettings` | `UCCRSettingsSubsystem` on every setting change |

---

## Android Build

The project is pre-configured for Android ARM64 + Vulkan:
- `MinSDKVersion = 26`, `TargetSDKVersion = 33`
- `bSupportsVulkan = True`, `bBuildForArm64 = True`
- Portrait orientation

To package: **Platforms → Android → Package Project**.

---

## License

Copyright 2024 CCR Studio. All Rights Reserved.

