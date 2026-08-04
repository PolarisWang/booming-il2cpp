---
task_id: 20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening
title: Phase 4 AOT Core IR And Native Codegen Strengthening
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 04:18:56 +08:00
updated_at: 2026-04-15 07:15:00 +08:00
current_dir: docs/dev/completed/20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening
parent_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_status: docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/STATUS.md
- parent_roadmap: docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-02.md
- parent_review: docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/review-v1-02.md
- phase_2_status: docs/dev/completed/20260415-03-phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation/STATUS.md
- phase_3_status: docs/dev/completed/20260415-04-phase-3-hotupdate-runtime-and-supplemental-metadata-foundation/STATUS.md
- plan: docs/dev/completed/20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening/plan-v1-01.md
- gap_ledger: docs/dev/completed/20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening/aot-core-ir-codegen-gap-ledger-v1-01.md

## 当前结论

- 本子任务已完成，但其作用现在被重新解释为 `Phase 4A`，而不是父 roadmap 原定义下完整的 `Phase 4`。
- 已完成范围：
  - `AotCoreIr` 正式进入 managed closure contract
  - `CodeGenStage` 改为 `typed-il -> AotCoreIr -> NativeAotEmitter`
  - `NativeAotEmitter` 打通最小静态 direct-call + reachable helper emission
  - `ManagedInstructionReference`、`NativeSymbol` 与 direct-call metadata 进入中层
  - 当前 codegen gap 被显式冻结
- 未完成范围：
  - object model
  - generic / sharing
  - metadata closure
  - exceptions
  - dispatch widening
  - ABI/type-system widening
- 因此：
  - 本子任务继续保持 completed
  - 但父 roadmap 已重新打开，并将本子任务视作 `Phase 4A` 已完成切片

## 最近摘要

- 2026-04-15 04:18:56 +08:00: 创建子任务，锁定 `AotCoreIr` contract、closure artifact 与 emitter 消费边界。
- 2026-04-15 04:55:11 +08:00: 完成 Task 1-2；`AotCoreIr` contract、direct-call metadata 与 reachable helper emission 落地。
- 2026-04-15 05:02:07 +08:00: 完成 Task 3-4；instruction reference carrier 进入 `AotCoreIr`，并建立 gap ledger。
- 2026-04-15 07:15:00 +08:00: 父 roadmap 经 post-closeout 审查后重新打开；本子任务被重新归类为 `Phase 4A` 已完成切片。

## 下一步

- next_action: 由父 roadmap 跟踪并执行 `20260415-06-phase-4b-aot-core-ir-object-model-and-runtime-surface`。
- owner: codex
- trigger: parent roadmap `roadmap-v1-02.md` 已把 4B-4D 重新列为待执行阶段，且 4B 子任务已经正式建档。

## 风险 / 阻塞

### risks

- 如果后续继续直接 patch emitter，而不是先扩 `AotCoreIr` contract，会破坏本子任务作为“正式中层入口”的意义。
- 如果把本子任务误读成“完整 Phase 4 已完成”，会继续误导 Phase 5/6 的关闭判断。

### blockers

- 当前无功能 blocker。

## 验证

1. python -m pytest tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/compatibility/test_multi_assembly_entry_override.py::MultiAssemblyEntryOverrideTests::test_driver_supports_multi_assembly_entry_override_for_closure_and_native_aot -q
   - 结果：5 passed
2. python -m pytest tests/unit/compatibility/test_managed_closure_contract_bundle.py tests/unit/compatibility/test_multi_assembly_entry_override.py tests/unit/performance/test_native_aot_workload_entry_bundle.py -q
   - 结果：9 passed
3. dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1
   - 结果：Build succeeded（保留既有 nullable warnings）
4. dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1
   - 结果：Build succeeded（保留既有 nullable warning）
5. python -m pytest tests/unit/compatibility -q
   - 结果：179 passed

## wiki

- 本子任务继续仅在 docs/dev 沉淀执行痕迹。
- 长期结构说明应跟随后续 4B-4D 完成度统一整理。
