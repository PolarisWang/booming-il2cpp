#ifndef CHAOS_IL2CPP_COM_PLATFORM_H_
#define CHAOS_IL2CPP_COM_PLATFORM_H_

// ── COM platform abstraction — backward-compat header ─────────────────
// This header forwards the legacy runtime_core::com_platform::Platform*
// names to the new PAL equivalents (chaos::il2cpp::pal::PalCom*).
//
// Consumers being migrated to PAL should include <chaos/pal/pal_com.h>
// directly and use chaos::il2cpp::pal::PalCom* names.

#include <chaos/pal/pal_com.h>

namespace chaos::il2cpp::runtime_core::com_platform {

inline CHAOS_IL2CPP_INT32 PlatformQueryInterface(void* unknown, const void* iid, void** ppv) {
    return chaos::il2cpp::pal::PalComQueryInterface(unknown, iid, ppv);
}

inline CHAOS_IL2CPP_UINT32 PlatformAddRef(void* unknown) {
    return chaos::il2cpp::pal::PalComAddRef(unknown);
}

inline CHAOS_IL2CPP_UINT32 PlatformRelease(void* unknown) {
    return chaos::il2cpp::pal::PalComRelease(unknown);
}

inline void* PlatformCoCreateInstance(const CHAOS_IL2CPP_UINT8* clsid_bytes,
                                       const CHAOS_IL2CPP_UINT8* iid_bytes) {
    return chaos::il2cpp::pal::PalComCoCreateInstance(clsid_bytes, iid_bytes);
}

inline void* PlatformCoCreateInstanceAggregated(const CHAOS_IL2CPP_UINT8* clsid_bytes,
                                                  const CHAOS_IL2CPP_UINT8* iid_bytes,
                                                  void* outer_unknown) {
    return chaos::il2cpp::pal::PalComCoCreateInstanceAggregated(clsid_bytes, iid_bytes, outer_unknown);
}

inline void* PlatformSysAllocString(const CHAOS_IL2CPP_UINT16* str) {
    return chaos::il2cpp::pal::PalComSysAllocString(str);
}

inline CHAOS_IL2CPP_UINT32 PlatformSysStringLen(void* bstr) {
    return chaos::il2cpp::pal::PalComSysStringLen(bstr);
}

inline void PlatformSysFreeString(void* bstr) {
    chaos::il2cpp::pal::PalComSysFreeString(bstr);
}

}  // namespace chaos::il2cpp::runtime_core::com_platform

#endif  // CHAOS_IL2CPP_COM_PLATFORM_H_
