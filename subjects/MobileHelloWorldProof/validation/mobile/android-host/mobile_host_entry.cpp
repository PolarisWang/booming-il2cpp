#include "host_main.h"

extern "C" int chaos_mobile_hello_world_android_host_entry(void)
{
    const char* argv[] = {"MobileHelloWorldProof"};
    return il2cpp_host_run(1, argv);
}
