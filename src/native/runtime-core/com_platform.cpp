#include "com_platform.h"

#include <cstring>

#if defined(_WIN32)
#include <objbase.h>   // IUnknown, CLSCTX_*, ::CoCreateInstance
#include <oleauto.h>   // SysAllocString, SysFreeString, SysStringLen
#endif

namespace chaos::il2cpp::runtime_core::com_platform {

// ── IUnknown vtable operations ─────────────────────────────────────────

// On all platforms, the IUnknown vtable has the same layout:
//   vtbl[0] = QueryInterface
//   vtbl[1] = AddRef
//   vtbl[2] = Release
// The abstracted functions below call through this vtable.

CHAOS_IL2CPP_INT32 PlatformQueryInterface(void* unknown, const void* iid, void** ppv) {
#if defined(_WIN32)
    if (unknown == nullptr || iid == nullptr || ppv == nullptr) return -1; // E_POINTER
    // Read vtable pointer and call slot 0 (QueryInterface).
    auto*** vtbl_ptr = static_cast<void***>(unknown);
    using QiFn = CHAOS_IL2CPP_INT32 (*)(void*, const void*, void**);
    auto qi = reinterpret_cast<QiFn>((*vtbl_ptr)[0]);
    return qi(unknown, iid, ppv);
#else
    (void)unknown;
    (void)iid;
    (void)ppv;
    return -1; // E_NOTIMPL
#endif
}

CHAOS_IL2CPP_UINT32 PlatformAddRef(void* unknown) {
#if defined(_WIN32)
    if (unknown == nullptr) return 0;
    auto*** vtbl_ptr = static_cast<void***>(unknown);
    using AddRefFn = CHAOS_IL2CPP_UINT32 (*)(void*);
    auto addref = reinterpret_cast<AddRefFn>((*vtbl_ptr)[1]);
    return addref(unknown);
#else
    (void)unknown;
    return 0;
#endif
}

CHAOS_IL2CPP_UINT32 PlatformRelease(void* unknown) {
#if defined(_WIN32)
    if (unknown == nullptr) return 0;
    auto*** vtbl_ptr = static_cast<void***>(unknown);
    using ReleaseFn = CHAOS_IL2CPP_UINT32 (*)(void*);
    auto release = reinterpret_cast<ReleaseFn>((*vtbl_ptr)[2]);
    return release(unknown);
#else
    (void)unknown;
    return 0;
#endif
}

// ── CoCreateInstance ───────────────────────────────────────────────────

void* PlatformCoCreateInstance(const CHAOS_IL2CPP_UINT8* clsid_bytes,
                                const CHAOS_IL2CPP_UINT8* iid_bytes) {
#if defined(_WIN32)
    if (clsid_bytes == nullptr || iid_bytes == nullptr) return nullptr;
    GUID clsid;
    GUID iid;
    std::memcpy(&clsid, clsid_bytes, sizeof(GUID));
    std::memcpy(&iid, iid_bytes, sizeof(GUID));
    IUnknown* p_unknown = nullptr;
    HRESULT hr = ::CoCreateInstance(
        clsid, nullptr, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
        iid, reinterpret_cast<void**>(&p_unknown));
    return SUCCEEDED(hr) ? p_unknown : nullptr;
#else
    (void)clsid_bytes;
    (void)iid_bytes;
    return nullptr;
#endif
}

void* PlatformCoCreateInstanceAggregated(const CHAOS_IL2CPP_UINT8* clsid_bytes,
                                          const CHAOS_IL2CPP_UINT8* iid_bytes,
                                          void* outer_unknown) {
#if defined(_WIN32)
    if (clsid_bytes == nullptr || iid_bytes == nullptr) return nullptr;
    GUID clsid;
    GUID iid;
    std::memcpy(&clsid, clsid_bytes, sizeof(GUID));
    std::memcpy(&iid, iid_bytes, sizeof(GUID));
    IUnknown* p_unknown = nullptr;
    HRESULT hr = ::CoCreateInstance(
        clsid, static_cast<IUnknown*>(outer_unknown),
        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
        iid, reinterpret_cast<void**>(&p_unknown));
    return SUCCEEDED(hr) ? p_unknown : nullptr;
#else
    (void)clsid_bytes;
    (void)iid_bytes;
    (void)outer_unknown;
    return nullptr;
#endif
}

// ── BSTR helpers ───────────────────────────────────────────────────────

#if defined(_WIN32)
// Resolve oleaut32 functions at runtime (no static link dependency).
// Windows-only; non-Win32 stubs do nothing.
namespace {

template<typename Fn>
Fn ResolveOleAut32Proc(const char* name) noexcept {
    static HMODULE s_mod = ::GetModuleHandleW(L"oleaut32.dll");
    if (s_mod == nullptr) {
        s_mod = ::LoadLibraryW(L"oleaut32.dll");
    }
    return s_mod ? reinterpret_cast<Fn>(::GetProcAddress(s_mod, name)) : nullptr;
}

using SysAllocStringFn = BSTR(__stdcall*)(const OLECHAR*);
using SysStringLenFn = UINT(__stdcall*)(BSTR);
using SysFreeStringFn = void(__stdcall*)(BSTR);

SysAllocStringFn GetSysAllocString() {
    static SysAllocStringFn fn = ResolveOleAut32Proc<SysAllocStringFn>("SysAllocString");
    return fn;
}

SysStringLenFn GetSysStringLen() {
    static SysStringLenFn fn = ResolveOleAut32Proc<SysStringLenFn>("SysStringLen");
    return fn;
}

SysFreeStringFn GetSysFreeString() {
    static SysFreeStringFn fn = ResolveOleAut32Proc<SysFreeStringFn>("SysFreeString");
    return fn;
}

}  // anonymous namespace
#endif

void* PlatformSysAllocString(const CHAOS_IL2CPP_UINT16* str) {
#if defined(_WIN32)
    auto fn = GetSysAllocString();
    return fn ? (*fn)(reinterpret_cast<const OLECHAR*>(str)) : nullptr;
#else
    (void)str;
    return nullptr;
#endif
}

CHAOS_IL2CPP_UINT32 PlatformSysStringLen(void* bstr) {
#if defined(_WIN32)
    auto fn = GetSysStringLen();
    return fn ? (*fn)(static_cast<BSTR>(bstr)) : 0;
#else
    (void)bstr;
    return 0;
#endif
}

void PlatformSysFreeString(void* bstr) {
#if defined(_WIN32)
    auto fn = GetSysFreeString();
    if (fn) (*fn)(static_cast<BSTR>(bstr));
#else
    (void)bstr;
#endif
}

}  // namespace chaos::il2cpp::runtime_core::com_platform
