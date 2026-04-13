# Subject Solution Core-Pack 收敛 Roadmap v1.01

Date: 2026-04-13
Status: draft-approved

## 1. 目标

把当前 `subjects/` 收敛成方案 A 对应的极少数 retained solution-mode subject，并让所有 retained subject 跑通统一的 `proof` 和 `benchmark` 流程。

本 roadmap 的直接结果应是：

- `subjects/` 从大量 legacy root 收敛到极少数 retained subject
- `SolutionCorePack` 成为绝大多数现有 subject 的归宿
- retained subject 全部升级为显式 `.sln`
- retained subject 内部入口统一
- retained subject 全部完成 `proof` / `benchmark` 验证闭环

## 2. 范围边界

本 roadmap 负责：

- retained subject 清单冻结
- legacy subject 到 retained subject 的迁移映射
- retained subject 的 solution 结构升级
- retained subject 的统一 launcher 与入口协议
- retained subject 的 proof / benchmark 最小覆盖定义
- registry / selector / CLI 对 retained subject 的对齐
- 对所有 retained solution-mode subject 的验证计划

本 roadmap 不负责：

- 一次性打通全部 device farm / mobile 实机自动化
- 一次性重构全部底层 pipeline 实现

## 3. 非目标

- 不再新增分域长期 canonical pack
- 不回退到 legacy `*Proof` / `Bench*` 主路径
- 不把 engineering workload 全部改写为 `ChaosBenchmark`

## 4. 阶段列表

### Phase 0: Retained Subject Freeze And Migration Matrix

- 冻结 retained subject 仅保留：
  - `SolutionCorePack`
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
- 输出全量 legacy subject 迁移矩阵
- 冻结 “何时允许新建顶层 subject” 的规则

### Phase 1: Explicit Solution Upgrade

- 让所有 retained subject 升级为显式 `.sln`
- 建立 retained subject 的统一 solution 布局
- 明确 subject slice 与 project graph

### Phase 2: SolutionCorePack 合并

- 把 archetype、mainline、interop、engine-facing、core benchmark、onboarding、mobile slice 并入 `SolutionCorePack`
- 退役绝大多数 legacy root

### Phase 3: HotUpdate / Mixed Execution 收敛

- 让 `HotUpdateHostPack` 与 `MixedExecutionFeaturePack` 成为真正的 retained subject
- 吸收与其 executable plan 对应的 legacy root

### Phase 4: 统一入口与 registry cutover

- retained subject 内部入口统一
- 外层 registry / selector / CLI 默认路径切到 retained subject

### Phase 5: Retained Subject Verification Closure

- 对所有 retained solution-mode subject 跑 proof
- 对所有 retained solution-mode subject 跑 benchmark
- 固化验证命令、summary、artifact、rerun path

### Phase 6: Compatibility Closure And Legacy Cleanup

- 删除或归档 legacy root
- 一次性删除全部 compatibility alias
- 更新文档和仓库心智入口
- 收尾清理 `tests/` 中非 retained-subject smoke 的 `subjects` 耦合，把机制测试改回 synthetic fixture / 通用功能分层

## 4.1 执行状态快照

Updated: 2026-04-13 13:26:39 +08:00

| Phase | status | result |
| --- | --- | --- |
| `Phase 0` | `completed` | retained subject 清单、迁移矩阵与边界规则已冻结 |
| `Phase 1` | `completed` | 3 个 retained subject 全部升级为显式 `.sln` source，并打通 `primaryProjectPath` |
| `Phase 2` | `completed` | `SolutionCorePack` 吸收 archetype/mainline/core benchmark 主体，并跑通真实 proof / benchmark |
| `Phase 3` | `completed` | `HotUpdateHostPack` 与 `MixedExecutionFeaturePack` 完成 retained 化并跑通真实 proof / benchmark |
| `Phase 4` | `completed` | retained subject public path、planner / registry / selector / CLI 路径已统一到 canonical retained subject |
| `Phase 5` | `completed` | 所有 retained solution-mode subject 与 `SolutionCorePack` archetype managed-output matrix 已完成真实验证 |
| `Phase 6` | `completed` | legacy `Solution*` roots 已删除，compatibility alias 已清零，并完成最小 `tests/` synthetic-fixture decoupling |

## 5. 每阶段完成定义

### Phase 0

- `goal`
  - 把“最终长期保留哪些 subject”和“其余 subject 去哪”一次性冻结，避免后续迁移反复摇摆。

