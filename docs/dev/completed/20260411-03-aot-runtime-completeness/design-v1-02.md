# Design v1-02

## Batch 3 最小实现设计

### 目标

为 `Phase 2 / Batch 3` 建立一个可验证的 threading + GC proof slice，使主线第一次具备以下最小能力：

- `runtime_core` 真实提供 thread-static / monitor / safepoint / root reporting / finalizer queue helper
- managed pipeline 识别 `ThreadStatic` 与 `Monitor` 相关 surface
- codegen 能为 `ThreadingProof` 选择独立 lowering family
- subject proof 能在 native 侧观测到线程隔离、互斥和最小 GC 表面

### 设计范围

#### Contracts / Loader

- 在 `ManagedFieldModel` 上补充最小字段元信息：
  - `IsStatic`
  - `IsThreadStatic`
- `LoaderStage`：
  - 从 `FieldAttributes.Static` 填充 `IsStatic`
  - 解析字段 custom attributes，识别 `System.ThreadStaticAttribute`
  - generic materialization / member reference materialization 继续沿用定义侧的 `IsThreadStatic`

#### SemanticWorld

- 扩展 method capability 识别：
  - 当方法访问 `IsThreadStatic` 字段时，打上 `requires-thread-static-storage`
  - 当方法调用 `System.Threading.Monitor::Enter` / `Exit` 时，打上 `requires-monitor-enter-exit`
- world capability 通过 method capability union 自动得到，无需单独建字段 capability 表

#### Linker

- 新增 `IncludeThreadStaticFieldClosure(...)`
  - 若可达方法命中 `requires-thread-static-storage`
  - 额外保留当前 world 内所有 `IsThreadStatic` 字段及其声明类型
- 扩展外部依赖 reason：
  - `System.Threading.Monitor::Enter(System.Object)` -> `monitor-enter-exit`
  - `System.Threading.Monitor::Exit(System.Object)` -> `monitor-enter-exit`

#### runtime-core

- 新增最小 helper：
  - `ThreadStaticInt32Add(...)`
  - `MonitorEnter(...)`
  - `MonitorExit(...)`
  - `GcSafepoint(...)`
  - `ReportThreadRoot(...)`
  - `EnqueueFinalizer(...)`
  - `DrainFinalizerQueue(...)`
- 实现策略：
  - `ThreadState` 持有一个 heap-allocated 的 thread-local context，内部用 `unordered_map<string, int32_t>` 维护 int slot
  - monitor 用进程级 registry + `std::recursive_mutex`
  - safepoint 只记录“当前线程已进入 safepoint”的最小状态
  - root reporting 只记录 root 条目数量，不做真实扫描
  - finalizer queue 只维护 FIFO 队列并同步执行 callback，不创建独立 finalizer 线程

#### CodeGen

- 新增 lowering family：`managed-threading.threadstatic-monitor.minimal`
- 选择条件：
  - assembly name 为 `ThreadingProof`
  - world capabilities 命中 `requires-thread-static-storage`
  - world capabilities 命中 `requires-monitor-enter-exit`
- 新增模板 `NativeReferenceProof.ThreadingThreadStaticMonitor.cpp.scriban`
  - 直接调用 `runtime_core` helper
  - 创建工作线程并在其中执行 attach / thread-static / monitor / root reporting
  - 在主线程执行 finalizer queue drain
  - 成功后通过 `EngineLogWrite` 输出固定 JSON

#### Subject

- 新增 `subjects/ThreadingProof/`
- `Program.cs` 包含：
  - `[ThreadStatic]`
  - `lock`
  - `volatile`
  - `ThreadingProofEntry.Run()`
- proof scaffold 复用现有 native-reference subject 结构

### 验证策略

- 先写 RED 测试冻结以下边界：
  - `ThreadingProof` subject 树存在
  - source 明确包含 `[ThreadStatic]` / `lock` / `volatile`
  - semantic/linker/codegen/template/runtime-core 表面存在
- 再补实现并运行：
  - `python -m pytest tests/unit/run/test_phase2_threading_gc_proof.py -v`
  - `python -m pytest tests/unit/run -v`
  - `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug`
  - `dotnet build subjects/ThreadingProof/source/ThreadingProof.csproj -c Debug`

### 风险

- 本轮 `ThreadStatic` 只先覆盖 int slot proof，不代表完整类型系统支持
- `MonitorEnter/Exit` 是 helper 级行为，不等于 CLR 对象头 sync block 已就位
- GC helper 只提供表面验证，不代表 collector 已可回收托管对象
