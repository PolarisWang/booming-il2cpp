// crt_stubs.cpp -- Stubs for MSVC 19.42+ CRT/STL symbols
// referenced by prebuilt chaos_runtime_core.lib.
#include <windows.h>
#include <cstring>
extern "C" int __cdecl _Thrd_sleep_for(const void* duration, void* remaining) {
    const auto* ts = static_cast<const long*>(duration);
    if (!ts) return -1;
    DWORD ms = static_cast<DWORD>(ts[0] * 1000 + ts[1] / 1000000);
    if (ms == 0 && (ts[0] > 0 || ts[1] > 0)) ms = 1;
    Sleep(ms);
    if (remaining) std::memset(remaining, 0, sizeof(long) * 2);
    return 0;
}
extern "C" int __cdecl _Cnd_timedwait_for_unchecked(void*, void*, const void*) {
    Sleep(1); return 0;
}
extern "C" const unsigned char* __cdecl __std_find_last_trivial_1(
    const unsigned char* first, const unsigned char* last, unsigned char val) {
    const unsigned char* it = last;
    while (it != first) { --it; if (*it == val) return it; }
    return last;
}
extern "C" const unsigned char* __cdecl __std_find_end_1(
    const unsigned char* hf, const unsigned char* hl,
    const unsigned char* nf, size_t nsz) {
    if (nsz == 0 || (size_t)(hl - hf) < nsz) return hl;
    const unsigned char* r = hl;
    const unsigned char* he = hl - nsz + 1;
    for (const unsigned char* it = hf; it < he; ++it)
        if (std::memcmp(it, nf, nsz) == 0) r = it;
    return r;
}
