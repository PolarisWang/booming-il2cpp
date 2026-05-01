---
task_id: 20260430-01-ldind-stind-full-series
title: ldind/stind Full Series Implementation
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-30 00:00:00 +08:00
updated_at: 2026-05-01 14:30:00 +08:00
current_dir: docs/dev/completed/20260430-01-ldind-stind-full-series
parent_task_id:
source_task_id:
source_relation: standalone
clearance_source: direct user confirmation
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
design_doc: n/a
plan_doc: docs/dev/completed/20260430-01-ldind-stind-full-series/plan-v1-01.md
active: false
---

# 20260430-01 ldind/stind Full Series

## Outcome

- C++ 侧 `chaos_load_indirect<T>` / `chaos_store_indirect<T>` 泛型模板已在 `native_types.h` 实现
- C# 侧 `EmitLoadIndirect` / `EmitStoreIndirect` 参数化发射器 + 15 个 switch case（ldind.i4/u1/i1/u2/i2/u4/i8/r4/r8/ref + stind.i4/i1/i2/i8/r4/r8/ref）已在 `MethodEmission.cs` 实现
- 旧方法 `EmitLoadIndirectInt32` / `EmitStoreIndirectInt32` 已删除
- 零抽象开销：C++ 模板编译内联后与手写代码等价

## Completion Evidence

- Code: `src/native/common/chaos/native_types.h` — `chaos_load_indirect`, `chaos_store_indirect`
- Code: `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs` — `EmitLoadIndirect`, `EmitStoreIndirect` + full switch dispatch
- Old `EmitLoadIndirectInt32`/`EmitStoreIndirectInt32` — confirmed deleted (grep returns no matches)
