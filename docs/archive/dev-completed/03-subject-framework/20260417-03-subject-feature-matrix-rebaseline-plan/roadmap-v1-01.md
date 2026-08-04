# Subject Feature Matrix Rebaseline Roadmap v1.01

Date: 2026-04-17
Status: proposed

## 1. 鐩爣

鎶婂綋鍓?`subjects/` 涓凡缁忓畬鎴愮殑 AOT / Mixed Execution / HotUpdate 鑳藉姏锛屾寜鐜拌缁熶竴娴嬭瘯妗嗘灦閲嶆瀯涓轰竴濂楁竻鏅般€佸彲鎵╁睍銆佸彲鑷姩鏍￠獙鐨勯獙璇佷綋绯伙細

- feature 鏈夊敮涓€ owner subject
- feature 鏈?machine-readable capability matrix
- proof / benchmark / hotupdate proof 鍏ㄩ儴鐢?`Chaos.TestFramework.Sdk` attribute 澹版槑
- collector 缁熶竴鐢熸垚 collection file
- managed / native / hotupdate 鍏ㄩ儴娑堣垂鍚屼竴浠?collection
- 宸ョ▼绾?solution 鏍蜂緥鍘嬬缉涓烘渶灏忓繀瑕侀泦鍚?
- 鎵嬪伐 proof/benchmark 鑱氬悎鍏ュ彛涓庢棫 `validation` 鏃佽矾琚交搴曞垹闄?
- 鏈€缁堣兘澶熶竴閿洖绛斺€滃凡瀹屾垚鑳藉姏鏄惁琚畬鏁撮獙璇佲€?

## 2. 鑼冨洿杈圭晫

鏈?roadmap 璐熻矗锛?

- 鐩樼偣骞跺喕缁撳凡瀹屾垚 feature inventory
- 鍘嬬缉 archetype/engineering scenario 鍒版渶灏忓繀瑕侀泦鍚?
- 璋冩暣 subject 鐩綍鍐呮寮忛獙璇佽祫浜х殑缁勭粐鏂瑰紡
- 鍒犻櫎鎵嬪伐鑱氬悎鍏ュ彛鍜屾棫楠岃瘉璺緞
- 鎶婂凡瀹屾垚 feature 閫愭潯閲嶆帴鍒?collection 涓荤嚎
- 瀵归綈娴嬭瘯銆乵anifest銆乸lanner銆乺egistry銆亀orkspace銆乨ashboard 瀵?subject 缁撴瀯鐨勬秷璐规柟寮?
- 杈撳嚭瀹屾暣鍥炲綊涓?coverage gap

鏈?roadmap 涓嶈礋璐ｏ細

- 鏂板灏氭湭瀹炵幇鐨?AOT Core IR 鑳藉姏
- 鏂板灏氭湭瀹炵幇鐨?HotUpdate 鑳藉姏
- 閲嶅仛鏁翠釜娴嬭瘯妗嗘灦澶ф灦鏋?
- 闈炴祴璇曚富绾跨殑涓氬姟鍔熻兘寮€鍙?

## 3. 闈炵洰鏍?

- 涓嶄繚鐣欌€滄柊鏃у弻杞ㄥ苟瀛樷€濈殑闀挎湡鍏煎鐘舵€?
- 涓嶇户缁墿澶?`subject.manifest.json` 涓殑鎵嬪伐 proof/benchmark 鍏ュ彛鍗忚
- 涓嶇户缁緷璧?`Program::Main + Console.WriteLine` 浣滀负姝ｅ紡 correctness 鍒ゅ畾鏂瑰紡
- 涓嶆妸 capability matrix 浠呭仛鎴愪竴浠戒汉宸ユ枃妗?

## 4. 鐩爣缁撴瀯

### 4.1 Subject 鐩爣缁撴瀯

鎺ㄨ崘鐩爣缁撴瀯锛?

