# Subject Stage F Perf And Multi-Subject Expansion 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 接入第二个正式 subject `GenericEcho`，并在 subject 模型下落地 `perf.dev` / `perf.release`、subject-aware perf baseline，以及第二个 subject onboarding pattern。

**架构：** 先把 `GenericEcho` 的 canonical source 与 benchmark subject manifest 固定下来，再引入不依赖 `HelloWorldObject` native proof 特例路径的 managed perf pipeline，随后补齐 perf baseline / report / public surface，最后把测试工程长期基线整理规则沉淀到文档与 wiki。

**技术栈：** Python 3、.NET 8、JSON manifests / summaries / reports、`unittest`

**设计文档：** `docs/dev/in-progress/20260407-01-subject-stage-f-perf-and-multi-subject-expansion/design-v1-01.md`

**预期知识沉淀：** `docs/architecture/subject-test-framework-v1/perf-and-multi-subject-expansion-v1.md`，并按执行结果更新 `wiki/06-测试验证/`

---

## Scope Correction

- This plan started as "GenericEcho is the second formal subject".
- The user then clarified a broader migration rule: every project under `tests/smoke/input/` should migrate to `subjects/`.
- Completed migration set for this correction:
- `HelloWorld`
- `GenericEcho`
- `ReflectionLite`
- `PInvokeLite`
- `HostEmbeddingLite`
- Legacy smoke suites and `run_manifest.json` entries now resolve to `subjects/*/source`.
- The old `tests/smoke/input/*` directories are intentionally still present for compatibility and baseline history; they are no longer the active source path.
- The remaining perf/reporting items in this plan should now be treated as follow-up work after the smoke-input migration correction.

## 目标文件与职责

- 新增：`subjects/GenericEcho/subject.manifest.json`
  - 第二个正式 subject 的 benchmark manifest。
- 新增：`subjects/GenericEcho/source/`
  - `GenericEcho` 的 canonical source tree。
- 修改：`build/toolchains/run/testing/subjects.py`
  - 支持 `perf.release` 附件目录与新 pipeline kind 的路径约定。
- 修改：`build/toolchains/run/testing/subject_workers.py`
  - 新增 managed perf worker：
    - `runtime-perf-collect`
    - `perf-evaluate`
- 修改：`build/toolchains/run/testing/perf.py`
  - 支持 subject-aware perf baseline 路径。
- 修改：`build/toolchains/run/testing/subject_reporting.py`
  - 扩展 perf matrix report / release report 附件索引。
- 修改：`build/toolchains/run/testing/registry.py`
  - 扫描并暴露 `subject/GenericEcho`。
- 修改：`build/toolchains/run/commands/test.py`
  - 让 `run test subject --id subject/GenericEcho --goal perf.dev|perf.release` 走新 pipeline。
- 修改：`tests/unit/run/test_subject_manifest_schema.py`
  - 锁第二个 subject manifest 的发现与 schema。
- 修改：`tests/unit/run/test_subject_planner.py`
  - 锁 `GenericEcho` 的 perf goal / matrix 规划。
- 新增：`tests/unit/run/test_subject_perf_policy.py`
  - 锁 subject-aware baseline 规则。
- 新增：`tests/unit/run/test_subject_workers_perf.py`
  - 锁 managed perf worker 的 RED/GREEN 契约。
- 修改：`tests/integration/run/test_registry_scan.py`
  - 锁 `subject/GenericEcho` 出现在 registry。
- 修改：`tests/tooling/run/test_subject_command.py`
  - 锁 `GenericEcho` 的 perf subject dispatch。
- 修改：`tests/tooling/run/test_test_watch_summary.py`
  - 锁 perf subject 结果在 summary/watch 的展示边界。
- 修改：`tests/tooling/run/test_tui.py`
  - 锁 benchmark subject 与 perf goal 的导航。
- 修改：`docs/dev/in-progress/20260407-01-subject-stage-f-perf-and-multi-subject-expansion/STATUS.md`
  - 维护执行阶段状态、风险与下一步。
- 修改：`docs/dev/ACTIVE.md`
  - 把活动入口切到本 child plan。
- 修改：`docs/dev/in-progress/20260406-03-subject-test-framework-rollout-roadmap/STATUS.md`
  - 记录 `Stage F` child plan 已派生并进入执行。
- 修改：`docs/dev/in-progress/20260406-03-subject-test-framework-rollout-roadmap/roadmap-v1-01.md`
  - 将 `Stage F` 从 queued-later-plan 更新为当前 child plan。
- 新增或修改：`notes/progress-*.md`
  - 每完成一个 chunk 记录验证、风险与 wiki 决策。

## 执行原则

- [ ] 本轮只固定一个新增 subject：`GenericEcho`
- [ ] 本轮只固定一个新增 host：`windows`
- [ ] 本轮不删除现有测试工程
- [ ] `subjects/GenericEcho/source/` 必须成为 `GenericEcho` 的正式 canonical source
- [ ] perf baseline 继续保留在 repo 内正式测试资产里，不写进 `artifacts/`
- [ ] 不把 `HelloWorldObject` 的 native proof worker 特例路径误当成第二个 subject 的默认模板
- [ ] 每完成一个 chunk，都更新本任务 `STATUS.md`、`notes/progress-*.md`、`docs/dev/ACTIVE.md` 与索引

## Chunk 1：接入第二个正式 subject `GenericEcho`

