# iOS Smoke

## 定位

这是 `Roadmap 0` 的 iOS compile/link/packaging 外壳，不承诺完整 iOS runtime 语义。

## 负责

- 冻结 iOS arm64 toolchain 路由入口
- 提供最小 Objective-C++ packaging shell
- 供 `verify-roadmap-0` 在 macOS host 上调度

## 不负责

- 不承载完整 UIKit / app lifecycle
- 不承载完整 Objective-C bridge 语义
- 不声明移动平台 runtime 已经闭环

## 入口

- preset: `ios-arm64-packaging`
- toolchain: `build/toolchains/ios-arm64.cmake`
- artifact root: `artifacts/platform/ios-smoke`
