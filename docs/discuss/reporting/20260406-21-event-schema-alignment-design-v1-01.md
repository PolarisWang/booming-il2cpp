# event schema 瀵归綈璁捐 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 鐩爣

鍦ㄥ凡缁忓浐瀹氾細

- `matrix report / subject summary / session summary` 涓夊眰 reporting schema
- executor / worker 鐨勮矗浠昏竟鐣?
- matrix / subject / session 涓夊眰浜х墿钀界偣

涔嬪悗锛屼笅涓€姝ュ繀椤绘妸 `events.jsonl` 涔熷榻愩€?

鍚﹀垯鍚庨潰浼氬嚭鐜颁笁绫诲吀鍨嬮棶棰橈細

- `report.json` 璇寸殑鏄竴濂楄涔夛紝`events.jsonl` 璇寸殑鏄彟涓€濂楄涔?
- matrix / subject / session 涓夊眰浜嬩欢娴侀噸澶嶄絾涓嶄竴鑷?
- 褰撳墠 `test watch` / TUI 缁х画渚濊禆鏃т簨浠讹紝鑰屾柊 subject 绠＄嚎鍙堝彟璧风倝鐏?

鎵€浠ヨ繖涓€姝ヨ鍥炵瓟锛?

- 涓夊眰浜嬩欢娴佸垎鍒湇鍔¤皝
- 鍝簺浜嬩欢鏄?canonical domain events
- 鍝簺浜嬩欢鍙槸缁欏綋鍓?watch/TUI 鐢ㄧ殑鍏煎鎶曞奖
- 浜嬩欢 envelope 鏈€灏忓瓧娈靛簲璇ユ槸浠€涔?

## 2. 鐜版湁绾︽潫

浠庡綋鍓嶄唬鐮佸拰娴嬭瘯鐪嬶紝宸茬粡鏈夊嚑涓幇瀹炵害鏉熶笉鑳藉拷鐣ワ細

- session 绾?`events.jsonl`
  - 宸茬粡琚?`test watch` / TUI 鐩存帴娑堣垂
- 褰撳墠 TUI 鐩存帴璇嗗埆鐨勪簨浠剁被鍨嬩富瑕佹槸锛?
  - `session-start`
  - `progress`
  - `stage-start`
  - `warning`
  - `artifact`
  - `final-summary`
- 褰撳墠 TUI 閫氳繃 `payload.completedUnits / totalUnits`
  - 鎺ㄥ杩涘害鐧惧垎姣?
- 褰撳墠 test reporting 娴嬭瘯宸茬粡瑕佹眰锛?
  - `final-summary` payload 涓嶇洿鎺ュ `suiteResults`

杩欐剰鍛崇潃锛?

- 鎴戜滑涓嶈兘绠€鍗曟妸 session event stream 鍏ㄩ儴鎺ㄥ€掗噸鏉?
- 浣嗕篃涓嶅簲璇ヨ鏂?subject 浜嬩欢姘镐箙鍥板湪鏃?`suite` 鍛藉悕閲?

## 3. 涓変釜鍙€夋柟鍚?

## 3.1 鏂规 A锛氬彧淇濈暀涓€鏉?session 浜嬩欢娴?

鍋氭硶锛?

- 涓嶅紩鍏?matrix / subject 涓撳睘浜嬩欢娴?
- 鎵€鏈変俊鎭兘杩?`artifacts/logs/tests/<run-id>/events.jsonl`

浼樼偣锛?

- 鏈€绠€鍗?
- 褰撳墠 watch/TUI 鍏煎鏈€瀹规槗

闂锛?

- matrix 灞傛帓闅滃お寮?
- subject 鍐呭涓?matrix 浼氭妸涓€鏉?stream 鎼呭湪涓€璧?
- 涓嶇鍚堟垜浠凡缁忔敹鏁涘嚭鐨?`matrix / subject / session` 涓夊眰妯″瀷

缁撹锛?

- 涓嶆帹鑽?

## 3.2 鏂规 B锛氫笁灞備簨浠舵祦閮戒娇鐢ㄥ悓涓€濂?canonical 浜嬩欢

鍋氭硶锛?

