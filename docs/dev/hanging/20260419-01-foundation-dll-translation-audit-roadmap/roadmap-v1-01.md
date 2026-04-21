# Foundation DLL Translation Audit Roadmap v1.01

Date: 2026-04-19 12:00:00 +08:00
Status: in-progress

## 1. 目标

建立一条新的 audit-first DLL translation roadmap，用来证明目标 DLL 不只是被 layer summary 标记为 `ok`，而是具备完整的、可审计的翻译与验证证据。

最终目标：

- `System.Private.CoreLib` 首先完成 full verification。
- Complex BCL 13 DLL 按约定顺序逐个完成验证。
- subject 中存在可生成 `.sln` 的验证项目。
- 用户能看到 `managed -> native -> hotupdate` 的完整产物链。
- 每个 DLL 都有 JSON + Markdown 的 codegen review。
- 每个阶段都能产出可机器检查、可人工审阅的 audit packet。

## 2. 范围边界

本 roadmap 负责：

- 定义 DLL 完整翻译的 audit schema。
- 定义 `System.Private.CoreLib` full verification gate。
- 定义 Complex BCL 13 DLL 的顺序执行与验收路径。
- 定义 `FoundationDllTranslationSolution` subject/workspace 产物链。
- 定义 codegen review JSON + Markdown 输出。
- 定义 managed/native/hotupdate 的验收路径。

本 roadmap 不负责：

- 在父 roadmap 文档中直接实现全部 DLL 翻译。
- 把 hotupdate 变成 foundation 能力发现入口。
- 用外部业务 DLL 取代 foundation DLL 验证。
- 把 Phase 4 中明显 dynamic-heavy 或 platform-heavy DLL 伪标为 complete。

## 3. 非目标

- 不接受只用 layer-level `finalStatus = ok` 作为完整 DLL 翻译结论。
- 不接受只跑 managed proof 就声称 native 完成。
- 不接受只生成 native code 但不进行 codegen review。
- 不接受 subject 没有 `.sln` 或用户无法本地编译验证。
- 不记录 Unity 目标信息；本 roadmap 只记录 DLL scope 与验证链路。

## 4. 阶段列表

### Phase 0: Audit Schema And Subject Chain Freeze

- goal:
  - 冻结 audit packet schema、codegen review schema、subject `.sln` 产物链。
  - 决定并落地 `FoundationDllTranslationSolution` 的最小骨架。
- exit_criteria:
  - `audit-packet.schema.json` 存在。
  - `codegen-review.schema.json` 存在。
  - `FoundationDllTranslationSolution.sln` 能生成或存在设计入口。
  - `collector -> registry -> workspace` gate 有自动化检查。
- deliverables:
  - `audit-packet.schema.json`
  - `codegen-review.schema.json`
  - `FoundationDllTranslationSolution` subject design/plan
  - workspace artifact chain test
- dependencies:
  - `docs/architecture/managed-native-hotupdate-test-pipeline.md`
  - `docs/architecture/reference-assembly-and-corelib-policy.md`
- open_questions:
  - 是否立即新增独立 solution。当前推荐新增。

### Phase 1: System.Private.CoreLib Full Verification

- goal:
  - 按方案 C 完整验证 `System.Private.CoreLib`。
- exit_criteria:
  - CoreLib audit packet 完整。
  - CoreLib surface ledger 无未枚举成员。
  - CoreLib semantic ledger 无未分类成员。
  - CoreLib runtime helper coverage 无未决 blocker。
  - CoreLib managed proof 通过。
  - CoreLib native proof 通过。
  - CoreLib hotupdate consumer proof 通过或明确记录不可实现 blocker。
  - CoreLib codegen review JSON + Markdown 通过。
