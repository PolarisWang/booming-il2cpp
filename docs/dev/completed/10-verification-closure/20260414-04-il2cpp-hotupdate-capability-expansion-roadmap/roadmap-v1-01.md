# IL2CPP 与热更能力覆盖扩展 Roadmap v1.01

Date: 2026-04-14 09:47:42 +08:00
Status: completed

## 1. 目标

在现有 retained-subject 体系基础上，推进下一轮 IL2CPP 与热更能力覆盖扩展，使仓库长期具备以下能力：

- 用稳定的二级 capability taxonomy 表达 C# 语义覆盖面。
- 用 canonical solution archetype 表达完整 solution 与完整热更 workflow。
- 把历史语法点重构为单文件条目，并按能力族归并到少量共享项目。
- 保持顶层 subject 数量不变，不继续扩大 string 调度协议。
- 让 framework、planner、registry、dashboard、docs 都能消费这套能力模型。

## 2. 范围边界

本 roadmap 负责：

- 冻结 capability taxonomy 与 archetype inventory。
- 扩展 `src/reference/Chaos.TestFramework` 的公开元数据模型。
- 扩展 `SolutionCorePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack` 的内部结构。
- 迁移历史 proof/benchmark 到新的能力族布局。
- 补齐 pipeline / reporting / dashboard / docs 对新元数据的消费。

本 roadmap 不负责：

- 新增第四个顶层 subject。
- 引入新的 executable plan 类型。
- 一次性打通所有移动端真机农场能力。
- 为每个历史语法点单独创建一个测试工程。

## 3. 非目标

- 不回退到 phase 风格命名与 subject-special test 目录。
- 不重新引入大量外置 `validation/`、`scenario/` 文件。
- 不把 `Chaos.TestFramework` 移到 `src/managed`。
- 不继续扩大字符串协议作为 planner / dashboard / registry 主键。

## 4. 阶段列表

### Phase 0: Capability Taxonomy And Inventory Freeze

- 冻结二级 capability taxonomy。
- 冻结 canonical solution archetype inventory。
- 冻结 3 个 retained subjects 的 ownership 和目标内部目录。
- 冻结 `CoreLibReferenceSolution` 的 repository-owned reference bundle 方案。

### Phase 1: `Chaos.TestFramework` Metadata Expansion

- 为 capability family / capability item / archetype / hot-update capability 扩展轻量枚举与 attribute 字段。
- 保持 proof/benchmark 继续采用 `ChaosUnitTest` / `ChaosBenchmark` / `Assert` 心智模型。
- 让 metadata discovery、compiled catalog、registry 能识别这些声明。

### Phase 2: `SolutionCorePack` Mechanical Split And Coverage Expansion

- 把历史语法点按单文件条目迁入 `FeatureSlices/`。
- 尽量按能力族合并到共享项目，不制造过多工程。
- 完善 `ReferenceAssemblySolution`、`CoreLibReferenceSolution`、`MixedReferenceClosureSolution` 等 archetype。
- 补齐 native baseline benchmark 映射。

### Phase 3: `HotUpdateHostPack` Full-Project Workflow Expansion

- 建立 `FullProjectHotUpdateSolution` 作为 canonical archetype。
- 明确 host / shared contracts / patch modules / package metadata 组织方式。
- 补齐 compatibility、rollback、metadata supplement、bridge callback 等 proof/benchmark。

### Phase 4: `MixedExecutionFeaturePack` Mixed Capability Closure

- 完善 mixed/interpreter 专属能力族。
- 补齐 managed/native/interpreter 三向对照 benchmark。
- 明确 mixed execution 的 supported / partial / unsupported 边界。

### Phase 5: Pipeline, Reporting And Docs Alignment

- 扩展 planner、registry、dashboard、reporting 消费 capability/archetype 元数据。
- 更新 architecture / usage 文档。
- 统一 unsupported / partial support 的显示与证据输出。

### Phase 6: Verification And Archive

- 重新跑 retained subjects proof/benchmark/archetype 真实链路。
- 跑关键测试套件。
- 归档 roadmap，并冻结新的 capability 基线。

## 5. 每阶段完成定义

### Phase 0

- `goal`
  - 在进入实现前冻结 vocabulary、inventory 与 ownership，防止后续边做边改方向。
- `exit_criteria`
  - 每个新增测试目标都能回答“属于哪个 subject、哪个能力族、哪个 archetype、是否需要 benchmark”。
