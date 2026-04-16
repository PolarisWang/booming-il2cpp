# Phase 0 Deferred Surface Freeze And Contract Ledger Design v1.01

Date: 2026-04-16 00:31:55 +08:00
Status: completed

## 1. 目标

把 post-phase-7 仍然 deferred 的 AOT shape 统一落到显式 contract ledger，避免继续以零散报错驱动 widening。

## 2. 设计决策

- capability ledger 按 capability family 记账，pipeline stage 作为影响维度而不是主键。
- owner subject / proof / benchmark / gate 只消费 contract，不反向定义 AOT core 语义。
- `supported / missing evidence / unsupported` 必须在 reporting 与 release evidence 层保持统一口径。

## 3. 实际落点

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `build/toolchains/run/testing/capability_coverage.py`
- `build/toolchains/run/testing/release_evidence_contracts.py`
- `build/toolchains/run/testing/declared_metadata_labels.py`
- `build/toolchains/run/testing/registry.py`
- `build/toolchains/run/testing/subject_reporting.py`

## 4. 完成定义

- deferred surface 已映射到 capability family / owner subject / support state。
- release evidence contract 能区分 `ok`、`missingProof`、`missingBenchmark`、`unsupported`。
- 后续 Phase 1-5 不再需要重新发明 contract 名义层。