- deliverables:
  - `audit/System.Private.CoreLib/input-manifest.json`
  - `audit/System.Private.CoreLib/surface-ledger.json`
  - `audit/System.Private.CoreLib/semantic-ledger.json`
  - `audit/System.Private.CoreLib/nativeization-plan.json`
  - `audit/System.Private.CoreLib/managed-proof-report.json`
  - `audit/System.Private.CoreLib/native-proof-report.json`
  - `audit/System.Private.CoreLib/hotupdate-proof-report.json`
  - `audit/System.Private.CoreLib/codegen-review.json`
  - `audit/System.Private.CoreLib/codegen-review.md`
  - `audit/System.Private.CoreLib/completion-report.json`
- dependencies:
  - Phase 0
- open_questions:
  - CoreLib helper gaps 如果出现，按 blocker 子任务拆分，不允许绕过 gate。

### Phase 2: Ordered Complex BCL 13 DLL Verification

- goal:
  - 按固定顺序逐个验证 13 个 Complex BCL DLL。
- ordered_scope:
  - `System.Collections.Immutable`
  - `System.Formats.Asn1`
  - `System.IO.Compression.Brotli`
  - `System.IO.Compression.ZipFile`
  - `System.IO.Pipelines`
  - `System.Linq`
  - `System.Net.ServerSentEvents`
  - `System.ObjectModel`
  - `System.Runtime.InteropServices`
  - `System.Runtime.Serialization.Formatters`
  - `System.Security.Principal.Windows`
  - `System.Text.Json`
  - `System.Threading.Tasks.Parallel`
- exit_criteria:
  - 每个 DLL 都有独立 audit packet。
  - 每个 DLL 都完成 managed proof、native proof、codegen review。
  - hotupdate 可实现时必须同时实现；不可实现时必须写入 blocker。
  - 前一个 DLL 未完成时，不启动后一个 DLL 的 completion 声明。
- deliverables:
  - `audit/<assembly>/completion-report.json`
  - `audit/<assembly>/codegen-review.json`
  - `audit/<assembly>/codegen-review.md`
  - ordered lane summary
- dependencies:
  - Phase 1
- open_questions:
  - `System.Runtime.InteropServices` 与 `System.Runtime.Serialization.Formatters` 可能需要额外 runtime service。

### Phase 3: Core System DLL Scope Expansion

- goal:
  - 扩展到常用底层 System DLL 范围。
- scope:
  - `System.Runtime`
  - `System.Runtime.Extensions`
  - `System.ObjectModel`
  - `System.Collections`
  - `System.Collections.NonGeneric`
  - `System.Collections.Concurrent`
  - `System.Memory`
  - `System.Buffers`
  - `System.ValueTuple`
  - `System.Runtime.CompilerServices.Unsafe`
  - `System.Linq`
  - `System.Text.Encoding`
  - `System.Text.Encoding.Extensions`
  - `System.Text.RegularExpressions`
  - `System.IO`
  - `System.IO.FileSystem`
  - `System.Threading`
  - `System.Threading.Tasks`
  - `System.Threading.Tasks.Extensions`
  - `System.Text.Json`
- exit_criteria:
  - 每个 DLL 满足 audit packet gate。
  - subject `.sln` 能编译并消费这些 DLL 的代表性链路。
- deliverables:
  - per-DLL audit packets
  - core system lane summary
- dependencies:
  - Phase 1
- open_questions:
  - 与 Phase 2 重叠的 DLL 使用同一 audit packet，不重复创建 completion authority。

### Phase 4: Network IO Reflection Interop And Crypto Expansion

- goal:
  - 扩展到网络、压缩、反射、Interop、诊断、Numerics 与 Crypto 常用 DLL。
- scope:
  - `System.Net.Http`
  - `System.Net.Primitives`
  - `System.Net.Requests`
  - `System.Net.Security`
  - `System.IO.Compression`
  - `System.IO.Compression.ZipFile`
  - `System.Threading.Channels`
  - `System.Reflection`
  - `System.Reflection.Primitives`
  - `System.Reflection.TypeExtensions`
  - `System.Runtime.InteropServices`
  - `System.Diagnostics.Debug`
  - `System.Diagnostics.DiagnosticSource`
  - `System.Numerics`
  - `System.Runtime.Numerics`
  - `System.Security.Cryptography.Primitives`
  - `System.Security.Cryptography.X509Certificates`
