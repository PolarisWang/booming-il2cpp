#include "abi_manifest.h"
#include <chaos/native_types.h>
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// ── FNV-1a hash (32-bit) ───────────────────────────────────────────────
static uint32_t AbiManifestHash(const uint8_t* data, size_t byte_count)
{
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < byte_count; i++)
    {
        hash ^= static_cast<uint32_t>(data[i]);
        hash *= 16777619u;
    }
    return hash;
}

// ── Validate a single carrier code is in range ─────────────────────────
static bool IsValidCarrierCode(ChaosAbiCarrierCode code)
{
    return code <= CHAOS_ABI_CARRIER_BY_REF_TO_VALUE_TYPE;
}

extern "C" {

uint32_t ChaosAbiManifestGetMethodParamOffset(const ChaosAbiManifestV0* manifest,
                                               uint32_t method_index)
{
    if (manifest == nullptr)
        return 0;
    if (method_index >= manifest->method_count)
        return 0;

    // O(1) path: prefix-sum array precomputed at compile time.
    if (manifest->param_offset_prefix_sum != nullptr) {
        return manifest->param_offset_prefix_sum[method_index];
    }

    // O(n) fallback: iterate entries (legacy manifests without prefix-sum).
    const auto* entries = CHAOS_ABI_MANIFEST_ENTRIES(manifest);
    uint32_t offset = 0;
    for (uint32_t i = 0; i < method_index; i++)
    {
        offset += entries[i].parameter_count;
    }
    return offset;
}

ChaosAbiManifestResult ChaosAbiManifestCheckMethodSignature(
    const ChaosAbiManifestV0* manifest,
    uint32_t method_index,
    ChaosAbiCarrierCode expected_return,
    const ChaosAbiCarrierCode* expected_params,
    uint8_t expected_param_count)
{
    if (manifest == nullptr)
        return CHAOS_ABI_MANIFEST_ERR_NULL;
    if (method_index >= manifest->method_count)
        return CHAOS_ABI_MANIFEST_ERR_INDEX;

    const auto* entries = CHAOS_ABI_MANIFEST_ENTRIES(manifest);
    if (entries[method_index].return_carrier != expected_return)
        return CHAOS_ABI_MANIFEST_ERR_CARRIER;
    if (entries[method_index].parameter_count != expected_param_count)
        return CHAOS_ABI_MANIFEST_ERR_CARRIER;

    uint32_t param_offset = ChaosAbiManifestGetMethodParamOffset(manifest, method_index);
    const auto* parameters = CHAOS_ABI_MANIFEST_PARAMETERS(manifest);
    for (uint8_t i = 0; i < expected_param_count; i++)
    {
        if (param_offset + i >= manifest->parameters_byte_count)
            return CHAOS_ABI_MANIFEST_ERR_CARRIER;
        if (parameters[param_offset + i] != expected_params[i])
            return CHAOS_ABI_MANIFEST_ERR_CARRIER;
    }

    return CHAOS_ABI_MANIFEST_OK;
}

ChaosAbiManifestResult ChaosAbiManifestValidate(const ChaosAbiManifestV0* manifest)
{
    if (manifest == nullptr)
        return CHAOS_ABI_MANIFEST_ERR_NULL;

    if (manifest->abi_version != CHAOS_ABI_MANIFEST_VERSION)
        return CHAOS_ABI_MANIFEST_ERR_VERSION;

    if (manifest->method_count == 0 || manifest->method_count > CHAOS_ABI_MANIFEST_MAX_METHODS)
        return CHAOS_ABI_MANIFEST_ERR_COUNT;

    const auto* entries = CHAOS_ABI_MANIFEST_ENTRIES(manifest);
    const auto* parameters = CHAOS_ABI_MANIFEST_PARAMETERS(manifest);

    size_t param_offset = 0;
    for (uint32_t i = 0; i < manifest->method_count; i++)
    {
        if (!IsValidCarrierCode(entries[i].return_carrier))
            return CHAOS_ABI_MANIFEST_ERR_CARRIER;

        for (uint8_t j = 0; j < entries[i].parameter_count; j++)
        {
            if (param_offset >= manifest->parameters_byte_count)
                return CHAOS_ABI_MANIFEST_ERR_CARRIER;
            if (!IsValidCarrierCode(parameters[param_offset]))
                return CHAOS_ABI_MANIFEST_ERR_CARRIER;
            param_offset++;
        }
    }

    // Verify checksum if non-zero
    if (manifest->checksum != 0)
    {
        // Hash entries[] + params[] (excludes header, which includes the
        // link-time param_offset_prefix_sum pointer that is not reproducible).
        size_t payload_offset = sizeof(ChaosAbiManifestV0);
        size_t payload_byte_count = manifest->method_count * sizeof(ChaosAbiMethodEntryV0) +
                                     manifest->parameters_byte_count;
        uint32_t expected_checksum = AbiManifestHash(
            reinterpret_cast<const uint8_t*>(manifest) + payload_offset,
            payload_byte_count);
        if (manifest->checksum != expected_checksum)
            return CHAOS_ABI_MANIFEST_ERR_CHECKSUM;
    }

    return CHAOS_ABI_MANIFEST_OK;
}

int ChaosAbiManifestCompatible(const ChaosAbiManifestV0* expected,
                                const ChaosAbiManifestV0* actual)
{
    if (expected == nullptr || actual == nullptr)
        return 0;

    if (expected->method_count != actual->method_count)
        return 0;

    if (expected->parameters_byte_count != actual->parameters_byte_count)
        return 0;

    // Compare entries and parameters by the total payload
    // Compare from entries[] onwards, skipping the entire header (which includes
    // the link-time param_offset_prefix_sum pointer that differs per DLL).
    size_t payload_offset = sizeof(ChaosAbiManifestV0);
    const uint8_t* expected_payload = reinterpret_cast<const uint8_t*>(expected) + payload_offset;
    const uint8_t* actual_payload   = reinterpret_cast<const uint8_t*>(actual) + payload_offset;
    size_t payload_size = expected->method_count * sizeof(ChaosAbiMethodEntryV0) +
                          expected->parameters_byte_count;

    return std::memcmp(expected_payload, actual_payload, payload_size) == 0 ? 1 : 0;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
