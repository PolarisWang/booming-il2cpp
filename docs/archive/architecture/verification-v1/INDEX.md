# Verification V1

## 目标

这一组文档定义新的 formal verification 架构。它不是旧测试管线的附注，而是新的 authority、schema、投影和切换规则集合。

本文档包只保留最小必要集合：

- [`spec.md`](./spec.md)
  - 主规格。定义 graph-first 架构、closure/claim/evidence 模型、状态语义、归并归档规则与迁移原则。
- [`schema.md`](./schema.md)
  - formal object 和文件 contract。定义 `latest/master/reports`、core record、projection row 和 codegen stub 索引。
- [`ownership-map.md`](./ownership-map.md)
  - 六条主线的权责图审核口径。回答谁负责、谁阻断、谁产出 formal evidence。
- [`projection-and-reporting.md`](./projection-and-reporting.md)
  - formal source 到 `testing-inventory`、`benchmark`、主页视图和阶段报告的投影规则。
- [`cutover-and-rollout.md`](./cutover-and-rollout.md)
  - 实施与切换顺序。定义如何从现有系统迁到 `Verification V1`，以及何时退役旧路径。
- [`roadmap-v1-01.md`](./roadmap-v1-01.md)
  - 可执行 roadmap。定义阶段、子任务映射、旧测试路径替代方案、全覆盖策略与旧流程退役清单。

## 阅读顺序

推荐按以下顺序阅读：

1. [`spec.md`](./spec.md)
2. [`schema.md`](./schema.md)
3. [`ownership-map.md`](./ownership-map.md)
4. [`projection-and-reporting.md`](./projection-and-reporting.md)
5. [`cutover-and-rollout.md`](./cutover-and-rollout.md)
6. [`roadmap-v1-01.md`](./roadmap-v1-01.md)

## 何时看哪篇

如果你要回答“新验证系统到底长什么样”，看 [`spec.md`](./spec.md)。

如果你要回答“formal JSON 要写什么字段、写到哪里”，看 [`schema.md`](./schema.md)。

如果你要回答“这次 closure 为什么被挡住、哪个 lane 负责”，看 [`ownership-map.md`](./ownership-map.md)。

如果你要回答“inventory、benchmark、dashboard 应该从哪里取数据”，看 [`projection-and-reporting.md`](./projection-and-reporting.md)。

如果你要回答“接下来怎么落地、旧路径何时能删”，看 [`cutover-and-rollout.md`](./cutover-and-rollout.md)。

如果你要回答“现在具体按什么顺序执行、每一阶段要勾哪些 checklist”，看 [`roadmap-v1-01.md`](./roadmap-v1-01.md)。

## 约束

- `Verification V1` 的 formal source 只认 `verification/archive/` 与 `verification/evidence/owners/<OwnerPack>/codegen-stubs/`。
- `artifacts/` 只放临时过程产物。
- 不使用 `run-id` 作为 formal 归档目录。
- `supportState` 与 `verificationState` 必须分离。
- `failed / blocked / missing / unsupported` 不得混写。

## 非目标

这组文档不负责：

- 复用旧测试管线的目录设计
- 定义具体脚本命令行
- 永久保留 run-oriented 双写逻辑
- 在文档层重复罗列所有现有测试项
