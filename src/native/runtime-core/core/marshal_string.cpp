#include "marshal_internal.h"
#include "runtime_core.h"
#include "runtime_stubs/stub_common.h"

namespace chaos::il2cpp::runtime_core {
using namespace chaos::il2cpp::marshal_abi;

CHAOS_IL2CPP_INT32 InteropKernel32GetLastError() {
#if defined(_WIN32)
    return static_cast<CHAOS_IL2CPP_INT32>(::GetLastError());
#else
    return 0;
#endif
}

CHAOS_IL2CPP_INT32 InteropKernel32GetCurrentProcessId() {
#if defined(_WIN32)
    return static_cast<CHAOS_IL2CPP_INT32>(::GetCurrentProcessId());
#else
    return 0;
#endif
}

CHAOS_IL2CPP_INT32 InteropKernel32GetCurrentThreadId() {
#if defined(_WIN32)
    return static_cast<CHAOS_IL2CPP_INT32>(::GetCurrentThreadId());
#else
    return 0;
#endif
}

CHAOS_IL2CPP_INTPTR InteropKernel32GetCurrentProcess() {
#if defined(_WIN32)
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(::GetCurrentProcess());
#else
    return 0;
#endif
}

CHAOS_IL2CPP_INTPTR InteropKernel32GetCurrentThread() {
#if defined(_WIN32)
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(::GetCurrentThread());
#else
    return 0;
#endif
}

bool InteropKernel32CloseHandle(CHAOS_IL2CPP_INTPTR handle) {
#if defined(_WIN32)
    return handle == 0 || ::CloseHandle(reinterpret_cast<HANDLE>(handle)) != 0;
#else
    (void)handle;
    return false;
#endif
}

bool InteropKernel32FreeLibrary(CHAOS_IL2CPP_INTPTR handle) {
#if defined(_WIN32)
    return handle == 0 || ::FreeLibrary(reinterpret_cast<HMODULE>(handle)) != 0;
#else
    (void)handle;
    return false;
#endif
}

// ── String marshalling helpers (P/Invoke) ────────────────────────────

CHAOS_IL2CPP_INT32 MarshalStringToWide(void* managed_string, CHAOS_IL2CPP_UINT16* wide_buf, CHAOS_IL2CPP_INT32 buf_capacity) {
    if (managed_string == nullptr) return 0;
    auto* hdr = static_cast<StringObjectHeader*>(managed_string);
    const char* utf8 = stub_string_data(hdr);
    const auto byte_len = static_cast<int>(hdr->byte_count);

#if defined(_WIN32)
    int needed = ::MultiByteToWideChar(CP_UTF8, 0, utf8, byte_len, nullptr, 0);
    if (needed <= 0) return 0;
    if (wide_buf == nullptr || buf_capacity <= 0) return needed;
    int written = ::MultiByteToWideChar(CP_UTF8, 0, utf8, byte_len, reinterpret_cast<wchar_t*>(wide_buf), buf_capacity);
    return written > 0 ? written : 0;
#else
    int needed = 0;
    const auto* in = reinterpret_cast<const unsigned char*>(utf8);
    const auto* end = in + byte_len;
    while (in < end) {
        uint32_t cp;
        if ((*in & 0x80) == 0) { cp = *in++; }
        else if ((*in & 0xE0) == 0xC0 && in + 1 < end) { cp = (*in & 0x1F) << 6 | (in[1] & 0x3F); in += 2; }
        else if ((*in & 0xF0) == 0xE0 && in + 2 < end) { cp = (*in & 0x0F) << 12 | (in[1] & 0x3F) << 6 | (in[2] & 0x3F); in += 3; }
        else if ((*in & 0xF8) == 0xF0 && in + 3 < end) { cp = (*in & 0x07) << 18 | (in[1] & 0x3F) << 12 | (in[2] & 0x3F) << 6 | (in[3] & 0x3F); in += 4; }
        else { cp = 0xFFFD; ++in; }
        if (cp < 0x10000) { ++needed; }
        else { needed += 2; }
    }
    if (wide_buf == nullptr || buf_capacity <= 0) return needed;

    int written = 0;
    in = reinterpret_cast<const unsigned char*>(utf8);
    while (in < end && written < buf_capacity) {
        uint32_t cp;
        if ((*in & 0x80) == 0) { cp = *in++; }
        else if ((*in & 0xE0) == 0xC0 && in + 1 < end) { cp = (*in & 0x1F) << 6 | (in[1] & 0x3F); in += 2; }
        else if ((*in & 0xF0) == 0xE0 && in + 2 < end) { cp = (*in & 0x0F) << 12 | (in[1] & 0x3F) << 6 | (in[2] & 0x3F); in += 3; }
        else if ((*in & 0xF8) == 0xF0 && in + 3 < end) { cp = (*in & 0x07) << 18 | (in[1] & 0x3F) << 12 | (in[2] & 0x3F) << 6 | (in[3] & 0x3F); in += 4; }
        else { cp = 0xFFFD; ++in; }
        if (cp < 0x10000) {
            wide_buf[written++] = static_cast<CHAOS_IL2CPP_UINT16>(cp);
        } else if (written + 1 < buf_capacity) {
            cp -= 0x10000;
            wide_buf[written++] = static_cast<CHAOS_IL2CPP_UINT16>(0xD800 | (cp >> 10));
            wide_buf[written++] = static_cast<CHAOS_IL2CPP_UINT16>(0xDC00 | (cp & 0x3FF));
        }
    }
    return written;
#endif
}

void* MarshalWideToString(RuntimeState* runtime_state, ThreadState* thread_state, const CHAOS_IL2CPP_UINT16* wide_buf, CHAOS_IL2CPP_INT32 wide_len) {
    if (!IsAttached(runtime_state, thread_state) || wide_buf == nullptr || wide_len <= 0) return nullptr;

#if defined(_WIN32)
    int needed = ::WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const wchar_t*>(wide_buf), wide_len, nullptr, 0, nullptr, nullptr);
    if (needed <= 0) return nullptr;

