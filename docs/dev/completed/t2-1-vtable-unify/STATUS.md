# T2-1: vtable_registry 双路径合并

> **parent_task_id**: 20260523-typesystem-vtable-industrialization
> **source_task_id**: t2-1-vtable-unify
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **创建日期**: 2026-05-23
> **更新日期**: 2026-05-23
> **task_type**: plan
> **phase**: completed
> **lifecycle_status**: completed
> **child_execution_mode**: auto
> **entry_skill**: dev-executing-plans

## Scope

消除 vtable_registry 双路径技术债。当前 `ResolveVirtualMethodPointer` 使用两种路径：
- **Path A**（继承链 + VTableSlot 线性扫描）：遍历 base_token 链，在每个类型的 VTableSlot[] 中线性查找 method_token
- **Path B**（vtable_array）：接口分派已走 vtable_array[slot]

目标：建立 `method_token → slot_index` 映射，使所有虚方法分派都走 `vtable_array[slot]`。

**方案**：在 TypeVTable 中增加 per-type `Map<method_token, slot_index>`，在注册时构建一次，ResolveVirtualMethodPointer 改为单次 map 查找 + vtable_array[slot]。

## Preflight Self-Review

- 当前 child 在父 roadmap 已批准边界内: ✅
- 测试安全网已建立（T0-2: 47 tests, T0-4: 6 concurrent tests）: ✅
- Phase 1 已完成，A4-Dual+V2 布局正确: ✅
- 需要修改 vtable_registry.h（TypeVTable 结构体）和 vtable_registry.cpp（注册+查找逻辑）: ✅
- 向后兼容：原有 ResolveVirtualMethodPointer 签名不变: ✅

**结果**: `pass`

## Exit Criteria

- ResolveVirtualMethodPointer 在 path A 场景走 vtable_array[slot]（不再线性扫描 VTableSlot）
- 注册时构建 slot_map，不额外增加 O(n) 复杂度
- 47 个 vtable_registry_test 全部通过
- 6 个并发压力测试通过
- 性能基准不低于当前基线

## 完成证据

- arch_review: ok
- authority_review: n/a
- verification:
  - canonicalCommand: ./build/testing/runtime-core/Release/vtable_registry_test.exe && ./build/testing/runtime-core/Release/vtable_concurrent_stress_test.exe && ./build/testing/runtime-core/Release/vtable_lookup_benchmark.exe
  - formalObject: vtable_registry_test (47 tests)
  - summaryPath: docs/dev/in-progress/20260523-typesystem-vtable-industrialization/children/t2-1-vtable-unify/STATUS.md
  - eventsPath: n/a
  - consolePath: n/a
- verification_data:
  - refreshCommand: n/a
  - verificationOutputRoot: n/a
  - latestResultPath: n/a
  - masterResultPath: n/a
  - reportSummaryPath: n/a
  - codegenStubPath: n/a
- test_result:
  - vtable_registry_test: 47/47 passed
  - vtable_concurrent_stress_test: 6/6 passed
  - vtable_lookup_benchmark: 3/3 passed (baseline maintained: direct 2.6ns, chain 3.0ns)
  - type_registry_test: 12/12 passed
  - test_hotupdate_vtable_sync: 5/5 passed
  - test_hotupdate_verification: 1/1 passed
- wiki: n/a
- next: T2-2 (vtable 一致性验证断言)
