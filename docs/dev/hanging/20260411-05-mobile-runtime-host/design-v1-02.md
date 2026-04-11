# Design v1-02

## 范围

本版只覆盖 Android Host 的 2.1 / 2.2 / 2.3：

- 新增 `src/mobile/android/CMakeLists.txt`
- 新增 `src/mobile/android/jni/mobile_host_jni.cpp`
- 新增 `src/mobile/android/app/build.gradle.kts`
- 新增 `src/mobile/android/app/src/main/AndroidManifest.xml`
- 新增 `src/mobile/android/app/src/main/java/com/chaos/mobilehost/MainActivity.kt`

本版不做：

- root preset 改接 `src/mobile/android/`
- 真实 APK 打包
- Android 启动实机验证
- crash 收集

## Android skeleton contract

### `MainActivity.kt`

- 包名：
  - `com.chaos.mobilehost`
- 负责：
  - `System.loadLibrary("chaos_mobile_android_host")`
  - 暴露 `external fun runHost(): Int`
  - 在 `onCreate(...)` 中调用 `runHost()`

### `mobile_host_jni.cpp`

- 暴露 JNI 符号：
  - `Java_com_chaos_mobilehost_MainActivity_runHost`
- 该 JNI 入口直接转发：
  - `il2cpp_host_run(0, nullptr)`

### `src/mobile/android/CMakeLists.txt`

- 构建目标：
  - `chaos_mobile_android_host`
- 编译源：
  - `jni/mobile_host_jni.cpp`
  - `src/mobile/shared/host_main.cpp`
  - `src/mobile/shared/result_reporter.cpp`

## 验证策略

- 新增 `tests/unit/run/test_phase4_mobile_runtime_host_batch2_android.py`
- 先锁定：
  - Android app / jni / CMake 文件存在
  - `MainActivity` 会加载 `chaos_mobile_android_host`
  - JNI bridge 调用 `il2cpp_host_run(0, nullptr)`
  - Android CMake 复用 `src/mobile/shared/*`
