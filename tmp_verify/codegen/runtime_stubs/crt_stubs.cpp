// crt_stubs.cpp -- Stubs for MSVC 19.42+ CRT/STL symbols
// referenced by prebuilt chaos_runtime_core.lib.
#include <windows.h>
#include <cstring>
// _Thrd_sleep_for: MSVC 19.42+ passes ms directly (unsigned long),
// NOT a struct timespec pointer.  x64 ABI: 4-byte ulong in register.
extern "C" void __stdcall _Thrd_sleep_for(unsigned long ms) {
    if (ms == 0) ms = 1;
    Sleep(ms);
}
// _Cnd_timedwait_for_unchecked: MSVC 19.42+ passes ms as unsigned int.
extern "C" int __stdcall _Cnd_timedwait_for_unchecked(void*, void*, unsigned int ms) {
    Sleep(ms > 0 ? ms : 1);
    return 0;  // _Thrd_result::_Success
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
