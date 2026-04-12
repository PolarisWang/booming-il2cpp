# 测试体系 Solution-First Subject 重设计 Roadmap v1.02

Date: 2026-04-12
Status: active-roadmap

## 1. 目标

把测试体系从“proof/benchmark 视角切顶层 subject”重规划为“solution-first subject 视角切 canonical source”，并给出可执行、可逐阶段验收的迁移路线。

本版 roadmap 的核心目标是：

- 让顶层 `subject` 回到 canonical solution source bundle 的语义。
- 让 `scenario` 与 `workload` 统一挂在 solution subject 下。
- 让 `tests/` 先回到 generic mechanism 层，再收敛现有 subject。
- 让 `golden`、proof-like、benchmark-like、hot-update、mobile 这些历史对象，都按 solution-first 模型重新归类。

本 roadmap 不直接进入单份稳定 `plan` 的原因是：

- 此次变化推翻了 v1.01 的核心对象模型，必须先重做迁移边界。
- 现有 `subjects/` 里既有按验证类型切的 subject，也有按 solution 雏形切的 subject。
- 如果不先冻结 solution-first naming / ownership / migration map，后续 child plan 会在根语义上返工。

## 2. 范围边界

本 roadmap 负责：

- 顶层 `subject` 的 solution-first 语义冻结
- `tests/` 的 generic taxonomy 收口
- `golden`、proof-like、benchmark-like、hot-update、mobile subject 的重新归类
- `scenario/workload` 挂载到 solution subject 的规则
- registry / selector / artifact / CLI / TUI 对新模型的切换顺序
- capability / availability / pipeline 挂到 solution subject 子对象上的落地路径

本 roadmap 不负责：

- 直接写代码 patch
- 一次性给出所有 subject 的最终命名
- 立即完成所有真机自动化
- 立即删除全部旧路径

## 3. 非目标

当前不在本 roadmap 中一次性解决：

- 每个历史 subject 的逐文件迁移脚本
- 最终对外发布命令的人机体验细节
- 全量设备农场调度系统
- 所有 dashboard 历史数据的一次性迁移方案实现

## 4. 阶段列表

### Phase 0：Solution-First Model Freeze And Migration Map

- 冻结顶层 `subject = canonical solution source bundle`
- 冻结 subject 命名规则、source 属性模型与旧 subject 收敛映射
- 明确 `golden`、proof-like、benchmark-like、hot-update、mobile 各类历史 subject 的目标归属

### Phase 1：Generic Test Taxonomy And De-Phase Cleanup

- 先整理 `tests/`
- 清理 `phase* / stage*` 命名
- 把 subject-specific canonical source 从 `tests/` 中抽离
- 收口 integration/tooling 的重复职责

### Phase 2：Solution Subject Schema And Root Cutover

- 为 solution-first subject 冻结正式目录与 manifest schema
- 引入 `sourceModel / dependencyModel / executablePlan`
- 引入 subject-local `scenarios/` 与 `workloads/`

### Phase 3：Solution Archetype Subjects Cutover

- 优先把 `golden` 升格成第一批 solution archetype subjects
- 让 `SolutionSimpleLib / SolutionMultiProject / SolutionPackageReference` 成为新的 canonical archetype
- 把工程级验证与 convert workload 先落在这批 archetype 上

### Phase 4：Functional / Mainline Solution Consolidation

- 把现有 proof-like functional subject 重新按 canonical solution source 收敛
- 让 `MainlineFeaturePack`、`HelloWorld*`、engine/interop/mobile proof 等归并到合理的 solution subjects
- proof 不再决定顶层根，只决定 scenario

### Phase 5：Benchmark / Hot-Update Solution Consolidation

- 把 `Bench*`、热更、mixed execution 等历史 benchmark/profiler subject 重新按 solution source 收敛
- workload 挂回对应 solution subject
- 把暂停中的 AOT benchmark backend 成果并入新模型

### Phase 6：Registry / Selector / Artifact / Public Cutover

- 让 suite/subject discovery、selector、artifact 与 public surface 切到 solution-first model
- 保留旧 subject id 的兼容 alias
- 让结果归属能稳定回溯到 `subject/scenario/workload`

### Phase 7：Capability / Availability / Pipeline Rollout

- 把 capability / availability / pipeline 正式挂到 solution subject 子对象上
- 定义 `dev-smoke`、`merge-gate`、`nightly-cross-platform`、`release-gate`
- 把 rollback、device evidence、弱环境、兼容矩阵等能力纳入正式 pipeline