```text
subjects/
  SolutionCorePack/
    source/
      EngineeringScenarios/
      FeatureSlices/
      Benchmarks/
    subject.manifest.json
    subject.features.json

  MixedExecutionFeaturePack/
    source/
      EngineeringScenarios/
      ManagedBridge/
      Lowering/
    subject.manifest.json
    subject.features.json

  HotUpdateHostPack/
    source/
      EngineeringScenarios/
      Host/
      PatchModules/
      SharedContracts/
    subject.manifest.json
    subject.features.json
```

璇存槑锛?

- `EngineeringScenarios/` 鏄褰撳墠 `Archetypes/` 鐨勬帹鑽愭敼鍚?
- `subject.features.json` 浣滀负姣忎釜 owner subject 鐨?machine-readable capability matrix
- `subject.manifest.json` 鏀舵暃涓?subject/source/pipeline/matrix 鎻忚堪锛屼笉鍐嶆壙鎷呭叿浣?proof/benchmark 鍏ュ彛鐪熸簮

### 4.2 Archetype 鏈€灏忓繀瑕侀泦鍚?

鏈€缁堝缓璁繚鐣欙細

- `SolutionCorePack`
  - `MultiProjectSolution`
  - `ReferenceAssemblySolution`
  - `CoreLibReferenceSolution`
  - `MixedReferenceClosureSolution`
- `MixedExecutionFeaturePack`
  - `MixedBridgeSolution`
- `HotUpdateHostPack`
  - `FullProjectHotUpdateSolution`

鏈€缁堝缓璁垹闄わ細

- `SolutionCorePack/SimpleLibrarySolution`
- `SolutionCorePack/PackageReferenceSolution`

### 4.3 姝ｅ紡楠岃瘉璧勪骇鐨勫敮涓€闆嗗悎

姣忎釜 feature 鏈€缁堝彧鍏佽閫氳繃浠ヤ笅璧勪骇琛ㄨ揪锛?

- `tests/unit/**`
- `tests/contracts/**`
- owner managed proof
- owner native proof
- 鎸夎鍒欒姹傜殑 hotupdate smoke / proof
- 鎸夐渶 benchmark

浠ヤ笅閮藉繀椤婚€€鍑烘寮忎富绾匡細

- `DefaultProofEntries`
- 鎵嬪伐 `switch` proof 璺敱
- `ProofEntry::Run()` 浣滀负闀挎湡鑱氬悎鐪熸簮
- `BenchmarkEntry::RunWorkload()` 鐨勬墜宸ラ厤缃湡婧?
- `subjects/*/validation/*` 鏃佽矾楠岃瘉鐩綍

## 5. 闃舵鍒楄〃

### Phase 0: Authority Freeze And Inventory Baseline

鐩爣锛?

- 鍐荤粨 feature inventory
- 鍐荤粨 archetype/engineering scenario inventory
- 鍐荤粨 capability matrix / collection / manifest 涓夎€呰竟鐣?
- 鍐荤粨 archetype 鏈€灏忓繀瑕侀泦鍚?

Checklist锛?

- 鍒楀嚭鎵€鏈夊凡瀹屾垚 AOT / Mixed / HotUpdate feature
- 涓烘瘡涓?feature 鎸囧畾锛?
  - `ownerSubjectId`
  - `capabilityFamily`
  - `capabilityItem`
  - `managed/native/hotupdate` 瑕嗙洊瑕佹眰
  - `benchmark requirement`
  - `engineering scenario coverage`
- 鍒楀嚭鎵€鏈夌幇鏈?archetype
- 瀵规瘡涓?archetype 鏍囪锛?
  - `keep`
  - `merge`
  - `delete`
- 鏄庣‘鍝簺鍦烘櫙浠嶄复鏃朵緷璧?`Program::Main`
- 鏄庣‘ manifest 涓摢浜涘瓧娈靛繀椤婚€€鍑衡€滄祴璇曠湡婧愨€濊鑹?

缁撴瀯璋冩暣锛?

- 鏂板 `subject.features.json` 濂戠害璁捐
- 璇勪及 `Archetypes -> EngineeringScenarios` 鍛藉悕鍒囨崲

楠屾敹鏍囧噯锛?

