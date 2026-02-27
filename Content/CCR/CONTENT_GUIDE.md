# CCR WORLD - Content Guide

This document describes the complete `Content/CCR/` folder structure and explains
what UE5 assets belong in each folder, with naming conventions and creation steps.

---

## Folder Tree

```
Content/CCR/
+-- Maps/                          <- Level maps (.umap) - one per location
+-- Story/                         <- UCCRStoryChunk data assets
|   +-- Prologue/
|   +-- Chapter1/  ... Chapter12/  <- Main story arc (Akt I-IV)
|   +-- Flashbacks/                <- Osobna os czasu: wspomnienia
|   +-- Parallel/                  <- Alternatywna os: inne perspektywy
+-- Characters/                    <- UCCRCharacterDefinition data assets
+-- Items/                         <- UCCRInventoryItemDefinition data assets
+-- Blueprints/
|   +-- Widgets/                   <- UMG Widget Blueprints (WBP_*)
|   +-- GameFramework/             <- BP_CCRGameInstance, BP_CCRGameMode, etc.
+-- Audio/
|   +-- Music/                     <- USoundWave / USoundCue
|   +-- VO/
|   |   +-- ANDRZEJ/
|   |   +-- ZUZIA/
|   |   +-- EVA/
|   |   +-- CIEN/
|   |   +-- EIMSTAIN/
|   |   +-- BATISTA/
|   |   +-- WERONIKA/
|   |   +-- DAVID/
|   |   +-- KAFKA/
|   +-- SFX/
|   +-- Ambient/
+-- Textures/
|   +-- Portraits/
|   |   +-- ANDRZEJ/
|   |   +-- ZUZIA/
|   |   +-- EVA/
|   |   +-- CIEN/
|   |   +-- EIMSTAIN/
|   |   +-- BATISTA/
|   |   +-- WERONIKA/
|   |   +-- DAVID/
|   |   +-- KAFKA/
|   +-- UI/
|   +-- Items/
+-- Cinematics/
    +-- Prologue/
    +-- Flashbacks/
    +-- Chapter12/                 <- Finalne sekwencje filmowe
```

---

## Maps / Lokacje

Create one `.umap` Level asset per location.

| Asset Name | Path | Powiazany rozdzial | Opis lokacji |
|---|---|---|---|
| `MainMenu` | `Maps/MainMenu.umap` | -- | Glowne menu |
| `Loading` | `Maps/Loading.umap` | -- | Poziom przejsciowy |
| `Prologue` | `Maps/Prologue.umap` | PROLOGUE | TODO: opis |
| `Chapter1` | `Maps/Chapter1.umap` | CH1 | TODO: opis |
| `Chapter2` | `Maps/Chapter2.umap` | CH2 | TODO: opis |
| `Chapter3` | `Maps/Chapter3.umap` | CH3 | TODO: opis |
| `Chapter4` | `Maps/Chapter4.umap` | CH4 | TODO: opis |
| `Chapter5` | `Maps/Chapter5.umap` | CH5 | TODO: opis |
| `Chapter6` | `Maps/Chapter6.umap` | CH6 | TODO: opis |
| `Chapter7` | `Maps/Chapter7.umap` | CH7 | TODO: opis |
| `Chapter8` | `Maps/Chapter8.umap` | CH8 | TODO: opis |
| `Chapter9` | `Maps/Chapter9.umap` | CH9 | TODO: opis |
| `Chapter10` | `Maps/Chapter10.umap` | CH10 | TODO: opis |
| `Chapter11` | `Maps/Chapter11.umap` | CH11 | TODO: opis |
| `Chapter12` | `Maps/Chapter12.umap` | CH12 (Finale) | TODO: opis |
| `Flashback1` | `Maps/Flashback1.umap` | FB1 | TODO: opis |
| `Flashback2` | `Maps/Flashback2.umap` | FB2 | TODO: opis |
| `Flashback3` | `Maps/Flashback3.umap` | FB3 | TODO: opis |
| `Parallel1` | `Maps/Parallel1.umap` | PAR1 (Cien) | TODO: opis |
| `Parallel2` | `Maps/Parallel2.umap` | PAR2 (Eva) | TODO: opis |

**Setup dla kazdego poziomu:**
1. Dodaj `BP_CCRGameMode` do WorldSettings -> GameMode Override.
2. Dodaj `ACCRSpawnPoint` z unikalnymi `SpawnTag`.
3. Dodaj `ACCREventTriggerActor` w strefach narracyjnych.
4. Dodaj `ACCRAmbientSoundZoneActor` z dźwiękiem otoczenia.
5. Ustaw `GameDefaultMap` w `DefaultEngine.ini` na `MainMenu`.

---

## Story Assets (UCCRStoryChunk)

### Os glowna (MAIN ARC)