### Phase 8：Compatibility Closure And Legacy Cleanup

- 清理旧的 proof/benchmark 顶层 subject 语义
- 退役旧目录、旧命名、旧别名
- 让 solution-first model 成为唯一 canonical 结构

## 5. 每阶段完成定义

### Phase 0

- `goal`:
  - 在任何物理迁移前，冻结 solution-first subject 模型、命名规则与旧 subject 的目标归宿。
- `structure_changes`:
  - 暂不做生产目录搬迁。
  - 在任务文档中新增：
    - `solution-subject-model-v1-01.md`
    - `legacy-subject-to-solution-map-v1-01.md`
    - `test-taxonomy-map-v1-01.md`
    - `compatibility-policy-v1-01.md`
- `checklist`:
  - 冻结顶层 `subject` 的 canonical 定义。
  - 为 subject manifest 补齐最小属性草案：
    - `sourceModel`
    - `dependencyModel`
    - `executablePlan`
  - 盘点所有现有顶层 subject，并分类为：
    - 已接近 canonical solution subject
    - 需并入其他 solution subject 的 proof-like root
    - 需并入其他 solution subject 的 benchmark-like root
    - archetype subject
    - legacy-only compatibility shell
  - 冻结 `golden` 的重命名与收敛方向：
    - `GoldenSimpleLib` -> `SolutionSimpleLib`
    - `GoldenMultiProject` -> `SolutionMultiProject`
    - `GoldenWithPackage` -> `SolutionPackageReference`
  - 冻结 `convert-golden-multi-project` 的去向：
    - `SolutionMultiProject/workloads/convert`
  - 明确 `HelloWorld`、`HelloWorldObject`、`MainlineFeaturePack`、`Bench*`、`HotUpdate*` 的目标归宿。
  - 冻结旧 subject id alias 策略。
- `exit_criteria`:
  - 所有现有 subject 都有明确目标归宿。
  - 顶层 subject 的 solution-first 含义不再摇摆。
  - 用户确认迁移映射与命名规则基线。
- `deliverables`:
  - solution subject 模型文档
  - 旧 subject -> 新 solution subject 映射表
  - 兼容策略文档
- `dependencies`: 无
- `open_questions`:
  - canonical 命名是否统一要求 `Solution*` 前缀，还是允许保留 `MainlineFeaturePack` 这类语义名。

### Phase 1

- `goal`:
  - 先把 `tests/` 收口为 generic mechanism 层，切断历史 phase 命名和具体 subject 真源耦合。
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
  - `tests/perf/` 只保留 generic perf 机制，不再保留 canonical solution workload。
  - `tests/proof/` 退役为历史兼容目录或完全移空。
- `checklist`:
  - 改名并迁移所有 `test_phase*` / `test_stage*` 文件。
  - 对 integration/tooling 重复文件逐一收口 ownership。
  - 把 generic tests 中的具体 subject 硬编码替换成 sample manifest 驱动。
  - 为 solution-first subject 模型准备 generic sample fixture。
  - 把 `tests/perf/convert-golden-multi-project` 标记为待迁出 canonical workload。
  - 把 `tests/` 中 subject-specific source/fixture/package probe 引用清理出来。
- `exit_criteria`:
  - `tests/unit/run/` 下不再存在 phase/stage 命名。
  - `tests/` 不再作为具体 solution subject 的 canonical source 根。
  - `tests/perf` 已明确只承载 generic perf 机制。
- `deliverables`:
  - 重组后的 `tests/` 目录
  - 新命名迁移表
  - generic sample fixtures
- `dependencies`:
  - Phase 0
- `open_questions`:
  - `tests/support.py` 是继续保留单文件，还是拆成 `tests/support/` package。

### Phase 2

- `goal`:
  - 冻结 solution-first subject 的正式 root schema 与 child object schema。
- `structure_changes`:
  - canonical root 统一为：
    - `subjects/<subject-id>/subject.manifest.json`
    - `subjects/<subject-id>/source/`
    - `subjects/<subject-id>/validation/`
    - `subjects/<subject-id>/fixtures/`
    - `subjects/<subject-id>/scenarios/`
    - `subjects/<subject-id>/workloads/`
  - 引入 child manifests：
    - `scenario.manifest.json`
    - `workload.manifest.json`
  - 引入 manifest 字段：
    - `sourceModel`
    - `dependencyModel`
    - `executablePlan`
    - `capabilities`
    - `availability`
    - `validationProfiles`
