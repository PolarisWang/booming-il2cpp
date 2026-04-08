# Native Reference Proof v0

## 鐩爣

鏈枃瀹氫箟 Runtime Baseline Stage 4 鐨勬渶灏?native reference proof 杈圭晫銆?
Stage 4 璐熻矗娑堣垂 Stage 3 宸插喕缁撶殑 managed closure bundle锛屾妸瀹?materialize 鎴?first generated C++ emission锛屽苟閫愭鎺ㄨ繘鍒?`Windows x64` reference host 涓婄殑鐪熷疄 native proof銆傚畠涓嶅啀鏀瑰啓 Stage 3 bundle 璇箟锛屼篃涓嶅湪绗竴杞苟琛岄摵寮€ `macOS` / `Linux`銆?

## 杈撳叆

Stage 4 褰撳墠鍞竴姝ｅ紡鐨?managed 涓婃父杈撳叆涓猴細

- `artifacts/proof/managed-closure/HelloWorldObject/typed-il-ir.json`
- `artifacts/proof/managed-closure/HelloWorldObject/aot-manifest.json`
- `artifacts/proof/managed-closure/HelloWorldObject/metadata-registration.json`
- `artifacts/proof/managed-closure/HelloWorldObject/code-registration.json`
- `artifacts/proof/managed-closure/HelloWorldObject/closure.manifest.json`

鍏朵腑 `closure.manifest.json` 缁х画浣滀负 Stage 4 娑堣垂鍏ュ彛鐨勮仛鍚堟竻鍗曪紝Stage 4 涓嶅簲缁曡繃瀹冮噸鏂板畾涔夊彟涓€濂?managed 杈撳叆鍗忚銆?

## 褰撳墠杈撳嚭

Stage 4 绗竴杞凡寤虹珛鐨?proof artifact 鏍圭洰褰曚负锛?

- `artifacts/proof/native-reference/HelloWorldObject/`

褰撳墠鏈€灏忚緭鍑洪泦鍚堬細

- `artifacts/proof/native-reference/HelloWorldObject/generated/native-reference.generated.cpp`
- `artifacts/proof/native-reference/HelloWorldObject/native-proof.manifest.json`

鍚庣画 native build / run 鎺ョ嚎绋冲畾鍚庯紝鍐嶇户缁ˉ锛?

- `artifacts/proof/native-reference/HelloWorldObject/run/`

## 褰撳墠 generated emission 杈圭晫

褰撳墠 `native-reference.generated.cpp` 鍙壙鎷?Stage 4 鐨?first emission skeleton 瑙掕壊锛?

- 浠?Stage 3 bundle 鎺ㄥ entry銆乧tor銆乮nstance method銆乫ield 涓?metadata token銆?
- 鏄庣‘浣跨敤 `register_codegen`銆?
- 鏄庣‘浣跨敤 `bootstrap_runtime`銆?
- 鏄庣‘淇濇寔 direct-call landing锛屼笉鍥為€€鍒?`method_invoke`銆?
- 鍦?generated path 鍐呮樉寮忚蛋 `field_set_value` / `field_get_value`銆乶arrow string concat 涓?`Console.WriteLine(string)` glue銆?

瀹冨綋鍓嶈繕涓嶇瓑浠蜂簬鏈€缁堝彲杩愯鐨勫畬鏁?generated method body 闆嗗悎銆傚悗缁?`runtime-core/bootstrap/support` 鎺ョ嚎瀹屾垚鍓嶏紝Stage 4 鍏佽鍏堟妸 generated emission 鍥哄畾鎴愬彲杩芥函銆佸彲娴嬭瘯銆佸彲鎸佺画鎵╁睍鐨?skeleton銆?

## Sample-Agnostic 杈圭晫

`src/**` 涓姝㈠啓鍏ユ祴璇曞伐绋嬬壒鍖栭€昏緫銆?

- 鐢熶骇浠ｇ爜鍙兘娑堣垂 Stage 3 bundle銆乧ontracts 鍜岄€氱敤 helper surface銆?
- `HelloWorldObject` 杩欑被鏍蜂緥淇℃伅鍙厑璁镐綔涓?canonical proof input / output truth 鍑虹幇鍦?`artifacts/proof/**`銆乣tests/proof/**` 涓庡搴旀枃妗ｄ腑銆?
- 濡傛灉鏌愭潯 Stage 4 閫昏緫鏃犳硶鑴辩娴嬭瘯宸ョ▼鍚嶈€屽瓨鍦紝鍒欒鏄庡畠杩樻病鏈夎姝ｇ‘鏀跺彛鍒?bundle-driven contract 杈圭晫銆?

## Proof Host 绾︽潫

- first proof host 鍥哄畾涓?`Windows x64`銆?
- 缁х画淇濈暀 `windows-x64-reference` 浣滀负 reference desktop 涓诲熀绾裤€?
- 涓嶅厑璁镐负浜嗘帴鍏?Stage 4 proof 鐩存帴鍒犻櫎鐜版湁 compile-only contract smoke 閿氱偣銆?

鎺ㄨ崘绛栫暐锛?

- 淇濈暀鐜版湁 reference preset 鐨?contract anchor銆?
- 鍦ㄥ悓涓€涓?preset 涓嬪閲忔帴鍏?Stage 4 proof target銆?
- 鍦?generated C++ 灏氭湭缁忚繃鐪熷疄閾炬帴楠岃瘉鍓嶏紝proof target 淇濇寔 `EXCLUDE_FROM_ALL`銆?

## 涓?Stage 2 / Stage 3 鐨勫叧绯?

Stage 4 浠嶅彈浠ヤ笅 canonical truth 绾︽潫锛?

- entry subject: `HelloWorldObject/Program::Main(System.String[])`
- stdout truth: `Hello, World!`
- exit code truth: `0`
- generated direct-call landing

Stage 4 鍙互鏂板 native 渚ф淳鐢熶骇鐗╋紝浣嗚繖浜涗骇鐗╁繀椤昏兘澶熷崟鍚戣拷婧洖 Stage 3 bundle锛岃€屼笉鏄噸鏂板彂鏄庡彟涓€濂?registration / metadata / subject 鏄犲皠鍗忚銆?

