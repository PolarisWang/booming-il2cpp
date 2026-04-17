# 04-宸ュ叿涓庨泦鎴?INDEX

## 鑼冨洿璇存槑
- `2026-04-17`: ?? `il2cpp-core-structure-and-scriban-governance.md`??? `ObjectModel` / `ExternalRuntimeHelpers` ?????`LoaderStage` / `LinkerStage` / `ManagedClosureContracts` ?????????????

- 璁板綍椤圭洰绾у伐鍏峰叆鍙ｃ€侀泦鎴愯竟鐣屻€乸roof baseline 涓庨暱鏈熺ǔ瀹氱殑鎿嶄綔绾︽潫銆?- 涓嶈褰曞崟娆′换鍔℃祦姘淬€佷复鏃惰瘖鏂垨浠呭鏌愭鎵ц鏈夋晥鐨勪笂涓嬫枃銆?- 鎵ц鏃ュ織淇濈暀鍦?`docs/dev/<lifecycle>/<task_id>/`锛涙矇娣€鍚庣殑闀挎湡鐭ヨ瘑鍐嶈繘鍏ユ湰鐩綍銆?
## 瀛愬垎绫?
| 鍒嗙被 | 璇存槑 | 绱㈠紩 |
| --- | --- | --- |
| `-` | 褰撳墠鐩綍鐩存帴缁存姢宸ュ叿涓庨泦鎴愪富棰橀〉 | `-` |

## 鍏抽敭椤甸潰

