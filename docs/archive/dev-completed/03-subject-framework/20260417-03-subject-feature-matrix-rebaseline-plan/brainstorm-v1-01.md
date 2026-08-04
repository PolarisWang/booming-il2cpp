# Subject 测试内容重梳与 Feature 重验 Brainstorm v1.01

Date: 2026-04-17
Status: concluded

## 1. 用户目标

- 重新梳理 `subjects/` 内的所有测试内容
- 按当前统一测试规范，逐条重新增加已经完成的 AOT feature 和 HotUpdate feature 用例
- 用现在的测试规范重新完整验证一遍已经完成的功能
- 清理旧入口、旧验证路径、旧手工注册逻辑，保持项目干净

## 2. 当前事实

### 2.1 测试规范已经明确

当前正式规范已经冻结为：

`managed solution -> dotnet 8 collection analysis -> collection files -> managed test host -> native test host -> hotupdate test host`

并且已经明确：

- subject / managed project 只直接引用 `Chaos.TestFramework.Sdk`
- managed/native/hotupdate 都消费同一份 collection file
- 验证以 `Assert` 为准，不依赖 `Console.WriteLine`
- native / hotupdate 执行绑定通过 manifest 分层

### 2.2 subject 侧已经有较多 feature 资产

- `SolutionCorePack`
  - proof: 59
  - benchmark: 21
- `MixedExecutionFeaturePack`
  - proof: 5
  - benchmark: 2
- `HotUpdateHostPack`
  - proof: 6
  - benchmark: 3

说明问题已经不是“完全没有 feature”，而是“这些 feature 没有被一套统一的能力矩阵和执行主线重新收口”。

### 2.3 旧逻辑仍然存在

已发现的典型旧残留：

- `SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/Program.cs`
  - 仍手工调用几十个 `*ProofEntry.Run()`
- `MixedExecutionFeaturePack/source/ManagedBridge/Program.cs`
  - 仍维护 `DefaultProofEntries`
  - 仍带手工 `switch`
- `HotUpdateHostPack/source/Host/Program.cs`
  - 仍维护 `DefaultProofEntries`
  - 仍带手工 `switch`
- `subjects/SolutionCorePack/validation/proof/native-reference/`
  - 仍是旧验证路径

这与当前 “attribute -> collection -> generated host” 规范不完全一致。

## 3. 候选方案

### 方案 A：最小补丁式回填

做法：

- 保留现有 subject 目录结构
- 只补缺失的 `ChaosUnitTest` / `ChaosBenchmark`
- 在现有手工入口之上继续兼容运行
- 最后补一轮回归

优点：

- 改动最小
- 能较快补一批 feature

问题：

- 旧入口继续存在
- 后续每次新增 feature 仍要改 attribute 和手工入口两处
- 无法真正证明项目已经切到现行规范

结论：

- 不推荐。它会把这次“重梳”退化成“继续叠历史包袱”。

### 方案 B：能力矩阵重基线

做法：

- 以 3 个 canonical subject 为固定 owner 边界
- 先冻结 completed feature inventory
- 为每个 feature 建立统一 capability matrix 行
- 按矩阵逐条回填/重写 owner proof、按需 benchmark、按触发规则补 hotupdate proof
- 切掉手工 proof/benchmark 聚合入口
- 删除旧 `validation` 残留
- 最后跑完整 managed/native/hotupdate 回归

优点：

- 完全符合当前测试治理规范
- 后续新增 feature 时只需要“补 feature + 跑 collector”
- 能清晰产出“已完成 feature 覆盖率”和“未完成 feature 缺口”

问题：

- 一次性整理量较大
- 需要先做 inventory freeze，不能直接边写边补

结论：

- 推荐。它是唯一能把“内容重梳”和“验证链路重梳”同时完成的方案。

### 方案 C：完全模板化重建 subject

做法：

- 用新模板重新生成 3 个 subject
- 现有 proof/benchmark 文件视为迁移来源
- 基本不保留当前人工布局

优点：

- 结构最干净
- 历史杂质最少

问题：

- 风险过大
- 容易把已经完成的 feature 资产打散
- 需要同时改动 subject、planner、registry、workspace、runner 多条主线

结论：

- 当前不推荐。更适合作为未来一次“二次架构整理”，不适合这次“重梳并完整回归”的目标。

## 4. 推荐结论

采用方案 B：`能力矩阵重基线`。

核心判断：

- 目前 subject 边界已经够收敛，不需要再新开更多 subject
- `Archetypes/*` 也不应原样全保留，而应压缩成“最小必要工程拓扑集合”
- 这次重点不是“换更多目录名”，而是：
  - feature 归属要清楚
  - 用例要按新规范重建
  - 旧手工入口要删除
  - 最终要能做一轮可重复的完整回归

## 5. 分流结论

该任务需要：

- 多阶段推进
- 同时涉及 subject 内容、collector/registry/workspace、generated hosts、full regression
- 会跨多次会话持续推进

因此应进入 `roadmap`，而不是一次性 `plan`。
