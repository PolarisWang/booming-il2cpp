# Subject Stage A+B+C Foundation And Windows Cutover 实现计划

> **面向执行 Agent：** 必须使用 `dev:subagent-driven-development` 或 `dev:executing-plans` 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 以 `HelloWorldObject` 为首个 `subject`，完成 `subjects/` 真源落地、planner / executor / worker 最小脊柱搭建，并让 Windows 首批四个 matrix 真实消费新 `subject` artifact 链，同时保持 Windows `verify-roadmap-0` 作为 mandatory compatibility gate。

**架构：** 先把 `subjects/HelloWorldObject`、contract source 与 artifact bucket 真源固定，再以最小可执行方式落 `subject.manifest.json`、planner / executor / worker，并拆掉 Stage 4 CMake 与 `verify-roadmap-0.py` 中的隐式 orchestration，最后只对 `HelloWorldObject` 的 Windows 四个 matrix 做内部 cutover。聚合 reporting、session compatibility 与 public CLI / registry / TUI 全部后置到下一阶段。

**技术栈：** Python 3、.NET 8、CMake、JSON manifests、`unittest`

**设计文档：** `docs/dev/in-progress/20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover/design-v1-01.md`

**预期知识沉淀：** `docs/archive/architecture/subject-test-framework-v1/`（按任务执行结果细分）

---

## 目标文件与职责

- 新增：`subjects/HelloWorldObject/subject.manifest.json`
  - `HelloWorldObject` 的唯一正式测试对象真源。
- 新增：`subjects/HelloWorldObject/source/`
  - `HelloWorldObject` 的 canonical source tree。
- 修改：`tests/proof/input/HelloWorldObject/`
  - 退化为兼容壳或迁移来源，不再作为正式真源。
- 修改：`build/toolchains/run/testing/contracts.py`
  - 从 `analysis/contracts` 切到 `contracts/` + `tests/contracts/`。
- 新增：`build/toolchains/run/testing/subjects.py`
  - 读取 `subjects/*/subject.manifest.json` 的最小 discovery helper。
- 新增：`build/toolchains/run/testing/subject_planner.py`
  - 根据 `subject.manifest.json` 产出 `planner output`。
- 新增：`build/toolchains/run/testing/subject_executor.py`
  - 消费 `planner output`，调度 stage worker，产出 matrix-local execution results。
- 新增：`build/toolchains/run/testing/subject_workers.py`
  - 落最小 worker：
    - `dotnet-host-input-builder`
    - `frontend-pipeline-worker`
    - `native-proof-emitter`
- 修改：`tests/proof/native-reference/HelloWorldObject/CMakeLists.txt`
  - 拆 Stage 4 隐式链路，让 `build-target` 只消费 `generated` bucket。
- 修改：`build/scripts/verify-roadmap-0.py`
  - 从 monolithic orchestrator 退化为 compatibility gate，显式消费 `HelloWorldObject` 新 artifact 链。
- 新增：`tests/unit/run/test_subject_manifest_schema.py`
  - 锁 `subject.manifest.json` 的首批 schema。
- 新增：`tests/unit/run/test_subject_contracts_source_cutover.py`
  - 锁 contract source 切换行为。
- 新增：`tests/unit/run/test_subject_planner.py`
  - 锁 planner output 对象。
- 新增：`tests/unit/run/test_subject_executor.py`
  - 锁 executor / worker IO contract。
- 新增或修改：`tests/unit/run/test_helloworld_subject_windows_cutover.py`
  - 锁首批 Windows matrix 走新 artifact 链。
- 修改：`docs/dev/in-progress/20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover/STATUS.md`
  - 维护计划阶段状态、风险与下一步。
- 修改：`docs/dev/in-progress/20260406-03-subject-test-framework-rollout-roadmap/STATUS.md`
  - 记录 child 计划任务已派生。
- 修改：`docs/dev/in-progress/20260406-03-subject-test-framework-rollout-roadmap/roadmap-v1-01.md`
  - 将 `Stage A/B/C` 计划任务状态更新为 `ready`，并明确 `Stage D+E`、`Stage F` 的后置计划位置。
- 修改：`docs/dev/INDEX.md`
  - 同步本 child 计划任务。
- 新增或修改：`docs/dev/in-progress/20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover/notes/progress-*.md`
  - 每个 chunk 完成后记录验证与剩余风险。

## 执行原则

