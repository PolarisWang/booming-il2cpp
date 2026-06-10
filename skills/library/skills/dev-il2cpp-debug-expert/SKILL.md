---
name: dev-il2cpp-debug-expert
description: il2cpp 调试专家 — il2cpp 领域专用的系统性调试，集成 trace 系统和 il2cpp 故障模式索引
---

# dev-il2cpp-debug-expert — il2cpp 调试专家

## 领域边界

### 我负责的
- il2cpp 运行时 crash / segfault / access violation 根因定位
- 测试失败、编译错误、意外行为的系统性调试
- GC 相关崩溃的定位（分配异常、写屏障、Page Decommission）
- 跨多层管线（Python → C# codegen → C++ 编译 → 运行时）的问题追踪
- interpreter / runtime-core 中的异常行为

### 我不负责的
- **翻译路径设计**（翻译问题定位后请转给 `dev-il2cpp-translation-expert`）
- **纯测试治理问题**（subject/manifest/runner）→ 请调用 `dev-project-test-governance`
- **性能优化**（profiling 基线分析）→ 请调用 `dev-optimization-campaign`
- **非技术性问题**（需求不清、设计讨论）→ 请调用 `dev-brainstorm`

---

## 触发条件

- 从 `dev-il2cpp-core-agent` 路由到本 expert
- 用户直接输入 `/dev-il2cpp-debug-expert`
- 用户输入包含：crash、segfault、访问冲突、测试失败、编译错误、异常行为、stack overflow

---

## 核心流程

本 expert 基于 `dev-systematic-debugging` 的**四阶段流程**，并针对 il2cpp 领域做了以下增强：

### 增强 1：il2cpp 专属故障模式索引

遇到常见 il2cpp 故障模式时，优先对照以下索引进行快速定位：

| 故障模式 | 典型症状 | 优先检查 | 已知根因 |
|---------|---------|---------|---------|
| Dangling VTable Pointer | RegisterTypeVTable 后的 segfault | `[[dangling-vtable-pointer-segfault]]` | 栈局部 VTable 超出作用域 |
| Page Decommission | stress test 中的访问冲突 | `[[page-decommission-free-list-constraint]]` | 100%-free page 的 freelist dangling pointer |
| TLS Guard Overhead | 热点路径性能骤降 | `[[avoid-thread-local-hashmaps-in-hot-stubs]]` | MSVC 对 function-scoped thread_local complex type 加 guard |
| Tiering Call_Count (memset) | interpreter 分派异常 | `[[tiering-call-count-guard]]` | SetupPatchMethod memset 覆盖 call_count |
| NurseryAllocateSlow 递归 | stack overflow | `[[nursery-allocateslow-oversized-recursion]]` | oversized 从 Phase 2 重入 Phase 1 |
| 枚举/Guid 基准测试失败 | p1_benchmark violation | `[[enum-guid-benchmark-limit]]` | stub 架构 vs managed JIT 栈分配的固有差异 |
| AOT Core IR 阻塞 | Subject_N 合成方法 hang | `[[emit-patch-data-aot-core-ir-blocker]]` | emit-patch-data 复用原始 aot-core-ir.json |
| Hotupdate SubjectId | access violation on 未映射 descriptor | `[[hotupdate-subject-id-resolver-fix]]` | ResolveSubjectId 应返回 metadata token |
| 非 CoreLib 构建失败 | 模板/管线异常 | `[[non-corelib-build-fixes]]` | 3 个已知 template/pipeline 问题 |
| **Crypto ExternalRuntimeFallback** | crypto 方法 value=-1 | `[[crypto-status]]` | 408 core crypto 方法未走 interpreter 路由 |
| **MSVC EH Table Corruption** | throw+catch 在大型 TU 中崩溃 | `[[msvc-eh-table-corruption]]` | MSVC 14.29 EH table 损坏 |
| **Try-Catch-Finally 架构缺口** | InterpreterVM findCatchHandler | `[[try-catch-finally-evaluation]]` | 4 层 EH 分析 |
| **并行 Codegen 构建竞争** | AOT/JIT 二进制损坏 | `[[parallel-codegen-build-race]]` | 并行 cmake 竞争 |
| **Entry.exe SEH 捕获缺口** | AOT 方法 AV 未捕获 | `[[entry-exe-seh-catch-gap]]` | 必须用 __try/__except 替代 C++ catch |
| **Shutdown AV Abort Longjmp** | SIGABRT 杀死验证进程 | `[[shutdown-av-abort-longjmp-fix]]` | longjmp recovery |
| **Low-Mem Pointer Truncation** | x64 AV 在低内存下 | `[[gc-low-mem-pointer-truncation]]` | HANDLE* stored in int (32-bit) |

### 增强 2：il2cpp trace 系统优先

在所有 il2cpp 调试中，trace 系统是**第一优先级**的证据收集工具：