- 姣忎釜宸插畬鎴?feature 閮芥湁鍞竴 matrix 琛?
- 姣忎釜 engineering scenario 閮芥湁鍞竴淇濈暀/鍒犻櫎鍐崇瓥
- 鍥㈤槦鑳芥槑纭洖绛旓細
  - collection 鏄粈涔堢湡婧?
  - manifest 鏄粈涔堢湡婧?
  - capability matrix 鏄粈涔堢湡婧?

### Phase 1: Subject Entry And Scenario Cutover

鐩爣锛?

- 鍒犻櫎鎵嬪伐 proof/benchmark 鑱氬悎鍏ュ彛
- 鍘嬬缉 archetype 鍒版渶灏忓繀瑕侀泦鍚?
- 灏介噺鎶婁繚鐣欎笅鏉ョ殑 engineering scenario 鍒囧埌 `Sdk + Assert + collection`
- 鏀剁缉 manifest 鐨勬墜宸?proof/benchmark 鍏ュ彛鍗忚

Checklist锛?

- 鍒犻櫎 `DefaultProofEntries`
- 鍒犻櫎鎵嬪伐 `RunAll(...)`
- 鍒犻櫎鎵嬪伐 proof `switch`
- 鍒犻櫎涓嶅啀淇濈暀鐨?archetype 鐩綍
- 鏇存柊淇濈暀 archetype 鐨勯」鐩紩鐢ㄣ€乻olution銆佽矾寰?
- 鎶婁繚鐣?engineering scenario 涓殑姝ｅ紡楠岃瘉杩佺Щ鍒?`ChaosUnitTest` / `ChaosBenchmark`
- 璋冩暣 `subject.manifest.json`
  - 鍘绘帀涓嶅啀闇€瑕佺殑 `entry` / `workloadEntry` / `entrySelection`
  - 淇濈暀 source solution / matrix / pipeline / default set 绾т俊鎭?
- 鏇存柊 planner / registry / workspace / subject schema 鐨勭浉鍏虫祴璇?

缁撴瀯璋冩暣锛?

- `Archetypes/` 閲嶅懡鍚嶄负 `EngineeringScenarios/`
- 娓呯悊 `subjects/SolutionCorePack/validation/`

楠屾敹鏍囧噯锛?

- 鏂板 feature 涓嶅啀闇€瑕佹敼鎵嬪伐鍏ュ彛
- 宸ョ▼绾ф牱渚嬪彧淇濈暀鏈€灏忓繀瑕侀泦鍚?
- 缁濆ぇ澶氭暟姝ｅ紡鍦烘櫙宸茬粡涓嶄緷璧?`Program::Main + Console`
- manifest 涓嶅啀鏄?proof/benchmark 鍏蜂綋鍏ュ彛鐪熸簮

### Phase 2: SolutionCorePack AOT Rebaseline

鐩爣锛?

- 鎸?capability matrix 閲嶅缓 `SolutionCorePack` 宸插畬鎴?AOT feature 鐨勬寮忛獙璇侀潰

Checklist锛?

- 閫愪釜鑳藉姏鍩熻ˉ榻愭垨閲嶅啓锛?
  - `PrimitivesAndOps`
  - `ObjectModelAndDispatch`
  - `GenericsAndCollections`
  - `ExceptionsAndControlFlow`
  - `InteropAndMarshaling`
  - `ReflectionAndMetadata`
  - `RuntimeServices`
  - `AsyncAndThreading`
- 涓烘瘡鏉?completed feature 瀵归綈锛?
  - `tests/unit` / `tests/contracts`
  - managed proof
  - native proof
  - 鎸夐渶 benchmark
- 鎶婂伐绋嬬骇 scenario 涓?feature matrix 鍏宠仈璧锋潵
- 娓呯悊閲嶅銆佹紓绉绘垨澶辨晥 proof

缁撴瀯璋冩暣锛?

- 鎸?capability family 娓呯悊 `FeatureSlices` 涓?`Benchmarks` 鍐呯殑鍛藉悕鍜屽綊灞?
- 瀵归綈 collection 涓殑 stableId 鍛藉悕

