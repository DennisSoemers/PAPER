#include "Papyrus.h"
#include "ResourceUtils.h"

namespace PAPER {

	constexpr std::string_view PaperUtils = "PAPER_SKSEFunctions";

	/**
	 * Will return true if and only if a file for the given path exists
	 * (may be either as a loose file or in a BSA).
	 */
    bool ResourceExists(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*,
                        std::string resourcePath) { 
        return ResourceUtils::ResourceExists(resourcePath);
	}

	/**
	 * Provide bindings for all our Papyrus functions.
	 */
	bool Bind(RE::BSScript::IVirtualMachine* vm) {
        vm->RegisterFunction("ResourceExists", PaperUtils, ResourceExists, true);

        return true;
    }

}