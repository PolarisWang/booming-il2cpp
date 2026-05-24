/// jit_sos.cpp — JIT SOS Debugger Extension for WinDbg
///
/// Commands are invoked as !jit.<command> in WinDbg after .load chaos_gc_sos.
/// Uses the same DbgEng infrastructure as gc_sos.cpp (shared via sos_common.h).
///
/// Architecture:
///   - Resolves extern "C" debug contract symbols (g_chaos_jit_debug_entries_ptr
///     etc.) to locate the JIT debug mirror in the target process.
///   - Reads JitDebugEntry and JitDebugInfo POD structs from target memory.
///   - For method name resolution: reads the method name string embedded in the
///     T4 code buffer (at code_start + method_name_off from JitDebugInfo).
///
/// Commands:
///   !jit.help       — Show available commands
///   !jit.info       — JIT subsystem state (entry count, pointers)
///   !jit.methods    — Enumerate all T4 JIT methods (address, size, name)
///   !jit.find <addr> — Find method by code address

#include "sos_common.h"

#include <cstdint>
#include <cstring>

// ══════════════════════════════════════════════════════════════════════════
// POD mirror of JitDebugEntry (jit_debug_contract.cpp)
//
// Layout must match the runtime definition exactly for cross-process reading.
// ══════════════════════════════════════════════════════════════════════════

#pragma pack(push, 8)
struct JitDebugEntryPod {
    ULONG64  code_start;           // void* (8 bytes)
    uint32_t code_size;            // uint32_t (4 bytes)
    uint32_t patch_method_token;   // uint32_t (4 bytes)
    ULONG64  nm;                   // const void* — JitMethod* in target (8 bytes)
};
#pragma pack(pop)

// ══════════════════════════════════════════════════════════════════════════
// POD mirror of JitDebugInfo (jit_method.h)
// ══════════════════════════════════════════════════════════════════════════

#pragma pack(push, 4)
struct JitDebugInfoPod {
    uint32_t magic;                // JitDebugInfo::kMagic = 0x544A4442
    uint32_t version;              // JitDebugInfo::kVersion = 2
    uint32_t code_size;
    uint32_t instr_offset_count;
    uint32_t instr_offsets_off;
    uint32_t method_name_off;
    uint32_t method_name_len;
};
#pragma pack(pop)

// ══════════════════════════════════════════════════════════════════════════
// Constants matching the JIT runtime
// ══════════════════════════════════════════════════════════════════════════

static constexpr uint32_t kJitDebugMagic   = 0x544A4442u;  // "BJDT"
static constexpr uint32_t kJitDebugVersion = 2u;

// ══════════════════════════════════════════════════════════════════════════
// Command: !jit.help
// ══════════════════════════════════════════════════════════════════════════

extern "C" HRESULT CALLBACK
jit_help(PDEBUG_CLIENT, PCSTR) {
    SosPrint("\n"
             "JIT SOS Debugger Extension\n"
             "══════════════════════════\n"
             "Commands:\n"
             "  !jit.help       - Show this help\n"
             "  !jit.info       - JIT subsystem state (entries/capacity)\n"
             "  !jit.methods    - Enumerate all T4 JIT methods\n"
             "  !jit.find <addr> - Find method by code address (hex)\n"
             "\n");
    return S_OK;
}

// ══════════════════════════════════════════════════════════════════════════
// Command: !jit.info — JIT subsystem state
// ══════════════════════════════════════════════════════════════════════════

extern "C" HRESULT CALLBACK
jit_info(PDEBUG_CLIENT, PCSTR) {
    ULONG64 entries_ptr  = SosResolvePointer("g_chaos_jit_debug_entries_ptr");
    ULONG64 count_ptr    = SosResolvePointer("g_chaos_jit_debug_entry_count_ptr");
    ULONG64 max_ptr      = 0;
    ULONG64 meta_ptr     = SosResolvePointer("g_chaos_metadata_registry_ptr");

    if (entries_ptr == 0) {
        SosPrint("ERROR: Cannot locate g_chaos_jit_debug_entries_ptr.\n"
                 "Ensure the target was built with jit_debug_contract enabled.\n");
        return E_FAIL;
    }

    // Read max entries if available.
    int32_t max_entries = 2048;
    if (SUCCEEDED(g_sos_symbols->GetOffsetByName(
            "g_chaos_jit_debug_max_entries", &max_ptr))) {
        SosReadTarget(max_ptr, &max_entries, sizeof(max_entries));
    }

    // Read entry count.
    int32_t entry_count = 0;
    if (count_ptr == 0 ||
        !SosReadTarget(count_ptr, &entry_count, sizeof(entry_count))) {
        SosPrint("WARNING: Cannot read entry count, assuming 0.\n");
    }

    SosPrint("\n"
             "JIT Subsystem State (Debug Mirror)\n"
             "══════════════════════════════════\n"
             "  Debug Entries Array:  0x%llx\n"
             "  Entry Count:          %d\n"
             "  Max Entries:          %d\n"
             "  Metadata Registry:    0x%llx  %s\n"
             "\n",
             entries_ptr,
             entry_count,
             max_entries,
             meta_ptr,
             meta_ptr ? "resolved" : "(null)");

    return S_OK;
}

