#include "host_main.h"

extern "C" int chaos_android_startup_smoke_entry()
{
    return il2cpp_host_run(0, nullptr);
}