Przepływ: PROLOGUE_01 -> PROLOGUE_02 -> CH1_01 -> CH1_02 -> ... -> CH12_02 -> End

| Chunk | AxisId | Rozdzial | Postacie | Plik manifestu |
|---|---|---|---|---|
| PROLOGUE_01, PROLOGUE_02 | PROLOGUE | Prolog | ANDRZEJ, ZUZIA | `Prologue/PROLOGUE_STORY.json` |
| CH1_01, CH1_02 | ACT1 | Rozdzial 1 | ANDRZEJ, EVA, CIEN | `Chapter1/CHAPTER1_STORY.json` |
| CH2_01, CH2_02 | ACT1 | Rozdzial 2 | ANDRZEJ, EIMSTAIN, BATISTA, WERONIKA | `Chapter2/CHAPTER2_STORY.json` |
| CH3_01, CH3_02 | ACT1 | Rozdzial 3 | ANDRZEJ, DAVID, KAFKA | `Chapter3/CHAPTER3_STORY.json` |
| CH4_01, CH4_02 | ACT1 | Rozdzial 4 | ANDRZEJ, ZUZIA, CIEN | `Chapter4/CHAPTER4_STORY.json` |
| CH5_01, CH5_02 | ACT1 | Rozdzial 5 | ANDRZEJ, EVA, ZUZIA, KAFKA | `Chapter5/CHAPTER5_STORY.json` |
| CH6_01, CH6_02 | ACT1 | Rozdzial 6 | ANDRZEJ, EIMSTAIN, EVA | `Chapter6/CHAPTER6_STORY.json` |
| CH7_01, CH7_02 | ACT2 | Rozdzial 7 | ANDRZEJ, BATISTA, WERONIKA, CIEN | `Chapter7/CHAPTER7_STORY.json` |
| CH8_01, CH8_02 | ACT2 | Rozdzial 8 | ANDRZEJ, DAVID, BATISTA | `Chapter8/CHAPTER8_STORY.json` |
| CH9_01, CH9_02 | ACT2 | Rozdzial 9 | ANDRZEJ, KAFKA, DAVID, ZUZIA | `Chapter9/CHAPTER9_STORY.json` |
| CH10_01, CH10_02 | ACT3 | Rozdzial 10 | ANDRZEJ, WERONIKA, KAFKA | `Chapter10/CHAPTER10_STORY.json` |
| CH11_01, CH11_02 | ACT3 | Rozdzial 11 | ANDRZEJ, CIEN, EIMSTAIN, WERONIKA | `Chapter11/CHAPTER11_STORY.json` |
| CH12_01, CH12_02 | ACT3 | Rozdzial 12 (Finale) | ANDRZEJ, EVA, CIEN | `Chapter12/CHAPTER12_STORY.json` |

### Os Wspomnien (FLASHBACK axis)

Osobna os czasu — uruchamiana ze scen glownych. Nie wchodzi w glowny lancuch Jump.

| Chunk | AxisId | Tytul | Postacie | Plik manifestu |
|---|---|---|---|---|
| FB1_01 | FLASHBACK | Wspomnienie 1 | ANDRZEJ, KAFKA | `Flashbacks/FLASHBACK1_STORY.json` |
| FB2_01 | FLASHBACK | Wspomnienie 2 | KAFKA, DAVID | `Flashbacks/FLASHBACK2_STORY.json` |
| FB3_01 | FLASHBACK | Wspomnienie 3 | ZUZIA, ANDRZEJ | `Flashbacks/FLASHBACK3_STORY.json` |

### Os Rownolegla (PARALLEL axis)

Alternatywne perspektywy — inne POV, te same wydarzenia.

| Chunk | AxisId | Tytul | POV | Plik manifestu |
|---|---|---|---|---|
| PAR1_01 | PARALLEL | Perspektywa: Cien | CIEN, EIMSTAIN | `Parallel/PARALLEL1_STORY.json` |
| PAR2_01 | PARALLEL | Perspektywa: Eva | EVA, WERONIKA | `Parallel/PARALLEL2_STORY.json` |

### Tworzenie Story Chunk w UE5 Editor

1. PPM w `Content/CCR/Story/<Rozdzial>/`.
2. **Miscellaneous -> Data Asset -> CCRStoryChunk**.
3. Nazwa: `DA_Story_<ChunkId>` (np. `DA_Story_CH4_01`).
4. Wypelnij `ChunkId`, `AxisId`, `EntryNodeId`, `ChapterTitle`, `ChapterSubtitle`.
5. Dodaj `FCCRNode` do tablicy `Nodes`.

### Konwencja nazewnictwa Node ID

```
<Skrot>_<Chunk>_<Sekwencja>
np.:  PRO_01_010    (Prologue, chunk 01, wezel 010)
      CH4_01_030    (Chapter 4, chunk 01, wezel 030)
      FB1_01_020    (Flashback 1, chunk 01, wezel 020)
      PAR2_01_010   (Parallel 2, chunk 01, wezel 010)
```

