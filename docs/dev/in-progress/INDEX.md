# 当前进行中的任务

当前 active 任务：
- [reflection-production-readiness](reflection-production-readiness/STATUS.md) — 反射生产机品质补齐（roadmap, 5 阶段串行）

> 相关跟进目录（非独立主线，作为归档/追踪用）：
> - [`aot-exception-type-resolution/`](aot-exception-type-resolution/STATUS.md) — 文件系统异常类型解析链路（根因链 4 层；层 1 + Path 1 已修；含 2026-09-22 误报复盘）

| task_id | task_type | phase | updated_at | next_step | active |
| --- | --- | --- | --- | --- | --- |
| [20260419-01-foundation-dll-translation-audit-roadmap](20260419-01-foundation-dll-translation-audit-roadmap/STATUS.md) | roadmap | roadmap | 2026-04-25 21:58:00 +08:00 | 维护 DLL-first reporting authority，跟踪 phase-2 DLL 推进状态 | false |
| [20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable](20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable/STATUS.md) | plan | blocked-before-start | 2026-04-25 21:58:00 +08:00 | 等待 `20260419-03` 解闸，需要 `System.Collections.Immutable` 的 blocked authority 和 proof | false |
| [20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening](20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/STATUS.md) | plan | implementation | 2026-04-25 21:58:00 +08:00 | 通过 4C family/router framework 扩展 `System.Private.CoreLib` supplemental runtime-skeleton translated coverage | false |
| [20260525-hotupdate-dml-pipeline](20260525-hotupdate-dml-pipeline/STATUS.md) | roadmap | planning | 2026-05-25 | 修复 foundation-dll hotupdate DML 管线 — 使 hotupdate 验证走真实 interpreter 路径 | true |
| [foundation-dll-phase-1-corelib](foundation-dll-phase-1-corelib/STATUS.md) | plan | in-progress | 2026-05-05 | Phase 1 System.Private.CoreLib 全量 il2cpp 开发 | false |
| [runtime-external-reflection-type](runtime-external-reflection-type/STATUS.md) | plan | completed | 2026-05-05 | Runtime External 依赖实现 — Reflection/Type + Exception | false |
| [reflection-production-readiness](reflection-production-readiness/STATUS.md) | roadmap | roadmap | 2026-09-12 | 启动 P0-1 建 ApiSurfaceScanner，先出反射 API 总量数字以定 Phase 2–3 粒度 | true |

| [debug-20-real-defects](debug-20-real-defects/STATUS.md) | plan | ready-to-start | 2026-09-15 | cdb 定位 20 个 realDefect 异常来源 | true |
