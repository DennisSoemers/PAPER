# PAPER
[![C++23](https://img.shields.io/static/v1?label=standard&message=C%2B%2B23&color=blue&logo=c%2B%2B&&logoColor=white&style=flat)](https://en.cppreference.com/w/cpp/compiler_support)
[![Platform](https://img.shields.io/static/v1?label=platform&message=windows&color=dimgray&style=flat)](#)
![GitHub](https://img.shields.io/github/license/DennisSoemers/PAPER)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/DennisSoemers/PAPER)

The PAper Papyrus ExtendeR. Plugin to expose new functions and events to Skyrim's "Papyrus" scripting language.

This is an [SKSE](https://skse.silverlock.org/) plugin that can be loaded at runtime (when playing The Elder Scrolls V: Skyrim) and expose additional functionality to the game's scripting language. This functionality can be used by third-party mod authors for scripts in their mods.

## New Functions and Events

Detailed documentation on how to use the new functions and events can be found on [this repository's Wiki pages](https://github.com/DennisSoemers/PAPER/wiki).

### New Events

- [OnHit Events](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Events#onhit-events)
    - [`Event OnImpact(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked)`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Events#onimpact)
- [Equip Events](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Events#equip-events)
    - [`Event OnSpellEquipped(Spell akSpell, ObjectReference akReference)`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Events#onspellequipped)
    - [`Event OnSpellUnequipped(Spell akSpell, ObjectReference akReference)`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Events#onspellunequipped)
    - [`Event OnShoutEquipped(Shout akShout, ObjectReference akReference)`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Events#onshoutequipped)
    - [`Event OnShoutUnequipped(Shout akShout, ObjectReference akReference)`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Events#onshoutunequipped)
- [Inventory Events](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Events#inventory-events)
    - [`Event OnBatchItemsAdded(Form[] akBaseItems, Int[] aiItemCounts, ObjectReference[] akSourceContainers)`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Events#onbatchitemsadded)
    - [Event OnBatchItemsRemoved(Form[] akBaseItems, Int[] aiItemCounts, ObjectReference[] akDestContainers)](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Events#onbatchitemsremoved)

### New Functions

- [Resources](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Functions#resources)
    - [`bool Function ResourceExists(String asResourcePath) global native`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Functions#resourceexists)
    - [`String[] Function GetInstalledResources(String[] asStrings) global native`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Functions#getinstalledresources)
- [ActorBase](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Functions#actorbase)
    - [`ColorForm[] Function GetWarpaintColors(ActorBase akActorBase) global native`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Functions#getwarpaintcolors)
- [Inventory Events](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Functions#inventory-events)
    - [`int[] Function GetInventoryEventFilterIndices(Form[] akEventItems, Form akFilter) global native`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Functions#getinventoryeventfilterindices)
    - [`int[] Function UpdateInventoryEventFilterIndices(Form[] akEventItems, Form akFilter, int[] aiIndices) global native`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Functions#updateinventoryeventfilterindices)
    - [`Form[] Function ApplyInventoryEventFilterToForms(int[] aiIndicesToKeep, Form[] akFormArray) global native`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Functions#applyinventoryeventfiltertoforms)
    - [`int[] Function ApplyInventoryEventFilterToInts(int[] aiIndicesToKeep, int[] aiIntArray) global native`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Functions#applyinventoryeventfiltertoints)
    - [`ObjectReference[] Function ApplyInventoryEventFilterToObjs(int[] aiIndicesToKeep, ObjectReference[] akObjArray) global native`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Functions#applyinventoryeventfiltertoobjs)
- [Other](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Functions#other)
    - [`int[] Function GetPaperVersion() global native`](https://github.com/DennisSoemers/PAPER/wiki/New-Papyrus-Functions#getpaperversion)

## Download

The plugin can be downloaded from [its NexusMods page](https://www.nexusmods.com/skyrimspecialedition/mods/73849).

## Runtime Requirements (for users)

At runtime, the plugin has the following requirements. Any user of this plugin (or mods that in turn require this plugin) need to have this installed locally.

- The Elder Scrolls V: Skyrim Special Edition.
  - Tested with versions 1.5.97 (SSE) and 1.6.353 (the "Anniversary Edition").
  - Also compatible with versions 1.6.xxx (beyond 1.6.353), including GOG (tested by users), thanks to [CommonLibSSE NG](https://github.com/CharmedBaryon/CommonLibSSE-NG).
  - As of version 2.0.0, also compatible with Skyrim VR (tested by users).
- [Skyrim Script Extender](https://skse.silverlock.org/)
- [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)

## Build Requirements (for plugin developers)

Building the code in this repository has the following requirements.

- [Visual Studio](https://visualstudio.microsoft.com/).
- [CMake](https://cmake.org/).
- [Vcpkg](https://github.com/microsoft/vcpkg).

This project was set up exactly as in the [CommonLibSSE NG Sample Plugin](https://gitlab.com/colorglass/commonlibsse-sample-plugin), and I refer to that repository for highly detailed instructions on installation and building.

## See also

- [powerofthree's Papyrus Extender](https://www.nexusmods.com/skyrimspecialedition/mods/22854), which is similar in that its primary purpose is to expose new Papyrus functions and events, but much more impressive with a significantly greater scope.

## Credits

Thanks to:
- CharmedBaryon for [CommonLibSSE NG](https://github.com/CharmedBaryon/CommonLibSSE-NG) and the [CommonLibSSE NG Sample Plugin](https://gitlab.com/colorglass/commonlibsse-sample-plugin).
- The SKSE authors for [SKSE](http://skse.silverlock.org/).
- Meh321 for [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444).
- powerof3 for various (MIT-licensed) SKSE plugins that I used as example code, in particular [Papyrus Extender SSE](https://github.com/powerof3/PapyrusExtenderSSE).
- Nightfallstorm for pointing me to the `SendAndRelayEvent()` function.

## Disclaimer

- THIS MATERIAL IS NOT MADE, GUARANTEED OR SUPPORTED BY ZENIMAX OR ITS AFFILIATES.
- ALL FILES IN THE DOWNLOAD ARE PROVIDED ''AS IS'' WITHOUT ANY WARRANTY OR GUARANTEE OF ANY KIND. IN NO EVENT CAN THE AUTHOR BE HELD RESPONSIBLE FOR ANY CLAIMS, WHETHER OR NOT THEY ARE RELATED TO THE DOWNLOAD.
