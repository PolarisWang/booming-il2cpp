# Subject Feature Matrix Rebaseline Roadmap v1.01

Date: 2026-04-17
Status: proposed

## 1. 目标

把当前 `subjects/` 中已经完成的 AOT / Mixed Execution / HotUpdate 能力，按现行统一测试框架重构为一套清晰、可扩展、可自动校验的验证体系：

- feature 有唯一 owner subject
- feature 有 machine-readable capability matrix
- proof / benchmark / hotupdate proof 全部由 `Chaos.TestFramework.Sdk` attribute 声明
- collector 统一生成 collection file
- managed / native / hotupdate 全部消费同一份 collection
- 工程级 solution 样例压缩为最小必要集合
- 手工 proof/benchmark 聚合入口与旧 `validation` 旁路被彻底删除
- 最终能够一键回答“已完成能力是否被完整验证”

## 2. 范围边界

本 roadmap 负责：

- 盘点并冻结已完成 feature inventory
- 压缩 archetype/engineering scenario 到最小必要集合
- 调整 subject 目录内正式验证资产的组织方式
- 删除手工聚合入口和旧验证路径
- 把已完成 feature 逐条重接到 collection 主线
- 对齐测试、manifest、planner、registry、workspace、dashboard 对 subject 结构的消费方式
- 输出完整回归与 coverage gap

本 roadmap 不负责：

- 新增尚未实现的 AOT Core IR 能力
- 新增尚未实现的 HotUpdate 能力
- 重做整个测试框架大架构
- 非测试主线的业务功能开发

## 3. 非目标

- 不保留“新旧双轨并存”的长期兼容状态
- 不继续扩大 `subject.manifest.json` 中的手工 proof/benchmark 入口协议
- 不继续依赖 `Program::Main + Console.WriteLine` 作为正式 correctness 判定方式
- 不把 capability matrix 仅做成一份人工文档

## 4. 目标结构

### 4.1 Subject 目标结构

推荐目标结构：

```text
subjects/
  SolutionCorePack/
    source/
      EngineeringScenarios/
      FeatureSlices/
      Benchmarks/
    subject.manifest.json
    subject.features.json

  MixedExecutionFeaturePack/
    source/
      EngineeringScenarios/
      ManagedBridge/
      Lowering/
    subject.manifest.json
    subject.features.json

  HotUpdateHostPack/
    source/
      EngineeringScenarios/
      Host/
      PatchModules/
      SharedContracts/
    subject.manifest.json
    subject.features.json
```

说明：

- `EngineeringScenarios/` 是对当前 `Archetypes/` 的推荐改名
- `subject.features.json` 作为每个 owner subject 的 machine-readable capability matrix
- `subject.manifest.json` 收敛为 subject/source/pipeline/matrix 描述，不再承担具体 proof/benchmark 入口真源

### 4.2 Archetype 最小必要集合

最终建议保留：

- `SolutionCorePack`
  - `MultiProjectSolution`
  - `ReferenceAssemblySolution`
  - `CoreLibReferenceSolution`
  - `MixedReferenceClosureSolution`
- `MixedExecutionFeaturePack`
  - `MixedBridgeSolution`
- `HotUpdateHostPack`
  - `FullProjectHotUpdateSolution`

最终建议删除：

- `SolutionCorePack/SimpleLibrarySolution`
- `SolutionCorePack/PackageReferenceSolution`

### 4.3 正式验证资产的唯一集合

每个 feature 最终只允许通过以下资产表达：

- `tests/unit/**`
- `tests/contracts/**`
- owner managed proof
- owner native proof
- 按规则要求的 hotupdate smoke / proof
- 按需 benchmark

以下都必须退出正式主线：

- `DefaultProofEntries`
- 手工 `switch` proof 路由
- `ProofEntry::Run()` 作为长期聚合真源
- `BenchmarkEntry::RunWorkload()` 的手工配置真源
- `subjects/*/validation/*` 旁路验证目录

## 5. 阶段列表

### Phase 0: Authority Freeze And Inventory Baseline

目标：

- 冻结 feature inventory
- 冻结 archetype/engineering scenario inventory
- 冻结 capability matrix / collection / manifest 三者边界
- 冻结 archetype 最小必要集合

Checklist：

