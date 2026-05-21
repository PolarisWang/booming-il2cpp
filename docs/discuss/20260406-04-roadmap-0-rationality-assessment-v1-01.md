# roadmap-0 娴嬭瘯鍚堢悊鎬ц瘎浼?v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 缁撹

`roadmap-0` 浣滀负鍘嗗彶闃舵鐨勨€滅粺涓€楠岃瘉宸ヤ綔娴佲€濇槸鍚堢悊鐨勶紱  
浣嗗湪褰撳墠宸茬粡鏄庣‘閲囩敤 `subject + goal + environment-matrix + artifact-chain` 鐨勬柊娴嬭瘯鏋舵瀯涓嬶紝`roadmap-0` 宸茬粡涓嶅啀閫傚悎浣滀负姝ｅ紡鐨勪竴绛夋祴璇曞璞＄户缁繚鐣欍€?

鏇村噯纭湴璇达細

- 瀹冩浘缁忔槸鍚堢悊鐨勯樁娈垫€ч泦鎴愬３灞?
- 浣嗗畠鐜板湪涓嶅啀鏄悎鐞嗙殑闀挎湡鏋舵瀯涓績
- 瀹冨簲琚檷绾т负鈥滃巻鍙茶縼绉绘潵婧?/ 鑳藉姏鎷嗚В鏉ユ簮鈥?
- 涓嶅簲缁х画浣滀负鍏紑娴嬭瘯妯″瀷銆佹寮?registry 涓诲璞℃垨 CLI 鏍稿績璇箟

## 2. 涓轰粈涔堣瀹冩浘缁忓悎鐞?

鍦ㄦ棫妗嗘灦閲岋紝`roadmap-0` 瑙ｅ喅鐨勬槸鈥滃厛鎶婁竴鏉￠」鐩富绾挎墦閫氣€濈殑闂锛岃€屼笉鏄€滄妸娴嬭瘯璇箟寤烘ā姝ｇ‘鈥濈殑闂銆?

浠庣幇鏈夋枃妗ｅ拰鑴氭湰鐪嬶紝瀹冩妸杩欎簺涓滆タ涓€娆℃€т覆浜嗚捣鏉ワ細

- schema / contract 鍩虹鏍￠獙
- managed smoke 椤圭洰鏋勫缓涓庤繍琛?
- native ABI / bridge compile-only smoke
- Windows / macOS reference desktop trace compare
- Android / iOS / Linux 鐨?routing 鎴?packaging gate

瀵瑰簲璇佹嵁锛?

- `roadmap-0 runtime` 鏄庣‘鎶婂畠瀹氫箟涓衡€滈」鐩骇楠岃瘉閾捐矾鏀舵暃涓烘寮?system 瀵硅薄鈥濓細
  [roadmap-0-runtime.md](/E:/agent/booming-il2cpp/wiki/06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/%E6%95%B4%E4%BD%93%E9%AA%8C%E8%AF%81%E5%9C%BA%E6%99%AF/roadmap-0-runtime.md#L8)
- Windows / macOS 鐨?`scenario.manifest.json` 涔熺‘瀹炴槸涓€涓仛鍚堝鍣細
  [scenario.manifest.json](/E:/agent/booming-il2cpp/tests/registry/system/roadmap-0-windows/scenario.manifest.json#L8)
  [scenario.manifest.json](/E:/agent/booming-il2cpp/tests/registry/system/roadmap-0-macos/scenario.manifest.json#L8)
- `verify-roadmap-0.py` 鎶婁粠 schema 鍒?smoke銆佸啀鍒?reference gate 鍜屽钩鍙?gate 鐨勫姩浣滈兘缂栨帓鍦ㄤ竴鏀剼鏈噷锛?
  [verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L200)

濡傛灉椤圭洰褰撴椂鐨勭洰鏍囨槸鈥滃揩閫熸嫢鏈変竴涓兘璺戠殑涓荤嚎楠屾敹鍏ュ彛鈥濓紝杩欎釜璁捐鏄垚绔嬬殑銆?

## 3. 涓轰粈涔堝畠鐜板湪涓嶅啀鍚堢悊

### 3.1 涓昏酱閿欎簡

浣犵幇鍦ㄥ凡缁忕‘璁ゆ祴璇曚富杞村簲璇ユ槸锛?

- 娴嬭瘯鐩爣锛歚correctness.dev` / `correctness.platform` / `perf.dev` / `perf.release`
- 浜х墿涓荤嚎锛歚source -> host-input -> analysis -> generated -> build -> runtime -> report`

鑰?`roadmap-0` 鐨勪富杞村嵈鏄細

- 鎸夊涓诲钩鍙板垎鎴?`roadmap-0-windows` / `roadmap-0-macos`
- 鍐嶆妸涓嶅悓绫诲瀷楠岃瘉纭杩涗竴涓?workflow / system 鑱氬悎瀵硅薄

杩欐剰鍛崇潃瀹冨洖绛旂殑鏄€滃綋鍓嶅涓荤殑涓€鏉″巻鍙茶矾绾块€氭病閫氣€濓紝鑰屼笉鏄€滄煇涓?subject 鍦ㄦ煇涓洰鏍囥€佹煇涓?matrix 涓嬫槸鍚﹁揪鍒颁簡棰勬湡鈥濄€?

杩欏拰鏂版灦鏋勮瑙ｅ喅鐨勯棶棰樺凡缁忎笉鏄悓涓€涓棶棰樸€?

### 3.2 瀹冩妸澶氱楠岃瘉璇箟娣峰湪涓€璧?

`verify-roadmap-0.py` 鍚屾椂鍋氫簡锛?

- JSON parse
- analysis contract validate
- trace schema validate
- native compile-only smoke
- managed output smoke
- reference preset build
- trace compare
- Android / iOS / Linux routing gate

鍙锛?
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L204)
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L216)
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L222)
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L233)
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L246)
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L280)

