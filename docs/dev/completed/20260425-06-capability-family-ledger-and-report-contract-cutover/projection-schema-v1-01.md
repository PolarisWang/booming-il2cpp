# Projection Schema v1.01

Date: 2026-04-26 00:00:00 +08:00
Status: draft
Phase: phase-2 (20260425-06)
Parent: foundation-dll-capability-closure-reporting-roadmap

## 1. 目标

定义 foundation DLL audit 的 **projection payload** 正式 schema，覆盖 program / DLL / family 三级数据。

核心原则：
- **报告 authority 由 projection payload 明示**，而不是 HTML 模板隐式推断
- payload 必须是 self-describing 的，consumer（HTML dashboard、archive）不依赖外部上下文来理解数据
- 所有字段必须有明确的类型、格式和约束

## 2. 文件结构

Projection 文件分三级，固定在 `verification/projections/foundation-dll-audit/` 下：

```
verification/projections/foundation-dll-audit/
  program.json                         ← Program 级总览（含双轴进度）
  dll-matrix.json                      ← DLL Matrix（14 行 × 7 gate 列 + 新双轴列）
  artifact-index.json                  ← Appendix artifact index（不变）
  dlls/
    System.Private.CoreLib.json        ← DLL 详情（含 capability families[]）
    System.Collections.Immutable.json
    ... (14 DLL files)
```

## 3. Program 级 Schema（program.json）

### 3.1 新增字段（与现有字段共存）

```json
{
  "schemaVersion": 2,
  "programId": "foundation-dll-translation-audit",
  "title": "Foundation DLL Translation Audit",

  "summary": {
    "dllCount": 14,
    "completedCount": 0,
    "inProgressCount": 1,
    "blockedCount": 1,
    "notStartedCount": 12,

    "capabilityClosure": {
      "totalFamilies": 120,
      "closedFamilies": 18,
      "closurePercent": 15.0,
      "waivedFamilies": 1,
      "excludedFamilies": 1,
      "platformBlockedFamilies": 0,
      "inProgressFamilies": 100
    },

    "workflowProgress": {
      "totalRequiredGates": 360,
      "passedGates": 45,
      "progressPercent": 12.5
    },

    "dllCompletion": {
      "completedDllCount": 0,
      "blockedDllCount": 1,
      "inProgressDllCount": 1,
      "notStartedDllCount": 12
    },

    "activeAssembly": "System.Private.CoreLib"
  },

  "authoritySnapshot": {
    "snapshotId": "snap-20260425-001",
    "snapshotAt": "2026-04-25T22:00:00+08:00",
    "ledgerVersion": "1.0.0",
    "originalAuthorityDigest": {
      "inputManifestVersion": "sha256:<hash>",
      "surfaceLedgerVersion": "sha256:<hash>",
      "semanticLedgerVersion": "sha256:<hash>",
      "nativeizationPlanVersion": "sha256:<hash>"
    }
  },

  "scopeAssemblies": [
    "System.Private.CoreLib",
    "System.Collections.Immutable",
    "..."
  ]
}
```

### 3.2 字段变更对照

| 字段 | 变更类型 | 说明 |
|------|----------|------|
| `summary.capabilityClosure` | **新增** | 双轴主轴：capability closure 进度 |
| `summary.workflowProgress` | **新增** | 双轴辅轴：workflow progress 进度 |
| `summary.dllCompletion` | **新增** | DLL completion 汇总（与旧 `completedCount` 共存确保向下兼容） |
| `summary.progressPercent` | **保留** | 向下兼容，但标记为 deprecated |
| `authoritySnapshot` | **新增** | 报告 authority 的快照信息，使 payload self-describing |

## 4. DLL Matrix Schema（dll-matrix.json）

### 4.1 新增字段

```json
{
  "schemaVersion": 2,
  "rows": [
    {
      "assemblyName": "System.Private.CoreLib",
      "orderIndex": 1,
      "dllState": "in-progress",
      "currentProject": "completion-certification",
      "riskTags": ["core-foundation"],

      "capabilityClosure": {
        "totalFamilies": 20,
        "closedFamilies": 18,
        "closurePercent": 90.0,
        "waivedFamilies": 1,
        "inProgressFamilies": 1
      },

      "workflowProgress": {
        "totalRequiredGates": 60,
        "passedGates": 45,
        "progressPercent": 75.0
      },

      "audit-input-and-ledger": "passed",
      "managed-proof": "passed",
      "native-proof": "passed",
      "hotupdate-proof": "passed",
      "benchmark": "conditional",
      "codegen-review": "passed",
      "completion-certification": "in-progress"
    },
    { "...": "..." }
  ]
}
```

### 4.2 字段变更对照

| 字段 | 变更类型 | 说明 |
|------|----------|------|
| `rows[].capabilityClosure` | **新增** | 每行的 capability closure 双轴数据 |
| `rows[].workflowProgress` | **新增** | 每行的 workflow progress 双轴数据 |
| `rows[].audit-input-and-ledger` etc. | **保留** | 保持现有 gate 列 |

## 5. DLL Detail Schema（dlls/<assembly>.json）

### 5.1 完整 payload 结构

```json
{
  "schemaVersion": 2,
  "assemblyName": "System.Private.CoreLib",
  "orderIndex": 1,
  "phase": "phase-1",
  "roadmapTaskId": "20260419-03-system-private-corelib-full-verification",
  "dllState": "in-progress",
  "currentProject": "completion-certification",
  "riskTags": ["core-foundation"],

  "capabilityClosure": {
    "totalFamilies": 20,
    "closedFamilies": 18,
    "waivedFamilies": 1,
    "excludedFamilies": 1,
    "platformBlockedFamilies": 0,
    "inProgressFamilies": 0,
    "closurePercent": 90.0
  },

  "workflowProgress": {
    "totalRequiredGates": 60,
    "passedGates": 45,
    "progressPercent": 75.0
  },

  "capabilityFamilies": [
    {
      "familyId": "family/System.Private.CoreLib/convert/to-char",
      "displayName": "Convert.ToChar",
      "description": "System.Convert.ToChar 方法族的所有重载",
      "closureStatus": "closed",
      "closureEvidence": {
        "coverageJson": "artifacts/.../runtime-skeleton.coverage.json",
        "nativeProofReport": "artifacts/.../native-hotupdate-audit.json",
        "codegenReview": "artifacts/.../codegen-review.json"
      },
      "methodCount": 18,
      "verificationGates": {
        "audit-input-and-ledger": "passed",
        "managed-proof": "passed",
        "native-proof": "passed",
        "hotupdate-proof": "passed",
        "benchmark": "conditional",
        "codegen-review": "passed"
      },
      "waiverRecords": [],
      "implementationFamilies": [
        {
          "familyId": "impl/System.Private.CoreLib/convert/to-char/checked-primitive-convert",
          "displayName": "Checked Primitive Convert",
          "methodSubjectIds": ["System.Convert::ToChar(System.SByte)", "..."]
        }
      ],
      "sourceGroups": [
        {
          "groupId": "src/System.Convert",
          "displayName": "System.Convert",
          "methodCount": 15
        }
      ]
    },
    {
      "familyId": "family/System.Private.CoreLib/convert/to-string",
      "displayName": "Convert.ToString",
      "closureStatus": "closed",
      "...": "..."
    }
  ],

  "projects": [
    { "... existing project entries ..." }
  ],

  "sourceLinks": {
    "subjectSource": "subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/",
    "verificationSource": "verification/catalog/programs/foundation-dll-translation-audit.program.json",
    "generatedCode": "artifacts/subjects/SolutionCorePack/runs/.../analysis/generated/runtime/",
    "evidence": "artifacts/subjects/SolutionCorePack/runs/.../",
    "authorityDocs": "docs/verification/foundation-dll-audit/"
  },

  "blockers": [
    { "... existing blocker entries ..." }
  ],

  "waiverSummary": {
    "totalWaivers": 1,
    "activeWaivers": 1,
    "expiredWaivers": 0,
    "totalExclusions": 1,
    "totalPlatformBlocked": 0
  }
}
```