    auto* temp = static_cast<char*>(CHAOS_IL2CPP_MALLOC(static_cast<CHAOS_IL2CPP_SIZE>(needed) + 1u));
    if (temp == nullptr) return nullptr;
    ::WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const wchar_t*>(wide_buf), wide_len, temp, needed, nullptr, nullptr);
    temp[needed] = '\0';
#else
    CHAOS_IL2CPP_SIZE needed = 0;
    for (int i = 0; i < wide_len; ++i) {
        uint32_t cp = wide_buf[i];
        if (cp >= 0xD800 && cp <= 0xDBFF && i + 1 < wide_len) {
            cp = 0x10000 + ((cp - 0xD800) << 10) + (wide_buf[++i] - 0xDC00);
        }
        needed += (cp < 0x80) ? 1 : (cp < 0x800) ? 2 : (cp < 0x10000) ? 3 : 4;
    }
    auto* temp = static_cast<char*>(CHAOS_IL2CPP_MALLOC(needed + 1u));
    if (temp == nullptr) return nullptr;
    CHAOS_IL2CPP_SIZE pos = 0;
    for (int i = 0; i < wide_len; ++i) {
        uint32_t cp = wide_buf[i];
        if (cp >= 0xD800 && cp <= 0xDBFF && i + 1 < wide_len) {
            cp = 0x10000 + ((cp - 0xD800) << 10) + (wide_buf[++i] - 0xDC00);
        }
        if (cp < 0x80) { temp[pos++] = static_cast<char>(cp); }
        else if (cp < 0x800) { temp[pos++] = static_cast<char>(0xC0 | (cp >> 6)); temp[pos++] = static_cast<char>(0x80 | (cp & 0x3F)); }
        else if (cp < 0x10000) { temp[pos++] = static_cast<char>(0xE0 | (cp >> 12)); temp[pos++] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F)); temp[pos++] = static_cast<char>(0x80 | (cp & 0x3F)); }
        else { temp[pos++] = static_cast<char>(0xF0 | (cp >> 18)); temp[pos++] = static_cast<char>(0x80 | ((cp >> 12) & 0x3F)); temp[pos++] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F)); temp[pos++] = static_cast<char>(0x80 | (cp & 0x3F)); }
    }
    temp[pos] = '\0';
#endif

    void* result = StringNewUtf8(runtime_state, thread_state, temp, static_cast<CHAOS_IL2CPP_UINTPTR>(needed));
    CHAOS_IL2CPP_FREE(temp);
    return result;
}

void* MarshalAnsiToString(RuntimeState* runtime_state, ThreadState* thread_state, const char* ansi_buf, CHAOS_IL2CPP_INT32 byte_len) {
    if (!IsAttached(runtime_state, thread_state) || ansi_buf == nullptr || byte_len <= 0) return nullptr;

#if defined(_WIN32)
    int wide_needed = ::MultiByteToWideChar(CP_ACP, 0, ansi_buf, byte_len, nullptr, 0);
    if (wide_needed <= 0) return nullptr;
    auto* wide = static_cast<wchar_t*>(CHAOS_IL2CPP_MALLOC(static_cast<CHAOS_IL2CPP_SIZE>(wide_needed) * sizeof(wchar_t)));
    if (wide == nullptr) return nullptr;
    ::MultiByteToWideChar(CP_ACP, 0, ansi_buf, byte_len, wide, wide_needed);

    int utf8_needed = ::WideCharToMultiByte(CP_UTF8, 0, wide, wide_needed, nullptr, 0, nullptr, nullptr);
    if (utf8_needed <= 0) { CHAOS_IL2CPP_FREE(wide); return nullptr; }
    auto* utf8 = static_cast<char*>(CHAOS_IL2CPP_MALLOC(static_cast<CHAOS_IL2CPP_SIZE>(utf8_needed) + 1u));
    if (utf8 == nullptr) { CHAOS_IL2CPP_FREE(wide); return nullptr; }
    ::WideCharToMultiByte(CP_UTF8, 0, wide, wide_needed, utf8, utf8_needed, nullptr, nullptr);
    utf8[utf8_needed] = '\0';
    CHAOS_IL2CPP_FREE(wide);

    void* result = StringNewUtf8(runtime_state, thread_state, utf8, static_cast<CHAOS_IL2CPP_UINTPTR>(utf8_needed));
    CHAOS_IL2CPP_FREE(utf8);
    return result;
#else
    return StringNewUtf8(runtime_state, thread_state, ansi_buf, static_cast<CHAOS_IL2CPP_UINTPTR>(byte_len));
#endif
}

}  // namespace chaos::il2cpp::runtime_core