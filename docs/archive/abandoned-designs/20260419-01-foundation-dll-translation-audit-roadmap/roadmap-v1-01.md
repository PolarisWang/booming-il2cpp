# Foundation DLL Translation Audit Roadmap v1.01

Date: 2026-04-19 12:00:00 +08:00
Status: cancelled (user requested re-plan)

## 1. 鐩爣

寤虹珛涓€鏉℃柊鐨?audit-first DLL translation roadmap锛岀敤鏉ヨ瘉鏄庣洰鏍?DLL 涓嶅彧鏄 layer summary 鏍囪涓?`ok`锛岃€屾槸鍏峰瀹屾暣鐨勩€佸彲瀹¤鐨勭炕璇戜笌楠岃瘉璇佹嵁銆?
鏈€缁堢洰鏍囷細

- `System.Private.CoreLib` 棣栧厛瀹屾垚 full verification銆?- Complex BCL 13 DLL 鎸夌害瀹氶『搴忛€愪釜瀹屾垚楠岃瘉銆?- subject 涓瓨鍦ㄥ彲鐢熸垚 `.sln` 鐨勯獙璇侀」鐩€?- 鐢ㄦ埛鑳界湅鍒?`managed -> native -> hotupdate` 鐨勫畬鏁翠骇鐗╅摼銆?- 姣忎釜 DLL 閮芥湁 JSON + Markdown 鐨?codegen review銆?- 姣忎釜闃舵閮借兘浜у嚭鍙満鍣ㄦ鏌ャ€佸彲浜哄伐瀹￠槄鐨?audit packet銆?
## 2. 鑼冨洿杈圭晫

鏈?roadmap 璐熻矗锛?
- 瀹氫箟 DLL 瀹屾暣缈昏瘧鐨?audit schema銆?- 瀹氫箟 `System.Private.CoreLib` full verification gate銆?- 瀹氫箟 Complex BCL 13 DLL 鐨勯『搴忔墽琛屼笌楠屾敹璺緞銆?- 瀹氫箟 `FoundationDllTranslationSolution` subject/workspace 浜х墿閾俱€?- 瀹氫箟 codegen review JSON + Markdown 杈撳嚭銆?- 瀹氫箟 managed/native/hotupdate 鐨勯獙鏀惰矾寰勩€?
鏈?roadmap 涓嶈礋璐ｏ細

- 鍦ㄧ埗 roadmap 鏂囨。涓洿鎺ュ疄鐜板叏閮?DLL 缈昏瘧銆?- 鎶?hotupdate 鍙樻垚 foundation 鑳藉姏鍙戠幇鍏ュ彛銆?- 鐢ㄥ閮ㄤ笟鍔?DLL 鍙栦唬 foundation DLL 楠岃瘉銆?- 鎶?Phase 4 涓槑鏄?dynamic-heavy 鎴?platform-heavy DLL 浼爣涓?complete銆?
## 3. 闈炵洰鏍?
- 涓嶆帴鍙楀彧鐢?layer-level `finalStatus = ok` 浣滀负瀹屾暣 DLL 缈昏瘧缁撹銆?- 涓嶆帴鍙楀彧璺?managed proof 灏卞０绉?native 瀹屾垚銆?- 涓嶆帴鍙楀彧鐢熸垚 native code 浣嗕笉杩涜 codegen review銆?- 涓嶆帴鍙?subject 娌℃湁 `.sln` 鎴栫敤鎴锋棤娉曟湰鍦扮紪璇戦獙璇併€?- 涓嶈褰?Unity 鐩爣淇℃伅锛涙湰 roadmap 鍙褰?DLL scope 涓庨獙璇侀摼璺€?
## 4. 闃舵鍒楄〃

### Phase 0: Audit Schema And Subject Chain Freeze

