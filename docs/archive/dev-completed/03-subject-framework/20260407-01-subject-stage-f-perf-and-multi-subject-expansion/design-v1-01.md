# Subject Stage F Perf And Multi-Subject Expansion 璁捐 v1.01

Date: 2026-04-07
Status: plan-ready

## Scope Correction

This design originally framed Stage F around `GenericEcho` as the second formal subject.
The user later corrected the immediate requirement: all projects under `tests/smoke/input/` should migrate to `subjects/`.

Implemented scope correction:
- Added canonical subject/source trees for `HelloWorld`, `GenericEcho`, `ReflectionLite`, `PInvokeLite`, and `HostEmbeddingLite`.
- Switched smoke suite manifests and legacy run-manifest smoke commands to `subjects/*/source`.
- Kept `tests/smoke/input/*` in place as compatibility and long-term baseline assets rather than deleting them.

Design implication:
- The migration concern is now complete for the smoke-input set.
- Remaining Stage F perf/reporting work should be treated as follow-up on top of the new multi-subject baseline.

## 1. 鐩爣

鍦ㄧ幇鏈?`subject` spine 涔嬩笂瀹屾垚 `Stage F` 鐨勯鐗堟敹鍙ｏ細

- 鎺ュ叆绗簩涓寮?subject锛歚GenericEcho`
- 寮曞叆 subject-aware 鐨?`perf.dev` / `perf.release`
- 鍥哄畾绗簩涓?subject onboarding pattern
- 鍥哄畾 perf baseline / report 鍦ㄦ柊 subject 妯″瀷涓嬬殑钀界偣

骞朵繚鎸佷互涓嬪師鍒欎笉鍥為€€锛?

- 涓嶅垹闄ょ幇鏈夋祴璇曞伐绋?
- 涓嶆妸 `Stage F` 鍐嶆媶鎴愮嫭绔?perf follow-up
- 不把第二个 subject 重新绑回旧一代 registry formal object 作为正式真源

## 2. 闈炵洰鏍?

鏈璁′笉鍖呭惈锛?

- 澶?subject 鎵归噺璋冨害
- macOS / Linux 鐨勯鎵?perf host
- 鍏ㄩ噺 smoke/gate/workflow 瀵硅薄閮借縼鎴?subject
- 瀵?`HelloWorldObject` native proof worker 鍋氬交搴曟硾鍖?

杩欎簺浠嶅睘浜?`Stage F` 涔嬪悗鐨勬墿灞曡寖鍥淬€?

## 3. 宸茬煡鐜扮姸

### 3.1 褰撳墠 subject spine 鐨勫己椤?

- manifest / planner / executor / reporting / registry / public command 宸插彲绋冲畾鏈嶅姟 `HelloWorldObject`
- `--goal` / `--matrix` 宸茶兘閫氳繃 `run test subject` 杩涘叆 planner
- session summary / TUI / registry 宸茶兘娑堣垂 subject 缁撴灉

### 3.2 褰撳墠 subject spine 鐨勬槑鏄捐竟鐣?

- `subject_workers.py` 鐨?`build-target` / `runtime-observe` 鏄庢樉缁戝畾 `HelloWorldObject` 鐨?native proof CMake 璺緞
- 绗簩涓?subject 鐩墠杩樻病鏈夋寮?manifest
- perf 浠嶅仠鐣欏湪鏃?`tests/perf/<suite>/baselines/<host>.json` 瑙勫垯锛屾病鏈?subject-aware 璋冨害

## 4. 璁捐鍐冲畾

## 4.1 绗簩涓?subject 鍥哄畾涓?`GenericEcho`

`GenericEcho` 浣滀负绗簩涓寮?subject锛屽師鍥犳槸锛?

- 鐢ㄦ埛宸茬粡鏄庣‘瑕佹眰淇濈暀 `echo`
- 浠撳簱鍐呭凡鏈夌ǔ瀹氱殑 managed smoke 杈撳叆
- 瀹冮€傚悎鎵挎媴 benchmark / perf subject 鐨勯涓牱鏉?

姝ｅ紡 canonical source 鍥哄畾涓猴細

- `subjects/GenericEcho/source/`

鏃ц矾寰勶細

- `tests/smoke/input/GenericEcho/`

缁х画淇濈暀涓?compatibility / long-term baseline 杈撳叆锛屼笉鍒犻櫎銆?

## 4.2 绗簩涓?subject 閲囩敤 benchmark 绫诲埆

`GenericEcho` 鐨?subject category 鍥哄畾涓猴細

- `benchmark`

鍘熷洜锛?

- 瀹冧笉闇€瑕佸鐢?`HelloWorldObject` 鐨?native proof correctness 璺緞
- 瀹冨彲浠ョ洿鎺ユ壙鎷?`perf.dev` / `perf.release`
- 瀹冨ぉ鐒堕€傚悎浣滀负绗簩涓?subject onboarding pattern 鐨勬渶灏忔牱鏉?

## 4.3 Stage F 鍏堣蛋 managed perf pipeline锛岃€屼笉鏄硾鍖?native proof pipeline

涓轰簡閬垮厤鎶?`HelloWorldObject` 鐨?native proof 鐗逛緥璺緞纭硾鍖栧埌绗簩涓?subject锛屾湰杞?`GenericEcho` 寮曞叆鏂扮殑 managed perf pipeline锛?

- `source-resolve`
- `host-input-build`
- `runtime-perf-collect`
- `perf-evaluate`
- `report-assemble`

璁捐鍚箟锛?

- `GenericEcho` 鐨?perf 涓嶄緷璧?`analysis/generated/build-target`
- 绗簩涓?subject onboarding pattern 鍙互鍏堝湪 managed runtime 涓婄ǔ瀹?
- `HelloWorldObject` 鐨?native correctness 璺緞淇濇寔鍘熻竟鐣岋紝涓嶅湪鏈疆琚揩娉涘寲

## 4.4 perf baseline 鏀逛负 subject-aware 璺緞锛屼絾缁х画鏀惧湪 `tests/perf/`

涓轰簡鍏煎椤圭洰鏃㈡湁 perf baseline 瑙勫垯锛屽悓鏃惰 baseline 璺?subject/matrix 瀵归綈锛屾湰杞浐瀹氾細

- baseline 鏍逛粛鏀惧湪 `tests/perf/`
- 浣?subject perf 鐨勬寮忚矾寰勬敼鎴愶細
  - `tests/perf/subjects/<subject-id>/<matrix-id>/baselines/<host>.json`

鍘熷洜锛?

- 淇濈暀鈥渂aseline 灞炰簬 repo 鍐呮寮忔祴璇曡祫浜р€濈殑鏃㈡湁瑙勫垯
- 閬垮厤鎶?baseline 娣疯繘 `artifacts/`
- 璁?subject perf 涓嶅啀浼鎴愭棫 suite perf

## 4.5 `perf.dev` 涓?`perf.release` 鍏辩敤涓€鏉?subject 鍏ュ彛

鍏紑鍏ュ彛涓嶆柊澧炴柊鍛戒护鏃忥紝缁熶竴淇濇寔锛?

- `run test subject --id subject/GenericEcho --goal perf.dev`
- `run test subject --id subject/GenericEcho --goal perf.release`

杩欐剰鍛崇潃锛?

- perf 鍦ㄦ柊妯″瀷涓嬩粛鏄?`subject + goal + matrix`
- 涓嶅啀棰濆寮曞叆鐙珛 `test perf subject ...` 璇硶

## 4.6 `perf.release` 鍏佽鎵╁嚭 `report/` 闄勪欢鐩綍

褰撳墠 `HelloWorldObject` 鐨?matrix 鎶ュ憡鍙啓锛?

- `matrices/<matrix-id>/report.json`

鏈疆瀵?`perf.release` 澧為噺鍏佽锛?

- `matrices/<matrix-id>/report/`

浣滀负 perf 闄勪欢鐩綍锛屼緥濡傦細

- `samples.json`
- `summary.json`
- `baseline-compare.json`

鍚屾椂缁х画淇濈暀锛?

- `matrices/<matrix-id>/report.json`

浣滀负 machine-first 鎬诲叆鍙ｃ€?

## 4.7 绗簩涓?subject onboarding pattern 鍙浐瀹氬埌 `GenericEcho`

鏈疆涓嶆壒閲忔妸锛?

- `ReflectionLite`
- `PInvokeLite`
- `HostEmbeddingLite`

涔熶竴璧疯縼鎴?subject銆?

鑰屾槸鍙妸 `GenericEcho` 鍥哄畾鎴愮浜屼釜姝ｅ紡 subject锛岀敤鏉ユ矇娣€锛?

- canonical source 钀界偣
- benchmark subject manifest 鍐欐硶
- managed perf pipeline
- perf baseline 瑙勫垯

## 4.8 娴嬭瘯宸ョ▼闀挎湡鍩虹嚎鏁寸悊鍙仛鏄犲皠涓庡綊妗ｈ涔夛紝涓嶅仛鍒犻櫎

Stage F 鐨勨€滄祴璇曞伐绋嬫暣鐞嗏€濆湪鏈疆鍙浐瀹氫袱浠朵簨锛?

- 鍝簺鏃у伐绋嬪凡缁忔湁鏂扮殑 canonical subject
- 鍝簺鏃у伐绋嬬户缁綔涓?compatibility / baseline 璧勪骇淇濈暀

涓嶅仛锛?

- 鍒犻櫎鏃у伐绋?
- 澶ц妯?rename

## 5. 妯″潡杈圭晫

## 5.1 `subjects/`

鏂板锛?

- `subjects/GenericEcho/subject.manifest.json`
- `subjects/GenericEcho/source/`

璐熻矗绗簩涓?subject 鐨勬寮?canonical source 涓?pipeline 瀹氫箟銆?

## 5.2 `subject_workers.py`

鏂板锛?

- `runtime-perf-collect`
- `perf-evaluate`

蹇呰鏃舵娊鍑?managed runtime helper锛岄伩鍏嶆妸 `HelloWorldObject` native proof worker 鐨勭壒渚嬮€昏緫缁х画濉炶繘鍚屼竴涓?worker 鍒嗘敮銆?

## 5.3 `subject_reporting.py`

澧為噺鏀寔锛?

- perf matrix report 鐨?metrics / baseline / compare 鎽樿瀛楁
- `perf.release` 鐨勯檮浠剁储寮?

浣嗕粛淇濇寔锛?

- `report.json` 鏄?matrix machine-first 涓诲叆鍙?

## 5.4 `perf.py`

浠庘€滄棫 suite perf baseline helper鈥濇墿鎴愨€滃弻鍏ュ彛 helper鈥濓細

- 鏃?`suite` perf 缁х画鍙敤
- 鏂?subject perf 璧?subject-aware baseline path

## 5.5 registry / public command / TUI

鐜版湁 `subject` object 涓?`test subject` 鍏ュ彛缁х画娌跨敤锛屽彧鍋氬閲忥細

- registry 鎵弿鍒?`subject/GenericEcho`
- public summary/watch/TUI 鑳芥纭睍绀烘柊鐨?perf goal / benchmark subject 缁撴灉

## 6. 鑼冨洿鏀舵暃

鏈疆 Stage F 鍥哄畾涓猴細

- host: `windows`
- second subject: `GenericEcho`
- goals: `perf.dev` / `perf.release`

杩欐牱鍙互淇濊瘉锛?

- `Stage F` 鐪熸寮€濮嬭В鍐?perf 涓?multi-subject
- 浣嗕笉浼氬悓鏃舵妸 host 鎵╁睍銆佸 subject 鎵瑰鐞嗗拰 native perf 涓€璧峰甫杩涙潵

## 7. 棰勬湡闀挎湡娌夋穩

褰撴湰璁″垝鏈潵鎵ц闂幆鍚庯紝搴旀妸闀挎湡杈圭晫鍥炲啓鍒帮細

- `docs/archive/architecture/subject-test-framework-v1/perf-and-multi-subject-expansion-v1.md`

骞舵寜瀹為檯 perf / second-subject public surface 鏇存柊锛?

- `wiki/06-娴嬭瘯楠岃瘉/`

## 8. 缁撹

`Stage F` 鐨勯鏉″疄鏂戒富绾垮彲浠ュ浐瀹氫负锛?

1. 鍏堟帴鍏ョ浜屼釜姝ｅ紡 subject `GenericEcho`
2. 鍐嶄负瀹冨缓绔?managed perf pipeline 涓?subject-aware baseline
3. 鏈€鍚庢敹鍙?`perf.release` report 涓庣浜屼釜 subject onboarding pattern

鍙淇濇寔杩欎釜椤哄簭锛屽氨涓嶄細鎶?`HelloWorldObject` 鐨?native proof 鐗逛緥璺緞璇綋鎴愭墍鏈夋湭鏉?subject 鐨勫敮涓€妯℃澘銆?