- `structure_adjustments`
  - 新增迁移矩阵文档
  - 新增 retained subject 命名规则文档

- `checklist`
  - 冻结 retained subject 清单为三类
  - 冻结“新增顶层 subject 必须证明有新 executable plan”
  - 盘点当前所有顶层 subject
  - 为每个 subject 指定迁移目标：
    - `SolutionCorePack`
    - `HotUpdateHostPack`
    - `MixedExecutionFeaturePack`
    - `compatibility-only alias`
  - 为每个 subject 指定迁移形式：
    - engineering validation
    - engineering workload
    - `ChaosUnitTest`
    - `ChaosBenchmark`
    - retained slice project
  - 标记需要逐条重判的边界项：
    - `GenericEcho`
    - `CompatibilityMatrixProof`
    - `MobileHelloWorldProof`
  - 记录两条挂起任务在新路线中的重挂接方式

- `deliverables`
  - retained subject freeze 文档
  - legacy-to-retained 迁移矩阵
  - executable plan 准入规则

- `exit_criteria`
  - 不再存在 “到底保留几个 canonical subject” 的分歧
  - 所有 legacy root 都有明确去向

- `acceptance`
  - 团队可以一句话回答任意 legacy subject 最终去哪

### Phase 1

- `goal`
  - 让 retained subject 从 project-set 升级为显式 solution-mode subject。

- `structure_adjustments`
  - 新增：
    - `subjects/SolutionCorePack/source/SolutionCorePack.sln`
    - `subjects/HotUpdateHostPack/source/HotUpdateHostPack.sln`
    - `subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.sln`
  - 新增 `Launcher/` project
  - retained subject 下新增分 slice project 目录

- `checklist`
  - 为三个 retained subject 设计 `.sln`
  - 为三个 retained subject 增加统一 launcher project
  - 设计 `Launcher` 与各 slice assembly 的引用关系
  - 设计 `subject.manifest.json` 的新 `source` 指向
  - 校验 registry/path resolver 能接受 solution source
  - 保持 `src/reference/Chaos.TestFramework` 为测试声明引用层
  - 明确 `src/managed/` 与 `src/reference/` 边界不回退

- `deliverables`
  - retained solution skeleton
  - updated source model design

- `exit_criteria`
  - retained subject 全部有显式 `.sln`
  - retained subject 都可以被识别为 solution-mode source

- `acceptance`
  - `subjects/` 下不再存在 “canonical 但没有 `.sln`” 的 retained subject

### Phase 2

- `goal`
  - 建立 `SolutionCorePack`，并把绝大多数现有 subject 吸收进去。

- `structure_adjustments`
  - 新增或整理 `SolutionCorePack` 的内部 slice：
    - `Archetypes/`
    - `Mainline/`
    - `Interop/`
    - `EngineHost/`
    - `Benchmarks/`
    - `Mobile/`
  - legacy roots 进入 alias / redirect 过渡

- `checklist`
  - 迁入 `SolutionSimpleLib`
  - 迁入 `SolutionMultiProject`
  - 迁入 `SolutionPackageReference`
  - 吸收 `MainlineFeaturePack`
  - 吸收 `PerformanceFeaturePack`
  - 吸收 `BenchDelegate`
  - 吸收 `BenchException`
  - 吸收 `BenchGameLoop`
  - 逐条重判 `GenericEcho`
  - 吸收 `HelloWorld`
  - 吸收 `HelloWorldObject`
  - 吸收 `PInvokeLite`
  - 吸收 `ReflectionLite`
  - 吸收 `HostEmbeddingLite`
  - 吸收 `EngineHostProof`
  - 吸收 `EngineLifecycleCallbackLite`
  - 吸收 `EngineLogWriteLite`
  - 吸收 `EngineObjectHandleLite`
  - 吸收 `BridgeRoundtripProof`
  - 吸收 `AutoBridgeProof`
  - 吸收 `GenericSupplementProof`
  - 重判 `CompatibilityMatrixProof`
  - 吸收 `MobileHelloWorldProof`
  - 为 `SolutionCorePack` 准备最小 proof
  - 为 `SolutionCorePack` 准备最小 benchmark

- `deliverables`
  - `SolutionCorePack` solution
  - core migration matrix implementation plan
  - deprecated legacy root list

- `exit_criteria`
  - `SolutionCorePack` 成为主 retained subject
  - 上述 subject 不再需要以独立顶层 root 继续长期存在

- `acceptance`
  - 新增 mainline/interop/archetype/core benchmark 测试点时，默认只需要改 `SolutionCorePack`

