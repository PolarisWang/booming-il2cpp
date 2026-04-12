# Wiki INDEX

> `wiki/` 用于沉淀长期有效知识。单次执行过程、阻塞、临时诊断与任务流水统一保留在 `docs/dev/<lifecycle>/<task_id>/`。

## 目录导航

| 分类 | 说明 | 入口 |
| --- | --- | --- |
| `01-项目总览` | 项目边界、目录结构与长期约束 | [`01-项目总览/INDEX.md`](./01-项目总览/INDEX.md) |
| `02-Skill体系` | 项目内 skill 的职责边界与注册表 | [`02-Skill体系/INDEX.md`](./02-Skill体系/INDEX.md) |
| `03-功能模块` | 核心功能模块与执行系统知识 | [`03-功能模块/INDEX.md`](./03-功能模块/INDEX.md) |
| `04-工具与集成` | 工具链入口、proof baseline 与集成规则 | [`04-工具与集成/INDEX.md`](./04-工具与集成/INDEX.md) |
| `05-项目经验` | 复用型经验与项目级约束 | [`05-项目经验/INDEX.md`](./05-项目经验/INDEX.md) |
| `06-测试验证` | 测试体系、验证入口与基线知识 | [`06-测试验证/INDEX.md`](./06-测试验证/INDEX.md) |

## 关键页面

| 页面 | 主题 | 摘要 |
| --- | --- | --- |
| [`01-项目总览/知识边界.md`](./01-项目总览/知识边界.md) | 知识边界 | 固化 `.codex`、`docs` 与 `wiki` 的职责边界。 |
| [`01-项目总览/仓库目录与产物边界.md`](./01-项目总览/仓库目录与产物边界.md) | 仓库目录与产物边界 | 固化 `build/`、`artifacts/`、`deploy/` 的目录职责。 |
| [`02-Skill体系/skill-registry.md`](./02-Skill体系/skill-registry.md) | Skill 注册表 | 汇总 `.codex/skills` 的长期职责边界。 |
| [`03-功能模块/01-执行系统/ACTIVE活动指针与任务目录机制.md`](./03-功能模块/01-执行系统/ACTIVE活动指针与任务目录机制.md) | ACTIVE 活动指针机制 | 固化 `ACTIVE.md` 与任务目录的职责分工。 |
| [`04-工具与集成/hot-update-e2e-runtime-baseline-v1.md`](./04-工具与集成/hot-update-e2e-runtime-baseline-v1.md) | hot update e2e runtime baseline | 固化 Phase C 的 package load、方法替换、auto bridge、回滚、benchmark CLI 与验证入口。 |
| [`04-工具与集成/interpreter-lowering-and-native-smoke-baseline-v1.md`](./04-工具与集成/interpreter-lowering-and-native-smoke-baseline-v1.md) | interpreter lowering、mixed execution 与 native smoke baseline | 固化 Phase 7 的真实 lowering、same-assembly `Call` / `CallVirt` 执行、真实 EH lowering、`try/catch` / `rethrow` / `try/finally` 最小 managed proof、字符串桥执行 proof 与 Windows 文件锁规避规则。 |
| [`04-工具与集成/metadata-supplement-bridge-baseline-v1.md`](./04-工具与集成/metadata-supplement-bridge-baseline-v1.md) | metadata supplement 与 bridge baseline | 固化 Phase 6 的 template/bridge baseline、proof 与项目级验证入口。 |
| [`04-工具与集成/hot-update-skeleton-v1.md`](./04-工具与集成/hot-update-skeleton-v1.md) | hot update skeleton | 固化 Phase 5 热更新骨架的项目边界、package 形状、runtime mode 与 proof/verification 入口。 |
| [`04-工具与集成/engine-host-proof-baseline-v1.md`](./04-工具与集成/engine-host-proof-baseline-v1.md) | engine host proof baseline | 固化 `EngineHostProof` host proof 与 `HostEmbeddingLite` ownership 基线。 |
| [`04-工具与集成/mobile-host-subject-routing-v1.md`](./04-工具与集成/mobile-host-subject-routing-v1.md) | mobile host subject routing | 说明 mobile subject route 的长期规则。 |

## 维护原则

- `wiki/` 只记录长期有效知识，不复制执行日志。
- 新增页面必须同步更新对应目录的 `INDEX.md`；影响导航时再更新本页。
- `docs/dev` 记录过程，`wiki/` 记录结论。

## 最近变更

- `2026-04-12`: 更新 `04-工具与集成/INDEX.md`，新增 `hot-update-interpreter-vm-v1.md` 与 `hot-update-e2e-runtime-baseline-v1.md`，沉淀 Phase C 的原生 Interpreter VM 与热更端到端基线。
- `2026-04-11`: 更新 `04-工具与集成/INDEX.md`，补充 Phase 7 的 same-assembly `CallVirt` 真实执行、`ManagedInstructionModel.IlOffset` 与真实 EH lowering，以及 `try/catch` / `rethrow` / `try/finally` 最小 managed proof。
- `2026-04-11`: 更新 `04-工具与集成/INDEX.md`，新增 `metadata-supplement-bridge-baseline-v1.md`，沉淀 Phase 6 metadata supplement 与 bridge baseline。
- `2026-04-11`: 更新 `04-工具与集成/INDEX.md`，新增 `hot-update-skeleton-v1.md`，沉淀 Phase 5 热更新骨架知识。
- `2026-04-11`: 更新 `04-工具与集成/INDEX.md`，新增 `engine-host-proof-baseline-v1.md` 与 `mobile-host-subject-routing-v1.md`。
