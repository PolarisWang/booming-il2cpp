// hotpatch_api.h — Public C API for hotpatch application and management
//
// This is the production-grade external interface for the hot-update subsystem.
// Unlike the internal patch_loader.h API (which exposes PatchContext, host arrays,
// and other implementation details), this API is a stable, minimal, self-contained
// surface that any external host application can link against.
//
// Key design:
//   - Self-contained: .patchdata blob carries all metadata needed (no external
//     host arrays required).  The API reads type/method names from the patch
//     itself, falling back to the patch DLL's own metadata.
//   - Transactional: Apply either succeeds for ALL methods in the patch or
//     reverts everything (no partial-apply state).
//   - Return codes are explicit: 0 = success, negative = specific failure mode.
//   - Thread-safe under the same concurrency model as the rest of the runtime
//     (patch_loader uses its own synchronization; callers must not concurrently
//     apply+revert the same token).
//
// Integration:
//   #include <chaos/hotpatch_api.h>
//   // link against libchaos_runtime_core (or libchaos_hot_update)
//   int32_t r = ChaosApplyPatch(patch_data, patch_size);
//   if (r == 0) printf("Hotpatch applied\n");
//   else if (r == -2) printf("Version mismatch\n");

#ifndef CHAOS_IL2CPP_HOTPATCH_API_H_
#define CHAOS_IL2CPP_HOTPATCH_API_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ── Return codes ──────────────────────────────────────────────────
#define CHAOS_PATCH_OK                       0   // Success (all methods committed)
#define CHAOS_PATCH_ERR_INVALID_FORMAT      -1   // Bad magic/version/header
#define CHAOS_PATCH_ERR_VERSION_MISMATCH    -2   // min_host_revision > host revision
#define CHAOS_PATCH_ERR_NO_METHODS          -3   // No methods matched AOT registry
#define CHAOS_PATCH_ERR_PARTIAL_ROLLBACK    -4   // Some methods failed → rolled back
#define CHAOS_PATCH_ERR_INTERNAL            -5   // Allocation failure, etc.

// ── Status entry ───────────────────────────────────────────────────
typedef struct {
    int32_t  token;            ///< Opaque patch token (from ChaosApplyPatch).
    int32_t  method_count;     ///< Number of methods in this patch.
    int32_t  generation;       ///< Host patch generation counter at apply time.
    uint64_t applied_at;       ///< Timestamp (host-defined epoch, ms since boot).
    uint8_t  status;           ///< 0=active, 1=reverted, 2=partial_fail.
    uint8_t  _pad[3];          ///< Reserved.
} ChaosPatchStatusEntry;

// ── Public API ─────────────────────────────────────────────────────

/// Apply a hotpatch from a self-contained .patchdata blob.
/// Validates the format, resolves all methods, then applies them transactionally.
/// @param data   Pointer to the .patchdata binary (borrowed — caller must keep
///               alive until Unpatch/Revert, or the API copies it internally).
/// @param size   Size of the data in bytes.
/// @return 0 on success (all methods committed), negative on failure.
int32_t ChaosApplyPatch(const void* data, size_t size);

/// Revert a previously applied patch by its token.
/// @param token  Token returned by ChaosApplyPatch. 0 = revert all.
/// @return 0 on success, negative on failure.
int32_t ChaosRevertPatch(int32_t token);

/// Query the status of applied patches.
/// Fills out_info with the current patch state.  If the array is too small,
/// returns the total number of patches (truncation hint).
/// @param max_patches  Capacity of out_info array (0 = query count only).
/// @param out_info     Output buffer, or NULL if max_patches==0.
/// @return Number of patches written (may be > max_patches — truncated).
int32_t ChaosPatchStatus(int32_t max_patches, ChaosPatchStatusEntry* out_info);

/// Set the host build revision.  Patches with min_host_revision > this value
/// will be rejected by ChaosApplyPatch.  Call once at startup or when the
/// host's revision changes (e.g. after a full update).
/// @param revision  Host build revision (0 = accept any patch).
void ChaosSetHostRevision(uint32_t revision);

#ifdef __cplusplus
}
#endif

#endif  // CHAOS_IL2CPP_HOTPATCH_API_H_