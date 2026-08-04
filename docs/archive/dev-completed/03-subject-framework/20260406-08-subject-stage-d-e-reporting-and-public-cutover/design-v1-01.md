# Subject Stage D+E Reporting And Public Cutover 璁捐 v1.01

Date: 2026-04-06
Status: plan-ready

## 1. 鐩爣

鍦ㄧ幇鏈?`HelloWorldObject` subject spine 涔嬩笂琛ラ綈锛?

- `matrix report / subject summary / session summary`
- `matrix events / subject events / session events`
- subject registry / public CLI / TUI 鐨勬渶灏忓垏鎹?

骞朵繚鎸佷互涓嬪師鍒欎笉鍥為€€锛?

- matrix 浠嶆槸璇婃柇涓诲叆鍙?
- subject / session 浠嶆槸瀵艰埅鑱氬悎灞?
- legacy `suite/session/watch` 浠嶅彲鐢紝浣嗗彧浣滀负 compatibility surface

## 2. 闈炵洰鏍?

鏈璁′笉鍖呭惈锛?

- `perf.dev / perf.release`
- 澶?subject onboarding
- 娓呯悊鎴栧垹闄ょ幇鏈夋祴璇曞伐绋?
- 鎶?legacy `suiteResults` 鍙樻垚鏂?subject 缁撴灉鐨勬寮忕湡婧?

杩欎簺閮界户缁綊 `Stage F`銆?

## 3. 宸茬煡鐜扮姸

### 3.1 宸插瓨鍦ㄧ殑 subject spine

- `build/toolchains/run/testing/subjects.py`
  - 宸茶兘璇诲彇 `subject.manifest.json`锛屽苟鍥哄畾 artifact roots / bucket manifest 璺緞
- `build/toolchains/run/testing/subject_planner.py`
  - 宸茶兘浜у嚭 `selection / artifactsRoot / stagePlan`
- `build/toolchains/run/testing/subject_executor.py`
  - 宸茶兘鎵ц stage worker锛屽苟鍐欏嚭鏈€灏?`report.json`

### 3.2 浠嶇劧鍋?legacy 鐨?public/test surface

- `build/toolchains/run/testing/reporting.py`
  - 褰撳墠鍙潰鍚?legacy suite/session
- `build/toolchains/run/testing/events.py`
  - 褰撳墠 envelope 浠嶄互 `suiteId / stage` 涓轰腑蹇?
- `build/toolchains/run/testing/registry.py`
  - 当前 registry 仍停留在旧一代 formal object 集合
- `build/toolchains/run/commands/test.py`
  - 褰撳墠 public dispatch 浠嶄互 legacy suite spec 鍜?registry object 涓轰富
- `build/toolchains/run/tui.py`
  - 褰撳墠 watch / summary / registry 鍏ュ彛閮藉亣璁?session 浜嬩欢娴佷粛鏄棫璇箟

## 4. 璁捐鍐冲畾

## 4.1 Reporting 閲囩敤 `B2`

鐩存帴閲囩敤 `docs/archive/discuss/reporting/20260406-20-reporting-schema-design-v1-01.md` 鐨勭粨璁猴細

- `matrices/<matrix-id>/report.json`
  - 鏄崟涓?matrix 鐨勮瘖鏂富鍏ュ彛
  - 淇濈暀 `selection`銆乣stageResults`銆乣artifactResults`銆乣errors`
- `subject-report/summary.json`
  - 鍙仛 subject 鑱氬悎涓庡鑸?
- `artifacts/logs/tests/<run-id>/summary.json`
  - 淇濈暀鐜版湁 session summary 璇箟
  - 澧為噺鍔犲叆 `subjectStatusCounts` 涓?`subjectResults`

## 4.2 Events 閲囩敤鏂规 `C`

鐩存帴閲囩敤 `docs/archive/discuss/reporting/20260406-21-event-schema-alignment-design-v1-01.md` 鐨勭粨璁猴細

- matrix events
  - canonical detailed stream
- subject events
  - canonical aggregate stream
- session events
  - compatibility projection

### 鍗曞啓鑰呭師鍒?

- matrix / subject events
  - 鐢?subject execution path 鍐?
- session events
  - 鐢?public command orchestration 鍐?

worker 涓嶇洿鎺?append `events.jsonl`銆?

## 4.3 Public cutover 浠?subject 涓轰竴绛夊璞★紝浣嗕笉鎶规帀 legacy surface

Stage E 鐨勭涓€鐗堜笉鏄€滃垹闄ゆ棫鍏ュ彛鈥濓紝鑰屾槸锛?

- 缁?registry 澧炲姞 `subject` object type
- 缁?public command 澧炲姞 `test subject --id <subject-id>`
- 璁?registry list / manifest / TUI 鑳藉彂鐜板苟鎵ц subject
- 鍚屾椂淇濈暀 legacy suite / watch / summary / registry 鐨勫吋瀹硅矾寰?

杩欐剰鍛崇潃锛?

- `subject` 鏄柊鐨勪竴绛夋墽琛屽璞?
- legacy suite 浠嶅彲浣滀负 alias / compatibility path 瀛樺湪
- `suiteResults` 涓嶆壙杞?`subject` 鐨勬寮忕湡鐩?

## 5. 妯″潡杈圭晫

## 5.1 `subject_reporting.py`

鏂板涓€涓嫭绔嬫ā鍧楋紝璐熻矗锛?

- 鎶?executor 鐨?`matrix execution result` 鍐欐垚 `report.json`
- 鑱氬悎鍚屼竴 subject 鐨?`subject-report/summary.json`
- 缁熶竴杩斿洖鍙啓鍏?event / session summary 鐨勮交閲忓璞?

杩欐牱鍙互閬垮厤锛?

- 鎶?legacy `reporting.py` 缁х画鑶ㄨ儉鎴愬悓鏃舵壙杞?old suite 鍜?new subject 鐨勫弻澶村疄鐜?

## 5.2 `events.py`

淇濈暀 `build_event()` 浣滀负缁熶竴鍏ュ彛锛屼絾鍋?additive 鎵╁睍锛?

- 澧炲姞 `streamScope`
- 澧炲姞 `subjectId`
- 澧炲姞 `matrixId`
- 澧炲姞 `goalId`
- 澧炲姞 `stageId`
- 澧炲姞 `bucket`
- 澧炲姞 `stageScope`

legacy `suiteId / stage` 缁х画淇濈暀锛屼緵鏃ф秷璐规柟鍏煎璇诲彇銆?

## 5.3 `subject_executor.py`

鑱岃矗鎵╁睍涓猴細

- 浠嶈礋璐?stage 鎵ц
- 鍦?stage 鐢熷懡鍛ㄦ湡涓婄敓鎴?matrix canonical events
- 鍦?`report-assemble` 涔嬪悗鎶婄粨鏋滀氦缁?`subject_reporting.py`

瀹冧笉璐熻矗锛?

- 鐩存帴鍐?session summary
- 鐩存帴鎵挎媴 TUI / watch 鍏煎鎶曞奖

## 5.4 `reporting.py`

缁х画鍙仛 run/session 灞傦紝浣嗘墿灞曚负鍙帴鍙?`subjectResults`锛?

- `summary.json`
  - 澧為噺鍐欏叆 `subjectStatusCounts` 涓?`subjectResults`
- `final-summary`
  - 澧為噺甯﹀嚭 `subjectResults`
- 涓嶆妸 `subjectResults` 鍥炵亴鎴?`suiteResults`

## 5.5 `registry.py` 涓?public command

`registry.py` 闇€瑕佹柊澧?`subject` collection锛屽苟鎶婂畠浠撼鍏ワ細

- `flatItems`
- `find_registry_object()`
- `expand_execution_plan()`

public command 灞傞渶瑕佹柊澧烇細

- `run test subject --id <subject-id>`

骞惰锛?

- `run test registry list`
- `run test registry check-consistency`
- TUI test menu / registry route

閮借兘璇嗗埆鏂扮殑 `subject` object type銆?

## 6. 鑼冨洿鏀舵暃

鏈疆 public cutover 缁х画缁戝畾鍒帮細

- `HelloWorldObject`
- 鍥涗釜鏃㈡湁 Windows matrix

杩欎繚璇侊細

- registry / CLI / TUI 鐨勭涓€鐗?subject public entry 鏈夌湡瀹炲疄鐜板璞?
- 涓嶄細鎶婂 subject 閫夋嫨銆侀粯璁ゆ帓搴忋€佽法 subject 姹囨€荤瓑 `Stage F` 璁鎻愬墠甯﹁繘鏉?

## 7. 棰勬湡闀挎湡娌夋穩

褰撴湰璁″垝鏈潵鎵ц闂幆鍚庯紝搴旀妸闀挎湡杈圭晫鍥炲啓鍒帮細

- `docs/archive/architecture/subject-test-framework-v1/reporting-and-public-cutover-v1.md`

骞舵寜瀹為檯 public surface 鏇存柊锛?

- `wiki/06-娴嬭瘯楠岃瘉/`

## 8. 缁撹

`Stage D + Stage E` 鐨勫疄鐜颁富绾垮凡缁忓彲浠ュ浐瀹氫负锛?

1. 鍏堣ˉ subject reporting 涓?canonical event
2. 鍐嶆妸 session summary / watch / TUI 鎺ユ垚鍏煎鎶曞奖
3. 鏈€鍚庢妸 subject 鍏紑涓?registry / public command / TUI 鐨勪竴绛夊璞?

鍙淇濇寔杩欎釜椤哄簭锛屽氨涓嶄細涓轰簡鈥滃厛鐪嬪埌 public 鍏ュ彛鈥濊€屾妸 reporting/session 璇箟鍋氬潖銆?
