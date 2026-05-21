# Subject Layout And Variant Traceability Roadmap v1.01

Date: 2026-04-07
Status: active-roadmap

## 1. 目标

把当前仓库从“subject 布局、执行产物、variant 语义和公开入口仍然交织”的状态，拆解为一条可以分阶段推进的落地路线：

- 先冻结 canonical 目录与 manifest 语义
- 再重构可追踪的 artifact/run 脊柱
- 再引入 variant-aware codegen/native
- 最后再处理 CLI 兼容、清理与 wiki 沉淀

本 roadmap 不直接承担实现，而是负责冻结阶段边界、依赖关系和 child task 切分。

## 2. 范围边界

本 roadmap 负责：

- `tests/`、`subjects/`、`src/`、`contracts/`、`artifacts/subjects/` 的目标职责划分
- `subject.manifest.json` 的新模型
- `run test subject --id ...` 的新选择语义
- `analysis -> codegen -> native -> report -> perf` 的可追踪落地骨架
- `CHECK / PROFILE / SHIP` 的正式引入顺序
- wiki 最终落地前的阶段化实施路径

本 roadmap 不负责：

- 直接修改实现代码
- 直接创建 child plan 之外的执行 chunk
- 直接落地 wiki 页面
- 立即删除所有旧路径和旧命令

## 3. 非目标

当前不在父 roadmap 中一次性解决：

- 每个历史 suite 的逐条兼容细节
- 所有宿主平台的完整矩阵扩展
- 全量发布流程包装
- 最终公共 TUI 展示形式

## 4. 阶段列表

### Phase 1: Path Resolver And Contract Samples Canonicalization

- 先抽象 `subjects`、`contracts`、`artifacts/subjects` 的 resolver
- 收敛 `contracts/examples -> samples`
- 把误导性的 `analysis/contracts/examples` 从长期真源里剥离

### Phase 2: Subject Root Expansion And Manifest Cutover

- 把 `subjects/` 升级为正式 subject 根
- 引入 `validation / expected / baselines`
- 把 `subject.manifest.json` 扩展为 typed validation + validationProfiles

### Phase 3: Run-Scoped Artifact Spine

- 以 `runs/<run-id>` 为 canonical 产物根
- 引入 `run -> analysis -> variant -> matrix` 分层
- 把 `pipeline-report/` 与 `validations/<kind>/` 分离

### Phase 4: Variant-Aware Codegen And Native

- 从 `codegen` 开始引入 `CHECK / PROFILE / SHIP`
- 让 codegen 宏与 native 编译选项成为同一组正式变体语义
- 冻结 validation 到 default variant 的映射规则

### Phase 5: Tooling/Test/Public Cutover

- 改造 `run test subject` 的 selector 与兼容层
- 让 `tests/` 仅保留 generic 资产
- 让 subject 内的 `xUnit` / perf harness 真正进入执行入口

### Phase 6: Compatibility Closure And Wiki Handoff

- 整理旧路径、旧命令、旧目录命名的保留与退出策略
- 把最终稳定规则沉淀到 `wiki/`
- 输出 `CHECK / PROFILE / SHIP` 宏定义与使用边界

## 5. 每阶段完成定义

### Phase 1

- `goal`: 冻结路径真源和 contracts 命名语义，避免后续实现继续硬编码旧物理路径。
- `exit_criteria`:
  - 存在统一 resolver，可表达 subject root、subject artifact root、contract root
  - `examples -> samples` 的命名迁移方案固定
  - `analysis/contracts/examples` 不再被视为 canonical source
- `deliverables`:
  - resolver 设计与迁移点清单
  - contract samples 命名迁移清单
  - 受影响路径索引
- `dependencies`: 无
- `open_questions`:
  - 全局 `contracts` 下哪些 sample 需要长期保留为跨 subject 参考集

### Phase 2