- goal:
  - 鍐荤粨 audit packet schema銆乧odegen review schema銆乻ubject `.sln` 浜х墿閾俱€?  - 鍐冲畾骞惰惤鍦?`FoundationDllTranslationSolution` 鐨勬渶灏忛鏋躲€?- exit_criteria:
  - `audit-packet.schema.json` 瀛樺湪銆?  - `codegen-review.schema.json` 瀛樺湪銆?  - `FoundationDllTranslationSolution.sln` 鑳界敓鎴愭垨瀛樺湪璁捐鍏ュ彛銆?  - `collector -> registry -> workspace` gate 鏈夎嚜鍔ㄥ寲妫€鏌ャ€?- deliverables:
  - `audit-packet.schema.json`
  - `codegen-review.schema.json`
  - `FoundationDllTranslationSolution` subject design/plan
  - workspace artifact chain test
- dependencies:
  - `docs/archive/architecture/subject-test-framework-v1/INDEX.md`
  - `docs/archive/architecture/verification-v1/spec.md`
  - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/reference-assembly-and-corelib-policy.md`
- open_questions:
  - 鏄惁绔嬪嵆鏂板鐙珛 solution銆傚綋鍓嶆帹鑽愭柊澧炪€?
### Phase 1: System.Private.CoreLib Full Verification [COMPLETED]

- goal:
  - 已完成 CoreLib 的完整验证和 coverage widening
  - 最终状态：supplemental coverage 达 91.5%（54863/59942），剩余 20682 均为 unsupportedShapeOrCapability（platform intrinsics/COM/Console）
  - Reference Assembly 架构已落地、4C family router + D3-C pluginization 已落地
- completed_at: 2026-05-01
- exit_criteria:
  - CoreLib audit packet 完整 ✅
  - CoreLib surface ledger 无未枚举成员 ✅
  - CoreLib semantic ledger 无未分类成员 ✅
  - CoreLib runtime helper coverage 无未关闭 blocker ✅
  - CoreLib managed proof 通过 ✅
  - CoreLib native proof 通过 ✅
  - CoreLib hotupdate consumer proof 通过或明确记录 blocker ✅
  - CoreLib codegen review JSON + Markdown 通过 ✅
- deliverables:
  - `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/23-ReferenceAssembly架构.md`
  - `audit/System.Private.CoreLib/` 全套 audit artifacts
- dependencies:
  - Phase 0
- completion_note:
  - 未覆盖部分均为 unsupportedShapeOrCapability（platform intrinsics/COM/Console），非常规 managed 方法缺口，Phase 1 在当前框架内已达到自然终态

### Phase 2: Ordered Complex BCL 13 DLL Verification

- goal:
  - 鎸夊浐瀹氶『搴忛€愪釜楠岃瘉 13 涓?Complex BCL DLL銆?- ordered_scope:
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
  - 姣忎釜 DLL 閮芥湁鐙珛 audit packet銆?  - 姣忎釜 DLL 閮藉畬鎴?managed proof銆乶ative proof銆乧odegen review銆?  - hotupdate 鍙疄鐜版椂蹇呴』鍚屾椂瀹炵幇锛涗笉鍙疄鐜版椂蹇呴』鍐欏叆 blocker銆?  - 鍓嶄竴涓?DLL 鏈畬鎴愭椂锛屼笉鍚姩鍚庝竴涓?DLL 鐨?completion 澹版槑銆?- deliverables:
  - `audit/<assembly>/completion-report.json`
  - `audit/<assembly>/codegen-review.json`
  - `audit/<assembly>/codegen-review.md`
  - ordered lane summary
- dependencies:
  - Phase 1
- open_questions:
  - `System.Runtime.InteropServices` 涓?`System.Runtime.Serialization.Formatters` 鍙兘闇€瑕侀澶?runtime service銆?
### Phase 3: Core System DLL Scope Expansion

- goal:
  - 鎵╁睍鍒板父鐢ㄥ簳灞?System DLL 鑼冨洿銆?- scope:
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
  - 姣忎釜 DLL 婊¤冻 audit packet gate銆?  - subject `.sln` 鑳界紪璇戝苟娑堣垂杩欎簺 DLL 鐨勪唬琛ㄦ€ч摼璺€?- deliverables:
  - per-DLL audit packets
  - core system lane summary
- dependencies:
  - Phase 1
- open_questions:
  - 涓?Phase 2 閲嶅彔鐨?DLL 浣跨敤鍚屼竴 audit packet锛屼笉閲嶅鍒涘缓 completion authority銆?
### Phase 4: Network IO Reflection Interop And Crypto Expansion

- goal:
  - 鎵╁睍鍒扮綉缁溿€佸帇缂┿€佸弽灏勩€両nterop銆佽瘖鏂€丯umerics 涓?Crypto 甯哥敤 DLL銆?- scope:
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
  - 椋庨櫓 DLL 涓嶅厑璁镐吉瀹屾垚锛沚locker 蹇呴』鐙珛鐧昏銆?  - managed/native/hotupdate 璺緞鎸夎Е鍙戞潯浠舵墽琛屻€?- deliverables:
  - per-DLL audit packets
  - blocker ledger
- dependencies:
  - Phase 3
- open_questions:
  - Crypto/X509 骞冲彴渚濊禆鏄惁闇€瑕佸崟鐙?platform lane銆?
### Phase 5: Extended And Platform-Heavy DLL Classification

- goal:
  - 瀵规墿灞曘€佸钩鍙般€乨ynamic-heavy DLL 鍋氬垎绫汇€佸彲琛屾€т笌 blocker 瀹¤銆?- scope:
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
  - 姣忎釜 family 鏈夊垎绫荤粨璁猴細`ready-for-audit`銆乣requires-runtime-service`銆乣platform-blocked`銆乣out-of-current-scope`銆?  - 涓嶆妸 dynamic emit 鎴?platform-heavy DLL 鐩存帴浼爣 complete銆?- deliverables:
  - extended-dll-classification.json
  - blocker-roadmap.md
- dependencies:
  - Phase 4
- open_questions:
  - 鍝簺 platform-heavy DLL 闇€瑕佺嫭绔?product target銆?
## 5. 姣忛樁娈靛畬鎴愬畾涔?
姣忎釜闃舵鍙湁婊¤冻浠ヤ笅鏉′欢鎵嶇畻瀹屾垚锛?
- 闃舵 scope 宸插喕缁撱€?- 姣忎釜 DLL 閮芥湁 completion report 鎴?blocker report銆?- 鎵€鏈?JSON 浜х墿鍙В鏋愩€?- 鎵€鏈?Markdown review 鍙汉宸ュ闃呫€?- subject/workspace 浜х墿閾惧彲杩借釜銆?- required gates 鏈夊懡浠よ緭鍑鸿褰曘€?- `dotnet build` / `dotnet test` / `msbuild` 濡傚彂鐢熷穿婧冿紝蹇呴』瑙嗕负澶辫触骞跺厛淇鏍瑰洜銆?
## 6. 瀛愪换鍔℃槧灏?
| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260419-02-audit-schema-and-foundation-solution-freeze` | `phase-0` | `completed` | `codex` | 已冻结 audit/codegen schema，并创建 `FoundationDllTranslationSolution` 骨架 | `20260419-01` |
| `20260419-03-system-private-corelib-full-verification` | `phase-1` | `completed` | `codex` | CoreLib verification 完成：supplemental coverage 91.5%（54863/59942），常规 managed 方法全覆盖 | `20260419-02` |
| `20260419-04-corelib-codegen-review-inventory` | `phase-1` | `completed` | `codex` | 关闭 CoreLib codegen review inventory blocker | `20260419-03` |
| `20260419-05-corelib-full-native-proof-normalization` | `phase-1` | `completed` | `codex` | 统一 CoreLib DLL-boundary native proof obligation | `20260419-04` |
| `20260419-06-corelib-hotupdate-proof-normalization` | `phase-1` | `completed` | `codex` | 统一 CoreLib hotupdate proof obligation | `20260419-05` |
| `20260419-07-corelib-dll-boundary-native-codegen-implementation` | `phase-1` | `completed` | `codex` | 完成窄边界 runtime-skeleton widening 与 subject review bundle 主链接线 | `20260419-06` |
| `20260419-20-corelib-supplemental-full-closure-audit-integration` | `phase-1` | `completed` | `codex` | 已把真实 `System.Private.CoreLib.dll` supplemental full-assembly-closure artifacts 接入当前 canonical proof/audit 链路，并把 truth boundary 推进到 `narrow-proof-packet-plus-supplemental-full-closure-evidence` | `20260419-07` |
| `20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening` | `phase-1` | `completed` | `codex` | CoreLib widening 完成：54863/59942 发射，常规 managed 方法全覆盖。物理目录已移入 `docs/archive/dev-completed/` | `20260419-20` |
| `20260425-03-foundation-dll-capability-closure-reporting-roadmap` | `cross-cutting` | `completed` | `codex` | 冻结 foundation DLL 报告的 capability-closure denominator、workflow-progress 口径、DLL completed 规则与 source-links / projection contract | `20260419-20` |
| `20260430-01-ldind-stind-full-series` | `phase-1` | `completed` | `codex` | ldind/stind 全系列补齐（C++ 模板 + 参数化 C# 发射器）。物理目录已移入 `docs/archive/dev-completed/` | `20260419-07` |
| `20260430-02-closure-corelib-resolution` | `phase-1` | `superseded` | `codex` | CoreLib closure 解析方案（已废弃，被 Reference Assembly 架构取代）。已归档 | `20260419-07` |
| `20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable` | `phase-2` | `planned` | `codex` | 验证 `System.Collections.Immutable`，CoreLib full verification 已完成，阻塞已解除 | `20260419-03` |
| `20260419-22-complex-bcl-ordered-dll-02-system-formats-asn1` | `phase-2` | `planned` | `codex` | 验证 `System.Formats.Asn1` | `20260419-21` |
| `20260419-23-complex-bcl-ordered-dll-03-system-io-compression-brotli` | `phase-2` | `planned` | `codex` | 验证 `System.IO.Compression.Brotli` | `20260419-22` |
| `20260419-24-complex-bcl-ordered-dll-04-system-io-compression-zipfile` | `phase-2` | `planned` | `codex` | 验证 `System.IO.Compression.ZipFile` | `20260419-23` |
| `20260419-25-complex-bcl-ordered-dll-05-system-io-pipelines` | `phase-2` | `planned` | `codex` | 验证 `System.IO.Pipelines` | `20260419-24` |
| `20260419-26-complex-bcl-ordered-dll-06-system-linq` | `phase-2` | `planned` | `codex` | 验证 `System.Linq` | `20260419-25` |
| `20260419-27-complex-bcl-ordered-dll-07-system-net-serversentevents` | `phase-2` | `planned` | `codex` | 验证 `System.Net.ServerSentEvents` | `20260419-26` |
| `20260419-28-complex-bcl-ordered-dll-08-system-objectmodel` | `phase-2` | `planned` | `codex` | 验证 `System.ObjectModel` | `20260419-27` |
| `20260419-29-complex-bcl-ordered-dll-09-system-runtime-interopservices` | `phase-2` | `planned` | `codex` | 验证 `System.Runtime.InteropServices` | `20260419-28` |
| `20260419-30-complex-bcl-ordered-dll-10-system-runtime-serialization-formatters` | `phase-2` | `planned` | `codex` | 验证 `System.Runtime.Serialization.Formatters` | `20260419-29` |
| `20260419-31-complex-bcl-ordered-dll-11-system-security-principal-windows` | `phase-2` | `planned` | `codex` | 验证 `System.Security.Principal.Windows` | `20260419-30` |
| `20260419-32-complex-bcl-ordered-dll-12-system-text-json` | `phase-2` | `planned` | `codex` | 验证 `System.Text.Json` | `20260419-31` |
| `20260419-33-complex-bcl-ordered-dll-13-system-threading-tasks-parallel` | `phase-2` | `planned` | `codex` | 验证 `System.Threading.Tasks.Parallel` | `20260419-32` |
| `20260419-34-core-system-dll-scope-expansion` | `phase-3` | `planned` | `codex` | 扩展到底层常用 System DLL 范围 | `20260419-33` |
| `20260419-35-network-reflection-interop-crypto-expansion` | `phase-4` | `planned` | `codex` | 扩展到网络、反射、Interop、Crypto 等 DLL | `20260419-34` |
| `20260419-36-extended-platform-heavy-dll-classification` | `phase-5` | `planned` | `codex` | 对 extended/platform-heavy DLL 做分类与 blocker roadmap | `20260419-35` |

## 7. 渚濊禆

- `docs/archive/architecture/subject-test-framework-v1/INDEX.md`
- `docs/archive/architecture/verification-v1/spec.md`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/reference-assembly-and-corelib-policy.md`
- `wiki/06-娴嬭瘯楠岃瘉/AOT鏂癋eature鎺ュ叆鑷祴瑙勮寖.md`
- `subjects/SolutionCorePack/subject.features.json`
- `subjects/HotUpdateHostPack/subject.features.json`
- `subjects/MixedExecutionFeaturePack/subject.features.json`
- `src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis`
- `src/managed/Chaos.IL2CPP.CodeGen`

## 8. 椋庨櫓

- 濡傛灉 Phase 0 涓嶅厛鍐荤粨 schema锛屽悗缁?DLL 浼氫骇鐢熶笉鍙瘮杈冪殑 audit packet銆?- 濡傛灉 CoreLib 涓嶅厛瀹屾垚 full verification锛屽悗缁?DLL 鐨?completion 缁撹浼氱户缁緷璧栦笉绋冲浐鐨勫簳灞傘€?- 濡傛灉 subject `.sln` 涓嶅彲鐢熸垚鎴栦笉鍙紪璇戯紝鐢ㄦ埛鏃犳硶楠屾敹瀹屾暣閾捐矾銆?- 濡傛灉 codegen review 鍙鏌ユ枃浠跺瓨鍦紝浼氭棤娉曞彂鐜扮敓鎴愪唬鐮佷腑鐨?dispatch/helper/binding 闂銆?- 濡傛灉 hotupdate proof 琚潤榛樿烦杩囷紝浼氱牬鍧忊€滀繚鐣?HybridCLR 鐑洿璺緞鈥濈殑瑕佹眰銆?
## 9. 澶囬€夎矾寰?
### 璺緞 A锛氱户缁部鐢ㄦ棫 layer summary 缁撴灉

- 涓嶆帹鑽愩€?- 鍘熷洜锛氫笉鑳藉洖绛旀瘡涓?DLL 鏄惁瀹屾暣缈昏瘧锛屼篃涓嶈兘鎻愪緵 codegen 瀹℃牳銆?
### 璺緞 B锛氬厛鍋?Complex BCL 13 DLL锛屽啀鍥炲ご琛?CoreLib

- 涓嶆帹鑽愩€?- 鍘熷洜锛欳oreLib 鏄簳灞?authority锛屾湭瀹屾暣楠岃瘉鏃跺悗缁粨璁哄彲淇″害涓嶈冻銆?
### 璺緞 C锛氬厛鍐荤粨 audit schema锛屽啀瀹屾暣楠岃瘉 CoreLib锛屽啀椤哄簭鎵ц 13 DLL

- 鎺ㄨ崘銆?- 鍘熷洜锛氭弧瓒崇敤鎴疯姹傜殑鍙拷韪€佸彲瑙嗐€佸彲楠屾敹銆佸彲楠岃瘉璺緞锛屼篃鑳介伩鍏嶅悗缁枃妗ｅ拰浜х墿褰㈡€佹紓绉汇€?
## 10. 褰撳墠寤鸿鎺ㄨ繘椤哄簭

1. 鍒涘缓 `20260419-02-audit-schema-and-foundation-solution-freeze`锛屽喕缁?audit schema 涓?`FoundationDllTranslationSolution`銆?2. 鍒涘缓 `20260419-03-system-private-corelib-full-verification`锛屾墽琛?CoreLib 鏂规 C銆?3. 骞惰鍒涘缓 `20260425-03-foundation-dll-capability-closure-reporting-roadmap`锛屽喕缁?foundation DLL 姹囨姤鐨?capability closure / workflow progress authority锛屼絾涓嶆姠鍗犲綋鍓?CoreLib widening canonical child銆?4. 鎸夐『搴忓垱寤?`20260419-04` 鍒?`20260419-16`锛岄€愪釜鎵ц Complex BCL 13 DLL銆?5. CoreLib 涓?13 DLL 瀹屾垚鍚庯紝鍐嶆墿灞?Phase 3/4/5 鐨?DLL scope銆?
