# Subject Stage D+E Reporting And Public Cutover 实现计划

> **面向执行 Agent：** 必须使用 `dev:subagent-driven-development`（如可用）或 `dev:executing-plans` 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 在 `HelloWorldObject` 现有 subject spine 之上补齐三层 reporting、三层 events，以及 subject registry / public CLI / TUI 的最小 cutover，同时保持 legacy `suite/session/watch` 的兼容边界不回退。

**架构：** 先把 `matrix report -> subject summary -> session summary` 这条对象链稳定下来，再把 canonical matrix / subject events 与 session compatibility projection 接上，最后让 subject 成为 registry 与 public command 的一等对象。整个计划继续限定在 `HelloWorldObject` 与既有四个 Windows matrix，不把 `Stage F` 的 perf、多 subject 和测试工程长期基线整理提前并入。

**技术栈：** Python 3、JSON manifests / summaries / events、`unittest`、现有 `run` manifest / TUI / registry 基础设施

**设计文档：** `docs/dev/in-progress/20260406-08-subject-stage-d-e-reporting-and-public-cutover/design-v1-01.md`

**预期知识沉淀：** `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/subject-test-framework-v1/reporting-and-public-cutover-v1.md`，并按执行结果回写 `wiki/06-测试验证/`

---

## 目标文件与职责

- 新增：`build/toolchains/run/testing/subject_reporting.py`
  - 负责 matrix report、subject summary 与 subject aggregate event payload 的组装。
- 修改：`build/toolchains/run/testing/events.py`
  - 把事件 envelope 扩展为支持 `streamScope / subjectId / matrixId / goalId / stageId / bucket / stageScope`。
- 修改：`build/toolchains/run/testing/subject_executor.py`
  - 在 stage 生命周期上发出 matrix canonical events，并把最终结果交给 `subject_reporting.py`。
- 按需修改：`build/toolchains/run/testing/subjects.py`
  - 暴露 subject report / events 路径与 public descriptor 需要的 helper。
- 修改：`build/toolchains/run/testing/reporting.py`
  - 在保留 legacy suite/session summary 的前提下，增量写入 `subjectStatusCounts` 与 `subjectResults`。
- 修改：`build/toolchains/run/commands/test.py`
  - 增加 subject-aware public dispatch，并让 `test summary` / `test watch` / final-summary payload 能读到新字段。
- 修改：`build/toolchains/run/testing/registry.py`
  - 新增 `subject` object type 与对应扫描 / 展平 / 执行展开逻辑。
- 按需新增：`build/toolchains/run/testing/subject_registry.py`
  - 如 `registry.py` 直接扩展会过重，则把 `subjects/*/subject.manifest.json` 到 registry object 的映射下沉到该模块。
- 修改：`build/toolchains/run/manifest.py`
  - 支持新的 `test subject --id ...` 解析入口。
- 修改：`build/toolchains/run/run_manifest.json`
  - 注册 `test-subject` 命令与说明文本。
- 修改：`build/toolchains/run/tui.py`
  - 让 TUI test menu、registry route 与 summary/watch 导航能够识别 subject 结果。
- 新增：`tests/unit/run/test_subject_reporting.py`
  - 锁 `report.json` / `subject-report/summary.json` / subject aggregate payload 的最小 schema。
- 修改：`tests/unit/run/test_events_schema.py`
  - 锁扩展后的 event envelope 仍保持 additive compatibility。
- 修改：`tests/unit/run/test_subject_executor.py`
  - 锁 executor 会产出 matrix canonical events 与 enriched matrix report。
- 按需新增：`tests/unit/run/test_subject_registry.py`
  - 锁 `subject` registry object 的扫描与 command shape。
- 按需新增：`tests/tooling/run/test_subject_command.py`
  - 锁 `run test subject --id HelloWorldObject` 的 public dispatch 与失败提示。
- 修改：`tests/integration/run/test_reporting_layout.py`
  - 锁 session summary 增量字段与 subject summary 路径。
- 修改：`tests/tooling/run/test_test_watch_summary.py`
  - 锁 `test summary` / `test watch` 能显示 `subjectResults` 但不要求把它们伪装成 `suiteResults`。
- 修改：`tests/integration/run/test_registry_scan.py`
  - 锁 registry snapshot 中的 `subject` collection 与 `flatItems`。
- 修改：`tests/tooling/run/test_registry_command.py`
  - 锁 registry list / object lookup / structured hint 对新的 `subject` object type 生效。
- 修改：`tests/tooling/run/test_command_manifest.py`
  - 锁新的 `test subject` manifest entry。
