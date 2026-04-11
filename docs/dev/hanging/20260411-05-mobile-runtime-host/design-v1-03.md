# Design v1-03

## 范围

本版只覆盖 iOS Host 的 3.1 / 3.2 / 3.3：

- 新增 `src/mobile/ios/CMakeLists.txt`
- 新增 `src/mobile/ios/bridge/mobile_host_bridge.mm`
- 新增 `src/mobile/ios/app/AppDelegate.mm`
- 新增 `src/mobile/ios/app/Info.plist`

本版不做：

- root preset 改接 `src/mobile/ios/`
- iOS simulator 启动验证
- device code signing / provisioning
- crash symbolication

## iOS skeleton contract

### `AppDelegate.mm`

- 暴露最小 `UIApplicationDelegate`
- 在 `application:didFinishLaunchingWithOptions:` 中调用：
  - `chaos_mobile_ios_run_host()`
- 通过 `NSLog` 输出最小结果

### `mobile_host_bridge.mm`

- 暴露：
  - `int chaos_mobile_ios_run_host(void)`
- 直接转发：
  - `il2cpp_host_run(0, nullptr)`

### `src/mobile/ios/CMakeLists.txt`

- 构建目标：
  - `chaos_mobile_ios_host`
- 编译源：
  - `bridge/mobile_host_bridge.mm`
  - `src/mobile/shared/host_main.cpp`
  - `src/mobile/shared/result_reporter.cpp`

## 验证策略

- 新增 `tests/unit/run/test_phase4_mobile_runtime_host_batch3_ios.py`
- 锁定：
  - iOS app / bridge / CMake 文件存在
  - `AppDelegate` 调用 `chaos_mobile_ios_run_host()`
  - bridge 调用 `il2cpp_host_run(0, nullptr)`
  - iOS CMake 复用 `src/mobile/shared/*`
