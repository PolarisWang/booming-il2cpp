#include "hotpatch_api.h"
#include "patch_loader.h"
#include "method_replacement.h"

#include <atomic>
#include <mutex>
#include <vector>

namespace chaos {
namespace il2cpp {
namespace runtime_core {

// Host build revision set via ChaosSetHostRevision.  Default 0 = accept any.
// Defined here (external linkage) so patch_loader.cpp version-compat check sees it.
std::atomic<uint32_t> g_host_revision{0};

namespace {

// Next token to hand out for an applied patch.
std::atomic<int32_t> g_next_token{1};
// List of live patch contexts, keyed by token.  These are deleted on Revert.
struct LivePatch {
    int32_t        token = 0;
    uint32_t       host_revision_at_apply = 0;
    PatchContext*  ctx   = nullptr;
    uint64_t       applied_at = 0;
};
// Guards all access to the live-patch container.  Apply/Revert/Status may now be
// called from different threads; a plain mutex is sufficient at this scale
// (Status's read is held only briefly while copying entries out).
std::mutex g_live_mutex;
// Returns the process-wide live-patch container, first-initialized on demand.
// A function-local static avoids both the raw `new` (never freed) and the global
// static-initialization-order problem of a namespace-scope pointer.
std::vector<LivePatch>& GetLivePatches() {
    static std::vector<LivePatch> live;
    return live;
}

PatchContext* ApplyBlob(const void* data, size_t size) {
    // Reject obviously-invalid input before doing anything.
    if (data == nullptr || size == 0) return nullptr;
    // Self-contained apply: we deliberately pass only namespace/metadata that
    // lives inside the .patchdata.  PatchDLL metadata carries type+method names,
    // so no host_name override arrays are required here.
    return ApplyPatchFromMemoryEx(
        data, size,
        nullptr,          // host_type_ns
        nullptr,          // host_type_names
        nullptr,          // host_method_names
        0);               // method_count -> no per-method overrides
}

}  // namespace

}  // namespace runtime_core
}  // namespace il2cpp
}  // namespace chaos

