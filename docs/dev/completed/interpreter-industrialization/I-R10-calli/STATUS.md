# I-R10: Calli in FastExecute — 完成

> **task_id**: I-R10
> **parent_task_id**: interpreter-industrialization
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed
> **completed_date**: 2026-05-23

## 完成摘要

在 FastExecute 中实现了 Calli（间接函数指针调用）处理器，替换了 Handle_Unsupported 桩。

### 变更文件

- `src/native/interpreter/fast_dispatch.cpp`：
  - Handle_Calli 实现：从栈弹出函数指针 → 使用 PopCallArgs 弹出参数 → InterpreterDispatchRaw 调用
  - 更新 opcode 范围守卫从 `>= 99` 到 `> 99`（Calli 是 opcode 99）
  - 添加 DISPATCH_CASE(99, Calli) 到调度表

### 验证

- chaos_interpreter.lib 编译通过（0 errors）

### Exit Criteria

| 标准 | 状态 |
|------|------|
| Calli 间接调用在 FastExecute 中可用 | ✅ |
| 函数指针为空时安全跳过 | ✅ |
| 编译通过 | ✅ |
