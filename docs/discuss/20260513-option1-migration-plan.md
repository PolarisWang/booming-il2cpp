# Option 1 迁移实施计划

## 概览

基于 `convert-char` family 做试点，迁移完成后验证 pipeline 可正常跑通 7 阶段，再推广到其他 family。

---

## Phase 1: 新目录创建 + 文件迁移

### 1.1 创建新目录结构

```bash
# subjects/ — 独立工程
mkdir verification/foundation-dll/System.Private.CoreLib/convert-char/managed/subjects/

# codegen/ — 纯输出
mkdir verification/foundation-dll/System.Private.CoreLib/convert-char/codegen/

# native/ — wrapper 工程
mkdir verification/foundation-dll/System.Private.CoreLib/convert-char/native/
```

### 1.2 managed/subjects/ — 新建独立工程

**创建 `managed/subjects/ConvertCharSubjects.csproj`**:

```xml
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <TargetFramework>net8.0</TargetFramework>
    <OutputType>Library</OutputType>
    <AssemblyName>ConvertCharSubjects</AssemblyName>
    <ImplicitUsings>disable</ImplicitUsings>
    <Nullable>enable</Nullable>
  </PropertyGroup>
</Project>
```

**`managed/subjects/ConvertCharSubjects.cs`** — 由 `family_entrypoint_generator.py` 自动生成覆盖。
- 内容：从旧 `ConvertCharNativeEntry.cs` 改名为 `ConvertCharSubjects`，方法名 `Subject_N`
- `Run(int entryIndex)` dispatch 不变

### 1.3 managed/ — Tests + Benchmark 工程

**创建 `managed/ConvertChar.csproj`**:

```xml
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <TargetFramework>net8.0</TargetFramework>
    <OutputType>Exe</OutputType>
    <AssemblyName>ConvertChar</AssemblyName>
    <ImplicitUsings>enable</ImplicitUsings>
    <Nullable>enable</Nullable>
  </PropertyGroup>
  <ItemGroup>
    <Compile Include="ConvertCharTests.cs" />
    <Compile Include="ConvertCharBenchmark.cs" />
    <PackageReference Include="xunit" Version="2.9.3" />
    <PackageReference Include="Microsoft.NET.Test.Sdk" Version="17.12.0" />
  </ItemGroup>
</Project>
```

**`managed/ConvertCharTests.cs`** — 从 `managed_test/tests/ConvertCharTests.cs` 迁移，内容不变。
**`managed/ConvertCharBenchmark.cs`** — 从 `managed_test/benchmarks/ManagedBenchmarkHarness.cs` 迁移，类名改为 `ConvertCharBenchmark`。

### 1.4 managed/patch/ — 保留

**`managed/patch/ConvertCharPatch.csproj`** — 从 `il2cpp_dist/entrypoint-patch/ConvertCharPatchEntry.csproj` 迁移。
**`managed/patch/ConvertCharPatchEntry.cs`** — 从 `il2cpp_dist/entrypoint-patch/` 迁移。

### 1.5 codegen/ — 目录创建

目录本身已创建，产出自 `chaos-il2cpp convert-to-cpp` 重定向输出后。

### 1.6 native/ — 手写 CMakeLists.txt + runtime-entry.cpp

**`native/CMakeLists.txt`**:

```cmake
cmake_minimum_required(VERSION 3.20)
project(chaos_entry CXX)
set(CMAKE_CXX_STANDARD 20)

# 编译选项
set(CMAKE_MSVC_EXCEPTION_HANDLING "/EHs")
add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_CHECK)

# codegen 文件路径（来自 codegen/ 目录）
set(CHAOS_CODEGEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../codegen")
set(CHAOS_CODEGEN_CPP "${CHAOS_CODEGEN_DIR}/native-aot.generated.cpp")

# native wrapper + runtime libs
set(CHAOS_NATIVE_BUILD "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../build/native")
set(CHAOS_RUNTIME_LIBS
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_support
    chaos_hot_update
)

add_executable(entry
    "runtime-entry.cpp"
    "runtime-patchdata.cpp"
    "${CHAOS_CODEGEN_CPP}"
)
target_include_directories(entry PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/runtime-core"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../contracts/native/v0"
)
target_link_directories(entry PRIVATE
    "${CHAOS_NATIVE_BUILD}/runtime-core/RelWithDebInfo"
    "${CHAOS_NATIVE_BUILD}/bootstrap/RelWithDebInfo"
    "${CHAOS_NATIVE_BUILD}/interpreter/RelWithDebInfo"
    "${CHAOS_NATIVE_BUILD}/support/RelWithDebInfo"
    "${CHAOS_NATIVE_BUILD}/hot-update/RelWithDebInfo"
)
target_link_libraries(entry PRIVATE ${CHAOS_RUNTIME_LIBS})
```

