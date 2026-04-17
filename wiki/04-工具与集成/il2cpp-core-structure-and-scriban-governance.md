# IL2CPP 鏍稿績缁撴瀯涓?Scriban 娌荤悊瑙勮寖

## 1. 鐩殑

鏈〉鍥哄寲 `IL2CPP` 鏍稿績浠ｇ爜灞傜殑闀挎湡缁撴瀯瑙勫垯锛岀害鏉燂細

- 鏂囦欢濡備綍鎷嗗垎銆?- `CodeGen` 濡備綍鍒嗗眰銆?- `Scriban` 鍦ㄩ」鐩腑鐨勮亴璐ｅ畾浣嶃€?- 鍝簺瑙勫垯蹇呴』鐣欏湪 C#锛屽摢浜涜鍒欏繀椤昏繘鍏ユā鏉垮眰銆?- 鍚庣画 feature 鎺ュ叆鏃跺浣曢伩鍏嶅啀娆℃妸閫昏緫鍫嗗洖瓒呭ぇ鏂囦欢銆?
鏈〉璁板綍闀挎湡鏈夋晥瑙勮寖锛屼笉璁板綍鍗曟浠诲姟娴佹按銆?
## 2. 閫傜敤鑼冨洿

閫傜敤浜庝互涓?managed 椤圭洰锛?
- `Chaos.IL2CPP.CodeGen`
- `Chaos.IL2CPP.Loader`
- `Chaos.IL2CPP.Linker`
- `Chaos.IL2CPP.Contracts`
- `Chaos.IL2CPP.Driver`
- `Chaos.IL2CPP.SemanticWorld`
- `Chaos.IL2CPP.EngineBinding`

鍏朵腑 `Chaos.IL2CPP.CodeGen` 鏄瑕佹不鐞嗗璞°€?
## 3. 鏍稿績鍘熷垯

### 3.1 鍏堝垎鑱岃矗锛屽啀鎵╄兘鍔?
褰?`IL2CPP` 闇€瑕佹帴鍏ユ柊鐨?`AOT` feature 鏃讹紝涓嶈兘榛樿缁х画鎶婇€昏緫鍫嗚繘鐜版湁澶ф枃浠躲€傚簲鍏堝垽鏂 feature 灞炰簬锛?
- `Loader`
- `CoreIr`
- `Planning`
- `RuntimeSupport`
- `Emission`
- `Templating`

娌℃湁褰掑睘鍒ゆ柇锛屼笉搴旂洿鎺ュ紑鏀广€?
### 3.2 planner 鍙仛 plan锛宔mitter 鍙仛 render

- `*Planner` 璐熻矗璇箟褰掑苟銆丄BI 閫夋嫨銆乨ispatch/EH/runtime support 瑙勫垝銆?- `*Emitter` 璐熻矗娑堣垂 plan 骞舵覆鏌撴渶缁堜骇鐗┿€?- emitter 涓嶅緱閲嶆柊鎺ㄥ planner 宸茬粡鍐冲畾鐨勪笟鍔¤鍒欍€?
### 3.3 Scriban 鏄牳蹇冧唬鐮佸眰鐨勪竴閮ㄥ垎

`Scriban` 鍦ㄦ湰浠撳簱涓笉鏄鍥存ā鏉垮伐鍏凤紝鑰屾槸 `IL2CPP` 鏍稿績 codegen 灞傜殑姝ｅ紡缁勬垚閮ㄥ垎锛?
- 妯℃澘鏂囦欢灞炰簬鏍稿績瀹炵幇銆?- 妯℃澘妯″瀷灞炰簬鏍稿績瀹炵幇銆?- 妯℃澘鍑芥暟涓庢覆鏌撳熀纭€璁炬柦灞炰簬鏍稿績瀹炵幇銆?
濡傛灉鍚庣画浠ｇ爜鐢熸垚瑙勫垯涓嶆弧瓒筹紝浼樺厛鎵╁睍 `Scriban` 鏀拺灞傦紝鑰屼笉鏄洖閫€鍒版柊鐨勫瓧绗︿覆鎷兼帴銆?
### 3.4 娴嬭瘯妗嗘灦涓?AOT Core 缁х画闅旂

- `Chaos.TestFramework.*` 璐熻矗娴嬭瘯澹版槑銆佽繍琛屻€佹柇瑷€涓庡涓绘秷璐广€?- `IL2CPP` core 鍙礋璐?pipeline銆乴owering銆乺untime support 涓?codegen銆?- 涓嶅厑璁告妸 `TestFramework` 杩愯閫昏緫娣峰叆 `AOT Core IR`銆乸lanner 鎴?emitter銆?
### 3.5 涓嶆墿澶?string 鍗忚闈?
- capability/category/plan kind/template kind 鍙鑳界敤鏋氫妇鎴栧己绫诲瀷瀵硅薄琛ㄨ揪锛屽氨涓嶈鏂板鑷敱瀛楃涓插崗璁€?- 妯℃澘閫夋嫨搴旂敱寮虹被鍨?plan 鏄犲皠瀹屾垚锛屼笉搴旂敱澶栭儴瀛楃涓茶嚜鐢遍┍鍔ㄣ€?
## 4. 鐩綍鍒嗗眰瑙勮寖