---

## Character Assets (UCCRCharacterDefinition)

| CharacterId | Wyswietlana nazwa | MetFlag | Pierwszy rozdzial |
|---|---|---|---|
| `ANDRZEJ` | Andrzej | -- | Protagonista – wszystkie |
| `ZUZIA` | Zuzia | `MET_ZUZIA` | Prolog |
| `EVA` | Eva | `MET_EVA` | Rozdzial 1 |
| `CIEN` | Cien | `MET_CIEN` | Rozdzial 1 |
| `EIMSTAIN` | Eimstain | `MET_EIMSTAIN` | Rozdzial 2 |
| `BATISTA` | Batista | `MET_BATISTA` | Rozdzial 2 |
| `WERONIKA` | Weronika | `MET_WERONIKA` | Rozdzial 2 |
| `DAVID` | David | `MET_DAVID` | Rozdzial 3 |
| `KAFKA` | Kafka | `MET_KAFKA` | Rozdzial 3 |

Pelna definicja: `Characters/CHARACTERS_MANIFEST.json`

### Konwencja nazewnictwa portretow

```
T_<CharacterId>_<StateName>
np.: T_ANDRZEJ_Neutral.png
     T_CIEN_Fear.png
```

Importuj do `Content/CCR/Textures/Portraits/<CharacterId>/`.

---

## Item Assets (UCCRInventoryItemDefinition)

Pelna definicja: `Items/ITEMS_MANIFEST.json`

Konwencja ikon: `T_Item_<ItemId>` -> `Content/CCR/Textures/Items/`

---

## Blueprint Widgets

| Blueprint | Baza C++ | Folder |
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

---

## Game Framework Blueprints

| Blueprint | Baza C++ |
|---|---|
| `BP_CCRGameInstance` | `UCCRGameInstance` |
| `BP_CCRGameMode` | `ACCRGameMode` |
| `BP_CCRGameHUD` | `ACCRGameHUD` |
| `BP_CCRTouchController` | `ACCRTouchController` |

---

## Audio

### Muzyka

| Klucz | Plik | Opis |
|---|---|---|
| `MENU` | `MUS_MainMenu.wav` | Temat menu glownego |
| `PROLOGUE` | `MUS_Prologue.wav` | Muzyka prologu |
| `ACT1` | `MUS_Act1.wav` | Temat Aktu I |
| `ACT2` | `MUS_Act2.wav` | Temat Aktu II |
| `ACT3` | `MUS_Act3.wav` | Temat Aktu III |
| `FLASHBACK` | `MUS_Flashback.wav` | Temat wspomnien |
| `PARALLEL` | `MUS_Parallel.wav` | Temat osi rownoleglejnej |
| `TENSION` | `MUS_Tension.wav` | Muzyka QTE / napiecia |
| `FINALE` | `MUS_Finale.wav` | Finale – Rozdzial 12 |

### SFX

| Asset | Opis |
|---|---|
| `SFX_QTE_Success` | Sukces QTE |
| `SFX_QTE_Fail` | Porazka QTE |
| `SFX_CheckpointSaved` | Zapis checkpointu |
| `SFX_ItemPickup` | Pobranie przedmiotu |
| `SFX_UIConfirm` | Potwierdzenie UI |
| `SFX_UIBack` | Cofniecie UI |

---

## Cinematics (Level Sequences)

| Asset | Rozdzial | Wezel | Opis |
|---|---|---|---|
| `LS_Prologue_Intro` | PROLOGUE_01 | PRO_01_CIN_001 | Intro prologu |
| `LS_Prologue_Outro` | PROLOGUE_02 | PRO_02_CIN_001 | Outro prologu |
| `LS_FB1_Intro` | FB1_01 | FB1_01_010 | Wspomnienie 1 - wejscie |
| `LS_FB3_Intro` | FB3_01 | FB3_01_010 | Wspomnienie 3 - wejscie |
| `LS_Chapter12_Finale` | CH12_02 | CH12_02_010 | Finale |
| `LS_Chapter12_Epilogue` | CH12_02 | CH12_02_050 | Epilog (mozliwy do pominiecia) |

---

## .gitignore dla Content

Binarne pliki `.uasset` i `.umap` nie powinny byc sledzone przez Git (uzywaj Perforce lub Git LFS).

Git LFS – dodaj do `.gitattributes`:
```
Content/**/*.uasset filter=lfs diff=lfs merge=lfs -text
Content/**/*.umap    filter=lfs diff=lfs merge=lfs -text
Content/**/*.uexp    filter=lfs diff=lfs merge=lfs -text
Content/**/*.ubulk   filter=lfs diff=lfs merge=lfs -text
```
