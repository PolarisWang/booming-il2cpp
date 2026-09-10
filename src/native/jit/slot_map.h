// slot_map.h — Reverse mapping from callee token → (JitMethod*, slot_index)
//
// Enables hotpatch-safe JIT call-site updates.  When a method is hotpatched,
// all JIT-compiled callers that called it need their slot pointers updated
// (for non-inlined calls) or their JitMethod invalidated (for inlined calls).
//
// Thread safety: all access happens under the global HotpatchNameRegistry lock.
// The singleton instance g_reverse_slot_map is defined in jit_engine.cpp.

#ifndef CHAOS_IL2CPP_JIT_SLOT_MAP_H_
#define CHAOS_IL2CPP_JIT_SLOT_MAP_H_

#include <cstdint>
#include <vector>

#include <chaos/log.h>
#include <chaos/pal/pal_mem.h>
#include <chaos/unordered_dense.h>

namespace chaos::il2cpp::jit {

struct JitMethod;

/// A single call-site entry in the reverse map.
/// Records that `caller` has a call slot at `slot_index` targeting `callee_token`.
struct SlotEntry {
    JitMethod* caller;     // JitMethod containing the call slot
    uint32_t slot_index;   // index into caller->call_site_slots[]
    uint32_t callee_token; // metadata token of the callee (for verification)
};

/// Reverse mapping from callee metadata token to all JIT callers.
/// When a method is hotpatched:
///   1. For non-inlined calls: updates caller->call_site_slots[slot_index] to new direct_ptr
///   2. For inlined calls: caller invalidation is handled separately (version check in JitStubDispatchImpl)
class ReverseSlotMap {
public:
    /// Register that `caller` has a call slot for `callee_token`.
    void Add(uint32_t callee_token, JitMethod* caller, uint32_t slot_index) noexcept {
        map_[callee_token].push_back({caller, slot_index, callee_token});
    }

    /// Remove all entries for a given caller (called when JitMethod is freed).
    void RemoveAll(JitMethod* caller) noexcept {
        for (auto it = map_.begin(); it != map_.end();) {
            auto& entries = it->second;
            entries.erase(std::remove_if(entries.begin(), entries.end(),
                                         [caller](const SlotEntry& e) {
                                             return e.caller == caller;
                                         }),
                          entries.end());
            if (entries.empty())
                it = map_.erase(it);
            else
                ++it;
        }
    }

    /// Update all slot pointers for `callee_token` to `new_direct_ptr`.
    /// Called from the hotpatch callback (under HotpatchNameRegistry lock).
    /// Slot tables are in RX code pages — uses PalVirtualProtect to temporarily
    /// enable writes. Protected region is per-page, so multiple slots on the
    /// same page are handled in one call.
    void UpdateAll(uint32_t callee_token, void* new_direct_ptr) noexcept {
        auto it = map_.find(callee_token);
        if (it == map_.end())
            return;
        for (auto& entry : it->second) {
            if (entry.caller->call_site_slots && entry.slot_index < entry.caller->call_site_slot_count) {
                void* addr = &entry.caller->call_site_slots[entry.slot_index];
                chaos::il2cpp::pal::PalVirtualProtect(addr, sizeof(void*), chaos::il2cpp::pal::kPalMemReadWrite);
                entry.caller->call_site_slots[entry.slot_index] = new_direct_ptr;
                if (!chaos::il2cpp::pal::PalVirtualProtect(addr, sizeof(void*), chaos::il2cpp::pal::kPalMemReadExec)) {
                    CHAOS_IL2CPP_LOG_ERROR_M("jit", "slot_map: failed to re-protect call slot to RX");
                }
            }
        }
    }

    /// Get all entries for a callee (used to find methods to invalidate for inlining).
    /// Returns nullptr if no entries exist.
    const std::vector<SlotEntry>* Lookup(uint32_t callee_token) const noexcept {
        auto it = map_.find(callee_token);
        return it != map_.end() ? &it->second : nullptr;
    }

private:
    using Map = ankerl::unordered_dense::map<uint32_t, std::vector<SlotEntry>>;
    Map map_;
};

/// Global singleton — defined in jit_engine.cpp.
extern ReverseSlotMap g_reverse_slot_map;

} // namespace chaos::il2cpp::jit

#endif // CHAOS_IL2CPP_JIT_SLOT_MAP_H_
