// pal_stubs.cpp — SDK-emitted PAL stubs for test builds.
// chaos_runtime_core.lib depends on chaos_pal.lib for platform-abstraction
// functions. Since chaos_pal.lib is not part of the SDK, these stubs provide
// no-op or minimal implementations sufficient for test entry points.
//
// On Win32, some functions delegate to the real Windows API so that GC,
// threading, and diagnostic paths can execute without crashing.
//
// NOTE: These stubs are NOT suitable for production use — they lack error
// handling, resource limits, and cross-platform correctness guarantees.

#include <cstddef>
#include <cstdint>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <objbase.h>

#include <chaos/pal/pal_com.h>
#include <chaos/pal/pal_dl.h>
#include <chaos/pal/pal_eh.h>
#include <chaos/pal/pal_encoding.h>
#include <chaos/pal/pal_error.h>
#include <chaos/pal/pal_etw.h>
#include <chaos/pal/pal_heap.h>
#include <chaos/pal/pal_low_mem.h>
#include <chaos/pal/pal_mem.h>
#include <chaos/pal/pal_cache.h>
#include <chaos/pal/pal_preempt.h>
#include <chaos/pal/pal_sync.h>
#include <chaos/pal/pal_thread.h>
#include <chaos/pal/pal_time.h>
#include <chaos/pal/pal_wakeable.h>

