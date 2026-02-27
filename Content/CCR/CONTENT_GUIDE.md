# CCR WORLD — Content Guide

## Zasady narracyjne (MUST READ przed tworzeniem assets)

| Zasada | Opis |
|---|---|
| **ANDRZEJ = ojciec ZUZI** | Protagonista podróżuje przez WSZYSTKIE 19 osi czasu, aby odnaleźć córkę – Tom 3 |
| **ZUZIA jest izolowana** | David i Weronika trzymają Zuzię z dala od Andrzeja przez cały Tom 3. NIE dochodzi do fizycznego spotkania. |
| **WERONIKA tylko z DAVIDEM** | Weronika NIGDY nie spotkała Evy. Weronika i Eva NIE pojawiają się razem |
| **CIEŃ = alter ego ANDRZEJA** | Odkrycie w osi T17: Cień to wersja Andrzeja, który poddał się szukaniu |
| **T19: REZONATOR, nie spotkanie** | W T19 (∞) Andrzej wysyła ostatnią wiadomość do Zuzi przez Kosmiczny Rezonator Świadomości. Nie dochodzi do fizycznego spotkania. Andrzej rozpływa się w wieczności. |
| **TOM 4: ZUZIA jako protagonistka** | Zuzia odbiera wiadomość ojca przez Rezonator i idzie jego śladami – to fundament Tomu 4 |
| **Otwarcie gry** | Po tytule: ciepły szept dziecka — *„Czas nie płynie… czas oddycha."* |

---

## Folder Tree

```
Content/CCR/
+-- Maps/                           <- Level maps (.umap) – jedna mapa na os czasu
+-- Story/
|   +-- Prologue/                   <- Otwarcie gry + szept dziecka
|   +-- T01_PoczatekSwiatla1679/    <- Os 1: POCZATEK SWIATLA 1679
|   +-- T02_Warszawa1983/           <- Os 2: WARSZAWA 1983
|   +-- T03_LecznaLublin2010/       <- Os 3: LECZNA–LUBLIN 2010–2018
|   +-- T04_Belzyce2016/            <- Os 4: BELZYCE 2016
|   +-- T05_Novaky2018/             <- Os 5: NOVAKY (SLOWACJA) 2018–2023
|   +-- T06_CsmSever2024/           <- Os 6: CSM SEVER (CZECHY) 2024
|   +-- T07_Cieszyn2025/            <- Os 7: CIESZYN 2025
|   +-- T08_CienSystemu2035/        <- Os 8: CIEN SYSTEMU 2035
|   +-- T09_FotonyPamieci2045/      <- Os 9: FOTONY PAMIECI 2045
|   +-- T10_ArchiwumCiszy2077/      <- Os 10: ARCHIWUM CISZY 2077
|   +-- T11_TerminalSwiatla2100/    <- Os 11: TERMINAL SWIATLA 2100
|   +-- T12_Chicago2135/            <- Os 12: CHICAGO 2135
|   +-- T13_TransmisjaWspomnien2150/ <- Os 13: TRANSMISJA WSPOMNIEN 2150
|   +-- T14_SwiatloProzni2222/      <- Os 14: SWIATLO W PROZNI 2222
|   +-- T15_NeoParis2345/           <- Os 15: NEO PARIS 2345
|   +-- T16_KodPamieci2378/         <- Os 16: KOD PAMIECI 2378
|   +-- T17_PowrotyCienia2378/      <- Os 17: POWROTY DO CIENIA 2378
|   +-- T18_DevilMind2777/          <- Os 18: DEVIL MIND 2777
|   +-- T19_SwiatloKtoreTrwa/        <- Os 19: SWIATLO KTORE TRWA INFINITY (final)
+-- Characters/
+-- Items/
+-- Blueprints/
|   +-- Widgets/
|   +-- GameFramework/
+-- Audio/
|   +-- Music/
|   +-- VO/
|   |   +-- ANDRZEJ/  ZUZIA/  EVA/  CIEN/  EIMSTAIN/
|   |   +-- BATISTA/  WERONIKA/  DAVID/  KAFKA/  DZIECKO/
|   +-- SFX/  Ambient/
+-- Textures/
|   +-- Portraits/
|   |   +-- ANDRZEJ/  ZUZIA/  EVA/  CIEN/  EIMSTAIN/
|   |   +-- BATISTA/  WERONIKA/  DAVID/  KAFKA/
|   +-- UI/  Items/
+-- Cinematics/
    +-- Prologue/  T18_DevilMind2777/  T19_SwiatloKtoreTrwa/
```

---

## Osie Czasu — 19 Lokacji