- [ ] 首份计划不纳入 `perf.dev / perf.release`。
- [ ] 首份计划不纳入 public CLI / registry / TUI。
- [ ] `subjects/HelloWorldObject/` 第一阶段必须物理落地。
- [ ] `tests/proof/input/HelloWorldObject` 不再作为正式真源。
- [ ] Windows `verify-roadmap-0` 必须保持为 mandatory compatibility gate。
- [ ] 任何桥接层都不能变成第二套正式 `subject` 框架。
- [ ] 每完成一个 chunk，都更新本任务 `STATUS.md`、`notes/progress-*.md` 与 `docs/dev/INDEX.md`。

## Chunk 1：落 `subjects/HelloWorldObject` 真源与 canonical filesystem

- [ ] 新建 `subjects/HelloWorldObject/subject.manifest.json`。
- [ ] 新建 `subjects/HelloWorldObject/source/`，把 `HelloWorldObject` canonical source 放到这里。
- [ ] 明确 `subject.manifest.json` 最小字段，至少覆盖：
  - `subjectId`
  - `displayName`
  - `defaultGoal`
  - `defaultMatrix`
  - `source`
  - `environmentMatrices`
  - `executionPipelines`
- [ ] 调整旧 `tests/proof/input/HelloWorldObject`，让它只保留兼容角色。
- [ ] 先写失败测试，再实现最小 subject manifest loader。

**验证：**

- [ ] `python -m unittest tests.unit.run.test_subject_manifest_schema`

## Chunk 2：完成 contract source cutover

- [ ] 先写失败测试，锁 `contracts.py` 优先读取 `contracts/` 与 `tests/contracts/`。
- [ ] 修改 `build/toolchains/run/testing/contracts.py`。
- [ ] 移除新实现对 `analysis/contracts` 的长期正式依赖。
- [ ] 明确 compatibility mirror 仍可保留，但不再作为 canonical source。

**验证：**

- [ ] `python -m unittest tests.unit.run.test_subject_contracts_source_cutover`

## Chunk 3：固定 artifact bucket 落点与 bucket manifest 命名

- [ ] 先写失败测试，锁以下路径规则：
  - `artifacts/subjects/<subject-id>/shared/source`
  - `artifacts/subjects/<subject-id>/shared/host-input`
  - `artifacts/subjects/<subject-id>/shared/analysis`
  - `artifacts/subjects/<subject-id>/shared/generated`
  - `artifacts/subjects/<subject-id>/matrices/<matrix-id>/build`
  - `artifacts/subjects/<subject-id>/matrices/<matrix-id>/runtime`
- [ ] 锁以下最小文件名：
  - `source.manifest.json`
  - `host-input.manifest.json`
  - `analysis.manifest.json`
  - `generated.manifest.json`
  - `build.manifest.json`
  - `runtime.manifest.json`
- [ ] 在 planner / executor 共用 helper 中固定这些路径拼装规则。

**验证：**

- [ ] `python -m unittest tests.unit.run.test_subject_manifest_schema`
- [ ] `python -m unittest tests.unit.run.test_subject_planner`

## Chunk 4：落 planner output 最小骨架

- [ ] 先写失败测试，锁 planner output 至少包含：
  - `planVersion`
  - `request`
  - `selection`
  - `artifactsRoot`
  - `stagePlan`
- [ ] 实现 `subject_planner.py` 最小能力：
  - 只支持 `HelloWorldObject`
  - 只支持四个 Windows matrix
  - 只支持 `correctness.dev` / `correctness.platform`
- [ ] 明确 `executionMode`
  - `reused | executed | invalidated`
- [ ] 明确 `evidenceTerminalBucket`
  - 来自 `artifactPlan`

**验证：**

- [ ] `python -m unittest tests.unit.run.test_subject_planner`

## Chunk 5：落 executor / worker 最小骨架

- [ ] 先写失败测试，锁单 stage request / single bucket write contract。
- [ ] 实现 `subject_executor.py` 最小能力：
  - 按 `stagePlan` 顺序执行
  - 支持 `reused / executed / invalidated`
  - 只写 matrix-local / bucket-local 产物
- [ ] 抽最关键的三个 worker：
  - `dotnet-host-input-builder`
  - `frontend-pipeline-worker`
  - `native-proof-emitter`
- [ ] 明确 worker 不直接写：
  - `subject summary`
  - `session summary`
  - session `events.jsonl`

**验证：**

- [ ] `python -m unittest tests.unit.run.test_subject_executor`

## Chunk 6：拆 Stage 4 CMake 隐式链路

