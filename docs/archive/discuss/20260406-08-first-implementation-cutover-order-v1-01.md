# 首批实施切换顺序 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 结论

下一步不应该继续扩展 matrix 范围，也不应该先改 CLI。

首批真正该进入实施设计的顺序应固定为：

1. 固定 contract source cutover
2. 固定阶段产物与 report 命名
3. 固定 `subject.manifest.json` 增量 schema
4. 抽取 stage worker
5. 拆 Stage 4 CMake 隐式链路
6. 最后才接 CLI / registry / TUI

## 2. 为什么是这个顺序

当前最危险的问题不是“命令怎么叫”，而是：

- 规范源路径还没完全定
- analysis / generated / report 的产物命名还没定
- 现有 Stage 4 把多个阶段糊在一起

如果在这之前先改 CLI / registry：

- 会把不稳定路径提前固化到外部接口
- 后面拆 stage 时又得回头改 public surface

所以必须先把内部执行契约定住。

## 3. Step 1：固定 contract source cutover

目标：

- 正式确定新实现一律读取 `contracts/` 与 `tests/contracts/`
- 不再把 `analysis/contracts` 当成长期正式输入

要固定的结论：

- `contracts/` = canonical source
- `tests/contracts/` = baseline / harness
- `analysis/contracts` = compatibility mirror，进入退役路径

首批需要同步调整的代码点：

- `build/toolchains/run/testing/contracts.py`
- 任何直接读 `analysis/contracts/*` 的 validator 或 helper

这一项不先做，后面 worker 和 report 的路径都会漂。

## 4. Step 2：固定阶段产物与 report 命名

目标：

- 把 `artifacts/subjects/<subject-id>/...` 下每个 bucket 的最小清单定下来

至少要固定：

- `source.manifest.json`
- `host-input.manifest.json`
- `analysis.manifest.json`
- `generated.manifest.json`
- `build.manifest.json`
- `exit-code.json`
- `trace-compare.report.json`
- `report.json`
- `events.jsonl`

这一项的重要性在于：

- worker 的输入输出要靠这些文件名对接
- `stage-results[]` / `artifact-results[]` 也要靠这些清单落地

## 5. Step 3：固定 `subject.manifest.json` 增量 schema

这一步只需要补最小必要字段，不要一次做大。

首批建议正式加入：

- `supportedGoals`
- `artifactPlan`

其中：

- `supportedGoals` 解决 goal 和 matrix 的合法组合问题
- `artifactPlan` 解决 matrix 终止桶不同的问题

这一项定完，才知道执行器到底该跑到哪一层停。

## 6. Step 4：抽取 stage worker

首批只抽最关键的三个：

- `dotnet-host-input-builder`
- `frontend-pipeline-worker`
- `native-proof-emitter`

原因：

- 这三段现在是前半条共享主线
- 也是最需要从 Stage 4 CMake 中解耦出来的部分

## 7. Step 5：拆 Stage 4 CMake 隐式链路

目标：

- 让 `build-target` 只消费 `generated` bucket
- 不再在 CMake `custom_command` 里偷偷执行 host-input / analysis / generated

这一步完成后，新的 stage 报告才有真实意义。

## 8. Step 6：最后再接 CLI / registry / TUI

只有当内部执行模型稳定后，才适合去做：

- `run test subject <subject-id>`
- subject registry scan
- TUI 视图改造

否则外部接口会比内部实现先冻结，代价更高。

## 9. 当前最值得立刻讨论的唯一问题

如果只选一个“下一步”继续往下收敛，我建议就是：

- 把 `artifacts/subjects/...` 的最小 manifest / report 文件命名定死

因为这一步会同时影响：

- worker 接口
- stage-results / artifact-results
- report 汇总
- CMake 解耦方式
- subject 调试路径

它是现在真正的中轴。