| Nr | AxisId | Lokacja | Rok | Postacie w scenie | Plik |
|---|---|---|---|---|---|
| 0 | `PROLOGUE` | Otwarcie – szept dziecka | — | DZIECKO (glos), ANDRZEJ, ZUZIA | `Prologue/PROLOGUE_STORY.json` |
| 1 | `T01` | POCZATEK SWIATLA | 1679 | ANDRZEJ, CIEN, EIMSTAIN | `T01_STORY.json` |
| 2 | `T02` | WARSZAWA | 1983 | ANDRZEJ, KAFKA, BATISTA | `T02_STORY.json` |
| 3 | `T03` | LECZNA – LUBLIN | 2010–2018 | ANDRZEJ, DAVID, WERONIKA *(EVA NIE)* | `T03_STORY.json` |
| 4 | `T04` | BELZYCE | 2016 | ANDRZEJ, KAFKA, EVA *(WERONIKA NIE)* | `T04_STORY.json` |
| 5 | `T05` | NOVAKY (SLOWACJA) | 2018–2023 | ANDRZEJ, BATISTA, CIEN | `T05_STORY.json` |
| 6 | `T06` | CSM SEVER (CZECHY) | 2024 | ANDRZEJ, EIMSTAIN, KAFKA | `T06_STORY.json` |
| 7 | `T07` | CIESZYN | 2025 | ANDRZEJ, DAVID, WERONIKA *(EVA NIE; Zuzia za granica)* | `T07_STORY.json` |
| 8 | `T08` | CIEN SYSTEMU | 2035 | ANDRZEJ, CIEN, EVA *(WERONIKA NIE)* | `T08_STORY.json` |
| 9 | `T09` | FOTONY PAMIECI | 2045 | ANDRZEJ, EIMSTAIN (echo Zuzi) | `T09_STORY.json` |
| 10 | `T10` | ARCHIWUM CISZY | 2077 | ANDRZEJ, BATISTA, KAFKA | `T10_STORY.json` |
| 11 | `T11` | TERMINAL SWIATLA | 2100 | ANDRZEJ, CIEN, DAVID, WERONIKA *(EVA NIE)* | `T11_STORY.json` |
| 12 | `T12` | CHICAGO | 2135 | ANDRZEJ, EVA, BATISTA *(WERONIKA NIE)* | `T12_STORY.json` |
| 13 | `T13` | TRANSMISJA WSPOMNIEN | 2150 | ANDRZEJ, KAFKA, CIEN | `T13_STORY.json` |
| 14 | `T14` | SWIATLO W PROZNI | 2222 | ANDRZEJ, EIMSTAIN | `T14_STORY.json` |
| 15 | `T15` | NEO PARIS | 2345 | ANDRZEJ, EVA, KAFKA *(WERONIKA NIE)* | `T15_STORY.json` |
| 16 | `T16` | KOD PAMIECI | 2378 | ANDRZEJ, DAVID, WERONIKA *(EVA NIE; Weronika zaczyna watpic)* | `T16_STORY.json` |
| 17 | `T17` | POWROTY DO CIENIA | 2378 | ANDRZEJ, CIEN, EIMSTAIN *(odkrycie: Cien = alter Andrzeja)* | `T17_STORY.json` |
| 18 | `T18` | DEVIL MIND | 2777 | ANDRZEJ, CIEN (finalna konfrontacja), BATISTA | `T18_STORY.json` |
| 19 | `T19` | SWIATLO KTORE TRWA | ∞ | ANDRZEJ, EIMSTAIN (echo), FX_REZONATOR – **BRAK fizycznego spotkania**. Andrzej wysyła wiadomość przez Kosmiczny Rezonator Świadomości. Andrzej rozpływa się w wieczności. Prolog Tomu 4. | `T19_STORY.json` |

### Przepływ narracyjny

```
PROLOGUE (szept: "Czas nie plynie... czas oddycha.")
  -> T01 (1679) -> T02 (1983) -> T03 (2010-2018) -> T04 (2016)
  -> T05 (Slowacja) -> T06 (Czechy) -> T07 (Cieszyn)
  -> T08 (2035) -> T09 (2045) -> T10 (2077) -> T11 (2100)
  -> T12 (Chicago 2135) -> T13 (2150) -> T14 (2222)
  -> T15 (Neo Paryż 2345) -> T16 (2378-A) -> T17 (2378-B)
  -> T18 (Devil Mind 2777)
  -> T19 (∞ – FINAŁ T3: Andrzej WYSYŁA WIADOMOŚĆ przez Kosmiczny Rezonator Świadomości)
         NIE dochodzi do fizycznego spotkania z Zuzią
         Prolog Tomu 4: Zuzia słyszy wiadomość ojca → "Idę."
```

---