// ══════════════════════════════════════════════════════════════════════════
// Command: !jit.methods — Enumerate all T4 JIT methods
// ══════════════════════════════════════════════════════════════════════════

extern "C" HRESULT CALLBACK
jit_methods(PDEBUG_CLIENT, PCSTR) {
    ULONG64 entries_ptr = SosResolvePointer("g_chaos_jit_debug_entries_ptr");
    if (entries_ptr == 0) {
        SosPrint("ERROR: Cannot locate jit_debug_entries.\n");
        return E_FAIL;
    }

    // Read entry count.
    int32_t entry_count = 0;
    ULONG64 count_ptr = SosResolvePointer("g_chaos_jit_debug_entry_count_ptr");
    if (count_ptr == 0 ||
        !SosReadTarget(count_ptr, &entry_count, sizeof(entry_count))) {
        SosPrint("ERROR: Cannot read entry count.\n");
        return E_FAIL;
    }

    if (entry_count <= 0) {
        SosPrint("No T4 JIT methods registered.\n");
        return S_OK;
    }

    SosPrint("\n"
             "T4 JIT Methods (%d entries)\n"
             "══════════════════════════\n"
             "  #  | Code Start        | Code Size | Token     | Method Name\n"
             "  ---+-------------------+-----------+-----------+----------------\n",
             entry_count);

    int printed = 0;
    for (int32_t i = 0; i < entry_count; i++) {
        // Read one JitDebugEntryPod from the target.
        JitDebugEntryPod entry;
        ULONG64 entry_addr = entries_ptr + static_cast<ULONG64>(i) * sizeof(entry);
        if (!SosReadTarget(entry_addr, &entry, sizeof(entry))) {
            SosPrint("  %2d | ERROR: Cannot read entry at 0x%llx\n", i, entry_addr);
            continue;
        }

        // Read method name from the code buffer (if debug_info is present)
        char method_name[256] = "<no debug info>";
        if (entry.code_start != 0 && entry.nm != 0) {
            // Read the JitMethod to get debug_info_offset.
            // JitMethod: code(8) + code_size(4) + instr_count(4) +
            //            call_sites(8) + call_site_count(4) = 28 bytes before
            //            deopt_entries.
            // debug_info_offset is at offset: 8+4+4+8+4+8+4+8+4+8+4+4+4+8+8+4
            // This is fragile. Instead, scan backwards from the end of the
            // code buffer for the JitDebugInfo magic.

            // Read the last few bytes of the code buffer to find JitDebugInfo.
            // The JitDebugInfo header (28 bytes) is appended at the end.
            // code_size from the entry matches the total buffer size.
            uint32_t cb_size = entry.code_size;

            // JitDebugInfo header is at code_start + code_size - sizeof(JitDebugInfoPod)
            // (it's the last thing emitted before Seal).
            ULONG64 di_addr = entry.code_start;
            if (cb_size >= sizeof(JitDebugInfoPod)) {
                di_addr += (cb_size - sizeof(JitDebugInfoPod));
            }

            JitDebugInfoPod di;
            if (SosReadTarget(di_addr, &di, sizeof(di)) &&
                di.magic == kJitDebugMagic &&
                di.version == kJitDebugVersion &&
                di.method_name_off != 0 &&
                di.method_name_len > 0 &&
                di.method_name_len < sizeof(method_name)) {

                ULONG64 name_addr = entry.code_start + di.method_name_off;
                ULONG read_len = di.method_name_len;
                if (read_len > sizeof(method_name) - 1) {
                    read_len = sizeof(method_name) - 1;
                }
                if (SosReadTarget(name_addr, method_name, read_len)) {
                    method_name[read_len] = '\0';
                } else {
                    std::strcpy(method_name, "<read error>");
                }
            } else if (di.magic != kJitDebugMagic) {
                std::strcpy(method_name, "<no JitDebugInfo>");
            }
        }

        SosPrint("  %2d | 0x%016llx | %9u | 0x%08x | %s\n",
                 i,
                 entry.code_start,
                 entry.code_size,
                 entry.patch_method_token,
                 method_name);
        printed++;
    }

    if (printed == 0) {
        SosPrint("  (no valid entries)\n");
    }
    SosPrint("\n");

    return S_OK;
}

