# AOT Skill Flow And Token Optimization Design v1.01

Date: 2026-04-17
Status: design-frozen

## 1. 鐩爣

鎶婂綋鍓?skill 娴佺▼浼樺寲鎴愪竴鏉℃洿閫傚悎 IL2CPP/AOT 涓荤嚎鐨勬寮忓伐浣滄祦锛屼娇瀹冨湪澶勭悊 `managed -> native -> hotupdate`銆乣proof / benchmark obligation` 鍜?`completed` 鏀跺彛鏃舵弧瓒充互涓嬭姹傦細

- capability銆乷wner subject銆乸roof / benchmark obligation 鍦?plan 闃舵鏄惧紡鍐荤粨锛岃€屼笉鏄殣寮忎緷璧栦綔鑰呰蹇嗭紱
- `completed` 涔嬪墠蹇呴』缁忚繃 formal verification gate锛岃€屼笉鏄彧璺戜竴杞硾鍖栤€滈」鐩祴璇曞浠垛€濓紱
- 楂橀 skill 鍙繚鐣?routing / gate / decision锛岄暱绡?reference 鍥炶惤鍒?architecture 涓?`wiki/06-娴嬭瘯楠岃瘉/`锛?
- 澶氫釜 skill 涔嬮棿瀵瑰悓涓€鏉¤鍒欏彧淇濈暀涓€涓?authority锛屼笉缁х画澶嶅埗绮樿创寮忓闀裤€?

## 2. 褰撳墠闂

### 2.1 authority 宸叉湁锛屼絾 control skill 娌℃湁瀹屽叏娑堣垂

褰撳墠 authority 宸插瓨鍦ㄤ簬锛?

- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/subject-test-framework-v1/INDEX.md`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/verification-v1/spec.md`
- `wiki/06-娴嬭瘯楠岃瘉/INDEX.md`
- `wiki/06-娴嬭瘯楠岃瘉/AOT鏂癋eature鎺ュ叆鑷祴瑙勮寖.md`

浣?control skill 浠嶅瓨鍦ㄤ袱涓己鍙ｏ細

- `writing-plans` 娌℃湁瑕佹眰鏄惧紡鍐欏嚭 `capabilityFamily / capabilityItem / ownerSubjectId / proofRequired / benchmarkRequired / hotupdateImpact`锛?
- `executing-plans` 涓?`subagent-driven-development` 杩樻病鏈夋妸 AOT formal verification chain 鍙樻垚 `completed` 涔嬪墠鐨勭‖闂ㄣ€?

### 2.2 楂橀 skill 閲嶅鎼哄甫鍚屼竴鏉¤鍒?

浠ヤ笅瑙勫垯鐩墠琚涓?skill 閲嶅鍙欒堪锛?

- `dotnet build/test/msbuild` 缂栬瘧宕╂簝蹇呴』杩芥牴鍥犲苟淇
- `completed` 鍓嶅繀椤婚獙璇?
- benchmark 涓嶈兘鏇夸唬 correctness
- AOT 涓荤嚎蹇呴』璧?managed/native/hotupdate 姝ｅ紡楠岃瘉

閲嶅鍙欒堪甯︽潵鐨勯棶棰樹笉鏄€滃瓧鏁颁笉濂界湅鈥濓紝鑰屾槸锛?

- token 娑堣€楀亸楂橈紱
- authority 瀹规槗婕傜Щ锛?
- 淇敼瑙勫垯鏃跺鏄撴紡鏀规煇涓€浠介暅鍍忋€?

### 2.3 AOT onboarding 缂哄皯蹇矾寰?

褰撳墠浠撳簱宸茬粡鏈夐潪甯告槑纭殑 AOT feature onboarding 瑙勫垯锛屼絾 skill 涓荤嚎杩樻病鏈変负杩欑被楂橀銆佹爣鍑嗗寲浠诲姟鎻愪緵瓒冲杞婚噺鐨勫叆鍙ｃ€傜粨鏋滄槸锛?

- 澶嶆潅搴﹀凡缁忚 architecture / wiki 鍐荤粨锛屼絾 skill 浠嶇劧姣忔閮借閲嶅鎼哄甫澶ч噺璇存槑锛?
- `using-booming` 鑳芥纭垎娴侊紝浣嗗悗缁?skill 浠嶅彲鑳芥妸浠诲姟鎵ц鎴愨€滄硾鍖栬鍒掆€濊€屼笉鏄€淎OT obligation-driven plan鈥濄€?

## 3. 璁捐鍘熷垯

### 3.1 authority 涓?control 鍒嗗眰

- authority锛歚wiki/03-功能模块/06-il2cpp核心架构/05-架构参考` 涓?`wiki/06-娴嬭瘯楠岃瘉/`
- control锛歚.codex/skills/**`

authority 璐熻矗璇存槑闀挎湡瑙勫垯锛宑ontrol 鍙礋璐ｏ細

- 浠€涔堟椂鍊欏繀椤昏繘鍏ュ摢鏉¤鍒欙紱
- 瀹屾垚鍓嶅繀椤昏繃鍝簺 gate锛?
- 濡傛灉缂鸿鍒欏璞★紝涓嬩竴姝ュ幓鍝噷琛ャ€?

### 3.2 plan 蹇呴』鍐荤粨 obligation

鎵€鏈?AOT / IL2CPP 涓荤嚎鐨勬寮忚鍒掞紝閮藉繀椤诲湪 plan 鏂囨。閲屽喕缁擄細

- `capabilityFamily`
- `capabilityItem`
- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- 鍑嗗鎵ц鐨?declared proof / benchmark 鎴?formal verification object

### 3.3 completed 蹇呴』缁戝畾 formal verification chain

瀵?AOT feature onboarding 绫讳换鍔★紝`completed` 涔嬪墠鐨勬寮忔墽琛岄摼鍥哄畾涓猴細

`collector/registry/workspace gate -> owner managed proof -> owner native proof -> hotupdate proof(鎸夐渶) -> benchmark(鎸夐渶) -> affected regressions`

鍏朵腑 benchmark 鍙綔涓鸿ˉ鍏呰瘉鎹紝涓嶆浛浠?correctness銆?

### 3.4 楂橀 skill 閲囩敤钖勫叆鍙?

楂橀 skill 搴旂缉鎴愶細

- 鐭?description
- 鏍稿績 gate
- 鎸囧悜 authority 鐨勬槑纭叆鍙?

涓嶅簲鍦ㄦ瘡涓?skill 姝ｆ枃閲岄兘瀹屾暣閲嶈涓€閬?owner subject銆乸roof obligation銆乧ollector gate 涓?benchmark 杈圭晫銆?

## 4. 鐩爣缁撴瀯

### 4.1 control skill 鍒嗗伐

- `using-booming`: 璐熻矗 routing锛屾槑纭摢浜涗换鍔″繀椤诲厛杩?`project-test-governance`
- `project-test-governance`: 璐熻矗娌荤悊杈圭晫涓?authority 鍏ュ彛锛屼笉璐熻矗瀹屾暣閲嶈鎵€鏈?AOT onboarding 缁嗚妭
- `writing-plans`: 璐熻矗鎶?capability / owner / obligation 鍐欒繘 plan
- `executing-plans` / `subagent-driven-development`: 璐熻矗鍦?`completed` 鍓嶅己鍒?formal verification gate
- `verification-before-completion`: 璐熻矗閫夋嫨姝ｅ紡瀵硅薄銆佹墽琛?`canonicalCommand`銆佸鐞?verification escalation

### 4.2 authority 鏂囨。鍒嗗伐

- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/subject-test-framework-v1/INDEX.md`: 执行主线边界、长期契约、分层原则
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/verification-v1/spec.md`: formal verification、projection 与 archive 边界
- `wiki/06-娴嬭瘯楠岃瘉/AOT鏂癋eature鎺ュ叆鑷祴瑙勮寖.md`: onboarding 椤哄簭銆乬ate銆乸roof / benchmark obligation
- `wiki/06-娴嬭瘯楠岃瘉/INDEX.md`: 姝ｅ紡鍏ュ彛涓庡璞″鑸?

## 5. 鎴愬姛鏍囧噯

瀹屾垚鍚庡簲婊¤冻锛?

- 涓€涓?AOT 鏂?feature 杩涘叆 plan 鏃讹紝涓嶉渶瑕佷綔鑰呴澶栧彛澶磋ˉ鍏?owner/proof/benchmark obligation锛?
- `completed` 鍓嶆棤娉曞彧闈?managed green 鎴?benchmark green 灏卞甯冨畬鎴愶紱
- `project-test-governance` skill 姝ｆ枃涓庡叾 wiki 闀滃儚銆乤rchitecture銆丄OT 鑷祴瑙勮寖涓嶅啀婕傜Щ锛?
- 楂橀鐑矾寰?token 鎴愭湰鐩告瘮褰撳墠缁勫悎涓嬮檷绾?25%~35%锛屼富瑕佹潵鑷幓閲嶏紝鑰屼笉鏄垹瑙勫垯锛?
- 閫氳繃 scenario-based 楠岃瘉锛岀‘璁?skill 浠嶈兘姝ｇ‘澶勭悊锛?
  - AOT 鏂?feature 鎺ュ叆
  - benchmark 寮傚父鎺掓煡
  - `completed` 鍓?formal verification gate
