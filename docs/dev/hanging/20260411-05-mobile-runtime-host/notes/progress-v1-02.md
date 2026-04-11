# 进展记录 v1-02

## 时间

- 2026-04-11 14:22:48 +08:00

## 本轮完成

- 完成 Batch 1 shared host framework
  - 新增 `src/mobile/shared/host_main.h`
  - 新增 `src/mobile/shared/host_main.cpp`
  - 新增 `src/mobile/shared/result_reporter.h`
  - 新增 `src/mobile/shared/result_reporter.cpp`
  - 更新 `tests/gate/android-smoke/*`
  - 更新 `tests/gate/ios-smoke/*`
- 完成 Batch 2 Android skeleton 的 2.1 / 2.2 / 2.3
  - 新增 `src/mobile/android/CMakeLists.txt`
  - 新增 `src/mobile/android/jni/mobile_host_jni.cpp`
  - 新增 `src/mobile/android/app/build.gradle.kts`
  - 新增 `src/mobile/android/app/src/main/AndroidManifest.xml`
  - 新增 `src/mobile/android/app/src/main/java/com/chaos/mobilehost/MainActivity.kt`
- 完成 Batch 3 iOS skeleton 的 3.1 / 3.2 / 3.3
  - 新增 `src/mobile/ios/CMakeLists.txt`
  - 新增 `src/mobile/ios/bridge/mobile_host_bridge.mm`
  - 新增 `src/mobile/ios/app/AppDelegate.mm`
  - 新增 `src/mobile/ios/app/Info.plist`
- 补齐当前阶段的设计文档
  - 新增 `brainstorm-v1-02.md` / `design-v1-02.md`
  - 新增 `brainstorm-v1-03.md` / `design-v1-03.md`

## 验证结果

- `python -m pytest tests/unit/run/test_phase4_mobile_runtime_host_batch1.py -v` -> 3 passed
- `python -m pytest tests/unit/run/test_phase4_mobile_runtime_host_batch2_android.py -v` -> 3 passed
- `python -m pytest tests/unit/run/test_phase4_mobile_runtime_host_batch3_ios.py -v` -> 3 passed
- `python -m pytest tests/unit/run/test_phase4_mobile_runtime_host_batch1.py tests/unit/run/test_phase4_mobile_runtime_host_batch2_android.py tests/unit/run/test_phase4_mobile_runtime_host_batch3_ios.py -v` -> 9 passed
- `python -m pytest tests/unit/run/test_subject_workers.py -k windows_validate_only_build_uses_visual_studio_generator_and_records_non_repo_cmake_binary_dir -v` -> 1 passed
- `cmake -S tests/gate/android-smoke -B artifacts/.tmp-mobile-android-gate -G "Visual Studio 17 2022"` -> configured
- `cmake --build artifacts/.tmp-mobile-android-gate --config Release` -> built `android_startup_smoke.dll`

## 备注

- 当前 Android/iOS 都还是 skeleton，不是 runtime 完整性证据。
- iOS 代码骨架已落地，但当前 Windows 宿主没有做 ObjC/Xcode 真实编译验证。
- root preset 仍然走 `tests/gate/*`；后续切换到 `src/mobile/android` / `src/mobile/ios` 时应保持分批收敛。

## 下一步

- 进入 Batch 4.1 `MobileHelloWorldProof`
- 先决定 subject 复用方式，再把 buildable / runtime evidence 接入当前 mobile skeleton
