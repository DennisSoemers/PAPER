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
	 * Returns the number of Face Tint Layers that the given ActorBase has.
	 * Returns 0 if the given ActorBase is None.
	 */
	int GetNumTintLayers(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*,
		RE::TESNPC* actorBase) {

		if (!actorBase) {
            a_vm->TraceStack("ActorBase is None", a_stackID);
            return 0;
		}

		return actorBase->tintLayers->size();
	}

	/**
	 * Provide bindings for all our Papyrus functions.
	 */
	bool Bind(RE::BSScript::IVirtualMachine* vm) {
        vm->RegisterFunction("GetPaperVersion", PaperSKSEFunctions, GetPaperVersion, true);
        vm->RegisterFunction("ResourceExists", PaperSKSEFunctions, ResourceExists, true);

		vm->RegisterFunction("GetNumTintLayers", PaperSKSEFunctions, GetNumTintLayers, true);

        return true;
    }

}