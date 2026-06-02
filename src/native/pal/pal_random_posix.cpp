// pal_random_posix.cpp — POSIX /dev/urandom-based random byte generation

#include <chaos/pal/pal_random.h>

#include <cstdio>
#include <cstring>

namespace chaos::il2cpp::pal {

/// Simple xorshift32 fallback when /dev/urandom is unavailable.
static uint32_t XorShift32() noexcept {
    static thread_local uint32_t s_state = 0x6C078965;  // arbitrary seed
    uint32_t x = s_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    s_state = x;
    return x;
}

bool PalRandomBytes(void* buffer, size_t count) noexcept {
    auto* buf = static_cast<uint8_t*>(buffer);

    std::FILE* fp = std::fopen("/dev/urandom", "rb");
    if (fp) {
        size_t nread = std::fread(buf, 1, count, fp);
        std::fclose(fp);
        if (nread == count) return true;
    }

    // Fallback: use xorshift32 if /dev/urandom failed.
    for (size_t i = 0; i < count; i += 4) {
        uint32_t v = XorShift32();
        size_t remaining = count - i;
        if (remaining >= 4) {
            std::memcpy(buf + i, &v, 4);
        } else {
            std::memcpy(buf + i, &v, remaining);
        }
    }
    return false;  // /dev/urandom failed, used fallback
}

}  // namespace chaos::il2cpp::pal
