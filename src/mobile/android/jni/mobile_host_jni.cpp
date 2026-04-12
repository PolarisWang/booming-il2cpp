#include <jni.h>

#include <string>

#include "host_main.h"
#include "hot_update.h"

namespace
{
chaos::il2cpp::hot_update::HotUpdatePackageHandle g_hot_update_package = {};

std::string jstring_to_utf8(JNIEnv* env, jstring value)
{
    if (env == nullptr || value == nullptr)
    {
        return {};
    }

    const char* utf8 = env->GetStringUTFChars(value, nullptr);
    if (utf8 == nullptr)
    {
        return {};
    }

    std::string result(utf8);
    env->ReleaseStringUTFChars(value, utf8);
    return result;
}
}

extern "C" JNIEXPORT jint JNICALL Java_com_chaos_mobilehost_MainActivity_runHost(JNIEnv*, jobject)
{
    return static_cast<jint>(il2cpp_host_run(0, nullptr));
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_chaos_mobilehost_MainActivity_loadHotUpdatePackage(
    JNIEnv* env,
    jobject,
    jstring package_root)
{
    const std::string package_root_utf8 = jstring_to_utf8(env, package_root);
    if (package_root_utf8.empty())
    {
        return JNI_FALSE;
    }

    return chaos::il2cpp::hot_update::LoadHotUpdatePackage(
        package_root_utf8.c_str(),
        &g_hot_update_package)
        ? JNI_TRUE
        : JNI_FALSE;
}

extern "C" JNIEXPORT void JNICALL Java_com_chaos_mobilehost_MainActivity_unloadHotUpdatePackage(JNIEnv*, jobject)
{
    chaos::il2cpp::hot_update::UnloadHotUpdatePackage(&g_hot_update_package);
}
