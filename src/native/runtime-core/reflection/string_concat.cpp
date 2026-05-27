// =====================================================================
// String helpers
// =====================================================================

#include "generated_code_compat.h"
#include "gc/gc_helpers.h"
#include "string_table.h"

extern "C" {
namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INTPTR ChaosReflectionConcatStringPairValues(
    CHAOS_IL2CPP_INTPTR left,
    CHAOS_IL2CPP_INTPTR right)
{
    // Fast path: both args are direct managed string pointers (from generated code).
    // Use direct field access — no ABI, no strlen, no stack buffer.
    if (left != 0 && right != 0 &&
        !chaos_is_string_id(left) && !chaos_is_string_id(right))
    {
        auto* chaos_left = reinterpret_cast<const chaos_managed_string*>(left);
        auto* chaos_right = reinterpret_cast<const chaos_managed_string*>(right);

        // Read lengths directly from string objects (no strlen needed).
        // Note: generated code uses CHAOS_IL2CPP_INTPTR for length, but the lower
        // CHAOS_IL2CPP_INT32 bytes are sufficient for all practical string sizes.
        const auto chaos_left_len = static_cast<CHAOS_IL2CPP_SIZE>(chaos_left->length);
        const auto chaos_right_len = static_cast<CHAOS_IL2CPP_SIZE>(chaos_right->length);
        const auto chaos_total = static_cast<CHAOS_IL2CPP_SIZE>(chaos_left_len + chaos_right_len);

        auto* chaos_raw = static_cast<char*>(
            GcAllocateAtomic(
                sizeof(CHAOS_IL2CPP_STRING_TYPE) + chaos_total + 1));
        if (chaos_raw == nullptr) return 0;

        auto* chaos_str = reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_raw);
        chaos_str->header.type_info = chaos_left->header.type_info;  // same type
        chaos_str->length = static_cast<CHAOS_IL2CPP_INT32>(chaos_total);
        chaos_str->utf8_data = chaos_raw + sizeof(CHAOS_IL2CPP_STRING_TYPE);
        chaos_str->string_id = 0;

        if (chaos_left_len > 0) {
            CHAOS_IL2CPP_MEMCPY(chaos_str->utf8_data, chaos_left->utf8_data, chaos_left_len);
        }
        if (chaos_right_len > 0) {
            CHAOS_IL2CPP_MEMCPY(chaos_str->utf8_data + chaos_left_len, chaos_right->utf8_data, chaos_right_len);
        }
        chaos_str->utf8_data[chaos_total] = '\0';

        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_str);
    }

    // Fallback: ABI-based path for StringIds (interpreter) or mixed inputs.
    if (left == 0 && right == 0) return 0;

    auto* runtime = GetCurrentRuntimeState();
    auto* thread  = GetCurrentThreadState();
    const auto* abi = GetRuntimeAbiV0();
    if (runtime == nullptr || thread == nullptr || abi == nullptr || abi->string_new_utf8 == nullptr)
        return 0;

    const char* left_str = DecodeStringValue(left);
    const char* right_str = DecodeStringValue(right);
    if (left_str == nullptr) left_str = "";
    if (right_str == nullptr) right_str = "";

    size_t left_len = std::strlen(left_str);
    size_t right_len = std::strlen(right_str);
    size_t total_len = left_len + right_len;

    // Stack-allocate buffer for the concatenation result.
    char buf[4096];
    if (total_len >= sizeof(buf))
        total_len = sizeof(buf) - 1;

    size_t copy_left = (std::min)(left_len, total_len);
    std::memcpy(buf, left_str, copy_left);
    if (copy_left < total_len) {
        size_t copy_right = (std::min)(right_len, total_len - copy_left);
        std::memcpy(buf + copy_left, right_str, copy_right);
    }
    buf[total_len] = '\0';

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        abi->string_new_utf8(runtime, thread, buf, total_len));
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"