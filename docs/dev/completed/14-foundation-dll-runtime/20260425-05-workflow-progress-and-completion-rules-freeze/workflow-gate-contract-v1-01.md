# Workflow Gate Contract v1.01

Date: 2026-04-25 23:30:00 +08:00
Status: draft
Phase: phase-1 (20260425-05)
Parent: foundation-dll-capability-closure-reporting-roadmap

## 1. 目标

定义 foundation DLL reporting 的 workflow gate 模型，包括：
- gate 的分类和层级
- policyState / executionState 的正式口径
- Workflow Progress 的计算公式
- gate 状态与 Capability Closure 的关系

## 2. Gate 层级模型

Workflow gate 分为两层：

```
DLL 级 gate
  └── completion-certification (仅 1 个)

Family 级 gate (每个 semantic family 独立维护)
  ├── audit-input-and-ledger
  ├── managed-proof
  ├── native-proof
  ├── hotupdate-proof    (policy-driven)
  ├── benchmark          (policy-driven)
  └── codegen-review
```

### 2.1 Family 级 gate

每个 semantic family 独立维护自己的 workflow gate 状态。这意味着同一 DLL 下不同 family 的 gate 状态可能不一样。

| Gate code | Display Name | Default Policy | 说明 |
|-----------|-------------|----------------|------|
| `audit-input-and-ledger` | Audit Input And Ledger | `required` | 该 family 的 input manifest、surface/semantic ledger 完整性 |
| `managed-proof` | Managed Proof | `required` | 该 family 的 managed proof 是否通过 |
| `native-proof` | Native Proof | `required` | 该 family 的 native proof 是否通过 |
| `hotupdate-proof` | HotUpdate Proof | `conditional` | 该 family 的 hotupdate proof（按 DLL policy） |
| `benchmark` | Benchmark | `conditional` | 该 family 的 benchmark（按 family 是否存在 perf obligation） |
| `codegen-review` | Codegen Review | `required` | 该 family 的 codegen review 是否通过 |

### 2.2 DLL 级 gate

只有一个 gate：`completion-certification`

| Gate code | Display Name | Default Policy | 说明 |
|-----------|-------------|----------------|------|
| `completion-certification` | Completion Certification | `required` | 汇总该 DLL 所有 family 的 closure 状态，给出 completion 结论 |

DLL 及 gate 的特征：
- 不进入 family 级 workflow denominator
- 仅在所有 family 的 required gates 全部通过后触发
- 其结论影响 DLL 的 `completionStatus`（`completed` / `blocked` / `in-progress`）

## 3. Gate 状态模型

沿用 `design-v1-01.md` §10.1 的状态模型。每个 gate 固定拆成两层状态：

### 3.1 policyState

| 值 | 含义 |
|----|------|
| `required` | 该 gate 是该 family 的强制 gate |
| `conditional` | 条件触发，由 policy 决定是否升级为 required |
| `not-required` | 该 family 不需要此 gate |

policyState 的变更只能通过正式 policy 调整，不能由执行状态自动变更。

### 3.2 executionState

| 值 | 含义 | 是否计入 denominator |
|----|------|----------------------|
| `pending` | 尚未开始 | 是（未完成） |
| `in-progress` | 正在执行 | 是（未完成） |
| `passed` | 已通过 | 是（已完成） |
| `failed` | 已失败，但 **不是** blocker | 是（未完成） |
| `blocked` | 被外部因素阻塞 | 是（未完成） |
| `missing-evidence` | 声称 passed 但缺少 evidence | 是（未完成） |
| `not-required` | 该 gate 对本 family 不要求 | 否（跳过） |

### 3.3 状态转换规则

```
pending ──→ in-progress ──→ passed ──→ (完成)
                    │            │
                    ├──→ failed ──→ 修复后重试 → in-progress
                    │
                    └──→ blocked ──→ blocker 解除 → in-progress

passed 在任何证据被重新验证不通过时 → missing-evidence → 重新执行
```

## 4. Workflow Progress 计算公式

### 4.1 Family 级 Workflow Progress