杩欎簺鍔ㄤ綔鍦ㄦ柊妯″瀷閲岃嚦灏戜細钀藉埌涓嶅悓鐨勶細

- `goal`
- `validationMode`
- `adaptationLevel`
- `artifact bucket`

鏃?`roadmap-0` 鎶婂畠浠崋鎴愪竴涓€滈€氳繃 / 涓嶉€氳繃鈥濈殑澶у璞★紝浼氱洿鎺ュ墛寮卞け璐ュ畾浣嶈兘鍔涖€?

### 3.3 瀹冩妸鈥滃钩鍙伴€傞厤楠岃瘉鈥濅笌鈥滃畬鏁村钩鍙版纭€р€濇贩娣嗕簡

鐜版湁 gate 鏂囨。宸茬粡鏄庣‘鎵胯锛?

- Linux gate 楠岃瘉鐨勬槸鈥滅煩闃靛彲璋冨害銆佽矾鐢卞彲鏋勫缓銆乻hell 鍙惤鐩樷€濓紝涓嶆槸瀹屾暣杩愯鏃堕棴鐜?
- reference desktop gate 楠岃瘉鐨勬槸鈥滃弬鑰冩闈㈠熀绾夸粛鍙瀯寤哄苟涓?trace contract 鏈紓绉烩€濓紝涓嶆槸鍏ㄩ噺 workflow

