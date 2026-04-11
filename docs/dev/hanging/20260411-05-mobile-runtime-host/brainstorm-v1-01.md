# Brainstorm v1-01

## 背景

- 当前移动端只存在 preset / toolchain / gate 级壳子：
  - `android-arm64-smoke`
  - `ios-arm64-packaging`
- `tests/gate/android-smoke/smoke_entry.cpp` 与 `tests/gate/ios-smoke/smoke_entry.mm` 只是返回 `0` 的空入口。
- 仓库里还没有 `src/mobile/`，也没有共享 host harness。
- `verify-runtime-baseline.py` 目前把 Android/iOS 视为 buildable / packaging gate，不提供共享 runtime 入口。

## 目标

- 先建立 Batch 1 shared host framework：
  - `src/mobile/shared/host_main.*`
  - `src/mobile/shared/result_reporter.*`
  - `extern "C" int il2cpp_host_run(int argc, const char** argv)`
- 让现有 Android/iOS gate 不再只编空 `smoke_entry.*`，而是编译共享 host harness。

## 方案比较

### 方案 A：直接把 Android/iOS gate 全部迁到 `src/mobile/android` / `src/mobile/ios`

- 优点
  - 目录一步到位
  - 更贴近最终结构
- 缺点
  - 改动面过大
  - 需要同时改 root CMake / gate 路由 / toolchain 假设
  - 在 shared host 还未稳定前就移动全部平台壳子，风险偏高

### 方案 B：先建立 `src/mobile/shared/`，继续让现有 `tests/gate/*` 消费共享 host harness

- 优点
  - 改动最小
  - 复用现有 preset / toolchain / gate 路由
  - 能快速把“空 smoke_entry”升级成“共享 host 入口”
- 缺点
  - Android/iOS 真正迁到 `src/mobile/android` / `src/mobile/ios` 还要后续 batch 完成

### 方案 C：继续维持空 `smoke_entry.*`，等完整 mobile runtime 再一起改

- 优点
  - 当前工作量最小
- 缺点
  - 不满足 Batch 1 目标
  - 会继续让移动端 gate 缺少共享 host 事实载体
  - 后续 Android/iOS bridge 仍然没有统一入口可接

## 结论

- 选择方案 B。
- 先把共享 host harness 收敛到 `src/mobile/shared/`，再让 `tests/gate/android-smoke` / `tests/gate/ios-smoke` 改为编译这些共享源文件。
- 等 Batch 2/3 再把平台壳子从 `tests/gate/*` 平滑迁到 `src/mobile/android/` / `src/mobile/ios/`。
