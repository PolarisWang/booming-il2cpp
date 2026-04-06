# manifest 消费边界设计 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 目标

在 `subject.manifest.json` 增量 schema 已经基本成型后，下一步需要明确：

- 哪些字段由 registry 读取
- 哪些字段由 planner 读取
- 哪些字段由 executor 读取
- 哪些字段只进入 reporting

否则 manifest 很容易继续膨胀成：

- 一个所有模块都随便读的杂糅配置文件

这会让后面的代码边界再次变差。

## 2. 结论

推荐把 manifest 的消费拆成四层：

- registry
- planner
- executor
- reporting

并采用一个很重要的原则：

- 同一个字段可以被多个层消费
- 但每个字段必须有一个“主消费方”

也就是：

- 主消费方负责定义该字段存在的直接理由
- 其他层只是在结果上引用，不重新解释语义

## 3. 四层分别负责什么

## 3.1 registry

负责：

- 扫描有哪些 subject
- 输出有哪些 matrix / goal / pipeline
- 做静态配置有效性检查

不负责：

- 执行 stage
- 计算 fingerprint
- 产出 runtime 结果

## 3.2 planner

负责：

- 解析用户请求
- 选择 goal / matrix / pipeline
- 展开执行计划
- 结合已有 manifest / fingerprint 决定哪些 stage `executed/reused/invalidated`

不负责：

- 真正执行 build/runtime
- 写具体 bucket 产物

## 3.3 executor

负责：

- 根据 planner 给出的 stage plan 真正执行各 stage
- 写 bucket manifest / report / logs
- 处理失败、失效重建与产物清理

不负责：

- 决定 subject 默认 matrix 是谁
- 重新推导 graph

## 3.4 reporting

负责：

- 聚合 matrix 结果
- 聚合 subject 结果
- 聚合 session 结果
- 输出 `report.json` / `summary.json` / `events.jsonl`

不负责：

- 决定要跑哪些 stage
- 决定 stage 复用策略

## 4. subject 顶层字段的主消费方

## 4.1 `subjectId`

主消费方：

- registry

次级消费：

- planner
- executor
- reporting

原因：

- 它首先决定“这是什么 subject”

## 4.2 `displayName`

主消费方：

- reporting

次级消费：

- registry

原因：

- 它主要服务人读展示

## 4.3 `category`

主消费方：

- registry

次级消费：

- planner

原因：

- 它首先是 subject 分类信息
- 未来 planner 可能据此做默认过滤或菜单分组

## 4.4 `defaultGoal`

主消费方：

- planner

次级消费：

- registry

原因：

- 它直接决定用户未给 `--goal` 时如何选

## 4.5 `defaultMatrix`

主消费方：

- planner

次级消费：

- registry

原因：

- 它直接决定用户未给 `--matrix` 时如何选

## 4.6 `source`

主消费方：

- executor

次级消费：

- registry
- planner

原因：

- 它真正决定 `source-resolve` 和后续执行输入

## 4.7 `tags`

主消费方：

- registry

次级消费：

- reporting

原因：

- 它主要用于检索、分组、过滤

## 4.8 `expectedArtifacts`

主消费方：

- registry

次级消费：

- reporting

原因：

- 它描述 subject 理论覆盖包络
- 不是执行器的直接 stage plan

## 4.9 `executionPipelines`

主消费方：

- planner

次级消费：

- executor
- registry

原因：

- 它的直接价值是展开执行图

## 4.10 `environmentMatrices`

主消费方：

- planner

次级消费：

- registry
- reporting

原因：

- 它决定用户可请求哪些 matrix
- 也决定 goal / pipeline / executionContext 的绑定

## 5. matrix 字段的主消费方

## 5.1 `matrixId`

主消费方：

- planner

次级消费：

- reporting
- executor

## 5.2 `pipelineId`

主消费方：

- planner

次级消费：

- registry
- executor

原因：

- planner 用它把 matrix 映射到 execution graph

## 5.3 `supportedGoals`

主消费方：

- planner

次级消费：

- registry

原因：

- 它直接决定 `goal + matrix` 组合是否合法

## 5.4 `executionContext`

主消费方：

- executor