## Maps (Lokacje UE5)

Kazda os czasu = osobna mapa `.umap`. Prefiks: `L_CCR_<AxisId>`.

| Mapa | Sciezka | Os | Opis |
|---|---|---|---|
| `L_CCR_MainMenu` | `Maps/MainMenu.umap` | — | Menu glowne |
| `L_CCR_Loading` | `Maps/Loading.umap` | — | Poziom przejsciowy |
| `L_CCR_Prologue` | `Maps/Prologue.umap` | PROLOGUE | Otwarcie gry |
| `L_CCR_T01` | `Maps/T01_PoczatekSwiatla1679.umap` | T01 | Rok 1679 |
| `L_CCR_T02` | `Maps/T02_Warszawa1983.umap` | T02 | Warszawa 1983 |
| `L_CCR_T03` | `Maps/T03_LecznaLublin2010.umap` | T03 | Leczna-Lublin 2010-2018 |
| `L_CCR_T04` | `Maps/T04_Belzyce2016.umap` | T04 | Belzyce 2016 |
| `L_CCR_T05` | `Maps/T05_Novaky2018.umap` | T05 | Novaky Slowacja 2018-2023 |
| `L_CCR_T06` | `Maps/T06_CsmSever2024.umap` | T06 | CSM Sever Czechy 2024 |
| `L_CCR_T07` | `Maps/T07_Cieszyn2025.umap` | T07 | Cieszyn 2025 |
| `L_CCR_T08` | `Maps/T08_CienSystemu2035.umap` | T08 | Cien Systemu 2035 |
| `L_CCR_T09` | `Maps/T09_FotonyPamieci2045.umap` | T09 | Fotony Pamieci 2045 |
| `L_CCR_T10` | `Maps/T10_ArchiwumCiszy2077.umap` | T10 | Archiwum Ciszy 2077 |
| `L_CCR_T11` | `Maps/T11_TerminalSwiatla2100.umap` | T11 | Terminal Swiatla 2100 |
| `L_CCR_T12` | `Maps/T12_Chicago2135.umap` | T12 | Chicago 2135 |
| `L_CCR_T13` | `Maps/T13_TransmisjaWspomnien2150.umap` | T13 | Transmisja Wspomnien 2150 |
| `L_CCR_T14` | `Maps/T14_SwiatloProzni2222.umap` | T14 | Swiatlo w Prozni 2222 |
| `L_CCR_T15` | `Maps/T15_NeoParis2345.umap` | T15 | Neo Paris 2345 |
| `L_CCR_T16` | `Maps/T16_KodPamieci2378.umap` | T16 | Kod Pamieci 2378 |
| `L_CCR_T17` | `Maps/T17_PowrotyCienia2378.umap` | T17 | Powroty do Cienia 2378 |
| `L_CCR_T18` | `Maps/T18_DevilMind2777.umap` | T18 | Devil Mind 2777 |
| `L_CCR_T19` | `Maps/T19_SwiatloKtoreTrwa.umap` | T19 | Swiatlo Ktore Trwa ∞ (final) |

**Setup dla kazdego poziomu:**
1. Dodaj `BP_CCRGameMode` do WorldSettings -> GameMode Override.
2. Dodaj `ACCRSpawnPoint` z unikalnymi `SpawnTag` dla ANDRZEJA i obecnych postaci.
3. Dodaj `ACCREventTriggerActor` w strefach narracyjnych.
4. Dodaj `ACCRAmbientSoundZoneActor` z muzyka tematyczna osi czasu.

---

## Story Assets (UCCRStoryChunk)

### Tworzenie Story Chunk w UE5 Editor

1. PPM w odpowiednim folderze `Content/CCR/Story/<OsCzasu>/`.
2. **Miscellaneous -> Data Asset -> CCRStoryChunk**.
3. Nazwa: `DA_Story_<ChunkId>` (np. `DA_Story_T01_01`).
4. Wypelnij `ChunkId`, `AxisId`, `EntryNodeId`, `ChapterTitle`, `ChapterSubtitle`.
5. Dodaj `FCCRNode` do tablicy `Nodes`.

### Konwencja NodeId

```
<AxisId>_<ChunkNr>_<Sekwencja>
np.:  T01_01_010    (os T01, chunk 01, wezel 010)
      T13_01_060    (os T13, chunk 01, wezel 060)
      PRO_01_WHISPER_002  (Prologue, chunk 01, wezel szeptu)
```

### Node Types uzywane w CCR WORLD

