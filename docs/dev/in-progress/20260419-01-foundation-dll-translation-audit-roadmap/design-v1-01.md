# Foundation DLL Translation Audit Design v1.01

Date: 2026-04-19 12:00:00 +08:00
Status: draft-approved-for-roadmap

## 1. 设计目标

建立一条明确、可追踪、可视、可验收、可验证的 DLL 翻译审计路径。

目标链路固定为：

`source implementation -> framework DLL input -> surface ledger -> semantic ledger -> nativeization plan -> generated native code -> native build/run -> hotupdate patch/host -> audit packet -> codegen review`

其中 `audit packet` 是每个 DLL 的最小验收单位。

## 2. 审计路径定义

每个 DLL 必须输出以下证据：

- `audit/<assembly>/input-manifest.json`
- `audit/<assembly>/surface-ledger.json`
- `audit/<assembly>/semantic-ledger.json`
- `audit/<assembly>/nativeization-plan.json`
- `audit/<assembly>/managed-proof-report.json`
- `audit/<assembly>/native-proof-report.json`
- `audit/<assembly>/hotupdate-proof-report.json`
- `audit/<assembly>/benchmark-report.json`
- `audit/<assembly>/codegen-review.json`
- `audit/<assembly>/codegen-review.md`
- `audit/<assembly>/completion-report.json`

`completion-report.json` 只能在以下状态全部满足时标记为 `completed`：

- public surface 没有未枚举成员。
- semantic ledger 没有未分类成员。
- runtime-backed 成员都有 helper contract 或明确 supported binding。
- nativeization plan 没有 unresolved lowering item。
- generated native code 可审查、可编译、可运行。
- managed proof 通过。
- native proof 通过。
- 按要求触发的 hotupdate proof 通过。
- codegen review JSON 与 Markdown 都存在并通过 schema/checklist。

## 3. `System.Private.CoreLib` Full Verification Gate

`System.Private.CoreLib` 是第一阶段唯一硬目标。

它的 completion gate 不接受以下替代证据：

- 只接受第一批 semantic-family 已通过。
- 只接受 dependency-layer summary 投影。
- 只接受 managed proof 通过。
- 只接受 native build 成功。
- 只接受 hotupdate consumer lane 通过。

它必须具备自己的 full verification packet：

- CoreLib runtime input manifest。
- CoreLib public/runtime surface ledger。
- CoreLib semantic-family coverage matrix。
- CoreLib runtime helper contract coverage。
- CoreLib generated native code review。
- CoreLib managed/native proof summary。
- CoreLib hotupdate consumer proof summary。
- CoreLib final certification report。

## 4. Complex BCL 13 DLL 顺序验证

Complex BCL 13 DLL 必须按固定顺序执行，不允许并行跳过前置失败项：

1. `System.Collections.Immutable`
2. `System.Formats.Asn1`
3. `System.IO.Compression.Brotli`
4. `System.IO.Compression.ZipFile`
5. `System.IO.Pipelines`
6. `System.Linq`
7. `System.Net.ServerSentEvents`
8. `System.ObjectModel`
9. `System.Runtime.InteropServices`
10. `System.Runtime.Serialization.Formatters`
11. `System.Security.Principal.Windows`
12. `System.Text.Json`
13. `System.Threading.Tasks.Parallel`

每个 DLL 的执行步骤固定为：

1. 生成或刷新 audit input manifest。
2. 校验 surface/semantic/nativeization plan。
3. 执行 managed proof。
4. 执行 native proof。
5. 如果能实现或触发条件成立，执行 hotupdate proof。
6. 生成 codegen review JSON。
7. 生成 codegen review Markdown。
8. 更新 per-DLL completion report。

## 5. Subject 与 `.sln` 产物链

推荐新增 `FoundationDllTranslationSolution`，挂在 `subjects/SolutionCorePack/source/EngineeringScenarios/` 下。

职责：

- 明确引用目标 DLL 集合。
- 提供用户可打开/可 build 的 `.sln`。
- 承载 managed proof project。
- 触发 native project/codegen 输出。
- 触发 hotupdate patch/host project 输出。
- 在 artifacts 中沉淀从代码到 native 再到 hotupdate 的产物链。

预期形态：

```text
subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/
  FoundationDllTranslationSolution.sln
  App/
  Proofs/
  HotUpdatePatch/
  HotUpdateHost/
```

产物链：

