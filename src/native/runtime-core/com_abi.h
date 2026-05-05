#ifndef CHAOS_IL2CPP_COM_ABI_H_
#define CHAOS_IL2CPP_COM_ABI_H_

#include <cstdint>

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

}  // namespace chaos::il2cpp::com_abi

#endif  // CHAOS_IL2CPP_COM_ABI_H_