- `deliverables`
  - capability taxonomy freeze
  - archetype inventory freeze
  - retained-subject ownership freeze
- `dependencies`
  - 无
- `open_questions`
  - 已冻结：
    - OQ2: 采用混合模型，但工程落地按“外部 `matrix/manifest` 为主，attribute 只放内在 metadata”执行
    - OQ3: `CoreLibReferenceSolution` 与相关 reference archetype 统一采用 repository-owned medium `foundation bundle`

### Phase 1

- `goal`
  - 建立新能力模型对应的公开 metadata 契约。
- `exit_criteria`
  - 单个 `[ChaosUnitTest]` / `[ChaosBenchmark]` 声明即可表达 capability / archetype / execution mode 基本信息。
- `deliverables`
  - 扩展后的 `Chaos.TestFramework`
  - metadata discovery / catalog schema 更新
- `dependencies`
  - Phase 0
- `open_questions`
  - OQ2 收敛后可关闭

### Phase 2

- `goal`
  - 让 `SolutionCorePack` 成为能力覆盖扩展的主承载体。
- `exit_criteria`
  - 绝大多数历史语法点都已迁入 capability family 目录，并能映射到真实 proof/archetype。
- `deliverables`
  - 新版 `FeatureSlices/`
  - 完整 archetype 目录布局
  - 补齐后的 proof/benchmark
- `dependencies`
  - Phase 1
- `open_questions`
  - 已冻结：
    - OQ4: 所有历史语法点先有 proof；benchmark 只覆盖高价值 runtime path，而不是为每个语法点机械补 benchmark

### Phase 3

- `goal`
  - 固化完整热更工程组织方式与验证闭环。
- `exit_criteria`
  - 用户可以直接从 `HotUpdateHostPack` 的 canonical archetype 理解全工程热更如何组织。
- `deliverables`
  - `FullProjectHotUpdateSolution`
  - compatibility / rollback / supplement / callback proof
- `dependencies`
  - Phase 1
- `open_questions`
  - patch package 最小公开契约是否需要单独 product-facing 文档

### Phase 4

- `goal`
  - 把 mixed/interpreter 覆盖面从“已有链路”提升到“完整能力边界”。
- `exit_criteria`
  - mixed execution 的 supported / partial / unsupported 边界可被明确表达。
- `deliverables`
  - mixed capability inventory
  - mixed proof / benchmark 扩展
- `dependencies`
  - Phase 1
- `open_questions`
  - 是否需要把部分 mixed case 同步回 `SolutionCorePack` 作为 shared baseline

### Phase 5

- `goal`
  - 让外层工具和文档真正消费新能力模型。
- `exit_criteria`
  - planner、registry、dashboard、architecture docs 对 capability/archetype 的表达一致。
- `deliverables`
  - dashboard / reporting / registry 更新
  - architecture / usage docs 更新
- `dependencies`
  - Phase 2, Phase 3, Phase 4
- `open_questions`
  - 已冻结：
    - OQ5: dashboard 采用双层入口，顶层总览按 capability family，二级 drill-down 按 archetype / solution / case，且以 managed baseline 作为默认对比基线

### Phase 6

- `goal`
  - 用最新真实证据关闭整条 follow-up。
- `exit_criteria`
  - retained subjects、文档、planner、dashboard 与关键测试套件都基于最新模型重新通过。
- `deliverables`
  - 最终验证证据
  - archived roadmap
- `dependencies`
  - Phase 5
