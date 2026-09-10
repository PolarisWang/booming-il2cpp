# Phase 6 Engineering Archetype Benchmark And Release Gates 设计 v1.01

Date: 2026-04-15 20:25:00 +08:00

## 1. 设计目标

- 让 benchmark/dashboard/unsupported report/release evidence 全部消费 Phase 5 冻结后的 capability truth。
- 把 diagnostics `47/48/49/50` 从“缺 proof”改成“proof optional 的 reporting/evidence-chain contract”。
- 把 engineering archetype gate 改成围绕 retained owner subjects 与 declared benchmark 的真实执行结果。

## 2. 数据真值流

1. `capability_coverage.py`
   - 负责 `ownerSubjectId`、`supportStates`、`proofRequired`、`benchmarkRequired`
2. `registry.py`
   - 把上面的 contract 显式暴露给 declared unit tests / benchmarks
3. reporting / dashboard / unsupported report
   - 只消费 registry 暴露的 contract，不重复硬编码 family/item 规则
4. release evidence
   - 基于 reporting 聚合结果出具最终结论

## 3. 结构调整

### 3.1 reporting / dashboard

- 调整 benchmark dashboard 数据模型：
  - 增加 owner subject / capability family / capability item / support states / proofRequired / benchmarkRequired
  - 缺失 benchmark 数据时，输出 machine-readable reason
- 调整 unsupported report：
  - 将 diagnostics proof-optional item 标记为 `reportingOwned`
  - 区分 `missingProof`、`missingBenchmark`、`proofOptional`、`unsupported`

### 3.2 engineering gates

- 重做 engineering archetype benchmark gate：
  - full-solution archetype
  - owner-subject declared benchmark
  - release evidence summary
- 输出统一 gate summary，避免旧 benchmark gate 与新 owner coverage 各自为政

## 4. 验收标准

- benchmark dashboard 能正确展示 owner subject、capability item、managed baseline 与各 execution mode 的缺失原因。
- unsupported report 不再把 diagnostics `47/48/49/50` 算作 missing proof。
- engineering archetype gate 能以 full-solution + owner-subject 真值输出 release evidence。
