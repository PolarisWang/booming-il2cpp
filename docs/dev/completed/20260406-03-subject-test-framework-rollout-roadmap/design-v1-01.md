# Subject 测试框架重构落地设计 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 设计目标

把 `docs/discuss/` 中已经确定的测试框架设计，收敛成一个可以实施的落地设计：

- 以 `subject` 为唯一正式测试对象
- 以 `goal + environment-matrix + artifact-chain` 为执行语义中轴
- 以内部分层先稳定、public surface 后置为实施原则

## 2. 设计输入

本设计以 `docs/discuss/test-framework/`、`docs/discuss/subject-framework/`、`docs/discuss/reporting/` 与 `docs/discuss/roadmap-0/` 下的连续讨论为输入，不另行在 `docs/discuss/` 生成新的同步副本。

其中最关键的冻结点已经包括：

- `subject / goal / environment-matrix / artifact-chain`
- `shared/source/host-input/analysis/generated`
- `matrices/<matrix-id>/build/runtime/report`
- `planner output`
- `executor / stage worker` 边界
- `matrix report / subject summary / session summary`
- `matrix / subject / session events`

## 3. 总体设计选择

总体采用：

- **internal-first subject spine**

也就是：

1. 先落 `subjects/` 与 artifact bucket
2. 先落 planner / executor / worker / reporting / events 的新脊柱
3. 再让 `HelloWorldObject` 的首批 Windows matrix 在新脊柱上真实跑通
4. 最后才切 public CLI / registry / TUI

## 4. 正式中轴

## 4.1 object model

正式中轴固定为：

- `subjects/<subject-id>/subject.manifest.json`

其中 matrix 的选择语义来自：

- `supportedGoals`
- `environmentMatrices`
- `pipelineId`
- `executionPipelines`
- `artifactPlan`

## 4.2 artifact model

正式 bucket 固定为：

- `source`
- `host-input`
- `analysis`
- `generated`
- `build`
- `runtime`
- `report`

落点固定为：

- `artifacts/subjects/<subject-id>/shared/...`
- `artifacts/subjects/<subject-id>/matrices/<matrix-id>/...`
- `artifacts/subjects/<subject-id>/subject-report/...`

## 4.3 execution model

执行语义固定为：

- planner
  - 选择 goal / matrix / pipeline
  - 展开 stage plan
  - 决定 `reused / executed / invalidated`
- executor
  - 调度 stage worker
  - 写 events
  - 兑现 reuse / invalidation
  - 产出 `matrix execution result`
- reporting
  - 把 `matrix execution result` 收成 `report.json`
  - 聚合成 subject/session summary

## 5. 首批正式实施边界

## 5.1 包含

首批实施建议只包含：

- `HelloWorldObject`
- Windows 首批 matrix：
  - `windows-dev-output`
  - `windows-reference-trace`
  - `windows-android-buildable`
  - `windows-linux-buildable`
- `correctness.dev`
- `correctness.platform`

这里的 `android/linux` 只指：

- buildable correctness

不指真实运行时验证。

## 5.2 暂不包含

首批实施明确后置：

- `perf.dev`
- `perf.release`
- macOS 首批 subject cutover
- 多 subject 并行扩展
- public CLI / public registry / TUI 完整改造

## 6. 当前代码到目标设计的主要断点

## 6.1 测试对象断点

当前：

- `tests/proof/hello-world-object/suite.manifest.json`

目标：

- `subjects/HelloWorldObject/subject.manifest.json`

所以：

- 需要从 `suite` 真源切到 `subject` 真源

## 6.2 执行器断点

当前：

- `registry.py` 展开的是 `suite/module/system/pipeline`
- `test.py` 产出的是 `suiteResults`

目标：

- `planner output -> executor -> matrix execution result`

所以：

- 旧 registry/session 不可能平滑变成新脊柱
- 中间需要桥接层，而不是字段硬改

