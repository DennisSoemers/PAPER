#pragma once

#include <RE/Skyrim.h>

namespace PAPER {
#pragma warning(push)
#pragma warning(disable : 4251)

	/** Bind all the new Papyrus functions */
	bool Bind(RE::BSScript::IVirtualMachine* vm);

#pragma warning(pop)
}  // namespace PAPER