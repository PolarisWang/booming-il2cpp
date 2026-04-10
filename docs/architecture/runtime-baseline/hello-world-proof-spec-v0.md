# HelloWorld Proof Spec v0

## 鐩爣

鏈枃浠跺畾涔?reboot 涓荤嚎涓涓€鏉?object-based `HelloWorld` proof 鐨勫敮涓€ canonical truth銆?

Stage 2 鐨勮亴璐ｄ笉鏄墦閫?managed closure 鎴?native runtime锛岃€屾槸鎶?proof sample source銆乤rtifact snapshot銆乶ative direct-call landing 鍜?gate 褰掑睘鏀跺彛鍒板悓涓€浠借鏍间笂銆?

## Canonical Truth

### 绋嬪簭鏍囪瘑

- assembly: `HelloWorldObject`
- entry method: `HelloWorldObject/Program::Main(System.String[])`

### Canonical Output

- stdout: `Hello, World!`
- exit code: `0`

### Managed Subjects

- type: `HelloWorldObject/Program`
- type: `HelloWorldObject/Greeter`
- field: `HelloWorldObject/Greeter::_name`
- method: `HelloWorldObject/Program::Main(System.String[])`
- method: `HelloWorldObject/Greeter::.ctor(System.String)`
- method: `HelloWorldObject/Greeter::BuildMessage()`

## Sample Shape

`HelloWorldObject` 鐨?canonical shape 鍥哄畾涓猴細

1. `Program.Main(System.String[])` 鏋勯€?`Greeter("World")`
2. `Greeter.BuildMessage()` 浜у嚭 `Hello, World!`
3. `Program.Main(System.String[])` 鎵撳嵃缁撴灉骞?`return 0`

杩欓噷淇濈暀鏍囧噯鍏ュ彛绛惧悕 `Main(string[] args)`锛屼絾 proof 璇箟涓嶄緷璧栬繍琛屾椂鍙傛暟锛屼篃涓嶅啀鍏佽榛樿鍥為€€鍒?`"object-world"` 涓€绫荤殑鍙彉杈撳叆銆?

## Canonical Artifact Truth Sources

Stage 2 閲囩敤浠ヤ笅鏂囦欢浣滀负 proof spec 鐨勪笂娓稿喕缁撶湡婧愶細

- `tests/contracts/analysis/v0/samples/typed-il-ir.min.json`
- `tests/contracts/analysis/v0/samples/aot-manifest.min.json`
- `tests/contracts/analysis/v0/samples/code-registration.min.json`
- `tests/contracts/analysis/v0/samples/metadata-registration.min.json`
- `tests/tests/contracts/native/v0/samples/bridge/direct-call-landing.cpp`

鍏朵腑锛?

- `tests/contracts/analysis/v0/samples/typed-il-ir.min.json` 鍥哄畾浜?3 涓柟娉曚綋鐨勬渶灏?IL 褰㈢姸
- `tests/contracts/analysis/v0/samples/aot-manifest.min.json` 鍥哄畾浜?first-proof entry 涓?retained world 鏈€灏忛泦鍚?- `tests/contracts/analysis/v0/samples/code-registration.min.json` 鍥哄畾浜?generated method pointer 涓?symbol
- `tests/contracts/analysis/v0/samples/metadata-registration.min.json` 鍥哄畾浜?assembly / type / field / method 娉ㄥ唽闆嗗悎
- `tests/contracts/native/v0/samples/bridge/direct-call-landing.cpp` 鍥哄畾浜?generated direct-call landing锛岃€屼笉鏄?`method_invoke` fallback

## Shared Identity

proof 鐩稿叧 artifact 蹇呴』鍏变韩鍚屼竴濂?canonical `subjectId` 瑙勫垯銆?

鍏抽敭 `subjectId` 涓猴細

- `HelloWorldObject/Program::Main(System.String[])`
- `HelloWorldObject/Greeter::.ctor(System.String)`
- `HelloWorldObject/Greeter::BuildMessage()`
- `HelloWorldObject/Greeter::_name`

`typed-il-ir` 鍙互缁х画淇濈暀 artifact-local `methodId`锛屼絾璺?artifact 杩芥函浠?`subjectId` 涓哄敮涓€姝ｅ紡 identity銆?

## Direct-Call Landing Constraint

first-proof 鐨?managed 璋冪敤钀界偣鏄庣‘鍥哄畾涓?generated direct-call landing銆?

鍏佽鐨勬渶灏忚矾寰勶細

1. `register_codegen`
2. `bootstrap_runtime`
3. `resolve_type_by_token`
4. `resolve_method_by_token`
5. `resolve_field_by_token`
6. `string_new_utf8`
7. `object_new`
8. direct call锛?
   - `HelloWorldObject_Greeter__ctor`
   - `HelloWorldObject_Greeter_BuildMessage`
   - `HelloWorldObject_Program_Main`

涓嶅厑璁哥殑鏇夸唬璺緞锛?

- `method_invoke` fallback
- 瀹夸富鎵嬪啓鐩磋繛鏇夸唬 generated entry
- 瑙ｉ噴鎵ц鏇夸唬 generated call landing

## Gate 褰掑睘

### Stage 2 鍐荤粨鐨?gate

- `proof-input-build`
  - `subjects/HelloWorldObject/source/HelloWorldObject.csproj` 鍙互绋冲畾 build
- `proof-artifact-snapshots`
  - canonical artifact snapshot 绋冲畾琛ㄨ揪 3 涓柟娉曘€? 涓瓧娈典笌 shared `subjectId`
- `proof-spec-consistency`
  - source / artifact / native landing / 鏂囨。涓嶅啀鍚勮鍚勮瘽

### 寤跺悗鍒?Stage 3 / Stage 4 鐨?gate

- `managed-minimal-closure`
  - managed pipeline 鑳戒粠绋嬪簭闆嗚蛋鍒?generated-code input
- `native-reference-proof`
  - reference platform 鐪熸杈撳嚭 `Hello, World!` 涓?exit code 涓?`0`

## Repository Entry Points

- sample source: `subjects/HelloWorldObject/source/Program.cs`
- subject manifest: `subjects/HelloWorldObject/subject.manifest.json`
- contract overview: `contracts/docs/v0/overview.md`

## 褰撳墠缁撹

濡傛灉 sample source 涓?canonical artifact / native landing 鍐嶆鍑虹幇鍐茬獊锛屽簲浼樺厛淇濇寔鏈枃浠朵笌涓婃父 contract truth 涓€鑷达紝鍐嶅榻?sample source锛岃€屼笉鏄噸鏂板紑鏀?Stage 1 宸插喕缁撶殑 contract surface銆?