- `checklist`:
  - 冻结 subject root 目录结构。
  - 定义 suite loader/selector 读取 subject + child manifest 的规则。
  - 定义 source/fixture/expected/baseline 的所有权边界。
  - 定义 `scenario` 和 `workload` 的最小 schema。
  - 定义旧 subject id 到新 canonical subject id 的 redirect 规则。
  - 明确从本阶段结束起新增测试点默认应新增 `scenario/workload`，而不是新顶层 subject。
- `exit_criteria`:
  - solution-first subject root 可被 loader 消费。
  - child manifests 拥有稳定 schema。
  - 目录结构与命名基线冻结。
- `deliverables`:
  - subject schema
  - scenario/workload schema
  - alias/redirect 规则
- `dependencies`:
  - Phase 0
  - 建议在 Phase 1 taxonomy 收口后启动大规模目录迁移
- `open_questions`:
  - `validation/` 下是否应预留 `perf-harness`、`platform-hosts`、`device-runners` 的固定子目录规范。

### Phase 3

- `goal`:
  - 把 `golden` 升格为第一批 canonical solution archetype subjects，并让工程级验证先在这些 archetype 上闭环。
- `structure_changes`:
  - 新增或重命名：
    - `subjects/SolutionSimpleLib/`
    - `subjects/SolutionMultiProject/`
    - `subjects/SolutionPackageReference/`
  - archetype subjects 下落：
    - `scenarios/project-graph/`
    - `scenarios/runtime-output/`
    - `scenarios/native-buildability/`
    - `workloads/convert/`
  - 旧 `subjects/golden/*` 退化为 compatibility shell。
- `checklist`:
  - 把 `GoldenSimpleLib` 迁为 `SolutionSimpleLib`。
  - 把 `GoldenMultiProject` 迁为 `SolutionMultiProject`。
  - 把 `GoldenWithPackage` 迁为 `SolutionPackageReference`。
  - 把工程级验证从旧 golden subject 名称切到新 canonical 名称。
  - 把 `convert-golden-multi-project` 迁为 `SolutionMultiProject/workloads/convert`。
  - 让 archetype subjects 能跑最小 correctness + workload 闭环。
- `exit_criteria`:
  - `golden` 不再是 canonical bucket。
  - 三个 archetype subjects 成为新的工程级验证真源。
  - convert workload 已进入 subject-local `workloads/`.
- `deliverables`:
  - 三个 archetype subjects
  - convert workload 切换结果
  - old golden aliases
- `dependencies`:
  - Phase 2
- `open_questions`:
  - archetype subjects 是否全部强制采用 `Solution*` 前缀，还是允许保留历史别名供长期展示。

### Phase 4

- `goal`:
  - 把现有 proof-like / functional 主线 subject 按 canonical solution source 收敛。
- `structure_changes`:
  - 对 `MainlineFeaturePack`、`HelloWorld`、`HelloWorldObject`、engine/interop/mobile proof 等进行：
    - 保留为独立 solution subject
    - 或并入更大的 canonical solution subject
  - proof-like 顶层 root 退化为 scenario alias 或 compatibility shell。
- `checklist`:
  - 盘点哪些历史 proof-like subject 实际共享同一 solution source。
  - 把 dispatch / generic-layout / delegate / exception / reflection / interop 等收敛为 `scenario`.
  - 确定 `HelloWorld*` 在新模型下是 archetype solution subject，还是并入更大 solution subject。
  - 确定 engine binding / mobile host / native interop 的 canonical solution root。
  - 更新 unit/integration fixtures 到新 subject/scenario ids。
- `exit_criteria`:
  - proof-like 顶层 subject 不再表达 source 根。
  - functional correctness 主线已经按 solution source 收口。
  - `scenario` 成为 correctness 扩展的默认单位。
- `deliverables`:
  - functional solution subjects
  - proof root -> scenario alias 表
  - source ownership 收敛结果
- `dependencies`:
  - Phase 2
  - 建议在 Phase 3 archetype subjects稳定后推进
- `open_questions`:
  - `HelloWorld` 与 `HelloWorldObject` 是否保留为独立 archetype solution subjects，还是只作为 onboarding solution fixtures。

### Phase 5

- `goal`:
  - 把 benchmark / hot-update / mixed execution 等历史对象，重新并入 solution-first subject 模型。
