# 04-工具与集成 INDEX

## 范围说明

- 记录项目级工具入口、集成边界、proof baseline 与长期稳定的操作约束。
- 不记录单次任务流水、临时诊断或仅对某次执行有效的上下文。
- 执行日志保留在 `docs/dev/<lifecycle>/<task_id>/`；沉淀后的长期知识再进入本目录。

## 子分类

| 分类 | 说明 | 索引 |
| --- | --- | --- |
| `-` | 当前目录直接维护工具与集成主题页 | `-` |

## 关键页面

| 页面 | 主题 | 摘要 |
| --- | --- | --- |
| [`interpreter-lowering-and-native-smoke-baseline-v1.md`](./interpreter-lowering-and-native-smoke-baseline-v1.md) | interpreter lowering、mixed execution 与 native smoke baseline | 固化 Phase 7 的真实 lowering proof、same-assembly `Call` / `CallVirt` 执行、真实 EH lowering、`try/catch` / `rethrow` / `try/finally` 最小 managed proof、字符串桥执行 proof、native `InterpreterVM` smoke proof 与 Windows 文件锁规避规则。 |
| [`metadata-supplement-bridge-baseline-v1.md`](./metadata-supplement-bridge-baseline-v1.md) | metadata supplement 与 bridge baseline | 固化 Phase 6 的 supplemental metadata template、bridge baseline、proof 与项目级验证入口。 |
| [`hot-update-skeleton-v1.md`](./hot-update-skeleton-v1.md) | hot update skeleton | 固化 `Chaos.IL2CPP.HotUpdate` 的项目边界、package 形状、runtime mode 语义与 `HotUpdateSkeletonProof`。 |
| [`engine-host-proof-baseline-v1.md`](./engine-host-proof-baseline-v1.md) | engine host proof baseline | 固化 `EngineHostProof` 的 host proof build、lifecycle stress 与 `HostEmbeddingLite` ownership 基线。 |
| [`mobile-host-subject-routing-v1.md`](./mobile-host-subject-routing-v1.md) | mobile host subject routing | 说明 `MobileHelloWorldProof` 的 subject-scoped Android/iOS host root、root CMake cache 变量与 mobile route fallback 规则。 |
| [`native-perf-and-convert-baselines-v1.md`](./native-perf-and-convert-baselines-v1.md) | native perf 与 convert baseline | 固化 `native-runtime-perf` payload、`tests/perf/*` 约束与 convert perf baseline。 |
| [`engine-binding-stub-baseline-v1.md`](./engine-binding-stub-baseline-v1.md) | engine binding stub baseline | 固化 `Chaos.IL2CPP.EngineBinding`、`[EngineExport]` / `[EngineCallback]` 标注入口与 smoke / compile-only baseline。 |
| [`project-graph-ingestion-v1.md`](./project-graph-ingestion-v1.md) | project graph ingestion | 固化 `Chaos.IL2CPP.ProjectGraph`、`project-graph.json` 与 golden convert 接线。 |
| [`共享-contract-v0.md`](./共享-contract-v0.md) | 共享 contract v0 | 固化 `contracts/shared/v0/` 的长期约束、验证入口与使用边界。 |
| [`统一入口脚本.md`](./统一入口脚本.md) | 统一入口脚本 `run.*` | 固化 bootstrap、TUI、prepare/clean、`--json` 与 harness 接线规则。 |
| [`统一测试框架.md`](./统一测试框架.md) | 统一测试框架 `run test` | 固化 suite manifest、summary/final-summary 与测试入口约束。 |
| [`unified-project-workspaces-and-deploy-core.md`](./unified-project-workspaces-and-deploy-core.md) | project workspaces 与 deploy core | 固化 `generate/build project`、`deploy core`、`subject-exec` 与 `artifacts/projects/**` / `deploy/core/**` 边界。 |
| [`Claude技能测试入口.md`](./Claude技能测试入口.md) | Claude 技能测试入口 | 固化 `.codex/tests/claude-code/` 的入口与约束。 |

## 维护约定

- 页面必须总结可复用结论，而不是复制任务过程。
- 新增页面时必须同步更新本目录 `INDEX.md`；如影响上层导航，再更新 `wiki/INDEX.md`。

## 最近变更

- `2026-04-11`: 更新 `interpreter-lowering-and-native-smoke-baseline-v1.md`，补充 same-assembly `CallVirt` 真实执行、`ManagedInstructionModel.IlOffset` 与真实 EH lowering，以及 `try/catch` / `rethrow` / `try/finally` 最小 managed proof。
- `2026-04-11`: 新增 `metadata-supplement-bridge-baseline-v1.md`，沉淀 Phase 6 supplemental metadata 与 bridge baseline 的长期规则。
- `2026-04-11`: 新增 `hot-update-skeleton-v1.md`，沉淀 Phase 5 热更新骨架的项目边界、package 形状、runtime mode 与 proof/verification 入口。
- `2026-04-11`: 新增 `engine-host-proof-baseline-v1.md`，沉淀 `EngineHostProof` host proof 与 `HostEmbeddingLite` ownership 基线。
- `2026-04-11`: 新增 `mobile-host-subject-routing-v1.md`，沉淀 mobile subject route 的长期规则。
- `2026-04-11`: 新增 `engine-binding-stub-baseline-v1.md`，沉淀 `Chaos.IL2CPP.EngineBinding` 的 smoke / compile-only baseline。
- `2026-04-11`: 新增 `native-perf-and-convert-baselines-v1.md`，沉淀 native perf 与 convert perf 基线。
- `2026-04-11`: 新增 `project-graph-ingestion-v1.md`，沉淀 `Chaos.IL2CPP.ProjectGraph` 与 `project-graph.json` 接线。
- `2026-04-11`: 新增 `共享-contract-v0.md`，沉淀 `contracts/shared/v0/` 的长期约束。
