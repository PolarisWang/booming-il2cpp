# Option 1: Family Verification Pipeline Integration Design

## 目录结构映射到 Pipeline 阶段

```
convert-char/
├── managed/
│   ├── subjects/                  ← Step 1a: 自动生成 + dotnet build
│   │   ├── ConvertCharSubjects.cs
│   │   └── ConvertCharSubjects.csproj
│   │
│   ├── ConvertChar.csproj         ← Step 4: dotnet test / dotnet run (托管验证)
│   ├── ConvertCharTests.cs
│   ├── ConvertCharBenchmark.cs
│   │
│   └── patch/
│       ├── ConvertCharPatch.csproj  ← Step 2d: patch data 生成
│       └── ConvertCharPatchEntry.cs
│
├── codegen/                       ← Step 1b: chaos-il2cpp 纯输出
│   └── native-aot.generated.cpp   ← 唯一文件，零后处理
│
├── native/                        ← Step 2: cmake + cl → entry.exe
│   ├── CMakeLists.txt
│   ├── runtime-entry.cpp
│   └── runtime-patchdata.cpp      ← Step 2d 生成
│
└── reports/                       ← Step 3: 验证报告
```

## verify_family() 各阶段对应

当前 pipeline_native_aot_runner.py 的 `run_family()` 7 步，映射如下：

| 当前 Stage | 代码函数 | 新方案路径变化 | 关键变更 |
|-----------|---------|---------------|---------|
| **0. Preflight** | 契约检查 | 不变 | — |
| **1a. Build subjects DLL** | `_build_entrypoint()` | 输出从 `il2cpp_dist/entrypoint/` → `managed/subjects/` | subjects/ 独立工程 |
| **1b. convert-to-cpp** | `_run_convert_to_cpp()` | 输出从 `il2cpp_dist/genuine/` → `codegen/` | 纯 C++，无 post-process |
| **1c. [删除] patch generated** | `_patch_generated_files()` | **删除** | 按约定不做后处理 |
| **1d. generate patch data** | `_generate_patch_data()` | 输入从 `entrypoint-patch/` → `managed/patch/` | 不变 |
| **2. Build entry.exe** | `_build_entry_exe()` | `il2cpp_dist/genuine/CMakeLists.txt` → `native/CMakeLists.txt` | CMakeLists.txt 位置变 |
| **3. Fact / Benchmark / HU** | 运行 entry.exe | `genuine/build/entry.exe` → `native/build/entry.exe` | 路径变 |

## 四个 pipelinestage 详细设计

### Stage 1a: Build subjects DLL

**输入**: `capability-family-contract.json` → `methodSubjectIds`

**Python 代码变化**:

```python
def _build_subjects_dll(family_slug, mids, *, verification) -> dict:
    v = verification or _VERIFICATION
    subjects_dir = v / family_slug / "managed" / "subjects"
    class_name = f"{family_slug.title().replace('-', '').replace('_', '')}Subjects"

    # 检查手写 custom.cs (保留 handwritten/ 只读模式)
    handwritten_dir = v / family_slug / "handwritten"
    if handwritten_dir.exists():
        cs_files = [f for f in handwritten_dir.iterdir() if f.is_file() and f.suffix == ".cs"]
        for f in cs_files:
            dest = subjects_dir / f.name
            dest.write_text(f.read_text(encoding="utf-8"), encoding="utf-8")

    result = generate_and_build(
        subjects_dir,              # ← 新路径: managed/subjects/
        assembly_name=assembly_name,
        family_id=f"family/{assembly_name}/{family_slug}",
        method_subject_ids=mids,
        class_name=class_name,
        variant=variant,
    )
    return result
```

核心变化:
- 输出目录 `il2cpp_dist/entrypoint/` → `managed/subjects/`
- 类名后缀 `NativeEntry` → `Subjects`
- csproj 输出 `Exe` → `Library`（只产生 DLL 无需 Main，il2cpp 不需要入口点）

**生成文件示例** (`managed/subjects/ConvertCharSubjects.cs`):

```csharp
// Auto-generated. Subject methods for il2cpp codegen input.
public static class ConvertCharSubjects
{
    public static int _exitCode;

    public static void Subject_7()
    {
        if ((int)(Convert.ToChar(42)) != 42) _exitCode = 1;
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex) { case 7: Subject_7(); break; ... }
    }
}
```

### Stage 1b: convert-to-cpp

**当前代码变化**:

```python
def _run_convert_to_cpp(family_slug, dll_path, *, verification, entry_point_subject_id) -> bool:
    v = verification or _VERIFICATION
    codegen_out = v / family_slug / "codegen"       # ← 从 il2cpp_dist/genuine/ 改为 codegen/
    codegen_out.mkdir(parents=True, exist_ok=True)

    cmd = [
        "dotnet", "run", "--no-build",
        "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
        "--", "convert-to-cpp",
        "--assembly", dll_path,
        "--assembly-dir", str(Path(dll_path).parent),
        "--output", str(codegen_out),               # ← 新路径
    ]
    # ... 其余不变

    # 检查产物: codegen/native-aot.generated.cpp 存在即可
    cpp_file = codegen_out / "native-aot.generated.cpp"
    if cpp_file.exists():
        print(f"    convert-to-cpp OK: {cpp_file.stat().st_size} bytes")
        return True
```

核心变化:
- `_patch_generated_files()` 整个函数**删除**（用户约定不做后处理）
- 输出目录 `il2cpp_dist/genuine/` → `codegen/`
- 不扫描子目录，直接检查 `codegen/native-aot.generated.cpp`

**产物**: `codegen/native-aot.generated.cpp` — 唯一文件。

### Stage 1c: [删除] Patch generated files

按约定，`_patch_generated_files()` 整个删除。所有 MSVC 兼容性问题在 codegen emitter 层修复。

当前 pipeline 中 `_patch_generated_files` 做的事：
1. `_patch_bypass_0xC0000409.py` — 包装 `#pragma runtime_checks("gs", off)` + setjmp
2. `_gen_supplemental_dispatch.py` — 生成 RunNativeAotAll / sentinel patchdata
3. `_gen_valuetype_forward_decls.py` — typedef-int 前向声明
4. `_gen_weak_stubs.py` — 外部 assembly 符号 stub
5. `_gen_external_runtime_stubs.py` — chaos_external_runtime_* stub

这些要么移入 codegen emitter 本身，要么移入 `native/CMakeLists.txt` 的编译中（不修改 codegen 源文件）。

### Stage 1d (原 2b): Generate patch data

```python
def _generate_patch_data(family_slug, *, verification) -> bool:
    v = verification or _VERIFICATION
    patch_dir = v / family_slug / "managed" / "patch"  # ← 从 il2cpp_dist/entrypoint-patch/ 改为 managed/patch/
    # ... 构建 patch DLL → emit patch data → 写入 native/runtime-patchdata.cpp
    native_dir = v / family_slug / "native"             # ← 新输出目录
```

输出: `native/runtime-patchdata.cpp`（定义 kPatchData[]/kPatchDataSize/kPatchDataHostClassName）

### Stage 2 (原 3): Build entry.exe

```python
def _build_entry_exe(family_slug, *, verification, config_tier) -> bool:
    v = verification or _VERIFICATION
    native_dir = v / family_slug / "native"             # ← 从 il2cpp_dist/genuine/ 改为 native/
    cmakelists = native_dir / "CMakeLists.txt"

    # codegen 文件从 codegen/ 引用
    codegen_dir = v / family_slug / "codegen"
    codegen_cpp = codegen_dir / "native-aot.generated.cpp"

    # 编译: codegen/native-aot.generated.cpp + native/runtime-entry.cpp
    # 链接: chaos_runtime_core.lib
    build_dir = native_dir / "build"
    # cmake -S {native_dir} -B {build_dir} ...
    # cmake --build {build_dir} --target entry ...
```

**产物**: `native/build/entry.exe`

### Stage 3: 执行验证（Fact / Benchmark / HotUpdate）

路径更新:
- entry.exe: `il2cpp_dist/genuine/build/entry.exe` → `native/build/entry.exe`
- 验证结果仍写入 `reports/`

---

## 三线并行验证流

```
Managed 侧 (dotnet)                    Native 侧 (entry.exe)
─────────────────────────              ─────────────────────────
                                   
dotnet test ConvertChar.csproj        entry.exe           → Fact Assert 验证
  ↓ xUnit Fact                          ↓ 跑所有 Subject_N
  Convert.ToChar(65) == 'A'             exit code = 0 = Pass

dotnet run ConvertChar.csproj          entry.exe --bench N  → 性能
  ↓ Benchmark                           ↓ 计时 Subject_N
  JIT 基线 timing                       Codegen timing 对比

                                       entry.exe --hotupdate  → 热更新
                                         ↓ 加载 patch → 再跑 Subjects
```

Managed 侧独立运行不进 il2cpp 闭包，Native 侧通过 subjects DLL 翻译而来。两者的 `Convert.ToChar` 行为可通过 Fact 互相验证。

---

## 验证编排（verify_family 主入口）

当前 7 阶段保留，但内部的 stage 数据流指向新路径:

```python
def verify_family(family_slug):
    # 0. Preflight — 不变
    contract = load_contract(family_slug)

    # 1a. Build subjects DLL
    subjects_dll = build_subjects_dll(family_slug, contract.mids)

    # 1b. Codegen (convert-to-cpp) — 无 post-process
    codegen_cpp = run_convert_to_cpp(family_slug, subjects_dll)

    # 1c. [删除] — 不 patch generated files

    # 1d. Patch data (hotupdate)
    patch_data_cpp = generate_patch_data(family_slug)

    # 2. Build entry.exe
    entry_exe = build_entry_exe(family_slug)

    # 3. Fact — 运行 entry.exe，验证 Assert
    fact_result = run_fact(entry_exe)

    # 4. Audit — 原理+机制审计
    audit()

    # 5. Benchmark — 可选
    benchmark()

    # 6. HotUpdate — 可选
    hotupdate()

    # 7. Aggregate
    aggregate()
```

## 迁移兼容性（老路径转新路径映射）

| 老路径 | 新路径 | 迁移方式 |
|--------|--------|---------|
| `il2cpp_dist/entrypoint/` | `managed/subjects/` | pipeline 函数换输出 |
| `il2cpp_dist/genuine/native-aot.generated.cpp` | `codegen/native-aot.generated.cpp` | convert-to-cpp 输出改 |
| `il2cpp_dist/genuine/runtime-entry.cpp` | `native/runtime-entry.cpp` | 手写 + 非 pipeline 复制 |
| `il2cpp_dist/genuine/CMakeLists.txt` | `native/CMakeLists.txt` | 手写 CMakeLists.txt |
| `il2cpp_dist/genuine/build/entry.exe` | `native/build/entry.exe` | build_exe 函数改路径 |
| `il2cpp_dist/entrypoint-patch/` | `managed/patch/` | patch_data 函数改路径 |
| `il2cpp_dist/native-reference.runtime-skeleton.coverage.json` | `reports/coverage.json` | 可选 |
| `il2cpp_dist/genuine/runtime-patchdata.cpp` | `native/runtime-patchdata.cpp` | patch 生成改输出 |
| `handwritten/` (只读源) | 不变 | 保留 |