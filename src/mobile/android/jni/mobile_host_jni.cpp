#include <jni.h>

#include "host_main.h"

extern "C" JNIEXPORT jint JNICALL Java_com_chaos_mobilehost_MainActivity_runHost(JNIEnv*, jobject)
{
    return static_cast<jint>(il2cpp_host_run(0, nullptr));
}
