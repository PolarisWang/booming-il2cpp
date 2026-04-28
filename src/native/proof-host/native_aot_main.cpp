#include <cstdint>
#include <cstring>
#include <chaos/native_types.h>

#ifndef CHAOS_NATIVE_AOT_ENTRY
#define CHAOS_NATIVE_AOT_ENTRY RunNativeAot
#endif

extern "C" int CHAOS_NATIVE_AOT_ENTRY(CHAOS_IL2CPP_INT32 entryIndex);

namespace {

constexpr char kEntryIndexPrefix[] = "--entry-index=";
constexpr char kCollectionPathPrefix[] = "--collection-path=";

CHAOS_IL2CPP_INT32 ParseEntryIndex(int argc, char** argv) {
    for (int index = 1; index < argc; ++index) {
        const char* argument = argv[index];
        if (argument == nullptr) {
            continue;
        }

        if (CHAOS_IL2CPP_STRNCMP(argument, kEntryIndexPrefix, sizeof(kEntryIndexPrefix) - 1) == 0) {
            return static_cast<CHAOS_IL2CPP_INT32>(CHAOS_IL2CPP_ATOI(argument + sizeof(kEntryIndexPrefix) - 1));
        }

        if (CHAOS_IL2CPP_STRNCMP(argument, kCollectionPathPrefix, sizeof(kCollectionPathPrefix) - 1) == 0) {
            continue;
        }
    }

    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    return CHAOS_NATIVE_AOT_ENTRY(ParseEntryIndex(argc, argv));
}
