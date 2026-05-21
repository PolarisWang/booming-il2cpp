# Repo Layout (Roadmap 0)

## Scope

本文描述当前仓库在 Roadmap 0 主线下的正式目录边界。

## Canonical Top-Level Layout

- `src/`: il2cpp 核心实现
- `build/`: 运行入口、toolchain、编排与辅助脚本
- `subjects/`: subject 测试工程正式根，承载 `source/`、`validation/`、`expected/`、`baselines/`
- `tests/`: 通用测试基础设施、通用验证脚本、registry 与平台 gate
- `contracts/`: artifact/native/trace contract 的唯一顶层真源
- `artifacts/`: 运行期和构建期可再生产物
- `deploy/`: 最终可分发或可部署产物
- `third_party/`: vendored 依赖
- `docs/`: 架构文档与执行记录
- `wiki/`: 长期知识沉淀

## Source Layout

- `src/managed/Chaos.IL2CPP.Loader`
- `src/managed/Chaos.IL2CPP.SemanticWorld`
- `src/managed/Chaos.IL2CPP.Linker`
- `src/managed/Chaos.IL2CPP.MetadataWriter`
- `src/managed/Chaos.IL2CPP.Generator`
- `src/managed/Chaos.IL2CPP.Pipeline`
- `src/native/`

这些目录共同承载分析、闭包、metadata、codegen 与 native runtime/bootstrap 能力。

## Explicit Non-Goals

- 根目录 `analysis/` 已退役，不再作为源码根、contract 根或长期兼容根
- run-scoped analysis 产物只允许出现在 `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/...`
- `tests/` 不承载某个具体 subject 的源码、unit test、perf harness 或 native proof host

## Boundary Rules

- 目录间协作必须通过 `contracts/` 的显式边界进行
- `subjects/` 内的测试源码和 baseline 不得回流到 `tests/`
- `artifacts/` 只承载可再生输出，不能作为 contract 真源
- Windows native `cmake` scratch 目录即使落在系统临时目录，也不改变 canonical artifact 根
