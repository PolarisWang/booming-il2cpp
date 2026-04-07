# Subject Stage D+E Reporting And Public Cutover 设计 v1.01

Date: 2026-04-06
Status: plan-ready

## 1. 目标

在现有 `HelloWorldObject` subject spine 之上补齐：

- `matrix report / subject summary / session summary`
- `matrix events / subject events / session events`
- subject registry / public CLI / TUI 的最小切换

并保持以下原则不回退：

- matrix 仍是诊断主入口
- subject / session 仍是导航聚合层
- legacy `suite/session/watch` 仍可用，但只作为 compatibility surface

## 2. 非目标

本设计不包含：

- `perf.dev / perf.release`
- 多 subject onboarding
- 清理或删除现有测试工程
- 把 legacy `suiteResults` 变成新 subject 结果的正式真源

这些都继续归 `Stage F`。

## 3. 已知现状

### 3.1 已存在的 subject spine

- `build/toolchains/run/testing/subjects.py`
  - 已能读取 `subject.manifest.json`，并固定 artifact roots / bucket manifest 路径
- `build/toolchains/run/testing/subject_planner.py`
  - 已能产出 `selection / artifactsRoot / stagePlan`
- `build/toolchains/run/testing/subject_executor.py`
  - 已能执行 stage worker，并写出最小 `report.json`

### 3.2 仍然偏 legacy 的 public/test surface

- `build/toolchains/run/testing/reporting.py`
  - 当前只面向 legacy suite/session
- `build/toolchains/run/testing/events.py`
  - 当前 envelope 仍以 `suiteId / stage` 为中心
- `build/toolchains/run/testing/registry.py`
  - 当前 registry 只有 `suite/module/system/pipeline`
- `build/toolchains/run/commands/test.py`
  - 当前 public dispatch 仍以 legacy suite spec 和 registry object 为主
- `build/toolchains/run/tui.py`
  - 当前 watch / summary / registry 入口都假设 session 事件流仍是旧语义

## 4. 设计决定

## 4.1 Reporting 采用 `B2`

直接采用 `docs/discuss/reporting/20260406-20-reporting-schema-design-v1-01.md` 的结论：

- `matrices/<matrix-id>/report.json`
  - 是单个 matrix 的诊断主入口
  - 保留 `selection`、`stageResults`、`artifactResults`、`errors`
- `subject-report/summary.json`
  - 只做 subject 聚合与导航
- `artifacts/logs/tests/<run-id>/summary.json`
  - 保留现有 session summary 语义
  - 增量加入 `subjectStatusCounts` 与 `subjectResults`

## 4.2 Events 采用方案 `C`

直接采用 `docs/discuss/reporting/20260406-21-event-schema-alignment-design-v1-01.md` 的结论：

- matrix events
  - canonical detailed stream
- subject events
  - canonical aggregate stream
- session events
  - compatibility projection

### 单写者原则

- matrix / subject events
  - 由 subject execution path 写
- session events
  - 由 public command orchestration 写

worker 不直接 append `events.jsonl`。

## 4.3 Public cutover 以 subject 为一等对象，但不抹掉 legacy surface

Stage E 的第一版不是“删除旧入口”，而是：

- 给 registry 增加 `subject` object type
- 给 public command 增加 `test subject --id <subject-id>`
- 让 registry list / manifest / TUI 能发现并执行 subject
- 同时保留 legacy suite / watch / summary / registry 的兼容路径

这意味着：

- `subject` 是新的一等执行对象
- legacy suite 仍可作为 alias / compatibility path 存在
- `suiteResults` 不承载 `subject` 的正式真相

## 5. 模块边界

## 5.1 `subject_reporting.py`

新增一个独立模块，负责：

- 把 executor 的 `matrix execution result` 写成 `report.json`
- 聚合同一 subject 的 `subject-report/summary.json`
- 统一返回可写入 event / session summary 的轻量对象

这样可以避免：

- 把 legacy `reporting.py` 继续膨胀成同时承载 old suite 和 new subject 的双头实现

## 5.2 `events.py`

保留 `build_event()` 作为统一入口，但做 additive 扩展：

- 增加 `streamScope`
- 增加 `subjectId`
- 增加 `matrixId`
- 增加 `goalId`
- 增加 `stageId`
- 增加 `bucket`
- 增加 `stageScope`

legacy `suiteId / stage` 继续保留，供旧消费方兼容读取。

## 5.3 `subject_executor.py`

职责扩展为：

- 仍负责 stage 执行
- 在 stage 生命周期上生成 matrix canonical events
- 在 `report-assemble` 之后把结果交给 `subject_reporting.py`

它不负责：

- 直接写 session summary
- 直接承担 TUI / watch 兼容投影

## 5.4 `reporting.py`

继续只做 run/session 层，但扩展为可接受 `subjectResults`：

- `summary.json`
  - 增量写入 `subjectStatusCounts` 与 `subjectResults`
- `final-summary`
  - 增量带出 `subjectResults`
- 不把 `subjectResults` 回灌成 `suiteResults`

## 5.5 `registry.py` 与 public command

`registry.py` 需要新增 `subject` collection，并把它们纳入：

- `flatItems`
- `find_registry_object()`
- `expand_execution_plan()`

public command 层需要新增：

- `run test subject --id <subject-id>`

并让：

- `run test registry list`
- `run test registry check-consistency`
- TUI test menu / registry route

都能识别新的 `subject` object type。

## 6. 范围收敛

本轮 public cutover 继续绑定到：

- `HelloWorldObject`
- 四个既有 Windows matrix

这保证：

- registry / CLI / TUI 的第一版 subject public entry 有真实实现对象
- 不会把多 subject 选择、默认排序、跨 subject 汇总等 `Stage F` 议题提前带进来

## 7. 预期长期沉淀

当本计划未来执行闭环后，应把长期边界回写到：

- `docs/architecture/subject-test-framework-v1/reporting-and-public-cutover-v1.md`

并按实际 public surface 更新：

- `wiki/06-测试验证/`

## 8. 结论

`Stage D + Stage E` 的实现主线已经可以固定为：

1. 先补 subject reporting 与 canonical event
2. 再把 session summary / watch / TUI 接成兼容投影
3. 最后把 subject 公开为 registry / public command / TUI 的一等对象

只要保持这个顺序，就不会为了“先看到 public 入口”而把 reporting/session 语义做坏。