- 列出所有已完成 AOT / Mixed / HotUpdate feature
- 为每个 feature 指定：
  - `ownerSubjectId`
  - `capabilityFamily`
  - `capabilityItem`
  - `managed/native/hotupdate` 覆盖要求
  - `benchmark requirement`
  - `engineering scenario coverage`
- 列出所有现有 archetype
- 对每个 archetype 标记：
  - `keep`
  - `merge`
  - `delete`
- 明确哪些场景仍临时依赖 `Program::Main`
- 明确 manifest 中哪些字段必须退出“测试真源”角色

结构调整：

- 新增 `subject.features.json` 契约设计
- 评估 `Archetypes -> EngineeringScenarios` 命名切换

验收标准：

- 每个已完成 feature 都有唯一 matrix 行
- 每个 engineering scenario 都有唯一保留/删除决策
- 团队能明确回答：
  - collection 是什么真源
  - manifest 是什么真源
  - capability matrix 是什么真源

### Phase 1: Subject Entry And Scenario Cutover

目标：

- 删除手工 proof/benchmark 聚合入口
- 压缩 archetype 到最小必要集合
- 尽量把保留下来的 engineering scenario 切到 `Sdk + Assert + collection`
- 收缩 manifest 的手工 proof/benchmark 入口协议

Checklist：

- 删除 `DefaultProofEntries`
- 删除手工 `RunAll(...)`
- 删除手工 proof `switch`
- 删除不再保留的 archetype 目录
- 更新保留 archetype 的项目引用、solution、路径
- 把保留 engineering scenario 中的正式验证迁移到 `ChaosUnitTest` / `ChaosBenchmark`
- 调整 `subject.manifest.json`
  - 去掉不再需要的 `entry` / `workloadEntry` / `entrySelection`
  - 保留 source solution / matrix / pipeline / default set 级信息
- 更新 planner / registry / workspace / subject schema 的相关测试

结构调整：

- `Archetypes/` 重命名为 `EngineeringScenarios/`
- 清理 `subjects/SolutionCorePack/validation/`

验收标准：

- 新增 feature 不再需要改手工入口
- 工程级样例只保留最小必要集合
- 绝大多数正式场景已经不依赖 `Program::Main + Console`
- manifest 不再是 proof/benchmark 具体入口真源

### Phase 2: SolutionCorePack AOT Rebaseline

目标：

- 按 capability matrix 重建 `SolutionCorePack` 已完成 AOT feature 的正式验证面

Checklist：

- 逐个能力域补齐或重写：
  - `PrimitivesAndOps`
  - `ObjectModelAndDispatch`
  - `GenericsAndCollections`
  - `ExceptionsAndControlFlow`
  - `InteropAndMarshaling`
  - `ReflectionAndMetadata`
  - `RuntimeServices`
  - `AsyncAndThreading`
- 为每条 completed feature 对齐：
  - `tests/unit` / `tests/contracts`
  - managed proof
  - native proof
  - 按需 benchmark
- 把工程级 scenario 与 feature matrix 关联起来
- 清理重复、漂移或失效 proof

结构调整：

- 按 capability family 清理 `FeatureSlices` 与 `Benchmarks` 内的命名和归属
- 对齐 collection 中的 stableId 命名

验收标准：

- 每条 completed AOT feature 都能从 matrix 追到正式验证资产
- 不存在“只在文件里有 proof，但不在正式回归中”的漂移项
- AOT 工程级验证被纳入正式 collection 主线，或在 matrix 中明确标注为临时 smoke

### Phase 3: MixedExecution And HotUpdate Rebaseline

目标：

- 按 capability matrix 重建 `MixedExecutionFeaturePack` 与 `HotUpdateHostPack` 已完成能力的正式验证面

Checklist：

- `MixedExecutionFeaturePack`
  - interpreter lowering
  - bridge dispatch
  - mixed generic flow
  - mixed exception flow
  - mixed delegate flow
- `HotUpdateHostPack`
  - skeleton/load
  - metadata supplement
  - method replacement
  - shared contract
  - patch integrity
  - rollback/versioning
- 逐条确认：
  - owner managed proof
  - owner native proof 或 mixed/native equivalent
  - required hotupdate smoke / proof
  - 按需 benchmark

结构调整：