| 椤甸潰 | 涓婚 | 鎽樿 |
| --- | --- | --- |
| [`hot-update-interpreter-vm-v1.md`](./hot-update-interpreter-vm-v1.md) | hot update interpreter vm | 鍥哄寲鍘熺敓 `InterpreterVM` 鐨?`InterpreterValue`銆?4 鏉?opcode catalog銆乶ative smoke 杈圭晫锛屼互鍙婁笌 managed interpreter 鐨勮亴璐ｅ垎灞傘€?|
| [`hot-update-e2e-runtime-baseline-v1.md`](./hot-update-e2e-runtime-baseline-v1.md) | hot update e2e runtime baseline | 鍥哄寲 Phase C 鐨?package load銆佹柟娉曟浛鎹€乤uto bridge銆佸洖婊氥€乥enchmark CLI 涓庨獙璇佸叆鍙ｃ€?|
| [`interpreter-lowering-and-native-smoke-baseline-v1.md`](./interpreter-lowering-and-native-smoke-baseline-v1.md) | interpreter lowering銆乵ixed execution 涓?native smoke baseline | 鍥哄寲 Phase 7 鐨勭湡瀹?lowering proof銆乻ame-assembly `Call` / `CallVirt` 鎵ц銆佺湡瀹?EH lowering銆乣try/catch` / `rethrow` / `try/finally` 鏈€灏?managed proof銆佸瓧绗︿覆妗ユ墽琛?proof銆乶ative `InterpreterVM` smoke proof 涓?Windows 鏂囦欢閿佽閬胯鍒欍€?|
| [`metadata-supplement-bridge-baseline-v1.md`](./metadata-supplement-bridge-baseline-v1.md) | metadata supplement 涓?bridge baseline | 鍥哄寲 Phase 6 鐨?supplemental metadata template銆乥ridge baseline銆乸roof 涓庨」鐩骇楠岃瘉鍏ュ彛銆?|
| [`hot-update-skeleton-v1.md`](./hot-update-skeleton-v1.md) | hot update skeleton | 鍥哄寲 `Chaos.IL2CPP.HotUpdate` 鐨勯」鐩竟鐣屻€乸ackage 褰㈢姸銆乺untime mode 璇箟涓?`HotUpdateSkeletonProof`銆?|
| [`engine-host-proof-baseline-v1.md`](./engine-host-proof-baseline-v1.md) | engine host proof baseline | 鍥哄寲 `EngineHostProof` 鐨?host proof build銆乴ifecycle stress 涓?`HostEmbeddingLite` ownership 鍩虹嚎銆?|
| [`mobile-host-subject-routing-v1.md`](./mobile-host-subject-routing-v1.md) | mobile host subject routing | 璇存槑 `MobileHelloWorldProof` 鐨?subject-scoped Android/iOS host root銆乺oot CMake cache 鍙橀噺涓?mobile route fallback 瑙勫垯銆?|
| [`native-perf-and-convert-baselines-v1.md`](./native-perf-and-convert-baselines-v1.md) | native perf 涓?convert baseline | 鍥哄寲 `native-runtime-perf` payload銆乣tests/perf/*` 绾︽潫涓?convert perf baseline銆?|
| [`engine-binding-stub-baseline-v1.md`](./engine-binding-stub-baseline-v1.md) | engine binding stub baseline | 鍥哄寲 `Chaos.IL2CPP.EngineBinding`銆乣[EngineExport]` / `[EngineCallback]` 鏍囨敞鍏ュ彛涓?smoke / compile-only baseline銆?|
| [`il2cpp-core-structure-and-scriban-governance.md`](./il2cpp-core-structure-and-scriban-governance.md) | IL2CPP core structure and Scriban governance | ?? `IL2CPP` ??????????????planner/emitter/template ???ObjectModel / ExternalRuntimeHelpers / Loader / Linker / Contracts ??????????????? |
| [`project-graph-ingestion-v1.md`](./project-graph-ingestion-v1.md) | project graph ingestion | 鍥哄寲 `Chaos.IL2CPP.ProjectGraph`銆乣project-graph.json` 涓?golden convert 鎺ョ嚎銆?|
| [`鍏变韩-contract-v0.md`](./鍏变韩-contract-v0.md) | 鍏变韩 contract v0 | 鍥哄寲 `contracts/shared/v0/` 鐨勯暱鏈熺害鏉熴€侀獙璇佸叆鍙ｄ笌浣跨敤杈圭晫銆?|
| [`缁熶竴鍏ュ彛鑴氭湰.md`](./缁熶竴鍏ュ彛鑴氭湰.md) | 缁熶竴鍏ュ彛鑴氭湰 `run.*` | 鍥哄寲 bootstrap銆乀UI銆乸repare/clean銆乣--json` 涓?harness 鎺ョ嚎瑙勫垯銆?|
| [`缁熶竴娴嬭瘯妗嗘灦.md`](./缁熶竴娴嬭瘯妗嗘灦.md) | 缁熶竴娴嬭瘯妗嗘灦 `run test` | 鍥哄寲 suite manifest銆乻ummary/final-summary 涓庢祴璇曞叆鍙ｇ害鏉熴€?|
| [`unified-project-workspaces-and-deploy-core.md`](./unified-project-workspaces-and-deploy-core.md) | project workspaces 涓?deploy core | 鍥哄寲 `generate/build project`銆乣deploy core`銆乣subject-exec` 涓?`artifacts/projects/**` / `deploy/core/**` 杈圭晫銆?|
| [`Claude鎶€鑳芥祴璇曞叆鍙?md`](./Claude鎶€鑳芥祴璇曞叆鍙?md) | Claude 鎶€鑳芥祴璇曞叆鍙?| 鍥哄寲 `.codex/tests/claude-code/` 鐨勫叆鍙ｄ笌绾︽潫銆?|

## 缁存姢绾﹀畾

- 椤甸潰蹇呴』鎬荤粨鍙鐢ㄧ粨璁猴紝鑰屼笉鏄鍒朵换鍔¤繃绋嬨€?- 鏂板椤甸潰鏃跺繀椤诲悓姝ユ洿鏂版湰鐩綍 `INDEX.md`锛涘褰卞搷涓婂眰瀵艰埅锛屽啀鏇存柊 `wiki/INDEX.md`銆?
## 鏈€杩戝彉鏇?
| [`il2cpp-core-structure-and-scriban-governance.md`](./il2cpp-core-structure-and-scriban-governance.md) | IL2CPP core structure and Scriban governance | ?? `IL2CPP` ??????????????planner/emitter/template ???ObjectModel / ExternalRuntimeHelpers / Loader / Linker / Contracts ??????????????? |
