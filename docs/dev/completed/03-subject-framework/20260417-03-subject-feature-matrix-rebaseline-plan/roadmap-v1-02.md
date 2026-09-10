# Subject/Test Framework Full Rebirth Roadmap v1.02

Date: 2026-04-17
Status: completed

## 1. 目标

以方案 C 为唯一执行路径，完成整个 subject 体系与测试框架大架构的全量重生。

最终状态必须满足：

- 不存在“新旧双轨并存”的长期兼容状态
- 正式 correctness 完全退出 `Program::Main + Console.WriteLine`
- `Chaos.TestFramework.Sdk -> collector -> collection -> hosts` 成为唯一正式测试主线
- `subject.features.json`、`subject.manifest.json`、collection 三者职责清晰且不混职
- planner / registry / workspace / reporting / dashboard 与新模型一致
- 旧 subject 结构、旧 validation、旧手工入口全部删除

## 2. 范围边界

本 roadmap 负责：

- 整个测试框架大架构
- subject 体系全量重生
- 为落地新测试框架所需的非测试主线业务功能调整

明确纳入范围的业务面：

- subject discovery / planner / registry / workspace
- generated managed/native/hotupdate hosts
- run/build/report orchestration
- benchmark dashboard / reporting
- 与 subject/source/pipeline 装配直接相关的 driver 与 generator

本 roadmap 不负责：

- 主动扩展新的 AOT Core IR feature
- 主动扩展新的 HotUpdate feature
- 与测试框架架构无关的业务功能开发

## 3. 非目标

- 不保留长期 compatibility alias
- 不保留旧 `validation/*` 旁路
- 不保留手工 proof/benchmark 聚合入口
- 不保留 `Archetypes`、`FeatureSlices` 等旧命名仅为兼容而继续存在
- 不接受“先加一层适配器，以后再清理”的长期路线

## 4. 阶段列表

### Phase 0: Full-Rebirth Authority Freeze

目标：

- 冻结全量重生后的目标架构
- 冻结单轨真源边界
- 冻结新的 subject/feature/scenario 命名模型

Checklist：

- 冻结 `subject.features.json` 契约
- 冻结 `subject.manifest.json` 目标职责
- 冻结 collection 契约职责
- 冻结 `Archetypes -> EngineeringScenarios`
- 冻结 `FeatureSlices -> Proofs`
- 冻结顶层 canonical subjects
- 冻结最小必要 engineering scenarios 集合
- 冻结“哪些业务管线必须一起改”

结构调整：

- 引入 `subject.features.json`
- 目标命名全部切换到新模型

验收标准：

- 团队可以明确回答：
  - 什么是测试声明真源
  - 什么是 coverage obligation 真源
  - 什么是 subject/source/pipeline 真源
- 新结构命名不再依赖历史背景解释

### Phase 1: Framework Contract And Generator Rebuild

目标：

- 重建测试框架 contract 层和生成层

Checklist：

- 重构 `Chaos.TestFramework.Sdk`
- 重构 `Chaos.TestFramework.Runtime`
- 固化 `.NET 8` collector
- 建立新 collection schema
- 建立 `subject.features.json` 读写与校验逻辑
- 建立新的 generated host/template contract
- 所有文件级生成优先用 Scriban

结构调整：

- 统一 framework contracts
- 统一 generator / template 层

验收标准：

- framework contract 不再依赖旧 subject 结构
- generated hosts 只消费新 contract
- 无旧 `Annotation` / alias / 双轨 contract

### Phase 2: Business Pipeline Rebuild

目标：

- 把业务消费管线切换到新 subject/test model

Checklist：

- 重构 planner
- 重构 registry
- 重构 workspace generation
- 重构 subject workers / run pipeline
- 重构 report pipeline
- 重构 benchmark dashboard 数据消费模型
- 清理旧 manifest/entry/workload 协议

结构调整：

- `subject.manifest.json` 从测试入口真源降级为 subject/source/pipeline 配置
- dashboard 改成 capability-aware + scenario-aware

验收标准：

- planner / registry / workspace / dashboard 全部能消费新模型
- 新业务管线不再依赖旧 subject 路径和旧入口约定

### Phase 3: Subject System Full Rebirth

目标：

- 在新架构上直接重生三大 canonical subject

