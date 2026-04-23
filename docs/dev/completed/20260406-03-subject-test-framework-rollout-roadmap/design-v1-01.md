# Subject 娴嬭瘯妗嗘灦閲嶆瀯钀藉湴璁捐 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 璁捐鐩爣

鎶?`docs/discuss/` 涓凡缁忕‘瀹氱殑娴嬭瘯妗嗘灦璁捐锛屾敹鏁涙垚涓€涓彲浠ュ疄鏂界殑钀藉湴璁捐锛?

- 浠?`subject` 涓哄敮涓€姝ｅ紡娴嬭瘯瀵硅薄
- 浠?`goal + environment-matrix + artifact-chain` 涓烘墽琛岃涔変腑杞?
- 浠ュ唴閮ㄥ垎灞傚厛绋冲畾銆乸ublic surface 鍚庣疆涓哄疄鏂藉師鍒?

## 2. 璁捐杈撳叆

鏈璁′互 `docs/discuss/test-framework/`銆乣docs/discuss/subject-framework/`銆乣docs/discuss/reporting/` 涓?`docs/discuss/roadmap-0/` 涓嬬殑杩炵画璁ㄨ涓鸿緭鍏ワ紝涓嶅彟琛屽湪 `docs/discuss/` 鐢熸垚鏂扮殑鍚屾鍓湰銆?
鍏朵腑鏈€鍏抽敭鐨勫喕缁撶偣宸茬粡鍖呮嫭锛?

- `subject / goal / environment-matrix / artifact-chain`
- `shared/source/host-input/analysis/generated`
- `matrices/<matrix-id>/build/runtime/report`
- `planner output`
- `executor / stage worker` 杈圭晫
- `matrix report / subject summary / session summary`
- `matrix / subject / session events`

## 3. 鎬讳綋璁捐閫夋嫨

鎬讳綋閲囩敤锛?

- **internal-first subject spine**

涔熷氨鏄細

1. 鍏堣惤 `subjects/` 涓?artifact bucket
2. 鍏堣惤 planner / executor / worker / reporting / events 鐨勬柊鑴婃煴
3. 鍐嶈 `HelloWorldObject` 鐨勯鎵?Windows matrix 鍦ㄦ柊鑴婃煴涓婄湡瀹炶窇閫?
4. 鏈€鍚庢墠鍒?public CLI / registry / TUI

## 4. 姝ｅ紡涓酱

## 4.1 object model

姝ｅ紡涓酱鍥哄畾涓猴細

- `subjects/<subject-id>/subject.manifest.json`

鍏朵腑 matrix 鐨勯€夋嫨璇箟鏉ヨ嚜锛?

- `supportedGoals`
- `environmentMatrices`
- `pipelineId`
- `executionPipelines`
- `artifactPlan`

## 4.2 artifact model

姝ｅ紡 bucket 鍥哄畾涓猴細

- `source`
- `host-input`
- `analysis`
- `generated`
- `build`
- `runtime`
- `report`

钀界偣鍥哄畾涓猴細

- `artifacts/subjects/<subject-id>/shared/...`
- `artifacts/subjects/<subject-id>/matrices/<matrix-id>/...`
- `artifacts/subjects/<subject-id>/subject-report/...`

## 4.3 execution model

鎵ц璇箟鍥哄畾涓猴細

- planner
  - 閫夋嫨 goal / matrix / pipeline
  - 灞曞紑 stage plan
  - 鍐冲畾 `reused / executed / invalidated`
- executor
  - 璋冨害 stage worker
  - 鍐?events
  - 鍏戠幇 reuse / invalidation
  - 浜у嚭 `matrix execution result`
- reporting
  - 鎶?`matrix execution result` 鏀舵垚 `report.json`
  - 鑱氬悎鎴?subject/session summary

## 5. 棣栨壒姝ｅ紡瀹炴柦杈圭晫

## 5.1 鍖呭惈

棣栨壒瀹炴柦寤鸿鍙寘鍚細

- `HelloWorldObject`
- Windows 棣栨壒 matrix锛?
  - `windows-dev-output`
  - `windows-reference-trace`
  - `windows-android-buildable`
  - `windows-linux-buildable`
- `correctness.dev`
- `correctness.platform`

