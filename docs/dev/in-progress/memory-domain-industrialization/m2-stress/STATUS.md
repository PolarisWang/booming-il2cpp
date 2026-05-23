# M2 — 压力/Soak 测试

> **task_id**: m2-stress
> **parent_task_id**: memory-domain-industrialization
> **source_task_id**: memory-domain-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: ready
> **entry_skill**: dev-executing-plans

## Scope

创建 memory domain 压力/soak 测试，在高频循环中调用域 API 验证稳定性。

## Implementation

创建 `testing/src/native/runtime-core/gc/memory_domain_stress_test.cpp`

使用 GoogleTest，在高频循环中调用 domain API：
- 域注册/查找/取消注册循环 1000 次
- DomainAllocateTagged/DomainFreeTagged 循环 10000 次
- DomainScope push/pop 循环 1000 次
- 跨域分配释放 1000 次
- 多线程域栈并发（可选）

测试注册在 `testing/src/native/runtime-core/gc/CMakeLists.txt`，使用 add_chaos_test 宏。
