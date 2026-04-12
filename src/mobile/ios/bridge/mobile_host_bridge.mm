#include "host_main.h"
#include "hot_update.h"

namespace
{
chaos::il2cpp::hot_update::HotUpdatePackageHandle g_hot_update_package = {};
}

extern "C" int chaos_mobile_ios_run_host(void)
{
    return il2cpp_host_run(0, nullptr);
}

extern "C" bool chaos_mobile_ios_load_hot_update_package(const char* package_root_utf8)
{
    if (package_root_utf8 == nullptr || package_root_utf8[0] == '\0')
    {
        return false;
    }

    return chaos::il2cpp::hot_update::LoadHotUpdatePackage(
        package_root_utf8,
        &g_hot_update_package);
}

extern "C" void chaos_mobile_ios_unload_hot_update_package(void)
{
    chaos::il2cpp::hot_update::UnloadHotUpdatePackage(&g_hot_update_package);
}
