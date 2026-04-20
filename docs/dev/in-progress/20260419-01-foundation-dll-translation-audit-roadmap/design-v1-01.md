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