```text
subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/analysis/source/
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/analysis/host-input/
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/analysis/analysis/
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/analysis/generated/
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/matrices/windows-native-check/build/
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/matrices/windows-native-check/runtime/
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/hotupdate/
  -> artifacts/subjects/SolutionCorePack/runs/<run-id>/audit/
```

## 6. Codegen Review Schema

`codegen-review.json` 最小字段：

```json
{
  "schemaVersion": 1,
  "assemblyName": "System.Private.CoreLib",
  "generatedAt": "2026-04-19T12:00:00+08:00",
  "sourceInputs": [],
  "generatedFiles": [],
  "nativeDispatchManifest": null,
  "hotupdateBindingManifest": null,
  "runtimeHelpers": [],
  "coverage": {
    "publicMembersTotal": 0,
    "publicMembersCodegenCovered": 0,
    "uncoveredMembers": []
  },
  "findings": [],
  "finalStatus": "pending"
}
```

`codegen-review.md` 必须包含：

- Assembly summary。
- Generated file inventory。
- Public member coverage。
- Runtime helper binding review。
- Native dispatch review。
- Hotupdate binding review。
- Findings。
- Final recommendation。

## 7. HotUpdate 路径原则

hotupdate path 保留为 consumer verification：

- foundation DLL completion 不由 hotupdate 反向驱动。
- 如果 DLL 的能力穿过 patch/host 边界，hotupdate proof 必须为 `Proof`。
- 如果当前无法实现完整 hotupdate proof，必须输出 blocker 到 per-DLL completion report，不能静默跳过。

## 8. 风险

- CoreLib full gate 可能暴露此前 semantic-family batch 没覆盖的 runtime-backed helper 缺口。
- 13 个 Complex BCL DLL 中，`System.Text.Json`、`System.Runtime.InteropServices`、`System.Runtime.Serialization.Formatters` 风险最高。
- 新增 `FoundationDllTranslationSolution` 会触碰 subject/workspace/codegen 主线，必须先补 unit/contracts，再改实现。
- codegen review 如果只做文件存在检查，会退化成伪审核，必须至少覆盖 manifest、dispatch、helper、coverage 和 findings。

## 9. DLL-First 审核与报告对象模型

本路线图的最终审核对象不再以 `subject`、`ScenarioBase` 或 capability projection 作为主入口，而改为以 DLL 为中心的正式对象：

`Program -> DLL -> Verification Project -> Verification Item -> Artifact`

约束如下：

- `subject` 仅保留为执行入口、`.sln` 承载体和产物沉淀位置，不再作为最终审核首页的主对象。
- `Program` 固定表示本轮 foundation DLL translation audit 的 14 个目标对象。
- `DLL` 固定表示一个可独立验收的 assembly 审核单元。
- `Verification Project` 固定表示一个 DLL 下的正式验证项目，而不是临时 run、日志或人工口头结论。
- `Artifact` 必须是可点击证据；没有证据的绿色状态一律视为 `missing-evidence`。

### 9.1 Program Scope

当前 program scope 固定为以下 14 个对象：

1. `System.Private.CoreLib`
2. `System.Collections.Immutable`
3. `System.Formats.Asn1`
4. `System.IO.Compression.Brotli`
5. `System.IO.Compression.ZipFile`
6. `System.IO.Pipelines`
7. `System.Linq`
8. `System.Net.ServerSentEvents`
9. `System.ObjectModel`
10. `System.Runtime.InteropServices`
11. `System.Runtime.Serialization.Formatters`
12. `System.Security.Principal.Windows`
13. `System.Text.Json`
14. `System.Threading.Tasks.Parallel`

`Program` 总进度必须直接回答：

- 14 个 DLL 当前完成了几个。
- 当前 active DLL 是哪个。
- 哪个 DLL 正在阻塞顺序推进。
- proof / benchmark / hotupdate 的 project 覆盖情况是什么。

### 9.2 Verification Project 模板

每个 DLL 固定使用以下 project 模板：

- `audit-input-and-ledger`
- `managed-proof`
- `native-proof`
- `hotupdate-proof`
- `benchmark`
- `codegen-review`
- `completion-certification`

默认必选 project：

- `audit-input-and-ledger`
- `managed-proof`
- `native-proof`
- `codegen-review`
- `completion-certification`

策略驱动 project：

- `hotupdate-proof`
- `benchmark`

### 9.3 Project Policy

