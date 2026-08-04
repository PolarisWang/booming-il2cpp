# Linux 开发工具链

> 本页补充 `04-工具与集成/INDEX.md` 引用的 Linux 推荐工具链参考。
> 平台主目标为 Windows（vs2022）；Linux x64 为次要目标。

## 推荐工具链

| 工具 | 版本 | 用途 |
|------|------|------|
| GCC | 14 | 编译器（x86_64-linux） |
| Ninja | - | 构建系统生成器 |
| CMake | 31 | 构建编排 |
| Clang | 19 | 替代编译器 / 静态分析 |
| CCache | - | 编译缓存 |
| Mold | - | 并行链接器 |
| GDB | - | 调试 |
| Valgrind | - | 内存检查 |
| Perf | - | 性能采样 |

## 环境配置

- .NET SDK 按项目 `dotnet-tools.json` / global.json 对齐。
- `CMakePresets.json` 提供 `linux-x64-profile` / `linux-arm64-smoke` 等 preset（见仓库根 `CMakePresets.json`）。
- 交叉 / 冒烟构建请参考 `scripts/arm64-deploy-test.sh`、`scripts/setup-qemu-arm64.sh`。

## 现状

- Linux 作为主干验证的次要目标；Windows 为主。
- 工具链细节以 `CMakePresets.json` 与 CI workflows（`.github/workflows/arm64-jit-ci.yml` 等）为准。
