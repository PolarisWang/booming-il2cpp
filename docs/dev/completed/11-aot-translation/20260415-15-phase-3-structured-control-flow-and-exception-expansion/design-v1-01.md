# Phase 3 Structured Control Flow And Exception Expansion Design v1.01

Date: 2026-04-15 22:04:44 +08:00
Status: active

## 1. 设计目标

让 native AOT planner 从 catch-only 最小异常形状，扩到 retained owner proof 真正需要的 structured EH / control-flow contract：

- `finally`
- `filter`
- `leave`
- `endfinally`
- 与上述形状共存的最小条件分支

## 2. 本阶段边界

本阶段负责：

- retained owner proof 的最小去噪
- `NativeAotLoweringPlanner` 对 structured EH / control-flow 的 widening
- 对应 compatibility RED/GREEN tests

本阶段不负责：

- virtual dispatch widening
- workload entry ABI widening
- emitter/template 的大规模 contract 扩张
- dashboard / reporting / gate 收口

## 3. Batch A：Finally/Filter Owner Proof

### 3.1 真实驱动

- proof: `CoreRuntimeFeatures/FinallyAndFilterProofEntry::Run()`

### 3.2 当前阻塞

- 当前 proof 的 filter 条件依赖 `ArgumentException.get_ParamName()`，native AOT 在 dispatch slot 解析阶段就失败。
- 这个 blocker 不属于 structured EH 本身，必须先从 owner proof 中剥离。

### 3.3 最小 contract

Batch A 至少需要：

- proof 使用 dispatch-free filter 条件，纯粹表达 filter / finally 语义
- planner 能消费 `AotCoreIrExceptionRegionKind.Finally / Filter`
- planner 能处理 proof 所需的 `leave / endfinally` 与最小条件分支
- generated C++ 对应 EH/control-flow 形状可被真实 emit

## 4. Batch B：Switch / Broader Structured Control Flow

- 在 Batch A 稳定后，再决定是否在同一 child task 内继续推进 `switch` 与更广泛的 structured branch。
- 若 Batch A 已经暴露出更深的 planner / emitter contract 问题，优先先把 finally/filter 关掉，不做范围膨胀。

## 5. 验证策略

- 先用 `FinallyAndFilterProofEntry::Run()` 锁定 RED
- 再跑 targeted compatibility suites
- 本阶段结束前至少跑一轮完整 `tests/unit/compatibility -q`
