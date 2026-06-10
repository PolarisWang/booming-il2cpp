---
name: dev-il2cpp-runtime-expert
description: il2cpp 运行时专家 — runtime-core/interpreter/bootstrap/support 领域的修改与调试
---

# dev-il2cpp-runtime-expert — il2cpp 运行时专家

## 领域边界

### 我负责的
- **runtime-core 模块**：运行时核心（对象分配、类型系统、VTable 注册表、method_table、线程状态、GC safepoint、AssemblyManager）
- **interpreter 模块**：解释器 VM 实现（InterpreterEntry、FastDispatch、Handle_*、指令循环）
- **bootstrap 模块**：运行时引导初始化流程
- **support 模块**：运行时支持函数、helper 实现
- **engine-bridge 模块**：引擎绑定、RuntimeAbiV0 实现
- **运行时合约**：`codegen_bridge.h`、`runtime_abi.h` 中的运行时侧实现

### 我不负责的
- **翻译路径设计**（Planner / Emission 文件修改）→ 请调用 `dev-il2cpp-translation-expert`
- **GC 核心算法**（分代策略、写屏障语义）→ 请调用 GC Expert 或 `dev-systematic-debugging`
- **C# codegen 工具链**（T4 模板、NativeAot lowering）→ 暂直接实现
- **测试治理**（subject/manifest/runner）→ 请调用 `dev-project-test-governance`
- **纯性能优化**（profile 驱动的优化）→ 请调用 `dev-optimization-campaign`

---

## 触发条件

- 从 `dev-il2cpp-core-agent` 路由到本 expert
- 用户直接输入 `/dev-il2cpp-runtime-expert`
- 用户输入包含：runtime-core、interpreter、VTable、method_table、bootstrap、线程状态、safepoint、InterpreterEntry、FastDispatch

---

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
| [`01-翻译管线/23-托管线程模型与GC协作.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/23-托管线程模型与GC协作.md) | 线程模型与 GC Safepoint |
| [`01-翻译管线/28-AssemblyManager与ALC.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/28-AssemblyManager与ALC.md) | Assembly 管理 |
| [`04-历史决策/15-B2+VTable虚方法分派决策.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/15-B2+VTable虚方法分派决策.md) | VTable 分派架构决策 |
| [`04-历史决策/16-RuntimeCentricCrossDllDispatch决策.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/16-RuntimeCentricCrossDllDispatch决策.md) | 跨 DLL 分派决策 |

---

## 执行流程

### Step 1：加载运行时语境

1. 阅读 [`11-CPP运行时模块分层.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/11-CPP运行时模块分层.md) 确认模块 DAG 依赖
2. 如果涉及跨 DLL 分派，阅读 [16-RuntimeCentricCrossDllDispatch决策.md](../../../wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/16-RuntimeCentricCrossDllDispatch决策.md)
3. 如果涉及内存分配，阅读 [`17-统一内存分配体系.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/17-统一内存分配体系.md)
4. 如果涉及合约接口变更，检查 `codegen_bridge.h` 和 `runtime_abi.h`

### Step 2：实现

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

---

## 集成点

| 上游 | 本 skill | 下游 |
|------|----------|------|
| `dev-il2cpp-core-agent` → 运行时路由 | **dev-il2cpp-runtime-expert** | `dev-systematic-debugging`（调试回退） |
| 用户直接输入 | | `dev-architecture-first-development`（翻译相关时） |
| | | `dev-project-test-governance`（涉及测试时） |
| | | `dev-verification-before-completion`（验证门） |
