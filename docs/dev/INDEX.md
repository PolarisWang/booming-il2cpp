# docs/dev 任务索引

当前 active 任务：
- `20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening`

- [进行中任务索引](in-progress/INDEX.md)
- [挂起任务索引](hanging/INDEX.md)
- [已完成任务索引](completed/INDEX.md)

| task_id | lifecycle_status | phase | updated_at | summary | active |
| --- | --- | --- | --- | --- | --- |
| [20260425-01-runtime-skeleton-4c-handler-framework-roadmap](completed/20260425-01-runtime-skeleton-4c-handler-framework-roadmap/STATUS.md) | completed | completed | 2026-04-25 11:10:52 +08:00 | `runtime-skeleton 4C` roadmap 已收口，family router、Convert/ConvertLike cutover、legacy dispatcher 清理与 formal verification evidence 已完成。 | false |
| [20260425-02-runtime-skeleton-4c-framework-foundation-cutover](completed/20260425-02-runtime-skeleton-4c-framework-foundation-cutover/STATUS.md) | completed | completed | 2026-04-25 11:10:52 +08:00 | 4C framework foundation cutover 已完成，`UnsupportedReason` contract、generic convert fallback 与 legacy family 清理已通过 canonical gate。 | false |
| [20260424-01-c3-verification-topology-and-unified-entry](completed/20260424-01-c3-verification-topology-and-unified-entry/STATUS.md) | completed | completed | 2026-04-24 06:00:00 +08:00 | verification-v1 hard cut 已完成，`verification/*` 成为唯一 authority/public entry。 | false |
| [20260423-05-verification-v1-gap-reopen](completed/20260423-05-verification-v1-gap-reopen/STATUS.md) | completed | completed | 2026-04-23 17:59:31 +08:00 | verification-v1 reopen 缺口已关闭，formal `codegen-stubs` 已落地，旧公开验证入口已退场。 | false |
| [20260423-01-verification-v1-roadmap](completed/20260423-01-verification-v1-roadmap/STATUS.md) | completed | completed | 2026-04-23 17:59:31 +08:00 | verification-v1 roadmap 已完成，formal source 固定到 `verification/*` 与 `subjects/*/verification/codegen-stubs/*`。 | false |
| [20260423-04-verification-v1-legacy-retirement](completed/20260423-04-verification-v1-legacy-retirement/STATUS.md) | completed | completed | 2026-04-23 14:12:40 +08:00 | 旧 benchmark docs authority consumer 已清理，旧测试流程入口已退化为 legacy。 | false |
| [20260423-03-verification-v1-wiki-cutover](completed/20260423-03-verification-v1-wiki-cutover/STATUS.md) | completed | completed | 2026-04-23 14:12:40 +08:00 | verification-v1 wiki canonical 入口切换已完成。 | false |
| [20260423-02-verification-v1-projection-cutover](completed/20260423-02-verification-v1-projection-cutover/STATUS.md) | completed | completed | 2026-04-23 14:12:40 +08:00 | inventory、benchmark 与首页固定视图已切到 formal source / projection generator。 | false |
| [20260419-07-corelib-dll-boundary-native-codegen-implementation](completed/20260419-07-corelib-dll-boundary-native-codegen-implementation/STATUS.md) | completed | completed | 2026-04-24 08:40:00 +08:00 | 窄边界 CoreLib assembly-bound runtime-skeleton widening 与 subject review bundle 主链接线已完成。 | false |
| [20260419-08-scriban-generator-formalization](hanging/20260419-08-scriban-generator-formalization/STATUS.md) | hanging | hanging | 2026-04-21 23:45:00 +08:00 | 用户要求暂挂 `20260419` 主线，恢复后按 `plan-v1-01.md` 先补 RED 测试再执行单次原地 cutover。 | false |
| [20260419-01-foundation-dll-translation-audit-roadmap](in-progress/20260419-01-foundation-dll-translation-audit-roadmap/STATUS.md) | in-progress | roadmap | 2026-04-25 21:58:00 +08:00 | DLL-first reporting sync 已补齐，`System.Collections.Immutable` 已有正式 blocked STATUS 入口，CoreLib canonical audit authority 同步收敛到 `20260419-37`。 | false |
| [20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable](in-progress/20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable/STATUS.md) | in-progress | blocked-before-start | 2026-04-25 21:58:00 +08:00 | `System.Collections.Immutable` 已有正式 DLL-first 审计入口，但在 `20260419-03` 真正关闸前仅保持 blocked authority。 | false |
| [20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening](in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/STATUS.md) | in-progress | implementation | 2026-04-25 21:58:00 +08:00 | 当前 active child 持续扩大 `System.Private.CoreLib` supplemental runtime-skeleton translated coverage，并作为 canonical CoreLib audit authority。 | true |
| [20260419-03-system-private-corelib-full-verification](completed/20260419-03-system-private-corelib-full-verification/STATUS.md) | completed | completed-blocked | 2026-04-24 21:20:00 +08:00 | 已不再只有窄 proof packet，但真实 CoreLib supplemental runtime-skeleton 仍存在大量 uncovered methods，`fullCoreLibTranslated = false`。 | false |
