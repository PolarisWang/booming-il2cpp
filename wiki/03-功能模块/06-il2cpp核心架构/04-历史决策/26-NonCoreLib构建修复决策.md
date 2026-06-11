# 26 — Non-CoreLib 构建修复决策

> 来源：`MEMORY.md > non-corelib-build-fixes.md`

## 问题

非 System.Private.CoreLib 程序集构建遇到 3 个互不相关的故障，每个都会导致链接或编译失败。

## 修复清单

### 1. profile_stats.cpp 过时 SDK (影响全量新构建)

- **根因**: `profile_stats.cpp` 添加到 CMakeLists.txt 后预构建的 chaos_runtime_core.lib 未更新
- **修复**: 在 CMakeLists.txt.scriban 中添加 CHAOS_PROFILE_STATS 配置，从 `${CHAOS_PROJECT_ROOT}/src/native/runtime-core/profile_stats.cpp` 本地编译
- **影响**: 任何非缓存的新构建在链接时出现 5 个 LNK2019 错误

### 2. CHAOS_FACT_CHECK MSVC C2713/C2712 (影响无 JIT 的程序集)

- **根因**: CHAOS_FACT_CHECK 宏展开为 try/catch，与 RunFactJsonMode 中的 __try/__except 在 MSVC 中不可共存
- **修复**: 添加 `#ifndef CHAOS_IL2CPP_JIT_MODE` 守卫 — JIT 模式得到 `((void)0)` 存根
- **注意**: Linter 将模板追加而非替换，导致重复块

### 3. build.py linter 损坏

- **根因**: Linter 的后处理代码将 Python 字符串字面量中的 `\n` 转义序列损坏为字面换行符
- **修复**: 手动恢复 `\n` 为 `\n`、`\` 为 `\\`

## 剩余问题

- 部分程序集根目录有 `_dll/namespace-partition.json` 而非 `namespace-partition.json`
- 多个程序集仅有缓存的 entry.exe（非当前 SDK 构建）
- Linter 在每次运行后还原 CMakeLists.txt.scriban 模板修改

## TRIGGER

非 CoreLib 程序集构建失败、LNK2019 链接错误、C2713/C2712 编译错误时。
