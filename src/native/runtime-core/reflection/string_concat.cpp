// =====================================================================
// String helpers
// =====================================================================

extern "C" {
namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INTPTR ChaosReflectionConcatStringPairValues(
    CHAOS_IL2CPP_INTPTR left,
    CHAOS_IL2CPP_INTPTR right)
{
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
    // This is faster than fmt::format_to_n ("{}{}") and avoids the
    // thread-safety issue of a static buffer.
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