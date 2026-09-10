# Test And Benchmark Inventory Redesign 实现计划 v1.01

> **面向执行 Agent：** 必须使用 `dev:executing-plans`、`dev:project-test-governance`、`dev:test-driven-development` 执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 在不改变现有 `docs/benchmark/dashboard.html` 辅助报告定位的前提下，建立 `Capability Inventory`、`UnitTest Inventory`、`Benchmark Inventory` 三张统一 authority 清单，并输出 `JSON / CSV / HTML` 三类产物。

**架构：** 先抽 shared inventory source，避免在 `registry.py`、`perf_dashboard.py`、`benchmark_dashboard_generator.py` 之外再复制第四份 declared/capability/evidence 投影逻辑；再派生三张 authority JSON；最后由 authority JSON 派生 CSV 与宽表 HTML。HTML 只负责展示和中文 hint 卡片，不承载 authority。

**技术栈：** Python testing tooling、现有 declared catalog / registry / benchmark docs authority、静态 HTML template、`pytest`

**架构审核模式：** `critical`

**结构告警重点：**
- 重复代码：`declared benchmark`、`capability contract`、`sourceEntry/workloadEntry`、benchmark latest evidence 目前已分散在 `registry.py`、`perf_dashboard.py`、`benchmark_dashboard_generator.py`；本轮不得再复制一份平行投影。
- 结构与抽象：authority 汇总、表行派生、CSV flatten、HTML render 必须拆层，不允许把状态判定和页面拼装混在一个大文件。
- 文件职责与拆分：单文件不要同时承载 authority 拼装、中文 hint 文案、CSV 序列化、CLI dispatch。
- `codegen / Scriban` 分层：`n/a`。若最终采用模板，仅允许模板承载 HTML 壳和静态列位，不允许在模板内写阶段状态或支持性判定语义。

