# Projection Payload Examples v1.01

Date: 2026-04-26 00:00:00 +08:00
Status: draft
Phase: phase-2 (20260425-06)
Parent: foundation-dll-capability-closure-reporting-roadmap

## 1. 概述

本文档提供 Phase 2 projection schema 的完整 JSON payload 示例，覆盖三级数据：
- Program 级总览（`program.json`）
- DLL Matrix（`dll-matrix.json`）
- DLL Detail（`dlls/System.Private.CoreLib.json`）

## 2. Program 级总览

```json
{
  "schemaVersion": 2,
  "programId": "foundation-dll-translation-audit",
  "title": "Foundation DLL Translation Audit",
  "roadmapPath": "docs/dev/in-progress/20260425-03-foundation-dll-capability-closure-reporting-roadmap/STATUS.md",
  "designPath": "docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md",
  "statusPath": "docs/dev/in-progress/20260425-03-foundation-dll-capability-closure-reporting-roadmap/STATUS.md",
  "subjectEntry": "subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/FoundationDllTranslationSolution.sln",
  "summary": {
    "dllCount": 14,
    "completedCount": 0,
    "inProgressCount": 1,
    "blockedCount": 1,
    "notStartedCount": 12,
    "progressPercent": 0.0,
    "activeAssembly": "System.Private.CoreLib",

    "capabilityClosure": {
      "totalFamilies": 120,
      "closedFamilies": 18,
      "waivedFamilies": 1,
      "excludedFamilies": 1,
      "platformBlockedFamilies": 0,
      "inProgressFamilies": 100,
      "closurePercent": 15.0
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
    }
  },
  "authoritySnapshot": {
    "snapshotId": "snap-20260426-001",
    "snapshotAt": "2026-04-26T00:00:00+08:00",
    "ledgerVersion": "1.0.0",
    "originalAuthorityDigest": {
      "inputManifestVersion": "sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
      "surfaceLedgerVersion": "sha256:a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a",
      "semanticLedgerVersion": "sha256:3f39d5c348e7b15a0c25f3b7f3b3d3f3f3d3f3d3f3d3f3d3f3d3f3d3f3d3f3f",
      "nativeizationPlanVersion": "sha256:5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8"
    }
  },
  "scopeAssemblies": [
    "System.Private.CoreLib",
    "System.Collections.Immutable",
    "System.Formats.Asn1",
    "System.IO.Compression.Brotli",
    "System.IO.Compression.ZipFile",
    "System.IO.Pipelines",
    "System.Linq",
    "System.Net.ServerSentEvents",
    "System.ObjectModel",
    "System.Runtime.InteropServices",
    "System.Runtime.Serialization.Formatters",
    "System.Security.Principal.Windows",
    "System.Text.Json",
    "System.Threading.Tasks.Parallel"
  ]
}
```

### 2.1 关键字段说明

**`summary.capabilityClosure`** 是双轴主轴：
- `totalFamilies = 120` = 所有 14 个 DLL 的总 semantic family 数
- `closedFamilies = 18` = 仅 System.Private.CoreLib 有已闭包的 family
- `closurePercent = 15.0%` = 程序级能力闭包进度

**`summary.workflowProgress`** 是双轴辅轴：
- `totalRequiredGates = 360` = 所有 family 的 required gate 总数
- `passedGates = 45` = 所有 family 中 executionState = "passed" 的 gate 数
- `progressPercent = 12.5%` = 程序级验证流程进度

**`summary.dllCompletion`** 是 DLL completion 视图：
- `completedDllCount = 0` = 还没有 DLL 达到 completion
- 注意：这与 `capabilityClosure.closurePercent = 15%` 不同，因为 closure 是 family 级的，completion 是 DLL 级的

**`authoritySnapshot`** 使 payload self-describing：
- consumer 可以检查 `snapshotAt` 判断数据时效性
- consumer 可以核对 `originalAuthorityDigest` 判断原始数据是否变更

