# I-R4: CallVirtConstrained in FastExecute — 完成

> **task_id**: I-R4
> **parent_task_id**: interpreter-industrialization
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed
> **completed_date**: 2026-05-23

## 完成摘要

在 FastExecute 中实现了 CallVirtConstrained 处理器，支持 constrained. 前缀语义。

### 变更文件

- `src/native/interpreter/fast_dispatch.cpp` — Handle_CallVirtConstrained 实现：
  - Struct/值类型 receiver：调用 direct_fn 或 InterpreterDispatchRaw（无需装箱或 vtable 调度）
  - ObjectRef receiver：共享 Handle_CallVirt 的 MIC 路径（Monomorphic Inline Cache）+ vtable 解析
  - 共享 PopCallArgs RAII 机制

### 验证

- chaos_interpreter.lib 编译通过（0 errors）

### Exit Criteria

| 标准 | 状态 |
|------|------|
| constrained. 前缀的方法在 FastExecute 中可用 | ✅ |
| 值类型 receiver 不走 vtable 调度 | ✅ |
| ObjectRef receiver 走 MIC/vtable 路径 | ✅ |
| 编译通过 | ✅ |
