# Scriban Generator Formalization 瀹炵幇璁″垝

> **闈㈠悜鎵ц Agent锛?* 蹇呴』鎸?TDD 椤哄簭鎵ц锛氬厛琛?鏀瑰け璐ユ祴璇曪紝鍐嶆敼瀹炵幇锛屽啀鍋?wiki 涓?authority 鏀跺彛銆?
**鐩爣锛?* 灏嗙幇鏈?`Chaos.IL2CPP.CodeGen` 姝ｅ紡 cutover 涓?`Chaos.IL2CPP.Generator`锛屽悓姝ユ敹鍙?`Scriban` 鐨勬牳蹇冨眰瀹氫綅銆丳ython 鐢熸垚鍏ュ彛銆亀iki 浣跨敤鏂囨。涓?codegen 瑙勮寖銆?
**鏋舵瀯锛?* 淇濇寔鍗曚竴鏍稿績鐢熸垚宸ョ▼锛屼笉鏂板缓骞惰澹抽」鐩€俙Scriban` 缁х画閫氳繃 vendored `ProjectReference` 杩涘叆鐢熸垚灞傦紝鎵€鏈夌粨鏋勫寲 codegen 瑙勫垯缁熶竴缁?`Chaos.IL2CPP.Generator` 鍜屽叾 `Templating/Templates` 鍩虹璁炬柦钀藉湴锛涘綋鐜版湁濮垮娍涓嶆敮鎸佹椂锛屾墿灞?Generator锛岃€屼笉鏄粫寮€瀹冨啓涓存椂鐢熸垚閫昏緫銆?
**鎶€鏈爤锛?* C#/.NET 8銆丮SBuild銆乿endored `Scriban`銆丳ython workspace/planning tooling銆乺epo wiki銆乣pytest`

**AOT/IL2CPP/Test Governance Intake锛?*
- capabilityFamily: n/a
- capabilityItem: scriban-generator-formalization
- ownerSubjectId: n/a
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects: []
- requiredGates: collector -> registry -> workspace

**璁捐鏂囨。锛?* `docs/dev/in-progress/20260419-08-scriban-generator-formalization/design-v1-01.md`

**棰勬湡鐭ヨ瘑娌夋穩锛?*
- `wiki/04-宸ュ叿涓庨泦鎴?il2cpp-generator-usage-and-codegen-governance.md`
- `wiki/04-宸ュ叿涓庨泦鎴?INDEX.md`
- 瑙嗘儏鍐靛悓姝ユ洿鏂?`wiki/04-宸ュ叿涓庨泦鎴?il2cpp-core-structure-and-scriban-governance.md`

**鏀跺熬绾︽潫锛?* 鎵ц瀹屾垚鍚庡繀椤昏繘鍏モ€滃瑙嗘灦鏋勫悎鐞嗘€?-> 娴嬭瘯閫氳繃 -> 鏂囨。/wiki 鏀跺彛 -> 淇濇寔鏃犲弻杞ㄦ棫鍚嶁€濈殑鍥哄畾閾捐矾銆?
---

## 鐩爣鏂囦欢涓庤亴璐?
- `src/managed/Chaos.IL2CPP.Generator/**`
  - 鏂?canonical 鐢熸垚宸ョ▼鐩綍涓庢簮鐮併€?- `src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj`
  - 鏀瑰紩鐢ㄥ埌 `Chaos.IL2CPP.Generator`銆?- `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`
  - 鏀?`using` 涓庤皟鐢ㄥ叆鍙ｃ€?- `src/managed/Chaos.IL2CPP.Pipeline/Chaos.IL2CPP.Pipeline.csproj`
  - 鏀瑰紩鐢ㄥ埌 `Chaos.IL2CPP.Generator`銆?- `src/managed/Chaos.IL2CPP.Pipeline/PipelinePlan.cs`
  - 鏀?`using` 涓?stage 渚濊禆銆?- `build/toolchains/run/subject/project_workspace.py`
  - Python 鏍稿績宸ヤ綔鍖虹敓鎴愬叆鍙ｆ敼涓鸿瘑鍒苟寮曠敤 `Chaos.IL2CPP.Generator`銆?- `tests/support.py`
  - shared helper 鏀瑰埌 `Chaos.IL2CPP.Generator` 璺緞銆?- `tests/unit/run/test_repo_layout.py`
  - 浠撳簱 canonical 椤圭洰甯冨眬涓庝緷璧栧叧绯诲垏鍒?`Generator`銆?- `tests/unit/planning/test_project_workspace.py`
  - Python 宸ヤ綔鍖虹敓鎴愮敤渚嬫敼涓哄紩鐢?`Generator`銆?- `tests/unit/compatibility/**`
  - 鎵€鏈夌洿鎺ラ敋瀹?`Chaos.IL2CPP.CodeGen` 璺緞/椤圭洰鍚嶇殑鐢ㄤ緥鍒囧埌 `Generator`銆?- `docs/dev/ACTIVE.md`
  - `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/subject-test-framework-v1/INDEX.md`
  - `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/spec.md`
  - authority 鏂囨。鏄庣‘ `Chaos.IL2CPP.Generator` 涓?IL2CPP 鏍稿績 codegen 宸ョ▼銆?- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/runtime-baseline/repo-layout.md`
  - 褰撳墠 repo layout 鐪熺浉婧愬垏鍒?`Generator`銆?- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/runtime-baseline/ownership-map.md`
  - ownership map 鍒囧埌 `Generator`銆?- `wiki/04-宸ュ叿涓庨泦鎴?il2cpp-core-structure-and-scriban-governance.md`
  - 鍚屾 canonical 鍚嶇О涓庘€滃Э鍔夸笉鏀寔灏辨墿灞?Generator鈥濈殑闀挎湡瑙勫垯銆?- `wiki/04-宸ュ叿涓庨泦鎴?il2cpp-generator-usage-and-codegen-governance.md`
  - 鏂板 Generator 浣跨敤涓?codegen 瑙勮寖椤点€?- `wiki/04-宸ュ叿涓庨泦鎴?INDEX.md`
  - 绾冲叆鏂伴〉闈笌鍏ュ彛銆?
