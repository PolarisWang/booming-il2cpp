# 测试体系 Suite 化重设计 Roadmap v1.01

Date: 2026-04-12
Status: active-roadmap

## 1. 目标

把已经冻结的 suite/scenario/workload 设计，落成一条可执行、可拆分、可逐阶段验收的实施路线，重点解决两条主线：

- `tests/` 从历史 `phase*` 与 subject-specific 资产里脱身，回到 generic mechanism 测试层。
- `subjects/` 从“一个测试点一个顶层 subject”的碎片化状态，收敛为少量 suite subject + 内部 `scenario/workload` 的稳定终态。

本 roadmap 不是直接进入单份稳定 `plan` 的原因是：

- 这次改动同时触及 `tests/`、`subjects/`、manifest、selector、registry、artifact、CLI/TUI、pipeline/gate。
- 如果不先冻结阶段边界，任何一个局部 plan 都会在“测试分类、subject 迁移、命令兼容”三者之间反复返工。
- 当前 `tests/` 和 `subjects/` 里还存在历史资产与重复入口，必须先把结构调整顺序定死，再进入具体实现。

## 2. 范围边界

本 roadmap 负责：

- `tests/` 的分类重组、`phase*` 命名清理与 generic mechanism 边界收口
- `subjects/` 的 suite 化收敛
- `ProofSuite/scenario` 与 `BenchmarkSuite/workload` 的 canonical 目录与 manifest 模型
- `availability / capabilities / pipeline` 进入正式测试对象模型
- `registry / selector / artifact / CLI / TUI` 对 suite 模型的切换顺序
- 热更、多平台、device evidence、rollback、release gate 等能力如何纳入测试体系

本 roadmap 不负责：

- 直接实施代码改动
- 直接产出单个阶段的实现 patch
- 立即删除所有旧路径与旧 subject
- 立即定义全部真机自动化细节

## 3. 非目标

当前不在本 roadmap 中一次性解决：

- 所有历史 subject 的逐文件迁移细节
- 所有平台的完整自动化设备农场能力
- 全量 release packaging 流程脚本
- 最终公共 TUI 的视觉交互细节

## 4. 阶段列表

### Phase 0：Taxonomy Freeze And Migration Map

- 冻结 `tests/` 与 `subjects/` 的目标结构、ownership 与迁移映射
- 明确每个旧 `test_phase*`、旧 subject、旧 gate 的去向
- 明确哪些旧入口保留兼容，哪些只是历史资产

### Phase 1：Generic Test Taxonomy And De-Phase Cleanup

- 先把 `tests/` 结构理顺
- 去掉 `phase*` 命名
- 抽离 subject-specific 测试资产
- 收口重复的 integration/tooling 测试职责

### Phase 2：Suite Subject Root And Manifest Cutover

- 建立 `ProofSuite` 与 `BenchmarkSuite` 的正式根目录
- 引入 `scenario.manifest.json` 与 `workload.manifest.json`
- 冻结 `availability / capabilities / validationProfiles` 的最小 schema

### Phase 3：ProofSuite Scenario Convergence

- 把 proof-like subject 收敛进 `ProofSuite/scenarios/*`
- 把 correctness / trace / rollback / mobile-host 场景纳入同一 subject
- 让旧 proof subject 退化为兼容壳而非 canonical 根

### Phase 4：BenchmarkSuite Workload Convergence

- 把 `Bench*` subject 收敛进 `BenchmarkSuite/workloads/*`
- 统一 workload contract、baseline、budget、harness 配置
- 把当前暂停的 AOT benchmark backend 子任务并入 suite 结构，而不是继续扩老的 `Bench*` 根

### Phase 5：Registry / Selector / Artifact / Public Cutover

- 让 registry、selector、artifact、CLI/TUI 以 suite/scenario/workload 为主轴
- 保留旧 subject id 的兼容别名
- 让 canonical 运行与结果归属切到 suite 模型

### Phase 6：Capability / Availability / Pipeline Rollout

