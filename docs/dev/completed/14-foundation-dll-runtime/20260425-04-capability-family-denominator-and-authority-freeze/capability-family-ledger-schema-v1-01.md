# Capability-Family Ledger Schema v1.01

Date: 2026-04-25 23:00:00 +08:00
Status: draft
Phase: phase-0 (20260425-04)
Parent: foundation-dll-capability-closure-reporting-roadmap

## 1. 目标

定义 `capability-family ledger` 的正式 schema，作为 foundation DLL capability closure 的**报告 authority** 来源。

Ledger 的核心作用：
- 为每个 foundation DLL 列出必须闭包的所有 capability family
- 为每个 family 提供可追溯的 composition authority
- 为报告生成器提供统一的分母（denominator）

## 2. 设计原则

1. **审核者优先**: 粒度以审核者能判断"这个能力族是否完整"为准，而非以工具方便为准
2. **authority 可追溯**: 每个 family 的组成和 closure 状态必须能追溯到原始 audit artifact
3. **迭代渐进**: 第一版 ledger 不追求完美，追求可用和可审核
4. **自动推导优先**: family 内容尽量由工具从现有 artifact 推导，减少手工维护
5. **从属不独立**: implementation family 和 source grouping 是 semantic family 的投影层，不作为独立权威粒度

## 3. Family 层次结构

Ledger 采用三层结构，但只有顶层是**主粒度**：

```
Semantic Family (主粒度 — 审核主体)
  ├── Implementation Family (从属 — codegen 实现映射)
  │     └── Source Group (从属 — 源码入口)
  └── Verification Gates (从属 — 流程验证状态)
```

### 3.1 Semantic Family（语义族）

定义：一组在语义上相关的方法/API，共同提供一项可独立判断"是否完整"的能力。

特征：
- 语义内聚：名称、行为、异常语义一致
- 审核者可独立判断闭包：审核者可以回答"这个族的所有方法是否都有了正确的 native 实现"
- 不跨 DLL：一个 semantic family 只属于一个 DLL（但一个 DLL 可以有多个 family）
- 适度宏观：不小于 5 个方法，不大于 200 个方法（经验值，允许例外）

示例：
- `System.Convert::ToXxx` 所有重载构成一个 semantic family
- `System.String::Format` 所有重载构成一个 semantic family
- `System.Buffer::MemoryCopy` 与 `System.Buffer::BlockCopy` 构成一个 semantic family

### 3.2 Implementation Family（实现族）

定义：semantic family 在 native codegen 层的实现映射。一个 semantic family 可以映射到多个 implementation family。

特征：
- 从属粒度：不独立出现于报告，仅作为 semantic family 的展开细节
- 实现形态一致：相同 codegen 策略/模板/emitter handler 的方法归为一组
- 可推导：由工具从 emitter routing 信息自动生成，不手工维护

示例（针对 `System.Convert::ToChar` semantic family）：
- `checked-primitive-convert`: `ToChar(SByte)`, `ToChar(Int16)`, `ToChar(UInt16)` 等共用 checked primitive convert 模板
- `primitive-return-forwarder`: `ToChar(Int32)`, `ToChar(Int64)` 共用 primitive return forwarder
- `boxed-iconvertible-invalid-cast`: `ToChar(Boolean)`, `ToChar(Single)`, `ToChar(Double)` 共用 boxed IConvertible invalid cast
- `boxed-value-type-invalid-cast`: `ToChar(Decimal)`, `ToChar(DateTime)` 共用 boxed value-type invalid cast
- `string-char-provider`: `ToChar(String)`, `ToChar(String, IFormatProvider)` 共用 string char provider
- `object-char-provider`: `ToChar(Object)`, `ToChar(Object, IFormatProvider)` 共用 object char provider

### 3.3 Source Group（源码分组）

定义：semantic family 在源码层的入口分组。这是最细粒度的映射层，直接对应 DLL 中的类型和方法集合。

特征：
- 仅用于跳转到源码的入口分组，不作为审核粒度
- 直接映射到 DLL method subject IDs
- 自动从 surface/semantic ledger 推导

示例（针对 `System.Convert::ToChar` semantic family）：
```
Source Group 1: System.Convert (all static ToChar overloads)
Source Group 2: System.IConvertible (ToChar interface implementation)
```

## 4. Ledger 字段定义

### 4.1 Ledger 顶层字段

```json
{
  "schemaVersion": "1.0.0",
  "ledgerId": "foundation-dll-capability-family-v1",
  "title": "Foundation DLL Capability Family Ledger",
  "generatedAt": "2026-04-25T23:00:00+08:00",
  "authoritySnapshotOf": "2026-04-25T22:00:00+08:00",
  "originalAuthorityDigest": {
    "auditInputManifestVersion": "...",
    "surfaceLedgerVersion": "...",
    "semanticLedgerVersion": "...",
    "nativeizationPlanVersion": "..."
  },
  "dlls": [ ... ]
}
```

### 4.2 DLL 条目字段

```json
{
  "assemblyName": "System.Private.CoreLib",
  "assemblyDisplayName": "System.Private.CoreLib",
  "orderIndex": 1,
  "phase": "phase-1",
  "families": [ ... ],
  "completionStatus": "in-progress",
  "sourceLinks": {
    "subjectSource": "subjects/SolutionCorePack/source/...",
    "verificationSource": "verification/catalog/programs/...",
    "generatedCode": "artifacts/subjects/SolutionCorePack/runs/.../generated/runtime/",
    "evidence": "artifacts/subjects/SolutionCorePack/runs/.../",
    "authorityDocs": "docs/verification/foundation-dll-audit/..."
  }
}
```

### 4.3 Capability Family 条目字段

```json
{
  "familyId": "family/System.Private.CoreLib/convert/to-char",
  "displayName": "Convert.ToChar",
  "description": "System.Convert.ToChar 方法族的所有重载，包括 checked primitive convert、primitive return forwarder、boxed IConvertible invalid-cast、boxed value-type invalid-cast、string/object char provider 和 IConvertible.ToChar 接口实现",
  "familyType": "semantic",
  "scope": {
    "methodCount": 18,
    "typeNames": ["System.Convert", "System.IConvertible"],
    "subjectIdPatterns": [
      "System.Convert::ToChar*",
      "System.IConvertible::ToChar*"
    ],
    "excluded": {
      "patterns": [],
      "reason": ""
    }
  },
  "closureStatus": "closed",
  "closureEvidence": {
    "coverageJson": "artifacts/subjects/SolutionCorePack/runs/.../runtime-skeleton.coverage.json",
    "nativeProofReport": "artifacts/subjects/SolutionCorePack/runs/.../native-hotupdate-audit.json",
    "codegenReview": "artifacts/subjects/SolutionCorePack/runs/.../codegen-review.json"
  },
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
    },
    {
      "familyId": "impl/System.Private.CoreLib/convert/to-char/boxed-iconvertible-invalid-cast",
      "displayName": "Boxed IConvertible Invalid Cast",
      "handlerName": "TryBuildRuntimeSkeletonConvertBoxedIConvertibleInvalidCastStub",
      "templateName": "NativeReferenceProof.RuntimeSkeleton.BoxedValueTypeInstanceCallStub.cpp.scriban",
      "methodSubjectIds": [
        "System.Convert::ToChar(System.Boolean)",
        "System.Convert::ToChar(System.Single)",
        "System.Convert::ToChar(System.Double)"
      ]
    },
    {
      "familyId": "impl/System.Private.CoreLib/convert/to-char/boxed-value-type-invalid-cast",
      "displayName": "Boxed Value-Type Invalid Cast",
      "handlerName": "TryBuildRuntimeSkeletonConvertBoxedValueTypeCharInvalidCastStub",
      "templateName": "NativeReferenceProof.RuntimeSkeleton.BoxedValueTypeInstanceCallStub.cpp.scriban",
      "methodSubjectIds": [
        "System.Convert::ToChar(System.Decimal)",
        "System.Convert::ToChar(System.DateTime)"
      ]
    },
    {
      "familyId": "impl/System.Private.CoreLib/convert/to-char/string-char-provider",
      "displayName": "String Char Provider",
      "handlerName": "TryBuildRuntimeSkeletonConvertStringCharProviderStub",
      "templateName": "NativeReferenceProof.RuntimeSkeleton.StringCharProviderStub.cpp.scriban",
      "methodSubjectIds": [
        "System.Convert::ToChar(System.String)",
        "System.Convert::ToChar(System.String,System.IFormatProvider)"
      ]
    },
    {
      "familyId": "impl/System.Private.CoreLib/convert/to-char/object-char-provider",
      "displayName": "Object Char Provider",
      "handlerName": "TryBuildRuntimeSkeletonConvertObjectCharProviderStub",
      "templateName": "NativeReferenceProof.RuntimeSkeleton.ObjectCharProviderStub.cpp.scriban",
      "methodSubjectIds": [
        "System.Convert::ToChar(System.Object)",
        "System.Convert::ToChar(System.Object,System.IFormatProvider)"
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
  ],
  "verificationGates": {
    "audit-input-and-ledger": "passed",
    "managed-proof": "passed",
    "native-proof": "passed",
    "hotupdate-proof": "passed",
    "benchmark": "conditional",
    "codegen-review": "passed"
  },
  "waiverRecords": [],
  "authorityRef": "original-authority://System.Private.CoreLib/semantic-ledger#convert-family"
}
```

### 4.4 关键字段约束

| 字段 | 约束 |
|------|------|
| `familyId` | 全局唯一，不可变；格式见 §5 |
| `familyType` | 固定为 `semantic`（顶层）、`implementation`（从属）、`source`（从属） |
| `closureStatus` | `closed` / `in-progress` / `waived` / `excluded` / `platform-blocked` |
| `closureEvidence` | `closureStatus != closed` 时可为空；`closed` 时至少一个 evidence 路径 |
| `implementationFamilies` | 从属信息，不可单独存在 |
| `sourceGroups` | 从属信息，不可单独存在 |
| `verificationGates` | 每个 gate 的状态与 §10.1 状态模型一致 |
| `waiverRecords` | 非空时必须包含 `authorityRef` 和 `reason` |

## 5. Family ID 命名规范

### 5.1 Semantic Family ID

格式: `family/{dll-name}/{category}[/{sub-category}]`

- `{dll-name}`: DLL 的程序集名称（不含 `.dll`）
- `{category}`: 小写连字符式语义分类，如 `convert`、`string-format`、`buffer`、`math`、`enum`
- `{sub-category}`: 可选，用于区分同一分类下的子族，如 `convert/to-char`、`convert/to-string`

示例:
- `family/System.Private.CoreLib/convert/to-char`
- `family/System.Private.CoreLib/convert/to-string`
- `family/System.Private.CoreLib/convert/to-datetime`
- `family/System.Private.CoreLib/math/abs`
- `family/System.Private.CoreLib/string/format`
- `family/System.Private.CoreLib/buffer/memory-copy`

### 5.2 Implementation Family ID

格式: `impl/{dll-name}/{category}[/{sub-category}]/{handler-short-name}`

示例:
- `impl/System.Private.CoreLib/convert/to-char/checked-primitive-convert`
- `impl/System.Private.CoreLib/convert/to-string/static-string-call-forwarder`

### 5.3 Source Group ID

格式: `src/{type-full-name}`

示例:
- `src/System.Convert`
- `src/System.IConvertible`
- `src/System.String`

### 5.4 命名约束

- 只使用小写字母、数字、连字符（`-`）和斜杠（`/`）
- 禁止使用空格、下划线、点号（DLL 名除外——DLL 名本身含点号时保留）
- DLL 名中的点号仅出现在第一段中（`family/System.Private.CoreLib/...`）
- category 段使用连字符分隔多单词（`to-string`，非 `tostring` 或 `to_string`）
- implementation family ID 必须在对应的 semantic family ID 后追加 `/{handler-short-name}`

## 6. Authority 分层模型

### 6.1 两层 authority

```
原始 authority (Original Authority)
  ├── audit input manifest: DLL 中存在哪些方法
  ├── surface ledger: 哪些方法是公共 surface
  ├── semantic ledger: 方法的语义分类
  └── nativeization plan: 方法到 native codegen 的映射策略

        ↓ 推导 / 聚合 / 冻结

报告 authority (Report Authority)
  └── capability-family ledger: 冻结的 family 构成和 closure 状态
```

### 6.2 原始 authority 的各来源

| 来源 | 用途 | 更新频率 |
|------|------|----------|
| `audit/<assembly>/input-manifest.json` | 提供 DLL 的完整方法清单，作为 family scope 的"底盘" | 随 DLL 版本更新 |
| `audit/<assembly>/surface-ledger.json` | 区分 public/internal/private surface，影响 family 闭包判定 | 随 DLL 版本更新 |
| `audit/<assembly>/semantic-ledger.json` | 提供方法的语义分类标签，是 family 划分的主要依据 | 随语义审查更新 |
| `audit/<assembly>/nativeization-plan.json` | 提供方法到 native codegen 的策略映射，推导 implementation family | 随 codegen emitter 更新 |

### 6.3 报告 authority 的特征

