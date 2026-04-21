# docs/dev 任务索引

当前 active 任务以 [ACTIVE.md](ACTIVE.md) 为准。

本页本轮只收口 `2026-04-21` 之前遗留任务；`20260421-*` 当前执行流由其他进程维护。

- [进行中任务索引](in-progress/INDEX.md)
- [挂起任务索引](hanging/INDEX.md)
- [已完成任务索引](completed/INDEX.md)

| task_id | lifecycle_status | phase | updated_at | summary | active |
| --- | --- | --- | --- | --- | --- |
| [20260419-07-corelib-dll-boundary-native-codegen-implementation](hanging/20260419-07-corelib-dll-boundary-native-codegen-implementation/STATUS.md) | hanging | hanging | 2026-04-21 23:45:00 +08:00 | 用户要求暂停 20260419 主线；当前 canonical CoreLib proof 边界冻结在 `translationUnitMethodCount = 125`、`runtimeSkeletonReservedStubCount = 0`、`fullCoreLibTranslated = false`。 | false |
| [20260419-08-scriban-generator-formalization](hanging/20260419-08-scriban-generator-formalization/STATUS.md) | hanging | hanging | 2026-04-21 23:45:00 +08:00 | 用户要求暂停 20260419 主线；`Chaos.IL2CPP.Generator` cutover 保持在方案确认完成、尚未开始正式 TDD/cutover 的状态。 | false |
| [20260419-01-foundation-dll-translation-audit-roadmap](hanging/20260419-01-foundation-dll-translation-audit-roadmap/STATUS.md) | hanging | hanging | 2026-04-21 23:45:00 +08:00 | 用户要求暂停 20260419 主线；保留当前 audit roadmap、CoreLib blocker 结论与子任务分解，待恢复后再继续推进。 | false |
| [20260419-03-system-private-corelib-full-verification](completed/20260419-03-system-private-corelib-full-verification/STATUS.md) | completed | completed-blocked | 2026-04-19 13:55:00 +08:00 | CoreLib audit skeleton 与 blocker normalization 已完成；full verification 仍 blocked，不能进入 Complex BCL 13 DLL。 | false |
| [20260419-06-corelib-hotupdate-proof-normalization](completed/20260419-06-corelib-hotupdate-proof-normalization/STATUS.md) | completed | completed | 2026-04-19 13:50:00 +08:00 | 已归一化 CoreLib-specific hotupdate proof 要求，HotUpdateHostPack 旧验证仅作为 related evidence。 | false |
| [20260419-05-corelib-full-native-proof-normalization](completed/20260419-05-corelib-full-native-proof-normalization/STATUS.md) | completed | completed | 2026-04-19 13:40:00 +08:00 | 已归一化 CoreLib DLL-boundary native proof 要求，六个 semantic family 均需明确证据。 | false |
| [20260419-04-corelib-codegen-review-inventory](completed/20260419-04-corelib-codegen-review-inventory/STATUS.md) | completed | completed | 2026-04-19 13:30:00 +08:00 | 已确认现有 generated artifacts 是 entry-driven，不能关闭 CoreLib DLL-boundary codegen review blocker。 | false |
| [20260419-02-audit-schema-and-foundation-solution-freeze](completed/20260419-02-audit-schema-and-foundation-solution-freeze/STATUS.md) | completed | completed | 2026-04-19 12:40:00 +08:00 | Phase 0 完成：audit/codegen schema 与 `FoundationDllTranslationSolution` 骨架已冻结，测试和 Release build 通过。 | false |
| [20260418-01-dotnet-foundation-pack-nativeization-roadmap](completed/20260418-01-dotnet-foundation-pack-nativeization-roadmap/STATUS.md) | completed | completed | 2026-04-19 10:22:00 +08:00 | Roadmap 已完成：dependency-layer 终态、HotUpdate consumer validation、`.NET 10` delta closure 均已收口。 | false |