- 给所有 scenario/workload 补齐 capability 与 availability
- 引入 `dev-smoke`、`merge-gate`、`nightly-cross-platform`、`release-gate`
- 把热更、多平台、device evidence、rollback、弱环境等能力放进正式 pipeline

### Phase 7：Compatibility Closure And Legacy Cleanup

- 收尾兼容层
- 退役旧的顶层 subject、`phase*` 命名与历史目录
- 让 suite 模型成为唯一 canonical 路径

## 5. 每阶段完成定义

### Phase 0

- `goal`:
  - 在任何物理迁移前，冻结测试分类、subject 收敛策略与结构调整边界。
- `structure_changes`:
  - 暂不做生产目录搬迁。
  - 在任务文档中新增：
    - `test-taxonomy-v1-01.md`
    - `subject-convergence-map-v1-01.md`
    - `legacy-compatibility-policy-v1-01.md`
- `checklist`:
  - 盘点 `tests/unit/run/` 中所有 `test_phase*` 文件，并标出目标分类。
  - 盘点 `tests/integration/run/` 与 `tests/tooling/run/` 的重复文件，并指定 ownership。
  - 盘点 `subjects/` 下所有现有 subject，并分类为：
    - proof -> `scenario`
    - benchmark -> `workload`
    - support asset -> 挪出 `subjects/` 或并入 suite support
    - legacy-only -> 兼容壳
  - 明确 `tests/proof`、`tests/perf`、`tests/platform`、`tests/gate` 各自的长期角色。
  - 明确 `HelloWorld`、`HelloWorldObject`、`MainlineFeaturePack` 等特殊目录的归宿。
  - 冻结 `golden` 的长期定位：
    - `golden` 是核心工程级验证样例，不是普通 fixture
    - canonical 迁入 `ProofSuite/scenarios/solution-*`
    - 首批命名方向：
      - `solution-simple-lib`
      - `solution-multi-project`
      - `solution-package-reference`
  - 冻结旧 subject id 的兼容期限与 alias 策略。
- `exit_criteria`:
  - 所有旧测试文件、旧 subject、旧 gate 都有明确去向。
  - 用户确认目标 taxonomy 与 subject 收敛表。
  - 后续 phase 不再允许临时改变目录终态。
- `deliverables`:
  - 测试分类映射表
  - subject 收敛映射表
  - 兼容策略文档
- `dependencies`: 无
- `open_questions`:
  - 无。已确认 `golden` 保留在正式 subject 体系内，但 canonical 收敛到 `ProofSuite/scenarios/solution-*`，不再保留 `subjects/golden/*` 作为长期结构。

### Phase 1

- `goal`:
  - 让 `tests/` 先完成 generic taxonomy 收口，避免后续 subject 迁移继续把历史命名带回来。
- `structure_changes`:
  - `tests/unit/run/` 重组为：
    - `schema/`
    - `selection/`
    - `planning/`
    - `execution/`
    - `reporting/`
    - `compatibility/`
    - `capabilities/`
  - `tests/integration/run/` 重组为：
    - `cli/`
    - `registry/`
    - `pipelines/`
    - `platform-hosts/`
    - `release-gates/`
  - `tests/tooling/run/` 重组为：
    - `tui/`
    - `wrappers/`
    - `bootstrap/`
    - `commands/`
  - `tests/proof/` 退役为历史兼容目录或完全移空。
  - `tests/platform/` 与 `tests/gate/` 的 generic 部分并入 integration/tooling taxonomy，subject-specific 部分迁回 `subjects/`.
  - `tests/support.py` 若继续增长，拆成 `tests/support/` package。
- `checklist`:
  - 把所有 `test_phase*` 文件改名并迁移到按职责分类的新目录。
  - 清理 `test_stage*` 这类仍表达历史阶段的文件名。
  - 对 `tests/integration/run/` 与 `tests/tooling/run/` 的重复文件逐一决策：
    - 归 tooling
    - 归 integration
    - 合并为一份共享测试
  - 把 generic tests 中硬编码的具体 subject 名替换为 sample manifest / fixture 驱动。
  - 为新 suite 模型准备最小 fixture：
    - `ProofSuite` sample
    - `BenchmarkSuite` sample
    - `scenario` sample
    - `workload` sample
  - 清理 `tests/proof` 内的 subject-specific source、native host、package fixture 引用。
