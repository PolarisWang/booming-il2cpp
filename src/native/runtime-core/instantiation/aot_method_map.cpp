namespace chaos::il2cpp::runtime_instantiation {
namespace {

struct PerModuleAotMap {
    const GenericMethodAotEntryV0* entries     = nullptr;
    CHAOS_IL2CPP_UINT32            entry_count = 0u;
    const uint32_t*                args        = nullptr;
    CHAOS_IL2CPP_UINT32            arg_count   = 0u;
};

// Per-module AOT maps indexed by module_id (fixed-size array, O(1) lookup).
PerModuleAotMap s_module_aot_maps[runtime_core::kMaxModules];

}  // anonymous namespace (AOT map)

void RegisterMethodAotEntries(
    CHAOS_IL2CPP_UINT32                module_id,
    const GenericMethodAotEntryV0*     entries,
    CHAOS_IL2CPP_UINT32                count,
    const uint32_t*                    args,
    CHAOS_IL2CPP_UINT32                arg_count)
{
    if (module_id >= runtime_core::kMaxModules) return;
    s_module_aot_maps[module_id].entries     = entries;
    s_module_aot_maps[module_id].entry_count = count;
    s_module_aot_maps[module_id].args        = args;
    s_module_aot_maps[module_id].arg_count   = arg_count;
}

MethodInfoHandle QueryAotMethod(
    CHAOS_IL2CPP_UINT32                module_id,
    CHAOS_IL2CPP_UINT32                open_method_token,
    const TypeInfoHandle*              type_args,
    CHAOS_IL2CPP_UINT32                arg_count)
{
    if (module_id >= runtime_core::kMaxModules) return 0u;

    const auto& mod = s_module_aot_maps[module_id];
    if (mod.entry_count == 0u) return 0u;

    // Binary search by open_token.
    CHAOS_IL2CPP_INT32 lo = 0;
    CHAOS_IL2CPP_INT32 hi = static_cast<CHAOS_IL2CPP_INT32>(mod.entry_count) - 1;

    while (lo <= hi) {
        const CHAOS_IL2CPP_INT32 mid = lo + (hi - lo) / 2;
        const uint32_t mid_token = mod.entries[mid].open_token;

        if (mid_token < open_method_token) {
            lo = mid + 1;
        } else if (mid_token > open_method_token) {
            hi = mid - 1;
        } else {
            /* Found matching open_token — scan type_args.
             * Multiple entries may share the same open_token (different type
             * arguments), so find the contiguous range first. */
            CHAOS_IL2CPP_INT32 range_start = mid;
            while (range_start > 0 &&
                   mod.entries[range_start - 1].open_token == open_method_token) {
                --range_start;
            }
            CHAOS_IL2CPP_UINT32 range_end = static_cast<CHAOS_IL2CPP_UINT32>(mid);
            while (range_end + 1u < mod.entry_count &&
                   mod.entries[range_end + 1u].open_token == open_method_token) {
                ++range_end;
            }

            // Scan the range for a type-arg match.
            for (CHAOS_IL2CPP_UINT32 ei = static_cast<CHAOS_IL2CPP_UINT32>(range_start);
                 ei <= range_end; ++ei) {
                const auto& entry = mod.entries[ei];
                if (entry.arg_count != arg_count) continue;

                bool match = true;
                for (CHAOS_IL2CPP_UINT32 ai = 0u; ai < arg_count; ++ai) {
                    const uint32_t arg_token   = runtime_core::GetTypeToken(type_args[ai]);
                    const uint32_t entry_token = mod.args[entry.args_start_index + ai];
                    if (arg_token != entry_token) {
                        match = false;
                        break;
                    }
                }

                if (match) {
                    // Return the closed token as a simple token-based
                    // MethodInfoHandle (module_id=0 for AOT root).
                    return static_cast<MethodInfoHandle>(
                        (static_cast<CHAOS_IL2CPP_UINT64>(0u) << 32) |
                        entry.closed_token);
                }
            }

            return 0u;  // open_token matched but type args did not
        }
    }

    return 0u;  // open_token not found in this module
}

}  // namespace chaos::il2cpp::runtime_instantiation

extern "C" CHAOS_RUNTIME_ABI_EXPORT const RuntimeInstantiationBridgeV0*
CHAOS_RUNTIME_ABI_CALL chaos_runtime_get_instantiation_bridge_v0(void) {
    return chaos::il2cpp::runtime_instantiation::GetBridgeV0();
}