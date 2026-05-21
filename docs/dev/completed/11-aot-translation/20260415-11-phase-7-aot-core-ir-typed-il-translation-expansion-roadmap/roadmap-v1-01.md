# Phase 7 AOT Core IR Typed-IL Translation Expansion Roadmap v1.01

Date: 2026-04-15 20:11:23 +08:00
Status: completed

## 1. 鐩爣

鍦ㄥ凡瀹屾垚鐨?HybridCLR 瀵归綈涓婚摼璺熀纭€涓婏紝缁х画鎵╁ぇ锛?
- `typed-il` 鍙〃杈剧殑鐪熷疄缈昏瘧杈撳叆闈?- `AotCoreIr` 鍙ǔ瀹氭壙杞界殑 typed contract
- `NativeAotLoweringPlanner` 鍙秷璐圭殑 lowering / ABI / dispatch 鑼冨洿
- `NativeAotEmitter + Scriban` 鍙緭鍑虹殑 native 浠ｇ爜褰㈢姸

骞剁‘淇濇柊鐨勬敮鎸侀潰鑳芥槧灏勫洖锛?
- owner subject
- proof / benchmark 璇佹嵁
- unsupported report / release evidence / engineering gate

## 2. 鑼冨洿杈圭晫

鏈?roadmap 璐熻矗锛?
- 鍩轰簬 Phase 0 鍐荤粨鐗╃户缁洏鐐瑰苟鍏抽棴 typed-IL translation gap
- 鎶婄湡瀹炴湭鏀寔鐐规暣鐞嗕负椤哄簭 child tasks
- 淇濇寔 `AotCoreIr -> NativeAotLoweringPlanner -> NativeAotEmitter + Scriban` 鐨勫崟鍚戞敹鍙?- 璁╂柊鏀寔闈㈠湪 owner subject / benchmark / gate 涓婃湁璇佹嵁闂幆

## 3. 闈炵洰鏍?
- 涓嶄竴娆℃€цˉ榻愬叏閮?C# feature
- 涓嶉噸寮€娴嬭瘯绯荤粺 generic 鍖栦富绾?- 涓嶆妸 `Chaos.TestFramework` 閫昏緫娣峰叆 AOT core / planner / emitter
- 涓嶅湪鏈疆澶勭悊 mobile host銆乪ngine binding銆侀澶栦骇鍝佸寲闂
- 涓嶆墿澶?subject 鍐呴儴鏂扮殑 string 璋冨害鍗忚

## 4. 闃舵鍒楄〃

### Phase 0: Translation Gap Freeze And Batch Contract

- goal:
  - 鍩轰簬鐪熷疄浠ｇ爜鐑偣涓?Phase 0 ledger锛屽喕缁撴柊鐨?translation gap 鎵ц鍙拌处
  - 鏄庣‘ capability batch銆乷wner銆乻tage tag銆乸roof / benchmark 瑕佹眰
- exit_criteria:
  - 鏂?gap 鍙拌处鍙互瑕嗙洊褰撳墠 loader / `AotCoreIrLowering` / planner / emitter 鐨勪富瑕佹湭鏀寔鐐?  - 姣忎釜 gap 椤归兘鑳芥槧灏勫埌 child task锛岃€屼笉鏄暎钀藉湪 notes 閲?- deliverables:
  - gap inventory / batch matrix
  - child-task mapping
  - 浼樺厛绾у拰 deferred 鍒ゆ柇
- dependencies:
  - `20260414-37`
  - `20260415-01`
- open_questions:
  - 鏄惁浠?capability-first 涓轰富銆乻tage tag 涓鸿緟鏉ュ缓鍙拌处
  - 鎺ㄨ崘锛氭槸銆傚惁鍒欎細鍥炲埌鍗曞眰鍋囬棴鐜?
### Phase 1: Loader And Metadata Decode Expansion

- goal:
  - 鎵╁ぇ loader 瀵?opcode銆乵etadata handle銆乻ignature shape 鐨?decode contract
- exit_criteria:
  - 鏈樁娈电洰鏍?capability 涓嶅啀鍦?loader 鐩存帴澶辫触
  - 鐩稿叧 typed-il 杈撳叆闈㈠叿澶囧畬鏁?reference / signature 淇℃伅
- deliverables:
  - Loader / metadata provider widening
  - 瀵瑰簲 unit / integration tests
  - subject/source anchor 鏄犲皠鏇存柊
- dependencies:
  - `Phase 0`
- open_questions:
  - function pointer 鏄彧鍋?decode锛岃繕鏄繛 planner contract 涓€骞惰璁?  - 鎺ㄨ崘锛氭湰闃舵鑷冲皯鍋?decode + identity锛涚湡姝?native emission 鍙斁鍏ュ悗缁樁娈?
### Phase 2: Scalar, Conversion, And ABI Carrier Widening

