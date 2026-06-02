// pal_random_win32.cpp — Win32 BCryptGenRandom implementation

#include <chaos/pal/pal_random.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>

namespace chaos::il2cpp::pal {

bool PalRandomBytes(void* buffer, size_t count) noexcept {
    NTSTATUS status = ::BCryptGenRandom(
        nullptr,
        static_cast<PUCHAR>(buffer),
        static_cast<ULONG>(count),
        BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    return status >= 0;
}

}  // namespace chaos::il2cpp::pal