- `exit_criteria`:
  - `tests/unit/run/` 下不再存在 `test_phase*`.
  - run 相关测试文件名只表达对象/契约/行为，不再表达历史阶段。
  - integration/tooling 重复测试职责收口完成。
  - `tests/` 不再是 subject-specific source 的 canonical 根。
- `deliverables`:
  - 重组后的 `tests/` 目录
  - 新命名规则与迁移表
  - generic test fixture 集
- `dependencies`:
  - Phase 0
- `open_questions`:
  - `tests/perf/convert-golden-multi-project` 最终应归为 generic fixture，还是 benchmark suite 内的 workload 资产。

### Phase 2

- `goal`:
  - 建立 suite subject 根与 child manifest 模型，为后续 proof/benchmark 收敛提供正式容器。
- `structure_changes`:
  - 新增：
    - `subjects/ProofSuite/subject.manifest.json`
    - `subjects/ProofSuite/source/`
    - `subjects/ProofSuite/validation/`
    - `subjects/ProofSuite/scenarios/`
    - `subjects/BenchmarkSuite/subject.manifest.json`
    - `subjects/BenchmarkSuite/source/`
    - `subjects/BenchmarkSuite/validation/`
    - `subjects/BenchmarkSuite/workloads/`
  - 新增 child manifest 约定：
    - `scenario.manifest.json`
    - `workload.manifest.json`
  - 引入 manifest 字段：
    - `capabilities`
    - `availability`
    - `validationProfiles`
    - `sourceOverrides`
    - `metricPolicy`
    - `budgetPolicy`
- `checklist`:
  - 冻结 `ProofSuite` 与 `BenchmarkSuite` 的 subject root 结构。
  - 设计并落最小 schema loader，能够读取 suite + child manifests。
  - 明确 suite 与 child 的 source 继承/override 规则。
  - 明确旧 subject 与新 suite child 的 alias/redirect 机制。
  - 明确哪些旧 subject 只是过渡壳，禁止继续新增顶层 subject。
  - 为 `availability` 的 `ready/planned/blocked/unsupported` 定义统一语义。
- `exit_criteria`:
  - `ProofSuite` 与 `BenchmarkSuite` 的 canonical 根目录存在且可被 loader 消费。
  - `scenario/workload` 模型具备稳定 schema。
  - 从本阶段结束起，新增测试点默认不得再新增顶层 subject。
- `deliverables`:
  - suite roots
  - child manifest schema
  - alias/redirect 规则
- `dependencies`:
  - Phase 0
  - Phase 1 的 sample fixture 命名规则
- `open_questions`:
  - `HelloWorld` / `HelloWorldObject` 是否都应退化为 `ProofSuite` 的基础 scenario fixture，而不再作为长期顶层 subject 保留。

### Phase 3

- `goal`:
  - 把 proof-like subject 统一收敛到 `ProofSuite/scenarios/*`，把 correctness/trace/recovery/mobile-host 变成 scenario，而不是顶层目录。
- `structure_changes`:
  - 新增 canonical 路径：
    - `subjects/ProofSuite/scenarios/<scenario-id>/scenario.manifest.json`
    - `subjects/ProofSuite/scenarios/<scenario-id>/expected/`
    - `subjects/ProofSuite/scenarios/<scenario-id>/baselines/`
    - `subjects/ProofSuite/scenarios/<scenario-id>/assets/`
    - `subjects/ProofSuite/scenarios/<scenario-id>/package-fixtures/`
    - `subjects/ProofSuite/scenarios/<scenario-id>/source-overrides/`
  - 原 proof-like 根目录变成 compatibility wrapper，不再是 canonical source。
