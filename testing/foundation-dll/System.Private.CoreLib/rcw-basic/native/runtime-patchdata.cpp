// Sentinel: no .patchdata available (hotpatch dispatch disabled)
#include <cstddef>
#include <cstdint>

extern const char* const kPatchDataHostClassName;
extern const char* const kPatchDataHostClassName = "";

extern const uint8_t kPatchData[];
extern const uint8_t kPatchData[] = { 0x00 };

extern const size_t kPatchDataSize;
extern const size_t kPatchDataSize = 0u;
