# Build Pipeline Fix Report

## 概述

本阶段修复了 chaos-il2cpp AOT 构建管线的 12 个独立问题，覆盖了从运行时头文件、代码生成器、Scriban 模板到 CI 管线工具的完整链路。修复后 26 个 foundation-DLL 全部通过 AOT build。

## 修复分类

### 运行时层（runtime-core）

| 问题 | 根因 | 修复 |
|------|------|------|
| `ChaosStringGetLength` 等 C2065 | `chaos_runtime_host.h` 引用了未声明的函数 | `string_stubs.h/cpp` 添加 4 个函数声明+实现 |
| `::VirtualProtect` C2838 | class 作用域调用（非函数内） | 移除 class-scope 的 VirtualProtect（table 保持 writable 无害） |
| `FillExternalRuntimeStubs` 括号不平衡 | for 循环缺少闭合 `}` → 函数体延伸到类作用域 | 添加缺失的 `}` |
| `ChaosGeneratedRuntimePrelude.h` C2873 | MSVC 拒绝 using-declaration | 替换为前向声明 |

### 代码生成层（Generator）

| 问题 | 根因 | 修复 |
|------|------|------|
| C2059 `'string'` on `extern "C"` | MSVC bug: `<string>` 包含后 `"C"` 字符串字面量触发解析错误 | `#define EXTERN_C extern "C"` 宏绕过 |
| `chaos_valuetype_*` C2061 | 过滤用 `CarrierKindCode` 排除了 CoreLib 值类型 | 改用 `TypeShape == ValueType` |
| `chaos_static_*` C2065 | `ldsflda` 的 RuntimeServiceKind 未被 scanner 识别 | 从 `_staticFieldDeclarations` 回填 |
| Dictionary duplicate key | 2 assembly 模式下 SubjectId 重复 | `GroupBy().First()` 去重 |
| JIT SUBJECT-MISS (模糊匹配) | ATG 索引 `__0` vs AOT IR 索引 `__52` | 去掉 `__NN` 后缀后比较 |
| Debug println `F_a64e93` | 遗留调试输出 | 删除 |

### 模板层（Scriban）

| 问题 | 根因 | 修复 |
|------|------|------|
| `s_functions` 缺少 `};` | 模板中初始化器未闭合 | 添加闭合 `};` |
| `ChaosGeneratedModuleActivate` 缺少 `}` | 函数体未闭合 | 添加 `}` |
| Scriban 多余 `}}` | `if` 块使用了 `{{ }}` | 改为 `{ }` |
| EXTERN_C 宏未使用 | 原始模板用了 `extern "C"` 直接声明 | 替换为 `EXTERN_C` 宏 |

### 构建工具层（TPG）

| 问题 | 根因 | 修复 |
|------|------|------|
| `CodegenOrchestrator.cs` 缺少 `}` | catch 块后缺少方法/类闭合 | 添加 2 个缺失 `}` |
| `Program.cs` scope 错误 | `additionalAssemblyPaths` 在 else 块内声明，块外使用 | 移出 else 块 |
| `Program.cs` duplicate `allAssemblyPaths` | 变量 shadow | 删除重复声明 |
| `Program.cs` merge conflict marker | stash 残留 | 清除 `<<<<<<<` 标记 |
| `CodegenOrchestrator.cs` 缺少 `using` | `AssemblyName` 未引用 | 添加 `using System.Reflection` |
| CMakeLists.txt 缺 `string_stubs.cpp` | 新函数在 SDK 预编译库中没有 | 加入本地编译 |

### CI 管线层（Python）

| 问题 | 根因 | 修复 |
|------|------|------|
| Fact "dropped methods" 误报 | 用 `totalMethods` 而非 `factMethodCount` | 改用 `factMethodCount` |
| Benchmark 过期数据累积 | `benchmark-history.jsonl` append 模式 | stage 启动时清空文件 |
| HotUpdate 错误详情不足 | 仅输出 "error: 0 passed" | 添加 stderr tail + exit code |

## 当前状态

- **26/26 DLL AOT build: ✅ 全部通过**
- 并行 codegen 稳定性: ✅ 已验证（3 chunk 同时构建无崩溃）
- 代码质量: P1+P2+P3 共 17 个问题已修复
- 知识记录: 已写入 `.claude/memory/`