- [ ] 先写失败测试，锁 Stage 4 CMake 不再隐式执行：
  - host-input build
  - frontend pipeline
  - native emission
- [ ] 修改 `tests/proof/native-reference/HelloWorldObject/CMakeLists.txt`
  - 让 `build-target` 只消费 `generated` bucket
- [ ] 必要时拆出新的 helper script 或 CMake arguments，让外部 executor 负责前半段产物准备。

**验证：**

- [ ] `python -m unittest tests.unit.run.test_helloworld_subject_windows_cutover`
- [ ] 如环境允许，补最小 CMake configure/build 验证

## Chunk 7：让首批 Windows matrix 跑在新脊柱上

- [ ] 先写失败测试，锁四个 matrix：
  - `windows-dev-output`
  - `windows-reference-trace`
  - `windows-android-buildable`
  - `windows-linux-buildable`
  都消费 `HelloWorldObject` 的新 artifact 链。
- [ ] 让 `windows-dev-output` 与 `windows-reference-trace` 共享到 `generated` 为止。
- [ ] 让 Android/Linux buildable matrix 成为 subject-aware build worker，而不是 generic preset gate wrapper。
- [ ] 本阶段只要求 matrix-local 成功与证据落点正确，不要求完成完整 subject/session 聚合 reporting。

**验证：**

- [ ] `python -m unittest tests.unit.run.test_helloworld_subject_windows_cutover`
- [ ] 如环境允许，补 Windows reference 最小 proof run 验证

## Chunk 8：桥接 `verify-roadmap-0` Windows compatibility gate

- [ ] 修改 `build/scripts/verify-roadmap-0.py`，让它不再自己从头 orchestrate 整条新 pipeline。
- [ ] 它应退化为：
  - 触发或消费 `HelloWorldObject` 的新 artifact 链
  - 校验 proof run 与 platform buildable gate
- [ ] 保证 Windows `verify-roadmap-0` 仍然是 mandatory non-regression gate。

**验证：**

- [ ] 在可运行环境下补 `verify-roadmap-0` Windows gate
- [ ] 若当前环境无法补跑，必须在进度记录里明确缺口

## Chunk 9：收口状态、索引与父 roadmap

- [ ] 更新本 child `STATUS.md`
- [ ] 更新父 parallel roadmap `STATUS.md` 与 `roadmap-v1-01.md`
- [ ] 在父任务中标记：
  - `Stage A+B+C` child 已进入计划完成态
  - `Stage D+E` 作为下一份合并计划后置
  - `Stage F` 继续单独后置
- [ ] 更新 `docs/dev/INDEX.md`

## 后续计划占位

本计划之外，父 roadmap 还应明确保留以下后续计划位置：

- `Stage D + Stage E`
  - 合并为第二份计划
  - 覆盖 reporting / events compatibility 与 public entry cutover
- `Stage F`
  - 单独作为第三份计划
  - 覆盖 perf 与 multi-subject 扩展
  - 如进入测试工程清理，只做结构收敛，不删除现有测试工程；现有测试工程全部转长期基线

## 关键验证命令

- [ ] `python -m unittest tests.unit.run.test_subject_manifest_schema`
- [ ] `python -m unittest tests.unit.run.test_subject_contracts_source_cutover`
- [ ] `python -m unittest tests.unit.run.test_subject_planner`
- [ ] `python -m unittest tests.unit.run.test_subject_executor`
- [ ] `python -m unittest tests.unit.run.test_helloworld_subject_windows_cutover`
- [ ] 如环境允许：Stage 4 Windows reference 最小 proof run
- [ ] 如环境允许：`build/scripts/verify-roadmap-0.py windows`

## 完成标准

- [ ] `subjects/HelloWorldObject/` 成为正式真源。
- [ ] contract source 已切到 `contracts/` + `tests/contracts/`。
- [ ] planner / executor / worker 最小脊柱存在且通过最小单测。
- [ ] Stage 4 CMake 不再隐式串起前半段 pipeline。
- [ ] `HelloWorldObject` Windows四个 matrix 真实消费新 artifact 链。
- [ ] Windows `verify-roadmap-0` 仍保持 mandatory compatibility gate。
- [ ] 计划执行边界仍然没有扩张到 `perf`、public CLI / registry / TUI、全量 reporting。
- [ ] 后续测试工程清理策略已明确不删除现有测试工程；现有测试工程全部转为长期基线。

## 执行后交接

计划已完成后，执行阶段应优先使用：

- `dev:subagent-driven-development`

如果执行环境不适合，则退回：

- `dev:executing-plans`
