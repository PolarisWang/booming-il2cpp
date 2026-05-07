
#include <cstdio>
#include <chaos/native_types.h>
extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_char(CHAOS_IL2CPP_INTPTR value);
extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Char_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
    return chaos_convert_tochar_char(chaos_fn_arg_0);
}
int main() {
    printf("calling...\n"); fflush(stdout);
    auto r = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Char_(65);
    printf("got %u\n", (unsigned)r); fflush(stdout);
    return (r == 65) ? 0 : 1;
}
