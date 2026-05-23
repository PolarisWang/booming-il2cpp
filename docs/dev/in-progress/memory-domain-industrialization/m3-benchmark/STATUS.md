# M3 — 域操作基准测试

> **task_id**: m3-benchmark
> **parent_task_id**: memory-domain-industrialization
> **source_task_id**: memory-domain-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: ready
> **entry_skill**: dev-executing-plans

## Scope

创建 memory domain 基准测试，覆盖主要操作路径的性能基线。

## Implementation

创建 `testing/src/native/runtime-core/gc/memory_domain_benchmark.cpp`

使用 GoogleTest benchmark 模式（简单循环计时），注册 mock 模块并调用真实 domain API：
- PushDomain / PopDomain — 100000 次
- DomainCurrentAllocateTagged / DomainFreeTagged — 100000 次
- DomainCurrentReallocateTagged — 10000 次
- FindDomainById / FindDomainByName — 100000 次
- RegisterMemoryDomain / UnregisterMemoryDomain — 10000 次

测试注册在 `testing/src/native/runtime-core/gc/CMakeLists.txt`，labeled "benchmark;core"。
