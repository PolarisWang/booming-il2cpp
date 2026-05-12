# Native C++ 项目结构与工程规范综合评估报告

## 概述

- **审查范围**: `src/native/` 全部子模块 + `contracts/native/v0/` 合约层 + 构建系统
- **审查日期**: 2026-05-13
- **代码行数估算**: ~15,000 LOC（不含 generated 目录）
- **构建目标**: Windows x64 主目标，CMakePresets 支持 macOS/android/ios/linux 跨平台
- **当前阶段**: 原型→工业化过渡期

---

## 一、构建系统评估

### 1.1 当前架构

7 个静态库构成的链式依赖：

```
chaos_common → chaos_runtime_core → chaos_hot_update
                                    → chaos_interpreter
                                    → chaos_support
                                    → chaos_engine_bridge
                                    → chaos_bootstrap
```

CMakePresets 三档配置（CHECK/PROFILE/SHIP）设计合理，条件编译通过 `cmake --preset <mode>` 一键切换。

### 1.2 问题

| # | 问题 | 严重度 | 说明 |
|---|------|--------|------|
| B1 | 缺少 `BUILD.bazel`/`CMakeLists.txt` 公共构建入口文档 | medium | 新开发者需要通读 CMakeLists.txt 才能理解构建流程 |
| B2 | 缺少 `PRESET` 级别的测试目标绑定 | low | 每个 preset 未显式声明对应的 test target |
| B3 | 依赖图存在"扇出"集中点 | low | `chaos_runtime_core` 是 5 个库的共同依赖，修改它触发全量重编 |

### 1.3 工业化要求

- **必须**: 每个 CMakeLists.txt 必须有 `PUBLIC`/`PRIVATE`/`INTERFACE` 的正确链接声明
- **必须**: 跨平台路径使用 `CMAKE_CURRENT_SOURCE_DIR` 而非硬编码相对路径
- **建议**: 引入 `ccache` 加速本地反复构建
- **建议**: `chaos_runtime_core` 考虑拆分为 `chaos_runtime_core_base` + `chaos_runtime_core_ext` 减少扇出

---

## 二、命名规范评估

### 2.1 命名空间 — 最严重的问题

**当前状态**: 两套命名体系并存

| 体系 | 范围 | 示例 |
|------|------|------|
| `ChaosIl2cpp::Common` | `common/` 模块 | `ChaosIl2cpp::Common::Log` |
| `chaos::il2cpp::<module>` | runtime-core, support 等 | `chaos::il2cpp::memory` |

**严重度**: **HIGH** — 这是最需要优先修复的问题。

**工业化要求**:
- 全库统一命名空间风格。推荐 `chaos::il2cpp::<module>::<sub>`（全小写 + snake_case，与 C++17 标准库风格一致）
- 禁止在头文件中使用 `using namespace`（`runtime_core.h:40` 存在此问题）
- 不在头文件中打开 namespace 后又关闭再打开（`runtime_core.h` 有多次开关）

### 2.2 文件命名

| 规范 | 当前 | 要求 |
|------|------|------|
| 文件名 | 全小写 snake_case | ✅ 一致，保留 |
| 头文件扩展名 | `.h` | ✅ 一致，保留 |
| 源文件扩展名 | `.cpp` | ✅ 一致，保留 |
| 合约头文件 | `contracts/native/v0/` | ✅ 路径设计合理 |

### 2.3 函数/类型命名

| 分类 | 当前 | 问题 |
|------|------|------|
| 类型 (class/struct/enum) | PascalCase `SafeSegfaultGuard` | ✅ |
| 函数 (common.h) | snake_case `log_init` | ✅ |
| 函数 (runtime-core) | PascalCase `GcAllocate` | ⚠️ 与 common 模块不一致 |
| 常量 | `k` 前缀 `kMaxSteps` | ✅ 但在 runtime_core.h 中未严格统一 |
| 宏 | `CHAOS_IL2CPP_` 前缀 | ✅ 全局统一，命名清晰 |

---

## 三、头文件与 Include 规范评估

### 3.1 Include 风格 — 不一致