- 对齐 patch/host/shared-contract 样例的正式验证角色
- 清理仍留在 Host/Program 或 ManagedBridge/Program 的手工分发

验收标准：

- 每条 completed mixed/hotupdate feature 都进入正式 collection 主线
- 命中 hotupdate 规则的 feature 全部有 hotupdate 验证
- mixed / hotupdate 的工程级样例与 capability matrix 形成一一对应

### Phase 4: Coverage Automation And Full Regression

目标：

- 让 capability matrix 可自动校验
- 跑完整回归
- 输出 coverage 与 gap 报表

Checklist：

- 建立 matrix 与 collection 的自动对照检查
- 建立 matrix 与 benchmark 覆盖的自动对照检查
- 建立 matrix 与 hotupdate requirement 的自动对照检查
- 运行完整 managed / native / hotupdate 回归
- 产出：
  - feature coverage report
  - benchmark coverage report
  - engineering scenario coverage report
  - remaining gap report

结构调整：

- 新增 coverage report 生成逻辑
- 更新 dashboard / reporting，使其能显示 capability 级视图

验收标准：

- 能自动发现：
  - declared but not covered
  - covered but not in matrix
  - benchmark required but missing
  - hotupdate proof required but missing
- 能明确回答“已完成能力是否完整验证”

### Phase 5: Documentation And Governance Closeout

目标：

- 把最终规则固化到 architecture / wiki / skill

Checklist：

- 更新 `docs/architecture/managed-native-hotupdate-test-pipeline.md`
- 更新 `wiki/04-工具与集成/统一测试框架.md`
- 更新 `wiki/06-测试验证/新增测试接入规范.md`
- 更新 `wiki/06-测试验证/AOT新Feature接入自测规范.md`
- 补充：
  - owner feature matrix 规则
  - engineering scenario 规则
  - manifest / collection / matrix 真源边界

验收标准：

- 新开发者仅看 wiki/architecture 就能正确接入新 feature
- 不再需要口头解释 `Archetypes`、手工入口、旧验证路径的历史背景

## 6. 每阶段完成定义

### Phase 0

- goal: 冻结 inventory、真源边界、最小 archetype 集合
- exit_criteria:
  - `subject.features.json` 结构定稿
  - archetype 保留/删除决策定稿
  - collection / manifest / matrix 权责定稿
- deliverables:
  - feature inventory
  - archetype inventory
  - matrix schema 草案
- dependencies: 无
- open_questions:
  - `subject.features.json` 放每个 subject 还是放仓库根部聚合
  - `Archetypes` 是否本轮改名为 `EngineeringScenarios`

### Phase 1

- goal: 切掉手工入口并压缩 engineering scenarios
- exit_criteria:
  - 无 `DefaultProofEntries`
  - 无手工 proof/benchmark 聚合真源
  - 最小必要 engineering scenario 集合落地
- deliverables:
  - 精简后的 subject 入口
  - 更新后的 manifest
  - 更新后的 tests
- dependencies: Phase 0
- open_questions:
  - 少量确实不能立刻 collection 化的 scenario 如何临时标记

### Phase 2

- goal: SolutionCorePack 形成完整 AOT owner 验证面
- exit_criteria:
  - completed AOT feature 全部映射到正式验证资产
  - 工程级 AOT scenario 进入正式主线或明确标临时状态
- deliverables:
  - 重基线后的 `SolutionCorePack`
  - 对应 unit/contracts/integration 证据
- dependencies: Phase 1
- open_questions: 无

### Phase 3

- goal: Mixed/HotUpdate owner 验证面完整化
- exit_criteria:
  - completed mixed/hotupdate feature 全部进入正式主线
  - required hotupdate validation 完整存在
- deliverables:
  - 重基线后的 `MixedExecutionFeaturePack`
  - 重基线后的 `HotUpdateHostPack`
- dependencies: Phase 1
- open_questions: 无

### Phase 4

- goal: 让 coverage 可自动审计，并完成全量回归
- exit_criteria:
  - coverage 差距可以自动报告
  - managed/native/hotupdate 完整回归可复跑
- deliverables:
  - coverage automation
  - reports
- dependencies: Phase 2, Phase 3
- open_questions:
  - dashboard 是否同时展示 capability 级和工程 scenario 级视图

### Phase 5

