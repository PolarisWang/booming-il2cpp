# Chaos IL2CPP 开发规则

> ⚠️ **第〇条规则（最优先）**：对用户每一条新消息，回复的**第一行必须是分类声明**：
> 格式：`本轮任务涉及 {域1(编号)} + {域2(编号)} ... ，{action} 操作，第 N 轮 → 加载 dev-xxx-expert`
>
> 域编号表：
> | 编号 | 域 | 说明 | 对应 Expert |
> |------|----|------|------------|
> | 1 | 运行时 | runtime-core/interpreter/VTable/bootstrap | `dev-il2cpp-runtime-expert` |
> | 2 | GC | 内存分配、分代、写屏障、stress test | `dev-il2cpp-gc-expert` |
> | 3 | 调试 | crash、segfault、测试失败、异常行为 | `dev-il2cpp-debug-expert` |
> | 4 | CodeGen | C# codegen、T4 模板、snapshot | `dev-il2cpp-codegen-expert` |
> | 5 | 测试 | foundation-dll、subject、manifest | `dev-il2cpp-fact-verification-expert` |
> | 6 | 翻译 | 新 IL 指令、Planner、Emission | `dev-il2cpp-codegen-expert` |
> | 7 | 构建 | 编译、链接、SDK、cmake | `dev-il2cpp-build-fixer` |
> | 8 | 热更新 | PatchLoader、patchdata | `dev-il2cpp-hotupdate-expert` |
>
> action: `read` / `fix` / `build` / `verify` / `plan`
>
> 示例：`本轮任务涉及 CodeGen(4) ，fix 操作，第 1 轮 → 加载 dev-il2cpp-codegen-expert`
>
> **执行协议**：
> 1. 输出分类声明（**必须包含 → 加载 dev-xxx-expert**）
> 2. `echo "本轮任务涉及 {域1(编号)} ... ，{action} 操作，第 N 轮 → 加载 dev-xxx-expert" > .claude/.classified`
>    （hook 验证格式 + 域编号 + action + Expert 名）
> 3. 通过 registry 发现流程加载 Expert 知识（读取对应的 SKILL.md）
> 4. 使用工具（hook 对 Edit/Write 域文件做分类声明格式验证，不强制 loaded_expert）
> 5. 响应结束时：
>    - 延续消息 → **保留** `.claude/.classified`
>    - 新任务 → `rm -f .claude/.classified`
> 6. **建议**：在编辑域文件前先通过 registry 发现加载对应 Expert 知识
>
>
> **Bash 规则**：
> - **管理 Bash**: `echo "..." > .claude/.classified`、`rm -f .claude/.classified` 等标记文件操作 → 无需分类
> - **只读 Bash**: `ls`, `cat`, `git status`, `grep`, `find`, `wc` 等 → 无需分类
> - **域操作 Bash**: 直接放行（loaded_expert 不强制）

## 全局优先级约束（强制）

以下三条优先级在所有开发阶段必须遵守，从 brainstorm 到验证验收覆盖全流程：

1. **性能最优（第一优先级）** — 一切以 il2cpp 生成的 C++ 代码运行性能为第一考量。方案选择、翻译路径决策、代码生成策略，当其他因素与性能冲突时，性能优先。
2. **方案完美性（第二优先级）** — il2cpp 最终方案的架构完整性、正确性、可维护性为第二考量。在性能不受损的前提下，追求架构的干净、一致、可维护。
3. **HotUpdate 支持（第三优先级）** — 最大化支持热更新为第三考量。在性能和方案完整性都满足的前提下，优先选择对热更新友好的方案。

**裁决规则**：低优先级与高优先级冲突时，低优先级让位于高优先级。但高优先级方案不应故意破坏低优先级——在保证高优先级的前提下，尽量兼顾低优先级。

## 分类硬规则（强制）

格式见顶部 ⚠️ 第〇条规则：`本轮任务涉及 {域1(编号)} + {域2(编号)}，{action} 操作，第 N 轮`