```
familyWorkflowProgress = completedGates / requiredGates

其中:
  completedGates = family 中 executionState = "passed" 的 gate 数量
  requiredGates  = family 中 policyState != "not-required" 的 gate 数量
                  （即: required + conditional-已升级为-required）
```

当 `familyWorkflowProgress = 1.0` 时，该 family 的 workflow 流程已完成。
但这**不代表**该 family 的 capability 已闭包——closure 由 Capability Closure 判定。

### 4.2 DLL 级 Workflow Progress

```
dllWorkflowProgress = weightedAverage(familyWorkflowProgress)

计算方式:
  dllWorkflowProgress = sum(all_family_requiredGates_passed) / sum(all_family_requiredGates)

  分子: 所有 family 中 executionState = "passed" 的 gate 总数
  分母: 所有 family 中 policyState != "not-required" 的 gate 总数
```

### 4.3 Workflow Progress 与 Capability Closure 的并行展示

报告必须同时显示两条指标，不允许合并为一条：

```
DLL: System.Private.CoreLib
  Capability Closure:  14/20 (70%)    ← 分母=capability family 总数
  Workflow Progress:   45/60 (75%)    ← 分母=required gate 总数
```

**两条指标的关系**：
- Capability Closure 是主轴，回答"能力覆盖是否完整"
- Workflow Progress 是辅轴，回答"形式化验证走到哪一步"
- 一个 family 的 closure = `closed` 且对应 workflow gates 全部通过
- 一个 family 的 closure = `closed` 但 workflow gates 未全通过 → **不允许**（完整性校验失败）

## 5. Policy State 的升级触发

### 5.1 conditional → required 的触发规则

| Gate | 触发条件 | 裁决者 |
|------|----------|--------|
| `hotupdate-proof` | DLL policy 中 `hotupdate-proof` = `required`（如 System.Private.CoreLib） | program.json projectPolicies |
| `benchmark` | 该 family 被显式声明存在 perf obligation | 审核者 |

### 5.2 conditional → not-required 的触发规则

| Gate | 触发条件 | 裁决者 |
|------|----------|--------|
| `hotupdate-proof` | family 不穿过 hotupdate 边界，或技术上无法实现 hotupdate proof 并有正式 blocker 记录 | 审核者 + waiver authority |
| `benchmark` | family 没有性能敏感路径 | 审核者 |

## 6. Family 级 gate 与 Capability Family Ledger 的对应关系

每个 semantic family 在 ledger 中的 `verificationGates` 字段映射到本 contract 定义的 gate 模型：

```json
{
  "familyId": "family/System.Private.CoreLib/convert/to-char",
  "verificationGates": {
    "audit-input-and-ledger": "passed",
    "managed-proof": "passed",
    "native-proof": "passed",
    "hotupdate-proof": "passed",
    "benchmark": "conditional",
    "codegen-review": "passed"
  }
}
```

每个 gate 的值存储的是 `executionState`。`policyState` 在 program.json 和 DLL policy 中定义，不在 ledger 的每个 family 条目中重复。

## 7. Gate 状态变更流程

```
1. 执行 gate 对应的动作（运行 proof、生成 review 等）
2. 更新 gate 的 executionState
3. 如果 executionState = "passed"，更新 evidence 路径
4. 重新计算 familyWorkflowProgress
5. 如果变化影响 DLL 级进度，更新 dllWorkflowProgress
6. 如果该 family 的 closure 受 gate 状态影响，同时更新 closureStatus
```

## 8. 与现有架构的兼容性

### 8.1 与 program.json 的 projectTemplates

本 contract 的 gate 列表与 `program.json` 的 `projectTemplates[]` 保持一致。每个 gate code 对应一个 project template code。

区别：
- program.json 的 project template 是 DLL 级的模板定义
- 本 contract 的 gate 是 family 级的实例化执行状态

### 8.2 与 design-v1-01.md 的 Verification Project

本 contract 不改变 `design-v1-01.md` §9.2 的 Verification Project 模板。仅将 project 的执行状态映射到每个 family。

## 9. 变更日志

| 日期 | 版本 | 变更 | 原因 |
|------|------|------|------|
| 2026-04-25 | 1.0.0 | 初始版本 | Phase 1 初始创建 |