- `goal`: 把 `subjects/` 变成正式测试工程根，并冻结 manifest 最小 schema。
- `exit_criteria`:
  - `subjects/<id>/validation|expected|baselines` 结构冻结
  - `subject.manifest.json` 支持 `validationProfiles` 与 typed validation
  - `xUnit` 与 C# perf harness 的 subject 内归属完成定义
- `deliverables`:
  - subject root 结构规范
  - manifest schema 迁移方案
  - 旧 `goal` 的兼容映射边界
- `dependencies`:
  - Phase 1
- `open_questions`:
  - `proof` 目录中 subject 侧静态资产与 pipeline 侧动态证据的最终命名边界

### Phase 3

- `goal`: 形成按 run 归档、可追踪、可回放的 subject artifact spine。
- `exit_criteria`:
  - `artifacts/subjects/<id>/runs/<run-id>` 成为 canonical 根
  - `current.json` / `last.json` 指针生效
  - `analysis`、`pipeline-report`、`validations` 的层级冻结
- `deliverables`:
  - run ledger 结构
  - artifact 目录骨架
  - summary/events/logs 的 subject-local 归档方案
- `dependencies`:
  - Phase 2
- `open_questions`:
  - 是否在后续阶段额外补一个只读全局聚合索引，但不改变 canonical 存储

### Phase 4

- `goal`: 让 `CHECK / PROFILE / SHIP` 成为从 codegen 到 native 的正式变体。
- `exit_criteria`:
  - `variant` 从 `codegen` 开始分叉
  - codegen 宏定义与 native 编译选项绑定到同一 variant
  - `proof/unit/perf` 到默认 variant 的映射冻结
- `deliverables`:
  - variant 模型定义
  - 宏定义与 native 选项映射表
  - variant-aware 目录与 report 规则
- `dependencies`:
  - Phase 3
- `open_questions`:
  - `SHIP` 是否在首批 child plan 中只要求产物生成，不要求完整运行验证

### Phase 5

- `goal`: 完成 tooling/tests/public surface 的结构性切换。
- `exit_criteria`:
  - `run test subject --id ...` 走新的 `validation-profile / validation / variant / matrix` 主轴
  - `goal` 只剩兼容层职责
  - `tests/` 与 `subjects/` 的职责边界在代码与测试中一致
- `deliverables`:
  - CLI 兼容与迁移方案
  - 通用 tests 与 subject-specific tests 切分方案
  - run/test tooling 改造清单
- `dependencies`:
  - Phase 4
- `open_questions`:
  - 哪些旧 selector 或 registry alias 需要保留一个过渡周期

### Phase 6

- `goal`: 关闭兼容收尾，并把长期知识统一沉淀到 wiki。
- `exit_criteria`:
  - 新目录与新语义已有稳定文档
  - `CHECK / PROFILE / SHIP` 宏定义写入 wiki
  - 本次结构调整的执行路径、产物骨架和兼容边界写入 wiki
- `deliverables`:
  - wiki 页面与索引更新清单
  - 兼容层退出建议
  - 最终迁移总结
- `dependencies`:
  - Phase 5
- `open_questions`:
  - wiki 页面应按“结构/执行/variant”拆分，还是按“操作路径/约束”拆分

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `phase-1-path-resolver-and-contract-samples-cutover` | `Phase 1` | `completed` | `codex` | 抽象路径真源，先切 resolver，再处理 `examples -> samples` 命名与 contract source 边界 | `-` |
| `phase-2-subject-root-and-manifest-cutover` | `Phase 2` | `completed` | `codex` | 把 `subjects/` 升级成正式 subject 根，并冻结 `validationProfiles + typed validation` manifest 模型 | `phase-1-path-resolver-and-contract-samples-cutover` |
| `phase-3-run-scoped-artifact-spine` | `Phase 3` | `completed` | `codex` | 把 canonical 产物和 run ledger 收敛到 `artifacts/subjects/<id>/runs/<run-id>` | `phase-2-subject-root-and-manifest-cutover` |
| `phase-4-variant-aware-codegen-and-native` | `Phase 4` | `in-progress` | `codex` | 引入 `CHECK / PROFILE / SHIP` 并贯通 codegen/native/report 语义 | `phase-3-run-scoped-artifact-spine` |
| `phase-5-tooling-tests-and-public-cutover` | `Phase 5` | `planned` | `codex` | 切换 CLI、tests 边界和 subject 内 validation 执行路径 | `phase-4-variant-aware-codegen-and-native` |
| `phase-6-compatibility-closure-and-wiki-handoff` | `Phase 6` | `planned` | `codex` | 收尾兼容层并把结构规则、流程骨架和宏定义沉淀到 wiki | `phase-5-tooling-tests-and-public-cutover` |

