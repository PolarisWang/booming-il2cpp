# I-R5: FastFrame 容量扩展

> **task_id**: I-R5
> **parent_task_id**: interpreter-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **blocking_questions**: []
> **question_clearance**: cleared
> **clearance_confirmed_by_user**: true
> **entry_skill**: dev-writing-plans

## Scope

FastFrame 的固定容量限制（kMaxStack=16, kMaxLocals=8, kMaxTracked=8）导致真实世界方法频繁降级到 InterpreterVM。本子任务将这些限制扩展为方案 B（stack=64, locals=32, tracked=32），并使 OsrState 与之对齐。

## Inputs

- `src/native/interpreter/fast_dispatch.h` — FastFrame struct
- `src/native/interpreter/osr_state.h` — OsrState struct（需要对齐）
- `src/native/interpreter/fast_frame_pool.h` — FastFramePool（检查 TLS 帧大小变化影响）

## Expected Outputs

- `fast_dispatch.h` — kMaxStack=64, kMaxLocals=32, kMaxTracked=32
- `osr_state.h` — kMaxStack=64, kMaxLocals=32, kMaxTracked=32
- 所有边界检查代码确认与新容量兼容

## Exit Criteria

- FastFrame 容量常量已修改
- OsrState 容量常量已对齐
- push/pop/access 边界检查与新容量一致
- FastFrame size 变化评估完成
- 编译通过，integration test 全部通过
