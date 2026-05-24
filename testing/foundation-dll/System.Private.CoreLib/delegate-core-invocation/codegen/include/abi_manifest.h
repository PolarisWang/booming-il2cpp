#ifndef CHAOS_ABI_MANIFEST_H_
#define CHAOS_ABI_MANIFEST_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ── Version ────────────────────────────────────────────────────────────
#define CHAOS_ABI_MANIFEST_VERSION 0u
#define CHAOS_ABI_MANIFEST_MAX_METHODS    65535u
#define CHAOS_ABI_MANIFEST_MAX_PARAMETERS 255u

// ── Carrier kind codes (mirrors AotCoreIrAbiCarrierKind enum values) ──
typedef uint8_t ChaosAbiCarrierCode;

enum {
    CHAOS_ABI_CARRIER_VOID                 = 0,
    CHAOS_ABI_CARRIER_INT32                = 1,
    CHAOS_ABI_CARRIER_NATIVE_INT           = 2,
    CHAOS_ABI_CARRIER_VALUE_TYPE_BY_VALUE  = 3,
    CHAOS_ABI_CARRIER_INT8                 = 4,
    CHAOS_ABI_CARRIER_UINT8                = 5,
    CHAOS_ABI_CARRIER_INT16                = 6,
    CHAOS_ABI_CARRIER_UINT16               = 7,
    CHAOS_ABI_CARRIER_FLOAT32              = 8,
    CHAOS_ABI_CARRIER_FLOAT64              = 9,
    CHAOS_ABI_CARRIER_INT64                = 10,
    CHAOS_ABI_CARRIER_UINT64               = 11,
    CHAOS_ABI_CARRIER_BY_REF               = 12,
    CHAOS_ABI_CARRIER_MULTI_RETURN         = 13,
    CHAOS_ABI_CARRIER_BY_REF_TO_VALUE_TYPE = 14,
};

// ── Per-method ABI signature entry ─────────────────────────────────────
typedef struct ChaosAbiMethodEntryV0 {
    ChaosAbiCarrierCode return_carrier;
    uint8_t             parameter_count;
} ChaosAbiMethodEntryV0;

// ── Per-DLL ABI manifest ───────────────────────────────────────────────
// The manifest is emitted as a constant in each NativeAOT DLL's generated
// code.  Variable-length payload follows immediately after the fixed header:
//   ChaosAbiMethodEntryV0 entries[method_count]
//   ChaosAbiCarrierCode   parameter_carriers[parameters_byte_count]
// Total bytes = sizeof(ChaosAbiManifestV0) +
//               method_count * sizeof(ChaosAbiMethodEntryV0) +
//               parameters_byte_count
typedef struct ChaosAbiManifestV0 {
    uint32_t abi_version;               // CHAOS_ABI_MANIFEST_VERSION
    uint32_t method_count;              // number of methods in the manifest
    uint32_t parameters_byte_count;     // total bytes in the parameters array
    uint32_t checksum;                  // FNV-1a hash over entries+params (0 = skip)
    const uint32_t* param_offset_prefix_sum;  // [method_count+1] prefix-sum, null = O(n) fallback
} ChaosAbiManifestV0;

// ── Accessor macros ─────────────────────────────────────────────────────
#define CHAOS_ABI_MANIFEST_ENTRIES(m) \
    ((const ChaosAbiMethodEntryV0*)((const uint8_t*)(m) + sizeof(ChaosAbiManifestV0)))

#define CHAOS_ABI_MANIFEST_PARAMETERS(m) \
    ((const ChaosAbiCarrierCode*)((const uint8_t*)(CHAOS_ABI_MANIFEST_ENTRIES(m)) + \
        ((m)->method_count * sizeof(ChaosAbiMethodEntryV0))))

// ── Validation result codes ────────────────────────────────────────────
typedef enum ChaosAbiManifestResult {
    CHAOS_ABI_MANIFEST_OK               = 0,
    CHAOS_ABI_MANIFEST_ERR_NULL         = 1,
    CHAOS_ABI_MANIFEST_ERR_VERSION      = 2,
    CHAOS_ABI_MANIFEST_ERR_CARRIER      = 3,
    CHAOS_ABI_MANIFEST_ERR_CHECKSUM     = 4,
    CHAOS_ABI_MANIFEST_ERR_COUNT        = 5,
    CHAOS_ABI_MANIFEST_ERR_INDEX        = 6,
} ChaosAbiManifestResult;

// ── Lookup helpers ─────────────────────────────────────────────────────

/// Returns the cumulative parameter count for all methods before the given
/// method_index.  Useful for computing the offset into the parameters array.
/// Returns 0 on overflow/error.
uint32_t ChaosAbiManifestGetMethodParamOffset(const ChaosAbiManifestV0* manifest,
                                               uint32_t method_index);

/// Validates that the method at method_index has the expected return carrier
/// and parameter carriers.  Returns CHAOS_ABI_MANIFEST_OK on match.
ChaosAbiManifestResult ChaosAbiManifestCheckMethodSignature(
    const ChaosAbiManifestV0* manifest,
    uint32_t method_index,
    ChaosAbiCarrierCode expected_return,
    const ChaosAbiCarrierCode* expected_params,
    uint8_t expected_param_count);

// ── Validation ─────────────────────────────────────────────────────────
/// Validates internal consistency: version match, carrier codes in range,
/// checksum verification.  Returns CHAOS_ABI_MANIFEST_OK on success.
ChaosAbiManifestResult ChaosAbiManifestValidate(const ChaosAbiManifestV0* manifest);

// ── Comparison ─────────────────────────────────────────────────────────
/// Compares two manifests for ABI equality (same method count and all
/// carrier codes match).  Returns 1 (true) if ABI-compatible, 0 otherwise.
int ChaosAbiManifestCompatible(const ChaosAbiManifestV0* expected,
                               const ChaosAbiManifestV0* actual);

#ifdef __cplusplus
}
#endif

#endif  // CHAOS_ABI_MANIFEST_H_
