# Stage 5 Post-Proof Split Roadmap v1-01

Date: 2026-04-06 12:48:45 +08:00
Status: completed

## 1. 目标

把 Stage 4 已跑通的 `HelloWorldObject` native reference proof，拆成稳定的 post-proof follow-on 路线。
目标不是在同一轮同时扩样例与扩平台，而是先把顺序、边界和交接点冻结成可执行资产。

## 2. 范围边界

本 roadmap 负责：

- 冻结 `GenericEcho -> ReflectionLite -> PInvokeLite` 的 Windows sample-first 路线。
- 把 `macOS` parity 延后到 Windows trilogy 之后，并明确其首轮覆盖子集。
- 维护 Stage 5A / 5B / 5C / 5D 的 parent-child 映射与阶段边界。

本 roadmap 不直接承载：

- 每个 follow-on 样例的实现级 chunk 细节。
- `macOS` parity 的真实宿主构建、链接、运行执行。
- 任何把样例特化逻辑写回 `src/**` 的做法。

## 3. 非目标

- 不在本轮同时推进新的语义样例与新的宿主平台。
- 不把完整 smoke `PInvokeLite`、`Marshal`、`NativeLibrary` 或 export 语义混入 Stage 5C 第一轮。
- 不在 Windows 会话里伪造 `macOS` 真机 / 真宿主 proof 结果。

## 4. 阶段列表

### Stage 5A：GenericEcho Windows Reference

- 在现有 `Windows x64` reference proof spine 上，验证第一条 follow-on 的 generic/object/call 路径。

### Stage 5B：ReflectionLite Windows Reference

- 在保持 Windows host 不变的前提下，引入最小 reflection / metadata 查询能力。

### Stage 5C：PInvokeLite Windows Reference

- 在 Windows host 上引入最小 interop proof，并保持 Stage 4 / Stage 5A / Stage 5B gate 不回退。

### Stage 5D：macOS Reference Parity

- 冻结首轮 `macOS` parity handoff 计划，而不是在 Windows 会话里直接实现它。
- 首轮 parity 子集固定为 Stage 4 `HelloWorldObject` + Stage 5A `GenericEchoClosedMinimal` + Stage 5B `ReflectionLiteQueryMinimal`。
- 当前 Stage 5C `kernel32.dll` / `MulDiv` proof 明确保留为 Windows interop foundation，不纳入首轮 `macOS` parity。

## 5. 每阶段完成定义

### Stage 5A

- `goal`: 用 `GenericEcho` 验证 Stage 4 proof spine 之上的第一条 follow-on 样例语义。
- `exit_criteria`:
  - `GenericEcho` 在 `Windows x64` reference host 上具备独立 proof/gate。
  - Stage 4 `HelloWorldObject` gate 持续通过。
  - `src/**` 保持 sample-agnostic。
- `deliverables`:
  - 子任务 `20260406-04-stage-5a-generic-echo-windows-reference`
  - `GenericEcho` proof spec / design / plan 文档
  - `GenericEcho` proof/gate 接入

### Stage 5B

- `goal`: 在 Stage 5A 稳定后，用 `ReflectionLite` 引入最小 reflection / metadata 查询能力。
- `exit_criteria`:
  - `ReflectionLite` 在 `Windows x64` reference host 上具备独立 proof/gate。
  - Stage 4 与 Stage 5A gate 持续通过。
  - 不把 reflection 样例特化逻辑写入 `src/**`。
- `deliverables`:
  - 子任务 `20260406-05-stage-5b-reflection-lite-windows-reference`
  - `ReflectionLite` proof spec / design / plan 文档
  - `ReflectionLite` proof/gate 接入

### Stage 5C

- `goal`: 在 Windows host 上引入最小 interop / `DllImport` proof。
- `exit_criteria`:
  - `PInvokeLiteDllImportMinimal` 在 `Windows x64` reference host 上具备独立 proof/gate。
  - Stage 4 / Stage 5A / Stage 5B gate 持续通过。
  - import metadata 与 generated glue 的样例特化不泄漏进生产代码。
- `deliverables`:
  - 子任务 `20260406-06-stage-5c-pinvoke-lite-windows-reference`
  - 最小 `DllImport` proof spec / design / plan 文档
  - `PInvokeLiteDllImportMinimal` proof/gate 接入

### Stage 5D

- `goal`: 冻结首轮 `macOS` parity handoff 计划，并明确哪些 Windows proof 可以直接迁移、哪些不能。
- `exit_criteria`:
  - 首轮 `macOS` parity 子集明确固定为 Stage 4 + Stage 5A + Stage 5B。
  - 当前 Stage 5C 明确作为 Windows-only interop foundation 保留，不伪装成跨宿主 parity。
  - `macOS` 宿主上的未来执行入口、验证命令与边界约束明确写入计划。
