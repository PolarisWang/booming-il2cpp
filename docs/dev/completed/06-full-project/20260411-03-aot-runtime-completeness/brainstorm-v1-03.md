# Brainstorm v1-03

## 背景

`Phase 2 / Batch 4` 原计划同时覆盖两条能力线：

- nested exception handling
- marshaling / reverse P/Invoke

当前代码基线已经具备的前置条件是：

- loader 已经能解码 `ExceptionRegionKind.Catch` / `Finally` / `Fault` / `Filter`
- `MainlineFeaturePack` 已有 simple `throw/catch/finally` proof
- `PInvokeLite` 已有 `DllImport` / `Marshal` / `UnmanagedCallersOnly` 的源级样例
- codegen 已有 simple exception family 与 simple `DllImport` family

但这两条线的落地成本并不对称：

- nested EH 可以沿现有 exception proof family 做窄扩展
- marshaling / reverse P/Invoke 需要额外处理 callback 边界、delegate ABI、native -> managed 回跳，范围明显更大

因此 Batch 4 不能同时深挖两条线，否则会把 proof-first 节奏重新拉回“大一统实现”。

## 候选方案

### 方案 A：优先落地 Nested EH 最小闭环

- 内容：
  - 新建 `NestedExceptionProof`
  - 只覆盖嵌套 `try/catch/finally`
  - semantic / linker / codegen 增补 nested EH 的最小识别与 lowering family
  - native template 继续沿用 `ManagedExceptionCarrier`，不扩展 runtime-core 公共 ABI
- 优点：
  - 可以最大化复用现有 exception surface
  - RED -> GREEN 路径短
  - 能先把 Batch 4 最容易失焦的能力线冻结下来
- 缺点：
  - `filter` / `fault` 仍停留在 loader 可见、proof 未闭环的状态
  - marshaling / reverse P/Invoke 还要留到后半段

### 方案 B：优先落地 Marshaling / reverse P/Invoke

- 内容：
  - 新建 `MarshalingProof`
  - 直接补 blittable marshal、string marshal、reverse P/Invoke callback
- 优点：
  - 一次性推进 interop 缺口
- 缺点：
  - 需要更大的设计面
  - 现有 proof 基线对 reverse P/Invoke 支撑最弱
  - 更容易把 Batch 4 做成长尾任务

### 方案 C：并行推进 nested EH 与 marshaling

- 内容：
  - 同时建两个 subject 和两条 lowering family
- 优点：
  - 表面覆盖看起来最完整
- 缺点：
  - 超出当前 proof-first 节奏
  - 验证面、状态维护和回归成本一起上升

## 选择

选择方案 A。

## 最小 proof slice

- subject：`NestedExceptionProof`
- source 形态：
  - 单个 `NestedExceptionHarness`
  - 一个嵌套 `try/catch/finally` 方法
  - 内层 `catch (InvalidOperationException ex)`
  - 内层、外层各一个 `finally`
- semantic 目标：
  - 新增 `requires-nested-exception-handler`
- linker 目标：
  - 冻结 nested EH shape 判定
  - 为 lowering 选择保留 `nested-throw-catch-finally` 事实入口
- codegen 目标：
  - 新增 family `managed-exceptions.nested-throw-catch-finally.minimal`
  - 新增 `NativeReferenceProof.NestedExceptionThrowCatchFinally.cpp.scriban`
- runtime 目标：
  - 继续复用 `ManagedExceptionCarrier`
  - 本轮不新增 runtime-core 公共 helper

## 非目标

- 本轮不做 `filter` / `fault` 的完整 proof 闭环
- 本轮不做 blittable struct marshal / string marshal 的新 lowering
- 本轮不做 reverse P/Invoke callback
- 本轮不新增 exception dispatcher ABI
