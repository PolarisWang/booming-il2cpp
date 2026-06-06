// Auto-generated stubs for patch_loader functions.
// The real definitions are in patch_loader.cpp (part of chaos_runtime_core.lib),
// but the prebuilt SDK lib may not include this file.
// Test environments don't use hotpatch, so stubs are sufficient.
#include <cstdint>
#include <cstddef>
namespace chaos { namespace il2cpp { namespace runtime_core {
struct PatchContext;
PatchContext* ApplyPatchFromMemoryEx(const void*, size_t, const char*,
    const char* const*, const char* const*, int) noexcept { return nullptr; }
bool Unpatch(PatchContext*) noexcept { return true; }
} } }
