# DLL Completed Contract v1.01

Date: 2026-04-25 23:30:00 +08:00
Status: draft
Phase: phase-1 (20260425-05)
Parent: foundation-dll-capability-closure-reporting-roadmap

## 1. 目标

定义 foundation DLL translation audit 中一个 DLL 被判定为 **`completed`** 的正式规则。

核心结论：
- `completed` = 该 DLL 的所有 capability family 都达到闭包条件 **且** 对应 workflow gates 全部通过
- `completion-certification` 是 DLL 级 gate，**不进入** family 级 workflow denominator
- 未闭包 family 必须有正式 waiver / exclusion / platform-blocked 记录

## 2. DLL 状态模型

| 状态 | 含义 | 进入条件 |
|------|------|----------|
| `not-started` | 尚未开始对该 DLL 的审计 | 创建时默认状态 |
| `in-progress` | 至少一个 family 已开始 | 首个 family 的状态非 pending |
| `blocked` | 存在 blocker 阻碍继续推进 | 阻塞条件触发（见 §8） |
| `completed` | 所有闭包条件满足 | 本 contract 的判定规则（§3） |

### 2.1 状态转换图

```
not-started
    │
    ├── 首个 family 开始工作 ──→ in-progress
    │
in-progress
    │
    ├── 所有 family 已闭包 ──→ completed
    │
    ├── blocker 触发 ──→ blocked ──→ blocker 解除 ──→ in-progress
    │
    └── 持续工作 ──→ in-progress（未达条件）

completed
    │
    └── 原始 authority 变更 ──→ in-progress（重新审核）
```

DLL 状态 `completed` 不是永久性标签。当原始 authority 变更（如 DLL 版本更新、semantic ledger 重新分类）时，DLL 必须回到 `in-progress` 重新验证。

## 3. DLL completed 判定规则

一个 DLL 必须同时满足以下 **所有** 条件才能标记为 `completed`。

### 规则 A: Family 闭包全覆盖

```
∀ family ∈ dll.families: family.closureStatus ∈ {closed, waived, excluded, platform-blocked}
```

解释：DLL 的所有 semantic family 要么已闭包，要么有正式豁免记录。

### 规则 B: 无未闭包 family

```
∄ family ∈ dll.families: family.closureStatus = "in-progress"
```

解释：没有 family 处于 `in-progress` 状态。

### 规则 C: 所有 required gates 通过

```
∀ family ∈ dll.families:
  ∀ gate ∈ family.requiredGates:
    gate.executionState = "passed"
```

解释：每个 family 的所有 `policyState != "not-required"` 的 gate 都必须为 `passed`。

### 规则 D: completion-certification gate 通过

```
dll.completionGate.executionState = "passed"
```

解释：DLL 级的 completion-certification gate 必须为 `passed`。这是最后的汇总 gate。

### 规则 E: waiver/exclusion/blocked 有正式 authority

```
∀ family ∈ dll.families:
  family.closureStatus ∈ {waived, excluded, platform-blocked} ⇒
    ∃ waiverRecord ∈ family.waiverRecords:
      waiverRecord.authorityRef 可解析 ∧ waiverRecord.reason 非空
```

解释：任何非 closed 的 family 都必须有正式的 waiver 记录（详见 waiver-contract）。

### 规则 F: 所有 evidence 存在

```
∀ family ∈ dll.families:
  family.closureStatus = "closed" ⇒
    family.closureEvidence 中所有 artifacts 的 exists = true
```

解释：closed 状态的 family 必须有可点击的证据 artefact。

## 4. completion-certification gate

### 4.1 定位

`completion-certification` 是 DLL 级别的汇总 gate，**不进入** family 级 workflow denominator。

它在以下时机触发：
- 所有 family 的 closure 状态不再为 `in-progress`
- 所有 family 的 required gates 已全部通过

### 4.2 Gate 执行内容

completion-certification gate 的执行包括：

1. **完整性校验**：验证所有 family 的 closure 条件是否满足（§3 规则 A-F）
2. **authority 一致性检查**：验证 ledger 的 `authoritySnapshotOf` 与原始 authority 版本一致
3. **evidence 存在性检查**：验证所有 closure evidence 的 artifact 存在性
4. **waiver 合法性检查**：验证 waiver 记录的 authorityRef 是否可解析
5. **生成 completion-report**：输出 `audit/<assembly>/completion-report.json`

### 4.3 completion-report.json 最小字段