1. **每收到一条用户消息后，回复第一行必须是分类声明**。即使上一个消息已经分类过。
2. 包括用户只说"继续"、"A"、"好"等简短回复——这些可能意味着新任务。
3. 单域 → Skill 加载 Expert 知识后自行实现
4. 多域（≥2）→ 默认走 Workflow 委托，不询问用户
5. **在输出分类声明之前，禁止使用任何工具**
6. **分类声明输出后，如果分类命中域编号表 1-8，必须先走 dev-il2cpp 工作流映射再回复**。
    不允许走通用回复（即不使用 dev-il2cpp 路由就给出答案或提问）。
    映射规则见 `dev-il2cpp/SKILL.md` 的"工作流映射"章节。

### Workflow 分发循环规则

Expert 分类矩阵、多域处理策略、终止守卫等路由规则详见 `skills/discovery/routing-rules.md`。

核心原则：
- ≥2 域 → Workflow 委托
- 连续 2 轮无进展 → 终止，需人工介入

## Trace 优先调试

遇到任何失败、测试红、编译错或意外行为时，**必须先查 trace 系统定位问题**，再分析代码或猜测原因：

1. `run trace` — 查看最新会话的 span 树定位失败阶段
2. `trace-analyze` — 分析阶段耗时分布
3. 只有 trace 信息不足时，才考虑加 print/log 或断点调试

详见 `wiki/04-工具与集成/统一追踪体系.md#错误排查工作流`。

## 架构优先开发

在开发任何 il2cpp 新功能或修改现有翻译行为前，必须先执行 `dev-architecture-first-development` 技能完成架构查询，确认翻译路径与既有架构一致。禁止在未查表的情况下直接进入实现。

## 完成前自测

任何开发任务完成后，必须按以下顺序完成自测：

1. **查 wiki 要求** — 查阅 `wiki/` 中对应功能模块的自测要求，按说明执行
2. **无要求则走 TDD** — 如果 wiki 中没有自测要求，则按 TDD 规范补充测试：先写失败测试，再改实现，直到测试通过
3. **验证管线** — 如果项目有对应验证管线（如 foundation-dll verification pipeline），必须跑通后再声称完成

## AI Agent Bug 修复规范

### 规则 1：系统性调试强制加载

任何以"修复"、"bug"、"测试失败"、"崩溃"、"编译错误"等为目的的任务，必须先调用 `dev-systematic-debugging` 技能完成四阶段调试流程（根因调查 → 模式分析 → 假设验证 → 实现修复）。禁止跳过第一阶段（根因调查）直接进入代码修改。

### 规则 2：根因记录

修复完成后，STATUS.md 或 commit message 必须包含以下三段式根因摘要：

- `root_cause` — 一句话根因（如 "memory_order_relaxed 导致跨线程可见性违反"）
- `fix_strategy` — 修复策略（如 "将 relaxed 改为 acquire-release"）
- `regression_check` — 验证范围（如 "运行 gc-stress + foundation-dll 全量 family"）

### 规则 3：三次修复规则

同一 bug 的第三次修复仍未解决时，禁止再尝试第四次修复。必须先调用 `dev-brainstorm` 或 `dev-architecture-first-development` 完成架构审视，确认不是架构层面的问题后再继续。

## 知识记录

- 新翻译路径必须记录到 `wiki/03-功能模块/il2cpp-核心架构/il2cpp-核心翻译路径参考.md`
- 设计讨论必须在 `docs/discuss/` 完成
- 每个功能的 STATUS.md 必须包含 `## 架构映射` 小节（记录格式见 `dev-architecture-first-development` 技能）

## 技能调用

技能源代码在 `.ai/skills/library/skills/`（git 跟踪）。`.claude/skills/` 只注册了一个入口技能 `dev-il2cpp`（对话启动时自动加载），所有子技能通过 discovery index 发现。

发现流程：
1. 读取 `.ai/skills/discovery/skill-index.md`（已预加载到上下文）
2. 根据任务领域选择对应的 registry 页面（`.ai/skills/discovery/registries/<domain>.md`）
3. 从 registry 中找到目标技能，获取其 SKILL.md 路径
4. 读取 `skills/library/skills/<name>/SKILL.md` 并遵循其指令执行

注册中心页面由 `skills/tooling/catalog/generate_skill_catalog.py` 自动生成。修改 `skill.manifest.json` 或 `discovery/domain-catalog.json` 后重新运行该脚本。

