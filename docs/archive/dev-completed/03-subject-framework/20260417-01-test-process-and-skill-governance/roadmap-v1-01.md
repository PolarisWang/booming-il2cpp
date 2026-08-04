# 娴嬭瘯娴佺▼涓?TestFramework Sdk 鍖?Roadmap v1.01

## 1. 鐩爣

鎶婂綋鍓嶆祴璇曚綋绯婚噸鏋勪负浠ヤ笅涓荤嚎锛?

`managed solution -> dotnet 8 collection analysis -> collection files -> managed test project -> native project -> native test project -> hotupdate patch project + hotupdate test host project`

骞跺畬鎴愶細

- `Chaos.TestFramework.Annotation -> Chaos.TestFramework.Sdk`
- `Assert` 涓?subject-side authoring API 鏀舵暃杩?`Sdk`
- `Runtime` 鏀舵暃涓?execution host surface
- native / hotupdate 鎵ц渚х殑 manifest 鍖栫粦瀹?
- 鏃у懡鍚嶃€佹棫娉ㄥ唽閫昏緫銆佹棫鍙岃建鏂囨。鍜屾棫 fixture 鐨勫交搴曟竻鐞?

## 2. 鑼冨洿杈圭晫

鏈?roadmap 璐熻矗锛?

- `Sdk / Runtime / collector / manifest` 鐨勮亴璐ｅ喕缁?
- test host 鐢熸垚涓庢秷璐归摼璺噸鏋?
- roadmap 涓槑纭€滄竻鐞嗘棫鍐欐硶鈥濅笌鈥滃垹闄ゆ棫璧勪骇鈥濈殑闃舵
- 鏂囨。銆亀iki銆乻kill 涓?`docs/dev` 涓诲叆鍙ｆ洿鏂?

鏈?roadmap 涓嶇洿鎺ヨ礋璐ｏ細

- AOT Core IR 鏂拌兘鍔涙墿灞?
- 闈炴祴璇曚富绾夸笟鍔″姛鑳藉紑鍙?
- 涓庢湰娆?Sdk 鍖栨棤鍏崇殑 benchmark 鎸囨爣鎵╁睍

## 3. 闈炵洰鏍?

- 涓嶄繚鐣欓暱鏈?`Annotation` compatibility alias
- 涓嶅厑璁镐繚鐣欌€滄柊鏃?runner 鍙岃建闀挎湡骞跺瓨鈥?
- 涓嶆妸 native 绗﹀彿缁戝畾缁嗚妭濉炶繘 collection file
- 涓嶆妸 patch 瑁呰浇缁戝畾閫昏緫濉炶繘 `Sdk`

## 4. 闃舵鍒楄〃

### Phase 0锛氬懡鍚嶄笌杈圭晫鍐荤粨

- 鍐荤粨 `Sdk / Runtime / collector / manifest` 鍥涘眰杈圭晫
- 鎶?`Annotation -> Sdk` 鏀瑰悕鍐欏叆璁捐銆乺oadmap銆乤rchitecture銆亀iki銆乻kill
- 鍐荤粨鈥滀笉淇濈暀闀挎湡 alias銆佽縼绉诲畬鎴愬嵆鍒犳棫鍐欐硶鈥?

### Phase 1锛歚Sdk` 宸ョ▼涓?subject-side API 鍒囨崲

- `Chaos.TestFramework.Annotation` 鏀逛负 `Chaos.TestFramework.Sdk`
- 鎶?`Assert`銆乣ChaosAssertionException`銆佹渶灏?assertion state 绉诲叆 `Sdk`
- 鎶?subject / managed project 缁熶竴鍒囧埌鍙紩鐢?`Sdk`

### Phase 2锛歚.NET 8` collector 涓?collection schema 鍥哄寲

- 鍥哄寲 `.NET 8` collector 宸ョ▼銆佽緭鍏ヨ緭鍑哄绾﹀拰 schema
- collector 浠?managed project 涓垎鏋?`Sdk` 澹版槑骞剁敓鎴?collection file
- collection 淇濇寔 execution-agnostic锛屼笉娣峰叆 native/hotupdate 缁戝畾缁嗚妭

### Phase 3锛歮anaged test project 鍒囨崲

- generated managed test project 鏀逛负 `Sdk + Runtime + collection file`
- 鍒犻櫎 managed 渚ф墜宸ユ敞鍐屼笌鏃у彂鐜伴€昏緫

### Phase 4锛歯ative test project 鍒囨崲

