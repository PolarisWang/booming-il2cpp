# Stage 5C PInvokeLite Windows Reference Brainstorm

Date: 2026-04-06 16:05:00 +08:00
Status: concluded

## 任务意图

Stage 5B 已经证明最小 query-only `ReflectionLite` 能沿现有 `Windows x64` reference proof spine 跑通真实 build/run，并拿到 `reflection-ok` 证据。  
Stage 5C 的任务不是立刻把现有完整 smoke `PInvokeLite` 全量搬到这条 spine 上，而是先找出“interop / native boundary 第一次真正卡住的最短路径”，再用一个最小 proof 把它打穿。

## Stage 5C 必须守住的边界

- 继续以 Stage 4 `HelloWorldObject`、Stage 5A `GenericEcho` 与 Stage 5B `ReflectionLite` gates 作为固定基线，不允许回退。
- 生产代码继续 sample-agnostic；任何 `PInvokeLite` 特化都只能留在 `tests/**`、`artifacts/**` 与任务文档中。
- 第一轮只留在 `Windows x64` reference spine，不并行展开 `macOS` parity。
- 第一轮只覆盖最小 import 子集，不把 `UnmanagedCallersOnly`、`Marshal`、`NativeLibrary`、导出符号解析与导出反射一口气带进来。

## 备选方案

### 方案 A：直接拿现有 smoke `PInvokeLite`

- 做法：直接尝试让 `tests/smoke/input/PInvokeLite/Program.cs` 通过当前 Stage 4 / Stage 5A / Stage 5B spine。
- 优点：
  - 与现有 smoke 样例完全一致。
  - 一次性覆盖 `DllImport`、native export、符号解析与基础 marshalling。
- 缺点：
  - `DllImport`、`UnmanagedCallersOnly`、`Marshal`、`NativeLibrary` 与导出反射会把首轮 blocker 混成多个 native boundary 同时失败。
  - 很难判断失败到底来自 import descriptor、generated import glue、runtime/loader 边界，还是更高阶 interop surface。

### 方案 B：最小 `DllImport` import-only proof

- 做法：新建一个最小 proof 输入，只覆盖：
  - `[DllImport("kernel32.dll", ExactSpelling = true)]`
  - 一个稳定的整数参数/返回值 native 调用，例如 `MulDiv(6, 7, 3) -> 14`
  - 稳定字符串输出
- 优点：
  - 能真实覆盖 “managed `DllImport` declaration -> descriptor materialization -> generated native import call -> proof run” 这条最短 interop 路径。
  - 可以把首轮 blocker 收敛到 import descriptor、codegen import glue 与 Windows link/run 边界。
  - 不会过早把 export、动态加载和 UTF-8 marshalling 混进来。
- 缺点：
  - 第一轮并不等于完整 smoke `PInvokeLite` parity。

### 方案 C：先做 export-only `UnmanagedCallersOnly`

- 做法：先围绕 `UnmanagedCallersOnly` 导出方法、符号名与反射属性做 proof。
- 优点：
  - 可以先冻结 native export contract。
- 缺点：
  - 首轮 blocker 会偏到导出 naming、host lookup 与反射验证，而不是 `DllImport` 主线。
  - 会推迟最基础 import path 的闭环。

## 当前推荐

- 采用 **方案 B**。

## 额外判断

Stage 5C 适合从 Stage 5 roadmap 进入单个 `plan` 子任务：

1. 当前真实 blocker 已经足够聚焦到“最小 `DllImport` import 子集”。
2. 可以先用 TDD 写出 import-only RED，再逐层补 managed descriptor、generated import glue 与 proof host。
3. `UnmanagedCallersOnly`、`Marshal`、`NativeLibrary`、导出反射等更高阶 surface 应留在 Stage 5C 后续扩展或 follow-up，而不是混入第一轮派生任务。
