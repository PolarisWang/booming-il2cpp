# 测试体系 Solution-First + Attribute-Declared Hybrid Roadmap v1.03

Date: 2026-04-12
Status: completed

## 1. 目标

把当前测试体系从“proof/benchmark 顶层 subject + heavy per-subject manifest”迁移到以下统一模型：

- 顶层 `subject` 只代表 canonical solution source bundle
- 工程级验证成为主验证层
- runtime proof / benchmark 通过 `ChaosUnitTest` / `ChaosBenchmark` 在 solution 内声明
- `subject.manifest.json` 变成薄 orchestration manifest
- `tests/` 只保留 generic mechanism 与 generic fixtures

本版 roadmap 的直接目标是：

- 落定结构与职责边界
- 给出分阶段 cutover 路线
- 给出每阶段的结构调整、执行 checklist 与验收标准

## 2. 范围边界

本 roadmap 负责：

- `subjects/` 的 solution-first 重组规则
- `subject.manifest.json` 的瘦身方向
- `ChaosUnitTest` / `ChaosBenchmark` 契约与发现模型
- 工程级验证与源码声明测试的分层
- `tests/` 的 taxonomy 清理与命名改造
- registry / selector / artifact / pipeline 对新模型的切换顺序

本 roadmap 不负责：

- 立即完成所有代码实现
- 立即落完所有历史 subject 迁移
- 立即完成所有设备农场与真机自动化
- 立即移除全部兼容 alias

## 3. 非目标

当前不作为本 roadmap 的一次性目标：

- 为每个历史 proof / benchmark 立刻拍板最终合并后的 subject 名称
- 立即统一所有 CLI 体验细节
- 一次性迁出所有历史 artifact 数据
- 一次性给全部平台建立 nightly/release 自动化

## 4. 阶段列表

### Phase 0: Hybrid Model Freeze And Migration Inventory

- 冻结 v1.03 设计基线
- 冻结 subject 命名与保留规则
- 盘点历史 subject 的归宿
- 盘点工程级验证与声明测试的边界

### Phase 1: Generic Test Taxonomy Cleanup

- 清理 `tests/` 目录职责
- 去掉 `phase*` / `stage*` 命名
- 把 fixture 从测试代码目录中剥离成 generic fixture
- 给后续 schema/discovery cutover 清出干净测试骨架

### Phase 2: `Chaos.TestFramework` Contract And Discovery Foundation

- 建立 `ChaosUnitTest` / `ChaosBenchmark` 契约
- 冻结枚举与紧凑类型策略
- 设计 declaration discovery 与 compiled catalog schema

### Phase 3: Thin Manifest And Engineering-Core Schema

- 把 per-subject heavy manifest 收敛成薄 manifest
- 冻结 `engineeringProfile`
- 冻结 orchestration profile 引用方式
- 设计工程级 validation/workload catalog

### Phase 4: Archetype Subject Cutover

- 先迁移 `golden` 到 `Solution*` archetype subjects
- 先把工程级验证主链打通
- 把 `convert-golden-multi-project` 迁回 archetype subject 的工程 workload

### Phase 5: Functional Subject Declared-Test Migration

- proof-like 顶层 subject 并回 canonical solution source
- 用 `ChaosUnitTest` 替代 source 外部的 proof 定义
- 建立 functional/mainline 类 subject 的统一声明模式

### Phase 6: Benchmark And Hot-Update Declared-Test Migration

- benchmark-like 顶层 subject 并回 canonical solution source
- 用 `ChaosBenchmark` 替代 runtime benchmark 外部定义
- 将热更与 mixed execution 类样例纳入统一 subject 模型

### Phase 7: Registry / Selector / Artifact / Pipeline Cutover

- 切换发现、选择、报告、artifact、pipeline 模型
- 让工程级条目与声明条目共同进入统一 catalog
- 建立新 CLI/report/ledger 归属方式

### Phase 8: Compatibility Closure And Legacy Cleanup

