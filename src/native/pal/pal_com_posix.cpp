// pal_com_posix.cpp — POSIX COM no-op stubs

#include <chaos/pal/pal_com.h>

namespace chaos::il2cpp::pal {

int32_t PalComQueryInterface(void* /*unknown*/, const void* /*iid*/, void** /*ppv*/) {
    return -1;  // E_NOTIMPL
}

uint32_t PalComAddRef(void* /*unknown*/) {
    return 0;
}

uint32_t PalComRelease(void* /*unknown*/) {
    return 0;
}

void* PalComCoCreateInstance(const uint8_t* /*clsid_bytes*/,
                              const uint8_t* /*iid_bytes*/) {
    return nullptr;
}

void* PalComCoCreateInstanceAggregated(const uint8_t* /*clsid_bytes*/,
                                        const uint8_t* /*iid_bytes*/,
                                        void* /*outer_unknown*/) {
    return nullptr;
}

void* PalComSysAllocString(const uint16_t* /*str*/) {
    return nullptr;
}

uint32_t PalComSysStringLen(void* /*bstr*/) {
    return 0;
}

void PalComSysFreeString(void* /*bstr*/) {
}

// ── COM apartment management ────────────────────────────────────────

int32_t PalComInitialize(int32_t /*apartment_type*/) noexcept {
    return 0;  // S_OK
}

void PalComUninitialize() noexcept {
}

}  // namespace chaos::il2cpp::pal
