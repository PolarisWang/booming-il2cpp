# 统一入口 test all 展示增强设计

## 1. 设计目标

把 `test all` 从“黑盒批量执行按钮”提升为“可预览、可跟踪、可定位失败”的统一测试入口。

本设计回答四个问题：

1. 启动前能否看清本次批次范围
2. 运行中能否看清当前执行状态
3. 完成后能否快速知道哪里失败
4. 失败后能否直接拿到报告路径和重跑命令

## 2. 设计范围

本次只设计展示增强，不进入实现。

覆盖范围：

- `test all` 在统一入口菜单中的文案与预览
- `test watch` / progress 页面
- `test summary` / 完成后摘要
- summary / event payload 为展示新增的最小聚合字段

不在范围内：

- 改变 `test all` 的选择对象集合
- 改变 suite / subject 的执行顺序
- 新增复杂交互式 dashboard
- 改写单个 suite 或单个 subject 的底层报告格式

## 3. 现状问题

### 3.1 入口表述过时

当前 `test all` 的标题仍然接近“默认 public test batch”，但真实行为已经扩展到：

- suite
- subject

用户无法在入口理解执行成本和批次组成。

### 3.2 运行中视图信息过薄

当前 progress 页面只有：

- `Command`
- `Status`
- `Progress`
- `Timeline`

缺少：

- 本次批次数量
- suite / subject 的分项统计
- 当前 active unit 的上下文
- 失败对象摘要

### 3.3 完成后摘要偏底层罗列

当前 summary 只是罗列：

- `Phases`
- `Subjects`
- `Suites`

但已有聚合信息如：

- `caseCounts`
- `trafficLightCounts`
- `subjectStatusCounts`

没有转成用户可读的摘要层。

## 4. 总体方案

采用“三层展示”结构：

### 4.1 启动前预览

在用户确认执行 `test all` 前，展示本次批次的计划信息。

目标：

- 让用户知道本次会跑多少对象
- 告诉用户有哪些 suite family 和多少 subject
- 告诉用户结果最终写到哪里

### 4.2 运行中统计

在 progress 页面中把“聚合状态”放在时间线之前。

目标：

- 第一眼能看到全局状态
- 第二眼能看到当前在跑谁
- 第三眼能看到最近失败什么

### 4.3 完成后摘要

在 summary 页面中优先展示：

- overall
- 分组统计
- failure digest
- 重要路径

目标：

- 不需要翻完整 timeline 就能知道结论

## 5. 启动前预览设计

### 5.1 入口标题

`test all` 在菜单里的标题应明确表达：

- 这是 host default batch
- 它包含 `suites + subjects`

推荐文案方向：

- `Run host default batch (suites + subjects)`

### 5.2 预览页内容

预览页固定展示：

- `Host`
- `Planned total units`
- `Suite count`
- `Subject count`
- `Suite families`
- `Subject preview`
- `Output paths`

其中：

- `Suite families` 按 family 聚合显示数量
- `Subject preview` 只显示前若干条，避免过长
- `Output paths` 至少展示：
  - `summary.json`
  - `events.jsonl`
  - `console.log`

### 5.3 预览页不做的事

- 不在预览页展开全部 suite 明细
- 不做分页浏览
- 不暴露底层 report.json 细节

## 6. 运行中 progress 设计

### 6.1 页面结构

推荐固定 5 个区块：

- `Batch`
- `Counts`
- `Current`
- `Failures`
- `Recent Timeline`

### 6.2 Batch 区块

显示：

- `Run`
- `Host`
- `Planned: total = suites + subjects`
- `Families`

示例：

- `Planned: 19 = 14 suites + 5 subjects`
- `Families: smoke 5 | contract 6 | gate 2 | workflow 1`

### 6.3 Counts 区块

显示运行中的全局状态计数：

- `ok`
- `fail`
- `skip`
- `running`
- `remaining`

并增加 type 维度：

- `suites: ok/fail/skip`
- `subjects: ok/fail/skip`

### 6.4 Current 区块

显示当前执行对象上下文。

若当前对象为 suite，展示：

- `id`
- `family`
- `level`
- `primaryModuleId`

若当前对象为 subject，展示：

- `id`
- `subjectId`
- `goalId`
- `matrixId`

如果没有这些字段，则回退成最小对象 id 展示。