## 6.3 契约源断点

当前：

- `contracts.py` 仍读 `analysis/contracts`

目标：

- `contracts/` 是 canonical source
- `tests/contracts/` 是 baseline / harness

所以：

- contract source cutover 必须早于 worker 抽取

## 6.4 隐式 orchestration 断点

当前：

- Stage 4 CMake 与 `verify-roadmap-0.py` 仍隐式串起多阶段

目标：

- `host-input-build`
- `analysis-frontend`
- `generated-native-proof`
- `build-target`
- `runtime-observe`
- `runtime-trace-compare`

都成为显式 stage

所以：

- 这是首批真正要拆的“暗箱层”

## 7. 推荐的实施架构

## 7.1 subject filesystem 先落地

第一阶段就应正式创建：

```text
subjects/
  HelloWorldObject/
    subject.manifest.json
    source/
```

同时：

- 旧 `tests/proof/input/HelloWorldObject`
  - 退化为兼容壳或迁移来源

## 7.2 worker 只围绕 bucket

worker 不再面向：

- suite
- pipeline
- monolithic workflow

而只面向：

- 单 stage
- 单 bucket
- 单 manifest 输入输出

## 7.3 report 与 event 只服务上层

普通 worker 不直接写：

- `report.json`
- `subject summary`
- `session summary`
- `events.jsonl`

它们只写：

- 当前 bucket manifest
- 当前 bucket 局部报告
- 当前 bucket 诊断文件

## 7.4 兼容层集中在 session / bridge

旧入口不直接理解新 subject 细节。

更合理的方式是：

- 新框架内部产生：
  - matrix report
  - subject summary
  - matrix / subject events
- session 层再投影出：
  - `progress`
  - `final-summary`
  - `subjectResults`

## 8. 风险评估

## 8.1 架构风险：双真相

如果 `subject` 与 `suite` 长期并列为正式真源，会形成：

- manifest 双真相
- report 双真相
- event 双真相

缓解方式：

- 尽早把 `subjects/` 定为唯一正式真源
- 旧对象只保留 compatibility wrapper 身份

## 8.2 实现风险：Stage 4 假拆分

如果只是在 Python 层补 planner/executor，但不拆 CMake / script 里的隐式链路，最终只是“表面 stage 化”。

缓解方式：

- 明确把 Stage 4 CMake 和 `verify-roadmap-0.py` 的拆解列入首批实施

## 8.3 产品风险：范围膨胀

如果第一份计划同时覆盖：

- correctness
- perf
- multi-subject
- public CLI
- TUI

大概率会再次变成超大 plan。

缓解方式：

- 首份计划只覆盖 `HelloWorldObject + Windows correctness + compatibility bridge`

## 8.4 迁移风险：parallel task 与 active 主线分叉

本任务现在是并行设计任务。如果后续不把它回收到 active reboot 主线，容易出现：

- 当前主线继续决策
- 并行任务也继续决策

缓解方式：

- 在进入 `plan` 前明确它的最终归属

## 9. 非目标

这份设计当前不承诺：

- 立即生成正式 `plan-v1-01.md`
- 立即创建 implementation child task
- 立即替换现有所有 public test commands
- 立即接入性能报告全覆盖

## 10. 进入 plan 前的 gate

这份设计已经足以进入 roadmap，但还不应该直接进入计划执行。

进入 `plan` 前，我认为必须冻结下面几项：

1. 本并行任务后续是否回收到 active reboot 主线
2. 首份 plan 是否明确排除 `perf.dev / perf.release`
3. `subjects/HelloWorldObject/` 是否第一阶段物理落地
4. public CLI / registry / TUI 是否确定后置
5. Windows `verify-roadmap-0` 是否保持 mandatory non-regression gate

## 11. 结论

当前推荐结论是：

- 设计通过
- 应进入 roadmap
- 但在关键 gate 问题拍板前，不进入 `writing-plans`