```json
{
  "schemaVersion": "1.0.0",
  "assemblyName": "System.Private.CoreLib",
  "generatedAt": "2026-04-25T23:30:00+08:00",
  "dllStatus": "completed",
  "checklist": {
    "allFamiliesClosedOrWaived": true,
    "noInProgressFamilies": true,
    "allRequiredGatesPassed": true,
    "completionGatePassed": true,
    "waiverAuthorityValid": true,
    "allEvidenceExists": true,
    "authorityDigestMatch": true
  },
  "failures": [],
  "familySummary": {
    "totalFamilies": 20,
    "closed": 18,
    "waived": 1,
    "excluded": 1,
    "platformBlocked": 0,
    "inProgress": 0
  },
  "authoritySnapshotOf": "2026-04-25T22:00:00+08:00",
  "finalStatus": "completed"
}
```

## 5. DLL 状态汇总到 Program 视图

### 5.1 Program 视图的统计规则

```
completedDllCount = count(dll where dll.completionStatus = "completed")
activeDllCount    = count(dll where dll.completionStatus = "in-progress")
blockedDllCount   = count(dll where dll.completionStatus = "blocked")
notStartedDllCount = count(dll where dll.completionStatus = "not-started")
```

### 5.2 Program 总进度

```
Program Capability Closure = totalClosedFamilies / totalFamilies
  totalClosedFamilies = sum(all DLLs 中 closureStatus = "closed" 的 family 数)
  totalFamilies       = sum(all DLLs 中所有 semantic family 数)

Program Workflow Progress = totalPassedGates / totalRequiredGates
  totalPassedGates    = sum(all DLLs 中 executionState = "passed" 的 gate 数)
  totalRequiredGates  = sum(all DLLs 中 policyState != "not-required" 的 gate 数)

Program DLL Completion  = completedDllCount / 14
```

## 6. Policed DLL 特殊规则

### 6.1 System.Private.CoreLib

CoreLib 的 `hotupdate-proof` 为 `required`：
- 所有 family 的 hotupdate-proof gate 必须先升级为 `required`
- 验证通过后才能进入 completion-certification

### 6.2 Conditional hotupdate DLL（其他 13 个 DLL）

- hotupdate-proof 的 policy 为 `conditional`
- 若某个 family 不穿过 hotupdate 边界，可降级为 `not-required`
- 降级必须有正式 waiver record

### 6.3 High-risk DLL（Interop / Formatters / Text.Json）

- 这三个 DLL 的 codegen-review gate 必须包含额外检查项：
  - `System.Runtime.InteropServices`: 必须检查 P/Invoke marshalling 覆盖
  - `System.Runtime.Serialization.Formatters`: 必须检查序列化格式兼容性
  - `System.Text.Json`: 必须检查 JSON 序列化/反序列化的完整路径覆盖
- 对应的 completion-certification 必须包含 risk 确认记录

## 7. DLL completed 后的维护

### 7.1 变更检测

completed 的 DLL 在以下情况触发重新验证：
- audit input manifest 变更（新增/删除方法）
- surface ledger 变更（surface 分类调整）
- semantic ledger 变更（family 归属重新分类）
- nativeization plan 变更（codegen 策略修改）
- 依赖的原始 authority 版本 hash 与 ledger 快照不匹配

### 7.2 自动降级

当检测到上述变更时，DLL `completionStatus` 自动从 `completed` 降级为 `in-progress`。

### 7.3 快速复审

如果变更范围很小（如只新增了几个方法到已有 family），允许快速复审路径：
1. 只重新验证受影响的 family
2. 重新运行 completion-certification
3. 其他 family 维持之前的状态和 evidence

## 8. Blocker 机制

### 8.1 Blocker 定义

Block 是指阻碍 family 或 DLL 继续推进的外部因素，且当前无法通过内部努力解决。

### 8.2 DLL 级 Blocker 触发条件

| 条件 | 说明 |
|------|------|
| 技术 blocker | 某个 family 的 native proof 因平台限制/工具链问题无法通过 |
| authority blocker | 原始 authority 缺失或矛盾，无法确定 family scope |
| dependency blocker | 该 DLL 依赖的前置 DLL 未完成 |

### 8.3 Blocker 记录格式

```json
{
  "blockerId": "BLOCKER-20260425-001",
  "title": "Native proof 在 Windows ARM64 上无法通过",
  "type": "technical | authority | dependency",
  "affectedFamilies": ["family/System.Private.CoreLib/...", "..."],
  "affectedGates": ["native-proof"],
  "description": "详细描述阻塞原因",
  "workaround": "当前是否有临时绕过方案",
  "resolutionPlan": "如何解除 blocker",
  "resolvedAt": null,
  "resolutionSummary": null
}
```

### 8.4 Blocker 解除

Blocker 解除后，DLL 自动回到 `in-progress` 继续推进。

## 9. 变更日志

| 日期 | 版本 | 变更 | 原因 |
|------|------|------|------|
| 2026-04-25 | 1.0.0 | 初始版本 | Phase 1 初始创建 |
