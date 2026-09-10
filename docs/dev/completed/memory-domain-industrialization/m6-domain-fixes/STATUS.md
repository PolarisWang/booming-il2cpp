# M6 — Domain 修复 (R2+R5+R6+R7)

> **task_id**: m6-domain-fixes
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

修复 memory_domain 自身的 4 项已知风险。

## Inputs

### R2：CachedCallInfo 跨域分配不一致
- 文件：interpreter_entry.cpp:616-618
- call_cache 用 new[]/delete[]（raw 域），RuntimeInstantiatedMethod 走 domain
- 修复：改用 `DOMAIN_CURRENT_ALLOCATE` + placement new + placement delete

### R5：TLS 域栈深度硬限制 + 静默失败
- 文件：memory_domain.cpp:264-268
- 嵌套超过 63 层时 PushDomain 静默失败
- 修复：添加 CHAOS_IL2CPP_LOG_ERROR 溢出日志 + return kDomainIdInvalid 语义

### R6：usage_limit 竞态条件
- 文件：memory_domain.h TrackAlloc/TrackFree
- current_usage 增减无互斥保护
- 修复：改用 std::atomic<CHAOS_IL2CPP_INT64> 或 relaxed atomic store

### R7：全局域注册表 O(n) 压缩
- 文件：memory_domain.cpp g_domains
- UnregisterMemoryDomain 的压缩操作是 O(n)
- 修复：替换为 tombstone 标记（is_unloaded=true），惰性清理

## Expected Outputs

- memory_domain.h：current_usage/peak_usage → std::atomic
- memory_domain.cpp：TLS 栈溢出断言 + g_domains tombstones
- interpreter_entry.cpp：R2 修复

## Exit Criteria

- 4 项风险全部清零
- Build 通过
- 现有 memory_domain 测试通过
- 无行为变化（域栈溢出场景除外）
