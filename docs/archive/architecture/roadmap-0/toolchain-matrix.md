# Toolchain Matrix (Roadmap 0)

## 目标

`Roadmap 0` 只冻结 host-specific matrix 所需的 toolchain 描述，不把 Android、iOS、Linux 直接推进到完整平台语义实现。

## 矩阵

| 目标 | Host | Preset | Toolchain | Gate 强度 | 备注 |
| --- | --- | --- | --- | --- | --- |
| Windows x64 reference | Windows | `windows-x64-reference` | `build/toolchains/windows-x64-reference.cmake` | 强 gate | 参考桌面主基线，承载高阶 ABI / bridge contract 组合验证 |
| macOS reference | macOS | `macos-reference` | `build/toolchains/macos-reference.cmake` | 强 gate 子集 | 参考桌面同语义子集，保持与 Windows 接近的 contract 边界 |
| Android arm64 | Windows | `android-arm64-smoke` | `build/toolchains/android-arm64.cmake` | startup smoke shell | 只要求真实 startup smoke 路径可调度，产物进入 `artifacts/` |
| iOS arm64 | macOS | `ios-arm64-packaging` | `build/toolchains/ios-arm64.cmake` | compile/link/packaging shell | 不要求完整 iOS runtime 语义，但必须保留完整 packaging 路径 |
| Linux x64 | Windows / macOS | `linux-x64-packaging` | `build/toolchains/linux-x64.cmake` | cross-compile / packaging shell | 作为 common gate 被两个 host profile 共同调度 |

## Preset 与 sourceDir 对齐

- `windows-x64-reference` 当前绑定 `tests/contracts/native/abi`，用 ABI compile-only smoke 作为 reference desktop 的低层 toolchain 锚点。
- `macos-reference` 当前绑定 `tests/contracts/native/bridge`，用 bridge compile-only smoke 作为 macOS reference 的低层 toolchain 锚点。
- `android-arm64-smoke` 与 `ios-arm64-packaging` 直接绑定各自的平台 harness 外壳。
- `linux-x64-packaging` 绑定 `tests/platform/linux-packaging`，用来冻结 Linux cross-compile / packaging shell 的独立入口。

## 环境变量

- Android: 可选读取 `ANDROID_NDK_ROOT`
- iOS: 依赖 Xcode / `DEVELOPER_DIR`
- Linux: 可选读取 `ROADMAP0_LINUX_SYSROOT`

## 结论

本文件冻结的是“矩阵如何被调度”，不是“每个平台已经完整实现到什么程度”。`Roadmap 0` 的重点仍然是 contract、smoke 与 verify gate 的入口一致性。