- 收尾历史目录、旧命名、旧 alias
- 把 solution-first hybrid model 变成唯一 canonical 模型

## 5. 每阶段完成定义

### Phase 0

- `goal`
  - 冻结 v1.03 基线，明确“什么是 subject、什么是工程级验证、什么是声明测试、哪些历史 root 应合并”。

- `structure_changes`
  - 文档层新增：
    - `design-v1-03.md`
    - `roadmap-v1-03.md`
  - 产出迁移映射清单：
    - legacy subject inventory
    - naming rules
    - engineering vs declared boundary matrix

- `checklist`
  - 冻结 `subject = canonical solution source bundle`
  - 冻结“不再默认使用 `scenarios/` / `workloads/` 物理目录”
  - 冻结 `ChaosUnitTest` / `ChaosBenchmark` 命名
  - 冻结“工程级验证优先，attribute 为增强层”的层次
  - 冻结低基数字段优先使用 `byte` / `ushort` / `uint`
  - 盘点所有现有顶层 subject，分类为：
    - archetype solution subject
    - mainline/feature solution subject
    - engine/interop solution subject
    - hot-update solution subject
    - mobile/device solution subject
    - legacy proof shell
    - legacy benchmark shell
  - 确认 `golden` 的迁移目标：
    - `GoldenSimpleLib` -> `SolutionSimpleLib`
    - `GoldenMultiProject` -> `SolutionMultiProject`
    - `GoldenWithPackage` -> `SolutionPackageReference`
  - 确认 `convert-golden-multi-project` 是工程 workload，不是 `ChaosBenchmark`
  - 冻结顶层命名规则：
    - archetype 用 `Solution*`
    - materially distinct 大 solution 可保留语义名
    - 默认不再新增 `*Proof` / `Bench*`

- `exit_criteria`
  - v1.03 设计被采纳为新基线
  - 历史 subject 都有明确迁移方向
  - 工程级验证与声明测试的边界不再摇摆

- `deliverables`
  - v1.03 设计文档
  - v1.03 roadmap
  - legacy mapping 清单

- `dependencies`
  - none

- `acceptance_checks`
  - 不再出现“是否继续以 proof/benchmark 为顶层 root”的分歧
  - 不再出现“convert/codegen 是否由 `ChaosBenchmark` 声明”的分歧

- `open_questions`
  - none after v1.03 freeze

### Phase 1

- `goal`
  - 清理 `tests/` 的职责与命名，让 `tests/` 只剩 generic mechanism 与 generic fixture。

- `structure_changes`
  - 建议目标目录：
    - `tests/fixtures/`
    - `tests/contracts/`
    - `tests/unit/`
    - `tests/integration/`
    - `tests/tooling/`
  - 拟迁出或合并：
    - `tests/proof/`
    - `tests/perf/` 中的 subject-specific 项
    - `tests/platform/`
    - `tests/gate/`
    - `tests/registry/` 中的 fixture 化目录
  - 目标子类目：
    - `tests/unit/registry`
    - `tests/unit/selection`
    - `tests/unit/planning`
    - `tests/unit/execution`
    - `tests/unit/reporting`
    - `tests/unit/performance`
    - `tests/unit/compatibility`
    - `tests/integration/cli`
    - `tests/integration/registry`
    - `tests/integration/pipelines`
    - `tests/integration/platform-hosts`
    - `tests/integration/device-runners`

- `checklist`
  - 列出所有 `test_phase*` / `test_stage*` 文件重命名表
  - 给每个现有 `tests/unit/run/test_phase*` 文件指定目标功能目录
  - 把 `tests/integration/run/fixtures/` 迁到 `tests/fixtures/integration/`
  - 把 `tests/registry/` 中 fixture 型内容迁到 `tests/fixtures/registry/`
  - 把 `tests/contracts/managed/*` 这类 generic sample 归拢到 `tests/fixtures/contracts/`
  - 标记 `tests/perf/convert-golden-multi-project` 为待迁出的工程 workload
  - 清理 `__pycache__`、`bin/`、`obj/` 等噪音目录进入 ignore/cleanup 规则
  - 保证 `tests/` 中不再保存 canonical subject source

