# Foundation DLL Audit 数据字典

> 本文档由 `docs/dev/scripts/generate-data-dictionary-draft.py` 自动生成，需人工审查确认。
> 生成时间: (请填写审查日期)

## 1. 顶层输出工件清单

| 工件 | 路径 | 用途 |
|------|------|------|
| program.json | artifact/verification-catalog/projections/foundation-dll-audit/program.json | 程序摘要 |
| dll-matrix.json | (同上目录) | DLL 矩阵行 |
| family-verification.json | (同上目录) | 每个 family 的 gate 评估结果 |
| family-verification-claims.json | (同上目录) | 验证 claims |
| truth-contracts.json | (同上目录) | 真值合约 |
| dashboard.html | (同上目录) | 主仪表板 HTML |
| dlls/{AssemblyName}.html | (同上目录)/dlls/ | 每个 DLL 详情页 |
| dlls/{AssemblyName}.json | (同上目录)/dlls/ | 每个 DLL 原始数据 |

## 2. program.json 摘要字段

| 字段 | 类型 | 当前值 | 推测来源 |
|------|------|--------|---------|
| `authoritySnapshot.denominatorStatus` | string | audit-confirmed |  |
| `authoritySnapshot.denominatorStrategy` | string | capability-family |  |
| `authoritySnapshot.ledgerVersion` | string | 1.0 |  |
| `authoritySnapshot.originalAuthorityDigest.inputManifestVersion` | string | v1 |  |
| `authoritySnapshot.originalAuthorityDigest.nativeizationPlanVersion` | string | v1 |  |
| `authoritySnapshot.originalAuthorityDigest.semanticLedgerVersion` | string | v1 |  |
| `authoritySnapshot.originalAuthorityDigest.surfaceLedgerVersion` | string | v1 |  |
| `authoritySnapshot.snapshotAt` | string | 2026-04-26T00:00:00+08:00 |  |
| `authoritySnapshot.snapshotId` | string | snap-20260426-audit-confirmed-v1 |  |
| `designPath` | string |  |  |
| `programId` | string | foundation-dll-translation-audit |  |
| `roadmapPath` | string | docs/dev/completed/20260427-04-dll-capability-verification-closure-roadmap/roadm |  |
| `schemaVersion` | string | 2 |  |
| `statusPath` | string | docs/dev/completed/20260427-04-dll-capability-verification-closure-roadmap/STATU |  |
| `subjectEntry` | string | subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSo |  |
| `summary.activeAssembly` | string | System.Private.CoreLib |  |
| `summary.blockedCount` | string | 0 |  |
| `summary.capabilityClosure.closedFamilies` | string | 0 |  |
| `summary.capabilityClosure.closurePercent` | string | 0.0 |  |
| `summary.capabilityClosure.excludedFamilies` | string | 0 |  |
| `summary.capabilityClosure.inProgressFamilies` | string | 116 |  |
| `summary.capabilityClosure.platformBlockedFamilies` | string | 0 |  |
| `summary.capabilityClosure.totalFamilies` | string | 116 |  |
| `summary.capabilityClosure.waivedFamilies` | string | 0 |  |
| `summary.completedCount` | string | 0 |  |
| `summary.dllCompletion.blockedDllCount` | string | 0 |  |
| `summary.dllCompletion.completedDllCount` | string | 0 |  |
| `summary.dllCompletion.inProgressDllCount` | string | 1 |  |
| `summary.dllCompletion.notStartedDllCount` | string | 13 |  |
| `summary.dllCount` | string | 14 |  |
| `summary.inProgressCount` | string | 1 |  |
| `summary.notStartedCount` | string | 13 |  |
| `summary.progressPercent` | string | 0.0 |  |
| `summary.workflowProgress.passedGates` | string | 144 |  |
| `summary.workflowProgress.progressPercent` | string | 20.69 |  |
| `summary.workflowProgress.totalRequiredGates` | string | 696 |  |
| `title` | string | Foundation DLL Translation Audit |  |

## 3. DLL Matrix（dll-matrix.json rows）

### rows[] 每行字段

| 字段 | 含义 | 计算流程 | 代码来源 | 枚举值 |
|------|------|---------|---------|--------|
| `assemblyName` | (待补充) | (待补充) |  | (待补充) |
| `audit-input-and-ledger` | (待补充) | (待补充) |  | (待补充) |
| `benchmark` | (待补充) | (待补充) |  | (待补充) |
| `capabilityClosure` | (待补充) | (待补充) |  | (待补充) |
| `codegen-review` | (待补充) | (待补充) |  | (待补充) |
| `completion-certification` | (待补充) | (待补充) |  | (待补充) |
| `currentProject` | (待补充) | (待补充) |  | (待补充) |
| `denominatorStatus` | (待补充) | (待补充) |  | (待补充) |
| `dllState` | (待补充) | (待补充) |  | (待补充) |
| `hotupdate-proof` | (待补充) | (待补充) |  | (待补充) |
| `managed-proof` | (待补充) | (待补充) |  | (待补充) |
| `native-proof` | (待补充) | (待补充) |  | (待补充) |
| `orderIndex` | (待补充) | (待补充) |  | (待补充) |
| `riskTags` | (待补充) | (待补充) |  | (待补充) |
| `workflowProgress` | (待补充) | (待补充) |  | (待补充) |

## 4. family-verification.json 结构


## 5. Native Proof 子视图

| 字段 | 含义 | 计算流程 | 代码来源 |
|------|------|---------|---------|

## 5. Managed Proof 子视图

| 字段 | 含义 | 计算流程 | 代码来源 |
|------|------|---------|---------|

## 5. HotUpdate Proof 子视图

| 字段 | 含义 | 计算流程 | 代码来源 |
|------|------|---------|---------|

## 5. Benchmark Proof 子视图

| 字段 | 含义 | 计算流程 | 代码来源 |
|------|------|---------|---------|

## 6. 枚举值字典

### dllState

| 值 | 含义 | 来源 |
|----|------|------|
| `completed` | DLL 已完成 | _dll_state() |
| `in-progress` | DLL 进行中 | _dll_state() |
| `blocked` | DLL 受阻 | _dll_state() |
| `not-started` | DLL 未开始 | _dll_state() |

### closureStatus

| 值 | 含义 |
|----|------|
| `closed` | 所有非豁免门控通过 |
| `waived` | 通过豁免记录放弃 |
| `excluded` | 排除在范围外 |
| `platform-blocked` | 平台限制 |
| `in-progress` | 至少一个门控未通过 |

### testCodeStatus

| 值 | 含义 |
|----|------|
| `present` | 测试代码存在 |
| `needs-tests` | 无测试目录 |
| `coverage-widened` | 前次合作为 needs-tests 或 coverage-widened |
| `no-coverage` | methodCount <= 0 |

### gateState（verificationGates 有效值）

| 值 | 含义 |
|----|------|
| `passed` | 已通过 |
| `pending` | 待处理 |
| `blocked` | 受阻 |
| `not-required` | 不要求 |
| `in-progress` | 进行中 |
| `missing-evidence` | 缺少证据 |

### denominatorStatus

| 值 | 含义 |
|----|------|
| `candidate-derived` | 候选推导 |
| `audit-confirmed` | 审计确认 |
| `auto-derived` | 自动推导 |

### derivationRule

| 值 | 含义 |
|----|------|
| `truth-contract:method-capability-contracts` | 从 method-capability-contracts.json 真值合约推导 |
| `gate-presence:managed-proof` | 从 managed-proof 门控存在性推导 |
| `gate-presence:hotupdate-proof` | 从 hotupdate-proof 门控存在性推导 |
| `gate-presence:benchmark` | 从 benchmark 门控存在性推导 |
| `ledger:testCode` | 从 ledger 的 testCode 条目推导 |
| `` (空) | 尚未推导（native-proof 在非 CoreLib DLL 中暂未推导） |

## 8. family-verification-claims.json 字段

| 字段路径 | 类型 | 含义 | 枚举值 |
|----------|------|------|--------|
| `schemaVersion` | int | Schema 版本号 | 1 |
| `assemblies[].schemaVersion` | int | 子 schema 版本号 | 1 |
| `assemblies[].assemblyName` | string | DLL 程序集名称 | |
| `assemblies[].ownerSubjectId` | string | 拥有者主体 ID（如 SolutionCorePack） | |
| `assemblies[].methodUniverseArtifactPaths` | string[] | 方法宇宙的原始数据文件路径列表 | |
| `assemblies[].claims[].claimId` | string | 声明的唯一标识（格式：familyId::gateCode） | |
| `assemblies[].claims[].familyId` | string | 所属 family 的 ID | |
| `assemblies[].claims[].gateCode` | string | 门控代码 | native-proof / managed-proof / hotupdate-proof / benchmark / test-code |
| `assemblies[].claims[].required` | boolean | 该门控是否为必需 | true / false |
| `assemblies[].claims[].denominator` | int | 分母（需覆盖/通过的方法数） | |
| `assemblies[].claims[].methodSubjectIds` | string[] | 方法主体 ID 列表 | |
| `assemblies[].claims[].ownerSubjectId` | string | 声明拥有者 | |
| `assemblies[].claims[].scope` | string | 声明的作用域描述 | |
| `assemblies[].claims[].derivationRule` | string | 推导规则 | truth-contract:... / gate-presence:... / ledger:... |
| `assemblies[].claims[].authorityRefs` | string[] | 权威引用（文件路径列表） | |

