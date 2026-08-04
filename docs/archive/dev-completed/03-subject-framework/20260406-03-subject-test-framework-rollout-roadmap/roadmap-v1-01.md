# Subject 测试框架重构落地 Roadmap v1.01

Date: 2026-04-06
Status: active-parallel-roadmap

## 1. 目标

把 `docs/archive/discuss/` 中已经冻结的大量测试框架设计，转成一条可执行、可拆分、可进入后续 `plan` 的实施路线。

本 roadmap 的目标不是直接写实现 chunk，而是：

- 固定实施顺序
- 固定阶段边界
- 固定哪些阶段先做，哪些阶段后做
- 固定进入 `plan` 之前必须由用户拍板的问题

## 2. 范围边界

本 roadmap 负责：

- `subject` 测试框架的首批落地路径
- 首批 `HelloWorldObject` 与 Windows matrix 的切入顺序
- 新旧测试框架之间的桥接策略
- 进入 `plan` 前的关键决策收口

本 roadmap 不负责：

- 直接生成实现级任务 chunk
- 直接进入代码改动
- 直接创建 perf 扩展计划
- 直接定义所有未来 subject 的长期路线

## 3. 非目标

当前不在本 roadmap 中展开：

- macOS 首批 subject cutover
- `perf.dev` / `perf.release` 的实现计划
- 多个 subject 的批量铺开
- public TUI 的最终形态

## 4. 阶段列表

### Stage A：Subject Root And Canonical Inputs

- 创建 `subjects/HelloWorldObject/`
- 切 `contracts/` canonical source
- 固定 subject filesystem 与 bucket 命名真源

### Stage B：Execution Spine Bootstrap

- 落 `subject.manifest.json`
- 落 planner / executor 最小骨架
- 抽取最关键的 stage worker

### Stage C：HelloWorldObject Windows Matrix Cutover

- 让 `HelloWorldObject` 的 Windows 首批 matrix 真正跑在新脊柱上
- 拆 Stage 4 CMake 与 `verify-roadmap-0.py` 隐式 orchestration

### Stage D：Reporting And Event Compatibility

- 接 matrix report / subject summary / session summary
- 接 matrix / subject / session events
- 保留 session 兼容投影

### Stage E：Public Entry And Registry Cutover

- 再接 public CLI / registry / TUI
- 收口旧 `suite/registry/session` 的兼容或退役

### Stage F：Performance And Multi-Subject Expansion

- 在 correctness 脊柱稳定之后，再纳入 perf 维度和更多 subject
- 在后续测试工程清理中，不删除现有测试工程，而是把它们全部收敛为长期基线

## 5. 每阶段完成定义

### Stage A

- `goal`: 把 `subject` 真源与 canonical 输入面固定下来。
- `exit_criteria`:
  - `subjects/HelloWorldObject/` 真实存在
  - `HelloWorldObject` 的 canonical source 不再以 `tests/proof` 为真源
  - `contracts.py` 与相关 helper 切到 `contracts/` + `tests/contracts/`
  - artifact bucket 文件名与目录落点冻结
- `deliverables`:
  - `subjects/HelloWorldObject/subject.manifest.json`
  - `subjects/HelloWorldObject/source/...`
  - contract source cutover 改造点清单
- `dependencies`: 无
- `open_questions`:
  - 旧 `tests/proof/input/HelloWorldObject` 是保留 compatibility wrapper 还是立即瘦身到只剩迁移壳

### Stage B

- `goal`: 建立新测试框架的最小执行脊柱。
- `exit_criteria`:
  - `subject.manifest.json` 增量 schema 可被消费
  - planner 能产出 `planVersion/request/selection/artifactsRoot/stagePlan`
  - executor 能消费单 stage request
  - 至少三个核心 worker 被抽离：
    - `dotnet-host-input-builder`
    - `frontend-pipeline-worker`
    - `native-proof-emitter`
- `deliverables`:
  - subject planner skeleton
  - subject executor skeleton
  - worker IO contract 实现骨架
- `dependencies`:
  - Stage A
- `open_questions`:
  - Stage B 是否允许暂时不暴露任何 public command，只保留内部驱动入口

### Stage C

- `goal`: 让 `HelloWorldObject` 的首批 Windows matrix 跑在新框架上。
- `exit_criteria`:
  - `windows-dev-output`
  - `windows-reference-trace`
  - `windows-android-buildable`
  - `windows-linux-buildable`
  都从新 `subject` artifact 链消费
  - Stage 4 CMake 不再隐式执行前半段 pipeline
  - Windows `verify-roadmap-0` 继续通过，但其角色退化为 compatibility gate
- `deliverables`:
  - 新框架下的 `HelloWorldObject` Windows matrix 骨架
  - 拆解后的 Stage 4 build / runtime 接线
  - compatibility `verify-roadmap-0` 适配层
- `dependencies`:
  - Stage B
- `open_questions`:
  - Windows `verify-roadmap-0` 是否整个阶段内都保持 mandatory gate

### Stage D

- `goal`: 把 reporting / event 体系接到新执行脊柱上，并保持 session 兼容。
- `exit_criteria`:
  - `report.json`
  - `subject-report/summary.json`
  - `artifacts/logs/tests/<run-id>/summary.json`
  三层 schema 落地
  - matrix / subject / session events 落地
  - 现有 `test watch` / `test summary` 不被打断
- `deliverables`:
  - reporting adapter
  - session compatibility projection
  - `subjectResults` 接入 session summary