- `structure_changes`:
  - 将 `Bench*`、`HotUpdate*`、mixed execution 相关历史 root：
    - 保留为独立 canonical solution subject
    - 或并入共享 solution subject 下的 `workloads/`
  - canonical workload 路径统一为：
    - `subjects/<subject-id>/workloads/<workload-id>/`
- `checklist`:
  - 盘点哪些 `Bench*` 实际只是 workload，不是独立 source root。
  - 把 `workloadEntry`、`harnessIterations`、warmup/measurement policy 迁入 subject-local workload manifests。
  - 把 managed / interpreter / native / mobile perf 语义统一到 `subject + workload + matrix`.
  - 把暂停中的 `20260412-05-phase-b-generic-native-aot-benchmark-backend` 成果并入新模型。
  - 处理 hot-update roundtrip / load / dispatch 等是否共用一个 canonical solution subject。
- `exit_criteria`:
  - `Bench*` 不再天然等于顶层 source root。
  - workload 成为 benchmark 扩展的默认单位。
  - AOT benchmark backend 已服从新 subject model。
- `deliverables`:
  - benchmark/hot-update solution subjects
  - workload cutover 结果
  - perf policy/budget policy 统一规则
- `dependencies`:
  - Phase 2
  - 建议在 Phase 4 functional 主线收口后推进
- `open_questions`:
  - 哪些 `Bench*` 值得保留为独立 canonical solution subject，哪些应合并为 shared benchmark solution subject。

### Phase 6

- `goal`:
  - 让 registry、selector、artifact、CLI/TUI 切到 solution-first model。
- `structure_changes`:
  - registry 发现对象扩展为：
    - subjects
    - scenarios
    - workloads
    - pipelines
  - artifact canonical 路径切到：
    - `artifacts/subjects/<subject-id>/runs/<run-id>/scenarios/<scenario-id>/...`
    - `artifacts/subjects/<subject-id>/runs/<run-id>/workloads/<workload-id>/...`
  - `tests/registry/` 与 `tests/support.py` 同步切到新模型。
- `checklist`:
  - 实现 subject/scenario/workload registry scan。
  - 实现 canonical selector 语义与旧 id alias。
  - 更新 CLI/TUI/JSON/reporting 展示新归属模型。
  - 更新 artifact ledger、summary、report 归属字段。
  - 更新 integration/tooling 测试覆盖新路径。
- `exit_criteria`:
  - 新模型成为 canonical 发现与执行入口。
  - artifact 能稳定回溯到 `subject/scenario/workload`.
  - 旧入口仅剩 compatibility adapter 职责。
- `deliverables`:
  - new registry model
  - selector/cutover 方案
  - artifact path 切换结果
- `dependencies`:
  - Phase 4
  - Phase 5
- `open_questions`:
  - canonical CLI 是否采用 `run test subject --id <subject> --scenario <id>` 形式，还是引入 `run test scenario/workload` 顶层快捷入口。

### Phase 7

- `goal`:
  - 把 capability / availability / pipeline 正式挂到 solution subject 的子对象上。
- `structure_changes`:
  - 为所有 `scenario/workload` 补齐：
    - `capabilities`
    - `availability`
    - `pipelines`
  - `tests/registry/pipelines/` 引入或重组：
    - `dev-smoke`
    - `merge-gate`
    - `nightly-cross-platform`
    - `release-gate`
  - solution subject 内补齐能力资产：
    - `platform-hosts/`
    - `device-runners/`
    - `rollback-fixtures/`
    - `compatibility-matrices/`
- `checklist`:
  - 给每个 scenario/workload 标注 capability。
  - 给每个平台标注 availability。
  - 把 version compatibility、rollback、state migration、device evidence、weak environment、reproducible build、engine/native boundary、release budget 纳入正式 pipeline。
  - 定义 nightly/release 的最小必过集。
  - 让 pipeline 能按 availability=`ready` 过滤默认执行项。
- `exit_criteria`:
  - capability/availability 成为正式可查询字段。
  - dev/merge/nightly/release 四级 pipeline 具有稳定对象定义。
  - 多平台 IL2CPP + 热更的额外能力进入正式测试模型。
- `deliverables`:
  - capability 覆盖表
  - availability 覆盖表
  - pipeline 定义
- `dependencies`:
  - Phase 6
- `open_questions`:
  - 首版 nightly 的真机证据采集是全自动还是半自动回传。

