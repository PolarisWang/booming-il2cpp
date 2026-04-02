# Trace Contract v0 验证说明

## 定位

本文定义的是 warmup trace 的验证 contract，不是核心 ABI contract，也不是 runtime bridge contract。它的用途是验证 lazy warmup 的事件顺序与最终结果是否符合预期，而不是承载运行时 ABI 细节。

## 适用范围

- 验证对象：lazy warmup 路径中的类型、方法、泛型上下文、委托等主体。
- 验证目标：顺序是否正确、最终状态是否正确、缓存或 replay 路径是否按预期出现。
- 非目标：native bridge 参数布局、ABI 兼容层、平台桥接二进制细节。

## 顶层结构

- 顶层 `formatVersion` 固定为 `"v0"`。
- 顶层 `traceName` 固定为 `warmupTrace`。
- `targetPlatform` 当前冻结为 `windows` 或 `macos`。
- `samples[]` 承载可读的验证样本；每个 sample 至少包含 `sampleId`、`scenario`、`events[]`。

## 事件最小冻结字段

每条事件至少包含以下字段：

- `eventName`
- `phase`
- `subjectKind`
- `subjectId`
- `order`
- `status`

这些字段名在 v0 内禁止随意改动。允许在事件对象上追加调试、来源、耗时、触发器等字段，但不能替换这组最小冻结字段。

## 枚举约束

- `phase` 只允许：`requested`、`started`、`completed`、`failed`、`skipped`、`cached`、`replayed`
- `status` 只允许：`ok`、`error`、`skipped`、`cached`
- `subjectKind` 只允许：`type`、`method`、`field`、`property`、`event`、`parameter`、`genericContext`、`delegate`、`assembly`

## 命名规则

- `eventName` 默认使用三段式 `<domain>.<subject>.<action>`。
- 当 subject 不需要单独显式表达时，允许两段式 `<domain>.<action>`。
- `domain` 只允许：`warmup`、`metadata`、`reflection`、`generic`、`delegate`、`bootstrap`、`interop`。
- v0 不冻结 action 词表，但要求 action 使用稳定、可读、低歧义的英文动词或名词。

## `subjectId` 规则

- `subjectId` 使用人读签名式字符串，而不是内部指针或随机标识。
- 最少应包含程序集、类型、成员签名这三类信息中的适用子集。
- 类型示例：`Game.Core/Game.Player`
- 方法示例：`Game.Core/Game.Player::TakeDamage(System.Int32)`
- 泛型场景必须带出实例化形态，例如：`Game.Core/Game.Inventory::AddItem<System.String>(System.String)`
- 委托或组合场景可以在保持人读性的前提下追加 `=>` 等连接语法，只要核心签名仍稳定可识别。

## Snapshot 约束

- Windows snapshot 至少包含一份 sample，并覆盖 lazy warmup 路径。
- macOS snapshot 至少包含一份 sample，并覆盖 lazy warmup 路径。
- 两个平台样本合计必须覆盖 `cached` 或 `replayed` 路径；当前基线中 Windows 覆盖 `cached`，macOS 覆盖 `replayed`。
- snapshot 是验证基线，不是 runtime 生产 ABI 的一部分。

## 演进原则

- v0 允许为事件追加耗时、线程、来源组件、错误描述等字段。
- 如果未来需要改变最小冻结字段名称、枚举语义或顶层结构，应通过提升 `formatVersion` 完成，而不是在 v0 内静默漂移。
