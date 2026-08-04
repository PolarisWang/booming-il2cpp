# Foundation DLL Capability Closure Reporting Roadmap v1.01

Date: 2026-04-25 22:30:00 +08:00
Status: in-progress

## 1. 目标

把 foundation DLL 报告从当前的 `Program -> DLL -> Verification Project -> Artifact` 流程导向视图，升级为适合审核者判断真实完成度的双轴报告模型：

- 主轴: `Capability Closure`
- 辅轴: `Workflow Progress`

最终目标：

- foundation DLL 总报告能显示每个 DLL 的真实 capability closure 进度，而不是仅显示 project gate 通过数
- 每个 DLL 页都能显示 capability family 细分、family 对应 gate 状态、源码入口、生成代码入口、证据入口和 waiver / exclusion 记录
- `completion-certification` 成为 DLL 级汇总 gate，不再污染 capability family 的 workflow denominator
- `docs/**` 只发布 HTML 审核视图和必要 JSON；低价值 `summary.md` 退出 docs 发布面

## 2. 范围边界

本 roadmap 负责：

- 定义 foundation DLL 报告的正式 denominator authority
- 定义 `Capability Closure` 与 `Workflow Progress` 的正式含义和计算口径
- 定义 DLL completed 的判定规则与 waiver authority
- 定义 DLL 页 `Source Links` 的正式对象边界
- 定义 HTML 报告、projection、archive 三层产物的职责分层

本 roadmap 不负责：

- 在本 roadmap 文档中直接实现新的 reporting generator
- 在本轮内重新定义 IL2CPP 核心能力本体的实现策略
- 把 testcase count 强行接成新的主进度分母
- 改写 `20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening` 的 widening 目标或当前执行优先级

## 3. 非目标

- 不再把 verification project 通过数当作 DLL 真实完成度
- 不把 method count 直接当作 capability closure 分母
- 不把 task `STATUS.md` 当作 waiver 的长期 authority
- 不把 `docs/**`、`subjects/**`、`verification/**` 混入 primary evidence

## 4. 阶段列表

### Phase 0: Capability Denominator And Authority Freeze

- goal:
  - 冻结 foundation DLL capability closure 的正式 denominator authority 和 family 粒度
- exit_criteria:
  - `capability-family ledger` 的 authority 关系被文档化
  - capability family 的主粒度固定为语义 family
  - implementation family 和 source grouping 的从属关系固定
- deliverables:
  - `capability-family ledger` schema/design
  - denominator authority contract
  - family granularity examples
- dependencies:
  - `verification/catalog/programs/foundation-dll-translation-audit.program.json`
  - current audit input / surface / semantic / nativeization artifacts
- resolved_decisions:
  - 主进度条 = capability closure
  - 原始 authority 不直接等于报告 authority
- watch_items:
  - family 粒度过细会退化为 method count
  - family 粒度过粗会掩盖高风险能力空洞

### Phase 1: Workflow Progress And Completion Rule Freeze

- goal:
  - 冻结 workflow denominator、DLL completed 规则和 waiver authority
- exit_criteria:
  - family 级 `requiredGates` 口径固定
  - `completion-certification` 被固定为 DLL 级 gate
  - waiver / exclusion / platform-blocked 的 authority 和最小字段固定
- deliverables:
  - workflow gate contract
  - DLL completed contract
  - waiver contract
- dependencies:
  - Phase 0
- resolved_decisions:
  - `Workflow Progress` 是辅助流程条，不代表 capability 覆盖本身
  - `completion-certification` 不进入 family denominator
- watch_items:
  - hotupdate / benchmark obligation 需要按 family 裁剪，不能 DLL 一刀切

### Phase 2: Capability-Family Ledger And Projection Contract Cutover

- goal:
  - 把新的 capability-family 数据模型接入 formal projection / docs projection contract
- exit_criteria:
  - program / DLL / capability family / source refs / evidence / waivers 的正式字段固定
  - projection contract 与 report consumer 一致
  - `program.json` 不再出现 dashboard 需要但 payload 不提供的空字段
- deliverables:
  - updated projection schema
  - updated generator contract
  - updated JSON payload examples
- dependencies:
  - Phase 1
- resolved_decisions:
  - 报告 authority 由 projection payload 明示，而不是 HTML 模板隐式推断
- watch_items:
  - 不能破坏现有 `artifacts/**` primary evidence 规则

### Phase 3: Report UX Split And DLL Source Links Cutover

- goal:
  - 重构 HTML 报告信息架构，使总报告看全局进度，DLL 页看 capability 细节和源码入口
- exit_criteria:
  - 总报告同时显示 `DLL Progress / Capability Closure / Workflow Progress`
  - DLL 页显示 capability matrix、source links、evidence、waivers
  - Artifact Index 从总 dashboard 主路径拆分到独立页面或次级入口