- 寮曞叆 native dispatch manifest
- native test project 鏀逛负鈥滆杽瀹夸富 + collection + dispatch manifest鈥?
- 鍒犻櫎 native 渚ф棫鍏ュ彛銆佹棫鎵嬪伐娉ㄥ唽鍜屾棫鑰﹀悎鍙戠幇閫昏緫

### Phase 5锛歨otupdate patch / host 鍒囨崲

- hotupdate patch project 鏀逛负鍙紩鐢?`Sdk`
- hotupdate test host project 鏀逛负 `Sdk + Runtime + collection + binding manifest`
- 鍒犻櫎 hotupdate 渚ф棫鎵弿銆佹棫缁戝畾銆佹棫鍙岃建璺緞

### Phase 6锛氭ā鏉垮寲娴嬭瘯涓庣敓鎴愭不鐞?

- Python tests 鍏ㄩ潰妯℃澘鍖?
- runner / manifest / host 鐢熸垚閫昏緫杞?Scriban
- `NativeAotEmitter` 鎸夋枃浠剁骇 Scriban 瑙勫垯缁熶竴

### Phase 7锛氭棫鍐欐硶娓呯悊涓庢敹鍙?

- 鍒犻櫎鏃ч」鐩悕銆佹棫 namespace銆佹棫 alias
- 鍒犻櫎鏃?fixture銆佹棫妯℃澘銆佹棫鏂囨。銆佹棫璇存槑
- 鍒犻櫎宸茬粡琚柊涓荤嚎鏇夸唬鐨勬棫 pipeline 浠ｇ爜

## 5. 姣忛樁娈靛畬鎴愬畾涔?

### Phase 0

- goal: 鍐荤粨鏂扮殑鍛藉悕銆佸紩鐢ㄥ叧绯诲拰瀹夸富杈圭晫
- exit_criteria:
  - 璁捐銆乺oadmap銆乤rchitecture銆亀iki銆乻kill 缁熶竴浣跨敤 `Sdk`
  - 鏂囨。閲屾槑纭?`Assert` 灞炰簬 `Sdk`
  - 鏂囨。閲屾槑纭?native/hotupdate 鐨勫鐞嗘柟寮?
- deliverables:
  - `design-v1-01.md`
  - `roadmap-v1-01.md`
  - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/subject-test-framework-v1/INDEX.md`
  - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/verification-v1/spec.md`
  - `wiki/*`
- dependencies: 鏃?
- open_questions:
  - native dispatch manifest 鐨勬渶缁堝瓧娈电矑搴?
  - hotupdate binding manifest 鐨勬渶缁堝瓧娈电矑搴?

### Phase 1

- goal: 瀹屾垚 `Sdk` 宸ョ▼鍒囨崲鍜?subject-side API 涓嬫矇
- exit_criteria:
  - managed project 鍙紩鐢?`Sdk`
  - `Assert` 涓嶅啀浣嶄簬 `Runtime`
  - 涓嶄繚鐣?`Annotation` 闀挎湡 alias
- deliverables:
  - `Chaos.TestFramework.Sdk`
  - 鏇存柊鍚庣殑 subject 寮曠敤鍏崇郴
  - 杩佺Щ娴嬭瘯
- dependencies: Phase 0
- open_questions:
  - `Sdk` 涓?benchmark authoring API 鏄惁杩橀渶瑕佹瀬绠€涓婁笅鏂囩被鍨?

### Phase 2

- goal: 璁?collection 鐢熸垚閾捐矾鐙珛銆佺ǔ瀹氥€佸彲娴嬭瘯
- exit_criteria:
  - `.NET 8` collector 鍙粠 managed project 鐢熸垚 collection
  - collection schema versioned 涓?enum-first
  - collection 涓笉鍖呭惈 native/hotupdate 涓撴湁缁戝畾淇℃伅
- deliverables:
  - `Chaos.TestFramework.CollectionGen`
  - collection contract tests
- dependencies: Phase 1
- open_questions:
  - build output 鍒嗘瀽涓?source/project 鍒嗘瀽鐨勬渶缁堥€夋嫨

### Phase 3

- goal: managed 鎵ц渚у垏鍒?collection-only
- exit_criteria:
  - managed test project 涓嶅啀鎵弿 `Sdk`
  - managed test project 涓嶅啀缁存姢鏃ф敞鍐岃〃
- deliverables:
  - managed test host generator
  - managed runtime contract tests
- dependencies: Phase 2
- open_questions: 鏃?

### Phase 4