- `exit_criteria`
  - `tests/` 不再依赖 `phase*` / `stage*` 命名
  - generic fixture 有独立位置
  - `tests/perf/` 不再被当作 subject-specific workload 长期容器

- `deliverables`
  - 新 `tests/` taxonomy 文档
  - 文件重命名映射表
  - fixture 收敛方案

- `dependencies`
  - Phase 0

- `acceptance_checks`
  - 新增机制测试时，团队能明确知道该放在哪个 functional bucket
  - 新增 fixture 时，不再误放进 `tests/unit/run` 或 `tests/registry`

- `open_questions`
  - `tests/support.py` 是否继续保持单文件，还是拆成 `tests/support/` package

### Phase 2

- `goal`
  - 建立 `Chaos.TestFramework` 与 declaration discovery 的稳定契约。

- `structure_changes`
  - 新增 reference project：
    - `src/reference/Chaos.TestFramework/`
  - 明确代码边界：
    - `src/managed/` 继续承载 IL2CPP 阶段核心代码
    - `src/reference/` 承载被 subject solution 直接引用的 C# 层框架代码
  - 可能新增或扩展：
    - declaration discovery schema
    - compiled catalog schema
  - 测试契约覆盖新增：
    - attribute schema tests
    - discovery contract tests

- `checklist`
  - 冻结 `ChaosUnitTestAttribute`
  - 冻结 `ChaosBenchmarkAttribute`
  - 冻结以下枚举的底层类型：
    - `ChaosUnitCategory : byte`
    - `ChaosBenchmarkCategory : byte`
    - `ChaosMetric : ushort`
    - `ChaosRuntimeFeature : uint`
    - `ChaosEvidenceKind : ushort`
  - 为全部枚举及枚举成员补齐注释，保证共享框架可直接作为公开引用面
  - 定义 `Alias` 仅为可选短名，不是主 identity
  - 定义稳定 ID 自动生成规则
  - 定义 `testDeclarationMode`：
    - `auto`
    - `none`
    - `require`
  - 设计 discovery 行为：
    - 不加载程序集执行
    - 直接读 metadata/custom attributes
  - 设计 compiled catalog 条目类型：
    - `declaredUnitTests`
    - `declaredBenchmarks`
  - 为“无 DLL 引用”“有 DLL 无条目”“有条目”三种情况定义输出语义
  - 设计 `ChaosFixture` / `ChaosExpect` / `ChaosBaseline` / `ChaosAssert` 一类 helper API，承接原本想落在 `fixtures/expected/baselines/validation` 目录中的语义
  - 定义 `assets/` 与 C# helper API 的边界，确保目录只保留无法 API 化的文件型资产

- `exit_criteria`
  - attribute API 冻结
  - 紧凑类型方案冻结
  - discovery contract 冻结
  - 目录语义 API 化边界冻结

- `deliverables`
  - `Chaos.TestFramework` 契约设计
  - discovery schema
  - compiled catalog schema
  - helper API 设计草案

- `dependencies`
  - Phase 0
  - 建议在 Phase 1 taxonomy 清理有明确方向后启动

- `acceptance_checks`
  - 不需要通过字符串数组表达 metrics / tags / capabilities
  - 新 subject 不需要为了新增一个测试点先改中央枚举 identity 表

- `open_questions`
  - compiled catalog 的持久化是否需要单独 artifact 文件，还是仅作为内存模型 + debug export

### Phase 3

- `goal`
  - 把 per-subject manifest 从 heavy declarative JSON 收敛为薄 manifest，并正式定义工程级验证模型。