次级消费：

- planner
- reporting

原因：

- `hostPlatform/targetPlatform/toolchainProfile/runtimeProfile`
- 这些最终是 executor 真正执行时要用的参数

## 5.5 `validationIntent`

主消费方：

- planner

次级消费：

- reporting
- executor

原因：

- planner 要据此理解这条 matrix 的验证目标
- reporting 要据此解释结果语义

### 其中更细一点：

- `validationMode`
  - 主消费方：planner
- `adaptationLevel`
  - 主消费方：reporting
- `expectedOutcome`
  - 主消费方：reporting

因为：

- `validationMode` 更像执行计划标签
- `adaptationLevel` / `expectedOutcome` 更像结果解读标签

## 5.6 `artifactPlan`

主消费方：

- planner

次级消费：

- executor
- reporting

原因：

- planner 用它理解 matrix 的 bucket 边界
- executor 用它知道不该越界多跑
- reporting 用它解释 evidence 终点

## 6. pipeline stage 字段的主消费方

## 6.1 `stageId`

主消费方：

- planner

次级消费：

- executor
- reporting

## 6.2 `kind`

主消费方：

- executor

次级消费：

- planner

原因：

- planner 只需要知道图结构
- executor 要靠 `kind` 选 worker

## 6.3 `scope`

主消费方：

- planner

次级消费：

- executor
- reporting

原因：

- planner 要根据 `scope=shared|matrix` 决定复用边界

## 6.4 `bucket`

主消费方：

- executor

次级消费：

- planner
- reporting

原因：

- executor 要按它写目录和 manifest

## 6.5 `dependsOn`

主消费方：

- planner

次级消费：

- executor

原因：

- 它首先定义执行顺序和失效传播

## 7. 这样拆分后的直接收益

## 7.1 registry 不会膨胀成执行器

registry 只做：

- 发现
- 校验
- 展示

不会去解释 worker 或 fingerprint。

## 7.2 planner 终于有独立角色

现在很多系统容易把：

- 选择 matrix
- 展开 graph
- 判定复用
- 执行 stage

全塞在一个执行器里。

这样后面越长越乱。

把 planner 单独拉出来后：

- `defaultGoal`
- `defaultMatrix`
- `supportedGoals`
- `pipelineId`
- `executionPipelines`
- `artifactPlan`
- `dependsOn`
- `scope`

这些字段都能有一个明确的主消费方。

## 7.3 executor 可以只关心真正的执行

executor 的输入应该已经是：

- 选定的 subject
- 选定的 matrix
- 展开的 stage plan
- 每个 stage 的 executionContext

它不应该再自己重新解释：

- 这条 matrix 合不合法
- 该选哪个 pipeline
- 是否要默认回退到别的 matrix

## 7.4 reporting 不再篡改执行语义

reporting 只读取已经执行过的结果和 manifest，不重新发明：

- stage 顺序
- graph 结构
- 复用策略

这样报告层就能保持稳定。

## 8. 首批代码层面的最小模块划分建议

如果未来进入实现，建议按下面拆：

- `subject_registry`
  - 扫描 subject manifest
  - 静态校验字段关系
- `subject_planner`
  - 解析 goal/matrix/pipeline
  - 产出 stage execution plan
  - 决定 executed/reused/invalidated
- `subject_executor`
  - 调度各 stage worker
  - 写 bucket 产物
- `subject_reporting`
  - 写 matrix report / subject summary / session summary

## 9. 当前可以正式固定的规则

这次可以正式固定为：

- manifest 字段必须有主消费方
- `executionPipelines + pipelineId + supportedGoals + artifactPlan`
  - 主消费方是 planner
- `executionContext + stage.kind + stage.bucket`
  - 主消费方是 executor
- `displayName + adaptationLevel + expectedOutcome`
  - 更偏 reporting
- registry 只做静态发现和静态校验，不解释 worker 语义

## 10. 这一步之后最自然的下一个问题

如果继续往下收敛，最自然的下一个问题就是：

- planner 的最小输出对象长什么样

因为现在 manifest 输入侧已经比较完整了，下一步就可以定义：

- planner 输出给 executor 的 stage plan 结构