### 6.5 Failures 区块

显示最近 N 条失败对象，不重复滚屏。

每条失败项应包含：

- `id`
- `type`
- `status`
- `rerun command`

必要时附：

- `reportPath`
- `subjectSummaryPath`

### 6.6 Recent Timeline 区块

保留现有时间线，但只承担“近期事件回放”职责，不再作为主信息区。

建议只展示最近 8-12 行。

## 7. 完成后 summary 设计

### 7.1 页面结构

推荐固定 4 个区块：

- `Overall`
- `Suite Breakdown`
- `Subject Breakdown`
- `Failure Digest`

后面再接：

- `Important outputs`
- `Artifacts`

### 7.2 Overall

显示：

- `Run`
- `Command`
- `Host`
- `Status`
- `Total units`
- `Suite count`
- `Subject count`

### 7.3 Suite Breakdown

按 family 汇总 suite 执行结果。

每个 family 展示：

- total
- ok
- fail
- skip

如已有 suite case 数据，还应汇总：

- `caseCounts`
- `trafficLightCounts`

### 7.4 Subject Breakdown

每个 subject 展示：

- `subjectId`
- `requestedGoalId`
- `status`
- `matrixStatusCounts`
- `subjectSummaryPath`

如果 subject 只有单矩阵结果，也不再只显示一行 `ok/fail`，而是保留最少的上下文信息。

### 7.5 Failure Digest

失败摘要是 summary 的核心区块。

每条失败项需要：

- 失败对象 id
- 对象类型
- 失败状态
- 推荐重跑命令
- 最关键报告路径

这样用户不需要再去通读 `console.log` 才知道下一步怎么定位。

## 8. 最小新增数据 contract

本次设计不改变根对象结构，只补展示需要的最小字段。

### 8.1 plannedCounts

建议挂在 summary / final event payload 下：

- `totalUnits`
- `suiteCount`
- `subjectCount`

### 8.2 countsByFamily

按 suite family 聚合：

- `smoke`
- `contract`
- `gate`
- `workflow`

每项至少包含：

- `total`
- `ok`
- `fail`
- `skip`

### 8.3 countsByType

聚合：

- `suite`
- `subject`

每类至少包含：

- `total`
- `ok`
- `fail`
- `skip`
- `aborted`

### 8.4 activeUnitContext

用于 progress 页面展示当前对象。

字段建议：

- `id`
- `type`
- `displayName`
- `family`
- `level`
- `primaryModuleId`
- `subjectId`
- `goalId`
- `matrixId`

实际填充时允许部分缺省。

### 8.5 failureItems[]

用于 summary 和 progress 的失败摘要。

每项建议包含：

- `id`
- `type`
- `status`
- `rerunCommand`
- `reportPath`
- `subjectSummaryPath`

### 8.6 复用现有字段

以下字段已存在，应优先复用：

- `suiteResults`
- `subjectResults`
- `phaseResults`
- `caseCounts`
- `trafficLightCounts`
- `subjectStatusCounts`

## 9. 渲染规则

### 9.1 摘要优先

无论是 progress 还是 summary，都应先显示聚合结果，再显示时间线和 artifacts。

### 9.2 type 区分清楚

混合批次里必须明确区分：

- suite
- subject

不能让用户只看到一个对象 id，而不知道它属于哪一层。

### 9.3 失败优先

当存在失败项时：

- failure digest 提前
- `Errors` 不再只是纯文本堆叠
- 要给出重跑命令和关键路径

### 9.4 控制屏幕长度

信息增强不等于铺满屏幕。

控制原则：

- 默认展示摘要
- 时间线只展示最近窗口
- 全量细节保留在 json / report 文件里

## 10. 验收标准

设计完成后，后续实现至少应满足：

1. 用户在执行前能看清 `test all` 的 suite/subject 组成
2. 用户在运行中能看清总量、当前对象和失败摘要
3. 用户在完成后能直接知道失败对象、报告路径和重跑命令
4. 现有执行语义、summary 路径、event 流机制保持兼容

## 11. 实现建议顺序

后续如果进入实现，建议顺序为：

1. 先补 summary / final event 的聚合字段
2. 再改 summary 文本渲染
3. 再改 progress 页面布局
4. 最后补菜单标题和启动前预览

这样可以先把数据 contract 冻结，再处理 UI 层。