- `deliverables`:
  - 子任务 `20260406-07-stage-5d-macos-reference-parity`
  - `macOS` parity brainstorm / design / plan 文档
  - 首轮 parity 范围与 Stage 5C 排除说明

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260406-04-stage-5a-generic-echo-windows-reference` | `Stage 5A` | `completed` | `codex` | 在现有 Windows reference proof spine 上收窄最小闭合泛型 proof，并完成 `GenericEcho` 的第一条 follow-on 路径闭环 | `20260406-02-stage-4-native-bootstrap-and-reference-proof` |
| `20260406-05-stage-5b-reflection-lite-windows-reference` | `Stage 5B` | `completed` | `codex` | 在 Windows host 上引入最小 query-only reflection / metadata 查询 proof，并保持 Stage 4 / Stage 5A 基线不回退 | `20260406-04-stage-5a-generic-echo-windows-reference` |
| `20260406-06-stage-5c-pinvoke-lite-windows-reference` | `Stage 5C` | `completed` | `codex` | 在 Windows host 上引入最小 `DllImport` 整数 import proof，并把 interop 第一轮收窄到可诊断的 Windows reference 切片 | `20260406-05-stage-5b-reflection-lite-windows-reference` |
| `20260406-07-stage-5d-macos-reference-parity` | `Stage 5D` | `completed` | `codex` | 冻结首轮 `macOS` parity handoff，明确 Stage 4 + Stage 5A + Stage 5B 为首批 parity 子集，并显式排除当前 Windows-only 的 Stage 5C sample | `20260406-04-stage-5a-generic-echo-windows-reference, 20260406-05-stage-5b-reflection-lite-windows-reference, 20260406-06-stage-5c-pinvoke-lite-windows-reference` |

说明：

- Stage 5A 已拿到真实 `generic-ok` / `exit code 0` 证据。
- Stage 5B 已拿到真实 `reflection-ok|...` 证据。
- Stage 5C 已拿到真实 `pinvoke-ok|14` 证据，但该 proof 明确绑定 Windows `kernel32.dll` / `MulDiv`。
- Stage 5D 已把首轮 `macOS` parity 收窄为可执行 handoff，而不是继续把 Windows-only interop 样例伪装成跨宿主 contract。

## 7. 依赖

全局依赖关系如下：

1. Stage 4 先提供真实可运行的 Windows reference proof spine。
2. Stage 5A 基于这条 spine 扩第一条 follow-on 样例。
3. Stage 5B 再在 Windows 上补最小 reflection / metadata 查询面。
4. Stage 5C 最后在 Windows 上引入最小 interop / `DllImport` foundation。
5. Stage 5D 不直接消费 Stage 5C 的当前 proof 样例，而是基于 Stage 5C 的结论冻结 parity 排除边界。

## 8. 风险

- 如果不把样例扩张与平台扩展分开，Stage 5 很容易重新膨胀成 semantic/runtime/platform 三轴混调的大任务。
- 如果后续 `macOS` parity 绕开 Stage 4 / Stage 5A / Stage 5B 已有 proof gate，proof spine 仍可能悄悄回退。
- 如果把当前 Stage 5C `kernel32.dll` / `MulDiv` 路径误当作跨平台 interop contract，会直接污染后续 parity 判断。

## 9. 备选路径

### 方案 A：样例优先，先走完 Windows trilogy，再冻结 `macOS` parity handoff

- 优点：最大化复用 Stage 4 已跑通的 Windows proof spine，问题定位最聚焦。
- 缺点：`macOS` 真正实现会延后到独立宿主会话。

### 方案 B：平台优先，先补 `macOS` parity

- 优点：更快恢复双 reference desktop 叙事。
- 缺点：会把当前主 blocker 切到新的 host/platform 面，打散 Windows trilogy 的诊断闭环。

### 方案 C：样例和平台同时展开

- 优点：表面推进速度最快。
- 缺点：风险最高，最容易重新扩张成新一轮大 roadmap。

当前采用：

- **方案 A**

## 10. 当前建议推进顺序

1. Stage 5A `GenericEcho Windows Reference` 已完成归档。
2. Stage 5B `ReflectionLite Windows Reference` 已完成归档。
3. Stage 5C `PInvokeLite Windows Reference` 已完成归档，Windows interop foundation 已闭环。
4. Stage 5D `macOS Reference Parity` 已完成首轮 handoff 计划归档；后续真正执行需在 `macOS` 宿主上按该计划重新开启。