## 7. 依赖

全局依赖关系固定为：

1. 先冻结路径真源与 contract 样例语义
2. 再冻结 subject root 和 manifest
3. 再落 run-scoped artifact spine
4. 再引入 variant-aware codegen/native
5. 再切换 tooling 与 public entry
6. 最后做兼容清理与 wiki 沉淀

不能倒置的关键原因是：

- manifest 不稳定时，artifact 目录无法稳定
- artifact 目录不稳定时，variant 无法做到可追踪
- variant 语义不稳定时，CLI 和 wiki 都会反复返工

## 8. 风险

### 风险 1：`goal` 语义继续膨胀

如果实施阶段继续让 `goal` 同时承载 validation bundle、variant 与 matrix 选择，新的 manifest 和 CLI 会再次回到旧耦合状态。

### 风险 2：variant 被折进 matrix

如果为了省实现量把 `CHECK / PROFILE / SHIP` 折进 `matrix`，codegen 宏定义和 native 编译选项将失去显式追踪能力，后续 wiki 也无法清晰表达。

### 风险 3：run ledger 仍保留全局 canonical

如果继续让 `artifacts/logs/tests` 做 canonical run ledger，就会违背“subject 运行中间产物和日志都进 `artifacts/subjects`”的核心约束。

### 风险 4：tests 与 subjects 职责回混

如果把 subject 自己的 xUnit/perf harness 继续放回 `tests/`，那 `subjects/` 就无法成为真正的 subject 根。

## 9. 备选路径

### 方案 A：trace-first internal cutover

- 优点：最符合当前已拍板的结构和追踪目标
- 缺点：前几阶段对外表面的变化不大

### 方案 B：CLI-first public cutover

- 优点：外部可见性更高
- 缺点：在内部目录和 variant 语义未冻结前，极易返工

### 方案 C：variant-first implementation

- 优点：可以更快看到 `CHECK/PROFILE/SHIP`
- 缺点：如果没有稳定的 run/artifact spine，variant 产物无法真正可追踪

当前采用：

- **方案 A：trace-first internal cutover**

## 10. 当前建议推进顺序

1. 先派生 `Phase 1` child plan，专注路径 resolver 与 `contracts/*/samples` 命名收口。
2. `Phase 1` 完成后，再派生 `Phase 2` child plan，冻结 subject root 与 manifest schema。
3. `Phase 2` 完成后，再派生 `Phase 3` child plan，把 `artifacts/subjects/<id>/runs/<run-id>` 变成 canonical 产物根。
4. 只有在 artifact spine 稳定后，才派生 `Phase 4` child plan 去真正引入 `CHECK / PROFILE / SHIP`。
5. `Phase 4` 通过后，再派生 `Phase 5` child plan 去切 CLI、tests 边界和兼容层。
6. 最后以 `Phase 6` child plan 收尾 wiki 与兼容退出建议。

## 11. 结论

本任务已经具备稳定父 roadmap 的条件。下一步不应直接开始实现，而应先由用户审查本路线图；若确认无误，再从 `Phase 1` 派生首个 child plan。
