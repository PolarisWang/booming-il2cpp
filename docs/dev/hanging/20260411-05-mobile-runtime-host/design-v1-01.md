# Design v1-01

## 范围

本版只覆盖 Batch 1 shared host framework：

- 新增 `src/mobile/shared/host_main.h`
- 新增 `src/mobile/shared/host_main.cpp`
- 新增 `src/mobile/shared/result_reporter.h`
- 新增 `src/mobile/shared/result_reporter.cpp`
- 更新 `tests/gate/android-smoke/CMakeLists.txt`
- 更新 `tests/gate/android-smoke/smoke_entry.cpp`
- 更新 `tests/gate/ios-smoke/CMakeLists.txt`
- 更新 `tests/gate/ios-smoke/smoke_entry.mm`

本版不做：

- `src/mobile/android/app/`
- `src/mobile/android/jni/`
- `src/mobile/ios/app/`
- `src/mobile/ios/bridge/`
- 真实 Android/iOS runtime 证据

## 共享 host contract

### `host_main.h`

- 定义 `HostRunResult`
- 暴露 C ABI：
  - `extern "C" int il2cpp_host_run(int argc, const char** argv);`

### `result_reporter.h`

- 暴露：
  - `void report_host_result(const HostRunResult& result);`

### `host_main.cpp`

- 负责组装最小 `HostRunResult`
- 调用 `report_host_result(...)`
- 返回 `result.exitCode`

### `result_reporter.cpp`

- 负责把 `HostRunResult` 输出到标准输出
- 暂不直接接入 `logcat` / `NSLog`
- Android/iOS 的平台特定日志桥接留给后续 batch

## gate 集成策略

### Android

- 保留 `tests/gate/android-smoke/smoke_entry.cpp`
- 入口函数继续叫：
  - `chaos_android_startup_smoke_entry`
- 该入口改为转发到：
  - `il2cpp_host_run(0, nullptr)`
- `tests/gate/android-smoke/CMakeLists.txt` 直接编译：
  - `smoke_entry.cpp`
  - `src/mobile/shared/host_main.cpp`
  - `src/mobile/shared/result_reporter.cpp`

### iOS

- 保留 `tests/gate/ios-smoke/smoke_entry.mm`
- 入口函数继续叫：
  - `chaos_ios_packaging_smoke_entry`
- 该入口改为转发到：
  - `il2cpp_host_run(0, nullptr)`
- `tests/gate/ios-smoke/CMakeLists.txt` 直接编译：
  - `smoke_entry.mm`
  - `src/mobile/shared/host_main.cpp`
  - `src/mobile/shared/result_reporter.cpp`

## 验证策略

### RED

- 新增 `tests/unit/run/test_phase4_mobile_runtime_host_batch1.py`
- 先锁定：
  - `src/mobile/shared/*` 文件存在
  - `il2cpp_host_run(...)` 合同存在
  - Android/iOS gate CMake 不再只编空 `smoke_entry.*`
  - Android/iOS gate 入口改为转发到共享 host

### GREEN

- 最小实现 shared host harness
- 跑新增单测
- 跑现有 `test_subject_workers.py`，确认 Android buildable gate 相关行为未回退

## 后续递进

- Batch 2/3 再把共享 host contract 向 `src/mobile/android` / `src/mobile/ios` 外扩
- 等平台桥接目录稳定后，再考虑把 `tests/gate/*` 的壳子迁出 gate 目录