`Chaos.IL2CPP.CodeGen` 鐩爣涓婂簲淇濇寔浠ヤ笅鐩綍鍒嗗眰锛?
```text
CodeGen/
  CoreIr/
  Planning/
  RuntimeSupport/
  Emission/
  ReferenceProof/
  Templating/
  Templates/
```

鑱岃矗璇存槑锛?
- `CoreIr/`: typed-IL 鍒?core IR 鐨勮涔夋壙鎺ヤ笌 contract銆?- `Planning/`: entry ABI銆乨ispatch銆丒H銆亀rapper/thunk銆乼ranslation unit plan銆?- `RuntimeSupport/`: object model銆乭elpers銆乵etadata銆乵arshaling 绛夋敮鎾戞ā鍨嬨€?- `Emission/`: plan 鍒拌緭鍑烘枃浠剁殑娓叉煋銆?- `ReferenceProof/`: reference proof 涓撶敤 planning/emission銆?- `Templating/`: `Scriban` 鐨?model adapter銆佸嚱鏁般€佹ā鏉垮姞杞戒笌娓叉煋銆?- `Templates/`: 妯℃澘鏂囦欢鏈綋銆?
## 5. 鏂囦欢灏哄瑙勮寖

- orchestrator 鏂囦欢鐩爣 `200-400` 琛屻€?- 瑙勫垯鍨?lowering/planning 鏂囦欢鐩爣 `300-600` 琛屻€?- 鍗曟枃浠惰秴杩?`800` 琛岃涓哄紓甯革紝闇€瑕佽鏄庣悊鐢便€?- 鍗曟枃浠惰秴杩?`1200` 琛岄粯璁よ涓哄繀椤绘媶鍒嗐€?
渚嬪鍙厑璁稿嚭鐜板湪绋冲畾 schema/鏄犲皠鏂囦欢锛屼笖闇€瑕佹樉寮忔敞閲婅鏄庛€?
## 6. 浠ｇ爜鐢熸垚瑙勫垯褰掑睘

### 6.1 蹇呴』淇濈暀鍦?C#

- IL 璇箟鍒嗘瀽
- ABI 閫夋嫨
- dispatch 绛栫暐
- EH shape 瑙勫垝
- runtime helper 閫夋嫨
- symbol 瑙勫垝
- plan/graph 鏋勯€?
### 6.2 蹇呴』杩涘叆妯℃澘灞?
- `.cpp`
- `.h`
- `.g.cs`
- `.json`
- 鍏跺畠缁撴瀯鍖栨枃鏈骇鐗?
### 6.3 妯℃澘鑳藉姏涓嶅鏃剁殑澶勭悊椤哄簭

1. 琛ユā鏉挎ā鍨嬪瓧娈点€?2. 琛?`TemplateFunctions` / filters / adapters銆?3. 琛ユā鏉垮姞杞芥垨娓叉煋鍩虹璁炬柦銆?4. 鍙湁褰撲骇鐗╀笉鏄粨鏋勫寲鏂囨湰鏃讹紝鎵嶅厑璁镐笉璧版ā鏉裤€?
绂佹浜嬮」锛?
- 鍥犱负褰撳墠妯℃澘涓嶆柟渚匡紝灏辩洿鎺ユ柊澧炲ぇ鍧?`StringBuilder.AppendLine(...)`銆?- 鍦ㄦā鏉夸腑瀹炵幇涓氬姟绾?lowering 閫昏緫銆?- 鍦?emitter 涓噸鏂板仛 ABI/dispatch/EH 鍐崇瓥銆?
## 7. 鍏跺畠鐑偣椤圭洰鐨勬媶鍒嗘柟鍚?
- `Loader`: `EntryDiscovery`銆乣HandleResolution`銆乣MethodBodyLoading`銆乣Diagnostics`
- `Linker`: `Closure`銆乣Reachability`銆乣Preservation`
- `Contracts`: `ManagedClosure`銆乣Schemas`銆乣Serialization`
- `Driver`: `Commands`銆乣Pipelines`銆乣Workspace`銆乣Diagnostics`

鍘熷垯鐩稿悓锛氬厛鎷嗚亴璐ｏ紝鍐嶈€冭檻鏄惁闇€瑕佹柊澧?assembly銆?
## 8. 鎵ц鍓嶆鏌?
褰撴柊澧炴垨閲嶆瀯 `IL2CPP` 鑳藉姏鏃讹紝鑷冲皯鍏堟鏌ヤ互涓嬮棶棰橈細

