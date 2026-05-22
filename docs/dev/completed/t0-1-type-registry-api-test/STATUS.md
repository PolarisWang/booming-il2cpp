# T0-1: type_registry API 单元测试

> **task_id**: t0-1-type-registry-api-test
> **parent_task_id**: 20260523-typesystem-vtable-industrialization
> **source_task_id**: 20260523-typesystem-vtable-industrialization
> **source_relation**: roadmap-child
> **创建日期**: 2026-05-23
> **完成日期**: 2026-05-23
> **phase**: completed
> **lifecycle_status**: question_clearance = cleared
> **clearance_source**: parent-roadmap
> **blocking_questions**: []
> **question_clearance**: cleared
> **clearance_confirmed_by_user**: true

## Scope

为以下 API 编写隔离单元测试：
1. `chaos_register_type(name, parent, shape, iface_map, iface_count)` — 动态类型注册
2. `chaos_find_type_by_stable_id(stable_id)` — 按 stable_id 查找动态类型
3. `ChaosTypeAddInterface(type_token, iface_stable_id, offset, count)` — 运行时追加接口映射
4. 边缘情况：重复注册、不存在的 stable_id、空接口映射、接口追加幂等性

## 完成证据

- arch_review: ok
- authority_review: n/a
- verification:
  - canonicalCommand: `ctest -R type_registry_test -C Debug`
  - result: 12/12 passed
- test_result: passed
- wiki: n/a
- next: T0-2 (vtable_registry 隔离测试)