`hotupdate-proof` 与 `benchmark` 不得再被隐含理解为“14 个 DLL 一刀切硬门槛”，而必须显式声明 policy：

- `System.Private.CoreLib` 的 `hotupdate-proof` 为 `required`
- 其余 13 个 DLL 的 `hotupdate-proof` 默认为 `conditional`
- 全部 14 个 DLL 的 `benchmark` 默认为 `conditional`
- 只有某个 DLL 被显式声明存在 perf obligation 时，`benchmark` 才能升级为 `required`

高风险 DLL 必须在 DLL 详情页暴露 `riskTag`：

- `System.Runtime.InteropServices`
- `System.Runtime.Serialization.Formatters`
- `System.Text.Json`

## 10. 状态模型与证据规则

### 10.1 状态模型

每个 `Verification Project` 固定拆成两层状态：

- `policyState`: `required | conditional | not-required`
- `executionState`: `pending | in-progress | passed | failed | blocked | missing-evidence | not-required`

每个 `DLL` 固定汇总为：

- `not-started`
- `in-progress`
- `blocked`
- `completed`

`Program` 状态按 14 个 DLL 汇总，但必须同时显示计数：

- `completedDllCount`
- `activeDllCount`
- `blockedDllCount`
- `notStartedDllCount`

### 10.2 Artifact 证据规则

`Artifact` 不是文件名字符串，而是可点击证据对象。固定规则如下：

- 如果证据是代码文件，则链接到具体文件路径。
- 如果证据是一组生成代码、运行输出或审计产物，则链接到目录路径。
- 如果证据是报告，则链接到具体 `json`、`md` 或后续 `html`。
- 如果证据来自命令执行，则链接到日志文件或归档目录，而不是只写 `passed`。

建议固定 `ArtifactRecord` 字段：

- `artifactKind`: `code-file | directory | report-json | report-md | log | binary | manifest`
- `displayName`
- `path`
- `linkTargetType`: `file | directory`
- `role`: `input | proof | benchmark | codegen | review | completion | log`
- `required`
- `exists`

`exists = false` 且 project 被标绿时，必须回退为 `missing-evidence`。

### 10.3 Project 级证据要求

每个 project 至少要能回答三件事：

- 怎么验证。
- 当前状态。
- 证据在哪。

各 project 的主证据规则固定为：

- `audit-input-and-ledger`
  - 目录：`audit/<assembly>/`
  - 文件：`input-manifest.json`、`surface-ledger.json`、`semantic-ledger.json`、`nativeization-plan.json`
- `managed-proof`
  - 文件：`managed-proof-report.json`
  - 目录：managed proof 工程或运行输出目录
- `native-proof`
  - 文件：`native-proof-report.json`
  - 目录：generated code 目录、native build/runtime 目录
- `hotupdate-proof`
  - 文件：`hotupdate-proof-report.json`
  - 目录：patch/host 对应输出目录
- `benchmark`
  - 文件：`benchmark-report.json`
  - 目录：benchmark 原始结果目录
- `codegen-review`
  - 文件：`codegen-review.json`、`codegen-review.md`
  - 目录：generated code 目录
- `completion-certification`
  - 文件：`completion-report.json`

## 11. 页面与报告合同

最终审核报告固定提供四层入口：

- `Program Overview`
- `DLL Matrix`
- `DLL Detail`
- `Artifact Index`

### 11.1 Program Overview

必须直接展示：

- 14 个 DLL 的总进度。
- 当前 active DLL。
- 顺序推进阻塞链。
- proof / benchmark / hotupdate 的 project 覆盖概览。

### 11.2 DLL Matrix

固定为一行一个 DLL，列为 `Verification Project` 模板。每个单元格必须同时提供：

- project 状态
- 主证据链接
- 如果阻塞，则显示 blocker 摘要

### 11.3 DLL Detail

每个 DLL 详情页必须按 project 展示：

- `policyState`
- `executionState`
- `verificationMethod`
- `verificationTarget`
- `completionRule`
- `artifacts`
- `blockers`

### 11.4 Artifact Index

Artifact Index 不是附录，而是正式审核入口。它必须允许审核者从 DLL 维度直接点进：

- 代码文件
- 生成代码目录
- proof / benchmark / hotupdate 输出目录
- JSON / Markdown 审计报告

如果某个 DLL 的结论无法跳转到证据，则该结论不成立。