| 风格 | 示例 | 出现位置 |
|------|------|----------|
| `"quotes.h"` | `#include "chaos/log.h"` | `common/` 内部头文件 |
| `<angle.h>` | `#include <chaos/log.h>` | `runtime-core` 等外部引用 |
| `"relative.h"` | `#include "../common/chaos/log.h"` | `bootstrap.cpp`, `runtime_core.cpp` |

**问题**: 同项目内同一头文件被用三种方式引用。`bootstrap.cpp` 使用 `"../common/chaos/log.h"` 的相对路径，而其他文件使用 `<chaos/log.h>`。

**工业化要求**:
- 统一为 `<chaos/module/header.h>` 体系（angle-bracket + 模块前缀）
- 仅当前目录的同级头文件使用 `"quotes.h"`
- 禁止使用 `../` 相对路径引用非同级头文件

### 3.2 伞状头文件

**`runtime_core.h`**: 367 行，一次性 `#include` 约 30 个子头文件，同时负责类型声明、函数声明、namespace 管理。

**问题**: 
- 单一伞状头文件导致任何子模块改动都触发全量重编
- 头文件中嵌入 `using namespace` 和多次 namespace 开关
- 末尾 `#include "exception_helpers.h"` 在 namespace 闭合之后

**工业化要求**:
- 拆分为细粒度头文件，每个子模块只暴露必要接口
- 提供 `runtime_core.h` 作为兼容性伞状头，但内部实现使用精确包含
- 头文件不得包含 `using namespace`（编译单元 .cpp 除外）
- 保持 namespace 结构整洁，每个头文件只在一个 namespace 块内

### 3.3 Include Guard

`#pragma once` 使用一致，工作良好。无需修改。

---

## 四、代码风格与质量评估

### 4.1 `.clang-format` 配置

```
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 120
BreakBeforeBraces: Attach
Standard: c++17
IncludeBlocks: Preserve
```

**评估**: 基础配置合理。但 `IncludeBlocks: Preserve` 导致 AI Agent 不会自动整理 include 顺序——这是当前 include 风格杂乱的间接原因之一。

**建议**: 改为 `IncludeBlocks: Regroup`，定义分组规则：
```
- `chaos/` (项目内部)
- `gtest/` (第三方)
- `<system>` (标准库)
```

### 4.2 CLAUDE.md 规范覆盖

CLAUDE.md 已覆盖：
- ✅ 三档构建配置及使用场景
- ✅ 日志级别使用规则（DEBUG 用完即删，INFO/WARN/ERROR 可保留）
- ✅ 热点路径约束（`fast_dispatch.cpp` 默认 LOG_LEVEL=0）
- ✅ 性能分析预埋点
- ✅ 内存分配约束（4 条硬性规则）
- ✅ 禁止 `git stash`

**缺失**:
- ❌ 未定义 `Include` 风格规范
- ❌ 未定义 namespace 规范
- ❌ 未定义函数命名约定（snake_case vs PascalCase）
- ❌ 未定义头文件拆分原则（单文件 vs 伞状）

### 4.3 代码异味

| # | 问题 | 位置 | 严重度 |
|---|------|------|--------|
| S1 | `extern "C"` 声明散落在 .cpp 文件中 | `bootstrap.cpp:22-25` | medium |
| S2 | `#include <stdio.h>` C 头风格 | `runtime_core.cpp` | low |
| S3 | `common.h` 末尾注释标记 `// namespace ChaosIl2cpp::Common` 风格不同步 | `common.h:8` | low |
| S4 | `exception_helpers.h` 在 namespace 外部被包含 | `runtime_core.h:366` | medium |

---

## 五、模块职责与边界评估

### 5.1 模块划分合理性

| 模块 | 职责 | 评估 |
|------|------|------|
| `common` | 日志、断言、Profile、基础类型 | ✅ 职责清晰 |
| `runtime-core` | IR 读取、Dispatch、GC、VTable | ⚠️ 偏大，混合了 dispatch + memory + type system |
| `hot-update` | 热更新补丁加载 | ✅ 职责单一 |
| `interpreter` | IL 解释器 | ✅ 职责单一 |
| `support` | 数值转换、字符串 | ✅ 但应更富 |
| `engine-bridge` | Unity 引擎桥接 | ✅ 边界清晰 |
| `bootstrap` | 初始化入口 | ✅ 职责单一 |

### 5.2 问题

