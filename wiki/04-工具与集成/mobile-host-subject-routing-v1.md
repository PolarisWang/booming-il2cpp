# Mobile Host Subject Routing v1

## 背景

Phase 4 早期的 Android/iOS gate 只证明 preset route 和最小 host shell 存在，但它们并不归属于某个 subject。  
从 Batch 4.1 开始，mobile buildable 证据需要挂到 subject 自己的目录上，否则后续 runtime/perf 仍然只能停留在通用 gate。

## 目录约定

- Android subject-owned host root:
  - `subjects/<SubjectId>/validation/mobile/android-host/`
- iOS subject-owned host root:
  - `subjects/<SubjectId>/validation/mobile/ios-host/`

当前基线 subject：

- `subjects/MobileHelloWorldProof/`

## root CMake 约定

root `CMakeLists.txt` 提供两个 cache 变量：

- `CHAOS_SUBJECT_ANDROID_HOST_ROOT`
- `CHAOS_SUBJECT_IOS_HOST_ROOT`

默认值仍然回退到通用 gate：

- `tests/gate/android-smoke`
- `tests/gate/ios-smoke`

这意味着：

1. 没有 subject-owned mobile host root 的旧路径不会立刻失效。
2. 新 subject 可以通过覆盖 cache 变量接入自己的 Android/iOS host root。

## worker 路由约定

`build/toolchains/run/testing/subject_workers.py` 的 `run_build_target(...)` 规则：

- 当 `targetPlatform == "android-arm64"` 时：
  - 优先选择 `subjects/<SubjectId>/validation/mobile/android-host`
  - 缺失时 fallback 到 `tests/gate/android-smoke`
- 当 `targetPlatform == "ios-arm64"` 时：
  - 优先选择 `subjects/<SubjectId>/validation/mobile/ios-host`
  - 缺失时 fallback 到 `tests/gate/ios-smoke`

此外，iOS 现在拥有与 Android 对齐的 validate-only `build-target` 路径：

- `preset_target = ios-arm64-packaging`
- `toolchain_file = build/toolchains/ios-arm64.cmake`

## MobileHelloWorldProof 基线

`MobileHelloWorldProof` 的最小矩阵：

- `windows-managed-output`
  - 用于桌面 managed runtime 自证
- `windows-android-buildable`
  - 用于 Android buildable route
- `windows-ios-buildable`
  - 用于 iOS buildable route

当前证据级别：

- Windows 宿主上已验证：
  - managed source 可运行
  - Android subject-owned host root 可桌面编译
  - Android/iOS root route 的 validate-only configure/build 可通过
- 尚未验证：
  - Android 真 startup/runtime
  - iOS simulator/device runtime

## 推荐验证命令

```powershell
python -m pytest tests/unit/run/test_phase4_mobile_hello_world_proof.py -v
python -m pytest tests/unit/run/test_subject_planner.py -k mobile_android_buildable -v
python -m pytest tests/unit/run/test_subject_workers.py -k validate_only_build -v

dotnet build subjects/MobileHelloWorldProof/source/MobileHelloWorldProof.csproj -c Release -o artifacts/.tmp-mobile-hello-world-proof
dotnet artifacts/.tmp-mobile-hello-world-proof/MobileHelloWorldProof.dll

cmake -S subjects/MobileHelloWorldProof/validation/mobile/android-host -B artifacts/.tmp-mobile-hello-world-proof-android -G "Visual Studio 17 2022"
cmake --build artifacts/.tmp-mobile-hello-world-proof-android --config Release
```

## 边界

- 这套 routing 规则解决的是“subject 是否拥有 mobile buildable 入口”，不是“mobile runtime 是否已完成”。
- 当后续要把 root preset 全量切换到 `src/mobile/android` / `src/mobile/ios` 时，仍然需要重新评估 subject-owned host root 与平台 skeleton 的组合方式。