### Phase 8

- `goal`:
  - 关闭兼容层，形成干净的 solution-first canonical 布局。
- `structure_changes`:
  - 退役或归档：
    - 旧 proof-like 顶层 subject
    - 旧 `Bench*` 顶层 subject
    - 旧 `subjects/golden/*`
    - 旧 `tests/proof/`
    - 残留 phase/stage 命名文件
    - 不再需要的 compatibility wrappers
  - 清理结构性噪音目录规范：
    - `__pycache__/`
    - `bin/`
    - `obj/`
  - 更新 wiki / onboarding / docs/dev 正式入口。
- `checklist`:
  - 删除或归档所有非 canonical subject roots。
  - 收口旧 selector alias，并标记 deprecation。
  - 清理最后的 phase/stage 命名。
  - 校验 `tests/` 与 `subjects/` 边界不再回混。
  - 补齐 ignore / cleanup 规则。
- `exit_criteria`:
  - solution-first model 是唯一 canonical 结构。
  - 旧 validation-type 顶层 subject 全部退出主路径。
  - `tests/` 内不再保留具体 solution subject 真源。
  - 文档、命令、artifact、registry 全部指向新模型。
- `deliverables`:
  - legacy cleanup 清单
  - compatibility closure 结果
  - wiki / onboarding 更新
- `dependencies`:
  - Phase 7
- `open_questions`:
  - 旧 subject id 的 deprecation 周期按 milestone 还是按 version 管理。

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `phase-0-solution-first-model-freeze-and-migration-map` | `Phase 0` | `ready` | `codex` | 冻结顶层 subject 的 solution-first 语义、命名和旧 subject 收敛映射 | `-` |
| `phase-1-generic-test-taxonomy-and-dephase-cleanup` | `Phase 1` | `planned` | `codex` | 先把 `tests/` 收口为 generic mechanism 层 | `phase-0-solution-first-model-freeze-and-migration-map` |
| `phase-2-solution-subject-schema-and-root-cutover` | `Phase 2` | `planned` | `codex` | 落 solution subject root schema 与 child manifests | `phase-0-solution-first-model-freeze-and-migration-map, phase-1-generic-test-taxonomy-and-dephase-cleanup` |
| `phase-3-solution-archetype-subjects-cutover` | `Phase 3` | `planned` | `codex` | 把 `golden` 升格为第一批 canonical solution archetype subjects | `phase-2-solution-subject-schema-and-root-cutover` |
| `phase-4-functional-mainline-solution-consolidation` | `Phase 4` | `planned` | `codex` | 把 proof-like / functional 主线 subject 收敛为 solution-first subjects + scenarios | `phase-2-solution-subject-schema-and-root-cutover, phase-3-solution-archetype-subjects-cutover` |
| `phase-5-benchmark-and-hotupdate-solution-consolidation` | `Phase 5` | `planned` | `codex` | 把 benchmark / hot-update 历史 subject 收敛为 solution-first subjects + workloads | `phase-2-solution-subject-schema-and-root-cutover, phase-3-solution-archetype-subjects-cutover` |
| `phase-6-registry-selector-artifact-and-public-cutover` | `Phase 6` | `planned` | `codex` | 切 registry、selector、artifact、CLI/TUI 到 solution-first model | `phase-4-functional-mainline-solution-consolidation, phase-5-benchmark-and-hotupdate-solution-consolidation` |
| `phase-7-capability-availability-and-pipeline-rollout` | `Phase 7` | `planned` | `codex` | 把 capability / availability / pipeline 变成 solution subject 子对象上的正式定义 | `phase-6-registry-selector-artifact-and-public-cutover` |
| `phase-8-compatibility-closure-and-legacy-cleanup` | `Phase 8` | `planned` | `codex` | 收尾旧模型、旧目录和历史命名 | `phase-7-capability-availability-and-pipeline-rollout` |

说明：

- `Phase 4` 与 `Phase 5` 可以并行准备，但建议在 `Phase 3` archetype subjects 稳定后再正式大规模迁移。
- 原暂停中的 AOT benchmark backend 子任务属于 `Phase 5` 输入，而不是独立 roadmap 主线。

## 7. 依赖

全局依赖关系固定为：

