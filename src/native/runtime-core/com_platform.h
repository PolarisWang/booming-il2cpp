#ifndef CHAOS_IL2CPP_COM_PLATFORM_H_
#define CHAOS_IL2CPP_COM_PLATFORM_H_

#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_core::com_platform {

// ── IUnknown vtable operations ─────────────────────────────────────────
// These abstract the first three vtable slots (QueryInterface, AddRef, Release)
// so that COM interop code does not need #if defined(_WIN32) at each call site.
// On Win32: delegates to the real IUnknown vtable.
// On non-Win32: returns E_NOTIMPL / 0 (stub).

/// Call IUnknown::QueryInterface on @a unknown.
/// @returns HRESULT (S_OK on success, E_NOINTERFACE/E_POINTER on failure).
CHAOS_IL2CPP_INT32 PlatformQueryInterface(void* unknown, const void* iid, void** ppv);

/// Call IUnknown::AddRef on @a unknown.
/// @returns the new reference count.
CHAOS_IL2CPP_UINT32 PlatformAddRef(void* unknown);

/// Call IUnknown::Release on @a unknown.
/// @returns the remaining reference count.
CHAOS_IL2CPP_UINT32 PlatformRelease(void* unknown);

// ── CoCreateInstance ───────────────────────────────────────────────────
// Abstracts ::CoCreateInstance for CLSID/IID byte arrays.

/// Create a COM object from CLSID/IID byte arrays (16 bytes each).
/// @returns IUnknown pointer, or nullptr on failure.
void* PlatformCoCreateInstance(const CHAOS_IL2CPP_UINT8* clsid_bytes,
                                const CHAOS_IL2CPP_UINT8* iid_bytes);

/// Aggregated variant — @a outer_unknown is the controlling IUnknown.
void* PlatformCoCreateInstanceAggregated(const CHAOS_IL2CPP_UINT8* clsid_bytes,
                                          const CHAOS_IL2CPP_UINT8* iid_bytes,
                                          void* outer_unknown);

// ── BSTR helpers ───────────────────────────────────────────────────────
// On Win32: dynamically resolve SysAllocString/SysFreeString/SysStringLen
// from oleaut32.dll at runtime.  On non-Win32: return 0 / nullptr (no-op).

/// Allocate a BSTR from a UTF-16 string (null-terminated).
/// @returns BSTR pointer, or nullptr on failure.
void* PlatformSysAllocString(const CHAOS_IL2CPP_UINT16* str);

/// Return the character length of a BSTR (excluding null terminator).
/// @returns character count, or 0 for null/invalid BSTR.
CHAOS_IL2CPP_UINT32 PlatformSysStringLen(void* bstr);

/// Free a BSTR previously allocated by PlatformSysAllocString.
void PlatformSysFreeString(void* bstr);

}  // namespace chaos::il2cpp::runtime_core::com_platform

#endif  // CHAOS_IL2CPP_COM_PLATFORM_H_
