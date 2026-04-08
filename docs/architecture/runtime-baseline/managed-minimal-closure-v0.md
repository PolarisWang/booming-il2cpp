# Managed Minimal Closure v0

## 鐩爣

鏈枃浠跺畾涔?Runtime Baseline Stage 3 鐨?managed minimal closure 杈圭晫銆?
Stage 3 璐熻矗鎶婁竴涓湡瀹炵紪璇戝嚭鐨?managed assembly 鏀舵潫涓?generated-code input bundle锛屼负鍚庣画 native proof 鎻愪緵绋冲畾涓婃父锛涘畠涓嶈礋璐?native bootstrap銆乺eference runtime 鎴栨渶缁?`Hello, World!` 鐨勬墽琛岃瘉鏄庛€?

## 杈撳叆

- proof input assembly锛?  - `subjects/<subject-id>/source/bin/Release/net8.0/<subject-id>.dll`
  - `subjects/<subject-id>/source/<subject-id>.csproj`

Stage 3 鐨勫疄鐜板繀椤昏鍙栫湡瀹?assembly metadata 鍜?method body锛屼笉鍏佽鐩存帴澶嶅埗 Stage 2 鐨?canonical artifact JSON 浣滀负浼疄鐜拌緭鍑恒€?

## 杈撳嚭

Stage 3 鎺ㄨ崘杈撳嚭鏍圭洰褰曪細

- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/`

鏍圭洰褰曚笅鏈€灏忔枃浠堕泦锛?

- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/typed-il-ir.json`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/aot-manifest.json`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/metadata-registration.json`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/code-registration.json`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/closure.manifest.json`

## 杈圭晫

- Stage 3 鍙粨鏉熷湪 managed closure bundle銆?
- Stage 3 涓嶇敓鎴?native bootstrap 鎴?reference runtime proof銆?
- Stage 3 鍙互鍦?proof-bound 鑼冨洿鍐呭彧鏀寔褰撳墠鏈€灏?metadata / IL 瀛愰泦锛屼絾杩欎簺闄愬埗蹇呴』浣撶幇鍦ㄩ€氱敤 managed pipeline 鐨勨€滄敮鎸佺煩闃碘€濋噷锛岃€屼笉鏄妸 test project / sample 鍚嶇О纭紪鐮佽繘 `src/managed` 鐨勬牳蹇冭亴璐ｉ噷銆?

## 涓?Stage 2 鐨勫叧绯?

Stage 3 杈撳嚭蹇呴』鑳藉榻?Stage 2 宸插喕缁撶殑 canonical truth锛?

- entry subject: `HelloWorldObject/Program::Main(System.String[])`
- stdout truth: `Hello, World!`
- exit code truth: `0`
- generated direct-call landing path

鍥涗釜鏍稿績 artifact 浠嶄互 Stage 2 鍐荤粨鐨?schema / example / proof spec 浣滀负涓婃父濂戠害鏉ユ簮锛宍closure.manifest.json` 浠呮壙鎷?Stage 3 鑱氬悎鍏ュ彛鑱岃矗銆?

