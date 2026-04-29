#include "support.h"

#include <chaos/native_types.h>

#include <cstdio>
#include <string>

namespace chaos::il2cpp::support {

namespace {

// Mirrors the proof-only UTF-8 string layout currently produced by runtime-core.
struct StringObjectHeader {
    TypeInfoHandle type;
    CHAOS_IL2CPP_UINTPTR byte_count;
};

const StringObjectHeader* TryGetStringHeader(const void* string_object) {
    if (string_object == nullptr) {
        return nullptr;
    }

    return reinterpret_cast<const StringObjectHeader*>(string_object);
}

}  // namespace

const char* TryGetUtf8View(const void* string_object, CHAOS_IL2CPP_UINTPTR* out_byte_count) {
    if (out_byte_count != nullptr) {
        *out_byte_count = 0u;
    }

    const StringObjectHeader* header = TryGetStringHeader(string_object);
    if (header == nullptr) {
        return nullptr;
    }

    if (out_byte_count != nullptr) {
        *out_byte_count = header->byte_count;
    }

    return reinterpret_cast<const char*>(header + 1);
}

void* CHAOS_RUNTIME_ABI_CALL ConcatStringPair(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* left_string,
    const void* right_string) {
    const RuntimeAbiV0* abi = chaos_runtime_get_abi_v0();
    if (abi == nullptr || runtime_state == nullptr || thread_state == nullptr) {
        return nullptr;
    }

    CHAOS_IL2CPP_UINTPTR left_byte_count = 0u;
    CHAOS_IL2CPP_UINTPTR right_byte_count = 0u;
    const char* left_utf8 = TryGetUtf8View(left_string, &left_byte_count);
    const char* right_utf8 = TryGetUtf8View(right_string, &right_byte_count);
    if ((left_string != nullptr && left_utf8 == nullptr) || (right_string != nullptr && right_utf8 == nullptr)) {
        return nullptr;
    }

    CHAOS_IL2CPP_STRING combined;
    combined.reserve(static_cast<CHAOS_IL2CPP_SIZE>(left_byte_count + right_byte_count));
    if (left_utf8 != nullptr) {
        combined.append(left_utf8, static_cast<CHAOS_IL2CPP_SIZE>(left_byte_count));
    }

    if (right_utf8 != nullptr) {
        combined.append(right_utf8, static_cast<CHAOS_IL2CPP_SIZE>(right_byte_count));
    }

    return abi->string_new_utf8(runtime_state, thread_state, combined.c_str(), combined.size());
}

void* CHAOS_RUNTIME_ABI_CALL ConcatStringTriple(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* first_string,
    const void* second_string,
    const void* third_string) {
    const RuntimeAbiV0* abi = chaos_runtime_get_abi_v0();
    if (abi == nullptr || runtime_state == nullptr || thread_state == nullptr) {
        return nullptr;
    }

    CHAOS_IL2CPP_UINTPTR first_byte_count = 0u;
    CHAOS_IL2CPP_UINTPTR second_byte_count = 0u;
    CHAOS_IL2CPP_UINTPTR third_byte_count = 0u;
    const char* first_utf8 = TryGetUtf8View(first_string, &first_byte_count);
    const char* second_utf8 = TryGetUtf8View(second_string, &second_byte_count);
    const char* third_utf8 = TryGetUtf8View(third_string, &third_byte_count);
    if ((first_string != nullptr && first_utf8 == nullptr) ||
        (second_string != nullptr && second_utf8 == nullptr) ||
        (third_string != nullptr && third_utf8 == nullptr)) {
        return nullptr;
    }

    CHAOS_IL2CPP_STRING combined;
    combined.reserve(static_cast<CHAOS_IL2CPP_SIZE>(first_byte_count + second_byte_count + third_byte_count));
    if (first_utf8 != nullptr) {
        combined.append(first_utf8, static_cast<CHAOS_IL2CPP_SIZE>(first_byte_count));
    }

    if (second_utf8 != nullptr) {
        combined.append(second_utf8, static_cast<CHAOS_IL2CPP_SIZE>(second_byte_count));
    }

    if (third_utf8 != nullptr) {
        combined.append(third_utf8, static_cast<CHAOS_IL2CPP_SIZE>(third_byte_count));
    }

    return abi->string_new_utf8(runtime_state, thread_state, combined.c_str(), combined.size());
}

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL WriteLineString(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* string_object) {
    (void)runtime_state;
    (void)thread_state;

    CHAOS_IL2CPP_UINTPTR byte_count = 0u;
    const char* utf8_text = TryGetUtf8View(string_object, &byte_count);
    if (string_object != nullptr && utf8_text == nullptr) {
        return 1;
    }

    if (utf8_text != nullptr && CHAOS_IL2CPP_FWRITE(utf8_text, 1u, static_cast<CHAOS_IL2CPP_SIZE>(byte_count), stdout) != byte_count) {
        return 1;
    }

    if (CHAOS_IL2CPP_FPUTC('\n', stdout) == EOF) {
        return 1;
    }

    return CHAOS_IL2CPP_FFLUSH(stdout) == 0 ? 0 : 1;
}

}  // namespace chaos::il2cpp::support