namespace chaos::il2cpp::pal {

// ── ETW stubs ─────────────────────────────────────────────────────────

PalEtwProvider* PalEtwRegister(const uint8_t provider_id[16]) noexcept {
    (void)provider_id;
    PAL_STUB_WARN(PalEtwRegister);
    return nullptr; // no-op: ETW not available in test builds
}

void PalEtwUnregister(PalEtwProvider* provider) noexcept {
    (void)provider; // no-op
}

bool PalEtwIsEnabled(PalEtwProvider* provider) noexcept {
    (void)provider;
    return false; // always disabled
}

bool PalEtwWrite(PalEtwProvider* provider, uint8_t event_id, uint8_t version,
                 uint8_t channel, uint8_t level, uint64_t keyword,
                 const void* data, uint32_t data_len) noexcept {
    (void)provider; (void)event_id; (void)version; (void)channel;
    (void)level; (void)keyword; (void)data; (void)data_len;
    PAL_STUB_WARN(PalEtwWrite);
    return true; // silently succeed
}

// ── Heap stubs ────────────────────────────────────────────────────────

void* PalHeapCreate() noexcept {
    return ::HeapCreate(0, 0, 0);
}

void* PalHeapAlloc(void* heap, size_t size) noexcept {
    return ::HeapAlloc(static_cast<HANDLE>(heap), 0, size);
}

void* PalHeapRealloc(void* heap, void* ptr, size_t new_size) noexcept {
    return ::HeapReAlloc(static_cast<HANDLE>(heap), 0, ptr, new_size);
}

void PalHeapFree(void* heap, void* ptr) noexcept {
    ::HeapFree(static_cast<HANDLE>(heap), 0, ptr);
}

size_t PalHeapSize(void* heap, void* ptr) noexcept {
    return ::HeapSize(static_cast<HANDLE>(heap), 0, ptr);
}

void PalHeapDestroy(void* heap) noexcept {
    ::HeapDestroy(static_cast<HANDLE>(heap));
}

// ── Cache stubs ───────────────────────────────────────────────────────

void PalFlushInstructionCache(void* addr, size_t size) noexcept {
    (void)addr; (void)size;
    ::FlushInstructionCache(::GetCurrentProcess(), addr, size);
}

// ── Memory management stubs ───────────────────────────────────────────

void* PalVirtualAlloc(size_t size) noexcept {
    return ::VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

bool PalVirtualFree(void* ptr, size_t size) noexcept {
    (void)size;
    return ::VirtualFree(ptr, 0, MEM_RELEASE) != FALSE;
}

bool PalVirtualProtect(void* ptr, size_t size, PalMemoryProtection prot) noexcept {
    DWORD old;
    DWORD winProt = PAGE_READWRITE;
    if (prot == kPalMemNone) winProt = PAGE_NOACCESS;
    else if (prot == kPalMemRead) winProt = PAGE_READONLY;
    else if (prot == kPalMemReadWrite) winProt = PAGE_READWRITE;
    else if (prot == kPalMemExec) winProt = PAGE_EXECUTE;
    else if (prot == kPalMemReadExec) winProt = PAGE_EXECUTE_READ;
    else if (prot == kPalMemReadWriteExec) winProt = PAGE_EXECUTE_READWRITE;
    return ::VirtualProtect(ptr, size, winProt, &old) != FALSE;
}

bool PalVirtualCommit(void* ptr, size_t size) noexcept {
    return ::VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != nullptr;
}

bool PalVirtualDecommit(void* ptr, size_t size) noexcept {
    return ::VirtualFree(ptr, size, MEM_DECOMMIT) != FALSE;
}

size_t PalGetPageSize() noexcept {
    SYSTEM_INFO si;
    ::GetSystemInfo(&si);
    return si.dwPageSize;
}

size_t PalGetLargePageSize() noexcept {
    return ::GetLargePageMinimum();
}

void* PalVirtualAllocLarge(size_t size) noexcept {
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
        return nullptr;
    tp.PrivilegeCount = 1;
    if (!::LookupPrivilegeValueW(nullptr, L"SeLockMemoryPrivilege", &tp.Privileges[0].Luid)) {
        ::CloseHandle(hToken);
        return nullptr;
    }
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    ::AdjustTokenPrivileges(hToken, FALSE, &tp, 0, nullptr, nullptr);
    ::CloseHandle(hToken);
    return ::VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
}

void PalGetMemoryStatus(PalMemoryStatus& out) noexcept {
    MEMORYSTATUSEX ms = { sizeof(MEMORYSTATUSEX) };
    if (::GlobalMemoryStatusEx(&ms)) {
        out.total_phys = static_cast<int64_t>(ms.ullTotalPhys);
        out.avail_phys = static_cast<int64_t>(ms.ullAvailPhys);
    } else {
        out.total_phys = 0;
        out.avail_phys = 0;
    }
}

bool PalVirtualAllocIsValid(const void* ptr) noexcept {
    MEMORY_BASIC_INFORMATION mbi;
    SIZE_T r = ::VirtualQuery(ptr, &mbi, sizeof(mbi));
    return r != 0 && mbi.State == MEM_COMMIT;
}

// ── COM stubs ─────────────────────────────────────────────────────────

int32_t PalComQueryInterface(void* unknown, const void* iid, void** ppv) {
    (void)unknown; (void)iid; (void)ppv;
    return 0x80004002; // E_NOINTERFACE
}

uint32_t PalComAddRef(void* unknown) {
    (void)unknown;
    return 1;
}

uint32_t PalComRelease(void* unknown) {
    (void)unknown;
    return 0;
}

void* PalComCoCreateInstance(const uint8_t* clsid_bytes,
                              const uint8_t* iid_bytes) {
    (void)clsid_bytes; (void)iid_bytes;
    return nullptr;
}

void* PalComCoCreateInstanceAggregated(const uint8_t* clsid_bytes,
                                        const uint8_t* iid_bytes,
                                        void* outer_unknown) {
    (void)clsid_bytes; (void)iid_bytes; (void)outer_unknown;
    return nullptr;
}

void* PalComSysAllocString(const uint16_t* str) {
    (void)str;
    return nullptr;
}

uint32_t PalComSysStringLen(void* bstr) {
    (void)bstr;
    return 0;
}

void PalComSysFreeString(void* bstr) {
    (void)bstr;
}

int32_t PalComInitialize(int32_t apartment_type) noexcept {
    (void)apartment_type;
    HRESULT hr = ::CoInitializeEx(nullptr, apartment_type);
    return static_cast<int32_t>(hr);
}

void PalComUninitialize() noexcept {
    ::CoUninitialize();
}

// ── SEH stub ──────────────────────────────────────────────────────────

bool PalTryCallNoExcept(uint64_t (*fn)(uint64_t, uint64_t, uint64_t, uint64_t,
                                        uint64_t, uint64_t, uint64_t, uint64_t),
                         uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
                         uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7,
                         uint64_t& out_result) noexcept {
    __try {
        out_result = fn(a0, a1, a2, a3, a4, a5, a6, a7);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return true; // hardware exception caught
    }
    return false;
}

// ── Low-memory monitor stubs ─────────────────────────────────────────

PalLowMemMonitor* PalLowMemCreate() noexcept {
    return nullptr; // not supported in test builds
}

int PalLowMemWait(PalLowMemMonitor* monitor, int timeout_ms) noexcept {
    (void)monitor; (void)timeout_ms;
    return 0; // timeout (no signal)
}

void PalLowMemSignalShutdown(PalLowMemMonitor* monitor) noexcept {
    (void)monitor;
}

void PalLowMemDestroy(PalLowMemMonitor* monitor) noexcept {
    (void)monitor;
}

// ── Event stubs ───────────────────────────────────────────────────────

PalEvent* PalEventCreate(bool initially_set, bool manual_reset) noexcept {
    HANDLE h = ::CreateEventW(nullptr, manual_reset ? TRUE : FALSE,
                               initially_set ? TRUE : FALSE, nullptr);
    return reinterpret_cast<PalEvent*>(h);
}

void PalEventDestroy(PalEvent* e) noexcept {
    if (e) ::CloseHandle(reinterpret_cast<HANDLE>(e));
}

void PalEventSet(PalEvent* e) noexcept {
    ::SetEvent(reinterpret_cast<HANDLE>(e));
}

void PalEventReset(PalEvent* e) noexcept {
    ::ResetEvent(reinterpret_cast<HANDLE>(e));
}

bool PalEventWait(PalEvent* e, uint64_t timeout_ms) noexcept {
    DWORD ms = (timeout_ms == UINT64_MAX) ? INFINITE : static_cast<DWORD>(timeout_ms);
    return ::WaitForSingleObject(reinterpret_cast<HANDLE>(e), ms) == WAIT_OBJECT_0;
}

int32_t PalEventWaitAny(PalEvent* const* events, size_t count, uint64_t timeout_ms) noexcept {
    DWORD ms = (timeout_ms == UINT64_MAX) ? INFINITE : static_cast<DWORD>(timeout_ms);
    DWORD result = ::WaitForMultipleObjects(
        static_cast<DWORD>(count),
        reinterpret_cast<const HANDLE*>(events),
        FALSE, ms);
    if (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + count)
        return static_cast<int32_t>(result - WAIT_OBJECT_0);
    return -1;
}

int32_t PalEventWaitAll(PalEvent* const* events, size_t count, uint64_t timeout_ms) noexcept {
    DWORD ms = (timeout_ms == UINT64_MAX) ? INFINITE : static_cast<DWORD>(timeout_ms);
    DWORD result = ::WaitForMultipleObjects(
        static_cast<DWORD>(count),
        reinterpret_cast<const HANDLE*>(events),
        TRUE, ms);
    return (result == WAIT_OBJECT_0) ? 0 : -1;
}

// ── Thread stubs ──────────────────────────────────────────────────────

uint64_t PalGetCurrentThreadId() noexcept {
    return static_cast<uint64_t>(::GetCurrentThreadId());
}

int32_t PalGetCurrentProcessId() noexcept {
    return static_cast<int32_t>(::GetCurrentProcessId());
}

intptr_t PalGetCurrentProcess() noexcept {
    return reinterpret_cast<intptr_t>(::GetCurrentProcess());
}

intptr_t PalGetCurrentThread() noexcept {
    return reinterpret_cast<intptr_t>(::GetCurrentThread());
}

void* PalDuplicateCurrentThreadHandle() noexcept {
    HANDLE hProcess = ::GetCurrentProcess();
    HANDLE hDup = nullptr;
    ::DuplicateHandle(hProcess, ::GetCurrentThread(), hProcess, &hDup,
                      0, FALSE, DUPLICATE_SAME_ACCESS);
    return hDup;
}

// ── Error stubs ────────────────────────────────────────────────────────

int32_t PalGetLastError() noexcept {
    return static_cast<int32_t>(::GetLastError());
}

void PalSetLastError(int32_t error) noexcept {
    ::SetLastError(static_cast<DWORD>(error));
}

// ── Encoding stubs ─────────────────────────────────────────────────────

int32_t PalUtf8ToUtf16(const uint8_t* utf8, int32_t utf8_len,
                       uint16_t* utf16, int32_t utf16_capacity) noexcept {
    return ::MultiByteToWideChar(CP_UTF8, 0,
        reinterpret_cast<LPCCH>(utf8), utf8_len,
        reinterpret_cast<LPWSTR>(utf16), utf16_capacity);
}

int32_t PalUtf16ToUtf8(const uint16_t* utf16, int32_t utf16_len,
                       uint8_t* utf8, int32_t utf8_capacity) noexcept {
    return ::WideCharToMultiByte(CP_UTF8, 0,
        reinterpret_cast<LPCWSTR>(utf16), utf16_len,
        reinterpret_cast<LPSTR>(utf8), utf8_capacity, nullptr, nullptr);
}

bool PalCloseHandle(intptr_t handle) noexcept {
    return ::CloseHandle(reinterpret_cast<HANDLE>(handle)) != FALSE;
}

// ── Dynamic library stubs ──────────────────────────────────────────────

bool PalDlClose(PalLibraryPtr handle) noexcept {
    return ::FreeLibrary(reinterpret_cast<HMODULE>(handle)) != FALSE;
}

// ── Preemption stubs ───────────────────────────────────────────────────

void PalPreemptInit(PalPreemptCallback callback) noexcept {
    (void)callback; // no-op: preemptive suspend not available in test builds
}

bool PalPreemptRequest(void* os_handle, uint64_t os_thread_id, uint64_t epoch) noexcept {
    (void)os_handle; (void)os_thread_id; (void)epoch;
    return false; // no-op
}

// ── Time stubs ────────────────────────────────────────────────────────

uint64_t PalGetProcessCpuTimeNs() noexcept {
    FILETIME create, exit, kernel, user;
    ::GetProcessTimes(::GetCurrentProcess(), &create, &exit, &kernel, &user);
    uint64_t k = (static_cast<uint64_t>(kernel.dwHighDateTime) << 32)
               | static_cast<uint64_t>(kernel.dwLowDateTime);
    uint64_t u = (static_cast<uint64_t>(user.dwHighDateTime) << 32)
               | static_cast<uint64_t>(user.dwLowDateTime);
    return (k + u) * 100; // 100-ns intervals → ns
}

// ── Wakeable stubs ────────────────────────────────────────────────────

PalWakeable* PalWakeableCreate() noexcept {
    HANDLE iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
    return reinterpret_cast<PalWakeable*>(iocp);
}

bool PalWakeablePost(PalWakeable* w, void (*callback)(void*), void* context) noexcept {
    // Pack callback+context into the completion key
    return ::PostQueuedCompletionStatus(
        reinterpret_cast<HANDLE>(w), 0,
        reinterpret_cast<ULONG_PTR>(callback),
        reinterpret_cast<LPOVERLAPPED>(context)) != FALSE;
}

int PalWakeableWait(PalWakeable* w, int timeout_ms) noexcept {
    DWORD bytes;
    ULONG_PTR key;
    LPOVERLAPPED ovl;
    BOOL ok = ::GetQueuedCompletionStatus(
        reinterpret_cast<HANDLE>(w), &bytes, &key, &ovl,
        (timeout_ms < 0) ? INFINITE : static_cast<DWORD>(timeout_ms));
    if (!ok && ::GetLastError() == WAIT_TIMEOUT)
        return 0;
    // If a callback was packed as the key, call it
    if (key) {
        auto cb = reinterpret_cast<void (*)(void*)>(key);
        cb(ovl);
    }
    return 1;
}

void PalWakeableDestroy(PalWakeable* w) noexcept {
    if (w) ::CloseHandle(reinterpret_cast<HANDLE>(w));
}

// ── Miscellaneous stubs ───────────────────────────────────────────────

void PalGetStackBounds(void*& low, void*& high) noexcept {
    NT_TIB* tib = reinterpret_cast<NT_TIB*>(::NtCurrentTeb());
    low = tib->StackLimit;
    high = tib->StackBase;
}

void PalYield() noexcept {
    ::SwitchToThread();
}

} // namespace chaos::il2cpp::pal

// ── Hotpatch stubs ────────────────────────────────────────────────────
// The runtime entry references ApplyPatchFromMemoryEx when --hotupdate or
// --patch-data flags are present.  In test builds without the full interpreter
// VM and patch loader infrastructure, provide a no-op stub that returns
// nullptr (no patches applied).

#include "patch_loader.h"

namespace chaos::il2cpp::runtime_core {

PatchContext* ApplyPatchFromMemoryEx(
    const void* data, size_t size,
    const char* host_type_ns,
    const char* const* host_type_names,
    const char* const* host_method_names,
    int method_count) noexcept {
    (void)data; (void)size; (void)host_type_ns;
    (void)host_type_names; (void)host_method_names; (void)method_count;
    return nullptr; // no-op: hotpatch not available in test builds
}

} // namespace chaos::il2cpp::runtime_core

// ── C-linkage GC helpers ─────────────────────────────────────────────
// These are declared extern "C" in the GC code and are not part of
// chaos_runtime_core.lib.  Provide minimal stubs for test builds.

extern "C" unsigned __int64 GcGetLargePageMinimum(void) {
    return 0; // no large page support
}

extern "C" void* GcTryAllocLargePages(unsigned __int64 size) {
    (void)size;
    return nullptr; // not supported
}

