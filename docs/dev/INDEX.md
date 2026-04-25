# docs/dev 任务索引

当前 active 任务：

- `20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening`

- [进行中任务索引](in-progress/INDEX.md)
- [挂起任务索引](hanging/INDEX.md)
- [已完成任务索引](completed/INDEX.md)

| task_id | lifecycle_status | phase | updated_at | summary | active |
| --- | --- | --- | --- | --- | --- |
| [20260425-01-runtime-skeleton-4c-handler-framework-roadmap](completed/20260425-01-runtime-skeleton-4c-handler-framework-roadmap/STATUS.md) | completed | completed | 2026-04-25 11:10:52 +08:00 | 独立 `runtime-skeleton 4C` roadmap 已收口：family router、Convert/ConvertLike cutover、legacy dispatcher 删除与 formal verification evidence 全部完成，并已恢复 `20260419` 主线入口。 | false |
| [20260425-02-runtime-skeleton-4c-framework-foundation-cutover](completed/20260425-02-runtime-skeleton-4c-framework-foundation-cutover/STATUS.md) | completed | completed | 2026-04-25 11:10:52 +08:00 | 已完成 4C framework foundation cutover：`UnsupportedReason` contract 落地、generic convert fallback 迁至 `ConvertLike family`、legacy family/dispatcher 清理，并通过 canonical subject 与 `verification-v1` gate。 | false |
| [20260424-01-c3-verification-topology-and-unified-entry](completed/20260424-01-c3-verification-topology-and-unified-entry/STATUS.md) | completed | completed | 2026-04-24 06:00:00 +08:00 | verification-v1 hard cut 已完成收口：`verification/*` 成为唯一 authority/public entry，真实 `verify` / `generate project all` 与旧路径扫描全部通过。 | false |
| [20260423-05-verification-v1-gap-reopen](completed/20260423-05-verification-v1-gap-reopen/STATUS.md) | completed | completed | 2026-04-23 17:59:31 +08:00 | verification-v1 reopen 缺口已关闭：formal `codegen-stubs` 正式落地，旧公开验证入口已退役，并已按 roadmap 重跑回归与 `test inventory`。 | false |
| [20260423-01-verification-v1-roadmap](completed/20260423-01-verification-v1-roadmap/STATUS.md) | completed | completed | 2026-04-23 17:59:31 +08:00 | verification-v1 roadmap 已完成收口：formal source 固定到 `docs/testing-inventory/verification/*` 与 `subjects/*/verification/codegen-stubs/*`，旧公共验证入口已退出主线。 | false |
| [20260423-04-verification-v1-legacy-retirement](completed/20260423-04-verification-v1-legacy-retirement/STATUS.md) | completed | completed | 2026-04-23 14:12:40 +08:00 | 旧 benchmark docs authority consumer 已清零，旧测试流程入口已退化为 legacy，run-id 仅保留在非 formal 的临时 artifacts 链路。 | false |
| [20260423-03-verification-v1-wiki-cutover](completed/20260423-03-verification-v1-wiki-cutover/STATUS.md) | completed | completed | 2026-04-23 14:12:40 +08:00 | `Verification-V1测试流程规范.md` 已成为 wiki canonical 入口，`wiki/06-测试验证/INDEX.md` 已完成 canonical 切换，旧 legacy 导航已下线。 | false |
| [20260423-02-verification-v1-projection-cutover](completed/20260423-02-verification-v1-projection-cutover/STATUS.md) | completed | completed | 2026-04-23 14:12:40 +08:00 | inventory、benchmark 与首页固定视图已全部切到 formal source / projection generator，真实命令链重验通过。 | false |
| [20260419-07-corelib-dll-boundary-native-codegen-implementation](completed/20260419-07-corelib-dll-boundary-native-codegen-implementation/STATUS.md) | completed | completed | 2026-04-24 08:40:00 +08:00 | 已完成窄边界 CoreLib assembly-bound runtime-skeleton widening、subject review bundle 主链接线与重新验证；`20260419-03` 仍因 `fullCoreLibTranslated = false` 保持 blocked。 | false |
| [20260419-08-scriban-generator-formalization](hanging/20260419-08-scriban-generator-formalization/STATUS.md) | hanging | hanging | 2026-04-21 23:45:00 +08:00 | 用户要求暂停 20260419 主线；恢复后按 `plan-v1-01.md` 先补 RED 测试，再执行 `Chaos.IL2CPP.Generator` 单次原地 cutover。 | false |
| [20260419-01-foundation-dll-translation-audit-roadmap](in-progress/20260419-01-foundation-dll-translation-audit-roadmap/STATUS.md) | in-progress | roadmap | 2026-04-25 11:10:52 +08:00 | 独立 `runtime-skeleton 4C` roadmap 已完成并归档；foundation audit 主线已恢复，推荐 child 重新切回 `20260419-37`。 | false |
| [20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening](in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/STATUS.md) | in-progress | plan | 2026-04-25 11:10:52 +08:00 | 当前 active child 已恢复；下一步是在新的 4C framework 上继续扩大 `System.Private.CoreLib` supplemental runtime-skeleton translated coverage。 | true |
| [20260419-03-system-private-corelib-full-verification](completed/20260419-03-system-private-corelib-full-verification/STATUS.md) | completed | completed-blocked | 2026-04-24 21:20:00 +08:00 | 已不再只有窄 proof packet；但真实 CoreLib supplemental runtime-skeleton 仍有 `54310` 个 uncovered methods，`fullCoreLibTranslated = false`。 | false |