楠屾敹鏍囧噯锛?

- 姣忔潯 completed AOT feature 閮借兘浠?matrix 杩藉埌姝ｅ紡楠岃瘉璧勪骇
- 涓嶅瓨鍦ㄢ€滃彧鍦ㄦ枃浠堕噷鏈?proof锛屼絾涓嶅湪姝ｅ紡鍥炲綊涓€濈殑婕傜Щ椤?
- AOT 宸ョ▼绾ч獙璇佽绾冲叆姝ｅ紡 collection 涓荤嚎锛屾垨鍦?matrix 涓槑纭爣娉ㄤ负涓存椂 smoke

### Phase 3: MixedExecution And HotUpdate Rebaseline

鐩爣锛?

- 鎸?capability matrix 閲嶅缓 `MixedExecutionFeaturePack` 涓?`HotUpdateHostPack` 宸插畬鎴愯兘鍔涚殑姝ｅ紡楠岃瘉闈?

Checklist锛?

- `MixedExecutionFeaturePack`
  - interpreter lowering
  - bridge dispatch
  - mixed generic flow
  - mixed exception flow
  - mixed delegate flow
- `HotUpdateHostPack`
  - skeleton/load
  - metadata supplement
  - method replacement
  - shared contract
  - patch integrity
  - rollback/versioning
- 閫愭潯纭锛?
  - owner managed proof
  - owner native proof 鎴?mixed/native equivalent
  - required hotupdate smoke / proof
  - 鎸夐渶 benchmark

缁撴瀯璋冩暣锛?

- 瀵归綈 patch/host/shared-contract 鏍蜂緥鐨勬寮忛獙璇佽鑹?
- 娓呯悊浠嶇暀鍦?Host/Program 鎴?ManagedBridge/Program 鐨勬墜宸ュ垎鍙?

楠屾敹鏍囧噯锛?

- 姣忔潯 completed mixed/hotupdate feature 閮借繘鍏ユ寮?collection 涓荤嚎
- 鍛戒腑 hotupdate 瑙勫垯鐨?feature 鍏ㄩ儴鏈?hotupdate 楠岃瘉
- mixed / hotupdate 鐨勫伐绋嬬骇鏍蜂緥涓?capability matrix 褰㈡垚涓€涓€瀵瑰簲

### Phase 4: Coverage Automation And Full Regression

鐩爣锛?

- 璁?capability matrix 鍙嚜鍔ㄦ牎楠?
- 璺戝畬鏁村洖褰?
- 杈撳嚭 coverage 涓?gap 鎶ヨ〃

Checklist锛?

- 寤虹珛 matrix 涓?collection 鐨勮嚜鍔ㄥ鐓ф鏌?
- 寤虹珛 matrix 涓?benchmark 瑕嗙洊鐨勮嚜鍔ㄥ鐓ф鏌?
- 寤虹珛 matrix 涓?hotupdate requirement 鐨勮嚜鍔ㄥ鐓ф鏌?
- 杩愯瀹屾暣 managed / native / hotupdate 鍥炲綊
- 浜у嚭锛?
  - feature coverage report
  - benchmark coverage report
  - engineering scenario coverage report
  - remaining gap report

缁撴瀯璋冩暣锛?

- 鏂板 coverage report 鐢熸垚閫昏緫
- 鏇存柊 dashboard / reporting锛屼娇鍏惰兘鏄剧ず capability 绾ц鍥?

楠屾敹鏍囧噯锛?

- 鑳借嚜鍔ㄥ彂鐜帮細
  - declared but not covered
  - covered but not in matrix
  - benchmark required but missing
  - hotupdate proof required but missing
- 鑳芥槑纭洖绛斺€滃凡瀹屾垚鑳藉姏鏄惁瀹屾暣楠岃瘉鈥?

### Phase 5: Documentation And Governance Closeout

鐩爣锛?

- 鎶婃渶缁堣鍒欏浐鍖栧埌 architecture / wiki / skill

Checklist锛?