Checklist：

- 重建 `SolutionCorePack`
- 重建 `MixedExecutionFeaturePack`
- 重建 `HotUpdateHostPack`
- 目录结构切为：
  - `Proofs/`
  - `Benchmarks/`
  - `EngineeringScenarios/`
  - `Host/`
  - `Patch/`
  - `SharedContracts/`
- 删除旧目录：
  - `Archetypes/`
  - `FeatureSlices/`
  - `validation/*`
- 压缩 engineering scenarios 到最小必要集合

结构调整：

- subject 目录按新结构落地
- subject IDs 可保留，内部结构全部重生

验收标准：

- 仓库中已经看不到旧 subject 结构
- 工程级 scenario 集合最小化完成
- 新增 feature 时只会进入新结构

### Phase 4: Completed Feature Remigration And Verification Rebuild

目标：

- 把已完成 AOT / Mixed / HotUpdate feature 全部重接到新 subject 体系

Checklist：

- 为每条 completed feature 重建：
  - unit/contracts evidence
  - managed proof
  - native proof
  - required hotupdate smoke / proof
  - required benchmark
- 把 engineering scenario 与 feature obligations 关联起来
- 删除旧 proof/benchmark 资产中的重复项、漂移项、失效项

结构调整：

- 对齐 stableId / capability IDs / category 命名

验收标准：

- 每条 completed feature 都能从 `subject.features.json` 追到正式验证资产
- 不再存在“有代码但不在正式验证主线里”的 feature

### Phase 5: One-Shot Cutover And Legacy Purge

目标：

- 完成一次性切换，并彻底删除旧体系

Checklist：

- 删除旧 tests 中对旧路径/旧入口的兼容逻辑
- 删除旧 runner / registry / fixture / docs
- 删除旧 `Program::Main + Console` correctness 测试路径
- 删除旧 dashboard 数据兼容层
- 删除旧 manifest 字段和旧 schema

结构调整：

- 仓库只保留新 subject/test framework 结构

验收标准：

- 仓库合入状态中不存在长期双轨
- 新旧结构不会并存

### Phase 6: Coverage Automation, Full Regression, Docs Closeout

目标：

- 建立自动 coverage 审计
- 跑完整回归
- 完成架构文档和 wiki 收口

Checklist：

- matrix vs collection 自动对照
- matrix vs benchmark 自动对照
- matrix vs hotupdate requirement 自动对照
- managed/native/hotupdate 全量回归
- 更新 architecture
- 更新 wiki
- 更新 skill / governance 文档

验收标准：

- 能自动回答“已完成 feature 是否完整验证”
- 新开发者只看文档就能按新主线接入

## 5. 每阶段完成定义

### Phase 0

- goal: 冻结全量重生后的单轨目标结构
- exit_criteria:
  - 新命名模型冻结
  - 新真源边界冻结
  - 最小 scenario 集合冻结
- deliverables:
  - architecture freeze 文档
  - features/manifests/schema 草案
- dependencies: 无
- open_questions:
  - `subject.features.json` 是否分 subject 存储

### Phase 1

- goal: 重建 contract 和 generator
- exit_criteria:
  - framework contract 稳定
  - generator 与模板层可服务新 subject 模型
- deliverables:
  - framework contracts
  - collection schema
  - templates
- dependencies: Phase 0
- open_questions:
  - host templates 的最小共享层怎么拆

### Phase 2

- goal: 业务管线切到新模型
- exit_criteria:
  - planner/registry/workspace/dashboard 不再吃旧协议
- deliverables:
  - rebuilt business pipelines
- dependencies: Phase 1
- open_questions:
  - dashboard 视图是否同时区分 feature 与 scenario

### Phase 3

- goal: subject 目录和结构全量重生
- exit_criteria:
  - canonical subjects 新结构落地
  - 旧结构从仓库中消失
- deliverables:
  - regenerated subjects
- dependencies: Phase 2
- open_questions:
  - 是否保留当前 subjectId，推荐保留

### Phase 4

- goal: 已完成 feature 全部重接到新体系
- exit_criteria:
  - completed features 全量可追踪、可执行、可验证
- deliverables:
  - rebuilt proofs/benchmarks/hotupdate validations