## 项目文档原则

- `wiki/` 记录长期知识，`docs/` 记录过程
- 仅当任务确实产生长期有效知识时才更新 wiki
- 历史决策只保留最终方案，不保留讨论过程

## Native 调试/性能分析规范

### 三档构建配置

| 配置 | LOG_LEVEL | TRACE | ASSERT | PROFILE_SCOPE | 用途 |
|------|-----------|-------|--------|---------------|------|
| CHECK | 3 (DEBUG) | 启用 | 启用 | 启用 | 日常开发 |
| PROFILE | 2 (INFO) | 启用 | 关 | 启用 | 性能分析 |
| SHIP | 0 (ERROR) | 关 | 关 | 关 | 发布 |

通过 CMakePresets.json 一键切换：`cmake --preset debug` / `cmake --preset profile` / `cmake --preset ship`。

### LOG_DEBUG/INFO 使用规则

- **追逻辑问题**：在怀疑路径直接插入 `CHAOS_IL2CPP_LOG_DEBUG(...)`，定位后**必须删除**（不允许残留到提交）
- **INFO**：可用于关键生命周期事件（初始化完成、模块加载等），可保留
- **WARN**：用于可恢复的异常情况，可保留
- **ERROR**：始终启用，用于不可恢复的错误

### 热点路径约束

`src/native/runtime-core/fast_dispatch.cpp` 是**最热执行路径**，默认 `#define CHAOS_IL2CPP_LOG_LEVEL 0` 仅保留 ERROR：
```cpp
// fast_dispatch.cpp 顶部
#define CHAOS_IL2CPP_LOG_LEVEL 0
```
其他文件如需强制覆盖日志级别，在 `#include "chaos/log.h"` 前定义：
```cpp
#define CHAOS_IL2CPP_LOG_LEVEL 3  // 临时开启 DEBUG
#include <chaos/log.h>
```

### PROFILE_SCOPE 预埋点

以下热点函数已预埋 `CHAOS_IL2CPP_PROFILE_SCOPE`。因 `config.h` 在 CHECK/PROFILE 构建下默认启用 `CHAOS_IL2CPP_PROFILE_ENABLED=1`，这些 scope 在 `cmake --preset debug` / `cmake --preset profile` 中自动生效。如需强制关闭，在包含 config.h 前定义 `#define CHAOS_IL2CPP_PROFILE_ENABLED 0`。

**runtime_core.cpp**: GcAllocate, GcAllocateAtomic, ObjectNew, ArrayNew, StringNewUtf8, BoxValueObject, MethodInvoke
**fast_dispatch.cpp**: FastExecute(主循环), Handle_Call, Handle_Box, Handle_NewObj, Handle_NewArr, Handle_LdArg, Handle_LdLoc, Handle_StLoc, Handle_LdFld, Handle_StFld, Handle_LdStr, Handle_Ret, Handle_Pop, Handle_Dup, Handle_Br, Handle_BrTrue, Handle_BrFalse, Handle_Throw, Handle_Leave, Handle_Unbox, Handle_LdLen, Handle_Conv_I4, Handle_Conv_I8, Handle_Conv_R4, Handle_Conv_R8, Handle_Add, Handle_Sub, plus 所有比较/分支/位运算 Handler
**runtime_instantiation.cpp**: InterpreterDispatch, InterpreterDispatchRaw
**vtable_registry.cpp**: ResolveVirtualMethodPointer
**method_table.cpp**: ResolveMethodTable
**thread_state.cpp**: SafepointPoll
**gc_bump_cache.h**: GcAllocateImpl
**interpreter_entry.cpp**: InterpreterEntryDirect, Step1_LowerIR, Step1c_2InstrFastPath, FastExecute, SetupFrame, FastExecuteCall

### profile.h 实现特性

`src/native/common/chaos/profile.h` 使用 RDTSC 做零 I/O 热点统计，具备以下优化：

