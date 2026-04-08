# Ownership Map (Roadmap 0)

## Track Ownership

- Managed core track: `src/managed/Chaos.IL2CPP.*`
- Native core track: `src/native/`
- Subject track: `subjects/`
- Verification track: `tests/`
- Contract track: `contracts/`
- Build and orchestration track: `build/`
- Artifact stewardship: `artifacts/`
- Deployment stewardship: `deploy/`
- Dependency stewardship: `third_party/`

## Analysis Capability Ownership

当前“analysis”是能力名称，不再对应顶层目录。

- 语义分析与装载能力位于 `src/managed/Chaos.IL2CPP.Loader` 与 `src/managed/Chaos.IL2CPP.SemanticWorld`
- managed closure / metadata / codegen 编排位于 `src/managed/Chaos.IL2CPP.Linker`、`src/managed/Chaos.IL2CPP.MetadataWriter`、`src/managed/Chaos.IL2CPP.CodeGen`、`src/managed/Chaos.IL2CPP.Pipeline`
- run 期 analysis 证据位于 `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/...`

## Guidance

- 跨轨改动需要同步更新 `contracts/` 或对应架构文档
- 若目录职责发生变化，应先更新本文件与 `repo-layout.md`
- 新实现禁止重新引入顶层 `analysis/` 作为源码根或 contract 根
