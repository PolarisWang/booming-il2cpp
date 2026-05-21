# Phase D 移动端基础设施收口设计

## 目标

把 Phase D 在当前 Windows 环境内能完成的基础设施一次收口，形成可验证的代码面和明确的后续环境边界。

## 设计范围

### 1. iOS 热更新合规校验

- 新增 `contracts/shared/v0/ios-hot-update-strategy.md`
- 新增 `src/managed/Chaos.IL2CPP.HotUpdate/IosComplianceValidator.cs`
- 在 `PackageValidator` 中增加 iOS 分发合规入口，而不是把 iOS 特化逻辑硬塞进现有 AOT 版本兼容校验

设计原则：
- 保持现有 `ValidateCompatibleTargetAotVersion()` 行为稳定
- 新增独立的 iOS 分发校验入口，供移动端发布或包审查路径调用
- 通过最小 managed smoke app 做行为验证

### 2. 移动端 perf 采集骨架

- 新增 `build/toolchains/run/testing/mobile_perf_collector.py`
- 新增 `run_mobile_native_perf()` worker
- 新 stage kind 采用 `mobile-native-perf`

设计原则：
- Android 与 iOS 采集路径分开
- Android 依赖 `adb push/shell/pull`
- iOS 依赖 `cmakeBinaryDir` 生成的工程与 `xcodebuild` 输出解析
- baseline 维度对移动端按 `targetPlatform` 归档，避免把 Android/iOS 指标写入 `windows.json`

### 3. Android/iOS 热更新入口胶水

- Android JNI 新增 load/unload hot update package wrapper
- iOS bridge 新增对应 C API
- `src/mobile/android/CMakeLists.txt` 与 `src/mobile/ios/CMakeLists.txt` 显式连上 `chaos_hot_update`

设计原则：
- 不把移动端平台细节反向塞回 `runtime-core`
- 入口只负责桥接包路径与 native hot-update ABI

### 4. Benchmark manifest 扩展

- 为至少 4 个 benchmark subject 新增 `mobile-native-benchmark` pipeline
- 新增 `android-arm64-native-perf` / `ios-arm64-native-perf` matrix

设计原则：
- 不污染现有桌面 `native-benchmark`
- 移动端 matrix 只声明骨架能力，不宣称真实设备证据已经具备

## 验证策略

1. Python 单元测试覆盖 mobile perf worker/collector 与 manifest
2. Python 驱动 managed smoke app 验证 iOS 合规校验
3. 源码断言覆盖 JNI/ObjC 热更新入口与移动端 CMake 连线

## 完成边界

本设计完成后，Phase D 剩余未完成项仅包括：

1. macOS + Xcode 下的 iOS build/run 证据
2. Android/iOS 真机 benchmark 实测结果
3. 跨平台性能对比报告的真实数据填充