杩欓噷鐨?`android/linux` 鍙寚锛?

- buildable correctness

涓嶆寚鐪熷疄杩愯鏃堕獙璇併€?

## 5.2 鏆備笉鍖呭惈

棣栨壒瀹炴柦鏄庣‘鍚庣疆锛?

- `perf.dev`
- `perf.release`
- macOS 棣栨壒 subject cutover
- 澶?subject 骞惰鎵╁睍
- public CLI / public registry / TUI 瀹屾暣鏀归€?

## 6. 褰撳墠浠ｇ爜鍒扮洰鏍囪璁＄殑涓昏鏂偣

## 6.1 娴嬭瘯瀵硅薄鏂偣

褰撳墠锛?

- `tests/proof/hello-world-object/suite.manifest.json`

鐩爣锛?

- `subjects/HelloWorldObject/subject.manifest.json`

鎵€浠ワ細

- 闇€瑕佷粠 `suite` 鐪熸簮鍒囧埌 `subject` 鐪熸簮

## 6.2 鎵ц鍣ㄦ柇鐐?

褰撳墠锛?

- `registry.py` 展开的是旧一代 registry formal objects
- `test.py` 浜у嚭鐨勬槸 `suiteResults`

鐩爣锛?

- `planner output -> executor -> matrix execution result`

鎵€浠ワ細

- 鏃?registry/session 涓嶅彲鑳藉钩婊戝彉鎴愭柊鑴婃煴
- 涓棿闇€瑕佹ˉ鎺ュ眰锛岃€屼笉鏄瓧娈电‖鏀?

## 6.3 濂戠害婧愭柇鐐?

褰撳墠锛?

- `contracts.py` 浠嶈 `analysis/contracts`

鐩爣锛?

- `contracts/` 鏄?canonical source
- `tests/contracts/` 鏄?baseline / harness

鎵€浠ワ細

- contract source cutover 蹇呴』鏃╀簬 worker 鎶藉彇

## 6.4 闅愬紡 orchestration 鏂偣

褰撳墠锛?

- Stage 4 CMake 涓?`verify-roadmap-0.py` 浠嶉殣寮忎覆璧峰闃舵

鐩爣锛?

- `host-input-build`
- `analysis-frontend`
- `generated-native-proof`
- `build-target`
- `runtime-observe`
- `runtime-trace-compare`

閮芥垚涓烘樉寮?stage

鎵€浠ワ細

- 杩欐槸棣栨壒鐪熸瑕佹媶鐨勨€滄殫绠卞眰鈥?

## 7. 鎺ㄨ崘鐨勫疄鏂芥灦鏋?

## 7.1 subject filesystem 鍏堣惤鍦?

绗竴闃舵灏卞簲姝ｅ紡鍒涘缓锛?

```text
subjects/
  HelloWorldObject/
    subject.manifest.json
    source/
```

鍚屾椂锛?

- 鏃?`tests/proof/input/HelloWorldObject`
  - 閫€鍖栦负鍏煎澹虫垨杩佺Щ鏉ユ簮

## 7.2 worker 鍙洿缁?bucket

worker 涓嶅啀闈㈠悜锛?

- suite
- pipeline
- monolithic workflow

鑰屽彧闈㈠悜锛?

- 鍗?stage
- 鍗?bucket
- 鍗?manifest 杈撳叆杈撳嚭

## 7.3 report 涓?event 鍙湇鍔′笂灞?

鏅€?worker 涓嶇洿鎺ュ啓锛?

- `report.json`
- `subject summary`
- `session summary`
- `events.jsonl`

瀹冧滑鍙啓锛?

- 褰撳墠 bucket manifest
- 褰撳墠 bucket 灞€閮ㄦ姤鍛?
- 褰撳墠 bucket 璇婃柇鏂囦欢

## 7.4 鍏煎灞傞泦涓湪 session / bridge

鏃у叆鍙ｄ笉鐩存帴鐞嗚В鏂?subject 缁嗚妭銆?

鏇村悎鐞嗙殑鏂瑰紡鏄細

- 鏂版鏋跺唴閮ㄤ骇鐢燂細
  - matrix report
  - subject summary
  - matrix / subject events
