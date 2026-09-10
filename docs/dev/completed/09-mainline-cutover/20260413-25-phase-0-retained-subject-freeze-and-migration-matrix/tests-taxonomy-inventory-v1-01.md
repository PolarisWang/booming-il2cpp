# Tests Taxonomy Inventory v1.01

Date: 2026-04-13 08:57:38 +08:00

## 1. 当前顶层目录

当前 `tests/` 顶层目录为：

- `contracts`
- `fixtures`
- `gate`
- `integration`
- `perf`
- `tooling`
- `unit`

另有噪音目录：

- `__pycache__`

## 2. 当前目录职责与目标归宿

### 2.1 `contracts`

当前内容：

- analysis contract sample / snapshot
- managed contract smoke sample

结论：

- 继续保留
- 归属 `generic mechanism`

长期职责：

- schema sample
- contract snapshot
- 供 tooling / discovery / reporting 测试消费的通用合同样本

### 2.2 `fixtures`

当前内容：

- declared metadata fixture
- framework/no-entry fixture

结论：

- 继续保留
- 归属 `generic fixture`

长期职责：

- 仅承载通用 fixture
- 不承载 canonical subject source

### 2.3 `integration`

当前内容：

- `platform-hosts/*`
- `registry/*`
- `run/*`

结论：

- 继续保留
- 但 `run/` 目录后续需要进一步去 legacy 命名

长期职责：

- CLI integration
- registry integration
- pipeline integration
- platform host integration

### 2.4 `tooling`

当前内容：

- `run/*` 相关命令与工具测试

结论：

- 继续保留
- 归属 tooling 验证层

长期职责：

- `run` 命令族
- bootstrap
- manifest / json / tui / subject command

### 2.5 `unit`

当前内容：

- 当前以 `compatibility/` 为主
- 里面已经包含大量 subject / pack / cutover 回归测试

结论：

- 继续保留
- 但 `compatibility/` 内的测试名后续要跟 retained subject 新结构对齐

长期职责：

- registry
- selection
- planning
- execution
- reporting
- compatibility

### 2.6 `perf`

当前内容：

- `convert-golden-multi-project`

结论：

- 不应长期保留为 subject-specific canonical workload 容器
- 这是最明确需要迁出的目录

目标归宿：

- `SolutionCorePack` 的 engineering workload fixture / baseline

### 2.7 `gate`

当前内容：

- `android-smoke`
- `ios-smoke`
- `linux-packaging`

结论：

- 不应继续作为长期独立 taxonomy
- 更适合作为 `integration/platform-hosts` 或 retained subject engineering gate 的兼容 wrapper

## 3. 当前 tests 体系与方案 A 的关系

方案 A 本身主要改的是 `subjects/`，不是 `tests/` 主结构。

但 `tests/` 仍有两个必须在 Phase 0 冻结的结论：

- `tests/perf/convert-golden-multi-project` 必须迁离 `tests/perf`
- `tests/gate/*` 不再作为独立长期 taxonomy，只能作为兼容过渡或并回 integration/platform-hosts

## 4. Phase 0 结论

### 保留为长期目录

- `contracts`
- `fixtures`
- `integration`
- `tooling`
- `unit`

### 进入迁移/收敛路径

- `perf`
- `gate`

### 噪音目录

- `__pycache__` 继续视为清理对象，不进入正式 taxonomy