- `structure_changes`
  - `subject.manifest.json` 目标保留：
    - `subjectId`
    - `displayName`
    - `source`
    - `sourceModel`
    - `dependencyModel`
    - `executablePlan`
    - `engineeringProfile`
    - `testDeclarationMode`
    - `orchestration`
    - `availability`
    - `compatibility`
  - 从长期结构中移出 per-subject 大段：
    - `validationProfiles`
    - `executionPipelines`
    - `environmentMatrices`
  - 引入 shared profile 引用：
    - `matrixProfile`
    - `pipelineProfile`
    - `budgetProfile`
    - `baselineProfile`

- `checklist`
  - 定义 `engineeringProfile` 枚举及其默认条目展开规则
  - 定义工程级 validation kinds
  - 定义工程级 workload kinds
  - 确认 convert/codegen/native link/native executable/package/hot-update path 均归工程级模型
  - 定义薄 manifest schema
  - 定义 availability 的最小状态集：
    - `ready`
    - `planned`
    - `blocked`
    - `unsupported`
  - 定义 compatibility alias/redirect 规则
  - 确认 shared profile 的来源与解析位置

- `exit_criteria`
  - heavy manifest 不再是目标形态
  - 工程级 catalog 条目可被稳定建模
  - subject 目录可在不引入 `scenarios/` / `workloads/` 的前提下表达完整主体语义

- `deliverables`
  - 薄 manifest schema
  - engineering profile 设计
  - orchestration profile 引用设计

- `dependencies`
  - Phase 0
  - Phase 2

- `acceptance_checks`
  - 一个 subject manifest 不再需要复制完整 pipeline DAG
  - 一个 runtime-only subject 也能被完整展开为工程级验证条目

- `open_questions`
  - shared profile 定义最终是放在代码内 registry，还是独立配置 root

### Phase 4

- `goal`
  - 先用 archetype solution subjects 打通工程级验证主链。

- `structure_changes`
  - 新增或重命名：
    - `subjects/SolutionSimpleLib/`
    - `subjects/SolutionMultiProject/`
    - `subjects/SolutionPackageReference/`
  - 历史 `subjects/golden/*` 退化为 compatibility shell 或 alias
  - `tests/perf/convert-golden-multi-project` 迁回 archetype subject 的工程 workload

- `checklist`
  - 把 `GoldenSimpleLib` 迁为 `SolutionSimpleLib`
  - 把 `GoldenMultiProject` 迁为 `SolutionMultiProject`
  - 把 `GoldenWithPackage` 迁为 `SolutionPackageReference`
  - 给三类 archetype 配置合适的：
    - `sourceModel`
    - `dependencyModel`
    - `executablePlan`
    - `engineeringProfile`
  - 建立 archetype 的默认工程验证集合
  - 迁出 `convert-golden-multi-project`
  - 校验 archetype subjects 可以承担：
    - project graph
    - dependency resolution
    - convert
    - codegen
    - native executable（如适用）

- `exit_criteria`
  - `golden` 不再是 canonical 结构
  - archetype subjects 成为新的工程级真源
  - 工程 workload 已经离开 `tests/perf`

- `deliverables`
  - 三个 archetype solution subjects
  - archetype 工程验证档位
  - old golden alias 策略

- `dependencies`
  - Phase 3

- `acceptance_checks`
  - 新人理解 archetype 时，不再需要先理解 `golden` 这个历史名字
  - archetype subjects 不引用 testing DLL 时，仍可完整参与核心工程验证

- `open_questions`
  - archetype 是否都默认要求 native executable 路径，还是允许部分先停留在 managed-output 档位

### Phase 5

- `goal`
  - 把 proof-like 顶层 root 合并为 canonical solution subject，并用 `ChaosUnitTest` 表达源码内 proof。

- `structure_changes`
  - proof-like 顶层 root 将被：
    - 合并进现有 canonical solution subject
    - 或退化为 compatibility shell
  - 典型主线 subject 将收敛为：
    - `MainlineFeaturePack`
    - `EngineHostPack`（候选）
    - `InteropPack`（候选）
    - `MobileHostPack`（候选）