- session 灞傚啀鎶曞奖鍑猴細
  - `progress`
  - `final-summary`
  - `subjectResults`

## 8. 椋庨櫓璇勪及

## 8.1 鏋舵瀯椋庨櫓锛氬弻鐪熺浉

濡傛灉 `subject` 涓?`suite` 闀挎湡骞跺垪涓烘寮忕湡婧愶紝浼氬舰鎴愶細

- manifest 鍙岀湡鐩?
- report 鍙岀湡鐩?
- event 鍙岀湡鐩?

缂撹В鏂瑰紡锛?

- 灏芥棭鎶?`subjects/` 瀹氫负鍞竴姝ｅ紡鐪熸簮
- 鏃у璞″彧淇濈暀 compatibility wrapper 韬唤

## 8.2 瀹炵幇椋庨櫓锛歋tage 4 鍋囨媶鍒?

濡傛灉鍙槸鍦?Python 灞傝ˉ planner/executor锛屼絾涓嶆媶 CMake / script 閲岀殑闅愬紡閾捐矾锛屾渶缁堝彧鏄€滆〃闈?stage 鍖栤€濄€?

缂撹В鏂瑰紡锛?

- 鏄庣‘鎶?Stage 4 CMake 鍜?`verify-roadmap-0.py` 鐨勬媶瑙ｅ垪鍏ラ鎵瑰疄鏂?

## 8.3 浜у搧椋庨櫓锛氳寖鍥磋啫鑳€

濡傛灉绗竴浠借鍒掑悓鏃惰鐩栵細

- correctness
- perf
- multi-subject
- public CLI
- TUI

澶ф鐜囦細鍐嶆鍙樻垚瓒呭ぇ plan銆?

缂撹В鏂瑰紡锛?

- 棣栦唤璁″垝鍙鐩?`HelloWorldObject + Windows correctness + compatibility bridge`

## 8.4 杩佺Щ椋庨櫓锛歱arallel task 涓?active 涓荤嚎鍒嗗弶

鏈换鍔＄幇鍦ㄦ槸骞惰璁捐浠诲姟銆傚鏋滃悗缁笉鎶婂畠鍥炴敹鍒?active reboot 涓荤嚎锛屽鏄撳嚭鐜帮細

- 褰撳墠涓荤嚎缁х画鍐崇瓥
- 骞惰浠诲姟涔熺户缁喅绛?

缂撹В鏂瑰紡锛?

- 鍦ㄨ繘鍏?`plan` 鍓嶆槑纭畠鐨勬渶缁堝綊灞?

## 9. 闈炵洰鏍?

杩欎唤璁捐褰撳墠涓嶆壙璇猴細

- 绔嬪嵆鐢熸垚姝ｅ紡 `plan-v1-01.md`
- 绔嬪嵆鍒涘缓 implementation child task
- 绔嬪嵆鏇挎崲鐜版湁鎵€鏈?public test commands
- 绔嬪嵆鎺ュ叆鎬ц兘鎶ュ憡鍏ㄨ鐩?

## 10. 杩涘叆 plan 鍓嶇殑 gate

杩欎唤璁捐宸茬粡瓒充互杩涘叆 roadmap锛屼絾杩樹笉搴旇鐩存帴杩涘叆璁″垝鎵ц銆?

杩涘叆 `plan` 鍓嶏紝鎴戣涓哄繀椤诲喕缁撲笅闈㈠嚑椤癸細

1. 鏈苟琛屼换鍔″悗缁槸鍚﹀洖鏀跺埌 active reboot 涓荤嚎
2. 棣栦唤 plan 鏄惁鏄庣‘鎺掗櫎 `perf.dev / perf.release`
3. `subjects/HelloWorldObject/` 鏄惁绗竴闃舵鐗╃悊钀藉湴
4. public CLI / registry / TUI 鏄惁纭畾鍚庣疆
5. Windows `verify-roadmap-0` 鏄惁淇濇寔 mandatory non-regression gate

## 11. 缁撹

褰撳墠鎺ㄨ崘缁撹鏄細

- 璁捐閫氳繃
- 搴旇繘鍏?roadmap
- 浣嗗湪鍏抽敭 gate 闂鎷嶆澘鍓嶏紝涓嶈繘鍏?`writing-plans`
