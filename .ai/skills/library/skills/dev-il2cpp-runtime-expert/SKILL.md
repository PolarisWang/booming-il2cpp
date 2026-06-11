---
name: dev-il2cpp-runtime-expert
description: dev-il2cpp-runtime-expert — il2cpp 运行时专家
---

> ⚠️ **本文件通过 Skill 工具加载，作用是注入领域知识到当前对话上下文。**
> 本文件**不是可执行的 agent**。当前 Agent 需阅读下方"执行流程"作为实现参考。
>
> **当前 Agent 请做**：阅读知识域和约束 → 自行实现代码 → 自行验证
> **当前 Agent 不要做**：加载后等待"Expert 自动执行"——它不会，Skill 只加载文本。
>

# dev-il2cpp-runtime-expert — il2cpp 运行时专家

## 领域边界

### 我负责的
- **runtime-core 模块**：运行时核心（对象分配、类型系统、VTable 注册表、method_table、线程状态、GC safepoint、AssemblyManager）
- **interpreter 模块**：解释器 VM 实现（InterpreterEntry、FastDispatch、Handle_*、指令循环）
- **bootstrap 模块**：运行时引导初始化流程
- **support 模块**：运行时支持函数、helper 实现
- **engine-bridge 模块**：引擎绑定、RuntimeAbiV0 实现
- **运行时合约**：`codegen_bridge.h`、`runtime_abi.h` 中的运行时侧实现

### 我不负责的（超出以下范围 → 标记 remaining，回 Dispatcher 重新分发）

- **翻译路径设计**（Planner / Emission 文件修改）→ 超出范围，标记 remaining，原因：需要翻译域知识
- **GC 核心算法**（分代策略、写屏障语义）→ 超出范围，标记 remaining，原因：需要 GC 域知识
- **C# codegen 工具链**（T4 模板、NativeAot lowering）→ 超出范围，标记 remaining，原因：需要 CodeGen 域知识
- **测试治理**（subject/manifest/runner）→ 超出范围，标记 remaining，原因：需要测试治理域知识
- **纯性能优化**（profile 驱动的优化）→ 超出范围，标记 remaining，原因：需要优化域知识
- **编译失败 / codegen stub**（LNK 错误、C++ 编译错、CMake 错误、dotnet build 失败）→ 超出范围，标记 remaining，原因：需要构建修复域知识

## 输出格式（Dispatcher 回读用）

每个 Expert 处理完任务后，必须在当前上下文中输出：

```
✅ done: [已处理的子任务 ID 列表]
⏳ remaining: [未处理的子任务 ID 列表 + 原因]
```

---

## 触发条件

- 从 `dev-il2cpp-core-agent` 路由到本 expert
- 用户直接输入 `/dev-il2cpp-runtime-expert`
- 用户输入包含：runtime-core、interpreter、VTable、method_table、bootstrap、线程状态、safepoint、InterpreterEntry、FastDispatch

---

---REF---

## 知识域

### 核心源文件

| 模块 | 关键文件 | 说明 |
|------|---------|------|
| **runtime-core** | `runtime_core.cpp` | 运行时核心（GcAllocate、ObjectNew、ArrayNew、StringNewUtf8、BoxValueObject、MethodInvoke） |
| | `fast_dispatch.cpp` | **最热执行路径** — Handle_* 指令分派，默认 LOG_LEVEL=0 |
| | `vtable_registry.cpp` | VTable 注册与虚方法解析（ResolveVirtualMethodPointer） |
| | `method_table.cpp` | 跨 DLL 方法表解析 |
| | `runtime_instantiation.cpp` | InterpreterDispatch / InterpreterDispatchRaw |
| | `thread_state.cpp` | 线程状态与 GC Safepoint（SafepointPoll） |
| | `assembly_manager.cpp` | AssemblyManager / ALC 实现 |
| | `gc_bridge.cpp` | GC 桥接（与 CRAG GC 的协作层） |
| **interpreter** | `interpreter_entry.cpp` | InterpreterEntryDirect、Step1_LowerIR、SetupFrame、FastExecute |
| | `interpreter.cpp` | 通用解释器逻辑 |
| **bootstrap** | `bootstrap.cpp` / `chaos_runtime.cpp` | 运行时初始化/引导 |
| **support** | `support.cpp` / 各 helper 文件 | 运行时 helper 实现 |

### 关键合约

| 合约 | 路径 | 说明 |
|------|------|------|
| `CodegenBridgeV0` | `contracts/native/v0/codegen_bridge.h` | codegen→runtime 桥接（注册、元数据、装箱、虚分派、委托、StringId、icall） |
| `RuntimeAbiV0` | `contracts/native/v0/runtime_abi.h` | 运行时 ABI（生命周期、分配、GC Handle、异常、字段访问、反射、泛型） |

### 相关架构文档

