# Linux Packaging

## 定位

这是 `Roadmap 0` 的 Linux x64 cross-compile / packaging 外壳，不承诺完整 Linux runtime 语义。

## 负责

- 冻结 Linux x64 packaging preset 与 toolchain 路由
- 提供独立于 Android 的最小 packaging shell
- 作为 `common` gate 被 Windows / macOS 两个 host profile 共同调度

## 不负责

- 不承载完整 Linux runtime 行为
- 不承载真实发行包布局细节
- 不承诺 sysroot、linker、loader 语义已在本阶段闭环

## 入口

- preset: `linux-x64-packaging`
- toolchain: `build/toolchains/linux-x64.cmake`
- artifact root: `artifacts/platform/linux-packaging`