- **Hash 加速槽查找**：FNV-1a 开放寻址哈希表取代 O(n) 线性扫描，scope 进入时 ~O(1) 定位
- **嵌套层级跟踪**：thread_local depth 计数器，`kProfileHashSize = 128` 低冲突率
- **RDTSC→ns 校准**：首次构造时通过 `QueryPerformanceFrequency` 校准，dump 输出 avg_ns/total_ns
- **编译时零开销**：`CHAOS_IL2CPP_PROFILE_ENABLED=0` 时展开为 `NullProfileScope`，编译器完全消除

### 调试模板（AI Agent 使用）

```cpp
// 1. 追踪值的变化 — 临时插入，用完即删
CHAOS_IL2CPP_LOG_DEBUG("变量 X 的值: %d (0x%x)", x, x);

// 2. 确认分支到达
CHAOS_IL2CPP_LOG_DEBUG("进入 OptimizedPath, is_special=%d", is_special);

// 3. 性能热点分析 — 已预埋 PROFILE_SCOPE，只需 cmake 开关
//    cmake -DCHAOS_IL2CPP_PROFILE_ENABLED=ON ...
//    运行后查看 stdout 的 RDTSC 耗时表
```

## 禁止 git stash（强制）

严禁在任何情况下使用 `git stash`。Stash 会隐藏工作区未提交变更，导致其他人或 AI Agent 在后续操作中丢失更改记录。`git stash drop` 更会造成不可逆的数据损失。需要临时切换上下文时，应使用 `git worktree` 或显式提交到分支。

## 统一内存分配约束（强制）

IL2CPP 生成 C++ 代码会被引入游戏引擎源码，因此分配行为必须遵循以下约束：

1. **禁止全局 operator new/delete 重载** — 任何情况下不得添加全局 `operator new` / `operator delete` 重载。生成代码引入游戏引擎后，全局重载会污染引擎的分配行为。
2. **codegen 输出必须使用 CHAOS_IL2CPP_ 宏** — codegen 生成 `new T{}` / `new T[N]` / `malloc` / `free` 都必须替换为 `CHAOS_IL2CPP_NEW_GC` / `CHAOS_IL2CPP_NEW_GC_ARRAY` / `CHAOS_IL2CPP_MALLOC` 等宏。
3. **禁止跨域分配/free 不匹配** — GC 域内存必须通过 GC 回收，Domain 域通过 heap->Destroy() 批量释放，Raw 域通过 std::free 释放。不得混用。
4. **新分配策略必须通过 GcAllocate/GcAllocateAtomic 内部切换** — A→B→C 三阶段（Bump Arena → TLS GC Cache → Precise Generational GC）对 codegen 完全透明，codegen 输出无需修改。

详见 `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/17-统一内存分配体系.md`。

## 命名空间规范（强制）

所有 native C++ 代码使用命名空间形式 `chaos::il2cpp::<module>`：

| 模块 | 命名空间 |
|------|----------|
| `common/chaos/` | `chaos::il2cpp::common` |
| `runtime-core/` | `chaos::il2cpp::runtime_core` |
| `bootstrap/` | `chaos::il2cpp::bootstrap` |
| `interpreter/` | `chaos::il2cpp::interpreter` |
| `support/` | `chaos::il2cpp::support` |
| `hot-update/` | `chaos::il2cpp::hot_update` / `chaos::il2cpp::method_replacement` |

`ChaosIl2cpp::Common` 是旧命名空间**别名**（通过 `namespace ChaosIl2cpp { namespace Common = chaos::il2cpp::common; }` 提供向后兼容），新代码**不得**直接使用 `ChaosIl2cpp::Common` 声明或引用。

禁止：
- 在头文件中使用 `using namespace`（文件作用域）
- 头文件中在 namespace 块外部放置类型声明

## Include 规范（强制）

四档优先级顺序（clang-format 自动管理）：

1. **对应的 `.h` 文件**（.cpp 中第一个 include）— `"foo.h"`（引号）
2. **项目内部头文件** — `<chaos/header.h>`（尖括号，`chaos/` 前缀）
3. **合约头文件** — `<contracts/header.h>`（尖括号，`contracts/` 前缀）
4. **标准库头文件** — `<cstdint>`、`<vector>`、`<string>` 等（尖括号）
5. **外部依赖** — `<gc.h>`、`<fmt/format.h>` 等（尖括号）