- 修改：`tests/tooling/run/test_tui.py`
  - 锁 TUI menu / routing / summary links 对 subject 结果保持可导航。
- 修改：`docs/dev/in-progress/20260406-08-subject-stage-d-e-reporting-and-public-cutover/STATUS.md`
  - 执行阶段维护 phase、摘要、风险与下一步。
- 修改：`docs/dev/ACTIVE.md`、`notes/progress-*.md` 与索引
  - 当真正进入执行时按执行技能要求维护 active 与进度。

## 执行原则

- [ ] 继续把范围固定在 `HelloWorldObject` 与既有四个 Windows matrix。
- [ ] 不把 `subjectResults` 映射回旧 `suiteResults` 作为正式真源。
- [ ] session `progress` / `final-summary` 保持 compatibility projection；不要提前推翻 `test watch` / TUI 现有消费契约。
- [ ] matrix report 是诊断主入口；subject / session summary 只做聚合与导航。
- [ ] 事件文件坚持单写者原则；worker 不直接 append `events.jsonl`。
- [ ] 不把 `perf`、多 subject、测试工程长期基线整理带进本计划。
- [ ] 每完成一个 chunk，都更新本任务 `STATUS.md`、`notes/progress-*.md`、索引与 `docs/dev/ACTIVE.md`（若已进入执行态）。

## Chunk 1：冻结 subject reporting 对象与失败测试

- [ ] 先写 `tests/unit/run/test_subject_reporting.py`，锁以下对象最小字段：
  - `matrices/<matrix-id>/report.json`
  - `subject-report/summary.json`
  - 供 session summary / event payload 复用的 subject aggregate object
- [ ] 明确 matrix report 至少保留：
  - `reportVersion`
  - `runId`
  - `subjectId`
  - `matrixId`
  - `goalId`
  - `selection`
  - `status`
  - `terminalStageId`
  - `terminalBucket`
  - `stageResults`
  - `artifactResults`
  - `errors`
- [ ] 明确 subject summary 至少保留：
  - `summaryVersion`
  - `runId`
  - `subjectId`
  - `requestedGoalId`
  - `status`
  - `matrixStatusCounts`
  - `matrixResults`
- [ ] 新增 `build/toolchains/run/testing/subject_reporting.py` 最小骨架，通过 RED -> GREEN 锁定对象组装边界。

**验证：**

- [ ] `python -m unittest tests.unit.run.test_subject_reporting`

## Chunk 2：扩展 canonical event envelope 与 matrix event 写入

- [ ] 先扩 `tests/unit/run/test_events_schema.py`，锁 `build_event()` 对旧字段兼容，并允许新增：
  - `streamScope`
  - `subjectId`
  - `matrixId`
  - `goalId`
  - `stageId`
  - `bucket`
  - `stageScope`
- [ ] 修改 `build/toolchains/run/testing/events.py`
  - 保持 `suiteId / stage` 可选兼容字段
  - 增量支持 subject/matrix canonical envelope
- [ ] 修改 `build/toolchains/run/testing/subject_executor.py`
  - 在 stage start / reuse / invalidation / finish 时产出 matrix canonical events
  - 最终把 matrix result 交给 `subject_reporting.py`
- [ ] 不让 worker 自己写事件文件。

**验证：**

- [ ] `python -m unittest tests.unit.run.test_events_schema`
- [ ] `python -m unittest tests.unit.run.test_subject_executor`

## Chunk 3：补齐 subject summary 与 session compatibility bridge

- [ ] 先扩 `tests/integration/run/test_reporting_layout.py`
  - 锁 session summary 增量字段：
    - `subjectStatusCounts`
    - `subjectResults`
  - 锁 `final-summary` payload 同样能带出 subject 聚合入口
- [ ] 修改 `build/toolchains/run/testing/reporting.py`
  - 继续保留 legacy `suiteResults`
  - 新增 `subjectStatusCounts` 与 `subjectResults`
  - 不把 `subject` 强塞回 `suiteResults`
- [ ] 修改 `build/toolchains/run/commands/test.py`
  - 让 final-summary/session summary 可接纳 subject aggregate payload
  - 为后续 `test subject` dispatch 预留统一 run context
- [ ] 先扩 `tests/tooling/run/test_test_watch_summary.py`
  - 锁 `test summary` / `test watch` 能展示 subject 结果概览
  - 保持旧 `Phases:` / `suiteResults` 展示不回退

**验证：**

- [ ] `python -m unittest tests.integration.run.test_reporting_layout`
- [ ] `python -m unittest tests.tooling.run.test_test_watch_summary`

## Chunk 4：引入 subject registry object 与 public command