- `open_questions`
  - 无

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260414-05-phase-0-capability-taxonomy-and-inventory-freeze` | `Phase 0` | `completed` | `codex` | 冻结二级 capability taxonomy、reference bundle 范围、subject ownership 与 archetype inventory | `-` |
| `20260414-06-phase-1-chaos-test-framework-metadata-expansion` | `Phase 1` | `completed` | `codex` | 扩展 `Chaos.TestFramework` 与 metadata discovery 契约 | `20260414-05-phase-0-capability-taxonomy-and-inventory-freeze` |
| `20260414-07-phase-2-solution-core-pack-mechanical-split` | `Phase 2` | `completed` | `codex` | 迁移历史语法点并扩展 `SolutionCorePack` capability/archetype | `20260414-06-phase-1-chaos-test-framework-metadata-expansion` |
| `20260414-08-phase-3-hotupdate-host-pack-full-project-workflow` | `Phase 3` | `completed` | `codex` | 建立完整热更 canonical workflow | `20260414-06-phase-1-chaos-test-framework-metadata-expansion` |
| `20260414-09-phase-4-mixed-execution-capability-closure` | `Phase 4` | `completed` | `codex` | 补齐 mixed/interpreter capability closure | `20260414-06-phase-1-chaos-test-framework-metadata-expansion` |
| `20260414-10-phase-5-pipeline-reporting-and-docs-alignment` | `Phase 5` | `completed` | `codex` | 对齐 planner / registry / dashboard / reporting / docs | `20260414-07-phase-2-solution-core-pack-mechanical-split, 20260414-08-phase-3-hotupdate-host-pack-full-project-workflow, 20260414-09-phase-4-mixed-execution-capability-closure` |
| `20260414-11-phase-6-verification-and-archive` | `Phase 6` | `completed` | `codex` | 全量验证并归档 follow-up roadmap | `20260414-10-phase-5-pipeline-reporting-and-docs-alignment` |

## 7. 依赖

- Phase 0 是全部后续工作的前置冻结阶段。
- Phase 1 必须在任何 retained-subject 结构改造前完成，否则 metadata vocabulary 会继续漂移。
- Phase 2、3、4 在 Phase 1 后可以并行规划，但建议优先推进 Phase 2，因为大部分历史语法点都会先落到 `SolutionCorePack`。
- Phase 5 依赖三条 subject 主线都达到可消费元数据的状态。
- Phase 6 最后执行。

## 8. 冻结决策

- OQ2
  - support state 采用混合模型。
  - 工程实现按外部 `matrix/manifest` 为主执行。
  - attribute 只承载内在 metadata，不承载大规模支持状态矩阵。
- OQ3
  - `CoreLibReferenceSolution` 与相关 reference archetype 统一使用 repository-owned medium `foundation bundle`。
  - 不走 machine-local SDK resolver，不做整包 reference pack 镜像。
- OQ4
  - 历史语法点一律先补 proof。
  - benchmark 只覆盖高价值 runtime path，例如 dispatch、generic、allocation、interop、hot-update load/dispatch、mixed bridge、interpreter lowering 热点。
- OQ5
  - dashboard 采用双层视图。
  - 顶层按 capability family 汇总。
  - 二级 drill-down 按 archetype / solution / case 展开。
  - managed baseline 作为默认对比基线。
## 9. 风险

### 风险 1: `SolutionCorePack` 再次膨胀成无边界容器

- 缓解：
  - 强制区分 `Archetypes/`、`FeatureSlices/`、`Benchmarks/`
  - 新增条目先定 capability family，再写文件

### 风险 2: `Chaos.TestFramework` 过度抽象成新的 DSL

- 缓解：
  - 只增加必要的轻量枚举和字段
  - 保持 `UnitTest` 风格 authoring 体验

### 风险 3: 历史语法点迁移后 proof 与 benchmark 命名失真

- 缓解：
  - Phase 0 先冻结命名规则
  - Phase 5 再统一 dashboard / planner 展示

### 风险 4: full-project hot-update archetype 定义过重

- 缓解：
  - 先冻结最小 canonical graph
  - 先验证 correctness，再扩展 benchmark 与 productization 配置

## 10. 备选路径

### 备选路径 A: 只补 capability matrix，不重构 subject 内部结构

- 优点：
  - 短期文档更快。
- 缺点：
  - 文档与真实代码结构持续脱节。

### 备选路径 B: 继续拆出更多顶层 subject

- 优点：
  - 单项目体量更小。
- 缺点：
  - 破坏 retained-subject 冻结结果，统一入口和 dashboard 会重新复杂化。

### 当前判断

- 两条备选都不如当前主路径稳定。

## 11. 当前建议推进顺序

1. 立即派生并执行 `20260414-05-phase-0-capability-taxonomy-and-inventory-freeze`。
2. 在 Phase 0 内把已冻结的 OQ2/OQ3/OQ4/OQ5 写入 freeze 文档，作为后续实现边界。
3. Phase 0 完成后，再创建 Phase 1 child task，开始扩展 `Chaos.TestFramework` 与 metadata discovery。
4. 只有在 Phase 1 稳定后，才进入 retained subjects 的大规模机械迁移。
