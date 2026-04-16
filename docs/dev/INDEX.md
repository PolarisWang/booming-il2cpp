# docs/dev 任务索引
按 `docs/dev` 生命周期组织任务：
| task_id | lifecycle_status | phase | updated_at | summary | active |
| --- | --- | --- | --- | --- | --- |
| [20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization](in-progress/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization/STATUS.md) | in_progress | executing | 2026-04-16 10:36:50 +08:00 | 已切入 `Phase 3`；当前批次锁定 `ldc.i8 + and/or/xor` planner legalization。 | true |
| [20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap](in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/STATUS.md) | in_progress | roadmap | 2026-04-16 10:36:50 +08:00 | `20260416-04` 已完成；父 roadmap 正在推进 `20260416-05`。 | true |
| [20260416-04-phase-2-aot-core-ir-semantic-surface-expansion](completed/20260416-04-phase-2-aot-core-ir-semantic-surface-expansion/STATUS.md) | completed | completed | 2026-04-16 10:36:50 +08:00 | 已收口 `ldtoken targetReference` 与 `Int64/UInt64` ABI carrier；相邻 compatibility 回归结果为 `30 passed`。 | false |
| [20260416-09-phase-1b-member-reference-parent-and-vararg-loader-closure](completed/20260416-09-phase-1b-member-reference-parent-and-vararg-loader-closure/STATUS.md) | completed | completed | 2026-04-16 09:43:20 +08:00 | 已收口 `MemberReference.Parent = MethodDefinition` 与 `__arglist` 的真实 C# loader 输入面；局部 compatibility 回归结果为 `10 passed`。 | false |
| [20260416-03-phase-1-loader-and-typed-il-input-closure](completed/20260416-03-phase-1-loader-and-typed-il-input-closure/STATUS.md) | completed | completed | 2026-04-16 09:31:14 +08:00 | 已收口 `TypedReference + sizeof(non-primitive struct)` 的真实 C# loader 输入面；局部 compatibility 回归结果为 `8 passed`。 | false |
| [20260416-02-phase-0-gap-ledger-and-owner-evidence-audit](completed/20260416-02-phase-0-gap-ledger-and-owner-evidence-audit/STATUS.md) | completed | completed | 2026-04-16 09:09:27 +08:00 | 已冻结 capability-first gap ledger 与 owner evidence matrix；代表性 compatibility 验证结果为 `28 passed`。 | false |