- exit_criteria:
  - 风险 DLL 不允许伪完成；blocker 必须独立登记。
  - managed/native/hotupdate 路径按触发条件执行。
- deliverables:
  - per-DLL audit packets
  - blocker ledger
- dependencies:
  - Phase 3
- open_questions:
  - Crypto/X509 平台依赖是否需要单独 platform lane。

### Phase 5: Extended And Platform-Heavy DLL Classification

- goal:
  - 对扩展、平台、dynamic-heavy DLL 做分类、可行性与 blocker 审计。
- scope:
  - `System.Xml*`
  - `System.Runtime.Serialization*`
  - `System.Reflection.Emit*`
  - `System.ComponentModel*`
  - `System.Data*`
  - `System.Runtime.Loader`
  - `System.Console`
  - `Microsoft.Win32*`
  - `System.Drawing`
  - `WindowsBase`
  - `System.Windows`
  - `System.Configuration`
  - `System.ServiceProcess`
  - `System.ServiceModel.Web`
  - `Microsoft.VisualBasic*`
  - `Microsoft.CSharp`
- exit_criteria:
  - 每个 family 有分类结论：`ready-for-audit`、`requires-runtime-service`、`platform-blocked`、`out-of-current-scope`。
  - 不把 dynamic emit 或 platform-heavy DLL 直接伪标 complete。
- deliverables:
  - extended-dll-classification.json
  - blocker-roadmap.md
- dependencies:
  - Phase 4
- open_questions:
  - 哪些 platform-heavy DLL 需要独立 product target。

## 5. 每阶段完成定义

每个阶段只有满足以下条件才算完成：

