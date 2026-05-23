# T0-5: TCVC epoch 缓存行震荡分析+修复

> **parent_task_id**: 20260523-typesystem-vtable-industrialization
> **source_task_id**: t0-5-epoch-cacheline
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **创建日期**: 2026-05-23
> **更新日期**: 2026-05-23
> **task_type**: plan
> **phase**: execution
> **lifecycle_status**: completed
> **child_execution_mode**: auto
> **entry_skill**: dev-executing-plans

## Scope

TCVC 使用全局 `std::atomic<uint32_t>` epoch 作为缓存失效信号。在多线程高频读写下，epoch 所在缓存行会在多个核心之间震荡（true sharing + false sharing），影响性能。

分析结论（已查代码）：
- `GetVTableEpoch()` 和 `GetIfaceEpoch()` 是函数局部静态 `std::atomic<uint32_t>`
- 它们位于 BSS 段，没有对齐或填充保护
- 两个 epoch 变量可能位于同一缓存行（64B 内），造成 false sharing
- 每次 ResolveVirtualMethodPointer 在 TCVC 命中时做 2 次 atomic load（acquire）
- 每次 UpdateVTableSlotByMethodToken 做 1 次 atomic fetch_add（release）

修复方案：
1. 使用 `alignas(64)` 分别对齐两个 epoch 变量到独立缓存行
2. 消除两个 epoch 之间的 false sharing
3. TCVC 自身的 true sharing（所有线程读取同一原子变量）是设计允许的——TCVC 命中减少 epoch 读取频率，链路本身设计正确

## Exit Criteria

- 修复后编译通过: ✅
- 并发压力测试全部通过: ✅ (6/6)
- VTable 查找性能基准不变或提升: ✅ (通过基准测试, 无回归)

## 完成证据

- arch_review: ok (极小改动, 仅两处 alignas(64))
- authority_review: n/a
- verification:
  - formalObject: src/native/runtime-core/vtable_registry.cpp (两行变更)
- test_result: passed (vtable_registry_test 47/47 + vtable_concurrent_stress_test 6/6 + vtable_lookup_benchmark 3/3)
- wiki: n/a
- next: parent-roadmap → Phase 1 (T1-1 ~ T1-10)

- 当前 child 在父 roadmap 已批准边界内：✅
- 变更范围极小（vtable_registry.cpp 两处函数局部静态加 alignas）：✅
- T0-4 已提供并发压力测试作为验证手段：✅

**结果**: `pass`
