# Trace Contract v0 楠岃瘉璇存槑

## 瀹氫綅

鏈枃瀹氫箟鐨勬槸 warmup trace 鐨勯獙璇?contract锛屼笉鏄牳蹇?ABI contract锛屼篃涓嶆槸 runtime bridge contract銆傚畠鐨勭敤閫旀槸楠岃瘉 lazy warmup 鐨勪簨浠堕『搴忎笌鏈€缁堢粨鏋滄槸鍚︾鍚堥鏈燂紝鑰屼笉鏄壙杞借繍琛屾椂 ABI 缁嗚妭銆?

## 閫傜敤鑼冨洿

- 楠岃瘉瀵硅薄锛歭azy warmup 璺緞涓殑绫诲瀷銆佹柟娉曘€佹硾鍨嬩笂涓嬫枃銆佸鎵樼瓑涓讳綋銆?
- 楠岃瘉鐩爣锛氶『搴忔槸鍚︽纭€佹渶缁堢姸鎬佹槸鍚︽纭€佺紦瀛樻垨 replay 璺緞鏄惁鎸夐鏈熷嚭鐜般€?
- 闈炵洰鏍囷細native bridge 鍙傛暟甯冨眬銆丄BI 鍏煎灞傘€佸钩鍙版ˉ鎺ヤ簩杩涘埗缁嗚妭銆?

## 椤跺眰缁撴瀯

- 椤跺眰 `formatVersion` 鍥哄畾涓?`"v0"`銆?
- 椤跺眰 `traceName` 鍥哄畾涓?`warmupTrace`銆?
- `targetPlatform` 褰撳墠鍐荤粨涓?`windows` 鎴?`macos`銆?
- `samples[]` 鎵胯浇鍙鐨勯獙璇佹牱鏈紱鏁扮粍鑷冲皯鍖呭惈 1 涓?sample锛屼笖姣忎釜 sample 鑷冲皯鍖呭惈 `sampleId`銆乣scenario`銆乣events[]`銆?

## 浜嬩欢鏈€灏忓喕缁撳瓧娈?

姣忔潯浜嬩欢鑷冲皯鍖呭惈浠ヤ笅瀛楁锛?

- `eventName`
- `phase`
- `subjectKind`
- `subjectId`
- `order`
- `status`

杩欎簺瀛楁鍚嶅湪 v0 鍐呯姝㈤殢鎰忔敼鍔ㄣ€傚厑璁稿湪浜嬩欢瀵硅薄涓婅拷鍔犺皟璇曘€佹潵婧愩€佽€楁椂銆佽Е鍙戝櫒绛夊瓧娈碉紝浣嗕笉鑳芥浛鎹㈣繖缁勬渶灏忓喕缁撳瓧娈点€?

## 鏋氫妇绾︽潫

- `phase` 鍙厑璁革細`requested`銆乣started`銆乣completed`銆乣failed`銆乣skipped`銆乣cached`銆乣replayed`
- `status` 鍙厑璁革細`ok`銆乣error`銆乣skipped`銆乣cached`
- `subjectKind` 鍙厑璁革細`type`銆乣method`銆乣field`銆乣property`銆乣event`銆乣parameter`銆乣genericContext`銆乣delegate`銆乣assembly`

## 鍛藉悕瑙勫垯

- `eventName` 榛樿浣跨敤涓夋寮?`<domain>.<subject>.<action>`銆?
- 褰?subject 涓嶉渶瑕佸崟鐙樉寮忚〃杈炬椂锛屽厑璁镐袱娈靛紡 `<domain>.<action>`銆?
- `domain` 鍙厑璁革細`warmup`銆乣metadata`銆乣reflection`銆乣generic`銆乣delegate`銆乣bootstrap`銆乣interop`銆?
- v0 涓嶅喕缁?action 璇嶈〃锛屼絾瑕佹眰 action 浣跨敤绋冲畾銆佸彲璇汇€佷綆姝т箟鐨勮嫳鏂囧姩璇嶆垨鍚嶈瘝銆?

## `subjectId` 瑙勫垯

- `subjectId` 浣跨敤浜鸿绛惧悕寮忓瓧绗︿覆锛岃€屼笉鏄唴閮ㄦ寚閽堟垨闅忔満鏍囪瘑銆?
- 鏈€灏戝簲鍖呭惈绋嬪簭闆嗐€佺被鍨嬨€佹垚鍛樼鍚嶈繖涓夌被淇℃伅涓殑閫傜敤瀛愰泦銆?
- 绫诲瀷绀轰緥锛歚Game.Core/Game.Player`
- 鏂规硶绀轰緥锛歚Game.Core/Game.Player::TakeDamage(System.Int32)`
- 娉涘瀷鍦烘櫙蹇呴』甯﹀嚭瀹炰緥鍖栧舰鎬侊紝渚嬪锛歚Game.Core/Game.Inventory::AddItem<System.String>(System.String)`
- 濮旀墭鎴栫粍鍚堝満鏅彲浠ュ湪淇濇寔浜鸿鎬х殑鍓嶆彁涓嬭拷鍔?`=>` 绛夎繛鎺ヨ娉曪紝鍙鏍稿績绛惧悕浠嶇ǔ瀹氬彲璇嗗埆銆?

## Snapshot 绾︽潫

- Windows snapshot 鑷冲皯鍖呭惈涓€浠?sample锛屽苟瑕嗙洊 lazy warmup 璺緞銆?
- macOS snapshot 鑷冲皯鍖呭惈涓€浠?sample锛屽苟瑕嗙洊 lazy warmup 璺緞銆?
- 姣忎釜 sample 鐨?`events[]` 鑷冲皯鍖呭惈 1 鏉′簨浠讹紱绌?sample 涓嶆瀯鎴愬悎娉曢獙璇佸熀绾裤€?
- 涓や釜骞冲彴鏍锋湰鍚堣蹇呴』瑕嗙洊 `cached` 鎴?`replayed` 璺緞锛涘綋鍓嶅熀绾夸腑 Windows 瑕嗙洊 `cached`锛宮acOS 瑕嗙洊 `replayed`銆?
- snapshot 鏄獙璇佸熀绾匡紝涓嶆槸 runtime 鐢熶骇 ABI 鐨勪竴閮ㄥ垎銆?

## 婕旇繘鍘熷垯

- v0 鍏佽涓轰簨浠惰拷鍔犺€楁椂銆佺嚎绋嬨€佹潵婧愮粍浠躲€侀敊璇弿杩扮瓑瀛楁銆?
- 濡傛灉鏈潵闇€瑕佹敼鍙樻渶灏忓喕缁撳瓧娈靛悕绉般€佹灇涓捐涔夋垨椤跺眰缁撴瀯锛屽簲閫氳繃鎻愬崌 `formatVersion` 瀹屾垚锛岃€屼笉鏄湪 v0 鍐呴潤榛樻紓绉汇€?