- `checklist`:
  - 把现有 proof-like subject 逐个映射到 scenario。
  - 合并共享 source 到 `ProofSuite/source/`。
  - 把 scenario 特有差异落到 `source-overrides/` 或 scenario manifest。
  - 把 expected、baseline、trace snapshot、package fixture 迁回对应 scenario。
  - 把 mobile / hot-update / rollback / compatibility proof 作为 scenario + availability 表达，而不是新 subject。
  - 更新 unit/integration fixtures，让它们引用新 scenario manifests。
  - 定义旧 proof subject id 到新 scenario id 的兼容跳转。
- `exit_criteria`:
  - 所有 proof-like subject 都有 canonical `scenario` 对应项。
  - `ProofSuite` 可以覆盖当前 proof 主线的关键测试点。
  - 旧 proof subject 目录不再接收新功能。
- `deliverables`:
  - `ProofSuite/scenarios/*`
  - proof subject -> scenario 对照表
  - proof compatibility aliases
- `dependencies`:
  - Phase 2
- `open_questions`:
  - `MainlineFeaturePack` 是否直接成为 `ProofSuite` 的主 source root，还是保留一段过渡期双根。

### Phase 4

- `goal`:
  - 把 `Bench*` subject 收敛到 `BenchmarkSuite/workloads/*`，并把 benchmark contract 从“每项一个 subject”切到 workload 模型。
- `structure_changes`:
  - 新增 canonical 路径：
    - `subjects/BenchmarkSuite/workloads/<workload-id>/workload.manifest.json`
    - `subjects/BenchmarkSuite/workloads/<workload-id>/baselines/`
    - `subjects/BenchmarkSuite/workloads/<workload-id>/budgets/`
    - `subjects/BenchmarkSuite/workloads/<workload-id>/package-fixtures/`
    - `subjects/BenchmarkSuite/workloads/<workload-id>/source-overrides/`
  - 统一 benchmark harness 到：
    - `subjects/BenchmarkSuite/validation/perf-harness/`
    - `subjects/BenchmarkSuite/validation/device-runners/`
  - 现有 `Bench*` 根目录变成 compatibility wrapper。
- `checklist`:
  - 把 `BenchArithmetic`、`BenchAllocation`、`BenchDispatch`、`BenchGeneric`、`BenchException`、`BenchGameLoop`、`BenchHotUpdate*`、`BenchMixed` 全部映射为 workload。
  - 把 `workloadEntry`、`harnessIterations`、warmup/measurement policy 迁入 workload manifest。
  - 把 perf baseline 与 budget 统一到 workload 目录。
  - 把 managed / interpreter / native / mobile benchmark 的 runtime policy 统一成 workload + matrix 组合。
  - 把已暂停的 `20260412-05-phase-b-generic-native-aot-benchmark-backend` 结果并入 `BenchmarkSuite`，不再继续扩旧 `Bench*` 根目录。
  - 保留历史 benchmark record lineage 与 dashboard 映射能力。
- `exit_criteria`:
  - 所有 `Bench*` 都有 canonical workload 对应项。
  - `BenchmarkSuite` 能承载 managed / interpreter / native 的统一 workload contract。
  - 新增 benchmark 项不再允许新增顶层 `Bench*` subject。
- `deliverables`:
  - `BenchmarkSuite/workloads/*`
  - benchmark subject -> workload 对照表
  - perf policy / budget policy 统一规则
- `dependencies`:
  - Phase 2
  - 建议在 Phase 3 收口 proof 主线后启动，避免同时迁两条主线
- `open_questions`:
  - 历史 dashboard 与 record 是否需要对旧 `Bench*` id 提供长期别名，还是只保留一次迁移期映射。

### Phase 5

- `goal`:
  - 让运行时对象发现、执行入口、artifact 归档与 public surface 切到 suite/scenario/workload 主轴。