- [ ] 先改 `tests/unit/run/test_subject_manifest_schema.py`，锁第二个 subject 的发现与关键字段
- [ ] 先改 `tests/unit/run/test_subject_planner.py`，锁 `GenericEcho` 的 `perf.dev` 默认 goal 与 perf matrix 规划
- [ ] 新建 `subjects/GenericEcho/subject.manifest.json`
- [ ] 新建 `subjects/GenericEcho/source/`
- [ ] 保持 `tests/smoke/input/GenericEcho/` 不删除，只作为 compatibility / baseline 资产保留

**验证：**

- [ ] `python -m unittest tests.unit.run.test_subject_manifest_schema tests.unit.run.test_subject_planner`

## Chunk 2：引入 managed perf pipeline 与 worker 契约

- [ ] 先写 `tests/unit/run/test_subject_workers_perf.py`，锁 `runtime-perf-collect` / `perf-evaluate` 的失败测试
- [ ] 让 `GenericEcho` 使用新的 managed perf pipeline，而不是 `HelloWorldObject` native proof pipeline
- [ ] 在 `subject_workers.py` 中实现最小 worker：
  - `runtime-perf-collect`
  - `perf-evaluate`
- [ ] 让 runtime perf 收集最少包含：
  - `samples`
  - `summaryMetrics`
  - `stdout/stderr`

**验证：**

- [ ] `python -m unittest tests.unit.run.test_subject_workers_perf`

## Chunk 3：引入 subject-aware perf baseline

- [ ] 先写 `tests/unit/run/test_subject_perf_policy.py`，锁 baseline 路径：
  - `tests/perf/subjects/GenericEcho/<matrix-id>/baselines/<host>.json`
- [ ] 扩展 `build/toolchains/run/testing/perf.py`
- [ ] 保持旧 suite perf baseline helper 兼容，不破坏已有 `tests/unit/run/test_perf_policy.py`

**验证：**

- [ ] `python -m unittest tests.unit.run.test_perf_policy tests.unit.run.test_subject_perf_policy`

## Chunk 4：扩展 perf matrix report 与 `perf.release` 附件目录

- [ ] 先扩 `tests/unit/run/test_subject_reporting.py`
- [ ] 让 perf matrix report 能带出：
  - `metrics`
  - `baseline`
  - `regressionStatus`
  - `releaseReportPaths`
- [ ] 若 `perf.release` 需要附件，固定写到：
  - `artifacts/subjects/<subject-id>/matrices/<matrix-id>/report/`
- [ ] 继续保留 `report.json` 作为 matrix machine-first 主入口

**验证：**

- [ ] `python -m unittest tests.unit.run.test_subject_reporting`

## Chunk 5：接通 registry / command / summary / TUI

- [ ] 先改 `tests/integration/run/test_registry_scan.py`
- [ ] 先改 `tests/tooling/run/test_subject_command.py`
- [ ] 先改 `tests/tooling/run/test_test_watch_summary.py`
- [ ] 先改 `tests/tooling/run/test_tui.py`
- [ ] 让 `subject/GenericEcho` 成为 registry 一等对象
- [ ] 让 `run test subject --id subject/GenericEcho --goal perf.dev|perf.release` 走新 pipeline
- [ ] 让 summary/watch/TUI 能看到第二个 subject 与 perf goal 结果

**验证：**

- [ ] `python -m unittest tests.integration.run.test_registry_scan tests.tooling.run.test_subject_command tests.tooling.run.test_test_watch_summary tests.tooling.run.test_tui`

## Chunk 6：文档、长期知识与长期基线整理规则收口

- [ ] 更新本任务 `STATUS.md` 与 `notes/progress-*.md`
- [ ] 更新父 roadmap `STATUS.md` / `roadmap-v1-01.md`
- [ ] 新增 `docs/architecture/subject-test-framework-v1/perf-and-multi-subject-expansion-v1.md`
- [ ] 若形成长期规则，回写 `wiki/06-测试验证/`
- [ ] 明确记录：旧 `tests/smoke/input/GenericEcho/` 与其他现有测试工程都保留为长期基线资产，不删除

## 关键验证命令

- [ ] `python -m unittest tests.unit.run.test_subject_manifest_schema tests.unit.run.test_subject_planner`
- [ ] `python -m unittest tests.unit.run.test_subject_workers_perf`
- [ ] `python -m unittest tests.unit.run.test_perf_policy tests.unit.run.test_subject_perf_policy`
- [ ] `python -m unittest tests.unit.run.test_subject_reporting`
- [ ] `python -m unittest tests.integration.run.test_registry_scan tests.tooling.run.test_subject_command tests.tooling.run.test_test_watch_summary tests.tooling.run.test_tui`

## 完成标准

- [ ] `GenericEcho` 已成为第二个正式 subject
- [ ] `perf.dev` / `perf.release` 已在 subject 模型下稳定落地
- [ ] subject-aware perf baseline 路径固定且受测试保护
- [ ] 第二个 subject onboarding pattern 已被代码与文档共同固定
- [ ] 测试工程长期基线整理规则已经收口为“全部保留、结构收敛”，没有删除现有工程

## 执行后交接

计划执行完成后，执行阶段应继续：

- [ ] 把长期边界写入 `docs/architecture/subject-test-framework-v1/perf-and-multi-subject-expansion-v1.md`
- [ ] 如形成长期知识，再更新 `wiki/06-测试验证/`