- `checklist`
  - 盘点所有 `*Proof` 顶层 subject 的 source 归属
  - 判定它们是：
    - 工程级验证项
    - `ChaosUnitTest`
    - 独立 solution subject
  - 对共享同一 source bundle 的 proof-like roots 执行合并设计
- 为需要源码内 proof 的 solution 引入 `Chaos.TestFramework` 引用
  - 把 proof 函数改写为 `ChaosUnitTest`
  - 保持无测试 DLL 的 subject 仍然只走工程级主线
  - 更新 unit/integration fixture 与 selector alias

- `exit_criteria`
  - proof-like 顶层 root 不再是增长主路径
  - correctness 类源码测试已有统一声明方式
  - functional/mainline 类 solution subject 的 owner 边界变清晰

- `deliverables`
  - functional subject consolidation 方案
  - `ChaosUnitTest` 应用样例
  - legacy proof alias 列表

- `dependencies`
  - Phase 2
  - Phase 3
  - 建议在 Phase 4 archetype 稳定后推进

- `acceptance_checks`
  - 新增一个 correctness proof 时，不再需要先造新顶层 `*Proof`
  - proof 入口可以从源码直接被发现，而不是从 subject 子目录猜测

- `open_questions`
  - `HelloWorld` / `HelloWorldObject` 最终是独立 archetype 还是并入更大 onboarding/mainline solution

### Phase 6

- `goal`
  - 把 benchmark-like 与 hot-update 类样例并回 solution subject，并用 `ChaosBenchmark` 表达 runtime benchmark。

- `structure_changes`
  - `Bench*` 顶层 roots 将被：
    - 合并为对应 subject 的 declared benchmark
    - 或退化为 compatibility shell
  - hot-update / mixed execution 将并入对应 host solution 或 feature solution

- `checklist`
  - 盘点所有 `Bench*` root，区分：
    - 实际是 runtime benchmark
    - 实际是工程 workload
    - 实际是独立 solution source
  - 对 runtime benchmark 使用 `ChaosBenchmark`
  - 对工程 workload 保持在 engineering layer
  - 设计 hot-update roundtrip/load/dispatch/rollback 的归宿
  - 设计 mixed execution 与 interpreter 相关条目的归宿
  - 把暂停中的 AOT benchmark backend 成果映射到新模型：
    - 工程 workload 部分进入 engineering layer
    - runtime benchmark 部分进入 `ChaosBenchmark`

- `exit_criteria`
  - `Bench*` 不再天然等于顶层 subject
  - runtime benchmark 有统一声明语法
  - 热更/混合执行类对象不再额外再造 benchmark root

- `deliverables`
  - benchmark subject consolidation 方案
  - hot-update consolidation 方案
  - `ChaosBenchmark` 应用样例

- `dependencies`
  - Phase 2
  - Phase 3
  - 建议在 Phase 5 functional consolidation 明朗后推进

- `acceptance_checks`
  - 新增一个 runtime benchmark 时，默认是给现有 subject 加方法声明
  - convert/codegen 这类系统 workload 不会被误建成 `ChaosBenchmark`

- `open_questions`
  - 哪些 `Bench*` 的 source bundle 足够独立，值得保留为独立 subject

### Phase 7

- `goal`
  - 切换 registry、selector、CLI、artifact、pipeline 到 hybrid model。

- `structure_changes`
  - registry 发现对象统一扩展为：
    - subject
    - engineering validation
    - engineering workload
    - declared unit test
    - declared benchmark
  - artifact 路径切换到：
    - `engineering/validations`
    - `engineering/workloads`
    - `declared/unit`
    - `declared/benchmark`
  - report/ledger 新增 entry family 字段

- `checklist`
  - 定义 unified catalog scan
  - 定义 selector 语义与兼容 alias
  - 更新 CLI/TUI 展示
  - 更新 JSON/reporting/ledger 归属字段
  - pipeline 解析 shared profile 引用
  - availability 参与默认过滤逻辑
  - budget/baseline 分别接到 engineering 与 declared benchmark

