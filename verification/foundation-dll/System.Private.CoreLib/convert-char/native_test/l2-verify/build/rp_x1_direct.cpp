
#include <cstdio>
#include <chaos/native_types.h>
extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_char(CHAOS_IL2CPP_INTPTR value);
int main() {
    printf("calling...\n"); fflush(stdout);
    auto r = chaos_convert_tochar_char(65);
    printf("got %u\n", (unsigned)r); fflush(stdout);
    return (r == 65) ? 0 : 1;
}
