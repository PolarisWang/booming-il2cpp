# Design v1-04

## 范围

本轮只覆盖 Batch 4.1 `MobileHelloWorldProof`：

- 新增 `subjects/MobileHelloWorldProof/`
- 新增 subject-scoped Android/iOS mobile host root
- root `CMakeLists.txt` 支持 subject-scoped mobile host route
- `build/toolchains/run/testing/subject_workers.py` 补齐 iOS `build-target` validate-only 路径

本轮不做：

- Android 真机 / emulator startup runtime
- iOS simulator / device runtime
- package size / perf baseline
- `src/mobile/android` / `src/mobile/ios` 的 root preset 全量切换

## subject 设计

### managed source

- 独立新增 `subjects/MobileHelloWorldProof/source/`
- 保持最小 `Program::Main` 输出：
  - `MobileHelloWorldProof entry reached.`
  - `args=<n>`
  - `register:Main`

### manifest

- category: `canonical`
- default goal: `correctness.dev`
- default matrix: `windows-managed-output`
- validation profile: `managed-output -> proof`

### pipelines

- `managed-runtime-output`
  - 用于桌面 managed runtime 证明 subject 自身可运行
- `platform-buildable`
  - 用于 Android/iOS mobile build route 证明

### matrices

- `windows-managed-output`
  - host=`windows-x64`
  - target=`windows-x64`
  - runtimeProfile=`managed-output`
- `windows-android-buildable`
  - host=`windows-x64`
  - target=`android-arm64`
  - toolchainProfile=`android-ndk`
  - runtimeProfile=`platform-build`
- `windows-ios-buildable`
  - host=`windows-x64`
  - target=`ios-arm64`
  - toolchainProfile=`ios-xcode`
  - runtimeProfile=`platform-build`

## subject-scoped mobile host route

### root CMake

- 新增 cache 变量：
  - `CHAOS_SUBJECT_ANDROID_HOST_ROOT`
  - `CHAOS_SUBJECT_IOS_HOST_ROOT`
- 默认值保持为：
  - `tests/gate/android-smoke`
  - `tests/gate/ios-smoke`
- 当 root preset 被 `build-target` 调用时，可以把它们覆盖到 subject 目录。

### worker 路由

- `run_build_target(...)`：
  - Android：优先找 `subjects/<SubjectId>/validation/mobile/android-host`
  - iOS：优先找 `subjects/<SubjectId>/validation/mobile/ios-host`
  - 若缺失，则 fallback 到 `tests/gate/*`
- iOS `targetPlatform == "ios-arm64"` 新增 validate-only 构建分支：
  - `preset_target="ios-arm64-packaging"`
  - `toolchain_file=build/toolchains/ios-arm64.cmake`

## subject-owned mobile host root

### Android

- 路径：`subjects/MobileHelloWorldProof/validation/mobile/android-host/`
- 目标：`mobile_hello_world_android_host`
- `mobile_host_entry.cpp` 传入：
  - `argv[0] = "MobileHelloWorldProof"`
  - `il2cpp_host_run(1, argv)`

### iOS

- 路径：`subjects/MobileHelloWorldProof/validation/mobile/ios-host/`
- 目标：`mobile_hello_world_ios_host`
- `mobile_host_entry.mm` 传入：
  - `argv[0] = "MobileHelloWorldProof"`
  - `il2cpp_host_run(1, argv)`

## 验证策略

- RED/GREEN:
  - `tests/unit/run/test_phase4_mobile_hello_world_proof.py`
  - `tests/unit/run/test_subject_planner.py -k mobile_android_buildable`
  - `tests/unit/run/test_subject_workers.py -k validate_only_build`
- 回归：
  - `test_phase4_mobile_runtime_host_batch1/2/3.py`
  - `test_subject_manifest_schema.py`
  - `test_subject_planner.py`
  - `test_subject_workers.py`
- 命令级证据：
  - `dotnet build` + `dotnet MobileHelloWorldProof.dll`
  - subject-owned Android host root 的桌面 CMake configure/build
  - root route 的 Android/iOS validate-only configure/build

## 当前边界

- 当前 Windows 宿主只证明：
  - subject 目录与矩阵已落地
  - Android/iOS buildable route 已挂到 subject
  - Android subject-owned host root 可桌面编译
- 当前未证明：
  - Android startup runtime
  - iOS simulator/device runtime