- matrix / subject / session 鍏ㄩ儴缁熶竴鎴愬悓涓€缁?eventType
- 褰撳墠 TUI / watch 涔嬪悗鏁翠綋鏀归€犲幓閫傞厤鏂颁簨浠?

浼樼偣锛?

- 鏋舵瀯鏈€鏁撮綈
- 娌℃湁鈥滃吋瀹逛簨浠垛€濊繖灞傛妧鏈€?

闂锛?

- 瀵圭幇鏈?watch/TUI 鍐插嚮澶ぇ
- 绗竴鐗堝疄鐜版椂浼氭妸浜嬩欢 schema 鏀归€犲拰 UI 鏀归€犲己缁戝畾

缁撹锛?

- 闀胯繙骞插噣锛屼絾涓嶉€傚悎绗竴鐗堣惤鍦?

## 3.3 鏂规 C锛歮atrix / subject 閲囩敤 canonical 浜嬩欢锛宻ession 淇濈暀鍏煎鎶曞奖

鍋氭硶锛?

- `matrix events.jsonl`
  - 浣滀负 detailed canonical stream
- `subject events.jsonl`
  - 浣滀负 aggregate canonical stream
- `session events.jsonl`
  - 缁х画鏈嶅姟缁熶竴 watch/TUI
  - 浠ュ吋瀹逛簨浠朵负涓?
  - 鍙閲忛檮甯﹂儴鍒?canonical 浜嬩欢

浼樼偣锛?

- 涓嶆墦鏂綋鍓?watch/TUI
- matrix 灞傚拰 subject 灞備粛鐒惰兘寤虹珛鏂扮殑姝ｈ璇箟
- 瀹炴柦鍒囧垎鍚堢悊锛岃縼绉婚闄╂渶浣?

闂锛?

- 鐭湡浼氬瓨鍦ㄢ€渃anonical + compatibility projection鈥濅袱灞傝涔?

缁撹锛?

- 鎺ㄨ崘閲囩敤鏂规 C

## 4. 鎺ㄨ崘缁撹

姝ｅ紡鎺ㄨ崘锛?

- 閲囩敤鏂规 C

骞跺浐瀹氫笁鏉″師鍒欙細

- matrix event stream
  - 鏄?stage 绾ф墽琛岀粏鑺傜殑鏉冨▉鏉ユ簮
- subject event stream
  - 鏄?subject 鑱氬悎杩囩▼鐨勬潈濞佹潵婧?
- session event stream
  - 鏄?run/watch/TUI 鐨勫吋瀹逛笌瀵艰埅鏉ユ簮

杩欏彞璇濆緢閲嶈锛屽洜涓哄畠鍐冲畾浜嗗悗闈㈣皝鎵嶆槸鈥滅湡鏃ュ織鈥濓細

- 鐪熸鐢ㄤ簬瀹氫綅 stage 琛屼负鐨勶紝鐪?matrix events
- 鐪熸鐢ㄤ簬鐪?subject 姹囨€昏繃绋嬬殑锛岀湅 subject events
- 鐪熸鐢ㄤ簬鐪嬬粺涓€鍏ュ彛瀹炴椂杩涘害鐨勶紝鐪?session events

## 5. 涓夊眰浜嬩欢鏂囦欢鐨勬寮忚亴璐?

## 5.1 matrix events

璺緞锛?

- `artifacts/subjects/<subject-id>/matrices/<matrix-id>/events.jsonl`

鑱岃矗锛?

- 璁板綍褰撳墠 matrix 鐨勮缁嗘墽琛屾椂闂寸嚎
- 璁板綍 stage start / reuse / invalidation / finish / warning / artifact
- 涓哄悗缁帓闅滃拰缁嗙矑搴?UI 鎻愪緵鏈€鐩存帴鏁版嵁婧?

涓嶈礋璐ｏ細

- 鑱氬悎澶氫釜 matrix
- 鎵挎媴 session 缁熶竴 watch 鍏煎

## 5.2 subject events

璺緞锛?

- `artifacts/subjects/<subject-id>/subject-report/events.jsonl`

鑱岃矗锛?

- 璁板綍褰撳墠 subject 鐨?matrix 鑱氬悎杩囩▼
- 璁板綍 `subject-start`
- 璁板綍鍚?matrix 鐨勫畬鎴愭憳瑕?
- 璁板綍 `subject-summary`

涓嶈礋璐ｏ細

- 杞偍姣忎釜 stage 鐨勮缁嗘墽琛岃繃绋?

涔熷氨鏄锛?

- subject events 涓嶉暅鍍?matrix 鐨勬墍鏈?stage 浜嬩欢

## 5.3 session events

璺緞锛?

- `artifacts/logs/tests/<run-id>/events.jsonl`

鑱岃矗锛?

- 缁х画鏈嶅姟 `test watch` / TUI / 褰撳墠 run 绾х敤鎴蜂綋楠?
- 鎻愪緵缁熶竴鍏ュ彛鐨勫疄鏃惰繘搴︿笌鏈€缁堟憳瑕?
- 浣滀负 subject 鏂版ā鍨嬫帴鍏ユ棫鍏ュ彛鐨勫吋瀹规ˉ鎺ュ眰

涓嶈礋璐ｏ細

- 鎴愪负 matrix 绾ц缁嗘帓闅滄棩蹇楃殑鍞竴鐪熸簮

## 6. canonical event envelope

鎺ㄨ崘鏂?subject 妗嗘灦閲岀殑 canonical event 鏈€灏?envelope 涓猴細

```json
{
  "schemaVersion": "2026-04-06",
  "streamScope": "matrix",
  "eventType": "stage-finished",
  "timestampUtc": "2026-04-06T08:20:00Z",
  "runId": "20260406-hello-001",
  "subjectId": "HelloWorldObject",
  "matrixId": "windows-reference-trace",
  "goalId": "correctness.platform",
  "stageId": "analysis-frontend",
  "bucket": "analysis",
  "stageScope": "shared",
  "status": "ok",
  "payload": {}
}
```

## 6.1 椤跺眰瀛楁

- `schemaVersion`
  - 浜嬩欢 schema 鐗堟湰
- `streamScope`
  - `matrix | subject | session`
- `eventType`
  - 浜嬩欢绫诲瀷
- `timestampUtc`
  - UTC 鏃堕棿鎴?
- `runId`
  - 褰撳墠 run
- `subjectId`
  - 褰撳墠 subject
- `matrixId`
  - 褰撳墠 matrix
- `goalId`
  - 褰撳墠 goal
- `stageId`
  - 褰撳墠 stage
- `bucket`
  - 褰撳墠 bucket
- `stageScope`
  - `shared | matrix`
- `status`
  - 褰撳墠浜嬩欢瀵瑰簲鐨勭粨鏋滅姸鎬?
- `payload`
  - 浜嬩欢鍏蜂綋鍐呭

## 6.2 涓庣幇鏈?`build_event()` 鐨勫吋瀹圭瓥鐣?

褰撳墠 `build_event()` 鍙湁锛?

- `runId`
- `suiteId`
- `stage`
- `status`

鎵€浠ユ帹鑽愭湭鏉ユ敼涓?additive 鎵╁睍锛岃€屼笉鏄牬鍧忔€ф浛鎹細

- 淇濈暀 `suiteId`
  - 仅用于 legacy suite/旧公开验证入口
- 淇濈暀 `stage`
  - 浠呬綔涓哄吋瀹瑰瓧娈?
- 鏂板锛?
  - `streamScope`
  - `subjectId`
  - `matrixId`
  - `goalId`
  - `stageId`
  - `bucket`
  - `stageScope`

涔熷氨鏄細

- 鏃ф秷璐规柟缁х画鍙
- 鏂?subject 浜嬩欢涓嶅啀琚揩濉炶繘 `suiteId / stage`

## 7. canonical eventType 闆嗗悎

绗竴鐗堟帹鑽愬浐瀹氫互涓?canonical eventType锛?

- `subject-start`
- `matrix-start`
- `stage-start`
- `stage-reused`
- `stage-invalidated`
- `stage-finished`
- `artifact`
- `warning`
- `matrix-summary`
- `subject-summary`
- `final-summary`

璇存槑锛?

- `progress`
  - 涓嶄綔涓烘柊 subject 妗嗘灦鐨?canonical event
  - 瀹冧繚鐣欎负 session compatibility projection

## 8. 姣忕被 canonical 浜嬩欢鐨勬渶灏忚涔?

## 8.1 `subject-start`

