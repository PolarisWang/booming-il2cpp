// string_stubs.cpp — String helper stub implementations
#include <chaos/native_types.h>
#include <cstring>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INTPTR ChaosStringContains(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (str == 0 || value == 0) return 0;
    auto* sh = reinterpret_cast<const StubStringHeader*>(str);
    auto* vh = reinterpret_cast<const StubStringHeader*>(value);
    if (vh->byte_count > sh->byte_count) return 0;
    if (vh->byte_count == 0) return 1;

    const char* sdata = stub_string_data(reinterpret_cast<const void*>(str));
    const char* vdata = stub_string_data(reinterpret_cast<const void*>(value));
    for (CHAOS_IL2CPP_UINTPTR i = 0; i <= sh->byte_count - vh->byte_count; ++i) {
        if (std::memcmp(sdata + i, vdata, vh->byte_count) == 0) return 1;
    }
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosStringStartsWith(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (str == 0 || value == 0) return 0;
    auto* sh = reinterpret_cast<const StubStringHeader*>(str);
    auto* vh = reinterpret_cast<const StubStringHeader*>(value);
    if (vh->byte_count > sh->byte_count) return 0;
    return (std::memcmp(stub_string_data(reinterpret_cast<const void*>(str)),
                        stub_string_data(reinterpret_cast<const void*>(value)),
                        vh->byte_count) == 0) ? 1 : 0;
}

CHAOS_IL2CPP_INTPTR ChaosStringEndsWith(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (str == 0 || value == 0) return 0;
    auto* sh = reinterpret_cast<const StubStringHeader*>(str);
    auto* vh = reinterpret_cast<const StubStringHeader*>(value);
    if (vh->byte_count > sh->byte_count) return 0;
    const char* sdata = stub_string_data(reinterpret_cast<const void*>(str));
    const char* vdata = stub_string_data(reinterpret_cast<const void*>(value));
    return (std::memcmp(sdata + sh->byte_count - vh->byte_count, vdata, vh->byte_count) == 0) ? 1 : 0;
}

CHAOS_IL2CPP_INTPTR ChaosStringJoinSs(CHAOS_IL2CPP_INTPTR separator, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (separator == 0 || value == 0) return 0;
    auto* sep_hdr = reinterpret_cast<const StubStringHeader*>(separator);
    auto* arr_hdr = reinterpret_cast<StubArrayHeader*>(value);

    CHAOS_IL2CPP_UINTPTR count = arr_hdr->length;
    CHAOS_IL2CPP_UINTPTR sep_len = sep_hdr->byte_count;

    CHAOS_IL2CPP_UINTPTR total = 0;
    CHAOS_IL2CPP_INTPTR* elements = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<void*>(arr_hdr + 1));
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < count; ++i) {
        auto* elem = reinterpret_cast<const StubStringHeader*>(elements[i]);
        if (elem) total += elem->byte_count;
    }
    if (count > 1) total += sep_len * (count - 1);

    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + total + 1));
    if (result == nullptr) return 0;
    result->type = 0;
    result->byte_count = total;

    char* dest = reinterpret_cast<char*>(result + 1);
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < count; ++i) {
        if (i > 0 && sep_len > 0) {
            std::memcpy(dest, stub_string_data(reinterpret_cast<const void*>(separator)), sep_len);
            dest += sep_len;
        }
        auto* elem = reinterpret_cast<const StubStringHeader*>(elements[i]);
        if (elem && elem->byte_count > 0) {
            std::memcpy(dest, stub_string_data(reinterpret_cast<const void*>(elements[i])), elem->byte_count);
            dest += elem->byte_count;
        }
    }
    *dest = '\0';

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

// ── String stubs (return nullptr) ──
CHAOS_IL2CPP_INTPTR ChaosStringToLower(CHAOS_IL2CPP_INTPTR str) noexcept { (void)str; return 0; }
CHAOS_IL2CPP_INTPTR ChaosStringToUpper(CHAOS_IL2CPP_INTPTR str) noexcept { (void)str; return 0; }
CHAOS_IL2CPP_INTPTR ChaosStringTrim(CHAOS_IL2CPP_INTPTR str) noexcept { (void)str; return 0; }
CHAOS_IL2CPP_INTPTR ChaosStringReplace(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR old_value, CHAOS_IL2CPP_INTPTR new_value) noexcept { (void)str; (void)old_value; (void)new_value; return 0; }
CHAOS_IL2CPP_INTPTR ChaosStringSubstring(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR start_index, CHAOS_IL2CPP_INTPTR length) noexcept { (void)str; (void)start_index; (void)length; return 0; }
CHAOS_IL2CPP_INTPTR ChaosStringRemove(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR start_index, CHAOS_IL2CPP_INTPTR length) noexcept { (void)str; (void)start_index; (void)length; return 0; }
CHAOS_IL2CPP_INTPTR ChaosStringFormat(CHAOS_IL2CPP_INTPTR format_str, CHAOS_IL2CPP_INTPTR arg0, CHAOS_IL2CPP_INTPTR arg1) noexcept { (void)format_str; (void)arg0; (void)arg1; return 0; }
CHAOS_IL2CPP_INTPTR ChaosStringFastAllocate(CHAOS_IL2CPP_INTPTR length) noexcept { (void)length; return 0; }
void ChaosStringAppend(CHAOS_IL2CPP_INTPTR builder, CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR arg) noexcept { (void)builder; (void)str; (void)arg; }

CHAOS_IL2CPP_INTPTR ChaosFormattablestringFactoryCreate(CHAOS_IL2CPP_INTPTR format, CHAOS_IL2CPP_INTPTR args) noexcept
{
    (void)format; (void)args;
    return 0;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core