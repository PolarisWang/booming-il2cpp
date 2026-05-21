# Progress v1.01

Date: 2026-04-09 00:05:09 +08:00

## 本轮完成

- 阅读 `docs/discuss/il2cpp-mainline/INDEX.md` 及关键讨论文档，确认 `2026-04-08` 最新 governing rule 已切到 generated native performance-first。
- 检查 `SemanticWorldStage`、`LinkerStage`、`CodeGenStage`、`NativeReferenceProofEmitter`，确认当前实现仍明显 sample-driven。
- 检查现有 subjects 与 manifest，确认仓库缺少同时承接 native output、trace、perf 的完整 subject。
- 新建父级 roadmap 任务 `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`。
- 写入 `brainstorm-v1-01.md`、`design-v1-01.md`、`roadmap-v1-01.md` 与 `STATUS.md`。

## 当前建议

- 推荐保留现有最小 probe subjects。
- 推荐新增完整 subject `MainlineFeaturePack`。
- 推荐先派生 `Phase 1: OptimizationFacts And Native Perf Contracts` child plan。

## 待确认

- 完整 subject 是否接受独立命名与独立宿主。
- 首批 capability 是否接受性能导向排序。
- engine binding 是否明确推迟到本 roadmap 之后。
