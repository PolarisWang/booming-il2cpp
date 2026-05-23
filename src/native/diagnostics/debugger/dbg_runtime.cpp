// dbg_runtime.cpp — Runtime hook implementation
//
// Called when DbgShouldPause returns true.  Sends the "stopped" event
// to the DAP client and blocks until the client sends "continue".

#include "dbg_runtime.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include "dbg_server.h"
#include "dbg_protocol.h"

// Runtime-core metadata APIs for method resolution.
#include <module_registry.h>
#include <metadata_interface.h>
#include <reflection_query_model.h>
#include <chaos/type_info.h>

#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <vector>

namespace chaos::il2cpp::diagnostics {

// Global flag: are any breakpoints set?
std::atomic<bool> g_dbg_any_breakpoints{false};

// Global flag: has the DAP client requested a pause?
std::atomic<bool> g_dbg_pause_requested{false};

namespace {

// Synchronization for DbgNotifyPaused / DbgSignalResume.
// Uses a condition variable instead of spin-wait so the interpreter
// thread sleeps while the debugger processes DAP requests.
std::mutex g_dbg_resume_mutex;
std::condition_variable g_dbg_resume_cv;
bool g_dbg_resume_flag = false;

// Frame snapshot — populated by runtime code when breakpoint is hit.
static DbgFrameSnapshot g_dbg_frame_snapshot;

}  // anonymous namespace

void DbgClearFrameSnapshot() noexcept {
    std::memset(&g_dbg_frame_snapshot, 0, sizeof(g_dbg_frame_snapshot));
}

DbgFrameSnapshot& DbgGetFrameSnapshot() noexcept {
    return g_dbg_frame_snapshot;
}

void DbgNotifyPaused(uint32_t method_token, uint32_t il_offset) noexcept {
    (void)method_token;
    (void)il_offset;

    // Determine the reason: pause request or breakpoint.
    // The pause flag is cleared after reading (exchange) so it's
    // a single-shot signal — the next breakpoint hit won't be
    // misinterpreted as a pause.
    DapStoppedReason reason = DapStoppedReason::Breakpoint;
    if (g_dbg_pause_requested.exchange(false)) {
        reason = DapStoppedReason::Pause;
    }

    // Send "stopped" event to the DAP client.
    DbgServerSendStopped(reason, 1);

    // Wait for "continue" from the debugger client.
    // Uses a condition variable so the interpreter thread sleeps
    // efficiently while the DAP server processes requests.
    std::unique_lock lock(g_dbg_resume_mutex);
    g_dbg_resume_flag = false;
    g_dbg_resume_cv.wait(lock, [] { return g_dbg_resume_flag; });
}

void DbgSignalResume() noexcept {
    {
        std::lock_guard lock(g_dbg_resume_mutex);
        g_dbg_resume_flag = true;
    }
    g_dbg_resume_cv.notify_one();
}

/// Strip directory and extension from a source path to get the filename stem.
/// e.g., "c:/project/Program.cs" → "Program", "Helper" → "Helper"
static void GetFilenameStem(const char* source_path, char* stem, int stem_size) noexcept {
    if (source_path == nullptr || stem_size <= 0) { if (stem_size > 0) stem[0] = '\0'; return; }

    // Find last slash or backslash.
    const char* last_slash = strrchr(source_path, '/');
    const char* last_bs = strrchr(source_path, '\\');
    const char* base = source_path;
    if (last_slash && last_slash > base) base = last_slash + 1;
    if (last_bs && last_bs > base) base = last_bs + 1;

    // Find last dot in the base name (extension separator).
    const char* dot = strrchr(base, '.');
    int len = dot != nullptr
        ? static_cast<int>(dot - base)
        : static_cast<int>(strlen(base));

    if (len >= stem_size) len = stem_size - 1;
    memcpy(stem, base, len);
    stem[len] = '\0';
}

void DbgResolveMethodsForSource(const char* source_path,
                                 std::vector<DbgMethodMatch>& out_matches) noexcept
{
    out_matches.clear();
    if (source_path == nullptr || source_path[0] == '\0') return;

    using namespace chaos::il2cpp::runtime_core;

    // Extract the filename stem (e.g., "Program" from "Program.cs").
    char filename_stem[256];
    GetFilenameStem(source_path, filename_stem, sizeof(filename_stem));
    if (filename_stem[0] == '\0') return;

    uint32_t module_count = GetModuleCount();
    for (uint32_t mi = 0; mi < module_count; ++mi) {
        const auto* mod = GetModuleByIndex(mi);
        if (mod == nullptr || mod->tombstone || mod->image == nullptr) continue;

        // Iterate all types in this module's image descriptor.
        for (uint32_t ti = 0; ti < mod->image->type_count; ++ti) {
            const auto* type_desc = mod->image->types[ti];
            if (type_desc == nullptr || type_desc->methods == nullptr) continue;

            // Match type name to filename stem.
            // Type names are simple (e.g., "Program", "Helper", "MyClass"),
            // not namespace-qualified. Compare against the filename stem.
            bool name_match = false;
            if (type_desc->name_utf8 != nullptr) {
                name_match = (strcmp(type_desc->name_utf8, filename_stem) == 0);
            }
            if (!name_match) continue;

            // Collect all methods from the matching type.
            for (uint32_t meth_i = 0; meth_i < type_desc->method_count; ++meth_i) {
                const auto& method_desc = type_desc->methods[meth_i];
                if (method_desc.metadata_token == 0) continue;

                DbgMethodMatch match;
                match.method_token = method_desc.metadata_token;
                match.method_name = method_desc.name_utf8;
                match.type_token = type_desc->metadata_token;
                match.line_number = 0;  // filled by caller
                out_matches.push_back(match);
            }
        }
    }
}

// ── Object field reading (for variable expansion) ──────────────────────

namespace {

using namespace chaos::il2cpp::runtime_core;

int ObjectHeaderSize(const TypeInfoHot* ti) noexcept {
    return (ti->flags & 0x03) == 0 ? 8 : 16;
}

int FieldSizeFromTypeName(const char* member_type) noexcept {
    if (member_type == nullptr) return 8;
    if (strstr(member_type, "Int32") || strstr(member_type, "Single") ||
        strcmp(member_type, "int") == 0 || strcmp(member_type, "bool") == 0 ||
        strcmp(member_type, "System.Boolean") == 0) return 4;
    if (strstr(member_type, "Int64") || strstr(member_type, "Double") ||
        strcmp(member_type, "long") == 0 || strcmp(member_type, "double") == 0) return 8;
    if (strstr(member_type, "Int16") || strcmp(member_type, "short") == 0) return 2;
    if (strstr(member_type, "Byte") || strcmp(member_type, "byte") == 0 ||
        strstr(member_type, "SByte") || strcmp(member_type, "sbyte") == 0) return 1;
    return 8;  // default: pointer-sized
}

bool IsObjectRefType(const char* member_type) noexcept {
    if (member_type == nullptr) return true;
    if (strstr(member_type, "Int32") || strstr(member_type, "Int64") ||
        strstr(member_type, "Single") || strstr(member_type, "Double") ||
        strstr(member_type, "Int16") || strstr(member_type, "Byte") ||
        strstr(member_type, "SByte") || strstr(member_type, "Boolean") ||
        strcmp(member_type, "int") == 0 || strcmp(member_type, "long") == 0 ||
        strcmp(member_type, "float") == 0 || strcmp(member_type, "double") == 0 ||
        strcmp(member_type, "bool") == 0 || strcmp(member_type, "short") == 0 ||
        strcmp(member_type, "byte") == 0 || strcmp(member_type, "sbyte") == 0)
        return false;
    return true;
}

void FormatFieldBytes(const uint8_t* field_ptr, int field_size,
                      bool is_ref, char* buf, int buf_size) noexcept
{
    if (is_ref) {
        uint64_t addr;
        memcpy(&addr, field_ptr, sizeof(addr));
        if (addr == 0) { snprintf(buf, buf_size, "null"); return; }
        snprintf(buf, buf_size, "0x%p", reinterpret_cast<void*>(addr));
        return;
    }
    switch (field_size) {
    case 1: { int8_t v; memcpy(&v, field_ptr, 1); snprintf(buf, buf_size, "%d", v); break; }
    case 2: { int16_t v; memcpy(&v, field_ptr, 2); snprintf(buf, buf_size, "%d", v); break; }
    case 4: { int32_t v; memcpy(&v, field_ptr, 4); snprintf(buf, buf_size, "%d", v); break; }
    case 8: { uint64_t v; memcpy(&v, field_ptr, 8); snprintf(buf, buf_size, "0x%llx", (unsigned long long)v); break; }
    default: snprintf(buf, buf_size, "<%d bytes>", field_size); break;
    }
}

void ReadTypeFields(uint64_t obj_addr,
                     const ReflectionQueryTypeDescriptor* type_desc,
                     std::vector<DbgObjectField>& out_fields,
                     int depth) noexcept
{
    if (obj_addr == 0 || type_desc == nullptr || type_desc->fields == nullptr || depth <= 0)
        return;

    const uint8_t* obj_bytes = reinterpret_cast<const uint8_t*>(obj_addr);
    const auto* type_info = *reinterpret_cast<const TypeInfoHot* const*>(obj_addr);
    if (type_info == nullptr) return;

    int offset = ObjectHeaderSize(type_info);
    for (uint32_t fi = 0; fi < type_desc->field_count; ++fi) {
        const auto& field = type_desc->fields[fi];
        if (field.name_utf8 == nullptr) continue;

        int fsize = FieldSizeFromTypeName(field.member_type_utf8);
        bool is_ref = IsObjectRefType(field.member_type_utf8);

        DbgObjectField df;
        df.name = field.name_utf8;
        df.type_name = field.member_type_utf8 ? field.member_type_utf8 : "unknown";

        char val_buf[64];
        FormatFieldBytes(obj_bytes + offset, fsize, is_ref, val_buf, sizeof(val_buf));
        df.value_str = val_buf;

        df.child_object_addr = 0;
        if (is_ref && depth > 1) {
            uint64_t child_addr;
            memcpy(&child_addr, obj_bytes + offset, sizeof(child_addr));
            if (child_addr != 0) {
                df.child_object_addr = child_addr;
            }
        }

        out_fields.push_back(df);
        offset += fsize;
    }
}

}  // anonymous namespace

std::vector<DbgObjectField> DbgReadObjectFields(uint64_t object_addr,
                                                  int max_depth) noexcept
{
    std::vector<DbgObjectField> fields;
    if (object_addr == 0 || max_depth <= 0) return fields;

    using namespace chaos::il2cpp::runtime_core;

    const auto* type_info = *reinterpret_cast<const TypeInfoHot* const*>(
        static_cast<uintptr_t>(object_addr));
    if (type_info == nullptr) return fields;

    // Find the ReflectionQueryTypeDescriptor matching this TypeInfoHot*.
    uint32_t mod_count = GetModuleCount();
    for (uint32_t mi = 0; mi < mod_count; ++mi) {
        const auto* mod = GetModuleByIndex(mi);
        if (mod == nullptr || mod->tombstone || mod->image == nullptr ||
            mod->type_info_ptrs == nullptr) continue;

        for (uint32_t ti = 0; ti < mod->type_count; ++ti) {
            if (mod->type_info_ptrs[ti] != type_info) continue;

            // Found matching type — read fields, then walk base types.
            int depth = max_depth;
            const auto* current_desc = mod->image->types[ti];
            const auto* current_ti = type_info;

            while (current_desc != nullptr && depth > 0) {
                ReadTypeFields(object_addr, current_desc, fields, depth);

                // Walk to parent type.
                if (current_ti->parent == nullptr) break;
                current_ti = current_ti->parent;

                // Find parent's ReflectionQueryTypeDescriptor.
                current_desc = nullptr;
                for (uint32_t mi2 = 0; mi2 < mod_count; ++mi2) {
                    const auto* mod2 = GetModuleByIndex(mi2);
                    if (mod2 == nullptr || mod2->tombstone || mod2->image == nullptr ||
                        mod2->type_info_ptrs == nullptr) continue;
                    for (uint32_t ti2 = 0; ti2 < mod2->type_count; ++ti2) {
                        if (mod2->type_info_ptrs[ti2] == current_ti) {
                            current_desc = mod2->image->types[ti2];
                            break;
                        }
                    }
                    if (current_desc != nullptr) break;
                }
                --depth;
            }
            return fields;
        }
    }

    return fields;
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER
