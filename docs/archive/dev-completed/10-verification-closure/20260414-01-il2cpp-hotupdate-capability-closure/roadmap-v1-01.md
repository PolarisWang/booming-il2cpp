# IL2CPP 与热更全能力收口 Roadmap v1.01

Date: 2026-04-14 01:06:01 +08:00
Status: draft

## 1. 目标

在现有 retained subject 体系基础上，建立一套长期可维护的 IL2CPP + 热更能力闭环，使仓库能够稳定回答以下问题：

- 哪些 C# runtime 语义能力已经被 native 路径验证
- 哪些能力已经被 hot-update / mixed execution 路径验证
- 哪些完整 solution 形态已经被完整 ingest、analysis、codegen、runtime 验证
- 一个引用 dotnet 核心 DLL 的完整 solution 如何进入 canonical 测试与 native 翻译
- 一个全工程热更 solution 如何组织 host、shared contracts、patch modules 与 package metadata
- 用户如何基于 `Chaos.TestFramework` 编写 proof / benchmark，并理解支持边界与使用方式

最终落点：

- 顶层 `subjects/` 仍只保留 `SolutionCorePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack`
- `Chaos.TestFramework` 成为唯一公开的 subject C# 测试声明 API
- subject 内部调度面继续收紧，不用 string 协议继续扩大
- retained subjects 覆盖 capability matrix、solution archetype matrix、full-project hot-update workflow
- pipeline / discovery / reporting / docs 能围绕这套矩阵稳定工作

## 2. 范围边界

本 roadmap 负责：

- 定义能力矩阵与支持状态模型
- 扩展 `src/reference/Chaos.TestFramework`
- 扩展 retained subjects 的内部结构与 solution archetype
- 把所有历史语法点整理为单文件 proof/benchmark 条目，并按能力族尽量合并在同一个项目内，而不是扩成更多测试工程
- 新增 dotnet 核心 DLL 引用 solution archetype
- 新增 full-project hot-update solution archetype
- 补齐 mixed/interpreter 相关能力闭环
- 更新 discovery / registry / planner / reporting / docs 以消费新元数据
- 输出架构文档与使用文档
- 完成新的 proof / benchmark / solution validation 验证闭环

本 roadmap 不负责：

- 一次性打通所有移动端真机农场
- 实现新的 executable plan 类型
- 为每个历史语法点都新建独立测试工程
- 替代现有 release / distribution policy 的产品流程

## 3. 非目标

- 不新增更多顶层 subject
- 不重新引入重型 `validation/` / `scenario/` / proof family 配置体系
- 不把 `Chaos.TestFramework` 放进 `src/managed`
- 不用 string 协议继续扩大 subject 内部调度面
- 不把 benchmark 变成所有 feature 的强制伴生物

## 4. 阶段列表

### Phase 0: Capability Taxonomy And Archetype Freeze

- 冻结能力矩阵、solution archetype matrix、subject ownership 与支持状态 vocabulary
- 冻结 `SolutionCorePack` / `HotUpdateHostPack` / `MixedExecutionFeaturePack` 的目标内部结构
- 冻结 dotnet 核心 DLL 引用方案与 full-project hot-update archetype 的基本 contract

### Phase 1: `Chaos.TestFramework` Contract Expansion

- 扩展 `src/reference/Chaos.TestFramework`
- 补齐 capability、archetype、hot-update 能力标注所需的轻量枚举和 attribute 字段
- 让 discovery / compiled catalog 能消费这些声明信息

### Phase 2: `SolutionCorePack` Capability And Archetype Expansion

- 把 `SolutionCorePack` 升级为真正的 capability matrix 宿主
- 新增更完整的 feature slices
- 新增 `ReferenceAssemblySolution`、`CoreLibReferenceSolution`、`MixedReferenceClosureSolution`
- 让完整 solution 验证成为 canonical gate，而不是附带样例

### Phase 3: `HotUpdateHostPack` Full-Project Workflow Expansion

- 新增 full-project hot-update solution archetype
- 补齐 shared contracts、patch modules、metadata supplement、compatibility、rollback 相关验证
- 形成可公开复用的热更工程组织与使用方式

### Phase 4: `MixedExecutionFeaturePack` Capability Closure

- 补齐 mixed/interpreter 语义能力族
- 强化 mixed native/interpreter/managed 三向对照
- 确保 benchmark / proof 都能表达 mixed execution 的支持状态

### Phase 5: Pipeline, Discovery, Reporting And Gate Alignment

- 让 registry / planner / run command / report / benchmark dashboard 能看到 capability 与 archetype 维度
- 增加 solution closure、corelib reference、hot-update compatibility 的 gate
- 统一 unsupported / partial support 的报告方式

### Phase 6: Architecture And Usage Documentation

- 输出 capability matrix、solution archetype、reference/corelib policy、full-project hot-update workflow 文档
- 更新 release / compatibility / benchmark 相关文档引用

### Phase 7: Verification And Archive

- 跑全量 retained subject proof / benchmark / archetype validation
- 跑核心 tests 与 registry refresh
- 汇总证据、关闭 open questions、归档 roadmap

## 5. 每阶段完成定义

### Phase 0

- `goal`
  - 冻结本轮能力建设的 vocabulary、结构边界和 archetype 目标，避免后续实现阶段边做边改。
- `structure_adjustments`
  - 新增 capability matrix freeze 文档
  - 新增 subject target structure 草图
- `checklist`
  - [ ] 列出 runtime 语义能力族及其 `native / hot-update / mixed / unsupported` 四态
  - [ ] 将 capability taxonomy 冻结到二级能力项，而不是只保留大类
  - [ ] 列出 canonical solution archetype 清单
  - [ ] 明确每个 archetype 的 subject owner
  - [ ] 冻结 `SolutionCorePack` 目标目录结构
  - [ ] 冻结 `HotUpdateHostPack` 目标目录结构
  - [ ] 冻结 `MixedExecutionFeaturePack` 目标目录结构
  - [ ] 冻结 `CoreLibReferenceSolution` 采用仓库内 `reference bundle`
  - [ ] 冻结 `FullProjectHotUpdateSolution` 的最小工程边界
- `deliverables`
  - capability taxonomy freeze
  - solution archetype freeze
  - target subject structure freeze
  - `CoreLibReferenceSolution = repository-owned reference bundle`
  - capability taxonomy = level-2 items
- `exit_criteria`
  - 每个新增测试目标都能回答“归属于哪个 subject、哪个 archetype、哪个能力族”
- `open_questions`
  - 无。`Q0` 与 `Q1` 已冻结：
    - `Q0`: `CoreLibReferenceSolution` 使用仓库内 `reference bundle`
    - `Q1`: capability taxonomy 先拆到二级能力项

### Phase 1

- `goal`
  - 让 `Chaos.TestFramework` 足以承载 capability matrix 与 archetype metadata，不再依赖外置机制。
- `structure_adjustments`
  - 修改 `src/reference/Chaos.TestFramework/Chaos.TestFramework.cs`
  - 修改 metadata discovery / compiled catalog 消费路径
- `checklist`
  - [ ] 新增 `ChaosSolutionArchetype`
  - [ ] 新增 `ChaosHotUpdateCapability`
  - [ ] 扩展 `ChaosRuntimeFeature`
  - [ ] 审核是否需要 `ChaosCapabilitySupport` 一类公开状态枚举
  - [ ] 为所有公开枚举补齐 XML 注释
  - [ ] 扩展 `ChaosUnitTestAttribute`
  - [ ] 扩展 `ChaosBenchmarkAttribute`
  - [ ] 更新 compiled catalog metadata shape
  - [ ] 更新 discovery 相关单元测试
- `deliverables`
  - 扩展后的 `Chaos.TestFramework`
  - 新的 metadata/discovery contract
- `exit_criteria`
  - subject 内部一个声明方法即可同时表达 category、runtime requirement、archetype/capability 元数据
- `open_questions`
  - 无。`Q2` 已冻结：
    - attribute 只声明内在属性：`Category`、`Requires`、`Archetype`、`HotUpdateCapability`
    - support-state 继续保留在外层 matrix/manifest

### Phase 2

- `goal`
  - 让 `SolutionCorePack` 成为 IL2CPP native 主线的 canonical capability host。
- `structure_adjustments`
  - 目标结构：
    - `subjects/SolutionCorePack/source/Archetypes/*`
    - `subjects/SolutionCorePack/source/FeatureSlices/*`
    - `subjects/SolutionCorePack/source/Benchmarks/*`
  - 现有 `Slices/MainlineFeaturePack`、`Slices/PerformanceFeaturePack` 需要收敛或重命名
- `checklist`
  - [ ] 定义 `FeatureSlices` 目录与命名规则
  - [ ] 将历史语法点整理为单文件 proof/benchmark 条目，并按能力族合并到共享项目
  - [ ] 把现有 mainline proof 按能力族重组
  - [ ] 评估并补齐 async/iterator 能力
  - [ ] 评估并补齐 reflection/metadata 能力
  - [ ] 评估并补齐 threading/synchronization 能力
  - [ ] 评估并补齐 unsafe/span/function-pointer 能力
  - [ ] 评估并补齐 linker/AOT closure 能力
  - [ ] 新增 `ReferenceAssemblySolution`
  - [ ] 新增 `CoreLibReferenceSolution`
  - [ ] 新增 `MixedReferenceClosureSolution`
  - [ ] 补齐 archetype 对应 proof
  - [ ] 补齐 native benchmark 选择与映射
  - [ ] 更新 `subject.manifest.json`
- `deliverables`
  - capability-oriented `SolutionCorePack`
  - 完整 solution archetype matrix 基础实现
- `exit_criteria`
  - `SolutionCorePack` 能同时回答“语言能力验证”和“完整 solution 形态验证”
- `open_questions`
  - 无。`Q3` 与 `Q4` 已冻结：
    - `Q3`: 先采用代表性中等闭包 `foundation bundle`，而不是极小样例或整包镜像
    - `Q4`: 现有 archetype 一次性重命名到 canonical 命名体系，不保留 compatibility alias

### Phase 3

- `goal`
  - 让 `HotUpdateHostPack` 提供完整、可复用的全工程热更标准用法。
- `structure_adjustments`
  - 目标结构：
    - `subjects/HotUpdateHostPack/source/Host/`
    - `subjects/HotUpdateHostPack/source/SharedContracts/`
    - `subjects/HotUpdateHostPack/source/PatchModules/`
    - `subjects/HotUpdateHostPack/source/Archetypes/FullProjectHotUpdateSolution/`
- `checklist`
  - [ ] 设计 full-project hot-update archetype 的 solution graph
  - [ ] 明确 host / shared contracts / patch modules / package metadata 的工程边界
  - [ ] 补齐 shared contract binding proof
  - [ ] 补齐 host-to-patch callback proof
  - [ ] 补齐 patch-to-host callback proof
  - [ ] 补齐 generic / delegate / exception 跨边界 proof
  - [ ] 补齐 metadata supplement / package integrity proof
  - [ ] 补齐 compatibility / rollback proof
  - [ ] 补齐 hot-update benchmark 与 archetype 的映射
  - [ ] 更新 `subject.manifest.json`
- `deliverables`
  - `FullProjectHotUpdateSolution`
  - 热更工程组织的 canonical 样例
- `exit_criteria`
  - 用户可以只看 retained subject 内的 canonical solution，就理解全工程热更如何组织
- `open_questions`
  - 无。`Q5` 已冻结：
    - patch package 的最小公开格式 contract 需要单独沉淀为长期 architecture 文档

### Phase 4

- `goal`
  - 把 mixed/interpreter 能力补齐为独立而清晰的语义矩阵。
- `structure_adjustments`
  - 目标结构：
    - `subjects/MixedExecutionFeaturePack/source/ManagedBridge/`
    - `subjects/MixedExecutionFeaturePack/source/Interpreter/`
    - `subjects/MixedExecutionFeaturePack/source/Lowering/`
    - `subjects/MixedExecutionFeaturePack/source/Archetypes/MixedBridgeSolution/`
- `checklist`
  - [ ] 梳理 mixed execution 能力族
  - [ ] 补齐 interpreter lowering proof
  - [ ] 补齐 generic / delegate / exception mixed proof
  - [ ] 补齐 managed/native/interpreter 三向 benchmark
  - [ ] 明确 mixed execution 的 unsupported 边界
  - [ ] 更新 `subject.manifest.json`
- `deliverables`
  - 完整 mixed execution capability closure
- `exit_criteria`
  - mixed execution 的支持边界可以被独立表达，而不再借用 hot-update 或 native 术语
- `open_questions`
  - 无。`Q6` 已冻结：
    - mixed execution 暂时保留单 archetype + 多 capability slice，只有出现明显不同的 solution graph 再拆分

### Phase 5

- `goal`
  - 让外层系统能消费 capability/archetype 维度，并把 unsupported/partial support 透明表达出来。
- `structure_adjustments`
  - 修改 registry / planner / reporting / benchmark dashboard / command manifest
- `checklist`
  - [ ] 扩展 compiled catalog 输出 capability/archetype metadata
  - [ ] 扩展 registry/index 消费新 metadata
  - [ ] 扩展 planner 对 archetype / capability 选择的理解
  - [ ] 更新统一入口命令输出与 rerun 信息
  - [ ] 更新 benchmark dashboard 的 subject/case/archetype 呈现
  - [ ] 增加 unsupported/partial support 报告模式
  - [ ] 增加 corelib reference / hot-update compatibility gate
  - [ ] 回归 tests/tooling / tests/unit / tests/integration
- `deliverables`
  - capability-aware pipeline 与 reporting
- `exit_criteria`
  - 外层工具链可以按 capability / archetype 看系统支持面，而不只看到 subject 名称
- `open_questions`
  - 无。`Q7` 已冻结：
    - 统一采用 `supported / partial / unsupported / not-applicable`
    - `partial` 与 `unsupported` 必须带 reason code

### Phase 6

- `goal`
  - 把能力设计转成长期可维护文档。
- `structure_adjustments`
  - 新增：
    - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/il2cpp-hotupdate-capability-matrix.md`
    - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/solution-archetype-matrix.md`
    - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/full-project-hotupdate-workflow.md`
    - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/reference-assembly-and-corelib-policy.md`
    - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/unsupported-feature-policy.md`
  - 更新：
    - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/INDEX.md`
    - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/release-checklist.md`
    - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/version-compatibility-matrix.md`
- `checklist`
  - [ ] 写 capability matrix 文档
  - [ ] 写 solution archetype 文档
  - [ ] 写 full-project hot-update workflow 文档
  - [ ] 写 reference/corelib policy 文档
  - [ ] 写 unsupported feature policy 文档
  - [ ] 更新现有 architecture 索引与交叉引用
- `deliverables`
  - 架构与使用文档集
- `exit_criteria`
  - 仅靠文档即可回答“怎么写、怎么跑、支持到哪、边界是什么”
- `open_questions`
  - 无。`Q8` 已冻结：
    - 稳定 contract 文档进入 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考`
    - 迁移顺序、阶段 checklist、一次性重命名明细保留在 dev task archive

### Phase 7

- `goal`
  - 用实际证据关闭本路线图。
- `structure_adjustments`
  - 更新任务状态文档与归档记录
- `checklist`
  - [ ] `run test registry refresh`
  - [ ] 跑 `SolutionCorePack` proof
  - [ ] 跑 `SolutionCorePack` benchmark
  - [ ] 跑 `SolutionCorePack` archetype matrices
  - [ ] 跑 `HotUpdateHostPack` proof
  - [ ] 跑 `HotUpdateHostPack` benchmark
  - [ ] 跑 `HotUpdateHostPack` full-project hot-update archetype
  - [ ] 跑 `MixedExecutionFeaturePack` proof
  - [ ] 跑 `MixedExecutionFeaturePack` benchmark
  - [ ] 跑核心 unit / integration suites
  - [ ] 汇总证据并关闭 open questions
  - [ ] 更新状态并归档
- `deliverables`
  - 最终验证证据
  - 归档后的 roadmap
- `exit_criteria`
  - retained subjects、capability matrix、solution archetypes、文档与外层工具链都有最新证据闭环
- `open_questions`
  - 无。进入本阶段前，所有架构性 open questions 必须已收敛。

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `phase-0-capability-taxonomy-and-archetype-freeze` | `Phase 0` | `completed` | `codex` | 冻结能力矩阵、solution archetype、subject 目标结构和关键 vocabulary | `-` |
| `phase-1-chaos-test-framework-contract-expansion` | `Phase 1` | `active` | `codex` | 扩展 `Chaos.TestFramework` 和 metadata discovery contract | `phase-0-capability-taxonomy-and-archetype-freeze` |
| `phase-2-solution-core-pack-expansion` | `Phase 2` | `planned` | `codex` | 扩展 `SolutionCorePack` capability slices 与 solution archetypes | `phase-1-chaos-test-framework-contract-expansion` |
| `phase-3-hotupdate-host-pack-full-project-workflow` | `Phase 3` | `planned` | `codex` | 扩展 full-project hot-update canonical solution 与验证链路 | `phase-1-chaos-test-framework-contract-expansion` |
| `phase-4-mixed-execution-capability-closure` | `Phase 4` | `planned` | `codex` | 补齐 mixed/interpreter capability matrix | `phase-1-chaos-test-framework-contract-expansion` |
| `phase-5-pipeline-discovery-reporting-alignment` | `Phase 5` | `planned` | `codex` | 扩展外层 registry/planner/reporting/gates 以消费 capability/archetype metadata | `phase-2-solution-core-pack-expansion, phase-3-hotupdate-host-pack-full-project-workflow, phase-4-mixed-execution-capability-closure` |
| `phase-6-architecture-and-usage-documentation` | `Phase 6` | `planned` | `codex` | 补齐长期 architecture 与 usage 文档 | `phase-2-solution-core-pack-expansion, phase-3-hotupdate-host-pack-full-project-workflow, phase-5-pipeline-discovery-reporting-alignment` |
| `phase-7-verification-and-archive` | `Phase 7` | `planned` | `codex` | 以真实 proof/benchmark/archetype 证据关闭路线图 | `phase-5-pipeline-discovery-reporting-alignment, phase-6-architecture-and-usage-documentation` |

## 7. 依赖

- `Phase 0` 是整个路线图的冻结前提。
- `Phase 1` 必须先完成，否则后续 subject 扩展会继续缺少统一 metadata vocabulary。
- `Phase 2`、`Phase 3`、`Phase 4` 在 `Phase 1` 之后可以并行准备，但建议优先推进 `SolutionCorePack`，因为它承载最多 archetype 与 capability。
- `Phase 5` 依赖 retained subjects 的 capability/archetype 元数据已经稳定。
- `Phase 6` 可以和 `Phase 5` 局部并行，但正式文档定稿应晚于元数据与结构定稿。
- `Phase 7` 最后执行。

## 8. 风险

### 风险 1: `SolutionCorePack` 再次变成大而混乱的容器

影响：

- capability 验证与 archetype 验证混在一起，目录失去可读性。

缓解：

- 强制区分 `Archetypes/`、`FeatureSlices/`、`Benchmarks/`
- 每个能力点先绑定归属，再实施

### 风险 2: `Chaos.TestFramework` 扩展过度，变成新的重型 DSL

影响：

- subject authoring 成本升高，声明式简洁性消失。

缓解：

- 只新增确实需要的轻量枚举与字段
- 保持 `ChaosUnitTest` / `ChaosBenchmark` / `Assert` 的简洁使用方式

### 风险 3: repository-owned reference bundle 的边界如果定义不当，会过轻或过重

影响：

- 过轻会失去代表性，过重会提升仓库体积和维护成本。

缓解：

- 在 `Phase 0` 就冻结为 repository-owned `foundation bundle`
- 在 `Phase 2` 明确最小代表性 DLL 集，而不是继续扩到整包镜像

### 风险 4: full-project hot-update archetype 过于贴近当前实现细节，缺少长期稳定性

影响：

- 文档与样例很快失真。

缓解：

- 先定义最小稳定 contract，再选择具体实现
- 把可变实现细节和长期公开边界分开

### 风险 5: pipeline/reporting 不跟进，导致 capability metadata 只停留在 subject 内部

影响：

- 用户仍然只能看到 subject 名称，看不到真实支持面。

缓解：

- 把 `Phase 5` 视为主线收口，不允许只做 subject 侧扩展

## 9. 备选路径

已否决：

- 新增更多顶层 subject 来承载不同能力域
- 恢复重型外置 scenario/validation 定义体系
- 直接把绝对 SDK 路径写进 corelib reference 测试工程
- 先写文档、后补 subject 和 metadata 结构

保留的唯一主路径：

- 保持 3 个 retained subject
- 用 `Chaos.TestFramework` 扩展 metadata contract
- 让 capability matrix + solution archetype 成为 retained subjects 的主组织方式

## 10. 当前建议推进顺序

1. 先做 `Phase 0: Capability Taxonomy And Archetype Freeze`，把 vocabulary 和结构边界定死。
2. 紧接着做 `Phase 1: Chaos.TestFramework Contract Expansion`，避免后续 subject 扩展继续缺少统一 metadata。
3. 先推进 `Phase 2: SolutionCorePack Capability And Archetype Expansion`，因为它承载最多 language/native/solution archetype 工作。
4. 再推进 `Phase 3: HotUpdateHostPack Full-Project Workflow Expansion`，建立全工程热更 canonical 用法。
5. 然后做 `Phase 4: MixedExecutionFeaturePack Capability Closure`。
6. 结构稳定后统一推进 `Phase 5` 和 `Phase 6`。
7. 最后用 `Phase 7` 做真实验证、证据汇总与归档。
