# Phase A Mainline Complete 定义 v1.01

Date: 2026-04-09
Status: frozen-for-phase-1-unblock

## 1. 文档目的

本文件定义当前 `il2cpp-mainline` 主线里，“complete”到底指什么。

这里的 complete 不是：

- total solution complete
- desktop/mobile 全平台 complete
- 全量高阶 `BCL` 语义 complete

这里的 complete 指的是：

- **`Phase A Mainline Complete`**

## 2. 定位

`Phase A Mainline Complete` 的含义是：

- 当前仓库已经形成一条可持续扩展的、以 generated native 性能为第一目标的 IL2CPP core mainline
- 当前仓库已经具备一套可以承接后续 capability 补全的完整 subject 与验证主线
- 当前仓库已经明确哪些内容属于当前主线交付，哪些内容属于后续 total-solution follow-up

## 3. 语义完整度

要被称为 `Phase A Mainline Complete`，当前主线至少必须覆盖以下强制交付语义：

- `virtual/interface dispatch`
- `closed generics specialization + layout`
- `arrays + boxing/unboxing`
- `delegates/callbacks`
- `exceptions`
- 必要的 `reflection/interop` 补位

这里的“必要”指：

- 能支撑 `MainlineFeaturePack` 的正式 capability slices
- 能支撑 current mainline proof / trace / native perf 主线
- 能支撑后续 `OptimizationFacts -> CodeGen -> Emitter` 主线稳定演进

当前不纳入本轮完成定义的语义包括：

- 全量高阶 `BCL` 语义兼容
- 超出当前 capability matrix 的额外语言/库表面
- `engine binding`

## 4. 平台完整度

`Phase A Mainline Complete` 当前只要求以下平台收口成立：

- `Windows reference`
- `Windows native`

这意味着：

- 当前主线必须能在 Windows host 上完成 reference / native / trace / perf 的统一验证
- `.NET 10` 输入兼容不属于当前完成定义
- `macOS parity` 不属于当前完成定义
- Android / iOS runtime subset 不属于当前完成定义

这些项不是被否定，而是被明确放到后续 total-solution follow-up。

## 5. 工程完整度

工程上，只有同时满足以下条件，才能称为 `Phase A Mainline Complete`：

- 保留现有最小 probe subjects：
  - `HelloWorldObject`
  - `GenericEcho`
  - `ReflectionLite`
  - `PInvokeLite`
- 新增独立完整 subject：
  - `MainlineFeaturePack`
- `MainlineFeaturePack` 不是巨型 smoke，而是 feature-sliced 主线宿主
- 存在统一的 `native output / trace / perf` 验证入口
- 存在正式 perf baseline 与 report gate
- `OptimizationFacts` 是正式 contract，而不是隐式推理
- `engine binding` 仍位于整个 roadmap 最后阶段

## 6. 对 roadmap 的直接约束

从本文件开始，后续 roadmap / child task 不得再把以下说法混用：

- “当前主线 complete”
- “完整 `C# -> C++ total solution` complete”

允许的准确表述只有：

- 当前主线达成了 `Phase A Mainline Complete`
- 当前主线尚未达成 total solution complete

## 7. 与 total solution 的关系

当 `Phase A Mainline Complete` 成立后，后续仍可继续扩展：

- `.NET 10`
- `macOS parity`
- Android / iOS runtime subset
- 更完整的 BCL/semantic surface
- `engine binding`

因此，这份完成定义的作用不是缩小目标，而是先把当前这轮主线说准。

## 8. 冻结结论

从现在开始，当前 roadmap 的“complete”默认解释为：

- `Phase A Mainline Complete`

而不是：

- total solution complete
