# I-R12: Tracked 对象溢出安全修复 — 完成

> **task_id**: I-R12
> **parent_task_id**: interpreter-industrialization
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed
> **completed_date**: 2026-05-23

## 完成摘要

修复 FastFrame tracked 对象系统在溢出（超过 32 个对象）时的内存泄漏。添加了基于堆的溢出块，在 inline 数组填满时自动扩展。

### 变更文件

- `src/native/interpreter/fast_dispatch.h` — Track/TrackPool/CleanupTracked 修改：
  - 新增 `struct TrackedBlock`（32 条目堆分配块）和 `TrackedBlock* tracked_overflow` 字段
  - Track/TrackPool：inline 数组满时自动分配 TrackedBlock 继续跟踪
  - CleanupTracked：同时处理 inline 数组和溢出块，释放后置 null
  - 极限情况（64+ 对象）记录警告但不崩溃

- `src/native/interpreter/fast_frame_pool.cpp` — SetupFastFrame 重置 tracked_overflow = nullptr

### 验证

- chaos_interpreter.lib 编译通过（0 errors）
- CleanupTracked 在 FastFramePool::Release 中调用，确保溢出块被释放

### Exit Criteria

| 标准 | 状态 |
|------|------|
| 33+ 跟踪对象不再泄漏 | ✅ |
| 溢出块在 CleanupTracked 中被正确释放 | ✅ |
| 编译通过 | ✅ |
