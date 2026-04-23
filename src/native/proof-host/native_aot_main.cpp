#include <cstdint>
#include <cstdlib>
#include <cstring>

#ifndef CHAOS_NATIVE_AOT_ENTRY
#define CHAOS_NATIVE_AOT_ENTRY RunNativeAot
#endif

extern "C" int CHAOS_NATIVE_AOT_ENTRY(std::int32_t entryIndex);

namespace {

constexpr char kEntryIndexPrefix[] = "--entry-index=";
constexpr char kCollectionPathPrefix[] = "--collection-path=";

std::int32_t ParseEntryIndex(int argc, char** argv) {
    for (int index = 1; index < argc; ++index) {
        const char* argument = argv[index];
        if (argument == nullptr) {
            continue;
        }

        if (std::strncmp(argument, kEntryIndexPrefix, sizeof(kEntryIndexPrefix) - 1) == 0) {
            return static_cast<std::int32_t>(std::atoi(argument + sizeof(kEntryIndexPrefix) - 1));
        }

        if (std::strncmp(argument, kCollectionPathPrefix, sizeof(kCollectionPathPrefix) - 1) == 0) {
            continue;
        }
    }

    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    return CHAOS_NATIVE_AOT_ENTRY(ParseEntryIndex(argc, argv));
}