## 6. 字段类型定义

### 6.1 CapabilityClosure

```
{
  totalFamilies:      integer(min: 0),         // 该作用域下所有 semantic family 数
  closedFamilies:     integer(min: 0),         // closureStatus = "closed"
  waivedFamilies:     integer(min: 0),         // closureStatus = "waived"
  excludedFamilies:   integer(min: 0),         // closureStatus = "excluded"
  platformBlockedFamilies: integer(min: 0),    // closureStatus = "platform-blocked"
  inProgressFamilies: integer(min: 0),         // closureStatus = "in-progress"
  closurePercent:     float(0.0, 100.0)        // closedFamilies / totalFamilies * 100
}
```

约束：`totalFamilies = closedFamilies + waivedFamilies + excludedFamilies + platformBlockedFamilies + inProgressFamilies`

### 6.2 WorkflowProgress

```
{
  totalRequiredGates: integer(min: 0),         // policyState != "not-required" 的 gate 总数
  passedGates:        integer(min: 0),         // executionState = "passed" 的 gate 数
  progressPercent:    float(0.0, 100.0)        // passedGates / totalRequiredGates * 100
}
```

### 6.3 VerificationGate

每个 gate 的值是 `executionState` 的字符串值：
```
"pending" | "in-progress" | "passed" | "failed" | "blocked" | "missing-evidence" | "not-required"
```

### 6.4 WaiverRecord

（同 waiver-contract-v1-01.md §3 的最小字段）

### 6.5 ImplementationFamily

```
{
  familyId:        string,                    // "impl/{dll}/{category}/{handler}"
  displayName:     string,
  handlerName:     string,                    // emitter handler 名
  templateName:    string?,                   // Scriban 模板路径（可选）
  methodSubjectIds: string[]                  // 属于该实现族的方法列表
}
```

### 6.6 SourceGroup

```
{
  groupId:     string,                        // "src/{TypeName}"
  displayName: string,
  typeNames:   string[],
  methodCount: integer(min: 0)
}
```

## 7. 与现有 schema 的兼容性

### 7.1 向后兼容原则

| 文件 | 兼容策略 |
|------|----------|
| `program.json` | 保留旧字段 (`summary.progressPercent`)，新增 `summary.capabilityClosure` / `summary.workflowProgress` / `summary.dllCompletion` / `authoritySnapshot` |
| `dll-matrix.json` | 保留 gate 列，新增 `capabilityClosure` / `workflowProgress` 行级对象 |
| `dlls/<assembly>.json` | 保留 `projects[]`，新增 `capabilityFamilies[]` / `sourceLinks` / `waiverSummary` |

### 7.2 schemaVersion 升级

`schemaVersion` 从 `1` 升级到 `2`。consumer 必须检查 `schemaVersion`：
- `1` → 旧格式，没有 capability closure 数据
- `2` → 新格式，包含全部双轴数据

### 7.3 空字段规则

如果某个 DLL 还没有 capability family 数据：
- `capabilityFamilies` = `[]`（空数组，不是 null）
- `capabilityClosure.totalFamilies` = `0`，`closurePercent` = `0.0`
- `workflowProgress.totalRequiredGates` = `0`，`progressPercent` = `0.0`

不允许出现 `null` 或字段缺失。Dashboard 看到 `totalFamilies: 0` 时显示 "no data" 而非错误。

## 8. 变更日志

| 日期 | 版本 | 变更 | 原因 |
|------|------|------|------|
| 2026-04-26 | 1.0.0 | 初始版本 | Phase 2 初始创建 |
