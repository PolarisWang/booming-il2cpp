// dbg_variables.cpp — Variable inspection with nested object expansion
//
// Reads local variables from the frame snapshot captured at breakpoint time
// and returns DAP variable values.  Object-typed locals are expandable:
// when the DAP client requests variables for the child reference, field
// values are read from the managed heap via DbgReadObjectFields().

#include "dbg_variables.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include "dbg_protocol.h"
#include "dbg_runtime.h"

#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <vector>

namespace chaos::il2cpp::diagnostics {

namespace {

// ── Variable reference registry ──────────────────────────────────────
// Maps DAP variablesReference IDs to managed object addresses.
// References 1..99 are reserved for scopes; 100+ are object instances.

struct VarRefEntry {
    uint64_t object_addr;   // raw managed object address
};

std::unordered_map<int, VarRefEntry> g_var_refs;
int g_next_var_ref = 100;

}  // anonymous namespace

void DbgVariablesInitialize() noexcept {
    g_var_refs.clear();
    g_next_var_ref = 100;
}

void DbgVariablesShutdown() noexcept {
    g_var_refs.clear();
}

std::vector<DapScope> DbgGetScopes(int frame_id) noexcept {
    std::vector<DapScope> scopes;
    const auto& snap = DbgGetFrameSnapshot();

    if (frame_id >= 0 && frame_id < snap.frame_count) {
        // Locals scope — reference 1 always means "locals of top frame".
        DapScope locals;
        locals.name = "Locals";
        locals.variables_reference = 1;
        locals.expensive = false;
        scopes.push_back(locals);
    }

    return scopes;
}

const char* TagToTypeName(uint8_t tag) noexcept {
    switch (tag) {
        case 0:  return "unknown";
        case 1:  return "int32";
        case 2:  return "int64";
        case 3:  return "float";
        case 4:  return "double";
        case 5:  return "object";
        default: return "unknown";
    }
}

void FormatValue(uint64_t val, uint8_t tag, char* buf, size_t buf_size) noexcept {
    switch (tag) {
        case 1: {  // int32
            int32_t v;
            std::memcpy(&v, &val, sizeof(v));
            std::snprintf(buf, buf_size, "%d", v);
            break;
        }
        case 2: {  // int64
            int64_t v;
            std::memcpy(&v, &val, sizeof(v));
            std::snprintf(buf, buf_size, "%lld", (long long)v);
            break;
        }
        case 3: {  // float
            float v;
            std::memcpy(&v, &val, sizeof(v));
            std::snprintf(buf, buf_size, "%g", v);
            break;
        }
        case 4: {  // double
            double v;
            std::memcpy(&v, &val, sizeof(v));
            std::snprintf(buf, buf_size, "%g", v);
            break;
        }
        case 5: {  // object
            std::snprintf(buf, buf_size, "0x%p", reinterpret_cast<void*>(val));
            break;
        }
        default:
            std::snprintf(buf, buf_size, "0x%llx", (unsigned long long)val);
            break;
    }
}

std::vector<DapVariable> DbgGetVariables(int variables_reference) noexcept {
    std::vector<DapVariable> vars;
    const auto& snap = DbgGetFrameSnapshot();

    if (variables_reference == 1) {
        // Return locals from the captured snapshot.
        // For object-typed locals, assign a child variables_reference
        // so the DAP client can expand them.
        char name_buf[32];
        char val_buf[64];

        for (int i = 0; i < snap.local_count; ++i) {
            DapVariable v;
            std::snprintf(name_buf, sizeof(name_buf), "local_%u", snap.locals[i].index);
            v.name = name_buf;

            if (snap.locals[i].tag == 5 && snap.locals[i].value != 0) {
                // Object reference — show pointer and assign child ref.
                std::snprintf(val_buf, sizeof(val_buf), "object 0x%p",
                             reinterpret_cast<void*>(snap.locals[i].value));
                v.value = val_buf;
                v.type = "object";

                int child_ref = g_next_var_ref++;
                g_var_refs[child_ref] = { snap.locals[i].value };
                v.variables_reference = child_ref;
            } else {
                FormatValue(snap.locals[i].value, snap.locals[i].tag,
                            val_buf, sizeof(val_buf));
                v.value = val_buf;
                v.type = TagToTypeName(snap.locals[i].tag);
            }

            vars.push_back(v);
        }
    } else {
        // Child variable reference: expand object fields.
        auto ref_it = g_var_refs.find(variables_reference);
        if (ref_it != g_var_refs.end()) {
            auto fields = DbgReadObjectFields(ref_it->second.object_addr);
            for (const auto& f : fields) {
                DapVariable v;
                v.name = f.name;
                v.value = f.value_str;
                v.type = f.type_name;

                if (f.child_object_addr != 0) {
                    int child_ref = g_next_var_ref++;
                    g_var_refs[child_ref] = { f.child_object_addr };
                    v.variables_reference = child_ref;
                }

                vars.push_back(v);
            }
        }
    }

    return vars;
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER
