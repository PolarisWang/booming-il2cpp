# Waiver Contract v1.01

Date: 2026-04-25 23:30:00 +08:00
Status: draft
Phase: phase-1 (20260425-05)
Parent: foundation-dll-capability-closure-reporting-roadmap

## 1. 目标

定义 capability family 的 **waiver / exclusion / platform-blocked** 三种豁免机制的正式 authority、最小字段和裁决流程。

核心结论：
- waive = 临时豁免（有时间限制），exclusion = 永久排除，platform-blocked = 平台能力受限
- 所有非 closed 的 family 必须有正式豁免记录
- 豁免记录必须引用 authority artifact，不允许仅引用 task STATUS.md

## 2. 三种豁免的定义

### 2.1 Waiver（临时豁免）

定义：某 family 当前未闭包，但有明确的计划在将来闭包。

特征：
- **有时间限制**：必须设置 `expiresAt` 日期
- **有闭包计划**：必须指向一个 plan 或 task
- **可撤销**：到期未完成必须重新评估，不能自动续期
- **使用场景**：已知需要实现但尚未完成的 family

示例：`Convert.ToString` 中的 `Object` 重载当前使用 `char.ToString(value)` 替代，但未来需要独立的实现。

### 2.2 Exclusion（永久排除）

定义：某 family 的 scope 中部分方法被正式排除在 audit 范围之外，这些方法在可见将来不需要闭包。

特征：
- **有范围限制**：必须精确指定 excluded 的方法或 patterns
- **无到期时间**：永久有效，除非 authority 重新确认
- **有排除理由**：必须是技术/业务上的正式理由
- **使用场景**：平台无关方法、已废弃 API、非 public surface

示例：`System.Convert::ToChar(System.SByte)` 如果被标记为 `not CLS-compliant` 且审核者决定不在当前审核计划中覆盖，可以 exclusion 但必须注明理由。

### 2.3 Platform-blocked（平台受限）

定义：某 family 在当前目标平台上无法实现，且没有可行的 workaround。

特征：
- **有平台范围**：必须指定受限制的目标平台
- **有技术理由**：必须说明为什么无法实现
- **有替代方案**：如果有，必须说明
- **使用场景**：依赖于目标平台不支持的 OS API、指令集或功能

示例：`System.Buffer::_Memcmp` 在某个指令集缺失的平台上无法使用 native intrinsic，且没有纯托管替代实现。

## 3. 最小字段定义

### 3.1 Waiver 记录最小字段

```json
{
  "waiverId": "WAIVER-20260425-001",
  "type": "waiver",
  "familyId": "family/System.Private.CoreLib/convert/to-string",
  "scope": {
    "affectedMethods": ["System.Convert::ToString(System.Object)"],
    "affectedGates": ["managed-proof", "native-proof"],
    "coverageImpact": "partial"  // partial | full (full=整个 family 被豁免)
  },
  "reason": "Object overload 的 IFormatProvider 分支需要新的 emitter handler，当前使用 char.ToString(value) 作为过渡",
  "authorityRef": "original-authority://System.Private.CoreLib/nativeization-plan#pending-handler",
  "createdAt": "2026-04-25T23:30:00+08:00",
  "createdBy": "codegen-review",
  "expiresAt": "2026-07-25T23:30:00+08:00",
  "closurePlan": {
    "planRef": "docs/dev/in-progress/20260419-37-.../plan-v1-01.md",
    "expectedCompletionBy": "2026-07-01"
  },
  "renewalCount": 0,
  "status": "active"  // active | expired | superseded
}
```

### 3.2 Exclusion 记录最小字段