禁止：
- **禁止使用 `#include "../module/header.h"` 相对路径** — 改用 CMake `target_include_directories` 后直接用 `"module/header.h"` 或 `<module/header.h>`
- **禁止使用 `#include "chaos/header.h"` 引号风格** — 统一使用 `<chaos/header.h>`

### clang-format 设置

项目 `.clang-format` 配置了 `IncludeBlocks: Regroup` 和 `IncludeCategories`。AI Agent 和开发者应运行以下命令统一格式：

```bash
clang-format -i src/native/**/*.cpp src/native/**/*.h
```

## extern "C" 使用规范

`extern "C"` 用于 ABI 导出，必须搭配注释说明原因：

```cpp
// ABI export: required for C-language linkage from managed/NativeAot code
extern "C" void ChaosFunction() noexcept;
```

所有 `extern "C"` 声明应放在：
- **集中化声明**：尽量放在对应的 `.h` 头文件中的 `extern "C" { ... }` 块内
- **避免散落定义**：函数定义应避免在 `.cpp` 文件中以 `extern "C"` 前缀修饰（不用 `extern "C"` 修饰函数定义体，改用头文件声明 + 普通 C++ 定义）

## 头文件原则

1. **自包含**：每个 `.h` 文件必须包含它自己的所有依赖，不依赖前置 include
2. **Include Guard**：统一使用 `#ifndef CHAOS_IL2CPP_*_H_` / `#define` / `#endif` 风格
3. **最小依赖**：头文件只 include 它直接使用的类型，不包含"可能用到的"
4. **Umbrella header 分层**：
   - `chaos/common.h` — 所有 common 头文件的聚合
   - `runtime_core.h` — 所有 runtime-core 头文件的聚合（有 namespace 块，保证声明顺序）
   - 子模块头文件不要相互 include 形成循环依赖

## unordered_dense 选型规则（AI Agent 专用）

编写 native C++ 代码时，遇到需要用 `unordered_map` 的场景，按以下规则选择宏：

**先用 `CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY`，降级按以下链判断：**

1. Key 类型是指针、整数、枚举、已哈希值（不是 string） → 选 `CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY`（用 `chaos::il2cpp::common::identity_hash` 跳过 wyhash）
2. Key 类型是 string，但无数据依赖需求 → 选 `CHAOS_IL2CPP_UNORDERED_DENSE_MAP`（默认 wyhash）

**退回到 `CHAOS_IL2CPP_UNORDERED_MAP` (std) 的条件：**
1. 需要 iterator/pointer 在 insert/erase 后仍然有效
2. 需要 heterogeneous lookup (`find("literal"sv)` on `map<string,T>`)
3. 代码属于 codegen 输出/热更新路径（保持语义一致）
4. value 类型 > 128 字节且 insert/erase 频繁
5. map 持有元素指针/引用在跨 insert 后访问

**经验法则**：查找表、注册表、缓存、id→ptr 映射、type→handler 映射，无脑用 `MAP_IDENTITY`。
string key 的查找表用 `MAP`。只有依赖 std 链式桶语义时才回退。

## SDK/TPG 输出边界约束（强制）

SDK 输出（`--sdk-out`）和 TPG 测试项目之间必须保持严格的自包含边界：

### 基本原则

1. **SDK 输出必须是自包含的可编译目录** — `chaos-sdk/` 包含消费方编译所需的一切：头文件（`include/`）、预构建库（`lib/`）、CMake 集成（`chaos-config.cmake`、`chaos-targets.cmake`）、运行时存根源文件（`runtime_stubs/`）。消费方只需 `find_package(chaos PATHS <sdk-dir>)` 即可使用。
2. **TPG 产出只能引用 SDK** — 测试项目的 `CMakeLists.txt` 中头文件搜索路径通过 `chaos::runtime` 目标传递，运行时存根 `.cpp` 必须从 SDK 的 `runtime_stubs/` 目录引用。禁止任何 `CHAOS_PROJECT_ROOT` 路径指向仓库源码树。
3. **代码生成器（Codegen）不生成 CMake 文件** — CMakeLists.txt 由 TPG 通过 `.scriban` 模板统一生成。`ConvertToCppHandler.cs` 中已移除所有 CMakeLists.txt 生成逻辑。

### 具体规则