## 鎵ц姝ラ

- [ ] **姝ラ 1锛氬厛琛?鏀瑰け璐ユ祴璇曪紝閿佸畾鏂?canonical 鍚嶇О**
  - 淇敼 `tests/unit/run/test_repo_layout.py`锛屾妸 managed project canonical 鍚嶇О浠?`Chaos.IL2CPP.CodeGen` 鏀逛负 `Chaos.IL2CPP.Generator`銆?  - 淇敼 `tests/unit/planning/test_project_workspace.py`銆乣tests/support.py` 鍙婃渶灏忓繀瑕?compatibility 鐢ㄤ緥锛屼娇瀹冧滑閿氬畾 `src/managed/Chaos.IL2CPP.Generator/**`銆?  - 杩愯鏈€灏忔祴璇曞瓙闆嗭紝纭鍦ㄦ簮鐮佸皻鏈噸鍛藉悕鍓嶇‘瀹炲け璐ャ€?
- [ ] **姝ラ 2锛氬疄鏂藉崟娆″伐绋?cutover**
  - 鍘熷湴閲嶅懡鍚嶇洰褰?`src/managed/Chaos.IL2CPP.CodeGen` -> `src/managed/Chaos.IL2CPP.Generator`銆?  - 閲嶅懡鍚?`.csproj` 涓?`Chaos.IL2CPP.Generator.csproj`銆?  - 鎵归噺淇敼 namespace / `using` / `ProjectReference` / Python 纭紪鐮佽矾寰勩€?  - 涓嶄繚鐣欓暱鏈?wrapper project銆乤lias project 鎴栧弻杞ㄨ矾寰勩€?
- [ ] **姝ラ 3锛氭敹鍙?Python 鐢熸垚鍏ュ彛**
  - 鏇存柊 `build/toolchains/run/subject/project_workspace.py` 鍜岀浉鍏虫祴璇曞す鍏凤紝浣?Python 鐢熸垚鏍稿績宸ヤ綔鍖烘椂寮曠敤 `Chaos.IL2CPP.Generator`銆?  - 鏄庣‘ Python 渚?codegen 搴旀秷璐?璋冪敤 Generator锛岃€屼笉鏄啀鍗曠嫭缁存姢鏃佽矾鐢熸垚閫昏緫銆?
- [ ] **姝ラ 4锛氬啓鍏?authority 鏂囨。涓?wiki**
  - 鏇存柊 architecture authority 鏂囨。涓殑 canonical 宸ョ▼鍚嶄笌杈圭晫銆?  - 鏂板 wiki 椤甸潰锛屾槑纭細
    - `Chaos.IL2CPP.Generator` 鐨勮亴璐?    - `Scriban` 鐨勬寮忎綅缃?    - 鏂囦欢绾?codegen 榛樿璧?Generator
    - 褰撳綋鍓嶅Э鍔夸笉鏀寔鏃讹紝搴旀墿灞?Generator锛岃€屼笉鏄啓涓存椂瀛楃涓叉嫾鎺?鏃佽矾 Python 鐢熸垚鍣?    - Python 濡備綍浣跨敤 Generator 鐢熸垚浠ｇ爜
  - 鏇存柊 `wiki/04-宸ュ叿涓庨泦鎴?INDEX.md`銆?
- [ ] **姝ラ 5锛氶獙璇佷笌鏀跺彛**
  - 璺?repo layout / planning / compatibility / build 楠岃瘉銆?  - 纭涓嶅瓨鍦ㄦ椿鍔ㄤ唬鐮佹爲涓殑 `Chaos.IL2CPP.CodeGen` canonical 鍏ュ彛娈嬬暀銆?  - 濡傞獙璇侀€氳繃锛屾洿鏂颁换鍔?`STATUS.md` 骞跺噯澶囧綊妗ｃ€?
## 鍏抽敭娴嬭瘯涓庨獙璇佸懡浠?
- `python -m pytest tests/unit/run/test_repo_layout.py -q`
- `python -m pytest tests/unit/planning/test_project_workspace.py -q`
- `python -m pytest tests/unit/compatibility/test_il2cpp_codegen_structure_governance.py -q`
- `python -m pytest tests/unit/compatibility/test_scriban_vendor_build.py -q`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`

寤鸿琛ュ厖锛?
- `python -m pytest tests/unit/planning/test_canonical_solution_project_paths.py -q`
- 浠婚€変竴鏉＄湡瀹?`Generator` 鐢熸垚閾惧懡浠わ紝楠岃瘉 Python/workspace -> Driver -> Generator -> Scriban 浠嶈兘浜у嚭 generated artifacts
