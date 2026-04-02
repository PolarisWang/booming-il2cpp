# Android Smoke

## 定位

这是 `Roadmap 0` 的 Android startup smoke 外壳，不承诺完整 Android runtime 语义。

## 负责

- 冻结 Android arm64 toolchain 路由入口
- 提供可落到 `artifacts/` 的最小 native startup shell
- 供 `verify-roadmap-0` 在 Windows host 上调度

## 不负责

- 不承载完整 JNI / activity / Java glue
- 不承载完整 Android packaging pipeline
- 不声明移动平台语义已经闭环

## 入口

- preset: `android-arm64-smoke`
- toolchain: `build/toolchains/android-arm64.cmake`
- artifact root: `artifacts/platform/android-smoke`
