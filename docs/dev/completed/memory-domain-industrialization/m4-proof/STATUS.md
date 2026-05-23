# M4 — Proof 测试套件

> **task_id**: m4-proof
> **parent_task_id**: memory-domain-industrialization
> **source_task_id**: memory-domain-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **entry_skill**: dev-executing-plans

## Scope

创建 memory domain proof 测试，覆盖关键合约：

- DomainScope RAII 正确性
- 标记分配跨域安全释放
- 域卸载安全
- usage_limit 硬限制
- 多域隔离
- TLS 域栈正确性
- 全局域注册表 tombstone 查找正确性（对应 R7 修复）

## Implementation

创建 `testing/src/native/runtime-core/gc/memory_domain_proof_test.cpp`

在既有 memory_domain_test.cpp 基础上扩展，新增 10+ 个测试用例覆盖上述合约。