// ══════════════════════════════════════════════════════════════════════════
// Command: !jit.find <addr> — Find method by code address
// ══════════════════════════════════════════════════════════════════════════

extern "C" HRESULT CALLBACK
jit_find(PDEBUG_CLIENT, PCSTR args) {
    if (args == nullptr || *args == '\0') {
        SosPrint("Usage: !jit.find <hex_address>\n"
                 "  e.g. !jit.find 7ff6a1b2c3d0\n");
        return E_FAIL;
    }

    // Parse the hex address argument.
    ULONG64 target_addr = 0;
    // Skip whitespace and optional 0x prefix.
    const char* p = args;
    while (*p == ' ' || *p == '\t') p++;
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) p += 2;

    char* end = nullptr;
    target_addr = strtoull(p, &end, 16);
    if (end == p || target_addr == 0) {
        SosPrint("ERROR: Cannot parse address from '%s'.\n", args);
        return E_FAIL;
    }

    // Read entry count.
    int32_t entry_count = 0;
    ULONG64 count_ptr = SosResolvePointer("g_chaos_jit_debug_entry_count_ptr");
    if (count_ptr == 0 ||
        !SosReadTarget(count_ptr, &entry_count, sizeof(entry_count))) {
        SosPrint("ERROR: Cannot read entry count.\n");
        return E_FAIL;
    }

    ULONG64 entries_ptr = SosResolvePointer("g_chaos_jit_debug_entries_ptr");
    if (entries_ptr == 0) {
        SosPrint("ERROR: Cannot locate jit_debug_entries.\n");
        return E_FAIL;
    }

    SosPrint("\n"
             "Searching for address 0x%llx in T4 JIT methods (%d entries)\n"
             "═══════════════════════════════════════════════════════════\n",
             target_addr, entry_count);

    int found = 0;
    for (int32_t i = 0; i < entry_count; i++) {
        JitDebugEntryPod entry;
        ULONG64 entry_addr = entries_ptr + static_cast<ULONG64>(i) * sizeof(entry);
        if (!SosReadTarget(entry_addr, &entry, sizeof(entry))) {
            continue;
        }

        if (entry.code_start == 0) continue;

        ULONG64 start = entry.code_start;
        ULONG64 end   = start + entry.code_size;

        if (target_addr >= start && target_addr < end) {
            // Read method name.
            char method_name[256] = "<no debug info>";
            if (entry.code_size >= sizeof(JitDebugInfoPod)) {
                ULONG64 di_addr = start + (entry.code_size - sizeof(JitDebugInfoPod));
                JitDebugInfoPod di;
                if (SosReadTarget(di_addr, &di, sizeof(di)) &&
                    di.magic == kJitDebugMagic &&
                    di.method_name_off != 0 &&
                    di.method_name_len > 0) {

                    ULONG read_len = di.method_name_len;
                    if (read_len > sizeof(method_name) - 1) {
                        read_len = sizeof(method_name) - 1;
                    }
                    ULONG64 name_addr = start + di.method_name_off;
                    if (SosReadTarget(name_addr, method_name, read_len)) {
                        method_name[read_len] = '\0';
                    }
                }
            }

            uint32_t offset = static_cast<uint32_t>(target_addr - start);
            SosPrint("  Entry %d: 0x%016llx - 0x%016llx (%u bytes)\n"
                     "    Token:      0x%08x\n"
                     "    Method:     %s\n"
                     "    Offset:     0x%x (+%d from entry)\n",
                     i, start, end, entry.code_size,
                     entry.patch_method_token,
                     method_name,
                     offset, offset);
            found++;
        }
    }

    if (found == 0) {
        SosPrint("  Address 0x%llx not found in any T4 JIT method.\n",
                 target_addr);
    }
    SosPrint("\n");

    return S_OK;
}

// ══════════════════════════════════════════════════════════════════════════
// DllMain — required for all Win32 DLLs
// ══════════════════════════════════════════════════════════════════════════
// Note: DebugExtensionInitialize is defined in gc_sos.cpp; that single
// initialization serves both GC and JIT commands in chaos_gc_sos.dll.