1. 先冻结 solution-first subject 模型与旧 subject 收敛映射。
2. 再把 `tests/` 退回 generic mechanism 层。
3. 再建立新的 subject root schema。
4. 再把 archetype subjects 作为第一批 canonical solution 真源落地。
5. 再分别收敛 functional 主线与 benchmark/hot-update 主线。
6. 只有 canonical subjects 稳定后，才切 registry / selector / artifact / public surface。
7. 最后再落 capability/pipeline 与兼容收尾。

不能倒置的关键原因是：

- 如果先迁 proof/benchmark subject，而 subject root 语义还没冻结，会再迁一次。
- 如果 `tests/` 还混着具体 subject 真源，后续 solution subject 迁移会把旧耦合一起搬过去。
- 如果 archetype subjects 没先稳定，functional/benchmark 两条主线会各自造一套 solution-first 解释。

## 8. 风险

### 风险 1：solution-first 语义又退回 validation-first

如果后续迁移时继续让 proof/benchmark 主导顶层命名，最终仍会回到旧结构。

### 风险 2：`golden` 只改名，不升格

如果 `golden` 只是换个名字但不真正作为 archetype subject 落地，Q0/Q1 仍然没有被解决。

### 风险 3：`tests/perf` 继续持有业务型 workload

如果 `convert-golden-multi-project` 这类检查继续长期留在 `tests/perf`，`tests/` 与 `subjects/` 的边界会再次回混。

### 风险 4：旧 `Bench*` 根目录继续 rollout

如果在 `Phase 5` 前继续沿旧 `Bench*` 根目录扩展 benchmark backend，后续 workload cutover 成本会上升。

### 风险 5：兼容 alias 长期不收口

如果旧 subject id / 旧 CLI / 旧 artifact path 不控制退役周期，solution-first model 会与旧模型长期双栈。

## 9. 备选路径

### 方案 A：solution-first，taxonomy-first，public-later

- 优点：最符合用户刚确认的顶层模型，返工最少。
- 缺点：前几阶段更多是在收敛边界和结构。

### 方案 B：继续沿 validation-first 局部修补

- 优点：短期实现阻力小。
- 缺点：与用户刚确认的长期方向冲突，后续仍会返工。

### 方案 C：public-first cutover

- 优点：入口看起来统一更快。
- 缺点：内部 canonical structure 未稳定前，CLI/TUI/registry 会先返工。

当前采用：

- **方案 A：solution-first，taxonomy-first，public-later**

## 10. 当前建议推进顺序

1. 立即创建 `phase-0-solution-first-model-freeze-and-migration-map` child plan。
2. 用 `Phase 0` 先完成 subject 命名、归宿、alias 与 archetype 定性。
3. 然后做 `Phase 1`，先把 `tests/` 收口。
4. `Phase 1` 完成后，再做 `Phase 2` 建新 subject schema。
5. `Phase 2` 完成后，优先完成 `Phase 3`，把 `golden` 真正升格成 archetype subjects。
6. 之后并行准备 `Phase 4` 与 `Phase 5`，分别收敛 functional 与 benchmark/hot-update 主线。
7. 两条主线都稳定后，再做 `Phase 6` public cutover。
8. 再做 `Phase 7` capability/pipeline rollout。
9. 最后用 `Phase 8` 做兼容层收尾。

## 11. 已确认的计划前提

以下边界已由用户确认，可直接作为后续 child plan 的前提：

1. 顶层 `subject` 的 canonical 语义应是 solution-first source bundle。
2. `scenario/workload` 挂在 solution subject 之下。
3. `tests/` 应回到 generic mechanism 层，subject-specific 配置必须迁回 `subjects/`.
4. `tests/unit/run/` 的 `phase*` 命名必须系统性清理。
5. `golden` 是核心工程级验证输入，不是普通 fixture。
6. `golden` 后续应升格为 archetype solution subjects。
7. `convert-golden-multi-project` 这类检查，应迁成对应 solution subject 的 workload，而不是长期留在 `tests/perf`.
8. 当前暂停的 AOT benchmark backend 子任务必须服从新的 solution-first model。

## 12. 结论

本版 roadmap 已按 solution-first subject 模型重规划完成。

它解决了前一版 roadmap 的根问题：

- 不再把 proof/benchmark 当作顶层 source root
- 不再把 `golden` 错当成 scenario family
- 不再把 solution-specific perf workload 留在 `tests/perf`

下一步应从 `Phase 0` child plan 开始，把 subject 命名、归宿、alias 与 archetype 定性冻结下来。只有这一层收口后，后续迁移才不会在根语义上返工。