- 阶段 scope 已冻结。
- 每个 DLL 都有 completion report 或 blocker report。
- 所有 JSON 产物可解析。
- 所有 Markdown review 可人工审阅。
- subject/workspace 产物链可追踪。
- required gates 有命令输出记录。
- `dotnet build` / `dotnet test` / `msbuild` 如发生崩溃，必须视为失败并先修复根因。

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260419-02-audit-schema-and-foundation-solution-freeze` | `phase-0` | `completed` | `codex` | 已冻结 audit/codegen schema，并创建 `FoundationDllTranslationSolution` 骨架 | `20260419-01` |
| `20260419-03-system-private-corelib-full-verification` | `phase-1` | `completed-blocked` | `codex` | 已建立 CoreLib audit packet skeleton 并完成 blocker normalization；仍需真实 DLL-boundary native/codegen/hotupdate 实现 | `20260419-02` |
| `20260419-04-complex-bcl-ordered-dll-01-system-collections-immutable` | `phase-2` | `blocked` | `codex` | 验证 `System.Collections.Immutable`，等待 CoreLib full verification 真正完成 | `20260419-03` |
| `20260419-05-complex-bcl-ordered-dll-02-system-formats-asn1` | `phase-2` | `planned` | `codex` | 验证 `System.Formats.Asn1` | `20260419-04` |
| `20260419-06-complex-bcl-ordered-dll-03-system-io-compression-brotli` | `phase-2` | `planned` | `codex` | 验证 `System.IO.Compression.Brotli` | `20260419-05` |
| `20260419-07-complex-bcl-ordered-dll-04-system-io-compression-zipfile` | `phase-2` | `planned` | `codex` | 验证 `System.IO.Compression.ZipFile` | `20260419-06` |
| `20260419-08-complex-bcl-ordered-dll-05-system-io-pipelines` | `phase-2` | `planned` | `codex` | 验证 `System.IO.Pipelines` | `20260419-07` |
| `20260419-09-complex-bcl-ordered-dll-06-system-linq` | `phase-2` | `planned` | `codex` | 验证 `System.Linq` | `20260419-08` |
| `20260419-10-complex-bcl-ordered-dll-07-system-net-serversentevents` | `phase-2` | `planned` | `codex` | 验证 `System.Net.ServerSentEvents` | `20260419-09` |
| `20260419-11-complex-bcl-ordered-dll-08-system-objectmodel` | `phase-2` | `planned` | `codex` | 验证 `System.ObjectModel` | `20260419-10` |
| `20260419-12-complex-bcl-ordered-dll-09-system-runtime-interopservices` | `phase-2` | `planned` | `codex` | 验证 `System.Runtime.InteropServices` | `20260419-11` |
| `20260419-13-complex-bcl-ordered-dll-10-system-runtime-serialization-formatters` | `phase-2` | `planned` | `codex` | 验证 `System.Runtime.Serialization.Formatters` | `20260419-12` |
| `20260419-14-complex-bcl-ordered-dll-11-system-security-principal-windows` | `phase-2` | `planned` | `codex` | 验证 `System.Security.Principal.Windows` | `20260419-13` |
| `20260419-15-complex-bcl-ordered-dll-12-system-text-json` | `phase-2` | `planned` | `codex` | 验证 `System.Text.Json` | `20260419-14` |
| `20260419-16-complex-bcl-ordered-dll-13-system-threading-tasks-parallel` | `phase-2` | `planned` | `codex` | 验证 `System.Threading.Tasks.Parallel` | `20260419-15` |
| `20260419-17-core-system-dll-scope-expansion` | `phase-3` | `planned` | `codex` | 扩展到底层常用 System DLL 范围 | `20260419-16` |
| `20260419-18-network-reflection-interop-crypto-expansion` | `phase-4` | `planned` | `codex` | 扩展到网络、反射、Interop、Crypto 等 DLL | `20260419-17` |
| `20260419-19-extended-platform-heavy-dll-classification` | `phase-5` | `planned` | `codex` | 对 extended/platform-heavy DLL 做分类与 blocker roadmap | `20260419-18` |

## 7. 依赖

- `docs/architecture/managed-native-hotupdate-test-pipeline.md`
- `docs/architecture/reference-assembly-and-corelib-policy.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`
- `subjects/SolutionCorePack/subject.features.json`
- `subjects/HotUpdateHostPack/subject.features.json`
- `subjects/MixedExecutionFeaturePack/subject.features.json`
- `src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis`
- `src/managed/Chaos.IL2CPP.CodeGen`

## 8. 风险

- 如果 Phase 0 不先冻结 schema，后续 DLL 会产生不可比较的 audit packet。
- 如果 CoreLib 不先完成 full verification，后续 DLL 的 completion 结论会继续依赖不稳固的底层。
- 如果 subject `.sln` 不可生成或不可编译，用户无法验收完整链路。
- 如果 codegen review 只检查文件存在，会无法发现生成代码中的 dispatch/helper/binding 问题。
- 如果 hotupdate proof 被静默跳过，会破坏“保留 HybridCLR 热更路径”的要求。

## 9. 备选路径

### 路径 A：继续沿用旧 layer summary 结果

- 不推荐。
- 原因：不能回答每个 DLL 是否完整翻译，也不能提供 codegen 审核。

### 路径 B：先做 Complex BCL 13 DLL，再回头补 CoreLib

- 不推荐。
- 原因：CoreLib 是底层 authority，未完整验证时后续结论可信度不足。

### 路径 C：先冻结 audit schema，再完整验证 CoreLib，再顺序执行 13 DLL

- 推荐。
- 原因：满足用户要求的可追踪、可视、可验收、可验证路径，也能避免后续文档和产物形态漂移。

## 10. 当前建议推进顺序

1. 创建 `20260419-02-audit-schema-and-foundation-solution-freeze`，冻结 audit schema 与 `FoundationDllTranslationSolution`。
2. 创建 `20260419-03-system-private-corelib-full-verification`，执行 CoreLib 方案 C。
3. 按顺序创建 `20260419-04` 到 `20260419-16`，逐个执行 Complex BCL 13 DLL。
4. CoreLib 与 13 DLL 完成后，再扩展 Phase 3/4/5 的 DLL scope。
