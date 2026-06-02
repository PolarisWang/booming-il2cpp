// pal_etw_posix.cpp — POSIX ETW no-op stubs

#include <chaos/pal/pal_etw.h>

namespace chaos::il2cpp::pal {

PalEtwProvider* PalEtwRegister(const uint8_t /*provider_id*/[16]) noexcept {
    return nullptr;  // ETW is Windows-only.
}

void PalEtwUnregister(PalEtwProvider* /*provider*/) noexcept {
    // No-op.
}

bool PalEtwIsEnabled(PalEtwProvider* /*provider*/) noexcept {
    return false;
}

bool PalEtwWrite(PalEtwProvider* /*provider*/,
                 uint8_t /*event_id*/, uint8_t /*version*/, uint8_t /*channel*/,
                 uint8_t /*level*/, uint64_t /*keyword*/,
                 const void* /*data*/, uint32_t /*data_len*/) noexcept
{
    return true;  // Silently succeed on POSIX.
}

}  // namespace chaos::il2cpp::pal
