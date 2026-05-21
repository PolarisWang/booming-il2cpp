# Stage 5D macOS Reference Parity Design

Date: 2026-04-06 16:19:51 +08:00
Status: ready-for-plan

## 设计目标

冻结首轮 `macOS` reference parity 的 handoff 边界，让未来真正切到 `macOS` 宿主执行时，有一份不自欺、可直接接手的实现计划。

## 设计边界

### 纳入

- 首轮 parity 子集的冻结：
  - Stage 4 `HelloWorldObject`
  - Stage 5A `GenericEchoClosedMinimal`
  - Stage 5B `ReflectionLiteQueryMinimal`
- `macOS` 宿主未来执行所需的入口、验证命令与边界约束。
- 对当前 Stage 5C 的显式排除说明。

### 不纳入

- 在 Windows 宿主上直接构建或运行真实 `macOS` proof。
- 重写 Stage 5C，或把 `kernel32.dll` / `MulDiv` 改造成跨平台 sample。
- iOS / Linux packaging 的额外扩张。

## 关键设计结论

1. Stage 5D 这轮是 handoff plan，而不是 Windows 会话里的伪实现。

原因：

- 当前宿主不是 `macOS`。
- Stage 5 父 roadmap 的职责是完成 follow-on 路线拆分与交接，而不是跨宿主硬闭环。

2. 首轮 `macOS` parity 子集固定为 Stage 4 + Stage 5A + Stage 5B。

原因：

- 这三条路径已经在 Windows 上拿到稳定 proof 证据。
- 它们代表 object / generic / reflection 三条核心 follow-on 语义轴。

3. 当前 Stage 5C 明确不纳入首轮 parity。

原因：

- `PInvokeLiteDllImportMinimal` 绑定 `kernel32.dll` / `MulDiv`。
- 这条 proof 证明的是 Windows import foundation，而不是跨宿主 interop contract。

4. `src/**` 继续保持 sample-agnostic。

要求：

- 任何未来 `macOS` parity 接入都只能新增通用 host / toolchain / proof wiring。
- 不允许把 `HelloWorldObject`、`GenericEchoClosedMinimal`、`ReflectionLiteQueryMinimal` 的样例判断写回生产代码。

## 设计结果

Stage 5D 作为单个 `plan` 子任务完成归档。下一步不在本会话继续实现，而是把执行入口冻结到 `plan-v1-01.md`，等待未来的 `macOS` 宿主会话按计划接手。