1. 杩欐閫昏緫搴旇钀藉湪鍝竴灞傘€?2. 鏄惁鍦ㄦ墿澶ц嚜鐢卞瓧绗︿覆鍗忚銆?3. 浜х墿鏄惁灞炰簬缁撴瀯鍖栨枃鏈紝鑻ユ槸鍒欐槸鍚﹀凡璧?`Scriban`銆?4. 鏄惁鎶婃祴璇曟鏋堕€昏緫璇杩涙牳蹇冨眰銆?5. 鏄惁璁╁崟鏂囦欢缁х画澶辨帶澧為暱銆?
## 9. 楠屾敹妫€鏌?
缁撴瀯鏀瑰姩瀹屾垚鍚庯紝鑷冲皯瑕佽兘鍥炵瓟锛?
1. planner銆乺untime support銆乪mitter銆乼emplate 鐨勮亴璐ｆ槸鍚﹀凡鍒嗙銆?2. 鏂板鐢熸垚瑙勫垯鏄惁宸茶繘鍏ユā鏉垮眰銆?3. 鏄惁浠嶅瓨鍦ㄩ噸澶嶇殑妯℃澘璺緞銆佸父閲忋€佹覆鏌撻€昏緫銆?4. 鐩爣 managed 椤圭洰鏄惁鍙瀯寤恒€?5. 鍏抽敭鍗曞厓娴嬭瘯涓庤嚦灏戜竴鏉＄湡瀹?`NativeAOT` 閾捐矾鏄惁淇濇寔鍙墽琛屻€?
## 10. 涓庝换鍔℃枃妗ｇ殑鍏崇郴

- 鎵ц杩囩▼銆侀樁娈垫€х姸鎬併€侀樆濉炰笌涓存椂鍐崇瓥鍐欏叆 `docs/dev/<lifecycle>/<task_id>/`銆?- 鏈〉鍙繚鐣欓暱鏈熺ǔ瀹氳鍒欍€?- 褰撶粨鏋勮鍒欏彂鐢熺ǔ瀹氬彉鏇存椂锛屽繀椤诲悓姝ユ洿鏂版湰椤典笌鏈洰褰?`INDEX.md`銆?
## 11. 当前已落地切片

- `ReferenceProof/NativeReferenceProofCatalog.cs`: native reference family 与 template path 的单一真相源
- `Templating/ScribanTemplateRenderer.cs`: shared template loading / caching / rendering
- `RuntimeSupport/NativeAotLoweringPlanner.RuntimeSupport.cs`: collection / monitor / thread / async prelude helpers
- `RuntimeSupport/NativeAotLoweringPlanner.RuntimePrelude.cs`: runtime prelude aggregator + span/memory prelude
- `Planning/NativeAotLoweringPlanner.MetadataSupport.cs`: custom attribute / reflection / static field metadata support builders
- `Planning/NativeAotLoweringPlanner.InvocationPlanning.cs`: reachability / dispatch / direct invocation target resolution
- `Emission/NativeAotLoweringPlanner.MethodEmission.cs`: 普通 opcode 发射与 managed method body 主路径
- `Emission/NativeAotLoweringPlanner.ExceptionEmission.cs`: catch/filter/finally 形状推导、线性 EH 发射与 exception-specific helper
- `RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs`: external runtime helper definition family 与 ABI helper 聚合
- `Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs`: object model / reflection object / symbol helper / custom attribute literal emission

## 12. 2026-04-17 收口更新

- `Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs` 已收回 object-model 主发射路径；object equality、reflection object helper、symbol/type-id utilities 分别拆到独立 partial file。
- `RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs` 已收回 dispatcher root；string/platform、collection/reflection、type resolution、invocation/ABI helper 分别拆到独立 partial file。
- `LoaderStage` 已按 `CrossAssemblyInstantiation / AssemblyLoading / InstructionDecoding / MetadataResolution / GenericMaterialization` 拆分。
- `LinkerStage` 已按 `Reachability / OptimizationFacts / DispatchResolution / OutputProjection` 拆分。
- `ManagedClosureContracts` 已按 request/artifact names、naming、models、method identity、semantic world、typed-il/aot-core-ir、artifact models 拆分。

## 13. 聚合真相源规则

- 当结构治理把一个 root file 拆成多个 partial / companion file 后，文本级 compatibility / governance 用例必须读取 `root + split files` 的聚合 source。
- root orchestrator 只保留入口与调度责任，不应再被当作“该模块全部结构事实”的唯一来源。
- 后续继续拆 `IL2CPP` 核心热点时，应先更新聚合 source helper，再推进结构搬移，避免旧断言阻塞重构。
