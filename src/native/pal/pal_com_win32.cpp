// pal_com_win32.cpp — Win32 COM implementation

#include <chaos/pal/pal_com.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601  // Windows 7+
#endif
#include <windows.h>
#include <objbase.h>   // IUnknown, CLSCTX_*, ::CoCreateInstance
#include <oleauto.h>   // SysAllocString, SysFreeString, SysStringLen

#include <cstring>

namespace chaos::il2cpp::pal {

// ── IUnknown vtable operations ─────────────────────────────────────────

int32_t PalComQueryInterface(void* unknown, const void* iid, void** ppv) {
    if (unknown == nullptr || iid == nullptr || ppv == nullptr) return -1; // E_POINTER
    auto*** vtbl_ptr = static_cast<void***>(unknown);
    using QiFn = int32_t (*)(void*, const void*, void**);
    auto qi = reinterpret_cast<QiFn>((*vtbl_ptr)[0]);
    return qi(unknown, iid, ppv);
}

uint32_t PalComAddRef(void* unknown) {
    if (unknown == nullptr) return 0;
    auto*** vtbl_ptr = static_cast<void***>(unknown);
    using AddRefFn = uint32_t (*)(void*);
    auto addref = reinterpret_cast<AddRefFn>((*vtbl_ptr)[1]);
    return addref(unknown);
}

uint32_t PalComRelease(void* unknown) {
    if (unknown == nullptr) return 0;
    auto*** vtbl_ptr = static_cast<void***>(unknown);
    using ReleaseFn = uint32_t (*)(void*);
    auto release = reinterpret_cast<ReleaseFn>((*vtbl_ptr)[2]);
    return release(unknown);
}

// ── CoCreateInstance ───────────────────────────────────────────────────

void* PalComCoCreateInstance(const uint8_t* clsid_bytes,
                              const uint8_t* iid_bytes) {
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
}

void* PalComCoCreateInstanceAggregated(const uint8_t* clsid_bytes,
                                        const uint8_t* iid_bytes,
                                        void* outer_unknown) {
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
}

// ── BSTR helpers ───────────────────────────────────────────────────────

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

void* PalComSysAllocString(const uint16_t* str) {
    auto fn = GetSysAllocString();
    return fn ? (*fn)(reinterpret_cast<const OLECHAR*>(str)) : nullptr;
}

uint32_t PalComSysStringLen(void* bstr) {
    auto fn = GetSysStringLen();
    return fn ? (*fn)(static_cast<BSTR>(bstr)) : 0;
}

void PalComSysFreeString(void* bstr) {
    auto fn = GetSysFreeString();
    if (fn) (*fn)(static_cast<BSTR>(bstr));
}

// ── COM apartment management ────────────────────────────────────────

int32_t PalComInitialize(int32_t apartment_type) noexcept {
    HRESULT hr = ::CoInitializeEx(nullptr, static_cast<DWORD>(apartment_type));
    return static_cast<int32_t>(hr);
}

void PalComUninitialize() noexcept {
    ::CoUninitialize();
}

}  // namespace chaos::il2cpp::pal
