# Phase 1 Contract Facade And Shim Certification Lane Design v1.01

Date: 2026-04-18 02:09:22 +08:00
Status: design-frozen-for-execution

## 1. 目标

把 `Phase 1` 收敛成 “contract/facade/shim 可认证输入层”，在不提前进入 `System.Private.CoreLib` implementation 语义 native 化的前提下，先解决：

- 哪些 foundation DLL 属于 `contract-facade` / `compat-shim` lane
- 这些 DLL 的全部公开 surface 如何映射到 canonical owner
- 什么条件下可以把某个 facade/shim DLL 标记为 `contract-complete`

## 2. 边界

### 2.1 本阶段负责

- 基于 `Phase 0` catalog 选定 `Phase 1` candidate assemblies
- 产出 `assembly-classification-v1-01.json`
- 产出 `public-surface-manifest-v1-01.json`
- 产出 facade/shim `surface-proof-report-v1-01.json`
- 产出 facade/shim `native-binding-proof-report-v1-01.json`
- 产出 facade/shim `manifest-benchmark-v1-01.json`

### 2.2 本阶段不负责

- `System.Private.CoreLib` implementation member 的语义 lowering
- implementation DLL 的 per-method native emit
- hotupdate / consumer validation
- 复杂 BCL 或外部 DLL

## 3. 初始对象范围

### 3.1 Seed Assemblies

- `mscorlib.dll`
- `netstandard.dll`
- `System.Runtime.dll`
- `System.dll`
- `System.Core.dll`

### 3.2 Candidate Closure Rule

- 纳入 seed assemblies 的公开 `type forward` 目标
- 纳入 seed assemblies 的公开 contract dependencies
- 仅当目标 assembly 满足 `contract-facade` / `compat-shim` 特征时，才进入 `Phase 1` lane

### 3.3 Exclusion Rule

- `System.Private.CoreLib.dll` 作为 canonical implementation owner 允许被引用，但不在本阶段被视为 completed target
- 任何需要进入真实 implementation lowering 才能判定完成的 DLL，一律留到 `Phase 2+`

## 4. 核心决策

### 4.1 Assembly Classification

- `contract-facade`
  - 以公开 contract、type forward、surface identity 为主
  - 不要求本阶段产出 implementation-level native emit
- `compat-shim`
  - 保持兼容 surface 或转发表象
  - 必须证明公开 surface 可以绑定到 canonical owner
- `implementation`
  - 含真实实现语义，需要进入 `Phase 2+`
- `runtime-backed`
  - 公开 surface 依赖 runtime/helper 合约
  - 如 helper contract 未冻结，不允许在 `Phase 1` 被标为 completed

### 4.2 `contract-complete` Gate

某个 facade/shim DLL 只有同时满足以下条件，才允许进入 `contract-complete`：

- 全部公开 types / members 已被枚举
- 全部公开 `type forward` 已解析到 canonical owner
- 全部公开 surface 的 owner / binding target 已明确
- native binding proof 证明 facade/shim 侧的公开语义在 native 世界不存在 dangling target
- 不存在“等后面实现 DLL native 化后再补判定”的悬置项

### 4.3 Cross-Version Rule

- `net8` 与 `net10` 使用同一套 classification / owner 规则
- 版本差异只允许体现在 machine-readable diff 结果里，不允许变成双轨规则

### 4.4 Benchmark Rule

`Phase 1` 需要沉淀可监控 benchmark，但它衡量的是 lane 构建成本而不是运行期性能：

- candidate assembly count
- public member count
- type forward count
- manifest / proof generation elapsed time

## 5. 验证口径

- 所有 JSON 产物必须可解析
- candidate assembly inventory 必须可由 `Phase 0` catalog 重建
- surface proof 必须给出 `unresolved public types/members/type forwards = 0`
- native binding proof 必须证明 facade/shim 可绑定到 canonical owner 或 runtime implementation，不存在 dangling symbol target
- benchmark report 必须记录 `net8` / `net10` 的对象数与耗时

## 6. 风险

- 如果 facade/shim 与 implementation 分类混用，`contract-complete` 会出现假阳性
- 如果 candidate closure 过宽，会把 implementation 语义提前拉入 `Phase 1`，造成返工
- 如果 canonical owner 映射在 `net8` / `net10` 之间不稳定，`Phase 7` 的 delta closure 会失真