- goal: native 鎵ц渚у垏鍒拌杽瀹夸富妯″紡
- exit_criteria:
  - native test project 鍙緷璧?collection + native dispatch manifest
  - native 渚ф棤鏃ф墜宸ユ敞鍐岄€昏緫
- deliverables:
  - native dispatch manifest generator
  - native host templates
  - native contract tests
- dependencies: Phase 2
- open_questions:
  - native dispatch manifest 鐨勭敓鎴愭椂鏈烘斁鍦?codegen 闃舵杩樻槸 host generation 闃舵

### Phase 5

- goal: hotupdate 鎵ц渚у垏鍒?patch/host 鍒嗙
- exit_criteria:
  - patch project 鍙紩鐢?`Sdk`
  - host project 寮曠敤 `Sdk + Runtime`
  - hotupdate 渚ф棤鏃ф壂鎻?鏃х粦瀹氶€昏緫
- deliverables:
  - hotupdate binding manifest
  - hotupdate host templates
  - hotupdate contract tests
- dependencies: Phase 2
- open_questions:
  - patch 鍏ュ彛缁戝畾鐨勬渶灏忕ǔ瀹氭爣璇嗛€夋嫨

### Phase 6

- goal: 瀹屾垚妯℃澘鍖栨祴璇曞拰鐢熸垚娌荤悊
- exit_criteria:
  - `tests/templates/` 瑕嗙洊 managed/native/hotupdate 涓荤嚎
  - 鐢熸垚鏁存枃浠堕粯璁よ蛋 Scriban
  - `NativeAotEmitter` 绾冲叆缁熶竴瑙勫垯
- deliverables:
  - Python templates
  - Scriban templates
  - 鐢熸垚閾捐矾娴嬭瘯
- dependencies: Phase 3, Phase 4, Phase 5
- open_questions: 鏃?

### Phase 7

- goal: 娓呯悊鏃у啓娉曪紝淇濇寔浠撳簱骞插噣
- exit_criteria:
  - 鍒犻櫎鏃?`Annotation` 鍛藉悕
  - 鍒犻櫎 compatibility alias
  - 鍒犻櫎鏃?runner/registry/fixture/doc
  - 浠ｇ爜搴撳彧淇濈暀鏂颁富绾?
- deliverables:
  - cleanup checklist 瀹屾垚璁板綍
  - 鍒犻櫎娓呭崟
  - 鍥炲綊娴嬭瘯璇佹嵁
- dependencies: Phase 1, Phase 3, Phase 4, Phase 5, Phase 6
- open_questions: 鏃?

