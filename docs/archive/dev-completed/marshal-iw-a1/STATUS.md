---
task_id: a1-test-fixture
parent_task_id: marshal-industrialization
source_relation: roadmap-child
phase: A
lifecycle_status: completed
entry_skill: dev-executing-plans
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# Phase A1 — marshal_test_fixture.h + CMake 集成

## 完成证据

- arch_review: ok
- authority_review: n/a (test infrastructure only)
- verification:
  - canonicalCommand: cmake --build build/testing --target marshal_smoke && ./build/testing/src/native/runtime-core/Debug/marshal_smoke.exe
  - testResult: 7/7 passed
- test_result: passed
- wiki: n/a
- next: a2-test-stubs

## 测试结果

```
[  PASSED  ] 7 tests.
- PreInitBridgeIsAvailable
- RuntimeStateIsAvailable
- MarshalAllocHGlobalReturnsNonNull
- MarshalAllocCoTaskMemReturnsNonNull
- MarshalReadWriteInt32
- MarshalAllocHGlobalNullState
- MarshalAllocCoTaskMemNegativeSize
```

## 关键实现决策

1. **不调用 runtime_init()** — 手动分配 RuntimeState/ThreadState 缓冲区，通过已知偏移写入 RuntimeConfig，避免 BGC 线程崩溃
2. **使用 malloc/free 作为默认分配器** — 无 GC 堆依赖
3. **完整 FullInit() 可选** — RegisterMinimal() + BootstrapMinimal() 保留供需要 codegen bridge 的场景使用
