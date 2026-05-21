# Stage 5D macOS Reference Parity Brainstorm

Date: 2026-04-06 16:19:51 +08:00
Status: completed

## 背景

Stage 5A / Stage 5B / Stage 5C 的 Windows follow-on trilogy 已经闭环，但当前会话宿主仍是 Windows。
其中 Stage 5C 的最小 interop sample 明确绑定 `kernel32.dll` / `MulDiv`，因此它证明的是 Windows import path，而不是跨宿主 parity。

## 方案 A：首轮 parity 只覆盖 Stage 4 + Stage 5A + Stage 5B

- 做法：把 `HelloWorldObject`、`GenericEchoClosedMinimal`、`ReflectionLiteQueryMinimal` 作为首轮 `macOS` parity 子集，Stage 5C 显式排除。
- 优点：
  - 不需要为了追求“形式完整”而重开已归档的 Stage 5C 范围。
  - `macOS` parity 的第一轮仍然覆盖 object/generic/reflection 三条关键语义路径。
  - 与当前 Windows 宿主现实一致，适合先冻结 handoff 计划。
- 缺点：
  - `PInvokeLite` 不会进入首轮 `macOS` parity。

## 方案 B：强行把 Stage 5C 一起纳入首轮 parity

- 做法：要求 `macOS` parity 第一轮同时覆盖当前 `PInvokeLiteDllImportMinimal`。
- 优点：
  - 表面上最完整。
- 缺点：
  - 当前 sample 绑定 `kernel32.dll` / `MulDiv`，无法直接迁移到 `macOS`。
  - 会迫使本轮回头重设计 Stage 5C，而不是完成交接。

## 方案 C：首轮 parity 只保留 Stage 4

- 做法：只把 `HelloWorldObject` 迁到 `macOS`，把 Stage 5 follow-on 全部后置。
- 优点：
  - 平台切换面最小。
- 缺点：
  - 价值不足，无法消费已经稳定的 Windows follow-on 语义资产。

## 结论

采用 **方案 A**：

- Stage 5D 不在 Windows 会话里假装完成真实 `macOS` proof。
- 首轮 parity 子集固定为 Stage 4 + Stage 5A + Stage 5B。
- 当前 Stage 5C 作为 Windows-only interop foundation 保留，并在设计与计划中显式排除。