- 是原始 authority 经过**推导、聚合和冻结**后的快照
- 一旦冻结，在两次冻结之间不变
- 报告生成器以 ledger 的 `authoritySnapshotOf` 时间戳为准
- 报告 authority 的变更必须通过正式流程（见 denominator-authority-contract）

## 7. Family 闭包判定规则

一个 semantic family 满足以下**全部**条件时可标记为 `closed`：

1. **scope 闭合**: 所有属于该 family 的 method subject ID 都已被枚举
2. **coverage 闭合**: 所有方法都有 native codegen coverage（evidence 中 coverage JSON 显示为 covered）
3. **native proof 通过**: 对应 native proof 报告为 `passed`（或 `passed-with-current-canonical-evidence`）
4. **codegen review 通过**: 对应 codegen review 中没有未解决的 finding
5. **waiver 可接受**: 任何未闭包的方法都有正式的 waiver / exclusion / platform-blocked 记录，且 authority 正确
6. **evidence 存在**: 所有闭包证据都是可点击的 artifact 路径，不是空口声明

## 8. 从属粒度的可推导规则

### 8.1 Implementation Family 推导

Implementation family 应由工具从以下信息自动推导：
- `NativeReferenceProofEmitter` 中 runtime-skeleton family handler 的 routing 配置
- 每个 handler 匹配到的 method subject ID 集合
- handler 对应的 Scriban 模板路径

推导产物：
- 每个 handler 对应一个 implementation family
- handler 匹配的所有 method subject ID 列为该 implementation family 的 `methodSubjectIds`

### 8.2 Source Group 推导

Source group 应由工具从以下信息自动推导：
- semantic ledger 中每个方法的 declaring type
- 按照 declaring type 分组，形成 source group

推导产物：
- 每个 declaring type 对应一个 source group
- 同一 `System.Convert` 类型下的所有方法归入同一个 source group

## 9. Ledger 完整性约束

- 每个 foundation DLL 条目必须包含至少一个 semantic family
- 每个 semantic family 的 `familyId` 必须在 ledger 内唯一
- 每个 semantic family 的 scope 方法集合不得与其他 semantic family 重叠（互斥约束）
- 每个 semantic family 必须覆盖 ledgerscope 中该 DLL 的全部相关方法
- implementation family 的 method 集合必须是其所属 semantic family 的子集
- source group 的 method 集合必须是其所属 semantic family 的子集
- closure 为 `closed` 的 family 必须提供至少一条 closure evidence 路径
- waiver 记录必须引用正式的 authority artifact（不允许仅引用 task STATUS.md）

## 10. 与现有架构的关系

### 10.1 与 `foundation-dll-translation-audit.program.json`

Ledger 的 DLL 列表和 orderIndex 必须与 `program.json` 的 `assemblies[]` 保持一致。

### 10.2 与 `Verification Project`

每个 semantic family 的 `verificationGates` 字段映射到 `program.json` 的 `projectTemplates[]`。Gate 状态的值与 `design-v1-01.md` §10.1 的状态模型一致：
- `policyState`: `required | conditional | not-required`
- `executionState`: `pending | in-progress | passed | failed | blocked | missing-evidence | not-required`

### 10.3 与 `DLL completion`

DLL 的 `completionStatus` 由所有 semantic family 的 `closureStatus` 汇总决定：
- 所有 family 为 `closed` → `completed`
- 存在 family 为 `in-progress` → `in-progress`
- 存在 family 为 `waived`/`excluded`/`platform-blocked` 且 authority 正确 → 取决于剩余 family 的 closure 状态

## 11. 变更管理

Ledger schema 的变更（增加字段、修改字段定义、修改约束）需要：
1. 更新本 schema 文档
2. 更新 `authoritySnapshotOf` 时间戳
3. 在变更日志中记录变更摘要
4. 触发对应 Phase 的 re-freeze

## 12. 附录：字段类型定义

```
familyId: string(pattern: "^family/[^/]+/.+$")
familyType: "semantic" | "implementation" | "source"
closureStatus: "closed" | "in-progress" | "waived" | "excluded" | "platform-blocked"
methodCount: integer(min: 1)
evidencePath: string(pattern: "^(artifacts|audit|verification)/.+")
authorityRef: string(pattern: "^(original-authority|report-authority)://.+")
```

## 13. 变更日志

| 日期 | 版本 | 变更 | 原因 |
|------|------|------|------|
| 2026-04-25 | 1.0.0 | 初始版本 | Phase 0 初始创建 |