**`native/runtime-entry.cpp`** — 从 `build/toolchains/run/testing/foundation_dll/runtime-entry.cpp` 复制，保持手写维护。

### 1.7 删除旧目录（迁移完成后）

```bash
rm -rf verification/.../convert-char/il2cpp_dist/entrypoint/
rm -rf verification/.../convert-char/il2cpp_dist/entrypoint-patch/
rm -rf verification/.../convert-char/il2cpp_dist/genuine/
rm -rf verification/.../convert-char/managed_test/
```

---

## Phase 2: Pipeline 代码修改

### 2.1 `pipeline_native_aot_runner.py` 修改清单

| 函数 | 修改内容 |
|------|---------|
| `run_family()` Stage 1a | 调用 `_build_subjects_dll()` 替代 `_build_entrypoint()` |
| `run_family()` Stage 1b | `_run_convert_to_cpp()` 改输出路径 |
| `run_family()` Stage 1c | **删除** `_patch_generated_files()` 调用 |
| `run_family()` Stage 1d | `_generate_patch_data()` 改输入/输出路径 |
| `run_family()` Stage 2 | `_build_entry_exe()` 改 CMakeLists.txt 路径 |
| `_build_entrypoint()` → 新建 `_build_subjects_dll()` | 输出到 `managed/subjects/`，类名 `*Subjects` |

### 2.2 `_build_subjects_dll()` 函数

```python
def _build_subjects_dll(
    family_slug: str,
    method_subject_ids: list[str],
    *,
    assembly_name: str = "System.Private.CoreLib",
    verification: Path | None = None,
    variant: str = "benchmark",
) -> dict:
    """Build the subjects DLL for il2cpp codegen input.

    Output goes to managed/subjects/ instead of il2cpp_dist/entrypoint/.
    The subjects DLL is a Library (not Exe), containing Subject_N methods.
    """
    v = verification or _VERIFICATION
    subjects_dir = v / family_slug / "managed" / "subjects"
    class_name = f"{family_slug.title().replace('-', '').replace('_', '')}Subjects"

    handwritten_dir = v / family_slug / "handwritten"
    if handwritten_dir.exists():
        cs_files = [f for f in handwritten_dir.iterdir() if f.is_file() and f.suffix == ".cs"]
        for f in cs_files:
            dest = subjects_dir / f.name
            dest.write_text(f.read_text(encoding="utf-8"), encoding="utf-8")

    result = generate_and_build(
        subjects_dir,
        assembly_name=assembly_name,
        family_id=f"family/{assembly_name}/{family_slug}",
        method_subject_ids=method_subject_ids,
        class_name=class_name,
        variant=variant,
    )
    return result
```

### 2.3 `_run_convert_to_cpp()` 输出路径

```python
# 修改前:
genuine_out = v / family_slug / "il2cpp_dist" / "genuine"

# 修改后:
codegen_out = v / family_slug / "codegen"
codegen_out.mkdir(parents=True, exist_ok=True)

# 输出到 codegen/
cmd = [
    "dotnet", "run", "--no-build",
    "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
    "--", "convert-to-cpp",
    "--assembly", dll_path,
    "--assembly-dir", str(Path(dll_path).parent),
    "--output", str(codegen_out),   # ← codegen/ 而非 il2cpp_dist/genuine/
]

# 检查产物: 直接检查 codegen/native-aot.generated.cpp (无子目录)
cpp_file = codegen_out / "native-aot.generated.cpp"
if cpp_file.exists():
    print(f"    convert-to-cpp OK: {cpp_file.stat().st_size} bytes")
else:
    # fallback: 检查旧格式的子目录结构
    for d in codegen_out.iterdir():
        if d.is_dir():
            per_asm_cpp = d / "generated" / "native-aot.generated.cpp"
            ...
```

