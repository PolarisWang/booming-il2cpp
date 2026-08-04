# I-R3: Cpblk/InitBlk in FastExecute — 完成

> **task_id**: I-R3
> **parent_task_id**: interpreter-industrialization
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed
> **completed_date**: 2026-05-23

## 完成摘要

在 FastExecute 中实现了 Cpblk（memcpy）和 InitBlk（memset）处理器，替换了之前的 Handle_Unsupported 桩。

### 变更文件

- `src/native/interpreter/fast_dispatch.cpp` — Handle_Cpblk 和 Handle_InitBlk 实现：
  - Handle_Cpblk: 从栈弹出 size > src > dst → std::memcpy(dst, src, size)
  - Handle_InitBlk: 从栈弹出 size > value > addr → std::memset(ptr, value, size)

### 验证

- chaos_interpreter.lib 编译通过（0 errors）

### Exit Criteria

| 标准 | 状态 |
|------|------|
| Cpblk 在 FastExecute 中可用而非退回到 InterpreterVM | ✅ |
| InitBlk 在 FastExecute 中可用而非退回到 InterpreterVM | ✅ |
| 编译通过 | ✅ |