瑙侊細
[roadmap-0-platform-gates.md](/E:/agent/booming-il2cpp/wiki/06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/%E6%95%B4%E4%BD%93%E9%AA%8C%E8%AF%81%E5%9C%BA%E6%99%AF/roadmap-0-platform-gates.md#L24)

杩欐伆濂借鏄?`roadmap-0` 閲屽緢澶氣€滃钩鍙伴獙璇佲€濆叾瀹炲彧鏄€傞厤鎬?/ buildability / routing capability 鐨?probe锛屼笉鏄钩鍙版纭€ф湰韬€?

鑰屼綘宸茬粡鏄庣‘瑕佹眰锛?

- 骞冲彴閫傞厤鎬у簲鍜屽钩鍙版纭€ф斁鍦ㄤ竴璧峰缓妯?
- 闇€瑕佸尯鍒嗕笉鍚屾垚鐔熷害涓庝笉鍚岄獙璇佹繁搴?

鍥犳鏃?`roadmap-0` 鐨勮〃杈剧矑搴﹀凡缁忎笉澶熴€?

### 3.4 瀹冩病鏈?subject 韬唤锛屽彧鏈夊伐浣滄祦韬唤

Windows 鑱氬悎瀵硅薄鍖呭惈锛?

- `system/runtime-baseline-windows`
- `system/android-startup-gate`
- `system/linux-packaging-gate`

瑙侊細
[scenario.manifest.json](/E:/agent/booming-il2cpp/tests/registry/system/roadmap-0-windows/scenario.manifest.json#L8)

macOS 鑱氬悎瀵硅薄鍖呭惈锛?

- `system/runtime-baseline-macos`
- `system/ios-packaging-gate`
- `system/linux-packaging-gate`

瑙侊細
[scenario.manifest.json](/E:/agent/booming-il2cpp/tests/registry/system/roadmap-0-macos/scenario.manifest.json#L8)

杩欒鏄庡畠鍏冲績鐨勬槸鈥滃綋鍓?host 鑳藉仛鍝簺浜嬧€濓紝鑰屼笉鏄€滃摢涓?subject 琚獙璇佷簡浠€涔堚€濄€?

浣嗕綘鐜板湪甯屾湜椤跺眰娴嬭瘯瀵硅薄鏄彲浠ユ寔缁墿灞曠殑澶ч噺璋冭瘯椤圭洰锛岃繖灏辫姹傛寮忔ā鍨嬪繀椤诲洿缁?`subjects/` 鏀舵暃锛岃€屼笉鏄洿缁曞巻鍙插伐浣滄祦鏀舵暃銆?

### 3.5 瀹冩妸鍏蜂綋鏍蜂緥鍜屼骇鐗╄矾寰勫啓姝诲湪搴曞眰鑴氭湰閲?

鑴氭湰鐩存帴纭紪鐮佷簡锛?

- `HelloWorld`
- `GenericEcho`
- `ReflectionLite`
- `PInvokeLite`
- `HostEmbeddingLite`
- `HelloWorldObject`
- `artifacts/proof/managed-closure/...`
- `artifacts/proof/native-reference/...`

瑙侊細
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L85)
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L222)

杩欏拰浣犲凡缁忕‘璁ょ殑鏂瑰悜鍐茬獊锛?

- `subjects/` 搴斾綔涓烘湭鏉ュぇ閲忔墿灞曡皟璇曢」鐩殑缁熶竴鍏ュ彛
- subject 搴斿彲閰嶇疆锛岃€屼笉鏄暎钀藉湪鑴氭湰甯搁噺閲?
- `managed-closure` 鍛藉悕鏈韩涔熷凡缁忚鍒ゅ畾涓轰笉鍚堢悊

### 3.6 瀹冪殑鎶ュ憡妯″瀷杩囩矖

`roadmap-0` 鏇村儚涓€娆″涓荤骇鈥滄€诲伐浣滄祦鎵ц鈥濓紝澶╃劧鏇撮€傚悎杈撳嚭鍗曟鑴氭湰鏃ュ織锛岃€屼笉鏄細

- `subject-report`
- `matrix-results[]`
- `stage-results[]`
- `artifact-results[]`

鍥犳瀹冧笉鍒╀簬浣犺姹傜殑涓ょ被缁撴灉锛?

- 寮€鍙戠幆澧冩纭€ч獙璇?
- 澶氬钩鍙?/ 鍙戝竷鎬ц兘鐨勫垎灞傛姤鍛婁笌瑕嗙洊杩借釜

## 4. 搴旇濡備綍瀹氭€?roadmap-0

寤鸿缁欏畠涓€涓槑纭殑鏂板畾浣嶏細

- 涓嶆槸姝ｅ紡娴嬭瘯瀵硅薄
- 涓嶆槸鏂?registry 鐨勫叕寮€瀵硅薄妯″瀷
- 涓嶆槸鏂?CLI 鐨勭洰鏍囧疄浣?
- 涓嶆槸闀挎湡鏂囨。鍏ュ彛

瀹冨簲璇ヨ瑙嗕负锛?

- 鍘嗗彶闃舵鐨勯泦鎴愯剼鏈泦鍚?
- 鏂版鏋惰縼绉绘椂鐨勮兘鍔涙竻鍗曟潵婧?
- 鐢ㄤ簬鎷嗚В鍑?`subject` / `matrix` / `goal` / `stage-check` 鐨勫弬鑰冨疄鐜?

## 5. roadmap-0 閲屽摢浜涗笢瑗垮€煎緱淇濈暀

涓嶆槸鏁翠釜 `roadmap-0` 淇濈暀锛岃€屾槸鎶婂叾涓湁鏁堣兘鍔涙媶鍑烘潵淇濈暀銆?

### 5.1 鍊煎緱淇濈暀鐨勮兘鍔?

- schema / contract 鏍￠獙鑳藉姏
- trace schema 涓?snapshot compare 鑳藉姏
- reference desktop trace gate 鑳藉姏
- Android / iOS / Linux 鐨?routing / packaging probe 鑳藉姏
- HostEmbeddingLite 鐩稿叧 trace export 鑳藉姏
- HelloWorldObject 鐨?native proof 杈撳叆涓?reference 杈撳嚭閾捐矾

### 5.2 涓嶅€煎緱淇濈暀鐨勫３灞?

- `workflow/roadmap-0-*` 杩欏眰鍏叡璇箟
- `system/roadmap-0-*` 浣滀负姝ｅ紡涓績瀵硅薄鐨勫湴浣?
- 鈥滀竴涓涓诲搴斾竴鏁存潯 roadmap 楠岃瘉娴佺▼鈥濈殑鍏ュ彛璁捐
- `verify-roadmap-0.*` 浣滀负娴嬭瘯妗嗘灦涓績鑴氭湰鐨勮鑹?

## 6. 鍦ㄦ柊鏋舵瀯涓嬫€庝箞鎷?

寤鸿鎸夆€渟ubject + matrix + goal鈥濇媶锛岃€屼笉鏄寜 鈥渞oadmap host workflow鈥?鎷嗐€?

### 6.1 绗竴浼樺厛绾?subject

- `subjects/HelloWorldObject`
  - 浣滀负 canonical proof subject
  - 瑕嗙洊 `source -> host-input -> analysis -> generated -> build -> runtime -> report`
- `subjects/HostEmbeddingLite`
  - 浣滀负 diagnostic subject
  - 瑕嗙洊 trace / host embedding / runtime 璇佹嵁
- `subjects/ReflectionLite`
- `subjects/PInvokeLite`
- `subjects/GenericEcho`

### 6.2 鎶婂師 roadmap-0 gate 鍙樻垚 matrix锛岃€屼笉鏄彉鎴?system

渚嬪 `HelloWorldObject` 鍙互澹版槑锛?

- `windows-dev-output`
- `windows-reference-trace`
- `macos-reference-trace`
- `windows-android-buildable`
- `macos-ios-buildable`
- `windows-linux-buildable`
- `macos-linux-buildable`

鐒跺悗閫氳繃锛?

- `validationMode`
- `adaptationLevel`
- `expectedOutcome`

鏉ヨ〃杈惧畠鍒板簳鏄湪楠岃瘉锛?

- compile-only
- build
- startup
- output
- trace
- perf

鑰屼笉鏄户缁敤 鈥渁ndroid-startup-gate / linux-packaging-gate / reference-gate鈥?杩欑被鍘嗗彶鍛藉悕鏉ユ壙杞戒富妯″瀷銆?

### 6.3 淇濈暀蹇呰鐨勪綆灞傚疄鐜帮紝浣嗗彧浣滀负 stage worker

`verify-roadmap-0.py` 閲屼笉灏戝姩浣滀粛鐒跺彲浠ュ鐢紝浣嗚鑹茶鍙樻垚锛?

- stage executor
- contract validator
- trace comparator
- preset probe helper

涓嶈兘缁х画鍏呭綋鈥滄暣涓祴璇曚綋绯荤殑鎬?orchestrator鈥濄€?

## 7. 鏈€缁堝垽鏂?

濡傛灉闂鏄細

鈥渀roadmap-0` 杩欎釜娴嬭瘯鍦ㄦ棫闃舵鏄惁鏈変环鍊硷紵鈥?

绛旀鏄細

- 鏈夛紝浣滀负鍘嗗彶涓荤嚎鎵撻€氶獙璇佹槸鍚堢悊鐨?

濡傛灉闂鏄細

鈥渀roadmap-0` 鍦ㄥ綋鍓嶆柊娴嬭瘯妗嗘灦閲屾槸鍚﹁繕搴旂户缁綔涓烘寮忔祴璇曞璞″瓨鍦紵鈥?

绛旀鏄細

- 涓嶅悎鐞?

鏇村悎閫傜殑澶勭悊鏂瑰紡鏄細

- 鎶?`roadmap-0` 闄嶇骇涓哄巻鍙茶縼绉绘潵婧?
- 鎶婇噷闈㈠彲澶嶇敤鐨勯獙璇佽兘鍔涙媶鍒版柊 `subject` 鏋舵瀯
- 鐢?`subject-report -> matrix-results -> stage-results -> artifact-results` 閲嶅缓鎶ュ憡闈?
- 涓嶅啀璁?`roadmap-0` 鍗犳嵁姝ｅ紡 CLI / registry / 鏂囨。鍏ュ彛

## 8. 瀵瑰綋鍓嶈璁＄鐨勫奖鍝?

杩欐潯鍒ゆ柇浼氬己鍖栦互涓嬭璁＄粨璁猴細

- 绗竴娉㈣縼绉诲彧淇濈暀 `HelloWorldObject` 鏄悎鐞嗙殑
- 鏃?`roadmap-0` 涓嶅仛瀵圭瓑杩佺Щ鏄悎鐞嗙殑
- 鏈潵鑻ラ渶瑕佹仮澶嶅叾涓煇浜涢獙璇侊紝蹇呴』浠ユ柊鐨?`subject` 褰㈠紡閲嶅缓
- 鏃х殑 platform gate 搴旇浆璇戞垚 matrix 鑳藉姏锛岃€屼笉鏄師鏍蜂繚鐣欎负涓绘ā鍨?