### 2.4 删除 `_patch_generated_files()`

从 `run_family()` 中删除调用。函数本体保留在文件中（不立即删除，避免影响其他分支），但 `run_family()` 中不再调用。

### 2.5 `_generate_patch_data()` 路径更新

```python
# patch DLL 输入
patch_dir = v / family_slug / "managed" / "patch"   # ← 从 il2cpp_dist/entrypoint-patch/ 改为 managed/patch/

# runtime-patchdata.cpp 输出
native_dir = v / family_slug / "native"              # ← 从 il2cpp_dist/genuine/ 改为 native/
patchdata_cpp = native_dir / "runtime-patchdata.cpp"
```

### 2.6 `_build_entry_exe()` 路径更新

```python
# CMakeLists.txt 位置
native_dir = v / family_slug / "native"              # ← 从 il2cpp_dist/genuine/ 改为 native/
cmakelists = native_dir / "CMakeLists.txt"

# build 输出
build_dir = native_dir / "build"

# 不再复制 enhanced runtime-entry.cpp（native/runtime-entry.cpp 已手写）
# 删除以下代码:
#   enhanced_runtime_entry = _HERE / "runtime-entry.cpp"
#   if enhanced_runtime_entry.exists():
#       dest = genuine_out / "runtime-entry.cpp"
```

---

## Phase 3: family_entrypoint_generator.py 修改

### 3.1 类名变更

```python
# 当前: ConvertCharNativeEntry (自动生成的类名)
# 修改为: ConvertCharSubjects

# 在 generate_and_build() 中，当 variant="benchmark" 且是 subjects 模式时:
if is_subjects_mode:  # 通过参数控制
    class_name = class_name.replace("NativeEntry", "Subjects")
```

### 3.2 方法名前缀变更

subject 方法不叫 `CustomEntryMethodN`，统一叫 `Subject_N`。

```python
# _method_slot_name() 支持 subjects 前缀
def _method_slot_name(index, *, prefix="Subject"):
    return f"{prefix}_{index}"
```

### 3.3 `_compute_entry_point_subject_id()` 更新

```python
# 当前:
#   f"{class_name}/{class_name}::Run:System.Int32(System.Int32)"
# 修改(legacy 适配):
#   f"{class_name}/{class_name}::Run"
#  或者保持原样但 class_name 变为 *Subjects
```

---

## Phase 4: 验证脚本更新

4.1 `fact_verifier.py` — entry.exe 路径从 `il2cpp_dist/genuine/build/` 改为 `native/build/`

4.2 `verify_family.py` orchestrator — Stage 1a 调用 `_build_subjects_dll` 替代 `_build_entrypoint`

---

## Phase 5: 迁移执行顺序

```
Step 1: 创建 managed/subjects/ + ConvertCharSubjects.csproj + cs
Step 2: 验证 dotnet build → ConvertCharSubjects.dll 成功
Step 3: 创建 managed/ConvertChar.csproj + Tests + Benchmark
Step 4: 验证 dotnet test + dotnet run 成功
Step 5: 创建 codegen/ 目录，修改 _run_convert_to_cpp() 输出到 codegen/
Step 6: 创建 native/CMakeLists.txt + runtime-entry.cpp
Step 7: 修改 _build_entry_exe() → native/ 构建
Step 8: 删除 _patch_generated_files() 调用
Step 9: 修改 _generate_patch_data() 路径
Step 10: 跑通 verify-family --family convert-char 全流程
Step 11: 删除旧目录 (il2cpp_dist/entrypoint, genuine, entrypoint-patch, managed_test)
Step 12: 推广到其他 family
```

## 风险清单

| 风险 | 影响 | 缓解措施 |
|------|------|---------|
| subjects DLL 不含 Main()，il2cpp 无法识别入口 | pipeline 崩溃 | `--entry-point` 指向 `Subjects::Run`，Library 格式也支持 |
| codegen 输出 `native-aot.generated.cpp` 的单文件模式破坏多 assembly 解析 | 生成目录结构变化 | 保留对子目录的 fallback 检查 |
| MSVC 0xC0000409 crash 在不做后处理时复现 | Stage 2 构建失败 | 在 codegen emitter 层修复，而非 post-process |
| 其他 family 的 handwritten 路径指向旧目录 | custom.cs 找不到 | 所有路径参数化，统一用新常量 |