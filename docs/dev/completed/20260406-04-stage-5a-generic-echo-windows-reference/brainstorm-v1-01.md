# Stage 5A GenericEcho Windows Reference Brainstorm

Date: 2026-04-06 13:05:16 +08:00
Status: concluded

## 任务意图

Stage 4 已经证明 `HelloWorldObject` 能在 `Windows x64` reference host 上走通真实 native proof。
Stage 5A 的任务不是立刻把完整 smoke `GenericEcho` 全量搬到这条 spine 上，而是先找出“泛型第一次真正卡住的最短路径”，再用一个最小 proof 把它打穿。

## Stage 5A 必须守住的边界

- 继续以 Stage 4 的 `HelloWorldObject` gate 作为固定基线，不允许回退。
- 生产代码继续 sample-agnostic；任何 `GenericEcho` 特化都只能留在 `tests/**`、`artifacts/**` 和任务文档里。
- 第一轮只留在 `Windows x64` reference spine，不并行展开 `macOS` parity。
- 第一轮只覆盖“闭合泛型静态方法 + 闭合泛型引用类型”这一最小组合，不直接吞下 tuple / 插值字符串 / reflection / interop。

## 备选方案

### 方案 A：直接拿现有 smoke `GenericEcho`

- 做法：直接尝试让 `tests/smoke/input/GenericEcho/Program.cs` 通过当前 Stage 4 spine。
- 优点：
  - 与现有 smoke 样例名字完全一致。
  - 一次性覆盖更多 generic 语义。
- 缺点：
  - tuple、插值字符串与额外 BCL 依赖会把第一轮 blocker 混成多层问题。
  - 很难判断失败到底来自泛型引用、额外 opcode，还是额外外部依赖。

### 方案 B：最小闭合泛型 proof

- 做法：新增一个最小 proof 输入，只保留：
  - 一个闭合泛型静态方法调用
  - 一个闭合泛型引用类型实例化
  - 一个最小字符串输出
- 优点：
  - 可以把 Stage 5A 的第一个真实 blocker 收敛到 `MethodSpecification` / `TypeSpecification`。
  - 更容易判断后续缺口到底是 loader、registration，还是 native proof emitter。
- 缺点：
  - 第一轮并不等于完整 smoke `GenericEcho` parity。

### 方案 C：只做泛型类型，不做泛型方法

- 做法：只覆盖 `EchoBox<string>` 这种闭合泛型类型，不覆盖 `Echo<string>` 方法实例化。
- 优点：
  - 改动更小。
- 缺点：
  - 无法真实覆盖 `MethodSpecification`，会留下最关键的缺口。

## 当前推荐

- 采用 **方案 B**。

## 额外判断

Stage 5A 已经适合从 roadmap 进入单个 `plan` 子任务：

1. 当前真实 blocker 已经明确，且集中在 managed pipeline 的泛型引用解析。
2. 可以先用 TDD 写出最小闭合泛型 RED，再逐层补 loader / closure bundle。
3. native reference proof 是否需要进一步泛化 generated path，应该在 managed closure 绿灯后再继续判断，而不是在 brainstorm 阶段混做。
