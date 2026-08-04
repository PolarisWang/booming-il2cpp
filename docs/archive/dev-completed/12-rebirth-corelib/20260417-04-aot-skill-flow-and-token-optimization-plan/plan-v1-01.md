# AOT Skill Flow And Token Optimization 瀹炵幇璁″垝 v1.01

> **闈㈠悜鎵ц Agent锛?* 蹇呴』浣跨敤 dev:subagent-driven-development锛堝鏋滃彲鐢ㄥ瓙 Agent锛夋垨 dev:executing-plans 鏉ユ墽琛屾湰璁″垝銆傛楠や娇鐢ㄥ閫夋锛坄- [ ]`锛夎娉曡窡韪€?

**鐩爣锛?* 鎶婂綋鍓?skill 涓荤嚎浼樺寲鎴愪竴鏉?obligation-driven 鐨?AOT 宸ヤ綔娴侊紝骞堕檷浣庨珮棰戠儹璺緞 token 鎴愭湰銆?

**鏋舵瀯锛?* 鍏堝喕缁?authority锛屽啀鏀?control skill銆傞珮棰?skill 鍙繚鐣?routing/gate锛屾妸 owner subject銆乸roof/native/hotupdate/benchmark 鐨勬繁瑙勫垯闆嗕腑鍒?architecture 涓?`wiki/06-娴嬭瘯楠岃瘉/`銆俙completed` 鍓嶇粺涓€寮哄埗 formal verification gate銆?

**鎶€鏈爤锛?* Markdown skill docs銆乸roject wiki銆乤rchitecture docs銆佹枃鏈牎楠屽懡浠ゃ€乻cenario-based skill validation

**璁捐鏂囨。锛?* `docs/dev/in-progress/20260417-04-aot-skill-flow-and-token-optimization-plan/design-v1-01.md`

**预期知识沉淀：** `wiki/02-Skill体系/`、`wiki/06-测试验证/`、`wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/subject-test-framework-v1/INDEX.md`、`wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/verification-v1/spec.md`

**鏀跺熬绾︽潫锛?* 鎵ц瀹屾垚鍚庡繀椤昏繘鍏モ€滃瑙嗘灦鏋勫悎鐞嗘€?-> 娴嬭瘯閫氳繃 -> 褰掓。 completed -> 鍚堝苟&鎻愪氦鈥濆浐瀹氶摼璺€?

---

## 娑夊強鏂囦欢

- `.codex/skills/dev-using-booming/SKILL.md`
  - 鏀剁揣 routing锛岄伩鍏嶉噸澶嶆壙杞?AOT 娣辫鍒?
- `.codex/skills/dev-project-test-governance/SKILL.md`
  - 浣滀负娌荤悊鍏ュ彛锛屾樉寮忕粦瀹?authority 涓?AOT onboarding 鍏ュ彛
- `.codex/skills/dev-writing-plans/SKILL.md`
  - 澧炲姞 capability / owner / obligation 璁″垝瀛楁
- `.codex/skills/dev-executing-plans/SKILL.md`
  - 鎶?AOT formal verification chain 缁戝畾鍒?`completed` gate
- `.codex/skills/dev-subagent-driven-development/SKILL.md`
  - 涓庨『搴忔墽琛屼富绾垮榻愮浉鍚岀殑 formal verification gate
- `.codex/skills/dev-verification-before-completion/SKILL.md`
  - 鏄庣‘ AOT onboarding 鏃剁殑瀵硅薄閫夋嫨涓?escalation 瑙勫垯
