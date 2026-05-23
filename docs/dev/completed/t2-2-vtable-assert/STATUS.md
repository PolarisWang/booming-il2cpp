# T2-2: vtable 一致性验证断言

> **parent_task_id**: 20260523-typesystem-vtable-industrialization
> **source_task_id**: t2-2-vtable-assert
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

在 vtable 注册和更新关键函数中添加 `CHAOS_IL2CPP_ASSERT` 一致性断言，确保 slots array 与 flat vtable_array 的对应关系始终正确。仅在 CHECK 构建中启用（ASSERT 自动由 LOG_LEVEL 控制）。

断言位置：

1. **RegisterTypeVTable** — 注册后验证：对每个 slot，slots[i].method_token != 0 时，vtable_array[i] == slots[i].method_pointer
2. **RegisterCodegenVTable** — 注册后验证（通过 RegisterTypeVTable）
3. **RegisterHotUpdateVTable** — 构建 vtable_array 并应用 override 后，验证一致性
4. **UpdateVTableSlotByMethodToken** — 更新后验证：对匹配的 slot，vtable_array[i] == new_method
5. **BuildRuntimeVTable** — 复制父 vtable_array 后，验证一致性

## Exit Criteria

- 5 个断言点全部添加（RegisterTypeVTable / RegisterHotUpdateVTable / UpdateVTableSlotByMethodToken / BuildRuntimeVTable）
- 仅在 CHECK 构建中启用（使用 CHAOS_IL2CPP_ASSERT）
- 47 个 vtable_registry_test 全部通过
- 6 个并发压力测试通过
- 性能基准不受影响（ASSERT 在 Release/SHIP 构建中编译消除）

## 完成证据

- arch_review: ok
- authority_review: n/a
- verification:
  - canonicalCommand: ./build/testing/runtime-core/Release/vtable_registry_test.exe && ./build/testing/runtime-core/Release/vtable_concurrent_stress_test.exe && ./build/testing/runtime-core/Release/vtable_lookup_benchmark.exe
  - formalObject: vtable_registry_test (47 tests)
  - summaryPath: docs/dev/completed/t2-2-vtable-assert/STATUS.md
- test_result:
  - vtable_registry_test: 47/47 passed
  - vtable_concurrent_stress_test: 6/6 passed
  - vtable_lookup_benchmark: 3/3 passed
- wiki: n/a
- next: T2-3 (EEClass 动态类型填充补齐)