- goal:
  - 鎵╁ぇ scalar 鏁板€笺€佹瘮杈冦€乧onversion銆乧arrier 涓?ABI slot 鐨?typed contract
- exit_criteria:
  - 鐩爣 capability 涓嶅啀鍙兘鐢?`Int32` / `NativeInt` 杩戜技琛ㄨ揪
  - planner / IR 瀵?widened carrier 鏈変竴鑷磋〃绀?- deliverables:
  - `AotCoreIrLowering` carrier 鎵╁睍
  - planner arithmetic / compare / conversion widening
  - 瀵瑰簲 proof 涓庡簳灞傚洖褰?- dependencies:
  - `Phase 1`
- open_questions:
  - 鏄厛鍋氭暣鏁版棌杩樻槸鍚屾椂鍋氭诞鐐规棌
  - 鎺ㄨ崘锛氬厛鍋氭暣鏁颁笌 signed/unsigned compare / conversion锛屾诞鐐逛綔涓哄悓闃舵鍚庡崐鎵规鎴栫揣闅忓叾鍚?
### Phase 3: Structured Control Flow And Exception Expansion

- goal:
  - 鎵╁ぇ structured control flow 涓?EH shape 鐨?AOT contract
- exit_criteria:
  - 涓嶅啀鍙敮鎸?catch-only exception shape
  - `switch` / `leave` / finally / fault / filter 鑷冲皯瀹屾垚 roadmap 鐩爣鎵规
- deliverables:
  - `AotCoreIr` EH contract widening
  - planner / emitter 瀵瑰簲鎺у埗娴佺敓鎴愯矾寰?  - proof 涓?compatibility tests
- dependencies:
  - `Phase 2`
- open_questions:
  - 鏄惁鎶?filter 涓?fault 鏀惧湪鍚屼竴鎵规
  - 鎺ㄨ崘锛氬悓闃舵浣嗘媶鎴愪笉鍚?checklist锛岄伩鍏?finally 鍏抽棴鍚庤 filter/fault 闃诲

### Phase 4: NativeAOT Planner, Entry ABI, And Scriban Widening

- goal:
  - 鎵╁ぇ planner / emitter 瀵?widened method shape銆乪ntry ABI銆乨ispatch銆乿alue/byref carrier 鐨勬秷璐硅兘鍔?- exit_criteria:
  - 鏂板 widening 涓嶅啀渚濊禆 legacy fallback ABI
  - 鎵€鏈夋柊澧?native 鐢熸垚缁撴瀯缁х画閫氳繃 Scriban 妯℃澘钀藉湴
- deliverables:
  - planner model widening
  - emitter model widening
  - `Templates/NativeAot*.scriban` 鏇存柊
  - artifact-level tests
- dependencies:
  - `Phase 3`
- open_questions:
  - 鏄惁鍏紑鎵╁ぇ workload ABI
  - 鎺ㄨ崘锛氬厛鎵╁ぇ鍐呴儴 planner / emitter contract锛涘灞傚叕寮€ workload ABI 鍙湪纭湁鍦烘櫙鏃跺啀寮€鏀?
### Phase 5: Owner Subject, Benchmark, And Gate Closure

- goal:
  - 鎶婃柊澧炴敮鎸侀潰鏄犲皠鍥?owner subject銆乸roof銆乥enchmark銆乨ashboard銆乺elease evidence
- exit_criteria:
  - 鏂板 capability 涓嶅啀鍋滅暀鍦ㄥ簳灞傛祴璇?  - `unsupported` 涓?`missing evidence` 鐨勫灞傛樉绀轰粛鍑嗙‘
- deliverables:
  - owner subject proof/benchmark 鏇存柊
  - benchmark / dashboard / unsupported report contract 鏇存柊
  - 宸ョ▼绾?gate 涓?release evidence 鏀跺彛
- dependencies:
  - `Phase 4`
- open_questions:
  - 鍝簺 capability 蹇呴』 benchmark锛屽摢浜涘彧闇€瑕?proof
  - 鎺ㄨ崘锛氭部鐢?Phase 0 translation ledger 鐨勯珮浠峰€艰矾寰勫垽瀹氾紝涓嶆妸姣忎釜 capability 閮藉己琛?benchmark 鍖?
## 5. 姣忛樁娈靛畬鎴愬畾涔?
- Phase 0 瀹屾垚锛歝hild task 鍜?gap 鍙拌处宸茬粡瓒冲绋冲畾锛屽悗缁墽琛屼笉鍐嶉渶瑕侀噸鏂板畾涔夋壒娆¤竟鐣?- Phase 1 瀹屾垚锛氱洰鏍?capability 鑷冲皯鑳藉畬鏁磋繘鍏?typed-il / linked world锛屼笉鍐嶈 loader 纭嫤鎴?- Phase 2 瀹屾垚锛歸idened scalar / conversion / compare / ABI carrier 鍦?`AotCoreIr` 涓?planner 涓竴鑷村瓨鍦?- Phase 3 瀹屾垚锛欵H / structured control flow 涓嶅啀鍙楅檺浜?catch-only 鏈€灏忓舰鐘?- Phase 4 瀹屾垚锛歱lanner / emitter / Scriban 瀵?widened shape 鐪熸鍙敓鎴?- Phase 5 瀹屾垚锛歰wner subject / benchmark / gate 瀵规柊鏀寔闈㈠叏閮ㄨ兘缁欏嚭姝ｇ‘ evidence