- deliverables:
  - updated dashboard information architecture
  - DLL detail page information architecture
  - source-links rendering rules
- dependencies:
  - Phase 2
- resolved_decisions:
  - `docs/**` 面向审核者，主入口只保留 HTML
- watch_items:
  - 不要重新把 artifact dump 堆回 dashboard 主视觉

### Phase 4: Archive / Wiki / Spec Sync And Formal Refresh

- goal:
  - 完成 reporting governance 的 authority 同步和正式 refresh
- exit_criteria:
  - `docs/archive/architecture/verification-v1/spec.md` 更新
  - `wiki/06-测试验证/INDEX.md` 同步新口径
  - `verification-v1` formal refresh 通过
  - docs/projection/archive 三层产物一致
- deliverables:
  - updated architecture/wiki docs
  - refreshed `verification-v1` artifacts
  - final report entry updates
- dependencies:
  - Phase 3
- resolved_decisions:
  - `docs/**` 下不再发布低价值 `summary.md`
- watch_items:
  - wiki 和 skill 规则必须同步，否则长期 authority 会分裂

## 5. 每阶段完成定义

每个阶段只有满足以下条件才算完成：

- 阶段 scope 已冻结
- 对应 authority / schema / projection / UI 规则已文档化
- 没有把新的 open question 留给下游 child 再拍板
- 如触及 projection / report contract，后续 child 必须包含正式 refresh 计划

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260425-04-capability-family-denominator-and-authority-freeze` | `phase-0` | `completed` | `codex` | 冻结 capability closure 的 denominator authority、family 粒度与 authority layering | `20260425-03` |
| `20260425-05-workflow-progress-and-completion-rules-freeze` | `phase-1` | `completed` | `codex` | 冻结 workflow required gates、DLL completed 规则和 waiver authority | `20260425-04` |
| `20260425-06-capability-family-ledger-and-report-contract-cutover` | `phase-2` | `completed` | `codex` | 把 capability-family ledger 接入 foundation DLL projection / report contract | `20260425-05` |
| `20260425-07-dll-report-pages-source-links-and-artifact-split` | `phase-3` | `completed` | `codex` | 重构 dashboard / DLL detail / artifact entry，并补 Source Links | `20260425-06` |
| `20260425-08-reporting-authority-sync-and-formal-refresh` | `phase-4` | `completed` | `codex` | 同步 architecture/wiki/skill authority，并执行 formal refresh | `20260425-07` |

## 7. 依赖

- `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/STATUS.md`
- `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/roadmap-v1-01.md`
- `verification/catalog/programs/foundation-dll-translation-audit.program.json`
- `build/toolchains/run/testing/foundation_dll_audit_generator.py`
- `docs/archive/architecture/verification-v1/spec.md`
- `wiki/06-测试验证/INDEX.md`
- `artifacts/subjects/SolutionCorePack/runs/*/analysis/generated/*coverage.json`

## 8. 风险

- 如果 capability family 分母没有独立 authority，报告字段会继续围绕模板实现漂移
- 如果 workflow progress 继续和 capability closure 混用，审核者仍然无法判断真实完成度
- 如果 waiver authority 仍散落在 task docs，DLL completed 会长期不可审计
- 如果 docs 继续暴露 `summary.md`，审核入口会继续重复和分叉

## 9. 备选路径

### 路径 A: 继续用 Verification Project 通过数作为主进度条

- 不推荐
- 原因: 只能表达流程推进，不能表达 DLL 能力闭包

### 路径 B: 直接用 method count / coverage count 作为主进度条

- 不推荐
- 原因: 容易被 overload、平台方法和噪音方法数误导

### 路径 C: 主进度按 capability family，流程进度按 required gates

- 推荐
- 原因: 既能表达真实能力覆盖，又能表达 formal verification 链走到哪一步

## 10. 当前建议推进顺序

1. 先创建 `20260425-04-capability-family-denominator-and-authority-freeze`
2. 再创建 `20260425-05-workflow-progress-and-completion-rules-freeze`
3. authority 和 denominator 冻结后，再推进 projection / report contract cutover
4. 最后再做 HTML 页面重构、wiki/spec 同步和 formal refresh

## 11. 子任务执行策略

- `child_execution_mode: manual`
- `auto_continue: false`
- `auto_stop_policy: manual-review`
- 当前 `foundation-dll-translation-audit-roadmap` 已有 canonical active child `20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening`
- 本 reporting governance roadmap 是并行支持主线，不应在没有额外用户确认的情况下自动抢占当前 widening 主线
- `recommended_next_child`: `20260425-04-capability-family-denominator-and-authority-freeze`
