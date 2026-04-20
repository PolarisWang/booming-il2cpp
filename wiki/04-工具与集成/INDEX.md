# 04-工具与集成 INDEX

## 范围说明

- 记录项目级工具入口、集成边界、proof baseline 与长期稳定的操作约束。
- 不记录单次任务流水、临时诊断或只对某次执行有效的上下文。
- 执行日志保留在 `docs/dev/<lifecycle>/<task_id>/`；沉淀后的长期知识再进入本目录。

最近更新：

- `2026-04-17`：更新 `il2cpp-core-structure-and-scriban-governance.md`，收口 `ObjectModel` / `ExternalRuntimeHelpers`，并同步 `LoaderStage` / `LinkerStage` / `ManagedClosureContracts` 的结构治理结论。
- `2026-04-19`：新增 `scriban-usage-and-codegen-rules.md`，明确 IL2CPP / Python codegen 默认优先 `Scriban`，能力不足时先扩展 `Scriban`。

## 子分类

| 分类 | 说明 | 索引 |
| --- | --- | --- |
| `-` | 当前目录直接维护工具与集成主题页 | `-` |

## 关键页面

| 页面 | 主题 | 摘要 |
| --- | --- | --- |
| [`hot-update-interpreter-vm-v1.md`](./hot-update-interpreter-vm-v1.md) | hot update interpreter vm | 固化原生 `InterpreterVM` 的 `InterpreterValue`、opcode catalog、native smoke 边界，以及与 managed interpreter 的职责分层。 |
| [`hot-update-e2e-runtime-baseline-v1.md`](./hot-update-e2e-runtime-baseline-v1.md) | hot update e2e runtime baseline | 固化 package load、方法替换、auto bridge、回滚、benchmark CLI 与验证入口。 |
| [`interpreter-lowering-and-native-smoke-baseline-v1.md`](./interpreter-lowering-and-native-smoke-baseline-v1.md) | interpreter lowering and native smoke baseline | 固化 lowering proof、EH lowering、字符串桥执行 proof 与 native `InterpreterVM` smoke proof。 |
| [`metadata-supplement-bridge-baseline-v1.md`](./metadata-supplement-bridge-baseline-v1.md) | metadata supplement bridge baseline | 固化 supplemental metadata template、bridge baseline、proof 与项目级验证入口。 |
| [`hot-update-skeleton-v1.md`](./hot-update-skeleton-v1.md) | hot update skeleton | 固化 `Chaos.IL2CPP.HotUpdate` 的项目边界、package 形状、runtime mode 与 `HotUpdateSkeletonProof`。 |
| [`engine-host-proof-baseline-v1.md`](./engine-host-proof-baseline-v1.md) | engine host proof baseline | 固化 `EngineHostProof` 的 host proof build、lifecycle stress 与 `HostEmbeddingLite` ownership 基线。 |
| [`mobile-host-subject-routing-v1.md`](./mobile-host-subject-routing-v1.md) | mobile host subject routing | 说明 `MobileHelloWorldProof` 的 subject-scoped Android / iOS host root 与 route fallback 规则。 |
| [`native-perf-and-convert-baselines-v1.md`](./native-perf-and-convert-baselines-v1.md) | native perf and convert baseline | 固化 `native-runtime-perf` payload、`tests/perf/*` 约束与 convert perf baseline。 |
| [`engine-binding-stub-baseline-v1.md`](./engine-binding-stub-baseline-v1.md) | engine binding stub baseline | 固化 `Chaos.IL2CPP.EngineBinding`、`[EngineExport]` / `[EngineCallback]` 标注入口与 smoke / compile-only baseline。 |
| [`il2cpp-core-structure-and-scriban-governance.md`](./il2cpp-core-structure-and-scriban-governance.md) | IL2CPP core structure and Scriban governance | 固化 `IL2CPP` 核心代码层的 planner / emitter / template 结构治理，以及 ObjectModel / ExternalRuntimeHelpers / Loader / Linker / Contracts 的长期拆分边界。 |
| [`scriban-usage-and-codegen-rules.md`](./scriban-usage-and-codegen-rules.md) | Scriban usage and codegen rules | 说明 `Scriban` 在 IL2CPP 与 Python codegen 中的正式使用方式、扩展顺序与禁止事项。 |
| [`project-graph-ingestion-v1.md`](./project-graph-ingestion-v1.md) | project graph ingestion | 固化 `Chaos.IL2CPP.ProjectGraph`、`project-graph.json` 与 golden convert 接线。 |
| [`共享-contract-v0.md`](./共享-contract-v0.md) | 共享 contract v0 | 固化 `contracts/shared/v0/` 的长期约束、验证入口与使用边界。 |
| [`统一入口脚本.md`](./统一入口脚本.md) | 统一入口脚本 `run.*` | 固化 bootstrap、TUI、prepare / clean、`--json` 与 harness 接线规则。 |
| [`统一测试框架.md`](./统一测试框架.md) | 统一测试框架 `run test` | 固化 suite manifest、summary / final-summary 与测试入口约束。 |
| [`unified-project-workspaces-and-deploy-core.md`](./unified-project-workspaces-and-deploy-core.md) | project workspaces and deploy core | 固化 `generate/build project`、`deploy core`、`subject-exec` 与 `artifacts/projects/**` / `deploy/core/**` 边界。 |
| [`Claude技能测试入口.md`](./Claude技能测试入口.md) | Claude 技能测试入口 | 固化 `.codex/tests/claude-code/` 的入口与约束。 |

## 维护约定

- 页面必须总结可复用结论，而不是复制任务过程。
- 新增页面时必须同步更新本目录 `INDEX.md`；如影响上层导航，再更新 `wiki/INDEX.md`。