| 规则 | 说明 | 违反后果 |
|------|------|---------|
| 测试 CMakeLists.txt 不得引用 `CHAOS_PROJECT_ROOT` | 运行时存根、profile_globals 等 .cpp 文件必须从 `${CHAOS_SDK_DIR}/runtime_stubs/` 引用 | 链接期可能使用过时的预构建 lib，或产生不可移植的构建 |
| 测试项目不得依赖仓库源码树头文件路径 | 头文件搜索通过 `target_link_libraries(entry PRIVATE chaos::runtime)` 传递的 `INTERFACE_INCLUDE_DIRECTORIES` 完成 | 构建环境与 SDK 发布环境不一致，导致 CI 通过但发布失败 |
| `runtime_stubs/*.cpp` 和 `profile_globals.cpp` 必须由 SdkEmitter 拷贝到 SDK | `SdkEmitter.CopyRuntimeStubSources()` 负责拷贝这些源码文件 | 测试 CMakeLists.txt 无法从 SDK 获取这些源文件，被迫回退到仓库路径 |
| 构建脚本中的 native lib 路径必须可配置 | `hotupdate.py` 中 native build 输出目录等硬编码路径必须提取为模块级常量 | 切换 CMake generator 或修改构建目录结构时遗漏更新 |
| 废弃的 codegen 构建代码必须标记 | `CmakeGenerator.cs` 等仅在测试中引用的死代码需添加 `[Obsolete]` 标记 | 新开发者误以为 CmakeGenerator 是当前方案，继续在其上开发 |

## 四层架构合规规范（强制）

验证管线的四层架构必须严格遵守。任何文件修改必须写入正确的层，禁止跨层写入。

### 四层定义

| 层 | 路径 | 允许产出 | 红线 |
|----|------|---------|------|
| **ATG** (C# 测试生成) | `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/` | `.cs`, `.csproj`, `.json` | 不得生成 C++ |
| **Codegen** (C# IL→C++) | `src/managed/Chaos.IL2CPP.Generator/` | `.generated.cpp`, `.generated.h`, `.json` | 产出必须自包含，不依赖后处理 |
| **TPG** (C# 脚手架) | `src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/` | `.cpp`, `.h` (via Scriban), `.cmake`, `.scriban` | 不得修改 `.generated.*` |
| **Python** (管线编排) | `testing/foundation-dll/verification/` | `.py`, `.json`, `.yaml`, `.md` | 不得 `write_text` 写入 `.cpp`/`.h` |

### AI Agent 执行规则

在执行任何文件写入操作前，必须:

1. **识别当前修改所属层**: 根据文件路径判断
2. **检查目标文件扩展名**:
   - `.generated.cpp/.h` → 只允许 Codegen 层写入
   - `.cpp/.h` (非 generated) → 只允许 TPG/Scriban 写入
   - `.py` → 只允许 Python 层写入
   - `.scriban` → 只允许 TPG 层写入
3. **如果跨层写入是必要的**:
   - 在前一行添加 `# BOUNDARY_OVERRIDE: https://github.com/.../issues/NNN`
   - 添加 `# Reason: ...` 和 `# Expires: YYYY-MM-DD`
   - 在 commit message 中说明为什么不能通过正确层解决
4. **写入 `.cs` (C# 测试代码)**: 优先通过 `ATG --generate-wrappers` 完成

### BOUNDARY_OVERRIDE 机制

所有跨层写入必须标注:

```python
# BOUNDARY_OVERRIDE: https://github.com/.../issues/1234
# Reason: ATG --custom-only not yet implemented, see issue for migration plan
# Expires: 2026-07-15
some_file.write_text('...')
```

过期超过 30 天的 BOUNDARY_OVERRIDE 会被 CI 检查拦截。

### 预提交检查

安装 hook 后自动运行:
```bash
git config core.hooksPath .githooks
```

pre-commit 检查:
1. Python 写入 `.cpp`/`.h` 无 BOUNDARY_OVERRIDE → 拦截
2. BOUNDARY_OVERRIDE 过期 → 拦截
3. 生成的 `.generated.*` 文件含相对 include → 警告

参见 `testing/foundation-dll/verification/preflight/check_layer_boundaries.py`。