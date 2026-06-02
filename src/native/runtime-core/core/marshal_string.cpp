#include "marshal_internal.h"
#include "runtime_core.h"
#include "runtime_stubs/stub_common.h"

#include <chaos/pal/pal_dl.h>
#include <chaos/pal/pal_error.h>
#include <chaos/pal/pal_encoding.h>

namespace chaos::il2cpp::runtime_core {
using namespace chaos::il2cpp::marshal_abi;

CHAOS_IL2CPP_INT32 InteropKernel32GetLastError() {
    return chaos::il2cpp::pal::PalGetLastError();
}

CHAOS_IL2CPP_INT32 InteropKernel32GetCurrentProcessId() {
    return chaos::il2cpp::pal::PalGetCurrentProcessId();
}

CHAOS_IL2CPP_INT32 InteropKernel32GetCurrentThreadId() {
    return chaos::il2cpp::pal::PalGetCurrentThreadId();
}

CHAOS_IL2CPP_INTPTR InteropKernel32GetCurrentProcess() {
    return static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::pal::PalGetCurrentProcess());
}

CHAOS_IL2CPP_INTPTR InteropKernel32GetCurrentThread() {
    return static_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::pal::PalGetCurrentThread());
}

bool InteropKernel32CloseHandle(CHAOS_IL2CPP_INTPTR handle) {
    return chaos::il2cpp::pal::PalCloseHandle(static_cast<intptr_t>(handle));
}

bool InteropKernel32FreeLibrary(CHAOS_IL2CPP_INTPTR handle) {
    return handle == 0 || chaos::il2cpp::pal::PalDlClose(reinterpret_cast<chaos::il2cpp::pal::PalLibraryPtr>(handle));
}

// ── String marshalling helpers (P/Invoke) ────────────────────────────

CHAOS_IL2CPP_INT32 MarshalStringToWide(void* managed_string, CHAOS_IL2CPP_UINT16* wide_buf, CHAOS_IL2CPP_INT32 buf_capacity) {
    if (managed_string == nullptr) return 0;
    auto* hdr = static_cast<StringObjectHeader*>(managed_string);
    const char* utf8 = stub_string_data(hdr);
    const auto byte_len = static_cast<int>(hdr->byte_count);

    int result = chaos::il2cpp::pal::PalUtf8ToUtf16(
        reinterpret_cast<const uint8_t*>(utf8), byte_len,
        wide_buf, buf_capacity);
    return result > 0 ? result : 0;
}

void* MarshalWideToString(RuntimeState* runtime_state, ThreadState* thread_state, const CHAOS_IL2CPP_UINT16* wide_buf, CHAOS_IL2CPP_INT32 wide_len) {
    if (!IsAttached(runtime_state, thread_state) || wide_buf == nullptr || wide_len <= 0) return nullptr;

    int needed = chaos::il2cpp::pal::PalUtf16ToUtf8(wide_buf, wide_len, nullptr, 0);
    if (needed <= 0) return nullptr;

    auto* temp = static_cast<char*>(CHAOS_IL2CPP_MALLOC(static_cast<CHAOS_IL2CPP_SIZE>(needed) + 1u));
    if (temp == nullptr) return nullptr;
    chaos::il2cpp::pal::PalUtf16ToUtf8(wide_buf, wide_len,
        reinterpret_cast<uint8_t*>(temp), needed);
    temp[needed] = '\0';

    void* result = StringNewUtf8(runtime_state, thread_state, temp, static_cast<CHAOS_IL2CPP_UINTPTR>(needed));
    CHAOS_IL2CPP_FREE(temp);
    return result;
}

void* MarshalAnsiToString(RuntimeState* runtime_state, ThreadState* thread_state, const char* ansi_buf, CHAOS_IL2CPP_INT32 byte_len) {
    if (!IsAttached(runtime_state, thread_state) || ansi_buf == nullptr || byte_len <= 0) return nullptr;

    // On all platforms, ACP is treated as UTF-8 for simplicity.
    // Win32 would use MultiByteToWideChar(CP_ACP) → WideCharToMultiByte(CP_UTF8),
    // but modern systems default to UTF-8 code page, making this pass-through safe.
    return StringNewUtf8(runtime_state, thread_state, ansi_buf, static_cast<CHAOS_IL2CPP_UINTPTR>(byte_len));
}

}  // namespace chaos::il2cpp::runtime_core