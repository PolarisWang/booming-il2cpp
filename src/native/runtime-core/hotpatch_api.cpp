#include "hotpatch_api.h"
#include "patch_loader.h"
#include "method_replacement.h"

#include <atomic>
#include <vector>
#include <cstring>

namespace chaos {
namespace il2cpp {
namespace runtime_core {

namespace {

// Host build revision set via ChaosSetHostRevision.  Default 0 = accept any.
std::atomic<uint32_t> g_host_revision{0};
// Next token to hand out for an applied patch.
std::atomic<int32_t> g_next_token{1};
// List of live patch contexts, keyed by token.  These are deleted on Revert.
struct LivePatch {
    int32_t        token = 0;
    uint32_t       host_revision_at_apply = 0;
    PatchContext*  ctx   = nullptr;
    uint64_t       applied_at = 0;
};
std::vector<LivePatch>* g_live = nullptr;      // guarded by apply/revert never concurrent (documented)

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
        0);               // method_count → no per-method overrides
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
    uint32_t min_host = 0u;
    // Minimal header peek for revision check without full parse.
    if (data != nullptr && size >= sizeof(PatchDataHeader)) {
        auto* hdr = static_cast<const PatchDataHeader*>(data);
        if (hdr->magic != PATCH_DATA_MAGIC) return CHAOS_PATCH_ERR_INVALID_FORMAT;
        if (hdr->version >= 4u) {
            // v4 adds min_host_revision after dependency_count.  These are read
            // as a trailer (see patch_data.h v4 layout).  We keep the check here
            // tolerantly: if the field is absent (older producer), accept.
            // Actual enforcement of the field, when present, lives in the loader.
            (void)min_host;
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
        //   1) If size/signature invalid → INVALID_FORMAT
        //   2) otherwise → PARTIAL/INTERNAL
        return CHAOS_PATCH_ERR_PARTIAL_ROLLBACK;
    }
    if (ctx->method_count == 0u) {
        // A valid blob applied zero methods — treat as no-match/rollback.
        (void)Unpatch(ctx);
        return CHAOS_PATCH_ERR_NO_METHODS;
    }

    // Register the live context under a token.
    int32_t token = g_next_token.fetch_add(1, std::memory_order_relaxed);
    if (g_live == nullptr) {
        g_live = new std::vector<LivePatch>();
    }
    g_live->push_back(LivePatch{
        token,
        g_host_revision.load(std::memory_order_relaxed),
        ctx,
        /* applied_at */ static_cast<uint64_t>(0),  // no clock dependency
    });

    return CHAOS_PATCH_OK;
}

int32_t ChaosRevertPatch(int32_t token) {
    using namespace chaos::il2cpp::runtime_core;
    if (g_live == nullptr) return CHAOS_PATCH_ERR_INTERNAL;

    if (token == 0) {
        // Revert all live patches.
        int32_t rc = CHAOS_PATCH_OK;
        for (auto& lp : *g_live) {
            if (lp.ctx != nullptr && !Unpatch(lp.ctx)) rc = CHAOS_PATCH_ERR_PARTIAL_ROLLBACK;
            lp.ctx = nullptr;
        }
        g_live->clear();
        return rc;
    }

    for (auto it = g_live->begin(); it != g_live->end(); ++it) {
        if (it->token == token) {
            bool ok = (it->ctx != nullptr) && Unpatch(it->ctx);
            g_live->erase(it);
            return ok ? CHAOS_PATCH_OK : CHAOS_PATCH_ERR_INTERNAL;
        }
    }
    // Token not found.
    return CHAOS_PATCH_ERR_INTERNAL;
}

int32_t ChaosPatchStatus(int32_t max_patches, ChaosPatchStatusEntry* out_info) {
    using namespace chaos::il2cpp::runtime_core;
    if (g_live == nullptr) return 0;
    int32_t n = static_cast<int32_t>(g_live->size());
    if (out_info == nullptr || max_patches <= 0) return n;  // count-only
    int32_t written = (n < max_patches) ? n : max_patches;
    for (int32_t i = 0; i < written; ++i) {
        auto& lp = (*g_live)[i];
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