`runtime-core` 模块职责偏重。当前已在其中实现：
- IR 读取与解析（`aot_core_ir_reader.cpp`）
- Dispatch 引擎（`fast_dispatch.cpp`, `runtime_instantiation.cpp`）
- 内存管理（`runtime_core.cpp` GC 相关）
- Type System（`vtable_registry.cpp`, `method_table.cpp`）
- 线程与同步

**工业化建议**: 
- 将内存管理提取为独立库 `chaos_memory`
- 将 Type System 提取为 `chaos_type_system`
- `runtime-core` 保留为纯 dispatch 引擎

---

## 六、综合评分

| 维度 | 评分 (1-5) | 说明 |
|------|-----------|------|
| 构建系统 | 4/5 | 设计合理，三档配置明确，preset 路由清晰 |
| 命名规范 | 2/5 | 命名空间分裂是最严重问题 |
| Include 规范 | 2/5 | 三种 include 风格混用，伞状头文件过大 |
| 代码风格 | 3/5 | clang-format 就位但规则不严，CLAUDE.md 覆盖不全 |
| 模块划分 | 3/5 | 整体合理但 runtime-core 过重 |
| 合约边界 | 4/5 | contracts/native/v0/ 设计清晰 |
| 跨平台支持 | 3/5 | CMakePresets 已定义但实际验证不足 |

**综合评分**: 3.0/5 — 可运行，但距工业化生产标准有显著差距。

---

## 七、修复优先级路线图

### P0 — 立即修复（工业化入口闸门）

1. **统一命名空间**
   - 将 `ChaosIl2cpp::Common` → `chaos::il2cpp::common`
   - 清理 `runtime_core.h` 中的 namespace 多次开关和 `using namespace`
   - 统一全库函数命名风格为 snake_case

2. **统一 Include 规范**
   - 全部项目内引用改为 `<chaos/module/header.h>` 风格
   - 消除所有 `../` 相对路径
   - 激活 clang-format 的 `IncludeBlocks: Regroup`

### P1 — 短期修复（1-2 个迭代）

3. **拆分伞状头文件**
   - `runtime_core.h` 拆分为细粒度头文件
   - 提供精确包含路径，减少依赖膨胀

4. **补充 CLAUDE.md 工程规范**
   - 命名空间约定
   - 函数命名约定
   - Include 风格规则
   - 头文件拆分原则

5. **外移 `extern "C"` 声明**
   - 集中到 `contracts/native/v0/` 对外合约头文件中

### P2 — 中期重构（3-6 个迭代）

6. **拆分 `runtime-core`**
   - 提取 `chaos_memory`（GC Allocator）
   - 提取 `chaos_type_system`（VTable, MethodTable）
   - `runtime-core` 保留 dispatch 引擎

7. **引入构建加速**
   - 配置 ccache
   - 考虑模块化构建（`CMAKE_MODULE`）

8. **跨平台 CI 验证**
   - 在每个 PR 中验证 macOS/android/linux 编译
   - 增加 `BUILD.gn` 为未来 Chromium 集成做准备

---

## 八、与主流工业化项目的对比

| 维度 | Unreal Engine | Chromium | 本项目当前 | 目标 |
|------|---------------|----------|-----------|------|
| Namespace | `UE::` | `base::` `net::` | 分裂 | 统一 `chaos::il2cpp::` |
| Include | `"foo.h"` + `<>` 体系 | `<base/logging.h>` 严格 | 混用 | `<chaos/...>` 严格 |
| 模块大小 | 数百小库 | 数千组件 | 7 库偏大 | 10-15 细粒度库 |
| 格式化 | .clang-format 强制 CI | .clang-format + 强制 CI | 静默 | 强制检查 |
| 构建系统 | UE .build.cs | GN/Ninja | CMake | CMake + 可选的 GN |

---

## 九、下一步建议

本报告定位为"诊断文档"，不涉及具体文件修改。建议顺序：

1. **先确认报告中的问题分类和优先级是否与团队认知一致**
2. **P0 项目（命名空间 + Include 规范）可以并行推进** — 这两个问题互不依赖
3. 每项修复应独立 PR，附带快照测试确认 codegen 输出不受影响
4. 全部 P0 完成后更新 CLAUDE.md，固化规则防止回退

是否开始实施 P0 修复？还是先对报告内容进行调整？