- `.codex/skills/dev-test-driven-development/SKILL.md`
  - 鍙繚鐣欎笌 failure-first discipline 鐩稿叧鐨勪粨搴撶粦瀹氾紝鍘绘帀鍙敱 authority 鎵挎媴鐨勯噸澶嶈鏄?
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/subject-test-framework-v1/INDEX.md`
  - 作为 authority，固定 execution spine / layering 的正式入口
- `wiki/06-娴嬭瘯楠岃瘉/INDEX.md`
  - 淇濇寔姝ｅ紡楠岃瘉鍏ュ彛涓庡璞″鑸竴鑷?
- `wiki/06-娴嬭瘯楠岃瘉/AOT鏂癋eature鎺ュ叆鑷祴瑙勮寖.md`
  - 浣滀负 owner/proof/native/hotupdate/benchmark obligation 娣辫鍒欑湡婧?
- `wiki/02-Skill浣撶郴/01-涓荤嚎宸ヤ綔娴?*.md`
  - 鍚屾涓荤嚎鎶€鑳介暅鍍?
- `wiki/02-Skill浣撶郴/04-璐ㄩ噺淇濋殰/*.md`
  - 鍚屾璐ㄩ噺淇濋殰 skill 闀滃儚
- `wiki/02-Skill浣撶郴/skill-registry.md`
  - 鍚屾 skill 鑱岃矗杈圭晫

## 璁″垝娓呭崟

- [ ] Task 1: 鍐荤粨 authority matrix锛屽苟琛ラ綈 `project-test-governance` 涓?AOT onboarding 瑙勫垯涔嬮棿鐨勭湡婧愮粦瀹?
  - 明确 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/subject-test-framework-v1/INDEX.md`、`wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/verification-v1/spec.md` 与 `wiki/06-测试验证/AOT新Feature接入自测规范.md` 的职责分层
  - 璁?`project-test-governance` skill 姝ｆ枃琛ラ綈锛?
    - `subject.features.json` 鏄?owner/proof/benchmark obligation authority
    - canonical proof / benchmark / host correctness 绂佹 stdout 鍒ゅ畾
    - AOT onboarding 蹇呴』鍥炲埌 `wiki/06-娴嬭瘯楠岃瘉/AOT鏂癋eature鎺ュ叆鑷祴瑙勮寖.md`
  - 鍒犻櫎涓?authority 鍐茬獊鎴栧凡缁忚繃鏃剁殑鏃ф弿杩?

- [ ] Task 2: 閲嶅缓 planning intake contract锛岃 plan 鏄惧紡鍐荤粨 obligation
  - 淇敼 `dev-writing-plans` 妯℃澘锛屾柊澧炲繀濉瓧娈碉細
    - `capabilityFamily`
    - `capabilityItem`
    - `ownerSubjectId`
    - `proofRequired`
    - `benchmarkRequired`
    - `hotupdateImpact`
    - `formalVerificationObjects`
  - 濡傛湁蹇呰锛屽悓姝?`brainstorming` 鐨?design quality 鎻愮ず锛岄伩鍏?plan 涓婃父婕忔帀 capability/owner 鍒ゆ柇
  - 鏄庣‘ AOT onboarding 绫昏鍒掍笉寰楀彧鍐欌€滆窇娴嬭瘯鈥濓紝蹇呴』鍐欐竻 proof/native/hotupdate/benchmark obligation

- [ ] Task 3: 鎶?formal verification gate 鎺ュ叆 `completed` 鍒ゅ畾
  - 淇敼 `dev-executing-plans` 涓?`dev-subagent-driven-development`
  - 瀵瑰懡涓?AOT / IL2CPP 涓荤嚎鐨勪换鍔★紝`completed` 鍓嶅繀椤绘樉寮忕‘璁わ細
    - collector / registry / workspace gate
    - owner managed proof
    - owner native proof
    - hotupdate proof锛堟寜闇€锛?
    - benchmark锛堟寜闇€锛?
    - affected regressions
  - 璁╂墽琛屼富绾挎樉寮忚皟鐢ㄦ垨娑堣垂 `dev-verification-before-completion` 鐨勬寮忓璞￠€夋嫨瑙勫垯锛岃€屼笉鏄悇鑷噸澶嶅畾涔?

- [ ] Task 4: 鏀剁揣 `verification-before-completion`锛岃瀹冩垚涓?formal object selector
  - 淇濈暀 registry / `canonicalCommand` / escalation 瑙勫垯
  - 琛ラ綈涓?AOT onboarding 瑙勮寖鐨勭洿鎺ユ槧灏勶細
    - 褰撹鍒掑啓鏄?`formalVerificationObjects` 鏃讹紝浼樺厛鎵ц杩欎簺姝ｅ紡瀵硅薄
    - 褰撳璞＄己澶辨椂锛屽厛琛ュ璞℃垨 case锛屽啀鍏佽瀹屾垚
  - 閬垮厤瀹冪户缁壙杞藉彲浠ユ斁鍥?authority 鏂囨。鐨勯暱绡?AOT 瑙ｉ噴

- [ ] Task 5: 鍋氶珮棰戠儹璺緞 token slimming
  - 澶嶆牳 `using-booming`銆乣project-test-governance`銆乣executing-plans`銆乣verification-before-completion`銆乣systematic-debugging`銆乣test-driven-development` 鐨勬鏂?
  - 鐩爣鏄妸閲嶅瑙勫垯鍘嬬缉鎴愨€滃崟涓€ authority + 澶氱偣寮曠敤鈥?
  - 閲嶇偣鍘婚噸锛?
    - `dotnet` 缂栬瘧宕╂簝闂搁棬
    - benchmark 涓嶆浛浠?correctness
    - `completed` 鍓嶅繀椤绘湁姝ｅ紡楠岃瘉璇佹嵁
    - AOT onboarding 鐨?owner/proof/native/hotupdate/benchmark 椤哄簭
  - 鑻ュ幓閲嶅悗鐑矾寰勪粛鐒跺亸閲嶏紝鍐嶈瘎浼版槸鍚︽柊澧炰竴涓交閲?`aot-feature-intake` skill 浣滀负楂橀鍏ュ彛

- [ ] Task 6: 鍋氭枃鏈洖褰掍笌 scenario-based skill 楠岃瘉
  - 鏂囨湰涓€鑷存€ф鏌ワ細
    - skill 姝ｆ枃涓?wiki 闀滃儚涓€鑷?
    - skill 姝ｆ枃涓?`wiki/03-功能模块/06-il2cpp核心架构/05-架构参考` / `wiki/06-娴嬭瘯楠岃瘉/` 涓嶅啿绐?
  - token 棰勭畻澶嶇畻锛?
    - 瀵规瘮鏀归€犲墠鍚庣殑楂橀鐑矾寰勭粍鍚堜綋绉?
  - scenario-based 楠岃瘉鑷冲皯瑕嗙洊锛?
    - 鈥滄柊澧炰竴涓?AOT feature锛岄渶瑕?owner subject銆乵anaged/native/hotupdate/proof/benchmark obligation鈥?
    - 鈥渂enchmark 寮傚父鎺掓煡锛岄渶瑕?routing 鍒?debugging + governance + TDD鈥?
    - 鈥滀换鍔″噯澶囧畬鎴愶紝璇曞浘鍦?formal verification object 缂哄け鏃跺綊妗?completed鈥?
  - 灏嗛獙璇佺粨璁哄啓鍥炰换鍔＄洰褰曚笌 wiki 闀滃儚

## 棰勬湡楠岃瘉

- `git diff --check -- .codex/skills wiki/03-功能模块/06-il2cpp核心架构/05-架构参考 wiki`
- 瀵圭洰鏍囨枃浠舵墽琛?`Select-String`锛岀‘璁や互涓嬪叧閿瘝鍜岄摼璺竴鑷达細
  - `ownerSubjectId`
  - `proofRequired`
  - `benchmarkRequired`
  - `hotupdateImpact`
  - `collector -> registry -> workspace`
  - `managed proof`
  - `native proof`
  - `hotupdate proof`
  - `canonicalCommand`
  - `required-before-completion`
- 澶嶇畻楂橀鐑矾寰勪綋绉紝纭甯歌 AOT 璁″垝鎵ц閾捐矾鐨勬€诲瓧绗︽暟鏄庢樉涓嬮檷
- 璁板綍鑷冲皯涓夌粍 skill scenario baseline/green 缁撴灉锛岃瘉鏄?routing銆乬ate 涓?completed 鍒ゅ畾娌℃湁閫€鍖?
