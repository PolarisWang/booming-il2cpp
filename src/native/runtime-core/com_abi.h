#ifndef CHAOS_IL2CPP_COM_ABI_H_
#define CHAOS_IL2CPP_COM_ABI_H_

#include <cstdint>
#include <chaos/native_types.h>

namespace chaos::il2cpp::com_abi {

// ── IUnknown Vtbl (Win32 COM ABI) ──
// https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nn-unknwn-iunknown
struct IUnknownVtbl {
    int32_t  (*QueryInterface)(void*, const void*, void**);
    uint32_t (*AddRef)(void*);
    uint32_t (*Release)(void*);
};

static_assert(sizeof(IUnknownVtbl) == 3 * sizeof(void*),
    "IUnknownVtbl must be exactly 3 pointers (QueryInterface, AddRef, Release)");

// ── IDispatch Vtbl (IUnknown + 4 IDispatch methods) ──
// https://learn.microsoft.com/en-us/windows/win32/api/oaidl/nn-oaidl-idispatch
struct IDispatchVtbl {
    int32_t  (*QueryInterface)(void*, const void*, void**);
    uint32_t (*AddRef)(void*);
    uint32_t (*Release)(void*);
    int32_t  (*GetTypeInfoCount)(void*, uint32_t*);
    int32_t  (*GetTypeInfo)(void*, uint32_t, uint32_t, void**);
    int32_t  (*GetIDsOfNames)(void*, const void*, void**, uint32_t, uint32_t, int32_t*);
    int32_t  (*Invoke)(void*, int32_t, const void*, uint32_t, uint16_t, void*, void*, void*, uint32_t*);
};

static_assert(sizeof(IDispatchVtbl) == 7 * sizeof(void*),
    "IDispatchVtbl must be exactly 7 pointers (3 IUnknown + 4 IDispatch)");

// ── DISPPARAMS (parameter array for IDispatch::Invoke) ──
// https://learn.microsoft.com/en-us/windows/win32/api/oaidl/ns-oaidl-dispparams
// Note: rgvarg points to an array of VARIANT (16 bytes each) in REVERSE order:
//   rgvarg[0] = last parameter, rgvarg[cArgs-1] = first parameter.
struct DISPPARAMS {
    void*    rgvarg;            // Pointer to array of VARIANT (16 bytes each, reverse order)
    int32_t* rgdispidNamedArgs; // Array of DISPIDs for named arguments
    uint32_t cArgs;             // Number of arguments
    uint32_t cNamedArgs;        // Number of named arguments
};

// ── EXCEPINFO (exception info for IDispatch::Invoke) ──
// https://learn.microsoft.com/en-us/windows/win32/api/oaidl/ns-oaidl-excepinfo
struct EXCEPINFO {
    uint16_t wCode;
    uint16_t wReserved;
    void*    bstrSource;        // BSTR
    void*    bstrDescription;   // BSTR
    void*    bstrHelpFile;      // BSTR
    uint32_t dwHelpContext;
    void*    pvReserved;
    void*    pfnDeferredFillIn;
    int32_t  scode;
};

// ── COM standard GUIDs (16-byte) ──

// IID_IDispatch: {00020400-0000-0000-C000-000000000046}
static constexpr CHAOS_IL2CPP_UINT8 kIidIDispatch[16] = {
    0x00, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46
};

// IID_ITypeInfo: {00020401-0000-0000-C000-000000000046}
static constexpr CHAOS_IL2CPP_UINT8 kIidITypeInfo[16] = {
    0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46
};

// IID_ITypeLib: {00020402-0000-0000-C000-000000000046}
static constexpr CHAOS_IL2CPP_UINT8 kIidITypeLib[16] = {
    0x02, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46
};

}  // namespace chaos::il2cpp::com_abi

#endif  // CHAOS_IL2CPP_COM_ABI_H_