extern "C" {

void ChaosSetHostRevision(uint32_t revision) {
    chaos::il2cpp::runtime_core::g_host_revision.store(revision, std::memory_order_relaxed);
}

int32_t ChaosApplyPatch(const void* data, size_t size) {
    using namespace chaos::il2cpp::runtime_core;

    PatchContext* ctx = nullptr;
    // Minimal header peek for magic + version-compat check before the deep parse.
    // Need at least the magic(4)+version(4)+header_size(4) to validate; the v4
    // trailing min_host_revision/patch_revision are only present when
    // header_size >= 140.  Read defensively: never beyond `size`.
    if (data == nullptr || size < 12u) return CHAOS_PATCH_ERR_INVALID_FORMAT;
    auto* hdr = static_cast<const PatchDataHeader*>(data);
    if (hdr->magic != PATCH_DATA_MAGIC) return CHAOS_PATCH_ERR_INVALID_FORMAT;
    if (hdr->version >= 4u && hdr->header_size >= 140u && size >= 140u) {
        // v4 trailing fields present -> enforce host revision requirement here so we
        // can return the precise VERSION_MISMATCH code (the loader returns nullptr
        // generically on version rejection and cannot be distinguished there).
        uint32_t host_rev = g_host_revision.load(std::memory_order_relaxed);
        if (host_rev > 0u && hdr->min_host_revision > host_rev) {
            return CHAOS_PATCH_ERR_VERSION_MISMATCH;
        }
    }

    // Attempt transactional apply.
    ctx = ApplyBlob(data, size);
    if (ctx == nullptr) {
        // Distinguish "no methods matched" from "bad format/internal":
        // loader returns a ctx with method_count==0 when nothing matched; a null
        // from ApplyBlob means hard failure.  We approximate: if header parsed,
        // but no methods were applied, it is NO_METHODS.  Because ApplyBlob only
        // returns non-null on a real context, and loader sets method_count==0 for
        // a valid blob with no matching methods, we return NO_METHODS when a blob
        // parsed but produced 0.  We cannot distinguish easily post-hoc, so:
        //   1) If size/signature invalid -> INVALID_FORMAT
        //   2) otherwise -> PARTIAL/INTERNAL
        return CHAOS_PATCH_ERR_PARTIAL_ROLLBACK;
    }
    if (ctx->method_count == 0u) {
        // A valid blob applied zero methods -- treat as no-match/rollback.
        (void)Unpatch(ctx);
        return CHAOS_PATCH_ERR_NO_METHODS;
    }

    // Register the live context under a token.
    int32_t token = g_next_token.fetch_add(1, std::memory_order_relaxed);
    {
        std::lock_guard<std::mutex> lock(g_live_mutex);
        GetLivePatches().push_back(LivePatch{
            token,
            g_host_revision.load(std::memory_order_relaxed),
            ctx,
            /* applied_at */ static_cast<uint64_t>(0),  // no clock dependency
        });
    }

    return CHAOS_PATCH_OK;
}

int32_t ChaosRevertPatch(int32_t token) {
    using namespace chaos::il2cpp::runtime_core;

    // Collect the PatchContext* to unpatch under the lock, then call Unpatch()
    // OUTSIDE the lock.  Unpatch() runs unbounded registry teardown
    // (DestroyPatchContext / name-registry writes); holding g_live_mutex across
    // it would both delay concurrent Status queries and risk a lock-order
    // inversion if Unpatch ever re-entered a Chaos* entry point.  Extracting the
    // clean context list first keeps the critical section to just the vector
    // mutation.
    std::vector<PatchContext*> to_unpatch;
    bool found = false;
    {
        std::lock_guard<std::mutex> lock(g_live_mutex);
        auto& live = GetLivePatches();
        if (live.empty()) return CHAOS_PATCH_ERR_INTERNAL;

        if (token == 0) {
            // Revert all live patches.
            for (auto& lp : live) {
                if (lp.ctx != nullptr) {
                    to_unpatch.push_back(lp.ctx);
                    lp.ctx = nullptr;
                }
            }
            live.clear();
            found = true;
        } else {
            for (auto it = live.begin(); it != live.end(); ++it) {
                if (it->token == token) {
                    to_unpatch.push_back(it->ctx);
                    it->ctx = nullptr;
                    live.erase(it);
                    found = true;
                    break;
                }
            }
        }
    }
    if (!found) return CHAOS_PATCH_ERR_INTERNAL;

    int32_t rc = CHAOS_PATCH_OK;
    for (auto* ctx : to_unpatch) {
        if (ctx != nullptr && !Unpatch(ctx)) rc = CHAOS_PATCH_ERR_PARTIAL_ROLLBACK;
    }
    return rc;
}

int32_t ChaosPatchStatus(int32_t max_patches, ChaosPatchStatusEntry* out_info) {
    using namespace chaos::il2cpp::runtime_core;
    std::lock_guard<std::mutex> lock(g_live_mutex);
    auto& live = GetLivePatches();
    int32_t n = static_cast<int32_t>(live.size());
    if (out_info == nullptr || max_patches <= 0) return n;  // count-only
    int32_t written = (n < max_patches) ? n : max_patches;
    for (int32_t i = 0; i < written; ++i) {
        auto& lp = live[i];
        out_info[i].token        = lp.token;
        out_info[i].method_count = (lp.ctx != nullptr) ? static_cast<int32_t>(lp.ctx->method_count) : 0;
        out_info[i].generation   = static_cast<int32_t>(chaos::il2cpp::runtime_core::g_patch_generation.load(std::memory_order_relaxed));
        out_info[i].applied_at   = lp.applied_at;
        out_info[i].status       = (lp.ctx != nullptr) ? 0u : 1u;
        out_info[i]._pad[0] = out_info[i]._pad[1] = out_info[i]._pad[2] = 0;
    }
    return n;
}

}  // extern "C"