- 更新 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/subject-test-framework-v1/INDEX.md`
- 更新 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/verification-v1/spec.md`
- 鏇存柊 `wiki/04-宸ュ叿涓庨泦鎴?缁熶竴娴嬭瘯妗嗘灦.md`
- 鏇存柊 `wiki/06-娴嬭瘯楠岃瘉/鏂板娴嬭瘯鎺ュ叆瑙勮寖.md`
- 鏇存柊 `wiki/06-娴嬭瘯楠岃瘉/AOT鏂癋eature鎺ュ叆鑷祴瑙勮寖.md`
- 琛ュ厖锛?
  - owner feature matrix 瑙勫垯
  - engineering scenario 瑙勫垯
  - manifest / collection / matrix 鐪熸簮杈圭晫

楠屾敹鏍囧噯锛?

- 鏂板紑鍙戣€呬粎鐪?wiki/architecture 灏辫兘姝ｇ‘鎺ュ叆鏂?feature
- 涓嶅啀闇€瑕佸彛澶磋В閲?`Archetypes`銆佹墜宸ュ叆鍙ｃ€佹棫楠岃瘉璺緞鐨勫巻鍙茶儗鏅?

## 6. 姣忛樁娈靛畬鎴愬畾涔?

### Phase 0

- goal: 鍐荤粨 inventory銆佺湡婧愯竟鐣屻€佹渶灏?archetype 闆嗗悎
- exit_criteria:
  - `subject.features.json` 缁撴瀯瀹氱
  - archetype 淇濈暀/鍒犻櫎鍐崇瓥瀹氱
  - collection / manifest / matrix 鏉冭矗瀹氱
- deliverables:
  - feature inventory
  - archetype inventory
  - matrix schema 鑽夋
- dependencies: 鏃?
- open_questions:
  - `subject.features.json` 鏀炬瘡涓?subject 杩樻槸鏀句粨搴撴牴閮ㄨ仛鍚?
  - `Archetypes` 鏄惁鏈疆鏀瑰悕涓?`EngineeringScenarios`

### Phase 1

- goal: 鍒囨帀鎵嬪伐鍏ュ彛骞跺帇缂?engineering scenarios
- exit_criteria:
  - 鏃?`DefaultProofEntries`
  - 鏃犳墜宸?proof/benchmark 鑱氬悎鐪熸簮
  - 鏈€灏忓繀瑕?engineering scenario 闆嗗悎钀藉湴
- deliverables:
  - 绮剧畝鍚庣殑 subject 鍏ュ彛
  - 鏇存柊鍚庣殑 manifest
  - 鏇存柊鍚庣殑 tests
- dependencies: Phase 0
- open_questions:
  - 灏戦噺纭疄涓嶈兘绔嬪埢 collection 鍖栫殑 scenario 濡備綍涓存椂鏍囪

### Phase 2

- goal: SolutionCorePack 褰㈡垚瀹屾暣 AOT owner 楠岃瘉闈?
- exit_criteria:
  - completed AOT feature 鍏ㄩ儴鏄犲皠鍒版寮忛獙璇佽祫浜?
  - 宸ョ▼绾?AOT scenario 杩涘叆姝ｅ紡涓荤嚎鎴栨槑纭爣涓存椂鐘舵€?
- deliverables:
  - 閲嶅熀绾垮悗鐨?`SolutionCorePack`
  - 瀵瑰簲 unit/contracts/integration 璇佹嵁
- dependencies: Phase 1
- open_questions: 鏃?

### Phase 3

- goal: Mixed/HotUpdate owner 楠岃瘉闈㈠畬鏁村寲
- exit_criteria:
  - completed mixed/hotupdate feature 鍏ㄩ儴杩涘叆姝ｅ紡涓荤嚎
  - required hotupdate validation 瀹屾暣瀛樺湪
- deliverables:
  - 閲嶅熀绾垮悗鐨?`MixedExecutionFeaturePack`
  - 閲嶅熀绾垮悗鐨?`HotUpdateHostPack`
- dependencies: Phase 1
- open_questions: 鏃?

### Phase 4

