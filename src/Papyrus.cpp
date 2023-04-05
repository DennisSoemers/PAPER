#include "Papyrus.h"
#include "ResourceUtils.h"
#include "Version.h"

namespace PAPER {

	constexpr std::string_view PaperSKSEFunctions = "PAPER_SKSEFunctions";

	/**
	 * Array with version number (major, minor, patch) for the PAPER plugin.
	 */
	std::vector<std::int32_t> GetPaperVersion(RE::StaticFunctionTag*) {
        return {PROJECT_VER_MAJOR, PROJECT_VER_MINOR, PROJECT_VER_PATCH};
    }

	/**
	 * Will return true if and only if a file for the given path exists
	 * (may be either as a loose file or in a BSA).
	 */
    bool ResourceExists(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*,
                        std::string resourcePath) { 
        return ResourceUtils::ResourceExists(resourcePath);
	}

    /**
     * Returns, from the given list of strings, a new array of strings containing only those
     * strings that are recognised as installed resources.
     */
    std::vector<std::string> GetInstalledResources(RE::StaticFunctionTag*, const RE::reference_array<std::string> strings) {
        std::vector<std::string> installedResources;
        installedResources.reserve(strings.size());

        auto it = strings.begin();
        while (it != strings.end()) {
            if (ResourceUtils::ResourceExists(*it)) {
                installedResources.push_back(*it);
            }
            ++it;
        }

        return installedResources;
    }

    /**
     * Returns an array of colours for all the warpaints for which we are able
     * to detect that they have been applied to the character's face.
     */
    std::vector<RE::BGSColorForm*> GetWarpaintColors(RE::BSScript::Internal::VirtualMachine* a_vm,
        RE::VMStackID a_stackID, RE::StaticFunctionTag*,
        RE::TESNPC* actorBase) {

        std::vector<RE::BGSColorForm*> warpaintColors;

        if (!actorBase) {
            a_vm->TraceStack("ActorBase is None", a_stackID);
            return warpaintColors;
        }

        while (actorBase->tintLayers == nullptr && actorBase->faceNPC != nullptr && actorBase->faceNPC != actorBase) {
            actorBase = actorBase->faceNPC;
        }

        const auto sex = actorBase->GetSex();
        const auto race = actorBase->race;
        const auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::BGSColorForm>();

        if (race && factory && actorBase->tintLayers) {
            const auto faceRelatedData = race->faceRelatedData[sex];

            if (faceRelatedData) {
                // Loop through the tint layers of the ActorBase
                auto layer = actorBase->tintLayers->begin();
                while (layer != actorBase->tintLayers->end()) {
                    // Need interpolation value > 0.0 for the layer to be visible
                    if ((*layer) && (*layer)->GetInterpolationValue() > 0.f) {
                        const auto tintIndex = (*layer)->tintIndex;
                        
                        // Figure out from tint assets defined in Race's face data whether
                        // or not this actually could be a paint tint
                        bool isPaint = false;
                        auto tintAsset = faceRelatedData->tintMasks->begin();
                        while (tintAsset != faceRelatedData->tintMasks->end()) {
                            if (tintAsset) {
                                auto tintLayer = &((*tintAsset)->texture);
                                if (tintLayer && tintLayer->index == tintIndex) {
                                    const auto tintLayerType = tintLayer->skinTone.get();

                                    if (tintLayerType ==
                                            RE::TESRace::FaceRelatedData::TintAsset::TintLayer::SkinTone::kNone ||
                                        tintLayerType ==
                                            RE::TESRace::FaceRelatedData::TintAsset::TintLayer::SkinTone::kPaint) {
                                        // I've found some things that very much look like warpaint with type None
                                        // in the Creation Kit (e.g., Forsworn stuff in the Breton race), so we'll
                                        // also allow that type.
                                        isPaint = true;
                                    }

                                    break;
                                }
                            }

                            ++tintAsset;
                        }

                        if (isPaint) {
                            // We've found something that is paint, so add its color
                            auto color = factory->Create();

                            if (color) {
                                color->flags.reset(RE::BGSColorForm::Flag::kPlayable);
                                color->color = (*layer)->tintColor;
                                warpaintColors.push_back(color);
                            }
                        }
                    }

                    ++layer;
                }
            }
        }

        return warpaintColors;
    }

