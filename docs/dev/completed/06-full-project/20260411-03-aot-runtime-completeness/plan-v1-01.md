# AOT Mainline Runtime Completeness 实现计划

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development 来执行本计划（多个独立模块可并行）。

**目标：** 把 AOT 主线从 proof-level 能力补齐到可承担主包代码，覆盖 interface dispatch、async、threading、GC、nested EH、linker stripping 和 debug baseline。

**架构：** 扩展现有 SemanticWorld/Linker/CodeGen/runtime-core 模块，每个新增能力独立为一个 lowering family + 一个 subject proof。同时新建 `Chaos.IL2CPP.Debug` 项目。

**技术栈：** C# (.NET 8), C/C++, Scriban templates, Python (tests)

**设计文档：** `phase-architecture-v1-01.md` Phase 2 节

**预期知识沉淀：** `wiki/` — runtime completeness 能力矩阵

---

## 步骤

### Batch 1: Interface Dispatch

- [ ] **1.1** 扩展 SemanticWorld：识别 interface implementation + constraint call
- [ ] **1.2** 扩展 Linker：interface dispatch resolution + VTable 构建
- [ ] **1.3** 扩展 CodeGen：新增 lowering family `interface-dispatch`
- [ ] **1.4** 扩展 runtime-core：interface dispatch table + itable lookup
- [ ] **1.5** 新增 Scriban 模板 `InterfaceDispatch.cpp.scriban`
- [ ] **1.6** 创建 `subjects/InterfaceDispatchProof/`
- [ ] **1.7** 验证：dispatch overhead < 5ns（perf baseline）

### Batch 2: Async/Await + Iterator

- [ ] **2.1** 扩展 SemanticWorld：识别 `IAsyncStateMachine` 实现类
- [ ] **2.2** 扩展 Linker：async 状态机的 reachability 闭包（包含编译器生成的类型）
- [ ] **2.3** 扩展 CodeGen：async 状态机 lowering（展开为同步调用 + continuation）
- [ ] **2.4** 创建 `subjects/AsyncAwaitProof/`
- [ ] **2.5** 验证：`async Task<int> ComputeAsync()` → 正确返回结果

### Batch 3: Threading + GC

- [ ] **3.1** 扩展 runtime-core：ThreadStatic storage 实现
- [ ] **3.2** 扩展 runtime-core：Monitor.Enter/Exit (lock) 实现
- [ ] **3.3** 扩展 runtime-core：GC safepoint 机制 + root reporting interface
- [ ] **3.4** 扩展 runtime-core：finalizer queue 基础骨架
- [ ] **3.5** 创建 `subjects/ThreadingProof/`
- [ ] **3.6** 验证：多线程 ThreadStatic 隔离 + lock 互斥

### Batch 4: Nested EH + Marshaling

- [ ] **4.1** 扩展 CodeGen：nested try-catch-finally + filter + fault handlers
- [ ] **4.2** 扩展 runtime-core：exception dispatch 支持多层嵌套
- [ ] **4.3** 创建 `subjects/NestedExceptionProof/`
- [ ] **4.4** 扩展 CodeGen：blittable struct marshal + string marshal (UTF-8/UTF-16)
- [ ] **4.5** 扩展 CodeGen：reverse P/Invoke（native → managed callback）
- [ ] **4.6** 创建 `subjects/MarshalingProof/`

### Batch 5: Linker Stripping + Debug Baseline

- [ ] **5.1** 扩展 Linker：metadata stripping（标记 unreachable types/methods，不写入 registration）
- [ ] **5.2** 扩展 Linker：`[Preserve]` attribute 支持
- [ ] **5.3** 扩展 Linker：stripping report 输出（被剥离的类型/方法清单）
- [ ] **5.4** 新建 `Chaos.IL2CPP.Debug` 项目
- [ ] **5.5** 实现 DebugMetadataWriter：IL offset → C++ line number 映射
- [ ] **5.6** 实现 native crash handler 中的 managed stack trace recovery
- [ ] **5.7** 验证：stripping rate ≥ 60% unreachable types

### Batch 6: Perf Governance

- [ ] **6.1** 建立 native perf baseline framework（subject × platform × variant）
- [ ] **6.2** GenericEcho perf baseline：meanDurationMs ≤ 12.0, 回归阈值 +20%
- [ ] **6.3** InterfaceDispatchProof perf baseline：dispatch ≤ 5ns
- [ ] **6.4** convert time baseline：GoldenMultiProject ≤ 10s
- [ ] **6.5** 运行完整测试套件，0 regressions
