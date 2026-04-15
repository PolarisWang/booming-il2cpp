---
task_id: 20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening
title: Phase 4 AOT Core IR And Native Codegen Strengthening
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 04:18:56 +08:00
updated_at: 2026-04-15 06:24:00 +08:00
current_dir: docs/dev/completed/20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening
parent_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_status: docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/STATUS.md
- parent_roadmap: docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-01.md
- parent_design: docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/design-v1-01.md
- phase_2_status: docs/dev/completed/20260415-03-phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation/STATUS.md
- phase_3_status: docs/dev/completed/20260415-04-phase-3-hotupdate-runtime-and-supplemental-metadata-foundation/STATUS.md
- plan: docs/dev/completed/20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening/plan-v1-01.md
- gap_ledger: docs/dev/completed/20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening/aot-core-ir-codegen-gap-ledger-v1-01.md

## 当前结论

- `AotCoreIr` 已正式进入 managed closure contract，`DriverEntry` 会稳定落盘 `aot-core-ir.json`。
- `CodeGenStage` 现在以 `typed-il -> AotCoreIr -> NativeAotEmitter` 为正式主线；`NativeAotEmitter` 不再直接消费 loader model。
- `AotCoreIr` 已携带 `NativeSymbol`、direct-call target metadata 与 `ManagedInstructionReference`，足以承接当前 native-aot 已支持的最小闭环。
- `NativeAotEmitter` 已能基于 `AotCoreIr` 发出 entry method、reachable static helper definition 与最小 direct-call 路径。
- generic / exception / object model / metadata closure 的未覆盖面已通过 `aot-core-ir-codegen-gap-ledger-v1-01.md` 显式冻结；这些缺口不会再以隐式“暂未验证”形式漂移。
- 结论：Phase 4 的退出标准按“建立稳定中层 contract 并显式冻结剩余 gap”口径已满足，可以归档并把后续覆盖扩张交回 parent roadmap 的 Phase 5/6 收口。

## 最近摘要

- 2026-04-15 04:18:56 +08:00: 创建 Phase 4 child task，锁定 `AotCoreIr` contract、closure artifact 暴露面与 `NativeAotEmitter` 消费边界。
- 2026-04-15 04:55:11 +08:00: 完成 Task 1-2；`AotCoreIr` contract、direct-call metadata 与 reachable static helper emission 落地。
- 2026-04-15 05:02:07 +08:00: 完成 Task 3-4；instruction reference carrier 进入 `AotCoreIr`，并建立 codegen gap ledger。
- 2026-04-15 06:24:00 +08:00: 完成 Task 5 收口判断；确认 Phase 4 不再需要继续扩写临时 emitter patch，后续 feature coverage 与 engineering gate 由 parent roadmap 复用既有 owner-subject 闭环继续完成。

## 下一步

- next_action: 返回父 roadmap，执行 Phase 5/6 的复用审计、项目级回归与最终归档。
- owner: codex
- trigger: Phase 4 已完成并归档。

## 风险 / 阻塞

### risks

- `AotCoreIr` 当前仍只覆盖最小 native-aot 子集；后续 generic / exception / object model / metadata closure 扩张必须继续围绕 gap ledger 做增量切片，不能回退成 ad-hoc opcode patch。
- 若后续 owner-subject capability 扩张不回写 translation surface / unsupported ledgers，会重新产生“代码已有，外层证据链缺失”的漂移。

### blockers

- 当前无功能 blocker。
- 本机仍存在遗留输出目录占用风险；涉及 subject 可执行 build/run 的验证继续使用唯一 `BaseOutputPath`。

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
- 待 parent roadmap 完成后，再统一评估是否把 `typed-il -> AotCoreIr -> native` 的长期关系补入 docs/architecture。
