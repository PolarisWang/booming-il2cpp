# HybridCLR 瀵归綈鐨?AOT 涓庣儹鏇村紑鍙戣鍒?Roadmap v1.01

Date: 2026-04-14 23:34:39 +08:00
Status: completed

## 1. 鐩爣

寤虹珛涓€鏉′互 `typed-il` 涓哄叡浜绾︺€佷互 HybridCLR 瀵归綈涓虹儹鏇存柟鍚戙€佷互 solution owner pack 涓?subject 杞戒綋鐨勯暱鏈熷紑鍙戣矾绾匡紝鏈€缁堣锛?
- C# feature 鎵╁睍鑳藉绋冲畾杞叆 IL2CPP AOT 涓庣儹鏇翠綋绯汇€?- 鍏ㄩ儴鐩爣 AOT translation surface 閮借兘琚樉寮忔槧灏勫埌 supported / partial / unsupported 杈圭晫锛岃€屼笉鏄彧璁板綍鎴愬姛璺緞銆?- `subjects/` 鏃㈡槸 capability owner锛屼篃鏄伐绋嬬骇 solution gate銆?- proof銆乥enchmark銆乫ull-solution銆乵ixed execution 鍜?hot update 鑳藉叡浜粺涓€璇佹嵁閾俱€?
## 2. 鑼冨洿杈圭晫

鏈?roadmap 璐熻矗锛?
- 鍐荤粨 `typed-il` / capability taxonomy / body availability / reference bundle 绛夊熀纭€濂戠害銆?- 鍐荤粨 AOT translation surface ledger 涓?unsupported surface ledger銆?- 鎶婂巻鍙茶娉曠偣鏈烘鎷嗘垚鍗曟枃浠?capability slice锛屽苟鎸夐鍩熷綊骞跺埌灏戦噺 owner project銆?- 鎶?`ChaosUnitTest` / `ChaosBenchmark` 鍥哄寲涓?proof / benchmark 澹版槑鍏ュ彛銆?- 閫愭寤虹珛 HybridCLR 瀵归綈鐨?dispatch銆乻upplemental metadata銆乥ridge/thunk 鍜?AOT Core IR銆?- 寤虹珛浠?capability proof 鍒板伐绋嬬骇 solution gate 鐨勭粺涓€楠岃瘉浣撶郴銆?
鏈?roadmap 涓嶈礋璐ｏ細

- 鍦?Phase 0 涔嬪墠鐩存帴璺宠繘澶ц妯″姛鑳藉疄鐜般€?- 缁х画鎵╁ぇ string 鍗忚椹卞姩鐨?subject 鍐呴儴璋冨害闈€?- 鎶婃墍鏈?benchmark 閮藉仛鎴愬己鍒堕」銆?- 鏃犱笂闄愬鍔犻《灞?`subject` 鏁伴噺銆?
## 3. 闈炵洰鏍?
- 涓嶆妸褰撳墠璁″垝瀹氫箟鎴愨€滄墍鏈?C# feature 涓€娆℃€цˉ榻愨€濄€?- 涓嶆妸鈥滄湭鍏ヨ处鐨勮兘鍔涚┖鐧解€濊鍒ゆ垚鈥滈粯璁ゅ凡鏀寔鈥濄€?- 涓嶉噸鏂板紩鍏ュ鏉傜殑 subject 鐩綍 DSL銆?- 涓嶄负浜嗗吋瀹瑰巻鍙插叆鍙ｉ暱鏈熶繚鐣欏濂?pipeline銆?- 涓嶆妸绉诲姩骞冲彴銆佸彂甯冧骇鍝佸寲鍜?desktop 寮€鍙戜富绾挎贩鎴愪竴涓樁娈垫帹杩涖€?
## 4. 闃舵鍒楄〃

### Phase 0: Contract Freeze And Capability Ledger

