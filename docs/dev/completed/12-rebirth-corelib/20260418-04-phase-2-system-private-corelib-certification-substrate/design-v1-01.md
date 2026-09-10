# Phase 2 System.Private.CoreLib Certification Substrate Design v1.01

Date: 2026-04-18 02:52:08 +08:00
Status: design-frozen-for-execution

## 1. 目标

把 `Phase 2` 收敛成 “`System.Private.CoreLib` 整 DLL certification substrate”，在不提前进入 per-family native emit 完成态的前提下，先冻结：

- `System.Private.CoreLib` 的完整 public surface inventory
- 面向后续 nativeization 的 semantic ledger 基础结构
- `runtime-backed` / `internal-call` / `pinvoke` / `managed-cil` 等 helper contract 输入层
- `corelib-surface-proof` 与 `corelib-semantic-proof` 的 formal object

## 2. 边界

### 2.1 本阶段负责

- 从 `.NET 8` / `.NET 10` foundation packs 选定 `System.Private.CoreLib.dll`
- 枚举 corelib 的完整公开 types / members / signatures
- 生成 `System.Private.CoreLib` semantic ledger
- 生成 runtime helper contracts inventory
- 生成 corelib surface / semantic proof 与 substrate benchmark

### 2.2 本阶段不负责

- `System.Private.CoreLib` per-family native emit 完成态
- 复杂 family 的真实 lowering 实现
- 其他 `System.*` BCL 分层 nativeization
- hotupdate consumer validation

## 3. 输入与输出

### 3.1 输入

- `Phase 0` catalog
- `Phase 1` 的 facade/shim classification 与 canonical owner authority
- `.NET 8` / `.NET 10` 的 `System.Private.CoreLib.dll` ref/runtime metadata

### 3.2 输出

- `assembly-semantic-ir-system-private-corelib-v1-01.json`
- `runtime-helper-contracts-v1-01.json`
- `corelib-surface-proof-report-v1-01.json`
- `corelib-semantic-proof-report-v1-01.json`
- `corelib-substrate-benchmark-v1-01.json`

## 4. 核心决策

### 4.1 `Phase 2` 仍然是 substrate，不是 completed nativeization

- 本阶段的完成标准是“所有公开 surface 与语义入口都被枚举、分类、证明”
- 本阶段不承诺所有 family 已经具备 native emit；那是 `Phase 3` 的职责

### 4.2 semantic ledger 的最小可信单位是公开成员

- ledger 必须至少记录每个公开方法/构造器/字段/属性/事件的：
  - owner type
  - member identity
  - signature arity / parameter count
  - implementation source classification
  - 是否存在 IL body
  - 是否依赖 runtime helper / internal-call / pinvoke / abstract/runtime semantics

### 4.3 helper contracts 先冻结“输入清单”，后续再冻结“实现”

- `runtime-helper-contracts-v1-01.json` 本阶段只负责把 helper obligations 枚举出来
- helper 的真实 native implementation 仍留到 `Phase 3+`

### 4.4 cross-version 规则继续保持单轨

- `net8` 与 `net10` 使用同一套 corelib ledger schema 与 proof rule
- 版本差异只能体现在 machine-readable diff 与 benchmark 指标里，不能裂变成双轨语义规则

## 5. 验证口径

- `corelib-surface-proof` 必须证明公开 surface 无漏项
- `corelib-semantic-proof` 必须证明公开成员全部有 semantic classification，不能存在未分类公开成员
- `runtime-helper-contracts` 必须能列出所有被识别的 helper obligations
- `benchmark` 必须记录 `net8` / `net10` 的成员规模与 ledger 生成耗时

## 6. 风险

- 如果 semantic ledger 粒度太粗，`Phase 3` 会重新退化回 proof-driven 补洞
- 如果 helper contract 只记“有 helper”而不记 helper 类型，后续 runtime substrate 会返工
- 如果把 corelib implementation lowering 提前混入 `Phase 2`，会再次打乱 substrate 与 implementation 的边界
