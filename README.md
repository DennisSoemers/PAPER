# PAPER
[![C++23](https://img.shields.io/static/v1?label=standard&message=C%2B%2B23&color=blue&logo=c%2B%2B&&logoColor=white&style=flat)](https://en.cppreference.com/w/cpp/compiler_support)
[![Platform](https://img.shields.io/static/v1?label=platform&message=windows&color=dimgray&style=flat)](#)
![GitHub](https://img.shields.io/github/license/DennisSoemers/PAPER)

The PAper Papyrus ExtendeR. Plugin to expose new functions and events to Skyrim's "Papyrus" scripting language.

This is an [SKSE](https://skse.silverlock.org/) plugin that can be loaded at runtime (when playing The Elder Scrolls V: Skyrim) and expose additional functionality to the game's scripting language. This functionality can be used by third-party mod authors for scripts in their mods.

## New Functions and Events

Documentation on how to use the new functions and events can be found on [this repository's Wiki pages](https://github.com/DennisSoemers/PAPER/wiki).

## Runtime Requirements (for users)

At runtime, the plugin has the following requirements. Any user of this plugin (or mods that in turn require this plugin) need to have this installed locally.

- [The Elder Scrolls V: Skyrim Special Edition](https://store.steampowered.com/app/489830/The_Elder_Scrolls_V_Skyrim_Special_Edition/).
  - Tested with version 1.6.353 (the "Anniversary Edition").
  - Probably also compatible with version 1.5.97 (SSE) and Skyrim VR, thanks to [CommonLibSSE NG](https://github.com/CharmedBaryon/CommonLibSSE-NG).
- [Skyrim Script Extender](https://skse.silverlock.org/)
- [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)

## Build Requirements (for plugin developers)

Building the code in this repository has the following requirements.

- [Visual Studio](https://visualstudio.microsoft.com/).
- [CMake](https://cmake.org/).
- [Vcpkg](https://github.com/microsoft/vcpkg).

This project was set up exactly as in the [CommonLibSSE NG Sample Plugin](https://gitlab.com/colorglass/commonlibsse-sample-plugin), and I refer to that repository for highly detailed instructions on installation and building.

## Credits

Thanks to:
- CharmedBaryon for [CommonLibSSE NG](https://github.com/CharmedBaryon/CommonLibSSE-NG) and the [CommonLibSSE NG Sample Plugin](https://gitlab.com/colorglass/commonlibsse-sample-plugin).
- The SKSE authors for [SKSE](http://skse.silverlock.org/).
- Meh321 for [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444).
- powerof3 for various (MIT-licensed) SKSE plugins that I used as example code, in particular [Papyrus Extender SSE](https://github.com/powerof3/PapyrusExtenderSSE).
