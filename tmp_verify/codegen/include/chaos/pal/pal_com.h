#ifndef CHAOS_IL2CPP_PAL_COM_H_
#define CHAOS_IL2CPP_PAL_COM_H_

// ── COM platform abstraction ─────────────────────────────────────────
// Abstracts Win32 COM operations (IUnknown vtable, CoCreateInstance,
// BSTR helpers) so that COM interop code does not need #if defined(_WIN32)
// at each call site.
//
// On Win32: delegates to the real IUnknown vtable and COM API.
// On non-Win32: returns E_NOTIMPL / 0 / nullptr (stub).

#include <cstdint>

namespace chaos::il2cpp::pal {

/// Call IUnknown::QueryInterface on @a unknown.
/// @returns HRESULT (S_OK on success, E_NOINTERFACE/E_POINTER on failure).
int32_t PalComQueryInterface(void* unknown, const void* iid, void** ppv);

/// Call IUnknown::AddRef on @a unknown.
/// @returns the new reference count.
uint32_t PalComAddRef(void* unknown);

/// Call IUnknown::Release on @a unknown.
/// @returns the remaining reference count.
uint32_t PalComRelease(void* unknown);

/// Create a COM object from CLSID/IID byte arrays (16 bytes each).
/// @returns IUnknown pointer, or nullptr on failure.
void* PalComCoCreateInstance(const uint8_t* clsid_bytes,
                              const uint8_t* iid_bytes);

/// Aggregated variant — @a outer_unknown is the controlling IUnknown.
void* PalComCoCreateInstanceAggregated(const uint8_t* clsid_bytes,
                                        const uint8_t* iid_bytes,
                                        void* outer_unknown);

/// Allocate a BSTR from a UTF-16 string (null-terminated).
/// @returns BSTR pointer, or nullptr on failure.
void* PalComSysAllocString(const uint16_t* str);

/// Return the character length of a BSTR (excluding null terminator).
/// @returns character count, or 0 for null/invalid BSTR.
uint32_t PalComSysStringLen(void* bstr);

/// Free a BSTR previously allocated by PalComSysAllocString.
void PalComSysFreeString(void* bstr);

// ── COM apartment management ────────────────────────────────────────

/// Initialize the COM apartment for the calling thread.
/// Win32: calls CoInitializeEx(nullptr, apartment_type).
/// POSIX: no-op, returns 0 (S_OK).
/// @param apartment_type  COINIT_APARTMENTTHREADED, COINIT_MULTITHREADED, etc.
/// @returns 0 (S_OK) on success, or a Win32 HRESULT on failure.
int32_t PalComInitialize(int32_t apartment_type) noexcept;

/// Uninitialize the COM apartment for the calling thread.
/// Win32: calls CoUninitialize().  POSIX: no-op.
void PalComUninitialize() noexcept;

}  // namespace chaos::il2cpp::pal

#endif  // CHAOS_IL2CPP_PAL_COM_H_