- [ ] 先写 `tests/integration/run/test_registry_scan.py` 与 `tests/tooling/run/test_registry_command.py` 的失败断言，锁新增：
  - `subject` collection
  - `subject/<subject-id>` object id
  - subject object 的 `canonicalCommand`
- [ ] 修改 `build/toolchains/run/testing/registry.py`
  - 把 subject 纳入 `RegistryIndex`
  - 扩展 `flatItems`、`find_registry_object()`、`expand_execution_plan()`
- [ ] 如 `registry.py` 过重，则新增 `build/toolchains/run/testing/subject_registry.py`，专门做 manifest -> registry object 的映射
- [ ] 修改 `build/toolchains/run/run_manifest.json` 与 `build/toolchains/run/manifest.py`
  - 注册并解析 `test subject --id <subject-id>`
- [ ] 修改 `build/toolchains/run/commands/test.py`
  - 新增 subject dispatch，内部调用 `subject_planner` / `subject_executor`
  - 统一接 session reporting / event bridge

**验证：**

- [ ] `python -m unittest tests.integration.run.test_registry_scan`
- [ ] `python -m unittest tests.tooling.run.test_registry_command`
- [ ] `python -m unittest tests.tooling.run.test_command_manifest`

## Chunk 5：完成 TUI / public navigation 最小 cutover

- [ ] 先扩 `tests/tooling/run/test_tui.py`
  - 锁 subject 结果的 summary/report 链接可导航
  - 锁 test menu / registry route 能识别 subject command 或 subject registry object
- [ ] 修改 `build/toolchains/run/tui.py`
  - 保持现有 watch / summary / registry 菜单不回退
  - 为 subject public entry 补最小导航入口
- [ ] 如有需要，扩 `build/toolchains/run/commands/test.py` 的文本渲染，让 subject 结果在 summary/watch 文本里可读但不喧宾夺主。

**验证：**

- [ ] `python -m unittest tests.tooling.run.test_tui`

## Chunk 6：HelloWorldObject public cutover 收口与非回退验证

- [ ] 为 `HelloWorldObject` subject 补最小 public happy-path 测试：
  - `run test subject --id HelloWorldObject`
  - registry list 能看到对应 subject object
  - session summary / final-summary 都能指向 `subject-report/summary.json`
- [ ] 继续锁 `tests.unit.run.test_helloworld_subject_windows_cutover`
  - 确保本轮 public/reporting 改动没有破坏 Stage C 已冻结的 Windows matrix contract
- [ ] 如环境允许，重跑 Windows `verify-roadmap-0` compatibility gate，确认 Stage D+E 没有回退现有 subject spine。

**验证：**

- [ ] `python -m unittest tests.unit.run.test_helloworld_subject_windows_cutover`
- [ ] `python build/scripts/verify-roadmap-0.py windows`

## 关键验证命令

- [ ] `python -m unittest tests.unit.run.test_subject_reporting`
- [ ] `python -m unittest tests.unit.run.test_events_schema`
- [ ] `python -m unittest tests.unit.run.test_subject_executor`
- [ ] `python -m unittest tests.integration.run.test_reporting_layout`
- [ ] `python -m unittest tests.tooling.run.test_test_watch_summary`
- [ ] `python -m unittest tests.integration.run.test_registry_scan`
- [ ] `python -m unittest tests.tooling.run.test_registry_command`
- [ ] `python -m unittest tests.tooling.run.test_command_manifest`
- [ ] `python -m unittest tests.tooling.run.test_tui`
- [ ] `python -m unittest tests.unit.run.test_helloworld_subject_windows_cutover`
- [ ] 如环境允许：`python build/scripts/verify-roadmap-0.py windows`

## 完成标准

- [ ] `HelloWorldObject` 的四个 Windows matrix 都能产出稳定的 `report.json`。
- [ ] `subject-report/summary.json` 成为 subject 聚合主入口。
- [ ] session `summary.json` 与 `final-summary` 已增量接入 `subjectStatusCounts` / `subjectResults`，但没有污染 `suiteResults`。
- [ ] matrix / subject canonical events 已落地，session events 仍保持 watch/TUI 兼容投影。
- [ ] subject 已成为 registry 与 public command 的一等对象。
- [ ] TUI / `test summary` / `test watch` 能导航到新的 subject 结果。
- [ ] `Stage F` 的 perf、多 subject 与长期基线整理仍未被提前并入。

## 执行后交接

计划执行完成后，执行阶段应优先：

- [ ] 把长期边界写入 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/subject-test-framework-v1/reporting-and-public-cutover-v1.md`
- [ ] 视 public surface 的最终形态回写 `wiki/06-测试验证/`