    std::vector<std::int32_t> GetInventoryEventFilterIndices(RE::StaticFunctionTag*,
                                                             const RE::reference_array<RE::TESForm*> akEventItems,
                                                             RE::TESForm* akFilter) {
        std::vector<std::int32_t> matchingIndices;

        auto formList = RE::TESForm::LookupByID<RE::BGSListForm>(akFilter->formID);
        if (formList) {
            for (int i = 0; i < akEventItems.size(); ++i) {
                if (formList->HasForm(akEventItems[i])) {
                    matchingIndices.push_back(i);
                }
            }
        } else {
            for (int i = 0; i < akEventItems.size(); ++i) {
                if (akFilter->formID == akEventItems[i]->formID) {
                    matchingIndices.push_back(i);
                }
            }
        }
        
        return matchingIndices;
    }

    std::vector<std::int32_t> UpdateInventoryEventFilterIndices(RE::StaticFunctionTag*,
                                                                const RE::reference_array<RE::TESForm*> akEventItems,
                                                                RE::TESForm* akFilter,
                                                                const RE::reference_array<std::int32_t> aiIndices) {
        std::vector<std::int32_t> matchingIndices;

        auto formList = RE::TESForm::LookupByID<RE::BGSListForm>(akFilter->formID);
        if (formList) {
            for (int i = 0; i < aiIndices.size(); ++i) {
                if (formList->HasForm(akEventItems[aiIndices[i]])) {
                    matchingIndices.push_back(aiIndices[i]);
                }
            }
        } else {
            for (int i = 0; i < aiIndices.size(); ++i) {
                if (akFilter->formID == akEventItems[aiIndices[i]]->formID) {
                    matchingIndices.push_back(aiIndices[i]);
                }
            }
        }

        return matchingIndices;
    }

    std::vector<RE::TESForm*> ApplyInventoryEventFilterToForms(RE::StaticFunctionTag*,
                                                               const RE::reference_array<std::int32_t> aiIndicesToKeep,
                                                               const RE::reference_array<RE::TESForm*> akFormArray) {
        std::vector<RE::TESForm*> remainingForms;

        for (int i = 0; i < aiIndicesToKeep.size(); ++i) {
            remainingForms.push_back(akFormArray[aiIndicesToKeep[i]]);
        }

        return remainingForms;
    }

    std::vector<int32_t> ApplyInventoryEventFilterToInts(RE::StaticFunctionTag*,
                                                         const RE::reference_array<std::int32_t> aiIndicesToKeep,
                                                         const RE::reference_array<std::int32_t> aiIntArray) {
        std::vector<int32_t> remainingInts;

        for (int i = 0; i < aiIndicesToKeep.size(); ++i) {
            remainingInts.push_back(aiIntArray[aiIndicesToKeep[i]]);
        }

        return remainingInts;
    }

    std::vector<RE::TESObjectREFR*> ApplyInventoryEventFilterToObjs(
        RE::StaticFunctionTag*, const RE::reference_array<std::int32_t> aiIndicesToKeep,
        const RE::reference_array<RE::TESObjectREFR*> akObjArray) {

        std::vector<RE::TESObjectREFR*> remainingObjs;

        for (int i = 0; i < aiIndicesToKeep.size(); ++i) {
            remainingObjs.push_back(akObjArray[aiIndicesToKeep[i]]);
        }

        return remainingObjs;
    }

	/**
	 * Provide bindings for all our Papyrus functions.
	 */
	bool Bind(RE::BSScript::IVirtualMachine* vm) {
        // Resources
        vm->RegisterFunction("ResourceExists", PaperSKSEFunctions, ResourceExists, true);
        vm->RegisterFunction("GetInstalledResources", PaperSKSEFunctions, GetInstalledResources, false);

        // ActorBase
		vm->RegisterFunction("GetWarpaintColors", PaperSKSEFunctions, GetWarpaintColors, false);

        // Helper functions for filtering arguments of Inventory Events
        vm->RegisterFunction("GetInventoryEventFilterIndices", PaperSKSEFunctions, GetInventoryEventFilterIndices,
                             false);
        vm->RegisterFunction("UpdateInventoryEventFilterIndices", PaperSKSEFunctions, UpdateInventoryEventFilterIndices,
                             false);
        vm->RegisterFunction("ApplyInventoryEventFilterToForms", PaperSKSEFunctions, ApplyInventoryEventFilterToForms,
                             false);
        vm->RegisterFunction("ApplyInventoryEventFilterToInts", PaperSKSEFunctions, ApplyInventoryEventFilterToInts,
                             false);
        vm->RegisterFunction("ApplyInventoryEventFilterToObjs", PaperSKSEFunctions, ApplyInventoryEventFilterToObjs,
                             false);

        // Other
        vm->RegisterFunction("GetPaperVersion", PaperSKSEFunctions, GetPaperVersion, true);

        return true;
    }

}