- goal: 文档、wiki、skill 与最终状态一致
- exit_criteria:
  - architecture / wiki / skill 全部同步
- deliverables:
  - 更新后的长期文档
- dependencies: Phase 4
- open_questions: 无

## 7. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260417-03-phase-0-authority-freeze-and-inventory-baseline` | Phase 0 | planned | codex | 冻结 feature/archetype inventory 与真源边界 | `-` |
| `20260417-03-phase-1-subject-entry-and-scenario-cutover` | Phase 1 | planned | codex | 删除手工入口并压缩 engineering scenario 集合 | `20260417-03-phase-0-authority-freeze-and-inventory-baseline` |
| `20260417-03-phase-2-solution-core-pack-aot-rebaseline` | Phase 2 | planned | codex | 重建 SolutionCorePack 的 owner AOT 验证面 | `20260417-03-phase-1-subject-entry-and-scenario-cutover` |
| `20260417-03-phase-3-mixed-hotupdate-rebaseline` | Phase 3 | planned | codex | 重建 MixedExecution/HotUpdate 的 owner 验证面 | `20260417-03-phase-1-subject-entry-and-scenario-cutover` |
| `20260417-03-phase-4-coverage-automation-and-full-regression` | Phase 4 | planned | codex | 建立 capability coverage 自动审计并完成全量回归 | `20260417-03-phase-2-solution-core-pack-aot-rebaseline, 20260417-03-phase-3-mixed-hotupdate-rebaseline` |
| `20260417-03-phase-5-documentation-and-governance-closeout` | Phase 5 | planned | codex | 固化最终规范到 architecture/wiki/skill | `20260417-03-phase-4-coverage-automation-and-full-regression` |

## 8. 依赖

- 现有 `Chaos.TestFramework.Sdk / Runtime / collector / manifest` 主线已经可用
- subject discovery / registry / workspace 具备继续演进空间
- 当前 3 个 canonical subject 已经存在，可作为 owner 收口容器
- 后续 AOT 主线能力补洞可以并行推进，但不应混淆本 roadmap 的验证重梳目标

## 9. 风险

- 如果不收缩 manifest 的手工入口协议，collection 主线仍然不是唯一真源
- 如果 capability matrix 只是文档，不做自动校验，很快会再次漂移
- 如果 archetype 只做目录压缩、不改验证方式，仍会留下 `Program + Console` 的旧轨
- 如果不在本轮删除旧 `validation/*` 与手工入口，后续新增 feature 会继续双轨
- 如果 coverage automation 最后才想起来补，Phase 2/3 的结果很难证明“完整”

## 10. 备选路径

### 方案 A：补用例但不改真源结构

- 优点：快
- 缺点：后续继续双轨，等于没完成重构

### 方案 B：能力矩阵重基线并同步切真源

- 优点：最符合当前测试框架
- 缺点：初始梳理成本更高

### 方案 C：整个 subject 体系全量重生

- 优点：最干净
- 缺点：风险过大，容易把已完成资产打散

当前采用：方案 B。

## 11. 当前建议推进顺序

1. 先做 `Phase 0`，冻结真源边界和最小 engineering scenario 集合
2. 再做 `Phase 1`，先切掉最关键的手工入口和旧 scenario 残留
3. 优先推进 `Phase 2`，把 `SolutionCorePack` 这个 AOT 主 owner 先打稳
4. 再推进 `Phase 3`，补齐 mixed/hotupdate owner 面
5. 最后用 `Phase 4` 做自动审计和全量回归
6. 收口到 `Phase 5`，把规则固化到长期文档

## 12. 重审结论

这份 roadmap 在重审后，比原设计更合理的地方有三点：

- 不再默认接受 `Archetypes/*` 维持旧 console smoke 角色，而是要求尽量回到统一 collection 主线。
- 不再把 capability matrix 当说明文档，而是把它提升为 machine-readable 契约，后续能自动对照 collection。
- 不再默认让 `subject.manifest.json` 继续承载 proof/benchmark 具体入口真源，而是明确要求收缩这层协议。

当前仍需注意但已纳入 roadmap 的唯一高风险点是：

- 这一轮不只是内容补齐，而是一次 manifest / tests / planner / registry / workspace 的真实 cutover，所以 Phase 1 的改动面会比表面看起来大。
