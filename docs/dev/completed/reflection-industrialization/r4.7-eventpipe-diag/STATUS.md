# R4.7 — EventPipe 反射诊断埋点

> **task_id**: r4.7-eventpipe-diag
> **parent_task_id**: reflection-industrialization
> **source_task_id**: reflection-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **创建日期**: 2026-05-23
> **entry_skill**: dev-executing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

为关键反射路径添加 EventPipe 诊断事件埋点：GetType, GetMethod, Invoke。

## Reference

参考已存在的 EventPipe bridge 模式：
- `src/native/diagnostics/eventpipe/ep_gc_bridge.h/.cpp`
- `src/native/diagnostics/eventpipe/ep_hotupdate_bridge.h/.cpp`

## Implementation

1. 创建 `src/native/diagnostics/eventpipe/ep_reflection_bridge.h` 和 `.cpp`
2. 注册 3 个新 EventPipe 事件类型：ReflectionQuery, ReflectionInvoke, TypeResolve
3. 在反射路径中插入埋点调用

## Exit Criteria

- 桥接文件创建
- chaos_eventpipe 构建通过