鍑虹幇浣嶇疆锛?

- `subject events`
- `session events`

鏈€灏?payload锛?

```json
{
  "requestedGoalId": "correctness.platform",
  "matrixIds": [
    "windows-dev-output",
    "windows-reference-trace"
  ]
}
```

## 8.2 `matrix-start`

鍑虹幇浣嶇疆锛?

- `matrix events`
- `session events`

鏈€灏?payload锛?

```json
{
  "pipelineId": "proof-runtime-trace",
  "expectedTerminalBucket": "runtime",
  "reportPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/report.json"
}
```

## 8.3 `stage-start`

鍑虹幇浣嶇疆锛?

- `matrix events`
- `session events`

鏈€灏?payload锛?

```json
{
  "order": 3,
  "completedStages": 2,
  "totalStages": 8,
  "activeUnit": "HelloWorldObject/windows-reference-trace/analysis-frontend"
}
```

## 8.4 `stage-reused`

鍑虹幇浣嶇疆锛?

- `matrix events`
- `session events` 鍙€?

鏈€灏?payload锛?

```json
{
  "order": 3,
  "completedStages": 3,
  "totalStages": 8,
  "reason": "fingerprint-match",
  "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json"
}
```

璇存槑锛?

- 瀹冭〃杈锯€滆繖涓€姝ユ病鏈夋墽琛?worker锛岃€屾槸鍛戒腑浜嗗鐢ㄢ€?

## 8.5 `stage-invalidated`

鍑虹幇浣嶇疆锛?

- `matrix events`
- `session events` 鍙€?

鏈€灏?payload锛?

```json
{
  "order": 3,
  "completedStages": 2,
  "totalStages": 8,
  "reason": "fingerprint-mismatch:driver-changed",
  "bucketRoot": "artifacts/subjects/HelloWorldObject/shared/analysis"
}
```

璇存槑锛?

- 瀹冭〃杈锯€渆xecutor 瀵瑰綋鍓?bucket 搴旂敤浜嗗け鏁堥噸寤衡€?
- 瀹冧笉鏄?stage 鎵ц瀹屾垚浜嬩欢

## 8.6 `stage-finished`

鍑虹幇浣嶇疆锛?

- `matrix events`
- `session events` 鍙€?

鏈€灏?payload锛?

```json
{
  "order": 3,
  "completedStages": 3,
  "totalStages": 8,
  "planMode": "invalidated",
  "actionTaken": "executed",
  "durationMs": 3280,
  "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
  "reportPaths": [
    "artifacts/subjects/HelloWorldObject/shared/analysis/contract-validate.report.json"
  ],
  "primaryEvidencePaths": [
    "artifacts/subjects/HelloWorldObject/shared/analysis/typed-il-ir.json"
  ],
  "failure": null
}
```

## 8.7 `artifact`

鍑虹幇浣嶇疆锛?

- `matrix events`
- `session events`

鏈€灏?payload锛?

```json
{
  "path": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
  "artifactKind": "manifest"
}
```

杩欓噷瑕佺壒鍒浐瀹氫竴鏉¤鍒欙細

- `artifact` 涓嶆槸涓?bucket 鍐呮墍鏈夋枃浠堕€愪釜鎵撶偣
- 瀹冨彧鐢ㄤ簬鏆撮湶鐢ㄦ埛鎴栬皟璇曡€呯湡姝ｄ細鍏冲績鐨?surfaced artifacts

渚嬪锛?

- 褰撳墠 bucket manifest
- 灞€閮?report
- 鍏抽敭 trace / generated source
- 鏈€缁?matrix / subject / session summary

## 8.8 `warning`

鍑虹幇浣嶇疆锛?

- 涓夊眰閮藉厑璁?

鏈€灏?payload锛?

```json
{
  "message": "subject summary written with 1 failed matrix"
}
```

## 8.9 `matrix-summary`

鍑虹幇浣嶇疆锛?

- `matrix events`
- `subject events`
- `session events` 鍙€?

鏈€灏?payload锛?

```json
{
  "status": "ok",
  "terminalBucket": "runtime",
  "reportPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/report.json",
  "eventsPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/events.jsonl"
}
```

## 8.10 `subject-summary`

鍑虹幇浣嶇疆锛?

- `subject events`
- `session events`

鏈€灏?payload锛?

```json
{
  "status": "fail",
  "matrixStatusCounts": {
    "total": 4,
    "ok": 3,
    "fail": 1,
    "skip": 0,
    "aborted": 0
  },
  "subjectSummaryPath": "artifacts/subjects/HelloWorldObject/subject-report/summary.json"
}
```

## 8.11 `final-summary`

鍑虹幇浣嶇疆锛?

- `session events`

瀹冪户缁綔涓?session 鐨勬渶缁堢粨鏉熶簨浠讹紝浣?payload 瑕佷笌鏂扮殑 session summary 瀵归綈銆?

鎺ㄨ崘鏈€灏?payload锛?

```json
{
  "finalStatus": "fail",
  "exitCode": 1,
  "errors": [],
  "summaryPath": "artifacts/logs/tests/20260406-hello-001/summary.json",
  "eventsPath": "artifacts/logs/tests/20260406-hello-001/events.jsonl",
  "sessionPath": "artifacts/logs/tests/20260406-hello-001/session.json",
  "consolePath": "artifacts/logs/tests/20260406-hello-001/console.log",
  "telemetryPath": "artifacts/logs/tests/20260406-hello-001/telemetry.json",
  "artifacts": [
    "artifacts/subjects/HelloWorldObject/subject-report/summary.json"
  ],
  "subjectStatusCounts": {
    "total": 1,
    "ok": 0,
    "fail": 1,
    "skip": 0,
    "aborted": 0
  },
  "subjectResults": [
    {
      "subjectId": "HelloWorldObject",
      "requestedGoalId": "correctness.platform",
      "status": "fail",
      "subjectSummaryPath": "artifacts/subjects/HelloWorldObject/subject-report/summary.json"
    }
  ]
}
```

## 9. session compatibility projection

杩欐槸褰撳墠鏈€鍏抽敭鐨勫吋瀹圭偣锛岄渶瑕佸崟鐙畾姝汇€?

鎺ㄨ崘瑙勫垯锛?

- session stream 鍏佽鍚屾椂鍖呭惈锛?
  - canonical 楂樺眰浜嬩欢
  - 鍏煎 watch/TUI 鐨?projection 浜嬩欢

鍏朵腑鍏煎浜嬩欢缁х画娌跨敤锛?

- `session-start`
- `progress`
- `stage-start`
- `warning`
- `artifact`
- `final-summary`

## 9.1 `progress` 鐨勫畾浣?

姝ｅ紡瑙勫畾锛?

- `progress`
  - 涓嶆槸 canonical event
  - 鍙槸 session compatibility projection

瀹冨瓨鍦ㄧ殑鍞竴涓昏鐞嗙敱鏄細

- 褰撳墠 TUI / watch 渚濊禆瀹冪殑 `completedUnits / totalUnits / activeUnit`

## 9.2 `progress` 鐨勬渶灏?payload

绗竴鐗堟帹鑽愮户缁吋瀹瑰綋鍓嶅瓧娈碉細

```json
{
  "completedUnits": 3,
  "totalUnits": 8,
  "activeUnit": "HelloWorldObject/windows-reference-trace/analysis-frontend",
  "suiteStatus": "ok",
  "subjectId": "HelloWorldObject",
  "matrixId": "windows-reference-trace",
  "stageId": "analysis-frontend"
}
```

杩欓噷淇濈暀 `suiteStatus` 铏界劧鍛藉悕涓嶇悊鎯筹紝浣嗙涓€鐗堜笉寤鸿鏀瑰悕銆?

鍘熷洜锛?

- 褰撳墠 TUI 鐩存帴璇诲彇杩欎釜瀛楁

鏈潵濡傛灉 TUI 瀹屾垚鏂?subject 浜嬩欢閫傞厤锛屽啀鑰冭檻鎶婂畠鍗囩骇鎴愭洿鍚堢悊鐨勫懡鍚嶃€?

## 10. 鍗曞啓鑰呭師鍒?

涓轰簡閬垮厤浜嬩欢椤哄簭閿欎贡锛屾帹鑽愭寮忓浐瀹氾細

- 姣忎釜 `events.jsonl` 鏂囦欢鍙厑璁镐竴涓€昏緫鍐欒€?