## 3. DLL Matrix

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

      "audit-input-and-ledger": "passed",
      "managed-proof": "passed",
      "native-proof": "passed",
      "hotupdate-proof": "passed",
      "benchmark": "conditional",
      "codegen-review": "passed",
      "completion-certification": "in-progress"
    },
    {
      "assemblyName": "System.Collections.Immutable",
      "orderIndex": 2,
      "dllState": "not-started",
      "currentProject": "audit-input-and-ledger",
      "riskTags": [],

      "capabilityClosure": {
        "totalFamilies": 0,
        "closedFamilies": 0,
        "waivedFamilies": 0,
        "excludedFamilies": 0,
        "platformBlockedFamilies": 0,
        "inProgressFamilies": 0,
        "closurePercent": 0.0
      },

      "workflowProgress": {
        "totalRequiredGates": 0,
        "passedGates": 0,
        "progressPercent": 0.0
      },

      "audit-input-and-ledger": "pending",
      "managed-proof": "pending",
      "native-proof": "pending",
      "hotupdate-proof": "pending",
      "benchmark": "pending",
      "codegen-review": "pending",
      "completion-certification": "pending"
    },
    {
      "assemblyName": "System.Runtime.InteropServices",
      "orderIndex": 10,
      "dllState": "not-started",
      "currentProject": "audit-input-and-ledger",
      "riskTags": ["high-risk"],

      "capabilityClosure": {
        "totalFamilies": 0,
        "closedFamilies": 0,
        "waivedFamilies": 0,
        "excludedFamilies": 0,
        "platformBlockedFamilies": 0,
        "inProgressFamilies": 0,
        "closurePercent": 0.0
      },

      "workflowProgress": {
        "totalRequiredGates": 0,
        "passedGates": 0,
        "progressPercent": 0.0
      },

      "audit-input-and-ledger": "pending",
      "managed-proof": "pending",
      "native-proof": "pending",
      "hotupdate-proof": "pending",
      "benchmark": "pending",
      "codegen-review": "pending",
      "completion-certification": "pending"
    }
  ]
}
```

### 3.1 行级数据的解读

**System.Private.CoreLib 行**：
- `capabilityClosure.closurePercent = 90%`：20 个 family 中 18 个已闭包
- `workflowProgress.progressPercent = 75%`：60 个 required gates 中 45 个已通过
- 两个百分比不同，因为 closure 是"能力是否完整"，progress 是"验证流程走到哪一步"
- CoreLib 已有 1 个 waived + 1 个 excluded family，但 DLL 状态仍是 `in-progress` 尚未完成

**System.Collections.Immutable 行**：
- `totalFamilies = 0`：ledger 中还没有这个 DLL 的 family 数据
- 所有 gate 为 `pending`：表示尚未开始

## 4. DLL Detail

```json
{
  "schemaVersion": 2,
  "assemblyName": "System.Private.CoreLib",
  "orderIndex": 1,
  "phase": "phase-1",
  "roadmapTaskId": "20260419-03-system-private-corelib-full-verification",
  "dllState": "in-progress",
  "currentProject": "completion-certification",
  "blockingReason": "",
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
      "description": "System.Convert.ToChar 方法族的所有重载，包含 checked primitive convert、primitive return forwarder、boxed IConvertible invalid-cast、boxed value-type invalid-cast、string/object char provider 和 IConvertible.ToChar 接口实现",
      "closureStatus": "closed",
      "closureEvidence": {
        "coverageJson": "artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json",
        "nativeProofReport": "artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/native-hotupdate-audit.json",
        "codegenReview": "docs/verification/foundation-dll-audit/codegen-review/System.Private.CoreLib.json"
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
          "handlerName": "TryBuildRuntimeSkeletonConvertCheckedPrimitiveConvertStub",
          "templateName": "NativeReferenceProof.RuntimeSkeleton.StaticCheckedByteConvertStub.cpp.scriban",
          "methodSubjectIds": [
            "System.Convert::ToChar(System.SByte)",
            "System.Convert::ToChar(System.Int16)",
            "System.Convert::ToChar(System.UInt16)",
            "System.Convert::ToChar(System.UInt32)",
            "System.Convert::ToChar(System.UInt64)"
          ]
        },
        {
          "familyId": "impl/System.Private.CoreLib/convert/to-char/primitive-return-forwarder",
          "displayName": "Primitive Return Forwarder",
          "handlerName": "TryBuildRuntimeSkeletonConvertStaticByteReturnForwarderStub",
          "templateName": "NativeReferenceProof.RuntimeSkeleton.StaticByteForwarderStub.cpp.scriban",
          "methodSubjectIds": [
            "System.Convert::ToChar(System.Int32)",
            "System.Convert::ToChar(System.Int64)"
          ]
        }
      ],
      "sourceGroups": [
        {
          "groupId": "src/System.Convert",
          "displayName": "System.Convert",
          "typeNames": ["System.Convert"],
          "methodCount": 15
        },
        {
          "groupId": "src/System.IConvertible",
          "displayName": "System.IConvertible (ToChar)",
          "typeNames": ["System.IConvertible"],
          "methodCount": 3
        }
      ]
    },
    {
      "familyId": "family/System.Private.CoreLib/convert/to-string",
      "displayName": "Convert.ToString",
      "description": "System.Convert.ToString 方法族的所有重载",
      "closureStatus": "closed",
      "closureEvidence": {
        "coverageJson": "artifacts/.../runtime-skeleton.coverage.json",
        "nativeProofReport": "artifacts/.../native-hotupdate-audit.json",
        "codegenReview": "..."
      },
      "methodCount": 19,
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
          "familyId": "impl/System.Private.CoreLib/convert/to-string/static-string-call-forwarder",
          "displayName": "Static String Call Forwarder",
          "handlerName": "TryBuildRuntimeSkeletonConvertStaticStringCallForwarderHandler",
          "methodSubjectIds": [
            "System.Convert::ToString(System.Char)",
            "System.Convert::ToString(System.Int32)",
            "..."
          ]
        }
      ],
      "sourceGroups": [
        {
          "groupId": "src/System.Convert",
          "displayName": "System.Convert",
          "typeNames": ["System.Convert"],
          "methodCount": 19
        }
      ]
    },
    {
      "familyId": "family/System.Private.CoreLib/convert/to-int32",
      "displayName": "Convert.ToInt32",
      "closureStatus": "closed",
      "methodCount": 16,
      "...": "..."
    },
    {
      "familyId": "family/System.Private.CoreLib/convert/to-int16",
      "displayName": "Convert.ToInt16",
      "closureStatus": "in-progress",
      "methodCount": 16,
      "...": "..."
    },
    {
      "familyId": "family/System.Private.CoreLib/buffer/memory-operations",
      "displayName": "Buffer Memory Operations",
      "closureStatus": "in-progress",
      "methodCount": 10,
      "...": "..."
    },
    {
      "familyId": "family/System.Private.CoreLib/enum/has-flag",
      "displayName": "Enum.HasFlag",
      "closureStatus": "in-progress",
      "methodCount": 3,
      "...": "..."
    },
    {
      "familyId": "family/System.Private.CoreLib/enum/to-string",
      "displayName": "Enum.ToString",
      "closureStatus": "in-progress",
      "methodCount": 5,
      "...": "..."
    },
    {
      "familyId": "family/System.Private.CoreLib/enum/parse",
      "displayName": "Enum.Parse/TryParse",
      "closureStatus": "in-progress",
      "methodCount": 8,
      "...": "..."
    }
  ],

  "waiverSummary": {
    "totalWaivers": 1,
    "activeWaivers": 1,
    "expiredWaivers": 0,
    "totalExclusions": 1,
    "totalPlatformBlocked": 0
  },

  "sourceLinks": {
    "subjectSource": "subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/",
    "verificationSource": "verification/catalog/programs/foundation-dll-translation-audit.program.json",
    "generatedCode": "artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/",
    "evidence": "artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/",
    "authorityDocs": "docs/verification/foundation-dll-audit/"
  },

  "projects": [
    {
      "projectCode": "audit-input-and-ledger",
      "executionState": "passed",
      "policyState": "required",
      "artifacts": [],
      "supportRefs": [],
      "blockers": [],
      "updatedAt": "2026-04-25T23:00:00+08:00"
    },
    {
      "projectCode": "native-proof",
      "executionState": "passed",
      "policyState": "required",
      "artifacts": [
        {
          "assemblyName": "System.Private.CoreLib",
          "projectCode": "native-proof",
          "displayName": "native-reference.runtime-skeleton.coverage.json",
          "path": "artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json",
          "artifactKind": "report-json",
          "linkTargetType": "file",
          "role": "proof",
          "required": false,
          "exists": true
        }
      ],
      "blockers": []
    },
    {
      "projectCode": "completion-certification",
      "executionState": "in-progress",
      "policyState": "required",
      "artifacts": [],
      "blockers": [],
      "updatedAt": ""
    }
  ]
}
```

### 4.1 DLL Detail 的解读

**Capability Closure 视图**：
- 20 个 family 中 18 个 closed、1 个 waived、1 个 excluded
- closurePercent = 90%，但 DLL 状态仍是 `in-progress` 而非 `completed`
- 原因：waived 和 excluded 的 family 虽然不阻塞，但 `completion-certification` gate 尚未通过
- 这展示了**Capability Closure ≠ DLL Completion**——closure 是能力覆盖，completion 是形式化流程

**capabilityFamilies[] 结构**：
- 每个 family 包含完整的 closure、gate、implementation、source 信息
- `waiverRecords` 为空（closed family 不需要 waiver）
- `implementationFamilies` 可跳转到具体的 codegen handler
- `sourceGroups` 可跳转到源码类型

**sourceLinks**：
- 提供审核者可直接点击的源码、验证代码、生成代码、证据入口
- 每个字段都是目录级路径，点击跳转到文件浏览器

## 5. 双轴 Progress 与之前单轴 progressPercent 的对比

### 5.1 之前（schemaVersion = 1）

```json
{
  "summary": {
    "dllCount": 14,
    "completedCount": 0,
    "progressPercent": 0.0
  }
}
```

缺少粒度：审核者不知道 0% 是因为能力不够还是流程没走。

### 5.2 现在（schemaVersion = 2）

```json
{
  "summary": {
    "dllCount": 14,
    "completedCount": 0,
    "progressPercent": 0.0,
    "capabilityClosure": {
      "totalFamilies": 120,
      "closedFamilies": 18,
      "closurePercent": 15.0
    },
    "workflowProgress": {
      "totalRequiredGates": 360,
      "passedGates": 45,
      "progressPercent": 12.5
    },
    "dllCompletion": {
      "completedDllCount": 0
    }
  }
}
```

审核者现在可以看到：
- 能力闭包 15%（120 个 family 中 18 个 closed）
- 流程推进 12.5%（360 个 gate 中 45 个 passed）
- DLL 完成 0 个（14 个 DLL 中还没达到 completion 的）

这三个百分比**不同是正常的**，因为它们计算不同的分母和分子。

## 6. 变更日志

| 日期 | 版本 | 变更 | 原因 |
|------|------|------|------|
| 2026-04-26 | 1.0.0 | 初始版本 | Phase 2 初始创建 |