- `dependencies`:
  - Stage C
- `open_questions`:
  - 是否允许 Stage D 完成前，public watch 只保证 session summary 可读，而不暴露 matrix 级 timeline

### Stage E

- `goal`: 完成 public surface 的 subject 化切换。
- `exit_criteria`:
  - 新 CLI / registry 能发现和执行 subject
  - 旧 `suite/registry/session` 入口完成桥接或退役策略
  - public TUI 至少能导航到新的 summary/report
- `deliverables`:
  - subject registry
  - public command cutover 方案
  - 旧入口退役清单
- `dependencies`:
  - Stage D
- `open_questions`:
  - 旧 registry object 是保留 alias，还是集中迁移到 compatibility adapters

### Stage F

- `goal`: 在 correctness 主线稳定后，再扩 perf 与多 subject。
- `exit_criteria`:
  - `perf.dev`
  - `perf.release`
  具备稳定 metric/report 基础
  - 至少第二个 subject 的接入方式稳定
  - 测试工程清理完成后，现有测试工程全部保留，并被纳入长期基线集合
- `deliverables`:
  - perf framework follow-up plan
  - multi-subject onboarding pattern
  - test-project cleanup strategy（只做结构清理，不做删除）
- `dependencies`:
  - Stage E
- `open_questions`:
  - 第二个 subject 选择哪类验证价值最高

## 6. 计划任务映射

| task_id | scope | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover` | `Stage A + Stage B + Stage C` | `completed` | `codex` | 首份实现计划，已完成 subject 真源落地、执行脊柱最小骨架和 Windows 首批 matrix cutover | `-` |
| `20260406-08-subject-stage-d-e-reporting-and-public-cutover` | `Stage D + Stage E` | `completed` | `codex` | 第二份 follow-up plan，已完成 reporting / events compatibility 与 subject registry / public entry cutover，并在 Windows compatibility gate 通过后归档 | `20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover` |
| `20260407-01-subject-stage-f-perf-and-multi-subject-expansion` | `Stage F` | `completed` | `codex` | 第三份 follow-up plan，已完成 perf、第二个正式 subject 与长期基线整理规则收口 | `20260406-08-subject-stage-d-e-reporting-and-public-cutover` |

说明：

- `Stage A + Stage B + Stage C` 已完成独立 child plan 并归档
- `Stage D + Stage E` 不单独拆两份 plan，而是合并为第二份 follow-up plan；当前已由 `20260406-08-subject-stage-d-e-reporting-and-public-cutover` 完成并归档
- `Stage F` 已由 `20260407-01-subject-stage-f-perf-and-multi-subject-expansion` 完成并归档，三份 follow-up plan 全部收口

## 7. 依赖

全局依赖关系固定为：

1. 先有 `subject` 真源与 canonical 输入面
2. 再有 planner / executor / worker 执行脊柱
3. 再让 `HelloWorldObject` 首批 Windows matrix 跑在新脊柱上
4. 再接 reporting / events
5. 最后才做 public cutover
6. perf 与多 subject 继续后置

## 8. 风险

### 风险 1：双控制面

如果新并行任务继续推进，而 active reboot roadmap 也继续推进，会形成双控制面。

### 风险 2：实现顺序被打乱

如果绕过 Stage A / Stage B，先改 public registry / TUI，后面几乎必然返工。

### 风险 3：compatibility 层失控

如果不严格限制旧入口的职责，兼容层会变成第二套正式框架。

### 风险 4：性能过早并入

如果 Stage F 提前，整个阶段划分会失去控制。

## 9. 备选路径

### 方案 A：internal-first，public-later

- 优点：最符合当前设计收敛状态
- 缺点：前期用户感知较弱

### 方案 B：长期双栈

- 优点：短期看似更稳
- 缺点：长期成本最高

### 方案 C：public-first

- 优点：外部可见度高
- 缺点：返工风险最大

当前采用：

- **方案 A**

## 10. 当前建议推进顺序

1. `20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover` 已完成并归档
2. `20260406-08-subject-stage-d-e-reporting-and-public-cutover` 已完成并归档
3. `20260407-01-subject-stage-f-perf-and-multi-subject-expansion` 已完成并归档

## 11. 已确认的计划前提

以下边界已由用户确认，因此可以直接作为后续 plan 的前提：

1. 首份 plan 明确排除 `perf.dev / perf.release`
2. `subjects/HelloWorldObject/` 在第一阶段物理落地
3. public CLI / registry / TUI 后置到 Stage E
4. Windows `verify-roadmap-0` 作为整个重构期间的 mandatory compatibility gate
5. 并行任务保持 `active: false`，不影响当前 active 主线
6. 后续任务不在首份 plan 中展开，而是在 roadmap 中以 `Stage D + Stage E`、`Stage F` 两个置后计划位管理
7. 后续测试工程清理只做结构收敛，不删除现有测试工程；现有测试工程全部转为长期基线

## 12. 结论

本 roadmap 现在已经从“等待 plan gate 确认”切换到“跟踪已完成 child plan 与当前最后一份 follow-up 计划”的状态。

当前实施顺序已经固定为：

1. `20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover`（已完成）
2. `20260406-08-subject-stage-d-e-reporting-and-public-cutover`（已完成）
3. `20260407-01-subject-stage-f-perf-and-multi-subject-expansion`（已完成）

只要保持这个顺序，后续就不会把 `perf`、public surface 和 reporting compatibility 提前并入首份实施计划。