鍏蜂綋鏄細

- matrix events
  - 鐢?executor 璐熻矗 append
- subject events
  - 鐢?executor 璐熻矗 append
- session events
  - 鐢?executor / test command orchestration 璐熻矗 append

reporting 涓嶇洿鎺ヨ嚜宸辫拷鍔犱簨浠舵枃浠躲€?

鏇村悎鐞嗙殑妯″紡鏄細

- reporting 鐢熸垚 summary/report 瀵硅薄
- executor 鎷垮埌缁撴灉鍚庣粺涓€鍐欏搴?summary event

杩欐牱涓嶄細鍑虹幇锛?

- executor 鍜?reporting 鍚屾椂寰€鍚屼竴鏂囦欢鎶㈠啓

## 11. 涓?reporting 涓夊眰 schema 鐨勫叧绯?

姝ｅ紡鍥哄畾杩欐潯寮曠敤鍏崇郴锛?

- `matrix-summary` payload
  - 鎸囧悜 matrix `report.json`
- `subject-summary` payload
  - 鎸囧悜 subject `summary.json`
- `final-summary` payload
  - 鎸囧悜 session `summary.json`

鍥犳锛?

- summary/report 鏄ǔ瀹氬璞?
- event 鍙槸鏃堕棿绾垮叆鍙?

event 涓嶈礋璐ｉ噸澶?summary/report 鐨勫叏閮ㄥ瓧娈点€?

## 12. 绗竴鐗堟槑纭笉鍋氱殑浜嬫儏

绗竴鐗堜笉寤鸿鐜板湪灏卞仛锛?

- 涓烘瘡涓?bucket 鍗曠嫭寤虹珛浜嬩欢鏂囦欢
- 鍦?subject events 閲岃浆鍌ㄥ叏閮?stage 浜嬩欢
- 璁?worker 鐩存帴 append 浜嬩欢鏂囦欢
- 璁?session events 鎴愪负 matrix 鎺掗殰鐨勫敮涓€鐪熸簮
- 褰诲簳绉婚櫎 `progress`

杩欎簺瑕佷箞澶粏锛岃涔堜細鎵撴柇褰撳墠鍏煎闈€?

## 13. 褰撳墠鍙互姝ｅ紡鍥哄畾鐨勮鍒?

杩欐鍙互姝ｅ紡鍥哄畾涓猴細

- 浜嬩欢娴侀噰鐢ㄤ笁灞傦細
  - matrix detailed stream
  - subject aggregate stream
  - session compatibility stream
- 閲囩敤鏂规 C锛?
  - matrix / subject canonical
  - session compatibility projection
- `progress` 涓嶆槸 canonical event
- matrix events 鏄?stage 琛屼负鐨勬潈濞佹潵婧?
- subject events 鏄?subject 鑱氬悎杩囩▼鐨勬潈濞佹潵婧?
- session events 涓昏鏈嶅姟 watch/TUI 涓庣粺涓€鍏ュ彛瀵艰埅
- event envelope 闇€瑕佸閲忔敮鎸侊細
  - `streamScope`
  - `subjectId`
  - `matrixId`
  - `goalId`
  - `stageId`
  - `bucket`
  - `stageScope`
- `artifact` 鍙毚闇?surfaced artifacts锛屼笉绌蜂妇 bucket 鍐呭叏閮ㄦ枃浠?
- 姣忎釜浜嬩欢鏂囦欢鍧氭寔鍗曞啓鑰呭師鍒?

## 14. 杩欎竴姝ヤ箣鍚庢渶鑷劧鐨勪笅涓€涓棶棰?

濡傛灉缁х画寰€涓嬫敹鏁涳紝鏈€鑷劧鐨勪笅涓€涓棶棰樻槸锛?

- registry / planner / executor / reporting 杩涘叆姝ｅ紡瀹炴柦鏃讹紝绗竴鎵规ā鍧楀垏鍒嗕笌 API 褰㈢姸濡備綍瀹?

涔熷氨鏄粠鈥滆璁″璞♀€濈户缁蛋鍒帮細

- Python 妯″潡杈圭晫
- 鍛戒护鍏ュ彛 cutover
- 棣栨壒瀹炵幇椤哄簭