```json
{
  "waiverId": "EXCLUSION-20260425-001",
  "type": "exclusion",
  "familyId": "family/System.Private.CoreLib/convert/to-char",
  "scope": {
    "excludedMethods": ["System.Convert::ToChar(System.UIntPtr)"],
    "exclusionPatterns": [],
    "affectedGates": ["managed-proof", "native-proof"],
    "coverageImpact": "partial"
  },
  "reason": "System.UIntPtr 是平台特定类型，不在当前 target framework 的 public surface 中",
  "authorityRef": "original-authority://System.Private.CoreLib/surface-ledger#uintptr-not-in-public-surface",
  "createdAt": "2026-04-25T23:30:00+08:00",
  "createdBy": "audit-review",
  "confirmedBy": "audit-lead",
  "exclusionBasis": "not-in-public-surface | deprecated-api | non-cls-compliant | platform-specific | out-of-scope"
}
```

### 3.3 Platform-blocked 记录最小字段

```json
{
  "waiverId": "PLATFORM-BLOCKED-20260425-001",
  "type": "platform-blocked",
  "familyId": "family/System.Private.CoreLib/buffer/memory-operations",
  "scope": {
    "blockedMethods": ["System.Buffer::_Memcmp"],
    "blockedOnPlatforms": ["windows-arm64"],
    "affectedGates": ["native-proof"],
    "coverageImpact": "partial"
  },
  "reason": "windows-arm64 平台缺少 _Memcmp 的 native intrinsic 实现",
  "authorityRef": "original-authority://System.Private.CoreLib/nativeization-plan#memcmp-no-arm64-intrinsic",
  "createdAt": "2026-04-25T23:30:00+08:00",
  "createdBy": "codegen-review",
  "confirmedBy": "codegen-lead",
  "workaround": {
    "hasWorkaround": true,
    "workaroundDescription": "回退到 managed _Memcmp 实现",
    "workaroundRef": "original-authority://System.Private.CoreLib/nativeization-plan#memcmp-fallback"
  },
  "resolutionPlan": {
    "planRef": null,
    "resolutionTarget": "等待 IL2CPP 工具链更新支持 ARM64 的 memcmp intrinsic",
    "expectedResolutionBy": null
  }
}
```

### 3.4 通用约束

| 字段 | 约束 |
|------|------|
| `type` | 固定为 `waiver` / `exclusion` / `platform-blocked` |
| `waiverId` | 全局唯一 |
| `familyId` | 必须匹配 ledger 中已存在的 family ID |
| `authorityRef` | 必须可解析为正式 authority artifact 中的条目 |
| `reason` | 非空，且不能是"TODO"或"TBD" |
| `createdBy` | 必须记录创建者角色（`codegen-review` / `audit-review` / `audit-lead`） |

## 4. Authority 要求

### 4.1 合法 authorityRef 的格式

```
authorityRef = "{authority-type}://{artifact-path}#{ref-id}"

authority-type: "original-authority" | "report-authority" | "design-doc" | "external"
```

合法示例：
- `original-authority://System.Private.CoreLib/surface-ledger#uintptr-not-in-public-surface`
- `original-authority://System.Private.CoreLib/nativeization-plan#memcmp-no-arm64-intrinsic`
- `report-authority://foundation-dll-capability-family-v1/family-123#waiver-basis`
- `design-doc://docs/dev/.../design-v1-01.md#waiver-rule`
- `external://ado-task/12345`

### 4.2 不合法 authorityRef

以下来源**不允许**作为 waiver 的 authority：
- `docs/dev/in-progress/*/STATUS.md`（task STATUS.md 不是正式 authority）
- 没有 artifact 路径的口头结论（如 "team decided"）
- 没有版本号或时间戳的 wiki 页面

### 4.3 创建者角色权限

| 角色 | 可创建 | 需确认 | 说明 |
|------|--------|--------|------|
| `codegen-review` | waiver, platform-blocked | exclusion 需 `audit-lead` | codegen 审核者可创建临时豁免和平台受限记录 |
| `audit-review` | waiver, exclusion, platform-blocked | exclusion 需 `audit-lead` | 审核者可以创建所有类型，但 exclusion 需要 lead 确认 |
| `audit-lead` | waiver, exclusion, platform-blocked | 不需要 | lead 可确认所有类型 |
| `codegen-lead` | platform-blocked | platform-blocked 需 `audit-lead` | codegen lead 可确认平台限制 |