- 鍐荤粨 repository-owned reference bundle 杈圭晫銆?- 鍐荤粨涓ょ骇 capability taxonomy銆?- 鍐荤粨 stable identity銆乥ody availability銆乸roof/benchmark 澹版槑妯″瀷銆?- 鍐荤粨 `AOT translation surface ledger` 涓?`unsupported surface ledger` 鐨勬渶灏忓瓧娈靛拰缁存姢鏂瑰紡銆?- 褰㈡垚 capability ledger 鍜?Phase 鍚庣画瀛愪换鍔＄殑鍞竴杈撳叆鍩虹嚎銆?
#### Phase 0 宸插喕缁撳熀绾匡紙2026-04-15 00:29:10 +08:00锛?
浠ヤ笅鍐呭宸插畬鎴愬喕缁擄紝骞朵綔涓哄悗缁叏閮?Phase 鐨勫綋鍓嶅绾﹀熀绾裤€傚畬鏁村喕缁撳唴瀹逛互瀵瑰簲鏂囨。涓哄噯锛宺oadmap 鍦ㄨ繖閲屼繚鐣欐樉寮忕储寮曞拰鍏抽敭缁撹锛?
1. `reference bundle` 鍐荤粨
   - 鏂囨。锛歚docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/reference-bundle-freeze-v1-01.md`
   - 缁撹锛?     - canonical base bundle 鍞竴浣嶇疆鍥哄畾涓?`assets/reference-bundles/dotnet-foundation/net8.0`
     - controlled supplement 褰撳墠鍥哄畾涓?`assets/reference-bundles/dotnet-foundation/lib/ReferenceGreeter.dll`
     - archetype 娑堣垂鐭╅樀鍥哄畾鍒?`ProjectReference / PackageReference / canonical base bundle / controlled supplement`
     - 绂佹鏈満缁濆璺緞銆佽繍琛屾椂鍋峰伔鏀瑰啓 `.csproj`銆佹妸 supplement 婕斿寲鎴愮浜屽 bundle

2. `capability taxonomy` 鍐荤粨
   - 鏂囨。锛歚docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/capability-taxonomy-freeze-v1-01.md`
   - 缁撹锛?     - taxonomy 鍥哄畾涓轰袱绾э細`family -> item`
     - Level-1 family 褰撳墠鍐荤粨涓猴細`PrimitivesAndOps`銆乣TypeSystemAndGenerics`銆乣AsyncAndIterators`銆乣DelegatesAndClosures`銆乣ExceptionsAndControlFlow`銆乣ReflectionAndMetadata`銆乣InteropAndMarshaling`銆乣ThreadingAndSynchronization`銆乣RuntimeServices`銆乣UnsafeAndLowLevel`銆乣DiagnosticsAndTracing`銆乣LinkerAndAotClosure`銆乣HotUpdateWorkflow`銆乣MixedExecution`
     - 姣忎釜 item 蹇呴』鏄惧紡缁戝畾 owner subject锛涜矾寰?鍛藉悕绌洪棿浣撶幇 family 鎴?domain锛屾枃浠跺悕浣撶幇 capability item

3. `stable identity` 涓?`body availability` 鍐荤粨
   - 鏂囨。锛歚docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/stable-identity-and-body-availability-v1-01.md`
   - 缁撹锛?     - public stable capability id 鍥哄畾涓?`((uint)family << 16) | item`
     - `ChaosUnitTest` / `ChaosBenchmark` 鐨勬渶灏忓叕寮€ metadata 瀛楁宸插喕缁擄紱`Alias` 鍙繚鐣欎负灞曠ず鏍囩锛屼笉鍐嶄綔涓虹ǔ瀹氳矾鐢变富閿?     - `BodyAvailability` 鐩爣鏋氫妇鍥哄畾涓猴細`None`銆乣NativeGenerated`銆乣InterpreterReady`銆乣BridgeDispatch`銆乣MetadataOnly`銆乣ExternalRuntime`銆乣Unsupported`
     - 鍚庣画涓嶅啀缁х画鎵╁ぇ string 璋冨害闈紱鏂拌皟搴﹂渶姹備紭鍏堟寕鍒?compact enum / numeric identity

