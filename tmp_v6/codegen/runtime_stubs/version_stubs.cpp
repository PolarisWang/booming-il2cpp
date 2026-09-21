// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// version_stubs.cpp — System.Version.Parse(string) implementation.
//
// Parses strings of the form "M.m", "M.m.b", "M.m.b.r" (major.minor,
// major.minor.build, major.minor.build.revision).  Components after
// the parsed portion are left at 0, matching the default-initialized
// behaviour of a Version constructed from fewer components.
#include <chaos/native_types.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cstdlib>

#include "generated_code_compat.h"
#include "gc_helpers.h"
#include "string_table.h"
#include "exception_helpers.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Decode a string subject value to a const char* ───────────────────
// Reused from parse_convert.cpp (same pattern).
static bool DecodeVersionString(CHAOS_IL2CPP_INTPTR value, const char*& out_data,
                                CHAOS_IL2CPP_INT32& out_len) noexcept
{
    out_data = nullptr;
    out_len = 0;
    if (value == 0) return false;

    if (chaos_is_string_id(value))
    {
        const auto view = string_table::Resolve(chaos_extract_string_id(value));
        out_data = view.utf8_data;
        out_len = static_cast<CHAOS_IL2CPP_INT32>(view.byte_count);
        return out_len > 0;
    }

    auto* ms = reinterpret_cast<const CHAOS_IL2CPP_STRING_TYPE*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(value));
    out_len = ms->length;
    if (out_len > 0) out_data = ms->utf8_data;
    return true;
}


CHAOS_IL2CPP_INTPTR ChaosVersionParse(CHAOS_IL2CPP_INTPTR value) noexcept
{
    const char* data = nullptr;
    CHAOS_IL2CPP_INT32 len = 0;
    if (!DecodeVersionString(value, data, len) || data == nullptr)
    {
        RaiseManagedException("System.ArgumentNullException",
            "String reference not set to an instance of a String.");
        return 0;
    }

    // Null-terminate a local copy for sscanf.
    char buf[64];
    if (len > static_cast<CHAOS_IL2CPP_INT32>(sizeof(buf) - 1))
        len = static_cast<CHAOS_IL2CPP_INT32>(sizeof(buf) - 1);
    std::memcpy(buf, data, static_cast<std::size_t>(len));
    buf[len] = '\0';

    int major = 0, minor = 0, build = 0, revision = 0;
    int parsed = std::sscanf(buf, "%d.%d.%d.%d", &major, &minor, &build, &revision);

    if (parsed < 2)
    {
        RaiseManagedException("System.FormatException",
            "Input string was not in a correct format. (expected \"M.m\", \"M.m.b\", or \"M.m.b.r\")");
        return 0;
    }

    // Allocate: 16B ThinLockableHeader + 4 × Int32 = 32 bytes.
    auto* instance = static_cast<CHAOS_IL2CPP_UINT8*>(GcAllocateAtomic(32));
    if (instance == nullptr) return 0;

    std::memset(instance, 0, 32);

    // Write fields at known offsets.
    // ThinLockableHeader at 0..15 is already zeroed by GcAllocateAtomic.
    std::memcpy(instance + 16, &major, sizeof(major));
    std::memcpy(instance + 20, &minor, sizeof(minor));
    if (parsed >= 3)
        std::memcpy(instance + 24, &build, sizeof(build));
    if (parsed >= 4)
        std::memcpy(instance + 28, &revision, sizeof(revision));

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(instance);
}

} // extern "C"
} // namespace chaos::il2cpp::runtime_core