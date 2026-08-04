# Scriban Generator Formalization Design v1.01

Date: 2026-04-19 22:45:00 +08:00
Status: draft

## 1. 璁捐缁撹

鎶婄幇鏈?`src/managed/Chaos.IL2CPP.CodeGen/` 鍘熷湴閲嶅懡鍚嶄负 `src/managed/Chaos.IL2CPP.Generator/`锛屽苟淇濇寔 `third_party/scriban/src/Scriban/Scriban.csproj` 浣滀负鍞竴绗笁鏂规ā鏉垮紩鎿庡叆鍙ｃ€?
杩欎笉鏄柊澧炰竴灞?wrapper锛屼篃涓嶆槸鍐嶉€犱竴涓?generator 瀛愮郴缁燂紱鏈川涓婃槸瀵圭幇鏈夋牳蹇冪敓鎴愬伐绋嬪仛 canonical naming cutover銆?
## 2. 鍛藉悕鍐崇瓥

### 2.1 Canonical 鍚嶇О

- 宸ョ▼鐩綍锛歚src/managed/Chaos.IL2CPP.Generator/`
- 椤圭洰鏂囦欢锛歚Chaos.IL2CPP.Generator.csproj`
- namespace锛歚Chaos.IL2CPP.Generator`
- 涓婃父寮曠敤锛氬叏閮ㄦ敼涓?`Chaos.IL2CPP.Generator`

### 2.2 涓嶉噰绾?`CHaos.IL2CPP.Generator`

涓嶅缓璁噰鐢?`CHaos.IL2CPP.Generator`锛屽師鍥犲涓嬶細

- 浠撳簱鐜版湁鍛藉悕缁熶竴涓?`Chaos.*`銆?- 鍗曢」鐩ぇ灏忓啓婕傜Щ涓嶄細鎻愬崌琛ㄨ揪鍔涳紝鍙細澧炲姞妫€绱€佽矾寰勩€佸紩鐢ㄥ拰鏂囨。涓嶄竴鑷淬€?- 鑻ヨ鍝佺墝鏀瑰啓锛屽簲浣滀负浠撳簱绾у埆浠诲姟澶勭悊锛岃€屼笉鏄€熻繖娆?Scriban 姝ｅ紡鎺ュ叆鍋氬眬閮ㄤ緥澶栥€?
## 3. Scriban 鐨勬寮忎綅缃?
`Scriban` 鍦?cutover 鍚庣殑瑙掕壊淇濇寔涓嶅彉锛屼絾娌荤悊鍦颁綅鎻愬崌涓烘樉寮?canonical 瑙勫垯锛?
- 浠嶉€氳繃 `ProjectReference` 鐩存帴寮曠敤 vendored 宸ョ▼锛?  - `third_party/scriban/src/Scriban/Scriban.csproj`
- 浠嶇敱鍏变韩娓叉煋鍏ュ彛缁熶竴鍔犺浇涓庢覆鏌擄細
  - `Templating/ScribanTemplateRenderer.cs`
- 浠嶄綔涓烘枃浠剁骇缁撴瀯鍖栨枃鏈敓鎴愮殑榛樿璺緞锛?  - `.cpp`
  - `.h`
  - `.g.cs`
  - `.json`

鏄庣‘涓嶅仛锛?
- 涓嶅垏鍒?NuGet 鍖呭紩鐢ㄣ€?- 涓嶅鍒?`Scriban` 婧愮爜鍒?`src/managed/Chaos.IL2CPP.Generator/`銆?- 涓嶄负 `Scriban` 鍐嶅涓€涓粨搴撳唴绉佹湁 fork assembly銆?
## 4. 鏋舵瀯杈圭晫

鏈鍙樻洿鍙鐞嗗懡鍚嶅拰 canonical 鎺ュ叆锛屼笉鎺ㄧ炕 2026-04-17 宸插喕缁撶殑缁撴瀯娌荤悊璺嚎銆?
鍥犳淇濈暀浠ヤ笅杈圭晫锛?
- 涓€涓牳蹇冪敓鎴愬伐绋嬶紝缁х画鎵胯浇锛?  - `CoreIr`
  - `Planning`
  - `RuntimeSupport`
  - `Emission`
  - `ReferenceProof`
  - `Templating`
  - `Templates`
- 涓嶆柊澧炲苟琛?managed assembly銆?- 涓嶆妸 `Scriban` 鍙樻垚鈥滃鍥村伐鍏峰眰鈥濄€?- 涓嶉噸鏂版斁瀹芥墜鍐?`StringBuilder` 浣滀负涓荤敓鎴愰潰銆?
## 5. 鏁版嵁娴?
cutover 鍓嶅悗锛屾牳蹇冪敓鎴愰摼涓嶅彉锛?
`Loader/SemanticWorld/Linker/MetadataWriter -> GeneratorStage -> lowering plans -> NativeReference/NativeAot emitters -> Scriban templates -> generated artifacts`

鍙樺寲鍙湁 canonical 鏍囪瘑锛?
- `Chaos.IL2CPP.CodeGen` -> `Chaos.IL2CPP.Generator`
- `using Chaos.IL2CPP.CodeGen;` -> `using Chaos.IL2CPP.Generator;`
- 椤圭洰璺緞銆佹祴璇曢敋鐐广€乤uthority 鏂囨。鍚屾杩佺Щ

## 6. 褰卞搷闈?
### 6.1 蹇呮敼婧愮爜

- `src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj`
- `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`
- `src/managed/Chaos.IL2CPP.Pipeline/Chaos.IL2CPP.Pipeline.csproj`
- `src/managed/Chaos.IL2CPP.Pipeline/PipelinePlan.cs`
- `src/managed/Chaos.IL2CPP.Generator/**` 涓嬫墍鏈?namespace 涓庣浉瀵瑰紩鐢?
### 6.2 蹇呮敼娴嬭瘯

- `tests/unit/run/test_repo_layout.py`
- `tests/unit/planning/test_project_workspace.py`
- 鐩存帴閿氬畾 `src/managed/Chaos.IL2CPP.CodeGen/**` 鐨?compatibility tests
- `tests/support.py`

### 6.3 蹇呮敼 authority / wiki

- `docs/archive/architecture/subject-test-framework-v1/INDEX.md`
- `docs/archive/architecture/verification-v1/spec.md`
- `docs/archive/architecture/runtime-baseline/repo-layout.md`
- `docs/archive/architecture/runtime-baseline/ownership-map.md`
- `wiki/04-宸ュ叿涓庨泦鎴?il2cpp-core-structure-and-scriban-governance.md`

### 6.4 涓嶅缓璁洖鍐欑殑鍘嗗彶鏂囨。

涓嶅缓璁ぇ瑙勬ā淇敼 `docs/dev/completed/**` 閲岀殑鍘嗗彶浠诲姟鏂囨。銆?
鍘熷洜锛?
- 瀹冧滑璁板綍鐨勬槸褰撴椂鐪熷疄鍙戠敓鐨勪换鍔′笌璺緞銆?- 鍏ㄩ噺鍥炲啓鏀剁泭寰堜綆锛屼笖浼氭ā绯婂巻鍙蹭笂涓嬫枃銆?- 鍙渶瑕佷繚璇佸綋鍓?authority 鏂囨。鍜屾椿鍔ㄤ唬鐮佹爲浣跨敤鏂板悕绉般€?
## 7. 閿欒澶勭悊涓庨闄?
### 7.1 椋庨櫓

- 鐩綍涓?`.csproj` 鏀瑰悕鍚庯紝娴嬭瘯涓殑纭紪鐮佽矾寰勪細澶ч噺澶辨晥銆?- 鏌愪簺 Python 瑙勫垝閫昏緫鎶?`Chaos.IL2CPP.CodeGen.csproj` 浣滀负 canonical 椤圭洰璺緞锛岃嫢婕忔敼浼氱洿鎺ョ牬鍧?workspace 鏋勯€犮€?- 濡傛灉淇濈暀鏃х洰褰曟垨 wrapper锛屼細璁?cutover 闀挎湡鎮€屾湭鍐炽€?
### 7.2 搴斿

- 涓€娆℃€?cutover锛屼笉淇濈暀鍙岃建銆?- 鍏堟敼 source/test/authority doc锛屽啀鍋氶獙璇併€?- 鍘嗗彶鏂囨。涓嶈拷姹傚叏閲忓洖鍐欙紝鍙洿鏂扳€滃綋鍓嶇湡鐩告簮鈥濄€?
## 8. 楠岃瘉寤鸿

鏈€灏戦獙璇侀摼锛?
1. `python -m pytest tests/unit/run/test_repo_layout.py -q`
2. `python -m pytest tests/unit/compatibility/test_il2cpp_codegen_structure_governance.py -q`
3. `python -m pytest tests/unit/compatibility/test_scriban_vendor_build.py -q`
4. `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`

鎺ㄨ崘琛ヤ竴鏉＄湡瀹炵敓鎴愰摼锛?
5. 浠婚€変竴涓幇鏈?native generation 鍏ュ彛锛岄獙璇?`Driver -> Generator -> Scriban` 浠嶇劧鑳戒骇鍑?generated artifacts銆?
## 9. 瀹炴柦椤哄簭

### Step 1

鍐荤粨 canonical 鍚嶇О涓?`Chaos.IL2CPP.Generator`銆?
### Step 2

鍘熷湴閲嶅懡鍚嶏細

- 鐩綍
- `.csproj`
- namespace
- 涓婃父 `ProjectReference`

### Step 3

淇娴嬭瘯涓?Python 瑙勫垝灞備腑鐨勮矾寰?宸ョ▼鍚嶉敋鐐广€?
### Step 4

鏇存柊 authority 鏂囨。鍜?wiki锛屾妸 `Generator` 璁句负褰撳墠鐪熺浉婧愩€?
### Step 5

璺戞渶灏忛獙璇侀摼骞剁‘璁ゆ病鏈夋畫鐣?`CodeGen` 鍙岃建鍏ュ彛銆?
## 10. 缁撹

濡傛灉浣犵殑鐩爣鏄€滄寮忔妸 Scriban 绾冲叆 IL2CPP 鏍稿績灞傦紝骞舵妸璇ュ眰鍛藉悕涓?Generator鈥濓紝閭ｆ纭搷浣滀笉鏄柊寤轰竴涓?`Generator` 宸ョ▼锛岃€屾槸鎶婄幇鏈?`Chaos.IL2CPP.CodeGen` 鍘熷湴 cutover 鎴?`Chaos.IL2CPP.Generator`锛屽悓鏃朵繚鐣?vendored `Scriban` 鐨勭洿鎺ュ紩鐢ㄦ柟寮忋€?