- goal: 璁?coverage 鍙嚜鍔ㄥ璁★紝骞跺畬鎴愬叏閲忓洖褰?
- exit_criteria:
  - coverage 宸窛鍙互鑷姩鎶ュ憡
  - managed/native/hotupdate 瀹屾暣鍥炲綊鍙璺?
- deliverables:
  - coverage automation
  - reports
- dependencies: Phase 2, Phase 3
- open_questions:
  - dashboard 鏄惁鍚屾椂灞曠ず capability 绾у拰宸ョ▼ scenario 绾ц鍥?

### Phase 5

- goal: 鏂囨。銆亀iki銆乻kill 涓庢渶缁堢姸鎬佷竴鑷?
- exit_criteria:
  - architecture / wiki / skill 鍏ㄩ儴鍚屾
- deliverables:
  - 鏇存柊鍚庣殑闀挎湡鏂囨。
- dependencies: Phase 4
- open_questions: 鏃?

## 7. 瀛愪换鍔℃槧灏?

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260417-03-phase-0-authority-freeze-and-inventory-baseline` | Phase 0 | planned | codex | 鍐荤粨 feature/archetype inventory 涓庣湡婧愯竟鐣?| `-` |
| `20260417-03-phase-1-subject-entry-and-scenario-cutover` | Phase 1 | planned | codex | 鍒犻櫎鎵嬪伐鍏ュ彛骞跺帇缂?engineering scenario 闆嗗悎 | `20260417-03-phase-0-authority-freeze-and-inventory-baseline` |
| `20260417-03-phase-2-solution-core-pack-aot-rebaseline` | Phase 2 | planned | codex | 閲嶅缓 SolutionCorePack 鐨?owner AOT 楠岃瘉闈?| `20260417-03-phase-1-subject-entry-and-scenario-cutover` |
| `20260417-03-phase-3-mixed-hotupdate-rebaseline` | Phase 3 | planned | codex | 閲嶅缓 MixedExecution/HotUpdate 鐨?owner 楠岃瘉闈?| `20260417-03-phase-1-subject-entry-and-scenario-cutover` |
| `20260417-03-phase-4-coverage-automation-and-full-regression` | Phase 4 | planned | codex | 寤虹珛 capability coverage 鑷姩瀹¤骞跺畬鎴愬叏閲忓洖褰?| `20260417-03-phase-2-solution-core-pack-aot-rebaseline, 20260417-03-phase-3-mixed-hotupdate-rebaseline` |
| `20260417-03-phase-5-documentation-and-governance-closeout` | Phase 5 | planned | codex | 鍥哄寲鏈€缁堣鑼冨埌 architecture/wiki/skill | `20260417-03-phase-4-coverage-automation-and-full-regression` |

## 8. 渚濊禆

- 鐜版湁 `Chaos.TestFramework.Sdk / Runtime / collector / manifest` 涓荤嚎宸茬粡鍙敤
- subject discovery / registry / workspace 鍏峰缁х画婕旇繘绌洪棿
- 褰撳墠 3 涓?canonical subject 宸茬粡瀛樺湪锛屽彲浣滀负 owner 鏀跺彛瀹瑰櫒
- 鍚庣画 AOT 涓荤嚎鑳藉姏琛ユ礊鍙互骞惰鎺ㄨ繘锛屼絾涓嶅簲娣锋穯鏈?roadmap 鐨勯獙璇侀噸姊崇洰鏍?

## 9. 椋庨櫓

- 濡傛灉涓嶆敹缂?manifest 鐨勬墜宸ュ叆鍙ｅ崗璁紝collection 涓荤嚎浠嶇劧涓嶆槸鍞竴鐪熸簮
- 濡傛灉 capability matrix 鍙槸鏂囨。锛屼笉鍋氳嚜鍔ㄦ牎楠岋紝寰堝揩浼氬啀娆℃紓绉?
- 濡傛灉 archetype 鍙仛鐩綍鍘嬬缉銆佷笉鏀归獙璇佹柟寮忥紝浠嶄細鐣欎笅 `Program + Console` 鐨勬棫杞?
- 濡傛灉涓嶅湪鏈疆鍒犻櫎鏃?`validation/*` 涓庢墜宸ュ叆鍙ｏ紝鍚庣画鏂板 feature 浼氱户缁弻杞?
- 濡傛灉 coverage automation 鏈€鍚庢墠鎯宠捣鏉ヨˉ锛孭hase 2/3 鐨勭粨鏋滃緢闅捐瘉鏄庘€滃畬鏁粹€?

## 10. 澶囬€夎矾寰?

### 鏂规 A锛氳ˉ鐢ㄤ緥浣嗕笉鏀圭湡婧愮粨鏋?

- 浼樼偣锛氬揩
- 缂虹偣锛氬悗缁户缁弻杞紝绛変簬娌″畬鎴愰噸鏋?

### 鏂规 B锛氳兘鍔涚煩闃甸噸鍩虹嚎骞跺悓姝ュ垏鐪熸簮

- 浼樼偣锛氭渶绗﹀悎褰撳墠娴嬭瘯妗嗘灦
- 缂虹偣锛氬垵濮嬫⒊鐞嗘垚鏈洿楂?

### 鏂规 C锛氭暣涓?subject 浣撶郴鍏ㄩ噺閲嶇敓

- 浼樼偣锛氭渶骞插噣
- 缂虹偣锛氶闄╄繃澶э紝瀹规槗鎶婂凡瀹屾垚璧勪骇鎵撴暎

褰撳墠閲囩敤锛氭柟妗?B銆?

## 11. 褰撳墠寤鸿鎺ㄨ繘椤哄簭

1. 鍏堝仛 `Phase 0`锛屽喕缁撶湡婧愯竟鐣屽拰鏈€灏?engineering scenario 闆嗗悎
2. 鍐嶅仛 `Phase 1`锛屽厛鍒囨帀鏈€鍏抽敭鐨勬墜宸ュ叆鍙ｅ拰鏃?scenario 娈嬬暀
3. 浼樺厛鎺ㄨ繘 `Phase 2`锛屾妸 `SolutionCorePack` 杩欎釜 AOT 涓?owner 鍏堟墦绋?
4. 鍐嶆帹杩?`Phase 3`锛岃ˉ榻?mixed/hotupdate owner 闈?
5. 鏈€鍚庣敤 `Phase 4` 鍋氳嚜鍔ㄥ璁″拰鍏ㄩ噺鍥炲綊
6. 鏀跺彛鍒?`Phase 5`锛屾妸瑙勫垯鍥哄寲鍒伴暱鏈熸枃妗?

## 12. 閲嶅缁撹

杩欎唤 roadmap 鍦ㄩ噸瀹″悗锛屾瘮鍘熻璁℃洿鍚堢悊鐨勫湴鏂规湁涓夌偣锛?

- 涓嶅啀榛樿鎺ュ彈 `Archetypes/*` 缁存寔鏃?console smoke 瑙掕壊锛岃€屾槸瑕佹眰灏介噺鍥炲埌缁熶竴 collection 涓荤嚎銆?
- 涓嶅啀鎶?capability matrix 褰撹鏄庢枃妗ｏ紝鑰屾槸鎶婂畠鎻愬崌涓?machine-readable 濂戠害锛屽悗缁兘鑷姩瀵圭収 collection銆?
- 涓嶅啀榛樿璁?`subject.manifest.json` 缁х画鎵胯浇 proof/benchmark 鍏蜂綋鍏ュ彛鐪熸簮锛岃€屾槸鏄庣‘瑕佹眰鏀剁缉杩欏眰鍗忚銆?

褰撳墠浠嶉渶娉ㄦ剰浣嗗凡绾冲叆 roadmap 鐨勫敮涓€楂橀闄╃偣鏄細

- 杩欎竴杞笉鍙槸鍐呭琛ラ綈锛岃€屾槸涓€娆?manifest / tests / planner / registry / workspace 鐨勭湡瀹?cutover锛屾墍浠?Phase 1 鐨勬敼鍔ㄩ潰浼氭瘮琛ㄩ潰鐪嬭捣鏉ュぇ銆?
