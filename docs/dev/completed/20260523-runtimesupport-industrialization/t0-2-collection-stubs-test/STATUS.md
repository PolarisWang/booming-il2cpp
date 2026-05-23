# T0-2: collection_stubs 单元测试

> **parent_task_id**: 20260523-runtimesupport-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **lifecycle_status**: completed
> **phase**: Phase 0

## 完成证据

- arch_review: ok
- verification: collection_stubs_test (34 tests, 3 suites) — 全部通过
- test_result: passed
- next: merge

## Scope

`collection_stubs.cpp` 全部 14 个 public API 的单元测试，覆盖 List、Dictionary、HashSet。

## 交付物

- `testing/src/native/runtime-core/runtime_stubs/collection_stubs_test.cpp`
- `src/native/runtime-core/runtime_stubs/collection_stubs.h`（extern "C" fix）
