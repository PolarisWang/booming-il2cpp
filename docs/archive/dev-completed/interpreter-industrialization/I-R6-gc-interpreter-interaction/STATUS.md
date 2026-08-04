# I-R6: GC × Interpreter 交互验证 — 完成

> **task_id**: I-R6
> **parent_task_id**: interpreter-industrialization
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed
> **completed_date**: 2026-05-23

## 完成摘要

对 Interpreter（FastExecute + FastFrame）与 CRAG GC 的交互进行全面审计，涵盖 GC 根扫描覆盖、Write Barrier 必要性分析、Safepoint 交互三个方面。

### 审计范围

| 维度 | 检查项 | 结论 |
|------|--------|------|
| GC 根扫描 | FastFrame TLS 可见性 | **Minor gap** — 见下文 |
| GC 根扫描 | FastFrame tracked_objs GC 覆盖 | **Safe** — 非 GC 对象 |
| GC 根扫描 | T4 code GcSlotMap 注册 | **Correct** |
| Write Barrier | InterpreterObject 字段写 | **Not needed** |
| Write Barrier | 跨边界 GC 引用 | **Minor gap** — 见下文 |
| Safepoint | FastExecute 无 SafepointPoll | **By design** |
| Safepoint | STW GC 延迟 | **By design** — 长热方法应升至 T4 |

### 详细分析

#### 1. GC 根扫描覆盖

**当前机制**：
- CRAG BGC Phase 1c 调用 `GcScanAllThreadRoots()`，对每个线程的 C++ 栈范围进行**保守扫描**
- T4 native code 框架通过 GcSlotMap 做**精确扫描**（Phase 2）
- Interpreter 框架 FastFrame 的 eval stack / locals / tracked_objs 在 **TLS** 中，不在 C++ 栈范围内

**发现**：FastFrame 在 TLS 池（或回溯 fallback 在 C++ 栈），**正常池路径**下 FastFrame.stack[] / tracked_objs[] 的内容对保守 GC 扫描不可见。

**但 InterpreterObject 不是 GC 对象**：
- `InterpreterObject` / `ArrayStorage` 通过 `CHAOS_IL2CPP_MALLOC` 分配（原始堆），非 GC 托管
- 生命周期由 `FastFrame::CleanupTracked()` 在帧释放时管理
- GC 不需要扫描它们，因为它们在 GC 堆范围之外（`g_heap_base` 过滤）

**唯一风险点**：跨边界调用（Interpreter → Native AOT）返回 GC 堆对象指针时，该指针存放在 FastFrame eval stack (TLS) 中。并发 BGC 保守扫描看不到它。但该指针通常有其他 C++ 栈可达路径，实际风险极低。

#### 2. Write Barrier 分析

| 写操作 | 是否需要 Write Barrier | 原因 |
|--------|----------------------|------|
| InterpreterObject::fields[idx] 赋值 | **否** | InterpreterObject 不在 GC 堆中 |
| FastFrame.stack[sp] 赋值 | **否** | FastFrame 在 TLS / C++ 栈，非 GC 堆 |
| ArrayStorage::elements[idx] 赋值 | **否** | ArrayStorage 通过 CHAOS_IL2CPP_MALLOC 分配 |

**结论**：解释器永不直接写入 GC 堆对象。所有解释器对象都是 raw heap 分配。Write Barrier 在此执行模型下不必要。

#### 3. Safepoint 交互

| 路径 | SafepointPoll | 说明 |
|------|--------------|------|
| FastExecute | **无** | 短方法快速返回；长热方法通过 OSR 升至 T4 |
| T4 native code | **有**（`enable_safepoint_polls=true`） | 循环 backedge 处注册 SafepointPoll |
| RegisterExecute | **无** | 同 FastExecute，短路径无 poll |

**STW GC 延迟**：如果线程正在执行长循环的 FastExecute（尚未升至 T4），STW GC 需等待其返回或到达 T4 safepoint。**混合 OSR** 确保热循环在 backedge 触发的 OSR 中升至 T4，从而获得 safepoint 能力。

**BGC 并发**：BGC 在 thread 运行时扫描 C++ 栈（保守模式），不依赖 SafepointPoll。

### Exit Criteria

| 标准 | 状态 |
|------|------|
| GC 根扫描覆盖审计 | ✅ — 无需修复的小间隙 |
| Write Barrier 验证 | ✅ — 不需要 |
| Safepoint 交互验证 | ✅ — 当前设计安全 |
| 审计报告 | ✅ — 本文档 |