## 6. 瀛愪换鍔℃槧灏?
| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260415-12-phase-0-translation-gap-freeze-and-batch-contract` | `Phase 0` | `completed` | `codex` | 鍐荤粨褰撳墠 typed-il translation gap 鍙拌处锛屽缓绔?capability-first batch mapping | `20260415-11` |
| `20260415-13-phase-1-loader-and-metadata-decode-expansion` | `Phase 1` | `completed` | `codex` | 鎵╁ぇ loader / metadata decode contract | `20260415-12` |
| `20260415-14-phase-2-scalar-conversion-and-abi-carrier-widening` | `Phase 2` | `completed` | `codex` | 鎵╁ぇ scalar / conversion / compare / ABI carrier 鏀寔闈?| `20260415-13` |
| `20260415-15-phase-3-structured-control-flow-and-exception-expansion` | `Phase 3` | `completed` | `codex` | 鎵╁ぇ switch / leave / finally / fault / filter 绛?control-flow / EH 褰㈢姸 | `20260415-14` |
| `20260415-16-phase-4-native-aot-planner-entry-abi-and-scriban-widening` | `Phase 4` | `completed` | `codex` | 鎵╁ぇ planner / entry ABI / emitter model锛屽苟缁х画閫氳繃 Scriban 杈撳嚭 | `20260415-15` |
| `20260415-17-phase-5-owner-subject-benchmark-and-gate-closure` | `Phase 5` | `completed` | `codex` | 鎶婃柊澧炴敮鎸侀潰鏀跺彛鍒?owner subject銆乥enchmark銆乨ashboard 涓?release gate | `20260415-16` |

## 7. 渚濊禆

- `docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-02.md`
- `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/`
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
- `src/managed/Chaos.IL2CPP.Loader/Metadata/TypeProviders.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/`

## 8. 椋庨櫓

- widening 浼氬悓鏃惰Е纰?loader銆両R銆乸lanner銆乪mitter锛屽鐐瑰悓鏃舵敼鍔紝鍥炲綊闈㈠ぇ
- 褰撳墠 planner 浠嶅瓨鍦ㄥ澶?legacy fallback ABI 涓?`NotSupportedException` 闃茬嚎锛屾壒娆¤竟鐣屽鏋滀笉娓呮锛屽鏄撳鑷?child task 鑼冨洿澶辨帶
- 鍙仛搴曞眰 widening 涓嶅仛 owner subject 鏀跺彛锛屼細鍐嶆閫犳垚 dashboard / unsupported report 涓庣湡瀹炶兘鍔涜劚鑺?- 濡傛灉鏂扮敓鎴愪唬鐮侀噸鏂板洖鍒版墜鍐欐嫾鎺ワ紝鍚庣画缁х画 widening 鐨勭淮鎶ゆ垚鏈細杩呴€熷崌楂?
## 9. 澶囬€夎矾寰?
### 澶囬€夎矾寰?A锛氬厛鍋?Loader 鍏ㄨ鐩栵紝鍐嶅仛涓嬫父

- 涓嶆帹鑽?- 鍘熷洜锛氬鏄撲骇鐢熷崟灞傚亣缁?
### 澶囬€夎矾寰?B锛氬厛浠?subject proof 缂哄彛鍙嶆帹

- 涓嶆帹鑽愪綔涓轰富璺嚎
- 鍘熷洜锛氬鏄撲负浜嗚 proof 杩囪€屽仛涓存椂搴曞眰 patch

### 澶囬€夎矾寰?C锛氭寜 capability batch 绾靛悜鎺ㄨ繘

- 鎺ㄨ崘
- 鍘熷洜锛氳兘鍚屾椂绾︽潫搴曞眰 contract 涓?owner evidence

## 10. 褰撳墠寤鸿鎺ㄨ繘椤哄簭

1. 绔嬪嵆鍒涘缓骞舵墽琛?`20260415-12-phase-0-translation-gap-freeze-and-batch-contract`
2. 鐢ㄦ柊鍙拌处纭 `Phase 1-4` 鐨勬壒娆¤竟鐣屾槸鍚﹁繕闇€瑕佹媶缁?3. 浠?`Loader/metadata decode` 寮€濮嬮€愰樁娈垫帹杩?4. 姣忎釜 child task 瀹屾垚鍚庡悓姝ユ洿鏂?`STATUS.md`銆乣docs/dev/ACTIVE.md`銆佺储寮曚笌 capability 璇佹嵁鐘舵€?