- `structure_changes`:
  - registry 扩展为可发现：
    - suites
    - scenarios
    - workloads
    - pipelines
  - artifact canonical 路径切到：
    - `artifacts/subjects/ProofSuite/runs/<run-id>/scenarios/<scenario-id>/...`
    - `artifacts/subjects/BenchmarkSuite/runs/<run-id>/workloads/<workload-id>/...`
  - `tests/registry/` 按新对象模型补充或重组索引。
  - `tests/support.py` 的 subject 选择助手同步切到 suite model。
- `checklist`:
  - 实现 suite/scenario/workload registry scan。
  - 实现 canonical selector 语义与旧 subject id alias。
  - 更新 CLI/TUI/JSON/reporting 使其展示 suite model。
  - 更新 artifacts ledger 与 summary/report 归属字段。
  - 为旧 subject id 保留 compatibility adapter。
  - 更新 integration/tooling 测试覆盖新的发现与执行路径。
- `exit_criteria`:
  - 新模型已成为 canonical 发现与执行入口。
  - artifacts 能稳定回溯到 suite/scenario/workload。
  - 旧入口仍可兼容，但只作为适配层。
- `deliverables`:
  - suite model registry
  - selector/cutover 方案
  - artifact 路径切换结果
- `dependencies`:
  - Phase 3
  - Phase 4
- `open_questions`:
  - canonical CLI 是继续以 `subject + scenario/workload` 组合表达，还是引入新的顶层子命令。

### Phase 6

- `goal`:
  - 把能力覆盖模型真正落到 manifests、pipelines 和 gate，而不只停留在设计文档。
- `structure_changes`:
  - 为所有 scenario/workload 补齐：
    - `capabilities`
    - `availability`
    - `pipelines`
  - `tests/registry/pipelines/` 收敛或新增：
    - `dev-smoke`
    - `merge-gate`
    - `nightly-cross-platform`
    - `release-gate`
  - subject 内补齐能力所需资产：
    - `package-fixtures/`
    - `device-runners/`
    - `platform-hosts/`
    - `rollback-fixtures/`
    - `compatibility-matrices/`
- `checklist`:
  - 给每个 scenario/workload 标注 capability。
  - 给每个平台标注 availability 状态。
  - 把以下能力纳入正式 pipeline：
    - version compatibility
    - rollback / fallback
    - state migration
    - device evidence
    - weak environment simulation
    - reproducible build / symbolization
    - engine/native boundary
    - release budget / package integrity
  - 让 pipeline 能按 `ready` 状态过滤默认执行项。
  - 给 nightly 与 release gate 定义最小必过集。
- `exit_criteria`:
  - capability 与 availability 成为正式可查询字段。
  - dev / merge / nightly / release 四级 pipeline 都有稳定对象定义。
  - 多平台与热更额外能力不再散落在文档或临时脚本里。
- `deliverables`:
  - capability/availability 覆盖表
  - 四级 pipeline 定义
  - 多平台/热更扩展能力接入清单
- `dependencies`:
  - Phase 5
- `open_questions`:
  - 真机证据采集在首版 nightly 中是全自动，还是先允许半自动回传。

### Phase 7

- `goal`:
  - 关闭兼容层和历史目录，形成干净的 suite-based canonical 布局。
- `structure_changes`:
  - 退役或归档：
    - 旧 `Bench*` 顶层 subject
    - 旧 proof-like 顶层 subject
    - `tests/proof/`
    - 残留 `test_phase*` / `test_stage*` 名称
    - 不再需要的 compatibility wrapper
  - 清理结构性噪音目录规范：
    - `__pycache__/`
    - `bin/`
    - `obj/`
  - 更新长期文档入口与 wiki。
- `checklist`:
  - 删除或归档所有非 canonical subject 根。
  - 删除旧 selector alias 或把它们降为明确 deprecated。
  - 清理历史 phase 命名测试的最后残留。
  - 校验 `tests/` 与 `subjects/` 边界不再回混。
  - 更新 onboarding、wiki、docs/dev 里的正式路径说明。
  - 对 repo 噪音目录补齐 ignore/清理策略。
