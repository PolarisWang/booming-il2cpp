# Phase 1 Loader And Metadata Decode Expansion Brainstorm v1.01

Date: 2026-04-15 20:43:14 +08:00
Status: active

## 1. 目标

本阶段不去提前处理 planner / emitter widening，而是先把目标 capability 稳定送进 `typed-il` 输入面：

- `calli`
- function pointer signature
- loader 仍然硬失败的 entry / handle / opcode decode 缺口

## 2. 首批切入点

首批选 `FunctionPointerProofEntry::Run()` 与 `FunctionPointerBenchmarkEntry::RunWorkload()`，原因：

- retained owner subject 已存在，不需要新增临时 subject
- 这条路径天然覆盖 `ldftn` + `calli`
- 当前 `LoaderStage.DecodeCalliInstruction()` 只保留原始 token，`ILToIRLowering.GetCallInputCount()` 又要求可解析 callee subject id，是真实断点

## 3. 执行原则

- 先 RED，再改实现
- 不把 `Chaos.TestFramework` 逻辑混进 AOT core / loader contract
- 本阶段只补 decode / identity / signature shape；真正 native emission widening 留给后续 Phase 4

## 4. 阶段内批次

1. Batch A: `calli/function pointer` 真实闭环
2. Batch B: `ldtoken` / method-spec / handle-kind 补齐
3. Batch C: loader opcode decode 与 metadata provider 遗留缺口收口