## 9. truth-contracts.json 字段

| 字段路径 | 类型 | 含义 |
|----------|------|------|
| `schemaVersion` | int | Schema 版本号 |
| `assemblies[].schemaVersion` | int | 子 schema 版本号 |
| `assemblies[].dllCapabilityManifest.assemblyName` | string | DLL 程序集名称 |
| `assemblies[].dllCapabilityManifest.ownerSubjectId` | string | 拥有者主体 ID |
| `assemblies[].dllCapabilityManifest.capabilityFamilies` | string[] | 能力 family ID 列表 |
| `assemblies[].dllCapabilityManifest.capabilityFamilyCount` | int | 能力 family 总数 |
| `assemblies[].dllCapabilityManifest.methodUniverseCount` | int | 方法宇宙总数 |
| `assemblies[].capabilityFamilyVerificationContracts[].assemblyName` | string | DLL 名称 |
| `assemblies[].capabilityFamilyVerificationContracts[].ownerSubjectId` | string | 拥有者 |
| `assemblies[].capabilityFamilyVerificationContracts[].familyId` | string | Family ID |
| `assemblies[].capabilityFamilyVerificationContracts[].displayName` | string | Family 显示名 |
| `assemblies[].capabilityFamilyVerificationContracts[].methodSubjectIds` | string[] | 方法主体 ID |
| `assemblies[].capabilityFamilyVerificationContracts[].methodSubjectCount` | int | 方法主体数量 |
| `assemblies[].capabilityFamilyVerificationContracts[].functionalObligation.required` | boolean | 功能验证是否必需 |
| `assemblies[].capabilityFamilyVerificationContracts[].performanceObligation.required` | boolean | 性能验证是否必需 |
| `assemblies[].capabilityFamilyVerificationContracts[].hotupdateFunctionalObligation.required` | boolean | 热更新功能验证是否必需 |
| `assemblies[].capabilityFamilyVerificationContracts[].hotupdatePerformanceObligation.required` | boolean | 热更新性能验证是否必需 |
| `assemblies[].capabilityFamilyVerificationContracts[].reviewBundleRequired` | boolean | 是否需要审查包 |
| `assemblies[].methodCapabilityContracts[].ownerSubjectId` | string | 方法级合约拥有者 |
| `assemblies[].methodCapabilityContracts[].familyId` | string | 所属 family |
| `assemblies[].methodCapabilityContracts[].methodSubjectId` | string | 方法主体 ID |
| `assemblies[].methodCapabilityContracts[].functionalRequired` | boolean | 功能验证是否必需 |
| `assemblies[].methodCapabilityContracts[].benchmarkRequired` | boolean | Benchmark 是否必需 |
| `assemblies[].methodCapabilityContracts[].hotupdateFunctionalRequired` | boolean | 热更新功能验证是否必需 |
| `assemblies[].methodCapabilityContracts[].hotupdateBenchmarkRequired` | boolean | 热更新 Benchmark 是否必需 |
| `assemblies[].methodCapabilityContracts[].reviewRequired` | boolean | 审查是否必需 |

## 7. 计算流程总图

```
program.json                    capability-family-ledger.json
    |                                   |
    v                                   v
build_foundation_dll_audit_payload()
  |-- _build_projects()           -> project rows + evidence
  |-- family_verification_claims  -> claims (methodSubjectIds)
  |-- verification_kernel         -> gate evaluations
  |-- case_index_loader           -> test/bench/hotupdate cases
  |-- truth_contracts             -> truth contracts
  |-- _compute_capability_closure -> closure stats
  |-- _compute_gate_progress      -> workflow progress
  |
  v
write_projection_bundle()
  |-- program.json, dll-matrix.json, family-verification.json
  |-- dashboard.html, dlls/{Name}.html
```
