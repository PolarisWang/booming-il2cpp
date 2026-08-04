# T0-4: 并发/压力测试

> **parent_task_id**: 20260523-typesystem-vtable-industrialization
> **source_task_id**: t0-4-concurrent-stress
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

编写 vtable_registry 并发/压力测试，覆盖以下场景：

1. **多线程 TCVC 并发填充** — 多个线程同时调用 ResolveVirtualMethodPointer，验证 TCVC 在线程安全（读-读并发）下的正确性
2. **128+ VTable 扩展性** — 注册大量类型（128+），验证 TryGetTypeVTable / ResolveVirtualMethodPointer 在注册表扩展后的正确性
3. **epoch 读-写冲突** — 读线程（ResolveVirtualMethodPointer）与写线程（UpdateVTableSlotByMethodToken）并发，验证 epoch 传播不丢失更新

## Exit Criteria

- vtable_concurrent_stress_test 可编译运行: ✅
- 所有并发测试通过: ✅ (6/6 passed)
- 测试目标带有 LABELS "stress;core": ✅

## 完成证据

- arch_review: ok
- authority_review: n/a
- verification:
  - canonicalCommand: cmake --build build/testing --target vtable_concurrent_stress_test --config Release && build/testing/runtime-core/Release/vtable_concurrent_stress_test.exe
  - formalObject: testing/src/native/runtime-core/vtable_concurrent_stress_test.cpp
  - summaryPath: n/a
  - eventsPath: n/a
  - consolePath: n/a
- test_result: passed
- wiki: n/a
- next: parent-roadmap → T0-5

## 测试结果

| Test | Status | Details |
|------|--------|---------|
| ConcurrentReadStress | ✅ PASS | 4 threads × 10000 iters, 0 errors total |
| ConcurrentInterfaceLookup | ✅ PASS | 4 threads × 10000 iters, 0 errors total |
| ConcurrentTryGetTypeVTable | ✅ PASS | 4 threads × 10000 iters, 0 errors total |
| Scalability128Plus | ✅ PASS | 150/150 found, 150/150 resolved |
| ReadWriteContention | ✅ PASS | 2s: 4.6M reads + 2.2M writes concurrent, no crashes |
| InterfaceEpochContention | ✅ PASS | 2s: 182M lookups + concurrent register, no crashes |