| 文档 | 说明 |
|------|------|
| [`01-翻译管线/11-CPP运行时模块分层.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/11-CPP运行时模块分层.md) | 8 个模块的 DAG 依赖关系 |
| [`01-翻译管线/14-VTable注册表.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/14-VTable注册表.md) | VTable 注册与解析 |
| [`01-翻译管线/13-MemoryDomain系统.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/13-MemoryDomain系统.md) | MemoryDomain 分配系统 |
| [`01-翻译管线/17-统一内存分配体系.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/17-统一内存分配体系.md) | A→B→C 三阶段分配策略 |
| [`01-翻译管线/19-解释器VM架构.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/19-解释器VM架构.md) | Interpreter VM 架构 |

### 运行时故障模式索引

| 故障模式 | 典型症状 | 优先检查 | 已知根因 |
|---------|---------|---------|---------|
| Dangling VTable Pointer | RegisterTypeVTable 后 segfault | `[[dangling-vtable-pointer-segfault]]` | 栈局部 VTable 超出作用域 |
| TLS Guard Overhead | 热点路径性能骤降 | `[[avoid-thread-local-hashmaps-in-hot-stubs]]` | MSVC function-scoped thread_local 加 guard |
| Tiering Call_Count (memset) | interpreter dispatch 异常 | `[[tiering-call-count-guard]]` | SetupPatchMethod memset 覆盖 call_count |
| 跨 DLL 类型身份 | TypeInfo 指针等式失败 | `[[module-boundaries]]` | 跨 DLL 必须用 stable_id 比较 |
| AOT Core IR 数据流 | 方法分派到错误实现 | `[[emit-patch-data-aot-core-ir-blocker]]` | aot-core-ir.json 复用问题 |
| Interpreter 栈帧溢出 | 深层递归调用崩溃 | interpreter_entry.cpp FramePool depth | FastFrame pool depth 限制 |
| Hotpatch Dispatch 竞争 | dispatch_entry 中间态 | 检查 memory_order release | flags + method_key 写入顺序 |
| Shutdown AV | 进程退出时 0xC0000409 | `[[shutdown-av-abort-longjmp-fix]]` | SIGABRT + longjmp recovery |
| Entry.exe SEH 缺口 | AOT 方法 AV 未捕获 | `[[entry-exe-seh-catch-gap]]` | 必须用 __try/__except |

### 热点路径性能基线

| 函数 | 位置 | 基线性能 | 说明 |
|------|------|---------|------|
| SafepointPoll | `thread_state.cpp` | ~0.5ns fast path | 无竞争时的最佳路径 |
| GcAllocate (bump) | `runtime_core.cpp` | 与 GC bump 缓存一致 | 分配热点入口 |
| FastExecute main loop | `fast_dispatch.cpp` | ~12ns/op | 指令分派主循环 |
| Handle_Call | `fast_dispatch.cpp` | ~8ns/op | 直接/虚/接口调用分派 |
| Handle_Box | `fast_dispatch.cpp` | ~25ns/op | 值类型装箱 |
| Handle_NewObj | `fast_dispatch.cpp` | ~35ns/op | 对象分配 |
| InterpreterDispatch | `runtime_instantiation.cpp` | ~15ns/op | 解释器分派入口 |
| ResolveVirtualMethodPointer | `vtable_registry.cpp` | ~5ns/op | VTable 虚方法解析 |
| ResolveMethodTable | `method_table.cpp` | ~8ns/op | 方法表查找 |
| [`01-翻译管线/23-托管线程模型与GC协作.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/23-托管线程模型与GC协作.md) | 线程模型与 GC Safepoint |
| [`01-翻译管线/28-AssemblyManager与ALC.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/28-AssemblyManager与ALC.md) | Assembly 管理 |
| [`04-历史决策/15-B2+VTable虚方法分派决策.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/15-B2+VTable虚方法分派决策.md) | VTable 分派架构决策 |
| [`04-历史决策/16-RuntimeCentricCrossDllDispatch决策.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/16-RuntimeCentricCrossDllDispatch决策.md) | 跨 DLL 分派决策 |

---

## 执行流程

### 🚨 第一步：输出分类声明（强制阻断）

**在使用任何工具（Read/Bash/Edit/Write/Grep/Glob）之前，必须先输出：**

```
classification: domains=[运行时] mode=knowledge-inject action=<你要做的操作> expert=dev-il2cpp-runtime-expert
```

**不输出分类声明，不得进行任何文件操作。**

---

### Step 0：架构语境加载（Architecture Pre-check）

1. **加载运行时模块 DAG** — 阅读 [`11-CPP运行时模块分层.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/11-CPP运行时模块分层.md)
2. **检查运行时故障模式索引**（9 种已知模式）
3. **如果涉及跨 DLL 分派** — 阅读 [16-RuntimeCentricCrossDllDispatch决策.md](../../../wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/16-RuntimeCentricCrossDllDispatch决策.md)
4. **如果涉及内存分配** — 阅读 [`17-统一内存分配体系.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/17-统一内存分配体系.md)
5. **如果涉及合约接口变更** — 检查 `codegen_bridge.h` + `runtime_abi.h`
6. **确认修改不违反 `03-架构约束/` 中的禁止模式**

### Step 1：实现

1. **运行时函数修改**：
   - 确认在正确的模块/命名空间中修改
   - 遵循 `chaos::il2cpp::<module>` 命名空间规范
   - 遵循 include 规范（5 档优先级）
   - 热点路径注意 LOG_LEVEL 设置

2. **AOT IR 数据流变更**：
   - 如果修改影响 AOT IR 数据流，检查 `.skip-aot-core-ir` 机制
   - 确认 `aot_core_ir_reader.cpp` 中的对应解析逻辑

3. **合约接口变更**：
   - 运行时接口变更必须同步更新 contract 头文件
   - 更新 `codegen_bridge.h` 状态码和文档
   - 更新 `runtime_abi.h` 函数签名

### Step 3：验证

1. 运行时修改必须通过：
   - 单元测试（在 `tests/` 目录下）
   - 如果涉及 VTable/method_table → 运行虚方法分派测试族
   - 如果涉及 GC 协作 → 运行 stress test
   - 如果涉及跨 DLL → 运行 foundation-dll 验证管线
2. 检查 `[[dangling-vtable-pointer-segfault]]` 等已知陷阱
3. 检查 `[[tiering-call-count-guard]]`（不 memset PatchMethod 的 call_count）

### 热点路径性能对比（如涉及 fast_dispatch.cpp / runtime_core.cpp / interpreter_entry.cpp）

修改热点路径后，必须在 commit message 中包含性能对比表：

```
## 热点路径性能对比
| 函数 | 位置 | 修改前(ns) | 修改后(ns) | 变化 |
|------|------|-----------|-----------|------|
| SafepointPoll | thread_state.cpp | 0.5 | 0.5 | 0% |
| Handle_Call | fast_dispatch.cpp | 8.0 | 12.5 | **+56%** ⚠️ |
| Handle_Box | fast_dispatch.cpp | 25.0 | 25.0 | 0% |
```

- 如果任何函数变化 >10% → 标记 ⚠️ 并分析原因
- 如果任何函数变化 >50% → 该修改不可接受，需重新设计
- 性能数据通过 `cmake --preset profile` + `entry.exe --profile` 采集

## 执行前 Checklist

```
□ 确认在正确的模块/命名空间中修改（chaos::il2cpp::<module>）
□ 如果涉及跨 DLL 分派 → 阅读 16-RuntimeCentricCrossDllDispatch决策.md
□ 如果涉及内存分配 → 阅读 17-统一内存分配体系.md
□ 如果涉及合约接口变更 → 检查 codegen_bridge.h + runtime_abi.h
□ 是否涉及热点路径（fast_dispatch.cpp, runtime_core.cpp）？LOG_LEVEL=0
```

## 执行后 Checklist

```
□ 单元测试通过
□ 如果是 VTable/method_table 修改 → 跑虚方法分派测试族
□ 如果是 GC 协作修改 → 跑 stress test
□ 如果是跨 DLL 修改 → 跑 foundation-dll 验证管线
□ 已知陷阱检查：dangling VTable, call_count memset, AOT IR compat
□ 合约接口变更 → 同步更新 contract 头文件
```

---

## 热点路径约束

以下文件是**最热执行路径**，默认 `LOG_LEVEL=0` 仅保留 ERROR：
- `runtime-core/fast_dispatch.cpp`
- `runtime-core/runtime_core.cpp`（GcAllocate、ObjectNew 等）
- `interpreter/interpreter_entry.cpp`（InterpreterEntryDirect、FastExecute）

在这些文件中：
- 禁止添加 `CHAOS_IL2CPP_LOG_DEBUG` 并保留到提交
- 添加新指令 Handler 时必须确认性能开销
- 预埋 `CHAOS_IL2CPP_PROFILE_SCOPE` 点已在 CLI-2024 文档中列出

---

## 输出约束

1. **合约同步** — 任何运行时接口变更必须同步更新对应的 contract 头文件
2. **命名空间合规** — 使用 `chaos::il2cpp::runtime_core` / `chaos::il2cpp::interpreter` 命名空间
3. **禁止全局 operator new/delete 重载** — 使用 `CHAOS_IL2CPP_*` 宏
4. **热点路径性能** — 修改热点路径后必须确认性能无损
5. **AOT IR 兼容** — AOT IR 数据流修改必须检查 `.skip-aot-core-ir` 和 reader 逻辑
6. **commit message 要求** — 修复完成后必须包含三段式根因：
   - `root_cause` — 一句话根因
   - `fix_strategy` — 修复策略
   - `regression_check` — 验证范围
   - 如果涉及热点路径 → 附加**热点路径性能对比表**（见验证流程）

---

## 集成点

| 上游 | 本 skill | 下游 |
|------|----------|------|
| `dev-il2cpp-core-agent` → 运行时路由 | **dev-il2cpp-runtime-expert** | `dev-systematic-debugging`（调试回退） |
| 用户直接输入 | | `dev-architecture-first-development`（翻译相关时） |
| | | `dev-project-test-governance`（涉及测试时） |
| | | `dev-verification-before-completion`（验证门） |
