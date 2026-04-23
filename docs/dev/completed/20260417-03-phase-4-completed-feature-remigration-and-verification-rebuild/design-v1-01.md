# Phase 4 Completed Feature Remigration And Verification Rebuild 璁捐 v1.01

Date: 2026-04-17 23:35:00 +08:00
Status: completed

## 鐩爣

- 浠?`subject.features.json` 涓?authority锛岄噸寤轰笁澶?canonical subject 鐨?completed feature 楠岃瘉闂幆銆?
- 鎶婃瘡鏉?feature 鐨?obligation 鏄庣‘缁戝畾鍒版寮忛獙璇佽祫浜э細
  - `unit/contracts`
  - `managed proof`
  - `native proof`
  - `hotupdate proof/smoke`
  - `benchmark`
- 鍒犻櫎浠嶇劧婕傛诞鍦ㄦ棫缁撴瀯銆佹棫鍛藉悕銆佹棫 correctness 璺緞涓婄殑閲嶅楠岃瘉涓庡け鏁堥獙璇併€?

## 杈撳叆鐪熸簮

- `subjects/SolutionCorePack/subject.features.json`
- `subjects/MixedExecutionFeaturePack/subject.features.json`
- `subjects/HotUpdateHostPack/subject.features.json`
- 褰撳墠 canonical subject 鐨?`Proofs / Benchmarks / EngineeringScenarios / Host / Patch / SharedContracts`
- 宸插瓨鍦ㄧ殑 `tests/unit/**`銆乣tests/contracts/**`銆乣tests/integration/**`

## 鑼冨洿杈圭晫

- 璐熻矗鎶娾€滃凡瀹屾垚鑳藉姏鈥濋噸鏂版寕鍒版柊 subject/test framework 涓荤嚎涓娿€?
- 涓嶈礋璐ｆ柊澧炴柊鐨?AOT Core IR feature銆?
- 涓嶈礋璐ｆ柊澧炴柊鐨?HotUpdate 璇箟鑳藉姏銆?
- 不把测试框架逻辑混进 IL2CPP core；审计与映射留在 subject/verification spine 侧。

## 鎵规绛栫暐

1. 鍏堝仛 authority 涓庣幇鏈夐獙璇佽祫浜х殑瀹¤鍩虹嚎锛屾壘鍑虹己鍙ｃ€侀噸澶嶉」鍜屾紓绉婚」銆?
2. 鍐嶆寜 subject 閫愪釜鏀跺彛锛?
   - `SolutionCorePack`
   - `MixedExecutionFeaturePack`
   - `HotUpdateHostPack`
3. 姣忎釜 subject 閮介伒寰細
   - 鍏堣ˉ RED 鐨?audit / contract 娴嬭瘯
   - 鍐嶈皟鏁?proof / benchmark / host 璧勪骇
   - 鏈€鍚庤窇 targeted regression
4. 鏁翠釜闃舵鏀跺熬鏃惰緭鍑哄彲杩涘叆 Phase 5 鐨?legacy purge 娓呭崟銆?

## 楠屾敹鍙ｅ緞

- 姣忔潯 `proofRequired = true` 鐨?feature 閮借兘杩藉埌姝ｅ紡 proof 璧勪骇銆?
- 姣忔潯 `benchmarkRequired = true` 鐨?feature 閮借兘杩藉埌姝ｅ紡 benchmark 璧勪骇銆?
- `supportStates` 瑙﹀強 hotupdate / mixed execution 鐨?feature锛屽繀椤昏拷鍒板搴?host 楠岃瘉璧勪骇銆?
- 涓嶅啀瀛樺湪鈥渇eature authority 宸插０鏄庯紝浣嗛獙璇佽祫浜т粛闈犳棫璺緞鎴栧巻鍙插吋瀹圭害瀹氬厹搴曗€濈殑鎯呭喌銆?