## 6. 瀛愪换鍔℃槧灏?

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260417-01-phase-0-sdk-boundary-freeze` | Phase 0 | completed | codex | 鍐荤粨鍛藉悕銆佽竟鐣屻€乶ative/hotupdate 澶勭悊鏂瑰紡 | `-` |
| `20260417-02-phase-1-sdk-cutover` | Phase 1 | completed | codex | 瀹屾垚 `Annotation -> Sdk` 涓?subject-side API 涓嬫矇 | `20260417-01-phase-0-sdk-boundary-freeze` |
| `20260417-03-phase-2-collector-and-schema` | Phase 2 | completed | codex | 瀹屾垚 `.NET 8` collector 涓?collection schema 鍥哄寲 | `20260417-02-phase-1-sdk-cutover` |
| `20260417-04-phase-3-managed-test-host-cutover` | Phase 3 | completed | codex | 瀹屾垚 managed test project collection-only 鍒囨崲 | `20260417-03-phase-2-collector-and-schema` |
| `20260417-05-phase-4-native-test-host-cutover` | Phase 4 | completed | codex | 瀹屾垚 native 钖勫涓讳笌 dispatch manifest 鍒囨崲 | `20260417-03-phase-2-collector-and-schema` |
| `20260417-06-phase-5-hotupdate-host-cutover` | Phase 5 | completed | codex | 瀹屾垚 hotupdate patch/host 鍒嗙涓?binding manifest | `20260417-03-phase-2-collector-and-schema` |
| `20260417-07-phase-6-template-and-scriban-governance` | Phase 6 | completed | codex | 瀹屾垚妯℃澘鍖栨祴璇曞拰 Scriban 缁熶竴鐢熸垚 | `20260417-04-phase-3-managed-test-host-cutover, 20260417-05-phase-4-native-test-host-cutover, 20260417-06-phase-5-hotupdate-host-cutover` |
| `20260417-08-phase-7-legacy-cleanup-and-cutover-removal` | Phase 7 | completed | codex | 鍒犻櫎鏃у啓娉曘€佹棫鍛藉悕銆佹棫鍏ュ彛涓庢棫 fixture | `20260417-02-phase-1-sdk-cutover, 20260417-04-phase-3-managed-test-host-cutover, 20260417-05-phase-4-native-test-host-cutover, 20260417-06-phase-5-hotupdate-host-cutover, 20260417-07-phase-6-template-and-scriban-governance` |

## 7. 渚濊禆

- `.NET 8` SDK 鍙ǔ瀹氳繍琛?collector
- 褰撳墠 subject / test / generator 浠ｇ爜鍙鍒嗛樁娈靛垏鎹?
- `Runtime` 鐨?managed host 鍗忚鍙互琚?native / hotupdate 瀹夸富澶嶇敤涓虹瓑浠风粨鏋滃崗璁?

## 8. 椋庨櫓

- 濡傛灉淇濈暀 `Annotation` alias锛屽悗缁細闀挎湡缁存寔鍙屽懡鍚嶆薄鏌?
- 濡傛灉鎶?native / hotupdate 缁戝畾淇℃伅濉炶繘 collection锛宻chema 浼氳鎵ц缁嗚妭姹℃煋
- 濡傛灉涓嶆妸 cleanup 鍗曞垪闃舵锛屾棫鍏ュ彛鍜屾棫 fixture 浼氭畫鐣欏苟缁х画琚紩鐢?
- 濡傛灉 `Assert` 浠嶇暀鍦?`Runtime`锛宻ubject 渚т細缁х画琚揩澶氬紩鐢ㄦ垨缁存寔閿欒杈圭晫

## 9. 澶囬€夎矾寰?

### 鏂规 A锛歚Sdk` 鎵挎媴 subject-side authoring API锛宍Runtime` 鎵挎媴鎵ц瀹夸富

- 浼樼偣锛氭渶绗﹀悎鈥渕anaged project 鍙紩鐢ㄤ竴涓?DLL鈥濈殑鐩爣
- 缂虹偣锛氶渶瑕佷竴娆℃€ц縼绉诲懡鍚嶅拰鏂█浣嶇疆

### 鏂规 B锛氫繚鐣?`Annotation + Runtime`锛屽啀缁?subject 棰濆寮曞叆 assertion 绋嬪簭闆?

- 浼樼偣锛氳涔夋洿缁?
- 缂虹偣锛氳繚鍙嶁€渕anaged project 鍙寘鍚竴涓祴璇?DLL鈥濈殑鐩爣

褰撳墠閲囩敤锛氭柟妗?A銆?

## 10. 褰撳墠寤鸿鎺ㄨ繘椤哄簭

1. 鍏堝畬鎴?Phase 0锛屽喕缁?`Sdk`銆乶ative/hotupdate 瀹夸富澶勭悊鍜?cleanup 鍘熷垯
2. 绔嬪嵆鎺ㄨ繘 Phase 1锛屾妸閿欒鐨?`Assert` 杈圭晫绾犳杩囨潵
3. 鍐嶆帹杩?Phase 2锛屾妸 collector 鍜?collection 濂戠害鍥哄畾
4. 鐒跺悗骞惰鍑嗗 Phase 3 / 4 / 5
5. 鏈€鍚庣粺涓€鍋?Phase 6 鍜?Phase 7锛岀‘淇濇ā鏉夸笌鏃у啓娉曚竴璧锋敹鍙?

## 11. 娓呯悊鏃у啓娉?Checklist

- [x] 鍒犻櫎 `Chaos.TestFramework.Annotation` 椤圭洰鍚嶃€佺洰褰曞悕銆乶amespace
- [x] 鍒犻櫎鏃ф枃妗ｄ腑鐨?`Annotation` 鍙ｅ緞
- [x] 鍒犻櫎 `Runtime.Assert` 鎴栫瓑浠锋棫鏂█鍏ュ彛
- [x] 鍒犻櫎 managed/native/hotupdate 鐨勬墜宸ユ祴璇曟敞鍐岄€昏緫
- [x] 鍒犻櫎鏃?runner discovery path
- [x] 鍒犻櫎鏃?compatibility alias
- [x] 鍒犻櫎宸茶妯℃澘鏇夸唬鐨勬棫 fixture / old generated sample
- [x] 鍒犻櫎宸茶 manifest 鏈哄埗鏇夸唬鐨勬棫缁戝畾閫昏緫
- [x] 鍒犻櫎鏃?wiki / 鏃?roadmap / 鏃ц鏄庝腑鐨勫弻杞ㄨ娉?