| Typ | Opis |
|---|---|
| `TitleCard` | Karta tytulowa (logo + napis). Uzywana na otwarciu gry. |
| `AmbientVoice` | Glos w tle bez portretu. Uzywany do szeptu dziecka. |
| `Cinematic` | Level Sequence cutscenka. |
| `Dialogue` | Kwestia postaci z portretem. |
| `Choice` | Wybor gracza (2-4 opcje). |
| `QTE` | Quick Time Event (Swipe / Tap / LongPress). |
| `SetValues` | Ustaw flagi/wartosci w stanie gry. |
| `Condition` | Rozgalezienie na podstawie stanu gry. |
| `Jump` | Przejscie do innego ChunkId. |
| `End` | Koniec podrozy / koniec gry. |

---

## Characters

| CharacterId | Rola | Relacje |
|---|---|---|
| `ANDRZEJ` | Protagonist, ojciec Zuzi | Szuka Zuzi przez WSZYSTKIE 19 osi czasu |
| `ZUZIA` | Córka Andrzeja | Izolowana przez Davida i Weronikę przez cały Tom 3. NIE spotyka się z Andrzejem fizycznie. W T19 słyszy wiadomość ojca przez Rezonator (tylko głos). TOM 4: protagonistka idąca śladami ojca. |
| `DZIECKO` | Glos otwarcia | Szept: "Czas nie plynie... czas oddycha." – tylko VO, brak portretu |
| `EVA` | Niezalezna sojuszniczka | NIGDY z Weronika w tej samej scenie |
| `CIEN` | Antagonista / alter ego | Odkrycie T17: to wersja Andrzeja ktory sie poddal |
| `EIMSTAIN` | Znawca osi czasu | Wyjasnia mechanizm podrozy |
| `BATISTA` | Obserwator -> sojusznik | W T18 definitywnie staje po stronie Andrzeja |
| `WERONIKA` | Strasznik izolacji Zuzi | TYLKO z DAVIDEM. Nigdy nie widziala EVY. Watpliwosci: T16 |
| `DAVID` | Strasznik izolacji Zuzi | Zawsze z WERONIKA. Wykonuje rozkazy systemu |
| `KAFKA` | Posrednik informacji | Zna podziemia kazdej osi czasu |

---

## Audio

### Muzyka tematyczna

| Klucz | Plik | Ery |
|---|---|---|
| `MUS_MainMenu` | `Audio/Music/MUS_MainMenu.wav` | Menu |
| `MUS_Prologue` | `Audio/Music/MUS_Prologue.wav` | Otwarcie + szept |
| `MUS_Historical` | `Audio/Music/MUS_Historical.wav` | T01–T07 (1679–2025) |
| `MUS_NearFuture` | `Audio/Music/MUS_NearFuture.wav` | T08–T11 (2035–2100) |
| `MUS_DeepFuture` | `Audio/Music/MUS_DeepFuture.wav` | T12–T15 (2135–2345) |
| `MUS_Eternal` | `Audio/Music/MUS_Eternal.wav` | T16–T19 (2378–∞) |
| `MUS_Tension` | `Audio/Music/MUS_Tension.wav` | Sceny z CIEN / QTE |
| `MUS_Finale` | `Audio/Music/MUS_Finale.wav` | T19 – finał Tomu 3 (Rezonator, rozpadnięcie się Andrzeja w wieczności) |

### VO Szept dziecka

| Asset | Sciezka | Uzycie |
|---|---|---|
| `VO_DZIECKO_WstepSzepty_01` | `Audio/VO/DZIECKO/VO_DZIECKO_WstepSzepty_01.wav` | Node PRO_01_WHISPER_002 |

Tekst: *„Czas nie płynie… czas oddycha."*  
Styl: ciepły, delikatny szept dziecka, bez muzyki w tle, cisza otoczenia.

---

## Cinematics

| Asset | Os | Wezel | Opis |
|---|---|---|---|
| `LS_Prologue_Intro` | PROLOGUE_01 | PRO_01_CIN_001 | Pierwsza cutscenka po szepcie |
| `LS_Prologue_Outro` | PROLOGUE_02 | PRO_02_CIN_001 | Przejscie do T01 |
| `LS_T18_FinalBattle` | T18_01 | T18_01_CIN (TODO) | Konfrontacja z Cieniem |
| `LS_T19_Epilog_Rezonator` | T19_01 | T19_01_CIN_EPILOG | Andrzej aktywuje Rezonator, wysyła wiadomość, rozpływa się w wieczności |

---

## .gitattributes (Git LFS)

```
Content/**/*.uasset filter=lfs diff=lfs merge=lfs -text
Content/**/*.umap    filter=lfs diff=lfs merge=lfs -text
Content/**/*.uexp    filter=lfs diff=lfs merge=lfs -text
Content/**/*.ubulk   filter=lfs diff=lfs merge=lfs -text
```
