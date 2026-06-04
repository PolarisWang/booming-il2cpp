# Hotupdate Return-Value 全链路验证 实现计划

> **面向执行 Agent：** 按以下步骤顺序执行。步骤使用复选框跟踪。

**目标：** 实现 foundation-dll chunk pipeline 的 hotupdate 全链路验证：ATG return-value 统一 → patch DLL 生成 → PatchDataExtractor → 原生 ApplyPatchFromMemory → semantic change 检测

**设计文档：** `docs/dev/in-progress/foundation-dll-hotupdate-pipeline/design-v1-01.md`

**问题清零来源：** brainstorm 完成，用户问题已清零

**架构审核模式：** normal

**结构告警重点：** TestEmitter 中 [Fact] 和 [HotUpdate] 合并为一个方法体

---

## 修改清单

| 步骤 | 文件 | 改动 | 规模 |
|------|------|------|------|
| 1 | `ValueGenerator.cs` | 新增 `GetResultToLongExpression()` + `GetPatchReturnExpression()` | ~60 行 |
| 2 | `TestEmitter.cs` | [Fact] + [HotUpdate] 合并，改为 `long` 返回，加 return 语句 | ~30 行 |
| 3 | `Program.cs` | 填充 `hotupdateMethodIndices`，新增 `--patch-mode` flag | ~50 行 |
| 4 | `RuntimeEntry.cpp.scriban` | 新增 `--patch-data` CLI + `ApplyHotpatchFromFile()` | ~40 行 |
| 5 | `CppProjectEmitter.cs` / `Program.cs` (TPG) | 传递 `has_external_patch_data` 到 scriban 模型 | ~15 行 |
| 6 | `ProjectModel.cs` (TPG) | 新增 `has_external_patch_data` 模型字段 | ~5 行 |
| 7 | `hotupdate_chunk.py` | 重写：ATG patch pass + csc + PatchDataExtractor + 运行 | ~100 行 |
| 8 | `context.py` | 新增 patch 相关路径字段 | ~10 行 |
| 9 | `chunk_pipeline.py` | 默认 stages 加入 hotupdate | ~3 行 |

---

## 步骤详解

### Step 1: ValueGenerator.cs — ResultToLong + Patch 表达式

新增两个静态方法：

```csharp
public static string GetResultToLongExpression(string returnTypeName, string varName)
{
    if (returnTypeName == "System.Void" || returnTypeName == "void")
        return "42L";
    if (IsIntegerType(returnTypeName))  // int, long, short, byte, sbyte, uint, ushort, nint, nuint, Int128, UInt128
        return $"(long)({varName})";
    if (returnTypeName == "System.Boolean")
        return $"{varName} ? 1L : 0L";
    if (returnTypeName == "System.Char")
        return $"(long)({varName})";
    if (returnTypeName == "System.Single")
        return $"BitConverter.SingleToInt32Bits({varName})";
    if (returnTypeName == "System.Double")
        return $"BitConverter.DoubleToInt64Bits({varName})";
    if (IsEnumType(returnTypeName))
        return $"(long)(int)({varName})";
    // Reference types / strings / arrays / objects
    return $"{varName} != null ? 1L : 0L";
}

public static string GetPatchReturnExpression(string returnTypeName, string varName)
{
    var baseline = GetResultToLongExpression(returnTypeName, varName);
    return returnTypeName switch
    {
        "System.Void" or "void" => "142L",
        "System.Boolean" => $"{varName} ? 0L : 1L",
        "System.Single" => $"BitConverter.SingleToInt32Bits({varName}) ^ 0xFFFF",
        "System.Double" => $"BitConverter.DoubleToInt64Bits({varName}) ^ 0xFFFF",
        _ when IsReferenceType(returnTypeName) => $"{varName} != null ? 0L : 1L",
        _ => $"({baseline}) ^ 0xFF",  // 整数+枚举+char: XOR 0xFF
    };
}
```

### Step 2: TestEmitter.cs — 合并 [Fact] + [HotUpdate]

当前结构：
```csharp
// [Fact] block
if (!skipFact) {
    sb.AppendLine("[Fact]");
    sb.AppendLine($"public void {methodSuffix}() {{ ... assert ... }}");
}

// [HotUpdate] block (TODO, 当前是空壳)
if (!skipFact) {
    sb.AppendLine("[HotUpdate]");
    sb.AppendLine($"public void HotUpdate_{methodSuffix}() {{ ... }}");
}
```

改为：
```csharp
// 单个方法，两个 attribute，返回 long
if (!skipFact) {
    sb.AppendLine("[Fact]");
    sb.AppendLine("[HotUpdate]");
    sb.AppendLine($"public long {methodSuffix}()");
    sb.AppendLine("{");
    // ... call + assert（不变）...
    // 新增 return 语句
    sb.AppendLine(GetResultToLongExpression(returnTypeName, result_var_name));
    sb.AppendLine("}");
}
```