4. `AOT translation surface ledger` 鍐荤粨
   - 鏂囨。锛歚docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/aot-translation-surface-ledger-v1-01.md`
   - 缁撹锛?     - 鍏ㄩ儴鐩爣缈昏瘧闈㈠繀椤诲綊鍏ヤ簲绫伙細`language surface`銆乣compiler-lowered surface`銆乣runtime service surface`銆乣metadata / closure surface`銆乣interop / ABI surface`
     - 姣忔潯 surface 蹇呴』鏄惧紡璁板綍 capability item銆乷wner subject銆乷wner archetype/source anchor銆乸roof銆乥enchmark 绛栫暐鍜岀洰鏍?`BodyAvailability`
     - engineering surface 鍚屾牱鍏ヨ处锛屽寘鎷?`ProjectReference`銆乣PackageReference`銆乪xplicit DLL reference銆乧orelib closure銆乵ixed reference closure銆乭ot-update full project銆乵ixed bridge solution

5. `AOT unsupported surface ledger` 鍐荤粨
   - 鏂囨。锛歚docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/aot-unsupported-surface-ledger-v1-01.md`
   - 缁撹锛?     - `partial / unsupported / not-applicable` 涓嶈兘闈欓粯缂哄腑锛屽繀椤绘樉寮忓叆璐?     - 褰撳墠 partial / unsupported baseline 宸插喕缁擄紝鍖呮嫭 `GenericConstraint`銆乬eneric dispatch銆乣MetadataSupplement`銆乣AotClosureValidation`銆乨iagnostics surfaces銆乭ot-update workflow items銆乵ixed execution items 绛?     - reason code 璇嶆眹鍐荤粨涓猴細`platform_host_gap`銆乣toolchain_gap`銆乣runtime_gap`銆乣metadata_gap`銆乣compatibility_gap`銆乣policy_blocked`銆乣scope_not_targeted`

6. Phase 0 瀛愪换鍔″綊妗ｄ笌鐘舵€佸熀绾?   - 鐘舵€佹枃妗ｏ細`docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/STATUS.md`
   - 绾︽潫锛?     - 鍚庣画 Phase 1-6 鑻ヤ慨鏀逛笂杩板绾︼紝蹇呴』鍚屾椂鍥炲啓瀵瑰簲鍐荤粨鏂囨。涓庢瘝 roadmap
     - 鏈繘鍏ヤ笂杩板喕缁撳熀绾跨殑鑳藉姏銆乺eason code銆乥undle 瑙勫垯锛屼笉寰楅粯璁よ涓哄凡鎵瑰噯濂戠害

### Phase 1: Subject Source Reshape And Test Metadata Cutover

- 鎶婂巻鍙茶娉曠偣鏈烘鎷嗘垚鍗曟枃浠?capability slice銆?- 鍦ㄥ皯閲?owner solution pack 鍐呴噸缁?project 杈圭晫銆?- 鐢?`ChaosUnitTest` / `ChaosBenchmark` 鏇夸唬鏃х殑 subject 鍐呯洰褰曞崗璁€?- 娓呯悊娈嬩綑 `Console.WriteLine` 鍒ゅ畾锛岀粺涓€ Assert 濂椾欢銆?
### Phase 2: Typed-IL Shared Contract And Hybrid Dispatch Foundation

- 鎶?`typed-il` 鍙樻垚 AOT銆佽В閲婃墽琛屻€乥ridge/thunk 鐨勫叡浜緭鍏ャ€?- 寮曞叆 `BodyAvailabilityResolver` 鍜?`HybridDispatchResolver`銆?- 寤虹珛浠?method identity 鍒版墽琛屾柟寮忕殑缁熶竴鍐崇瓥闈€?
### Phase 3: HotUpdate Runtime And Supplemental Metadata Foundation

- 寤虹珛 `HotUpdateAssemblyLoader`銆?- 寤虹珛 `SupplementalMetadataRegistry`銆?- 鎵撻€氬涓荤▼搴忛泦銆佺儹鏇寸▼搴忛泦鍜岀ǔ瀹?identity 鐨勫榻愩€?- 褰㈡垚鏈€灏?mixed execution 闂幆銆?
### Phase 4: AOT Core IR And Native Codegen Strengthening

- 浠?`typed-il` 闄嶅埌 `AotCoreIr`銆?- 鍦?`AotCoreIr` 涓婃墿灞曞璞℃ā鍨嬨€佽皟鐢ㄦā鍨嬨€佹硾鍨嬨€佸紓甯搞€佸厓鏁版嵁闂寘銆?- 閫愭鏇夸唬褰撳墠 ad-hoc `NativeAotEmitter` 鐩村嚭璺緞銆?
### Phase 5: Feature Coverage Expansion On Owner Subjects

- 浠?capability family/item 鍜?translation surface ledger 涓哄崟浣嶈ˉ榻愬叏閮ㄧ洰鏍?AOT 缈昏瘧闈€?- 姣忎釜鑳藉姏鐐归兘钀藉埌 owner subject銆乸roof銆佸彲閫?benchmark 鍜?archetype gate銆?- 淇濇寔鈥滃崟鏂囦欢 capability slice + 灏戦噺 owner project鈥濈殑缁撴瀯绾緥銆?
### Phase 6: Engineering Archetype, Benchmark And Release Gates

- 鎵撻€氬畬鏁?solution ingestion銆乺eference bundle銆乫ull-solution translation 涓?native run銆?- 鏀跺彛 benchmark dashboard銆乻upport-state 鍜?reason code銆?- 褰㈡垚 mixed execution / hot update / productization 鐨勭粺涓€鍙戝竷绾ц瘉鎹摼銆?
## 5. 姣忛樁娈靛畬鎴愬畾涔?
### Phase 0

- `goal`
  - 鍐荤粨鎵€鏈夊悗缁疄鐜颁細鍙嶅渚濊禆鐨勫熀纭€濂戠害銆?- `exit_criteria`
  - Q0 `reference bundle` 鐨勪粨搴撹竟鐣屻€佸埛鏂扮瓥鐣ャ€佷娇鐢?archetype 鍜屾牎楠屾柟寮忔槑纭€?  - Q1 `capability taxonomy` 鍥哄畾涓轰袱绾х粨鏋勩€?  - `body availability` 鏋氫妇涓?stable identity 濂戠害鏄庣‘銆?  - `ChaosUnitTest` / `ChaosBenchmark` 鐨勬渶灏忓叕鍏?contract 鏄庣‘銆?  - 鍏ㄩ儴鐩爣 AOT translation surface 閮借繘鍏?`aot-translation-surface-ledger`銆?  - 鏄庣‘涓嶆敮鎸佹垨鏈疆鏆傜紦鐨勭炕璇戦潰閮借繘鍏?`aot-unsupported-surface-ledger`锛屽苟闄?reason code銆?- `deliverables`
  - capability ledger
  - aot translation surface ledger
  - aot unsupported surface ledger
  - reference bundle policy
  - stable identity policy
  - body availability enum draft
- `dependencies`
  - 鏃?- `open_questions`
  - 鏃犮€傞粯璁ゅ喕缁撳涓嬶細
  - `reference bundle` 閲囩敤鈥渞epository-owned canonical base bundle + 鍙楁帶 supplement鈥濇ā鍨嬨€?  - 涓ょ骇 `capability taxonomy` 鎸夎涔?杩愯鏃跺煙鍒掑垎 family銆?  - `public stable id` 閲囩敤 `family enum + item enum` 澶嶅悎绋冲畾 numeric id銆?
### Phase 1

- `goal`
  - 瀹屾垚 subject 婧愮爜鍜屾祴璇曞厓鏁版嵁鐨勭粨鏋勫寲閲嶇粍銆?- `exit_criteria`
  - 鍘嗗彶璇硶鐐瑰畬鎴愬崟鏂囦欢鍒囩墖銆?  - owner subject pack 鍜?project 褰掑苟杈圭晫绋冲畾銆?  - 鏃х殑 subject 鍐呭瓧绗︿覆鐩綍鍗忚涓嶅啀鎵╁ぇ銆?  - proof 鍏ㄩ儴浣跨敤 Assert 濂椾欢琛ㄨ揪缁撴灉銆?- `deliverables`
  - subject migration matrix
  - owner project layout note
  - `Chaos.TestFramework` metadata contract expansion
- `dependencies`
  - Phase 0
- `open_questions`
  - 鏃犮€傞粯璁ゅ喕缁撳涓嬶細
  - 璺緞鎴栧懡鍚嶇┖闂翠綋鐜?family / domain锛屾枃浠跺悕浣撶幇 capability item銆?
### Phase 2

- `goal`
  - 寤虹珛 typed-il 鍏变韩濂戠害鍜岀粺涓€ dispatch 鍩虹灞傘€?- `exit_criteria`
  - AOT銆佽В閲婃墽琛屻€乥ridge/thunk 鑷冲皯鍏变韩鍚屼竴濂?method/type identity銆?  - `BodyAvailabilityResolver` 鑳界粰鍑虹ǔ瀹氱粨鏋溿€?  - mixed execution 涓嶅啀渚濊禆涓存椂鍒嗘敮鍒ゆ柇銆?- `deliverables`
  - typed-il contract freeze note
  - hybrid dispatch design patch set
  - body availability integration note
- `dependencies`
  - Phase 0
  - Phase 1
- `open_questions`
  - 鏃犮€傞粯璁ゅ喕缁撳涓嬶細
  - `typed-il` 鍏堥檷鍒拌杽灞?`interpreter IR`锛岃€屼笉鏄洿鎺ヨ瑙ｉ噴鎵ц灞傛秷璐癸紝涔熶笉鍗曠嫭婕斿寲鎴愮浜屽鎬绘ā鍨嬨€?
### Phase 3

- `goal`
  - 寤虹珛 HybridCLR 瀵归綈鐨勭儹鏇存渶灏忚繍琛屾椂涓荤嚎銆?- `exit_criteria`
  - supplemental metadata 鍙敞鍐屻€佸彲鏌ヨ銆佸彲涓庣▼搴忛泦 identity 瀵归綈銆?  - 鐑洿鍏ュ彛鍙€氳繃缁熶竴 dispatch 杩涘叆 mixed execution銆?  - 鑷冲皯涓€鏉?host -> hot update -> fallback 鐨勯棴鐜窇閫氥€?- `deliverables`
  - hot update runtime foundation
  - supplemental metadata registry
  - bridge/thunk minimum contract
- `dependencies`
  - Phase 2
- `open_questions`
  - 鏃犮€傞粯璁ゅ喕缁撳涓嬶細
  - bridge/thunk 閲囩敤缂栬瘧鏈熶富瀵笺€佽繍琛屾椂鏈夐檺琛ュ厖鐨勭瓥鐣ャ€?
### Phase 4

- `goal`
  - 寮哄寲 AOT codegen锛屼娇鍏跺缓绔嬪湪绋冲畾涓眰涔嬩笂銆?- `exit_criteria`
  - `AotCoreIr` 鍙壙鎺ュ叧閿璞?璋冪敤/娉涘瀷/寮傚父/鍏冩暟鎹棴鍖呰兘鍔涖€?  - 褰撳墠 `NativeAotEmitter` 鐨勬牳蹇冭矾寰勫紑濮嬭縼绉诲埌 `AotCoreIr`銆?  - AOT 涓庣儹鏇村叡浜?identity锛屼笉鍐嶅舰鎴愬钩琛屼笘鐣屻€?- `deliverables`
  - `AotCoreIr` contract
  - native codegen migration slices
  - codegen capability gap ledger
- `dependencies`
  - Phase 2
  - Phase 3
- `open_questions`
  - 鏃犮€傞粯璁ゅ喕缁撳涓嬶細
  - 鍏堝喕缁?generic context / sharing contract锛屽啀浼樺厛琛?codegen closure 涓庢樉寮忓疄渚嬪寲锛屽悓鏃剁粰 runtime sharing 棰勭暀閽╁瓙銆?
### Phase 5

- `goal`
  - 浠?capability ledger 涓?translation surface ledger 涓轰富绾挎墿灞曞畬鏁?AOT 缈昏瘧瑕嗙洊銆?- `exit_criteria`
  - 姣忎釜鏂板 capability item 閮借兘鏄犲皠鍒?owner subject銆乸roof銆佸彲閫?benchmark銆乻upport-state銆?  - 姣忎釜鐩爣 translation surface 鏉＄洰閮借兘鏄犲皠鍒?owner subject銆乸roof銆佸彲閫?benchmark 涓庡綋鍓嶇姸鎬併€?  - 鏄庣‘ deferred / unsupported 鐨勭炕璇戦潰閮戒繚鐣欑ǔ瀹氳褰曪紝鑰屼笉鏄粠璁″垝涓秷澶便€?  - 涓嶅啀鏂板鏃?owner 鐨勫疄楠屾€?subject銆?  - 鍘嗗彶璇硶鐐硅縼绉绘敹鍙ｅ畬鎴愩€?- `deliverables`
  - capability slice batches
  - translation surface coverage refresh
  - unsupported surface report refresh
  - proof coverage expansion
  - selective benchmark expansion
- `dependencies`
  - Phase 1
  - Phase 4
- `open_questions`
  - 鏃犮€傞粯璁ゅ喕缁撳涓嬶細
  - benchmark 閲囩敤鍒嗗眰绛栫暐锛氶珮浠峰€?runtime path 蹇呭甫 benchmark锛屽叾浣欒兘鍔涚偣淇濈暀 correctness proof 鍗冲彲銆?
### Phase 6

- `goal`
  - 鎶婅兘鍔涜鐩栦粠鍗曠偣 proof 鎻愬崌鍒板伐绋嬬骇鍜屽彂甯冪骇 gate銆?- `exit_criteria`
  - full-solution銆乺eference bundle銆乥enchmark dashboard銆乵ixed execution銆乭ot update 褰㈡垚缁熶竴璇佹嵁閾俱€?  - native generate 娴佺▼鍦?retained owner subjects 涓婄ǔ瀹氳窇閫氥€?  - dashboard 鑳戒互 managed baseline 涓轰腑蹇冨睍绀?solution 鍐呮洿缁嗙矑搴︽潯鐩€?  - unsupported / partial translation surface 鑳界ǔ瀹氳繘鍏ュ灞傛姤鍛娿€乨ashboard 鎴?release 鏂囨。銆?- `deliverables`
  - engineering archetype gate matrix
  - dashboard/reason-code closure
  - unsupported feature report
  - release-gate checklist
- `dependencies`
  - Phase 3
  - Phase 4
  - Phase 5
- `open_questions`
  - 鏃犮€傞粯璁ゅ喕缁撳涓嬶細
  - 涓荤嚎鍏堢撼鍏?mobile-ready contract 涓庢渶灏?smoke gate锛屽畬鏁?device/package/perf gate 浣滀负鍚庣疆骞冲彴涓撻」銆?
## 6. 瀛愪换鍔℃槧灏?
| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `phase-0-contract-freeze-and-capability-ledger` | `Phase 0` | `ready` | `codex` | 鍐荤粨 reference bundle銆乧apability taxonomy銆乻table identity銆乥ody availability 涓?AOT translation surface 鍙屽彴璐?| `-` |
| `phase-1-subject-source-reshape-and-test-metadata-cutover` | `Phase 1` | `completed` | `codex` | 瀹屾垚 subject 鍗曟枃浠跺垏鐗囥€乷wner project 褰掑苟鍜屾祴璇?attribute 鏀跺彛 | `phase-0-contract-freeze-and-capability-ledger` |
| `phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation` | `Phase 2` | `completed` | `codex` | 建立 typed-il 共享契约和统一 dispatch 基础层 | `phase-0-contract-freeze-and-capability-ledger, phase-1-subject-source-reshape-and-test-metadata-cutover` |
| `phase-3-hotupdate-runtime-and-supplemental-metadata-foundation` | `Phase 3` | `completed` | `codex` | 建立热更运行时、supplemental metadata 和 mixed execution 最小闭环 | `phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation` |
| `phase-4-aot-core-ir-and-native-codegen-strengthening` | `Phase 4` | `completed` | `codex` | 寮曞叆 AOT Core IR 骞舵妸 native-aot 姝ｅ紡杩佺Щ鍒?`typed-il -> AotCoreIr -> native` 涓荤嚎 | `phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation, phase-3-hotupdate-runtime-and-supplemental-metadata-foundation` |
| `phase-5-feature-coverage-expansion-on-owner-subjects` | `Phase 5` | `completed` | `codex` | 閫氳繃 `phase-5-6-reuse-audit-v1-01.md` 澶嶇敤鏃㈡湁 owner-subject capability batch锛屼笌褰撳墠 ledger / gap ledger 瀹屾垚瀵硅处 | `phase-1-subject-source-reshape-and-test-metadata-cutover, phase-4-aot-core-ir-and-native-codegen-strengthening` |
| `phase-6-engineering-archetype-benchmark-and-release-gates` | `Phase 6` | `completed` | `codex` | 閫氳繃澶嶇敤 full-solution / mixed-execution / hot-update / dashboard gate锛屽苟鍦ㄥ綋鍓嶄唬鐮佺嚎鍥炲綊楠岃瘉瀹屾垚鏀跺彛 | `phase-3-hotupdate-runtime-and-supplemental-metadata-foundation, phase-4-aot-core-ir-and-native-codegen-strengthening, phase-5-feature-coverage-expansion-on-owner-subjects` |

## 7. 渚濊禆

- Phase 0 鏄暣涓矾绾跨殑鍐荤粨鍏ュ彛锛屾病鏈夎繖涓€姝ワ紝鍚庣画鎵€鏈夊疄鐜伴兘浼氱户缁竟鍋氳竟鏀瑰绾︺€?- Phase 1 璐熻矗 subject 鍜?proof/benchmark 鎵胯浇闈㈢殑绋冲畾鍖栵紝鏄?capability 鎵╁睍鐨勮緭鍏ユ竻娲楅樁娈点€?- Phase 2 鍜?Phase 3 鍐冲畾鍚庣画绯荤粺鏄惁鐪熺殑 HybridCLR 瀵归綈锛岃€屼笉鏄€滅儹鏇磋ˉ涓侀檮鐫€鍦?AOT 鍚庨潰鈥濄€?- Phase 4 蹇呴』鍦?Phase 2/3 涔嬪悗鎺ㄨ繘锛屽惁鍒?AOT core IR 浠嶄細缂哄け mixed execution 鍜?metadata 璇箟銆?- Phase 5 鍜?Phase 6 鏄?capability 鎵╁睍涓庡伐绋嬬骇鏀跺彛闃舵锛屼緷璧栧墠闈㈠绾﹀凡缁忕ǔ瀹氥€?
## 8. 椋庨櫓

### 椋庨櫓 1: roadmap 鍐嶆閫€鍖栦负鈥滃垪璇硶鐐规墦琛ㄢ€?
- 缂撹В
  - 鍏堝喕缁撲袱绾?capability taxonomy銆?  - 姣忎釜鍘嗗彶璇硶鐐瑰厛鏄犲皠 capability item锛屽啀鍐冲畾 owner pack 涓庡疄鐜伴『搴忋€?
### 椋庨櫓 2: subject 鍗曟枃浠跺寲婕斿彉鎴?project 鐖嗙偢

- 缂撹В
  - 鍗曟枃浠跺寲鍙彂鐢熷湪 capability slice 灞傘€?  - project 浠嶆寜棰嗗煙鑱氬悎锛屼笉寮哄埗涓€鑳藉姏涓€宸ョ▼銆?
### 椋庨櫓 3: 鐑洿涓?AOT 鍚勮嚜缁存姢 identity 鍜?metadata

- 缂撹В
  - Phase 0 鏄庣‘ stable identity銆乥undle policy 鍜?body availability銆?  - Phase 2/3 缁熶竴 dispatch 涓?supplemental metadata銆?
### 椋庨櫓 4: dashboard 涓庣湡瀹炶兘鍔涚姸鎬佺户缁紓绉?
- 缂撹В
  - dashboard 缁熶竴浠?managed baseline 涓哄熀绾裤€?  - 缂哄け鏁版嵁蹇呴』褰掑叆鏄庣‘鐨?`Unsupported` 鎴?reason code锛岃€屼笉鏄潤榛樹涪澶便€?
## 9. 澶囬€夎矾寰?
### 澶囬€夎矾寰?A: 鍏堝彧鍋?AOT codegen 澧炲己锛岀儹鏇村悗缃?
- 浼樼偣
  - 鐭湡鏇村儚鐩存帴鈥滆ˉ鍔熻兘鈥濄€?- 缂虹偣
  - 澶ф鐜囧湪 bridge銆乵etadata銆乨ispatch 涓婅繑宸ャ€?
### 澶囬€夎矾寰?B: 鍏堝彧鍋?subject/test 娓呯悊锛岃繍琛屾椂璁捐鍚庣疆

- 浼樼偣
  - 鏂囨。鍜岀洰褰曠煭鏈熷彉骞插噣銆?- 缂虹偣
  - 濡傛灉娌℃湁缁熶竴 runtime 濂戠害锛宻ubject 娓呯悊寰堝揩浼氬啀娆℃紓绉汇€?
### 褰撳墠鍒ゆ柇

- 鎺ㄨ崘淇濇寔褰撳墠涓昏矾绾匡細
  - 鍏堝喕缁撳绾?  - 鍐嶆敹鍙?subject 涓?metadata
  - 鍐嶅仛 hybrid dispatch / hot update foundation
  - 鍐嶅己鍖?AOT core IR
  - 鏈€鍚庢墿澶ц兘鍔涜鐩栧拰宸ョ▼绾?gate

## 10. 宸插喕缁撳喅绛?
- Phase 0 鍐荤粨浜х墿宸叉樉寮忛敋瀹氬湪 `4 -> Phase 0 宸插喕缁撳熀绾縛锛屽悗缁墽琛屼互璇ヨ妭鍜屽搴?freeze 鏂囨。涓哄綋鍓嶇湡鐩告簮銆?- `Q0`
  - `reference bundle` 閲囩敤鈥滃崟涓€ canonical base bundle + 鍙楁帶 supplement鈥濄€?- `Q1`
  - `capability taxonomy` 鍥哄畾涓轰袱绾э紝骞舵寜璇箟/杩愯鏃跺煙鍒掑垎 family銆?- `Q2`
  - `public stable id` 閲囩敤 `family enum + item enum` 澶嶅悎绋冲畾 numeric id銆?- `Q3`
  - capability slice 鍛藉悕閲囩敤鈥渇amily/domain 璺緞 + item 鏂囦欢鍚嶁€濄€?- `Q4`
  - `typed-il` 闄嶅埌钖勫眰 `interpreter IR`锛屼笉鐩存帴瑁镐緵瑙ｉ噴鍣紝涔熶笉鍒嗗弶鎴愮浜屽鎬绘ā鍨嬨€?- `Q5`
  - bridge/thunk 閲囩敤缂栬瘧鏈熶富瀵笺€佽繍琛屾椂鏈夐檺琛ュ厖銆?- `Q6`
  - 娉涘瀷绛栫暐鍏堝喕缁?generic/sharing contract锛屽啀浼樺厛瀹屾垚 codegen closure锛屽悓鏃堕鐣?runtime sharing 閽╁瓙銆?- `Q7`
  - benchmark 閲囩敤鍒嗗眰绛栫暐锛屽彧寮哄埗瑕嗙洊楂樹环鍊?runtime path銆?- `Q8`
  - 绉诲姩骞冲彴鍏堣繘鍏?contract + smoke gate锛屼笉绔嬪嵆鎷夊叆瀹屾暣涓荤嚎鍙戝竷 gate銆?
## 11. 褰撳墠寤鸿鎺ㄨ繘椤哄簭

1. 鍏堣繘鍏?`Phase 0`锛岄€愭潯纭 Q0銆丵1 浠ュ強鍓╀綑 open questions 鐨勯粯璁よВ銆?2. Phase 0 瀹屾垚鍚庯紝绔嬪嵆鍒?`Phase 1`锛屾妸 subject 婧愮爜銆佽兘鍔涘垏鐗囧拰娴嬭瘯 metadata 鍏堟敹鍙ｃ€?3. 鐒跺悗杩涘叆 `Phase 2` 鍜?`Phase 3`锛屽缓绔?HybridCLR 瀵归綈鐨?shared contract 涓?hot update foundation銆?4. 鍐嶆墽琛?`Phase 4`锛屾妸 AOT codegen 鐪熸鎶埌绋冲畾涓眰涓娿€?5. 鏈€鍚庢墽琛?`Phase 5` 鍜?`Phase 6`锛屽畬鎴?capability 鎵╁紶銆乥enchmark/dashboard 鏀跺彛鍜屽伐绋嬬骇 gate銆?