```
   第一步：打开 trace 查看器
     run trace                      → 查看最新会话的 span 树
     run trace --exception          → 只看异常记录
     trace-analyze                  → 分析阶段耗时分布

   第二步：定位失败阶段和操作
     trace 会显示：
     - 每个 pipeline 阶段的状态（pass/fail）
     - 异常的 exception 类型和 message
     - 每个操作的源文件:行号
     - 跨语言（Python/C#/C++）的调用链

   第三步：trace 信息不够时，再补诊断埋点
```

详见 `wiki/04-工具与集成/统一追踪体系.md#错误排查工作流`。

### 增强 3：多层管线调试

il2cpp 调试经常涉及 Python → C# codegen → C++ native 三层管线，按以下顺序检查：

```
第 1 层（Python 层）：
  - Python 脚本（hotupdate.py 等）的输入参数是否正确？
  - 环境变量、路径传递是否正确？

第 2 层（C# Codegen 层）：
  - codegen 生成的 C++ 代码是否符合预期？
  - AOT IR 数据流是否正确？（检查 aot-core-ir.json）
  - T4 模板渲染是否有异常？

第 3 层（C++ Native 层）：
  - 编译阶段是否有 warning 被忽略？
  - 运行时 crash 的 stack trace 指向哪个模块？
  - LOG_DEBUG 输出是否揭示了异常值？
```

### 四阶段流程（继承自 dev-systematic-debugging）

#### Phase 1：根本原因调查

1. **仔细阅读错误信息** — stack trace、行号、文件路径、错误码
2. **一致地重现** — 能否可靠触发？精确步骤？
3. **检查最近变更** — `git diff`、最近提交、配置变更
4. **在多组件系统中收集证据** — trace 优先（见增强 2），根据需要加诊断埋点
5. **追踪数据流** — 坏值从哪里产生？一直追踪到源头

#### Phase 2：模式分析

1. 对照 **il2cpp 故障模式索引**（见增强 1）匹配已知模式
2. 找到同一代码库中类似的有效代码
3. 识别有效和损坏之间的差异
4. 理解依赖关系和假设

#### Phase 3：假设与测试

1. 形成单一假设："我认为 X 是根因，因为 Y"
2. 最小化测试 — 每次只改变一个变量
3. 验证后再继续

#### Phase 4：实现

1. 创建失败测试用例
2. 实现单一修复（解决根因而非症状）
3. 验证修复（测试通过 + 无回归）
4. 如果 ≥ 3 次修复失败 → **质疑架构**（调用 `dev-architecture-first-development` 或 `dev-brainstorm`）

## 执行前 Checklist

```
□ 对照 il2cpp 故障模式索引检查（已扩展至 16 种模式）
□ trace 系统优先：run trace → run trace --exception → trace-analyze
□ 检查最近变更：git diff, 最近提交
□ 可以可靠重现吗？
□ 如果是多层管线问题 → 先定位层（Python/C#/C++）
```

## 执行后 Checklist

```
□ 根因确认（不猜测）
□ 创建失败测试用例（最小重现）
□ 实现单一修复（不"顺便"改进）
□ 验证修复（测试通过 + 无回归）
□ commit message 三段式根因记录
□ 如果 ≥ 3 次修复 → 质疑架构（非继续尝试）
```

---

## 调试日志规范

临时插入的 `CHAOS_IL2CPP_LOG_DEBUG` 必须满足：

```cpp
// 1. 追踪值的变化 — 临时插入，用完即删
CHAOS_IL2CPP_LOG_DEBUG("变量 X 的值: %d (0x%x)", x, x);

// 2. 确认分支到达
CHAOS_IL2CPP_LOG_DEBUG("进入 OptimizedPath, is_special=%d", is_special);
```

**规则**：
- 定位后用 **必须删除** LOG_DEBUG（不允许残留到提交）
- WARN/ERROR 日志可以保留
- 热点路径（`fast_dispatch.cpp` 等）默认 LOG_LEVEL=0 仅保留 ERROR

---

## 输出约束

1. **根因记录** — 修复完成后 commit message 必须包含三段式根因：
   - `root_cause` — 一句话根因
   - `fix_strategy` — 修复策略
   - `regression_check` — 验证范围

2. **三次修复规则** — 第三次修复仍未解决时，禁止第四次尝试。必须先调用 `dev-brainstorm` 或 `dev-architecture-first-development` 完成架构审视

3. **禁止直接修复** — 未完成 Phase 1（根因调查）前，不得修改任何代码

---

## 集成点

| 上游 | 本 skill | 下游 |
|------|----------|------|
| `dev-il2cpp-core-agent` → 调试路由 | **dev-il2cpp-debug-expert** | `dev-systematic-debugging`（底层流程） |
| 用户直接输入 | | `dev-test-driven-development`（创建失败测试） |
| | | `dev-verification-before-completion`（验证修复） |
| | | `dev-architecture-first-development`（架构质疑时） |
| | | `dev-brainstorm`（多次修复失败时） |