**权责图审核主题：**
- `declarations.py` / `compiled_catalog.py` 只拥有 declared entry 事实。
- `capability_coverage.py` / `subject.features.json` 只拥有 owner、support、proof/benchmark obligation。
- `registry.py` 只拥有 projection、goal/matrix、source/workload、hotupdate capability。
- `docs/benchmark/overview.json` 与 `docs/benchmark/subjects/*.json` 只拥有 benchmark latest evidence。
- 新 inventory 生成器只做聚合与派生，不得回写上游 authority 语义。
- `CSV` / `HTML` 只做导出与阅读，不得反向成为 authority。

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: `test-inventory-and-benchmark-inventory`
- capabilityItem: `inventory-authority-redesign`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `false`
- benchmarkRequired: `false`
- hotupdateImpact: `None`
- formalVerificationObjects: `[]`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`

**设计文档：** `docs/dev/completed/20260422-01-test-and-benchmark-inventory-redesign/design-v1-01.md`

**问题清零来源：** `docs/dev/completed/20260422-01-test-and-benchmark-inventory-redesign/STATUS.md`、`design-v1-01.md`、direct user confirmation

**计划来源：** `direct-plan`

**预期知识沉淀：** `n/a`

**收尾约束：** 执行完成后必须进入“结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交”固定链路；未完成 authority、导出、验证三层闭环前不得宣称完成。

---

## 涉及文件与职责

- `build/toolchains/run/testing/inventory_source.py`
  - 新增 shared source 层，统一 declared entry、capability contract、registry projection、benchmark evidence 的读取与标准化。
- `build/toolchains/run/testing/inventory_generator.py`
  - 新增 authority 派生层，负责三张 inventory 的 JSON 行模型、header hint schema、CSV flatten、HTML payload。
- `build/toolchains/run/testing/templates/testing-inventory.html`
  - 新增 inventory HTML 宽表模板，只负责表壳、分区与 hint 卡片容器。
- `build/toolchains/run/commands/test.py`
  - 新增统一生成入口，建议落为 `run test inventory`。
- `build/toolchains/run/manifests/run/commands.test-and-verify.json`
  - 补命令元数据与帮助说明。
- `build/toolchains/run/run_manifest.json`
  - 同步运行清单中的命令注册。
- `build/toolchains/run/testing/benchmark_dashboard_generator.py`
  - 只保留 benchmark dashboard 专属 summary/report 逻辑，能复用 shared inventory source 的部分应回收复用，避免重复投影。
- `build/toolchains/run/testing/perf_dashboard.py`
  - 与 inventory source 对齐 declared benchmark / capability contract 的来源，不再平行维护同类解析逻辑。
- `tests/unit/performance/test_testing_inventory_source.py`
  - 新增 shared source authority tests。
- `tests/unit/performance/test_testing_inventory_generator.py`
  - 新增三张清单行粒度、状态派生、hint schema、CSV/HTML payload tests。
- `tests/tooling/run/test_subject_command_inventory.py`
  - 新增 CLI 入口与输出路径 smoke tests。
- `docs/dev/completed/20260422-01-test-and-benchmark-inventory-redesign/STATUS.md`
  - 持续维护 planning / executing / completion 状态。

## 执行步骤

- [ ] Task 1: 冻结 shared inventory authority 和内部行模型
  - 新建 shared source 层，统一 declared entry、capability contract、registry projection、benchmark latest evidence 的读取入口。
  - 明确 `UnitTest Inventory` 内部 authority 模型固定为：
    - `StageRequirement`
    - `StageCoverage`
    - 派生 `StageStatus`
    - 派生 `StatusReason`
  - 明确 `CSV / HTML` 只消费压缩后的 `StageStatus` / `StatusReason`，但 authority JSON 保留 requirement / coverage 原值，避免语义损失。
  - 在此阶段完成一次权责图审核，确认没有把上游 authority 语义搬进 renderer 或 command 层。

- [ ] Task 2: 落三张 authority JSON
  - 生成统一 `inventory-source.json` 中间产物，供后续三张 authority JSON 派生。
  - 输出：
    - `docs/testing-inventory/capability-inventory.json`
    - `docs/testing-inventory/unit-test-inventory.json`
    - `docs/testing-inventory/benchmark-inventory.json`
  - 固定三张表的行粒度：
    - `Capability Inventory = capability item`
    - `UnitTest Inventory = declared-unit-test x stage`
    - `Benchmark Inventory = declared-benchmark x device`
  - 把中文 header hint schema 作为 authority 数据的一部分输出，而不是 HTML 内部硬编码。

- [ ] Task 3: 落 CSV 导出与 HTML 宽表
  - 从 authority JSON 派生：
    - `docs/testing-inventory/capability-inventory.csv`
    - `docs/testing-inventory/unit-test-inventory.csv`
    - `docs/testing-inventory/benchmark-inventory.csv`
    - `docs/testing-inventory/inventory.html`
  - HTML 主视图固定为宽表，不引入 card/tab/chart 作为主入口。
  - 每个表头必须支持详细中文 hover hint，至少覆盖：
    - 字段含义
    - 为什么有这列
    - 数据来源
    - 取值说明
    - 判定规则
    - 空值代表什么
  - `Benchmark Inventory` 保持 `declared-benchmark x device`，不退回 `x platform` 聚合。

- [ ] Task 4: 接入统一命令入口与现有 benchmark auxiliary report 边界
  - 在 `run test` 命令下新增 inventory 生成入口，默认输出到 `docs/testing-inventory/`。
  - 保持 `run benchmark --dashboard` 继续只负责 benchmark auxiliary report，不接管完整 inventory authority。
  - 如果 `benchmark_dashboard_generator.py` 或 `perf_dashboard.py` 需要复用 shared source，只允许做去重，不允许引入反向耦合。
  - 命令帮助、manifest、输出路径和默认行为要能直接恢复任务，不依赖人工解释。

- [ ] Task 5: 测试与收口
  - 先补 RED tests：
    - shared source authority
    - `UnitTest Inventory` 阶段派生
    - `Benchmark Inventory` 的 `declared-benchmark x device`
    - 中文 hint schema 完整性
    - CLI 输出路径与文件集
  - 再做 GREEN implementation，并确保 legacy benchmark dashboard tests 不回退。
  - 完成后先做结构告警与架构审视：
    - 是否又出现新的平行 helper / mapper
    - 是否出现单个大文件混装 authority + render + command
    - 是否把 hint 文案、状态规则或数据聚合塞进模板
  - 通过验证后再归档 completed，不需要额外人工复核阶段。

## 验收标准

- 存在三张 authority JSON、三张 CSV、一个 HTML，且全部由统一 source 派生。
- `UnitTest Inventory` 每行固定为 `declared-unit-test x stage`，并能同时表达 requirement、coverage 与压缩后的最终状态。
- `Benchmark Inventory` 每行固定为 `declared-benchmark x device`，并展示 `managed/native/interpreter` 三列模式状态与关键指标。
- HTML 表头 hover hint 全部为中文详细解释，不依赖额外说明文档才能读懂。
- 现有 `docs/benchmark/dashboard.html` 仍保留为 auxiliary report，不被误用为 inventory authority。
- 本轮不新增第四套 declared/capability/evidence 平行投影逻辑。

## 预期验证

- `python -m pytest tests/unit/performance/test_testing_inventory_source.py tests/unit/performance/test_testing_inventory_generator.py -q`
- `python -m pytest tests/unit/performance/test_benchmark_dashboard_generator_contracts.py tests/unit/performance/test_perf_dashboard.py tests/unit/registry/test_declared_registry_matrix_selection.py -q`
- `python -m pytest tests/tooling/run/test_subject_command_inventory.py tests/tooling/run/test_command_manifest_structure.py -q`
- `python build/toolchains/run/run.py test inventory`
- `git diff --check -- build/toolchains/run tests docs/testing-inventory docs/dev/completed/20260422-01-test-and-benchmark-inventory-redesign`

## 风险控制

- 如果 shared source 抽取不彻底，inventory generator 会把 `benchmark_dashboard_generator.py`、`perf_dashboard.py`、`registry.py` 中的重复逻辑再复制一遍，后续维护成本会继续上升。
- 如果把 `StageRequirement` / `StageCoverage` 压成单字段 authority，后续无法区分 obligation 缺口和 evidence 缺口。
- 如果把中文 hint 文案直接硬写在 HTML 模板内，后续 CSV/JSON 无法共享字段说明，也会导致 authority 与 render 分裂。
- 如果命令入口挂在 benchmark 专属命令下，会继续强化“inventory = benchmark dashboard”的错误心智，应保持 test/inventory 独立入口。
