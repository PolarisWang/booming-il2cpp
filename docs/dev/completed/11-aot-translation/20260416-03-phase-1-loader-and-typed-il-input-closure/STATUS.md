---
task_id: 20260416-03-phase-1-loader-and-typed-il-input-closure
title: Phase 1 Loader And Typed-IL Input Closure
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-16 09:20:00 +08:00
updated_at: 2026-04-16 09:31:14 +08:00
current_dir: docs/dev/completed/20260416-03-phase-1-loader-and-typed-il-input-closure
parent_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_relation: roadmap-child-batch-a
design_doc: docs/dev/completed/20260416-03-phase-1-loader-and-typed-il-input-closure/design-v1-01.md
plan_doc: docs/dev/completed/20260416-03-phase-1-loader-and-typed-il-input-closure/plan-v1-01.md
total_tasks: 5
current_task: completed
active: false
---

## 关键文档

- parent_status: `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/STATUS.md`
- parent_roadmap: `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/roadmap-v1-01.md`
- design: `docs/dev/completed/20260416-03-phase-1-loader-and-typed-il-input-closure/design-v1-01.md`
- plan: `docs/dev/completed/20260416-03-phase-1-loader-and-typed-il-input-closure/plan-v1-01.md`
- progress_note: `docs/dev/completed/20260416-03-phase-1-loader-and-typed-il-input-closure/notes/progress-20260416-0931.md`

## 当前结论

- 本批次完成了 `LoaderStage` 对两类真实 C# failing surface 的输入闭环：
  - `TypedReference`：`mkrefany` / `refanytype` / `refanyval`
  - `sizeof(non-primitive struct)`：`sizeof`
- 新增 `tests/unit/compatibility/test_phase1_loader_typed_reference_decode.py`，使用真实 C# fixture 验证 `loader -> typed-il -> aot-core-ir` 三层工件都保留了正确的 opcode、operand、resultType 与 type reference。
- 本批次没有扩展 planner / emitter 语义；`Phase 1` 剩余 metadata shape 继续留在后续 loader batch 处理。

## 最近摘要

- 2026-04-16 09:20:00 +08:00: 冻结首个 Phase 1 真实 failing surface，确认 `TypedReference` 在 loader 处因 `Mkrefany` 直接失败。
- 2026-04-16 09:26:00 +08:00: 通过 TDD 新增 compatibility 测试，先观察 `Mkrefany` / `Sizeof` 的真实失败，再按最小范围补 loader decode。
- 2026-04-16 09:31:14 +08:00: 完成 `TypedReference + sizeof` loader widening，局部 compatibility 回归结果为 `8 passed`。

## 下一步

- next_action: 无。本 child task 已完成；父 roadmap 继续推进 `20260416-09-phase-1b-member-reference-parent-and-vararg-loader-closure`。
- owner: codex
- trigger: 用户要求继续执行 roadmap。

## 风险 / 阻塞

### risks

- 仍有 `MemberReference.Parent = MethodDefinition`、`__arglist`、以及其它未冻结的 metadata shape 没有收口，不能把整个 `Phase 1` 误判为已完成。
- 如果后续 batch 把 vararg / member-reference parent / ldtoken shape 混成一个大补丁，会重新增加定位成本。

### blockers

- 当前无外部 blocker。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase1_loader_typed_reference_decode.py tests/unit/compatibility/test_phase7_loader_function_pointer_decode.py tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py tests/unit/compatibility/test_loader_switch_opcode.py -q`
  - 结果：`8 passed`

## wiki

- 暂不写入 wiki；等待 `Phase 1` 后续 loader batch 一并稳定后再决定是否沉淀。
