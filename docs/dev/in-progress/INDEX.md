# 当前进行中的任务

当前 active 任务：
- none

| task_id | task_type | phase | updated_at | next_step | active |
| --- | --- | --- | --- | --- | --- |
| [20260419-01-foundation-dll-translation-audit-roadmap](20260419-01-foundation-dll-translation-audit-roadmap/STATUS.md) | roadmap | roadmap | 2026-04-25 21:58:00 +08:00 | 维护 DLL-first reporting authority，跟踪 phase-2 DLL 推进状态 | false |
| [20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable](20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable/STATUS.md) | plan | blocked-before-start | 2026-04-25 21:58:00 +08:00 | 等待 `20260419-03` 解闸，需要 `System.Collections.Immutable` 的 blocked authority 和 proof | false |
| [20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening](20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/STATUS.md) | plan | implementation | 2026-04-25 21:58:00 +08:00 | 通过 4C family/router framework 扩展 `System.Private.CoreLib` supplemental runtime-skeleton translated coverage | false |
| [20260525-hotupdate-dml-pipeline](20260525-hotupdate-dml-pipeline/STATUS.md) | roadmap | planning | 2026-05-25 | 修复 foundation-dll hotupdate DML 管线 — 使 hotupdate 验证走真实 interpreter 路径 | true |
| [half-op-explicit-c3861](half-op-explicit-c3861/handoff-2026-08-26.md) | debug | closed | 2026-08-26 | 已闭合: AOT+JIT 彻底从零 build 均通过(C3861 不复现); build.py JIT 旧 page 残留是唯一残留(交 build-fixer)——核心修复=SdkEmitter.CopyNativeLibs stale-lib 刷新(commit 8ed4ddcb2) | false |
| [fact-266-stub-codegen](fact-266-stub-codegen/handoff-2026-08-27.md) | debug | planned | 2026-08-27 | CodeGen 专项: system-2 fact 266 个 failed(全 value=0 fallback stub)需生成真实实现; 前置已闭合(entry 崩溃修复 db58077f3); 分步识别-批修-fact门禁 | false |
| [foundation-dll-phase-1-corelib](foundation-dll-phase-1-corelib/STATUS.md) | plan | in-progress | 2026-05-05 | Phase 1 System.Private.CoreLib 全量 il2cpp 开发 | false |
| [runtime-external-reflection-type](runtime-external-reflection-type/STATUS.md) | plan | completed | 2026-05-05 | Runtime External 依赖实现 — Reflection/Type + Exception | false |
| [gc-align-coreclr](gc-align-coreclr/STATUS.md) | roadmap | roadmap | 2026-08-10 | 代码级对齐 CoreCLR GC；next=GC-A1(young GC 全根集扫描) | true |
| [repo-cleanliness](repo-cleanliness/dev-plan.md) | techdebt | completed | 2026-08-27 | 根目录残留清理(删根级CMake构建目录/日志/NuGet/artifacts+build释放约13GB) + 监控(pre-commit硬门+Claude实时hook双保险)；计划详见 dev-plan.md | false |
