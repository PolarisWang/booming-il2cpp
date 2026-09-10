# Brainstorm v1-01

## 背景

当前 `Phase 2` 还没有任何 async/await proof。现有主线的关键缺口有两类：

- `LoaderStage` 目前只把顶层类型的方法放进 `methodOwners`，编译器生成的嵌套 state machine 方法不会进入闭包。
- 现有 runtime/codegen 仍是 proof-level 方案，不具备通用 `Task` / continuation runtime，因此不能直接把“完整 async runtime”当成本轮最小目标。

## 候选方案

### 方案 A：完整实现通用 async runtime

- 内容：引入 `Task`/awaiter/runtime continuation 的真实执行路径。
- 优点：能力最完整。
- 缺点：范围过大，会同时击穿 runtime-core、bootstrap、metadata、codegen；不适合作为 Batch 2 的第一轮入口。

### 方案 B：proof-first 的最小 async slice

- 内容：只支持 `async Task<int>` 的单 proof 形态，主线补齐 loader/semantic/linker/codegen/subject 的表面，并让 lowering 在 native proof 里同步化输出结果。
- 优点：能快速建立 Phase 2 Batch 2 的 RED/GREEN 闭环；改动面可控。
- 缺点：仍是 proof-level，不是通用 async runtime。

### 方案 C：只做语义标记，不做 proof

- 内容：只加 capability 和 reachability，不做 codegen/subject。
- 优点：开发最快。
- 缺点：无法提供可验证证据，容易形成假完成。

## 选择

选择方案 B。

## 最小 proof slice

- subject：`AsyncAwaitProof`
- source 形态：`async Task<int> ComputeAsync()`，内部包含单次 `await Task.FromResult(...)`
- entry 形态：同步入口 `Run()`，通过 `.GetAwaiter().GetResult()` 取出结果并输出
- 语义目标：
  - loader 能拿到编译器生成的嵌套 state machine 方法
  - semantic 能标记 async state machine / awaiter surface
  - linker 能把 state machine 闭包保留下来
  - codegen 能选出单独 lowering family

## 非目标

- 不在本轮实现通用 continuation scheduler
- 不在本轮实现 `ValueTask` / iterator / 多 await / 自定义 awaiter
- 不在本轮把 runtime-core 升级成完整 Task runtime