### Phase 3

- `goal`
  - 让 hot-update 与 mixed execution 形成清晰 retained subject，不与 core pack 混乱交织。

- `structure_adjustments`
  - `HotUpdateHostPack` 下整理：
    - `Host/`
    - `Patch/`
    - `SharedContracts/`
    - `Benchmarks/`
  - `MixedExecutionFeaturePack` 下整理：
    - `Managed/`
    - `Interpreter/`
    - `Benchmarks/`

- `checklist`
  - `HotUpdateHostPack` 升级为显式 `.sln`
  - 吸收 `MethodReplacementProof`
  - 吸收 `SharedContractProof`
  - 吸收 `VersionRollbackProof`
  - 为 `HotUpdateHostPack` 补齐最小 benchmark
  - `MixedExecutionFeaturePack` 升级为显式 `.sln`
  - 整理 mixed execution proof / benchmark 入口
  - 为 `MixedExecutionFeaturePack` 保证 proof / benchmark 都可直接运行

- `deliverables`
  - upgraded `HotUpdateHostPack`
  - upgraded `MixedExecutionFeaturePack`

- `exit_criteria`
  - retained subject 三分结构稳定
  - hot-update / mixed execution 不再继续衍生新的 legacy root

- `acceptance`
  - 新增 hot-update 测试点默认只改 `HotUpdateHostPack`
  - 新增 mixed execution 测试点默认只改 `MixedExecutionFeaturePack`

### Phase 4

- `goal`
  - 把 retained subject 的内部入口和外层入口全部统一起来。

- `structure_adjustments`
  - retained subject 统一使用 `Launcher/Program.cs`
  - registry / selector / CLI 默认展示 retained subject

- `checklist`
  - 统一三个 retained subject 的 `Program::Main`
  - 入口协议改为 typed dispatch
  - 引入或冻结内部枚举：
    - `ChaosSubjectEntryKind`
    - `ChaosSubjectSlice`
  - 为全部公共枚举及枚举成员补注释
  - 更新 manifest/source entry
  - 更新 registry 展示
  - 更新 selector 默认行为
  - 更新 CLI 示例与帮助信息
  - 让 legacy alias 只做 redirect，不再作为独立规划对象

- `deliverables`
  - unified retained launcher model
  - updated registry / selector / CLI behavior

- `exit_criteria`
  - retained subject 内部入口风格一致
  - 用户默认通过 retained subject 运行测试

- `acceptance`
  - 不再需要记忆几十个 legacy `Program::Run*` 入口

### Phase 5

- `goal`
  - 对所有 retained solution-mode subject 跑通 proof 和 benchmark，并形成正式验收记录。

- `structure_adjustments`
  - 可能新增验证清单文档
  - 可能新增 per-subject verification ledger

- `checklist`
  - `run test registry refresh`
  - 验证 `SolutionCorePack` proof
  - 验证 `SolutionCorePack` benchmark
  - 验证 `HotUpdateHostPack` proof
  - 验证 `HotUpdateHostPack` benchmark
  - 验证 `MixedExecutionFeaturePack` proof
  - 验证 `MixedExecutionFeaturePack` benchmark
  - 记录每次运行的：
    - command
    - summary path
    - events path
    - rerun command
  - 对失败项形成 failure digest
  - 必要时回补最小 proof / benchmark 条目

- `deliverables`
  - retained subject verification report
  - rerun command ledger

- `exit_criteria`
  - retained subject 全部 proof 通过
  - retained subject 全部 benchmark 通过

- `acceptance`
  - 可以明确说出 “所有 solution-mode subject 已跑通 proof 和 benchmark”，并且附带最新验证证据

### Phase 6

- `goal`
  - 完成 legacy cleanup，让仓库默认心智只剩 retained solution-mode subject。

- `structure_adjustments`
  - 删除、归档或 redirect：
    - `*Proof`
    - `Bench*`
    - `*Lite`
    - `HelloWorld*`
    - 分散的 `Solution*` legacy root
  - 更新 docs / onboarding / wiki / command examples

- `checklist`
  - 清理 legacy subject root
  - 一次性删除全部 compatibility alias
  - 更新 `subjects/` 文档入口
  - 更新 `run test` 示例
  - 更新 roadmap / wiki / onboarding
  - 清理 `tests/unit/planning/test_subject_planner.py`、`tests/unit/planning/test_project_workspace.py` 等历史残留的 subject-heavy 机制测试，重新按通用机制 / retained-subject smoke 分层
  - 为挂起任务补写新的恢复入口说明

