#pragma once

#include <RE/Skyrim.h>

namespace ResourceUtils {
#pragma warning(push)
#pragma warning(disable : 4251)

    inline bool ResourceExists(const std::string& resourcePath) {
        return RE::BSResourceNiBinaryStream(resourcePath).good();
    }

    inline bool ResourceExists(const char* resourcePath) {
        return RE::BSResourceNiBinaryStream(resourcePath).good();
    }

#pragma warning(pop)
}  // namespace ResourceUtils