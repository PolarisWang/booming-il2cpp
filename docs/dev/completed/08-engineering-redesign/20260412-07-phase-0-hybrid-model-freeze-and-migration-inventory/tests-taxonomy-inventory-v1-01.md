# Tests Taxonomy Inventory v1.01

**日期：** 2026-04-12 17:55 +08:00

## 1. 目标

盘点当前 `tests/` 目录结构，识别：

- 哪些 bucket 是 generic mechanism
- 哪些 bucket 是历史命名噪音
- 哪些 bucket 实际承载了 subject-specific 内容
- 后续应归并到什么目标 taxonomy

## 2. 当前顶层目录

当前 `tests/` 顶层目录：

- `contracts`
- `gate`
- `integration`
- `perf`
- `platform`
- `proof`
- `registry`
- `tooling`
- `unit`

## 3. 结构观察

### 3.1 `tests/unit/run`

问题最集中。

现状：

- 大量文件仍使用 `test_phase*` / `test_stage*` 命名
- 文件命名混合了：
  - 演进阶段
  - 功能责任
  - 具体 subject

结论：

- `tests/unit/run` 需要在 Phase 1 被拆到功能目录
- `phase*` / `stage*` 不应继续作为长期 taxonomy

### 3.2 `tests/tooling/run`

现状：

- 主要覆盖 CLI、bootstrap、wrapper、TUI、命令契约
- 功能边界相对清晰

结论：

- 可作为未来 `tests/tooling/{bootstrap,wrappers,tui,commands}` 的直接来源

### 3.3 `tests/integration/run`

现状：

- 仅有 `fixtures/`
- 说明当前 integration fixture 与 integration test 本身尚未完全分层

结论：

- 未来应拆成：
  - `tests/integration/...`
  - `tests/fixtures/integration/...`

### 3.4 `tests/registry`

现状：

- 包含 `modules/`
- 包含 `pipelines/`
- 包含 `system/`

结论：

- 这里混合了 registry 机制测试与系统样本数据
- 长期应拆分为：
  - `tests/unit/registry`
  - `tests/integration/registry`
  - `tests/integration/pipelines`
  - `tests/fixtures/registry`

### 3.5 `tests/platform` 与 `tests/gate`

现状：

- 两者都有：
  - `android-smoke`
  - `ios-smoke`
  - `linux-packaging`

结论：

- 这是重复 taxonomy
- 后续应合并为 `tests/integration/platform-hosts/` 与 `tests/integration/pipelines/`

### 3.6 `tests/perf`

现状：

- 当前只剩 `convert-golden-multi-project`

结论：

- 这是 subject-specific engineering workload，不应继续留在 `tests/perf`
- 长期归宿应回到对应 subject

### 3.7 `tests/proof`

现状：

- 仅剩 `README.md` 与 `Directory.Build.props`

结论：

- 这是历史残留 compatibility bucket
- 后续应删除或归档

## 4. 目标 taxonomy 映射

| 当前 bucket | 观察到的职责 | 主要问题 | 目标归宿 |
| --- | --- | --- | --- |
| `tests/contracts` | 合同样本、schema、managed/native sample | 部分 sample 与 fixture 混放 | `tests/contracts/` + `tests/fixtures/contracts/` |
| `tests/gate` | 平台 smoke / packaging gate | 与 `tests/platform` 重复 | `tests/integration/pipelines/` |
| `tests/integration` | integration fixture | fixture 与 integration test 未分层 | `tests/integration/` + `tests/fixtures/integration/` |
| `tests/perf` | subject-specific perf item | 不应承载 canonical subject workload | generic perf mechanism only |
| `tests/platform` | 平台 smoke / packaging | 与 `tests/gate` 重复 | `tests/integration/platform-hosts/` |
| `tests/proof` | 历史 proof bucket | 已失去长期职责 | 删除或归档 |
| `tests/registry` | modules/pipelines/system | 机制、系统样本、fixture 混在一起 | `tests/unit/registry` / `tests/integration/registry` / `tests/fixtures/registry` |
| `tests/tooling` | CLI/TUI/bootstrap/wrapper | 仍集中在 `run/` 下 | `tests/tooling/{bootstrap,wrappers,tui,commands}` |
| `tests/unit` | 单测主入口 | `phase*` / `stage*` 命名噪音最重 | `tests/unit/{registry,selection,planning,execution,reporting,performance,compatibility}` |

## 5. Phase 0 结论

Phase 0 对 `tests/` 的冻结结论：

- `tests/` 的主要问题不是数量，而是分类轴错误
- `phase*` / `stage*` 需要被功能分类替代
- `tests/perf` 中的 `convert-golden-multi-project` 必须迁回 subject
- `tests/platform` 与 `tests/gate` 必须去重
- `tests/proof` 不再是长期 bucket