- `exit_criteria`
  - hybrid model 成为执行主入口
  - report/artifact 能明确区分 engineering vs declared
  - legacy selectors 仅剩 adapter 责任

- `deliverables`
  - unified registry model
  - selector/CLI cutover 方案
  - artifact layout cutover 方案

- `dependencies`
  - Phase 4
  - Phase 5
  - Phase 6

- `acceptance_checks`
  - 用户可以稳定选择：
    - 某个 subject 的工程 workload
    - 某个 subject 的 unit test
    - 某个 subject 的 benchmark
  - report 不再把工程 workload 和 runtime benchmark 混在一个桶里

- `open_questions`
  - CLI 最终是否要暴露 `engineering` 一等命令，还是作为 `run test subject` 的子模式

### Phase 8

- `goal`
  - 关闭兼容层，形成干净的 solution-first hybrid canonical 布局。

- `structure_changes`
  - 退役或归档：
    - 历史 `*Proof` roots
    - 历史 `Bench*` roots
    - 历史 `subjects/golden/*`
    - 历史 `tests/proof/`
    - 历史 `tests/perf/` 中 subject-specific workload
    - 历史 `phase*` / `stage*` 文件名
    - 历史 compatibility wrappers
  - 更新 onboarding / wiki / docs/dev 正式入口

- `checklist`
  - 删除或归档所有非 canonical subject roots
  - 收敛所有 legacy alias 生命周期
  - 清理最终残留的 `phase*` / `stage*` 命名
  - 校验 `tests/` 与 `subjects/` 边界不再回混
  - 补齐 ignore / cleanup 规则

- `exit_criteria`
  - v1.03 hybrid model 成为唯一 canonical 模型
  - 顶层 `subject` 不再混杂 proof/benchmark roots
  - `tests/` 不再承载 subject 真源
  - 文档、命令、artifact、report 全部对齐新模型

- `deliverables`
  - legacy cleanup 清单
  - deprecation/retirement 记录
  - onboarding/wiki 更新

- `dependencies`
  - Phase 7

- `acceptance_checks`
  - 新人阅读仓库时，不再需要先理解一整套历史 phase/stage/proof/bench root 语义
  - 新增测试样例与新增测试点的路径都能一句话讲清

- `open_questions`
  - legacy alias 的退役周期按 milestone 还是按版本号管理

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `phase-0-hybrid-model-freeze-and-migration-inventory` | `Phase 0` | `ready` | `codex` | 冻结 v1.03 基线、命名规则与迁移盘点 | `-` |
| `phase-1-generic-test-taxonomy-cleanup` | `Phase 1` | `planned` | `codex` | 清理 `tests/` taxonomy、命名与 fixture 归属 | `phase-0-hybrid-model-freeze-and-migration-inventory` |
| `phase-2-chaos-testing-contract-and-discovery-foundation` | `Phase 2` | `planned` | `codex` | 建立 `ChaosUnitTest` / `ChaosBenchmark` 与 `Chaos.TestFramework` discovery contract | `phase-0-hybrid-model-freeze-and-migration-inventory` |
| `phase-3-thin-manifest-and-engineering-core-schema` | `Phase 3` | `completed` | `codex` | 建立薄 manifest 与 engineering-core schema | `phase-0-hybrid-model-freeze-and-migration-inventory, phase-2-chaos-testing-contract-and-discovery-foundation` |
| `phase-4-archetype-subject-cutover` | `Phase 4` | completed | `codex` | 把 `golden` 升级为 `Solution*` archetype subjects 并打通工程主链 | `phase-3-thin-manifest-and-engineering-core-schema` |
| `phase-5-functional-subject-declared-test-migration` | `Phase 5` | completed | `codex` | 把 proof-like roots 并回 canonical subject，并迁到 `ChaosUnitTest` | `phase-2-chaos-testing-contract-and-discovery-foundation, phase-3-thin-manifest-and-engineering-core-schema, phase-4-archetype-subject-cutover` |
| `phase-6-benchmark-and-hotupdate-declared-test-migration` | `Phase 6` | `completed` | `codex` | 把 benchmark-like/hot-update roots 并回 canonical subject，并迁到 `ChaosBenchmark` | `phase-2-chaos-testing-contract-and-discovery-foundation, phase-3-thin-manifest-and-engineering-core-schema, phase-4-archetype-subject-cutover` |
| `phase-7-registry-selector-artifact-pipeline-cutover` | `Phase 7` | `completed` | `codex` | 切换 discovery、selector、CLI、artifact、pipeline 到 hybrid model | `phase-4-archetype-subject-cutover, phase-5-functional-subject-declared-test-migration, phase-6-benchmark-and-hotupdate-declared-test-migration` |
| `phase-8-compatibility-closure-and-legacy-cleanup` | `Phase 8` | `completed` | `codex` | 关闭兼容层并清理历史目录、旧命名与 alias | `phase-7-registry-selector-artifact-pipeline-cutover` |