- `deliverables`
  - cleaned `subjects/` root
  - updated developer docs
  - legacy retirement note

- `exit_criteria`
  - 仓库默认心智中只剩 retained subject
  - compatibility alias 全部清零
  - 新增测试点时不会再自然想到创建 legacy root

- `acceptance`
  - 打开 `subjects/`，可以一眼看清 retained subject 与其职责

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `phase-0-retained-subject-freeze-and-migration-matrix` | `Phase 0` | `completed` | `codex` | 冻结 retained subject 清单、输出迁移矩阵 | `-` |
| `phase-1-explicit-solution-upgrade` | `Phase 1` | `completed` | `codex` | 把 retained subject 升级为显式 `.sln` | `phase-0-retained-subject-freeze-and-migration-matrix` |
| `phase-2-solution-core-pack-consolidation` | `Phase 2` | `completed` | `codex` | 建立 `SolutionCorePack` 并吸收绝大多数 legacy subject | `phase-1-explicit-solution-upgrade` |
| `phase-3-hotupdate-and-mixed-consolidation` | `Phase 3` | `completed` | `codex` | 完成 `HotUpdateHostPack` 与 `MixedExecutionFeaturePack` retained 化 | `phase-1-explicit-solution-upgrade` |
| `phase-4-unified-entry-and-registry-cutover` | `Phase 4` | `completed` | `codex` | 统一入口、registry、selector 与 CLI | `phase-2-solution-core-pack-consolidation, phase-3-hotupdate-and-mixed-consolidation` |
| `phase-5-retained-subject-verification-closure` | `Phase 5` | `completed` | `codex` | 对 retained subject 全量跑 proof / benchmark 并记录证据 | `phase-4-unified-entry-and-registry-cutover` |
| `phase-6-compatibility-closure-and-legacy-cleanup` | `Phase 6` | `completed` | `codex` | 清理 legacy root，完成最终收口 | `phase-5-retained-subject-verification-closure` |

## 7. 依赖

- Phase 0 是全部阶段前置
- Phase 1 必须先完成，否则 retained subject 还不是显式 solution
- Phase 2 与 Phase 3 在 Phase 1 之后可并行准备
- Phase 4 必须等 core/hotupdate/mixed 三类 retained subject 都稳定
- Phase 5 必须在 Phase 4 之后，因为需要统一入口和 registry 已切换
- Phase 6 最后执行，避免过早清理导致回归难定位

## 8. 风险

### 风险 1：`SolutionCorePack` 成为“黑盒大包”

影响：

- 合并后难定位
- 团队不敢继续往里加测试点

缓解：

- 保持 slice/project 层次
- 统一 subject，不统一到单 project

### 风险 2：旧 benchmark 边界再次跑偏

影响：

- engineering workload 和 runtime benchmark 混淆

缓解：

- Phase 0 明确重判表
- Phase 2 逐条迁，不整包搬

### 风险 3：入口统一后仍残留大量字符串协议

影响：

- subject 内部长期不可维护

缓解：

- 内部入口用 enum
- 外部字符串只停留在 CLI 边界

### 风险 4：挂起任务恢复路径丢失

影响：

- 旧阶段成果无法回收

缓解：

- 在 Phase 0 和 Phase 6 都记录挂起任务的新挂接方式

## 9. 备选路径

已被用户否决：

- 分域长期并存的 canonical packs 路线
- 只统一入口、暂不做物理收敛的路线

当前唯一执行路线：

- 方案 A：极限大合并，保留一个超大 `SolutionCorePack`，外加极少数因 executable plan 差异而保留的 retained subject

## 10. 当前建议推进顺序

1. 先做 `Phase 0`，把 retained subject 与迁移矩阵冻结。
2. 立刻推进 `Phase 1`，补齐显式 `.sln`。
3. 先做 `SolutionCorePack`，因为它会吸收最多 legacy root。
4. 再做 `HotUpdateHostPack` 与 `MixedExecutionFeaturePack` retained 化。
5. 然后切统一入口和 registry。
6. 最后统一做 proof / benchmark 验收和 legacy cleanup。

这个顺序的原因很直接：

- 不先冻住 retained subject，后面所有迁移都会反复变更目标
- 不先补显式 `.sln`，就无法把“solution 模式 subject”真正落到工程层
- 不先做 `SolutionCorePack`，仓库里最大的一坨 legacy root 就不会消失
- 不先统一入口，就无法稳定验证全部 retained subject 的 proof / benchmark
