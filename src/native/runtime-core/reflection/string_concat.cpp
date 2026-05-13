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

    static char s_buf[4096];
    auto result = fmt::format_to_n(s_buf, sizeof(s_buf) - 1, "{}{}", left_str, right_str);
    size_t len = result.size;
    if (len >= sizeof(s_buf)) len = sizeof(s_buf) - 1;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        abi->string_new_utf8(runtime, thread, s_buf, len));
}

}  // namespace chaos::il2cpp::runtime_core
}  // extern "C"