- `exit_criteria`:
  - suite model 是唯一 canonical 结构。
  - 旧 proof/benchmark 顶层 subject 全部退出主路径。
  - `tests/` 内不再保留 subject-specific canonical 资产。
  - 文档、命令、artifact 与 registry 都只指向新结构。
- `deliverables`:
  - legacy cleanup 清单
  - compatibility closure 结果
  - wiki / onboarding 更新
- `dependencies`:
  - Phase 6
- `open_questions`:
  - 旧 subject id 的 deprecation 周期是按里程碑还是按版本号管理。

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `phase-0-taxonomy-freeze-and-migration-map` | `Phase 0` | `ready` | `codex` | 冻结测试分类、subject 收敛映射与兼容策略，给后续所有结构调整提供单一真源 | `-` |
| `phase-1-generic-test-taxonomy-and-dephase-cleanup` | `Phase 1` | `planned` | `codex` | 重组 `tests/`，去掉 `phase*`，抽出 subject-specific 资产 | `phase-0-taxonomy-freeze-and-migration-map` |
| `phase-2-suite-subject-root-and-manifest-cutover` | `Phase 2` | `planned` | `codex` | 落 `ProofSuite` / `BenchmarkSuite` 与 child manifest schema | `phase-0-taxonomy-freeze-and-migration-map` |
| `phase-3-proofsuite-scenario-convergence` | `Phase 3` | `planned` | `codex` | 把 proof-like subject 收敛为 `ProofSuite/scenarios/*` | `phase-1-generic-test-taxonomy-and-dephase-cleanup, phase-2-suite-subject-root-and-manifest-cutover` |
| `phase-4-benchmarksuite-workload-convergence` | `Phase 4` | `planned` | `codex` | 把 `Bench*` 收敛为 `BenchmarkSuite/workloads/*`，并吸收暂停的 AOT benchmark backend 结果 | `phase-2-suite-subject-root-and-manifest-cutover` |
| `phase-5-registry-selector-artifact-and-public-cutover` | `Phase 5` | `planned` | `codex` | 切 registry、selector、artifact、CLI/TUI 到 suite model | `phase-3-proofsuite-scenario-convergence, phase-4-benchmarksuite-workload-convergence` |
| `phase-6-capability-availability-and-pipeline-rollout` | `Phase 6` | `planned` | `codex` | 把 capability / availability / pipeline 变成正式可执行对象 | `phase-5-registry-selector-artifact-and-public-cutover` |
| `phase-7-compatibility-closure-and-legacy-cleanup` | `Phase 7` | `planned` | `codex` | 退役旧目录、旧别名与历史 phase 命名，完成干净收尾 | `phase-6-capability-availability-and-pipeline-rollout` |

说明：

- `Phase 1` 与 `Phase 2` 在 Phase 0 之后可以部分并行准备，但建议先完成 `Phase 1` 的 taxonomy 收口，再进行大规模 subject 物理迁移。
- `Phase 4` 不应直接沿旧 `Bench*` 根目录继续 rollout，而应把现有 `BenchArithmetic` AOT backend 成果并入新的 workload 模型。

## 7. 依赖

全局依赖关系固定为：

1. 先冻结 taxonomy、收敛映射与兼容策略。
2. 再把 `tests/` 做 generic 化，避免后续迁移继续制造历史命名。
3. 再建立 suite roots 与 child manifest schema。
4. 再收敛 proof 与 benchmark 两条主线。
5. 只有在 canonical source 已切到 suite model 后，才切 registry / selector / artifact / public surface。
6. 最后再把 capability、pipeline 和兼容层收尾纳入长期结构。

不能倒置的关键原因是：

- `tests/` 边界不清时，subject 迁移会把旧命名和旧耦合一起搬过去。
- suite schema 不稳定时，proof/benchmark 收敛没有稳定容器。
- canonical subject 还没收口时，public selector 与 artifact 路径会必然返工。

## 8. 风险

### 风险 1：先迁 subject，后清 tests