- dependencies: Phase 3
- open_questions: 无

### Phase 5

- goal: 一次性 cutover 并删除旧体系
- exit_criteria:
  - 无长期双轨
  - 无旧 console correctness
- deliverables:
  - legacy purge record
- dependencies: Phase 4
- open_questions: 无

### Phase 6

- goal: coverage 自动化 + 完整回归 + 文档收口
- exit_criteria:
  - 自动审计可用
  - 回归可重复
  - 文档与代码一致
- deliverables:
  - regression reports
  - docs/wiki updates
- dependencies: Phase 5
- open_questions: 无

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260417-03-phase-0-full-rebirth-authority-freeze` | Phase 0 | completed | codex | 冻结单轨真源边界、新命名模型和最小 scenario 集合，并完成 file-backed capability authority seed 落地 | `-` |
| `20260417-03-phase-1-framework-contract-and-generator-rebuild` | Phase 1 | completed | codex | 重建 Sdk/Runtime/collector/schema/template contract，并完成模板边界与 collector 工具归位 | `20260417-03-phase-0-full-rebirth-authority-freeze` |
| `20260417-03-phase-2-business-pipeline-rebuild` | Phase 2 | completed | codex | 重建 planner/registry/workspace/run/report/dashboard 业务消费管线 | `20260417-03-phase-1-framework-contract-and-generator-rebuild` |
| `20260417-03-phase-3-subject-system-full-rebirth` | Phase 3 | completed | codex | 在新架构上重生三大 canonical subject | `20260417-03-phase-2-business-pipeline-rebuild` |
| `20260417-03-phase-4-completed-feature-remigration-and-verification-rebuild` | Phase 4 | completed | codex | 把已完成 feature 全部重接到新 subject/test 主线 | `20260417-03-phase-3-subject-system-full-rebirth` |
| `20260417-03-phase-5-one-shot-cutover-and-legacy-purge` | Phase 5 | completed | codex | 删除旧体系并完成一次性切换 | `20260417-03-phase-4-completed-feature-remigration-and-verification-rebuild` |
| `20260417-03-phase-6-coverage-automation-full-regression-and-docs-closeout` | Phase 6 | completed | codex | 自动审计、全量回归、文档治理收口 | `20260417-03-phase-5-one-shot-cutover-and-legacy-purge` |

## 7. 依赖

- 现有 `Chaos.TestFramework.Sdk / Runtime / collector` 基础可继续演进
- 当前三大 canonical subject 可作为外部稳定锚点保留 subjectId
- AOT/HotUpdate 主能力扩展可以与本 roadmap 并行，但不能打乱本 roadmap 的单轨重生目标

## 8. 风险

- 范围明显大于上一版，执行期会更长
- Phase 2 和 Phase 3 的耦合度高，不能把 subject 重生和业务管线切开太久
- 若 Phase 5 不彻底，仓库会重新退化为双轨
- 若 dashboard/report 不一起重建，coverage 无法真正可见

## 9. 备选路径

### 方案 A：在旧 subject 上继续增量修补

- 优点：短期看起来快
- 缺点：永远留在双轨里

### 方案 B：先做半重构，再逐步替换

- 优点：风险看起来较低
- 缺点：长期兼容成本高，不符合当前目标

### 方案 C：整个 subject 体系全量重生

- 优点：最符合当前目标，最终结构最清晰
- 缺点：一次性改动最大

当前采用：方案 C。

## 10. 当前建议推进顺序

1. `Phase 0`: 冻结单轨架构和真源边界
2. `Phase 1`: 重建框架 contract 与 generator
3. `Phase 2`: 重建业务消费管线
4. `Phase 3`: 在新管线上重生 subject 体系
5. `Phase 4`: 把已完成 feature 全量重接
6. `Phase 5`: 一次性删除旧体系
7. `Phase 6`: 自动审计、全量回归、文档收口

## 11. 重审结论

按用户新要求，这版 roadmap 比前一版更合理的地方在于：

- 真正把“整个测试框架大架构”纳入了范围，而不是只整理 subject 内容
- 明确允许为落地测试框架去调整必要的业务功能
- 明确禁止长期双轨
- 明确禁止 console correctness
- 明确选择了“subject 体系全量重生”，而不是继续增量 cutover
