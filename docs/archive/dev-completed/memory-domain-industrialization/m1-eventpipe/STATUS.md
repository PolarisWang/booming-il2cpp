# M1 — EventPipe 域诊断桥接

> **task_id**: m1-eventpipe
> **parent_task_id**: memory-domain-industrialization
> **source_task_id**: memory-domain-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **entry_skill**: dev-executing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

为 Memory Domain 关键路径添加 EventPipe 诊断事件埋点，对标 ep_gc_bridge / ep_reflection_bridge 模式。

## Inputs

- 既有 EventPipe bridge 模式：`src/native/diagnostics/eventpipe/ep_gc_bridge.h/.cpp`
- 域注册/卸载/使用量超限等事件点

## Expected Outputs

1. 创建 `src/native/diagnostics/eventpipe/ep_memorydomain_bridge.h` 和 `.cpp`
2. 注册 3 个新 EventPipe 事件类型：DomainRegistered (0x0601)、DomainUnloaded (0x0602)、DomainUsageLimitExceeded (0x0603)
3. 在 `memory_domain.cpp` 的 RegisterMemoryDomain / UnregisterMemoryDomain / TrackAlloc (超限路径) 中插入埋点调用
4. 在 `ep_instance.cpp` 的 EpInitialize/EpShutdown 中注册/反注册桥接
5. 更新 diagnostics CMakeLists.txt

## Exit Criteria

- 3 个事件类型定义完成
- ep_memorydomain_bridge.h/.cpp 创建完成
- chaos_eventpipe 构建通过
- 3 个桥接事件均可用