对于纯 void 方法（无 ref 参数）：继续跳过，只生成 `[Benchmark]`（不变）。

需要关注的关键点：
- 对于 `hasException` 的情况（Assert.Throws），不生成 return 语句（因为不会执行到）
- 对于 `method.IsVoid || isPlainTask` 的情况，需要生成 sentinel return `42L`

### Step 3: Program.cs — metadata + --patch-mode

**metadata 变更**：
```csharp
// 填充 hotupdateMethodIndices: 与 customEntryIndices 一致（所有 fact 也是 hotupdate）
// 因为所有非 benchmark 的方法都同时有 [Fact] + [HotUpdate]
var hotupdateMethodIndices = new List<int>(customEntryIndices);
// ...
HotupdateMethodIndices: hotupdateMethodIndices.Count > 0 ? hotupdateMethodIndices : null,
```

**--patch-mode 新增**：
- CLI flag：`--patch-mode`
- 同 --all-types 一样的扫描流程，但：
  - 隐含 `--skip-probe`
  - `--emit-metadata` 改为 `--emit-patch-metadata`
  - 在 Phase 4 emit 时使用 `GetPatchReturnExpression` 替代 `GetResultToLongExpression`
  - Subject 不生成 `[Fact]`/`[HotUpdate]` attribute（patch DLL 不需要）
  - 输出文件命名加上 `.patch` 后缀

### Step 4: RuntimeEntry.cpp.scriban — 外部 patch data 加载

新增条件编译块：
```cpp
{{ if has_external_patch_data }}
static PatchContext* ApplyHotpatchFromFile(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) { printf("ERROR: cannot open patch data: %s\n", path); return nullptr; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    uint8_t* data = (uint8_t*)malloc(size);
    if (!data) { fclose(f); return nullptr; }
    fread(data, 1, size, f);
    fclose(f);
    auto* ctx = chaos::il2cpp::runtime_core::ApplyPatchFromMemory(
        data, static_cast<size_t>(size), nullptr);
    free(data);
    return ctx;
}

// 修改 RunHotupdateMode() 接受可选文件路径
static int RunHotupdateMode(const char* patchDataPath) {
    // ... 现有 baseline capture ...
    PatchContext* patch_ctx = nullptr;
    if (patchDataPath) {
        patch_ctx = ApplyHotpatchFromFile(patchDataPath);
    } else {
        patch_ctx = ApplyHotpatchIfAvailable();
    }
    // ... 后续语义检测 + revert ...
}
{{ end }}
```

CLI 参数解析：
```cpp
if (strcmp(argv[1], "--hotupdate") == 0) {
    const char* patchDataPath = nullptr;
    if (argc >= 4 && strcmp(argv[2], "--patch-data") == 0) {
        patchDataPath = argv[3];
    }
    return RunHotupdateMode(patchDataPath);
}
```

### Step 5-6: TPG 模型传递

`ProjectModel.cs` 新增 `has_external_patch_data` 字段（默认 false，hotupdate 构建时设为 true）。
`CppProjectEmitter.cs` 将 `.patchdata` 路径传递给 scriban 模型。

### Step 7: hotupdate_chunk.py

```python
def run_hotupdate_chunk(ctx, stages):
    # 1. ATG --patch-mode
    patch_dll = ctx.managed_dir / "PatchSubjects.dll"
    run_autotestgen("--patch-mode", ...)
    
    # 2. csc compile
    subprocess.run(["dotnet", "build", ...])
    
    # 3. PatchDataExtractor
    patch_data = ctx.native_dir / "patch.patchdata"
    subprocess.run(["dotnet", "exec", "PatchDataExtractor.dll",
        "--dll", patch_dll, "--output", patch_data, "--subject-only"])
    
    # 4. Run entry.exe --hotupdate --patch-data
    result = subprocess.run([ctx.entry_exe_path, "--hotupdate", "--patch-data", patch_data])
    
    # 5. Verify semantic + revert
    ...
```

### Step 8: context.py

```python
@property
def patch_subjects_dll_path(self) -> Path:
    return self.managed_dir / "PatchSubjects.dll"

@property
def patch_data_path(self) -> Path:
    return self.native_dir / "patch.patchdata"
```

### Step 9: chunk_pipeline.py

```python
parser.add_argument("--stages", default="build,fact,hotupdate,coverage-audit", ...)
```

---

## 验证方式

1. 单 chunk E2E（numerics）：
   ```
   python -m verification.chunk_pipeline --chunk numerics --stages build,fact,hotupdate
   ```
   预期：build passed → fact passed → hotupdate passed (allSemantic=true, allRevert=true)

2. 验证 allSemantic=false 场景（故意传错 patch data）→ hotupdate failed

3. 全量 chunks 验证：
   ```
   python -m verification.chunk_pipeline --all-chunks
   ```

---

## 收尾约束

执行完成后：结构告警审视 → 测试通过 → 合并&提交