如果在 `tests/` 仍保留 `phase*` 与 subject-specific 依赖的情况下先大规模搬 `subjects/`，只会把旧耦合复制到新目录。

### 风险 2：Benchmark 继续沿旧根目录 rollout

如果 Phase 4 之前继续把 benchmark 结果铺到旧 `Bench*` 顶层 subject，后续 workload cutover 会增加二次迁移成本。

### 风险 3：兼容别名失控

如果旧 subject id、旧 CLI、旧 artifact path 长期不收口，suite model 会和旧模型长期双栈并存。

### 风险 4：capability 只写文档，不进 manifest

如果 capability/availability 只存在文档，不进入 manifests 与 pipelines，release gate 最终仍会退化成手工清单。

### 风险 5：特殊目录归宿不清

像 `golden`、`HelloWorld`、`MainlineFeaturePack` 这类目录如果 Phase 0 不先定性，后续每个阶段都会重复争论。

## 9. 备选路径

### 方案 A：taxonomy-first，suite-first，public-later

- 优点：最稳，返工最少，能先把 `tests/` 与 `subjects/` 的边界理干净。
- 缺点：前两阶段对外可见变化有限。

### 方案 B：subject-first bulk migration

- 优点：表面上更快看到新 `ProofSuite/BenchmarkSuite`。
- 缺点：如果 `tests/` 还没 generic 化，迁过去的会是旧耦合。

### 方案 C：public-first compatibility cutover

- 优点：外部入口更早统一。
- 缺点：内部 canonical structure 未稳定前，CLI/TUI 和 artifact 会高概率返工。

当前采用：

- **方案 A：taxonomy-first，suite-first，public-later**

## 10. 当前建议推进顺序

1. 先创建 `phase-0-taxonomy-freeze-and-migration-map` child plan。
2. `Phase 0` 完成后，进入 `Phase 1`，先把 `tests/` 收口。
3. 在 `Phase 1` 命名和 ownership 稳定后，进入 `Phase 2`，落 suite roots 与 manifests。
4. `Phase 2` 完成后，优先做 `Phase 3`，先收敛 proof 主线。
5. `Phase 3` 稳定后，再做 `Phase 4`，把 benchmark 主线并入 `BenchmarkSuite`。
6. proof/benchmark 都完成 canonical 收口后，再做 `Phase 5` 的 registry/selector/artifact/public cutover。
7. 然后做 `Phase 6`，把 capability/availability/pipeline 变成正式对象。
8. 最后以 `Phase 7` 关闭兼容层、老目录和历史命名。

## 11. 已确认的计划前提

以下边界已由用户确认，可直接作为后续 child plan 的前提：

1. `tests/` 应回到 generic mechanism 层，subject-specific 配置必须迁回 `subjects/`.
2. `tests/unit/run/` 的 `phase*` 命名必须系统性清理。
3. proof 方向要合并为 suite subject，不再继续增殖 proof 顶层 subject。
4. benchmark 方向也要合并为 suite subject，不再继续增殖 `Bench*` 顶层 subject。
5. 理想扩展单位是 `scenario` 与 `workload`，不是新顶层 subject。
6. 多平台 IL2CPP + 热更的额外能力必须进入测试模型，而不是停留在文档口头约定。
7. 当前暂停的 AOT benchmark backend 子任务后续必须服从新的 suite/workload 结构，而不是反过来主导结构。
8. `golden` 被定义为核心工程级 subject 样例，canonical 进入 `ProofSuite/scenarios/solution-*`，而不是迁成普通 support asset。

## 12. 结论

本 roadmap 已把这次重构明确拆成 8 个阶段，并给出了每阶段的：

- 结构调整
- 修改 checklist
- 阶段性目标
- 验收标准

下一步不应直接大面积改代码，而应从 `Phase 0` child plan 开始，把 taxonomy、subject 收敛映射和兼容策略写成可执行的第一份 plan。只有 Phase 0 收口之后，后续的 `tests/` 与 `subjects/` 迁移才不会失控。
