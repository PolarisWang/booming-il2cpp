# docs/dev 任务索引

当前 active 任务：

- `无`

- [进行中任务索引](in-progress/INDEX.md)
- [挂起任务索引](hanging/INDEX.md)
- [已完成任务索引](completed/INDEX.md)

| task_id | lifecycle_status | phase | updated_at | summary | active |
| --- | --- | --- | --- | --- | --- |
| [20260424-01-c3-verification-topology-and-unified-entry](completed/20260424-01-c3-verification-topology-and-unified-entry/STATUS.md) | completed | completed | 2026-04-24 06:00:00 +08:00 | verification-v1 hard cut 已完成收口：`verification/*` 成为唯一 authority/public entry，真实 `verify` / `generate project all` 与旧路径扫描全部通过。 | false |
| [20260423-05-verification-v1-gap-reopen](completed/20260423-05-verification-v1-gap-reopen/STATUS.md) | completed | completed | 2026-04-23 17:59:31 +08:00 | verification-v1 reopen 缺口已关闭：formal `codegen-stubs` 正式落地，旧公开验证入口已退役，并已按 roadmap 重跑回归与 `test inventory`。 | false |
| [20260423-01-verification-v1-roadmap](completed/20260423-01-verification-v1-roadmap/STATUS.md) | completed | completed | 2026-04-23 17:59:31 +08:00 | verification-v1 roadmap 已完成收口：formal source 固定到 `docs/testing-inventory/verification/*` 与 `subjects/*/verification/codegen-stubs/*`，旧公共验证入口已退出主线。 | false |
| [20260423-04-verification-v1-legacy-retirement](completed/20260423-04-verification-v1-legacy-retirement/STATUS.md) | completed | completed | 2026-04-23 14:12:40 +08:00 | 旧 benchmark docs authority consumer 已清零，旧测试流程入口已退役为 legacy，run-id 仅保留在非 formal 的临时 artifacts 链路。 | false |
| [20260423-03-verification-v1-wiki-cutover](completed/20260423-03-verification-v1-wiki-cutover/STATUS.md) | completed | completed | 2026-04-23 14:12:40 +08:00 | `Verification-V1测试流程规范.md` 已成为 wiki canonical 入口，`wiki/06-测试验证/INDEX.md` 已完成 canonical 切换，旧 legacy 导航已下线。 | false |
| [20260423-02-verification-v1-projection-cutover](completed/20260423-02-verification-v1-projection-cutover/STATUS.md) | completed | completed | 2026-04-23 14:12:40 +08:00 | inventory、benchmark 与首页固定视图已全部切到 formal source / projection generator，真实命令链重验通过。 | false |
| [20260419-07-corelib-dll-boundary-native-codegen-implementation](hanging/20260419-07-corelib-dll-boundary-native-codegen-implementation/STATUS.md) | hanging | hanging | 2026-04-21 23:45:00 +08:00 | 用户要求暂停 20260419 主线；恢复后从 `translationUnitMethodCount = 125`、`runtimeSkeletonReservedStubCount = 0`、`fullCoreLibTranslated = false` 的 canonical CoreLib proof 边界继续 widening。 | false |
| [20260419-08-scriban-generator-formalization](hanging/20260419-08-scriban-generator-formalization/STATUS.md) | hanging | hanging | 2026-04-21 23:45:00 +08:00 | 用户要求暂停 20260419 主线；恢复后按 `plan-v1-01.md` 先补 RED 测试，再执行 `Chaos.IL2CPP.Generator` 单次原地 cutover。 | false |
| [20260419-01-foundation-dll-translation-audit-roadmap](hanging/20260419-01-foundation-dll-translation-audit-roadmap/STATUS.md) | hanging | hanging | 2026-04-21 23:45:00 +08:00 | 用户要求暂停 20260419 主线；保留当前 audit roadmap、CoreLib blocker 结论与子任务分解，待恢复后再继续推进。 | false |
| [20260419-03-system-private-corelib-full-verification](completed/20260419-03-system-private-corelib-full-verification/STATUS.md) | completed | completed-blocked | 2026-04-19 13:55:00 +08:00 | CoreLib audit skeleton 与 blocker normalization 已完成；full verification 仍 blocked，不能继续推进 Complex BCL 13 DLL。 | false |