## 5. 裁决流程

### 5.1 创建流程

```
1. 发现需要豁免的 family →
2. 填写最小字段（type / familyId / scope / reason / authorityRef） →
3. 根据类型和角色确认权限 →
4. 在 ledger 的 family.waiverRecords[] 中追加记录 →
5. 如果该 family 之前为 in-progress，更新 closureStatus 为对应类型 →
6. 如果该 waiver 影响 DLL completed 判定，重新运行 completion-certification
```

### 5.2 更新流程

```
1. waiver 到期复查 →
2. 评估是否已闭包：
   a. 已闭包 → 移除 waiver 记录，更新 closureStatus = "closed"
   b. 仍未闭包但有进展 → 更新 expiresAt，renewalCount++
   c. 仍未闭包且无进展 → 升级为 blocker
3. 更新变更日志
```

### 5.3 争议裁决

当 waiver/exclusion/platform-blocked 的合法性存在争议时：

```
1. 记录争议双方的立场和理由
2. 提交给 audit-lead 裁决
3. audit-lead 的裁决为最终决定
4. 无论裁决结果如何，记录必须保留争议记录
```

## 6. Waiver 的生命周期

```
创建 (active)
  │
  ├── expiresAt 到达 ──→ 复查
  │                        │
  │                   ├── 已闭包 ──→ 移除 waiver，closureStatus = "closed"
  │                   │
  │                   ├── 有进展 ──→ 续期 (expiresAt 延长，renewalCount++)
  │                   │
  │                   └── 无进展 ──→ 升级 blocker
  │
  ├── authority 变更 ──→ 重新评估 waiver 合法性
  │                        │
  │                   ├── 仍合法 ──→ 保持 active
  │                   │
  │                   └── 不合法 ──→ 标记 superseded
  │                                    family 回到 in-progress
  │
  └── 主动撤销 ──→ superseded
                       family 回到 in-progress
```

### 6.1 Waiver 续期限制

| 条件 | 限制 |
|------|------|
| 最大续期次数 | 3 次（超过后必须升级为 blocker 或正式项目） |
| 单次有效期 | 不超过 90 天 |
| 续期理由 | 必须与原始理由不同（不能重复"还在进行中"） |

### 6.2 Exclusion 的不可逆性

Exclusion 一旦创建，是永久性的。只有以下情况可以撤销：
- 原始排除的依据已不再成立（如方法重新进入 public surface）
- audit-lead 确认撤销

## 7. Waiver 对报告的影响

### 7.1 对 Capability Closure 分母的影响

```
分母 = totalFamilies (不变)
分子 = closedFamilies (waived/excluded/platform-blocked 不计入 closure)

Capability Closure = closedFamilies / totalFamilies

DLL 完成需要: 所有 family 的 closureStatus ∈ {closed, waived, excluded, platform-blocked}
但不是所有 waive 都接受——必须有正式 authority。
```

### 7.2 对 Workflow Progress 的影响

```
分母 = requiredGates (不变)
分子 = passedGates (waive 不影响 gate 计数)

即使 family 被 waive 了，其 required gates 仍然必须通过。
唯一的例外是 exclusion 同时将对应的 gate policy 设置为 "not-required"。
```

### 7.3 报告中的展示规则

```
- waived family: 在报告中标示 ⚠️ 图标 + 过期时间
- excluded family: 在报告中标示 ⊘ 图标 + 排除理由
- platform-blocked family: 在报告中标示 🛡️ 图标 + 受限平台
- 所有豁免记录必须在 DLL Detail 页可见
- 点击豁免记录可跳转到 authorityRef
```

## 8. 变更日志

| 日期 | 版本 | 变更 | 原因 |
|------|------|------|------|
| 2026-04-25 | 1.0.0 | 初始版本 | Phase 1 初始创建 |