## 7. 依赖

关键依赖关系如下：

- Phase 0 是全部阶段的前置
- Phase 1 可与 Phase 2 部分并行准备，但正式落地最好在 v1.03 freeze 后进行
- Phase 3 依赖 Phase 2，因为 manifest 的 `testDeclarationMode` 与 catalog 结构需要 discovery contract
- Phase 4 依赖 Phase 3，因为 archetype subjects 要首先站在薄 manifest + engineering profile 上
- Phase 5 / Phase 6 都依赖 Phase 2 + Phase 3
- Phase 7 必须等待 archetype 与 declared-test 两条主链都稳定
- Phase 8 必须等待 Phase 7 成为主入口

## 8. 风险

### 风险 1：工程级验证被 attribute 化

影响：

- convert/codegen/native executable 语义错位
- 工程主链回归不完整

应对：

- Phase 0 和 Phase 3 明确工程级模型边界

### 风险 2：类型系统虽然更强，但框架 DLL 膨胀

影响：

- subject 引入的测试 DLL 变重
- 维护成本上升

应对：

- Phase 2 严格控制 `Chaos.TestFramework` 只提供最小 attribute + enum

### 风险 3：共享 profile 外部化后可读性下降

影响：

- subject manifest 变薄，但定位实际 pipeline 变慢

应对：

- Phase 3 / Phase 7 需要提供 profile 展开视图与 report 回显

### 风险 4：迁移期间 alias 太多

影响：

- 用户心智继续混乱

应对：

- Phase 8 设定清晰 retirement 策略

## 9. 备选路径

已否决路径：

- 路径 A：纯 attribute-driven
  - 问题：跨平台 orchestration 无法干净承载
- 路径 B：继续 physical `scenario/` / `workload/` 目录
  - 问题：subject 目录继续膨胀，声明离源码仍然过远
- 路径 C：把所有 proof/benchmark 合成少数 mega-suite 顶层 subject
  - 问题：丢失 solution-first source truth，工程验证与业务语义都被稀释

当前推荐路径：

- 路径 D：solution-first + engineering-core + attribute-declared hybrid

## 10. 当前建议推进顺序

建议按以下顺序推进：

1. 立即以 v1.03 冻结设计基线。
2. 从 `phase-0-hybrid-model-freeze-and-migration-inventory` 开始，先完成盘点与命名冻结。
3. 并行准备 Phase 1 的 `tests/` taxonomy mapping 和 Phase 2 的 testing contract 草案。
4. 先落 Phase 3，再迁 archetype subjects。
5. 等 archetype 工程主链稳定后，再批量推进 proof-like 与 benchmark-like subject 的 declared-test 化。
6. 最后再切 registry/CLI/artifact/pipeline 主入口并清理兼容层。

这个顺序的原因是：

- 不先冻结 engineering-core 边界，后续会不断把系统 workload 误迁到 attribute
- 不先打通 archetype solution，functional/benchmark 迁移会缺乏稳定基准
- 不先清理 `tests/` taxonomy，后续机制测试会持续向历史命名回流

