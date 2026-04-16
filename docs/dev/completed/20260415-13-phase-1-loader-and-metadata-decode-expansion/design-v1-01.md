# Phase 1 Loader And Metadata Decode Expansion Design v1.01

Date: 2026-04-15 20:43:14 +08:00
Status: active

## 1. 设计目标

让目标能力在 loader 阶段具备稳定、可消费的 typed contract：

- 指令级 `Op`
- 可消费的 `ResultType`
- 足够精确的 `ManagedInstructionReference`
- 必要时补充 `Callee` / signature shape，让下游 lowering 不再依赖模糊 token

## 2. 本阶段边界

本阶段负责：

- `LoaderStage`
- `Metadata/TypeProviders`
- 必要的 `MetadataTypeResolver`
- 必要的 `ManagedClosureContracts`
- 为现有 interpreter lowering 提供最小可消费 contract

本阶段不负责：

- `HybridDispatchKind.Bridge` / `ExternalRuntime`
- widened entry ABI
- NativeAOT emitter / Scriban 模板 widening

## 3. Batch A 设计

### 3.1 真实入口

- proof: `CoreRuntimeFeatures/FunctionPointerProofEntry::Run()`
- benchmark: `CoreRuntimeBenchmarks/FunctionPointerBenchmarkEntry::RunWorkload()`

### 3.2 当前断点

- `TypeProviders.GetFunctionPointerType()` 仍直接 `throw`
- `LoaderStage.DecodeCalliInstruction()` 仅保留原始 token，并把返回类型写死为 `System.Object`
- `ILToIRLowering` 在 `calli` 上无法解析输入参数个数，因为没有稳定 callee/signature metadata

### 3.3 最小 contract

Batch A 至少需要：

- loader 能把 `calli` 解析为稳定的 function-pointer signature contract
- `ManagedInstructionModel` 能携带 `calli` 所需的参数 / 返回 shape
- `ILToIRLowering` 对 `calli` 走独立输入计数路径，不再错误复用 method subject id 解析

## 4. 验证策略

- 单元 / 真实驱动测试双重验证
- 以 driver 输出的 `typed-il-ir.json` / `aot-core-ir.json` 为主证据
- proof 与 benchmark 两条入口都要覆盖，避免只对 proof 生效
