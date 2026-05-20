# Verification Dispatch Code 移出核心层 — 设计方案

## 1. 问题

当前 `NativeAot.DispatchEntryCode.cpp.scriban` 模板（位于 `Chaos.IL2CPP.CodeGen` 核心层）生成了以下代码：

| 生成物 | 用途 | 是否验证专用 |
|--------|------|------------|
| `kAotMethods[]` | flat 函数指针数组 | 否 — codegen 内部引用 |
| `kBenchmarkWrappers[]` | 带默认参数的 lambda 包装 | **是** |
| `kSubjectEntryIndices[]` | subject→kAotMethod 索引映射 | **是** |
| `RunNativeAot()` | 单方法 hotpatch 路由 | 否 — 运行时行为 |
| `RunNativeAotAll()` | 全量 fact 循环 | **是** |
| `RunNativeAotBench()` | 全量 benchmark/setjmp 循环 | **是** |
| `BenchmarkMethod()` | switch-based 直接 dispatch + 计时循环 | **是** |

其中 **RunNativeAotAll, RunNativeAotBench, BenchmarkMethod, kBenchmarkWrappers, kSubjectEntryIndices** 都是验证专用代码。`RunNativeAot()` 是运行时行为，保留。

设计要求：**以上 5 项验证专用代码全部移出 Scriban 模板**，由 Python 脚本在 verification 阶段读取 codegen 输出的 manifest 并生成 `verification_dispatch.generated.cpp`。

---

## 2. 架构

```
┌─────────────────────────────────────────────────────────────┐
│                    Core Codegen (C#)                         │
│                                                             │
│  NativeAotLoweringPlanner.cs                                │
│    └─ BuildDispatchEntryCode()                              │
│         → 不再渲染 Scriban 模板                              │
│         → 改为输出 methods-manifest.json                     │
│                                                             │
│  KEEP in Scriban template:                                  │
│    • kAotMethods[]                                          │
│    • RunNativeAot() (hotpatch routing)                      │
│    • s_hotpatch_entries[]                                   │
│    • kAotMethodCount                                        │
│                                                             │
│  REMOVE from Scriban:                                       │
│    • kBenchmarkWrappers[]                                   │
│    • kSubjectEntryIndices[]                                 │
│    • RunNativeAotAll()                                      │
│    • RunNativeAotBench()                                    │
│    • BenchmarkMethod()                                      │
└──────────────────────┬──────────────────────────────────────┘
                       │ methods-manifest.json
                       ▼
┌─────────────────────────────────────────────────────────────┐
│         Verification Layer (Python)                          │
│                                                             │
│  generate_verification_dispatch.py                           │
│    └─ 读取 methods-manifest.json                             │
│    └─ 生成 verification_dispatch.generated.cpp               │
│         ├─ RunFactAll()       — 全量 fact 循环 + try/catch   │
│         ├─ RunBenchmark()     — switch 直接 dispatch + 计时  │
│         ├─ RunHotpatchAll()   — interpreter 路由全量循环     │
│         ├─ RunHotpatchBenchmark() — interpreter 路由 + 计时  │
│         └─ kSubjectEntryIndices                              │
│                                                             │
│  CMakeLists.txt                                             │
│    └─ add_library(verification_dispatch OBJECT ...)          │
│    └─ target_link_libraries(entry verification_dispatch)     │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. methods-manifest.json 格式

Codegen 在 `native-aot.generated.cpp` 同目录输出 `methods-manifest.json`。

```json
{
  "schemaVersion": 2,
  "familySlug": "threading-tasks-primitives",
  "assemblyName": "System.Private.CoreLib",
  "methodCount": 47,
  "defaultStringId": 15309235603972353506,
  "methods": [
    {
      "index": 0,
      "nativeSymbol": "Subject_0__SystemVoid_SystemThreadingThreadGetCurrentThread",
      "paramCount": 0,
      "params": [],
      "isInstance": false,
      "subjectIndex": 0
    },
    {
      "index": 1,
      "nativeSymbol": "Subject_1__SystemInt32_SystemThreadingThreadGetManagedThreadId",
      "paramCount": 0,
      "params": [],
      "isInstance": false,
      "subjectIndex": 1
    },
    {
      "index": 5,
      "nativeSymbol": "Subject_5__SystemVoid_SystemThreadingThreadStart",
      "paramCount": 0,
      "params": [
        {"isString": false, "isThis": true}
      ],
      "isInstance": true,
      "subjectIndex": 5
    }
  ]
}
```

生成位置：`codegen/<AssemblyName>/generated/methods-manifest.json`

### BuildDispatchEntryCode 方法修改

`NativeAotLoweringPlanner.cs::BuildDispatchEntryCode()` (L2757) 修改：

```csharp
private string BuildDispatchEntryCode(IReadOnlyList<AotCoreIrMethodArtifact> methods)
{
    if (methods.Count == 0) return string.Empty;

    // 1. Build the Scriban model for kAotMethods[] + RunNativeAot() only
    var methodEntries = BuildMethodEntries(methods);
    var model = new ScriptObject
    {
        ["methods"] = methodEntries,
        ["methods_count"] = methods.Count,
    };

    // 2. Render the simplified template (no wrappers, no benchmark, no fact)
    var template = NativeAotTemplateCatalog.GetDispatchEntryCodeTemplate();
    var cppCode = ScribanTemplateRenderer.RenderTemplate(template, model);

    // 3. Export methods-manifest.json alongside generated.cpp
    ExportMethodsManifest(methods, methodEntries);

    return cppCode;
}

private void ExportMethodsManifest(
    IReadOnlyList<AotCoreIrMethodArtifact> methods,
    List<ScriptObject> methodEntries)
{
    var manifest = new
    {
        schemaVersion = 2,
        familySlug = _familySlug,
        assemblyName = _assemblyName,
        methodCount = methods.Count,
        defaultStringId = (long)(_stringIdMapping is { Count: > 0 }
            ? _stringIdMapping.First().Value : 0UL),
        methods = methodEntries.Select((entry, i) =>
        {
            var method = methods[i];
            return new
            {
                index = entry["index"],
                nativeSymbol = entry["native_symbol"],
                paramCount = method.ParameterCount + (method.IsStatic ? 0 : 1),
                params = BuildParamDescriptors(method),
                isInstance = !method.IsStatic,
            };
        }).ToList(),
    };

    var json = JsonSerializer.Serialize(manifest, new JsonSerializerOptions
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
    });

    var manifestPath = Path.Combine(_outputDir, "methods-manifest.json");
    File.WriteAllText(manifestPath, json);
}
```

---

## 4. Scriban 模板精简

### 新的 `NativeAot.DispatchEntryCode.cpp.scriban`

只保留 `kAotMethods[]` + `RunNativeAot()`：

```cpp
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
static void (*kAotMethods[{{ methods_count }}])() = {
{{~ for method in methods ~}}
    reinterpret_cast<void(*)()>(&{{ method.native_symbol }}),
{{~ end ~}}
};

// Single-method dispatch via hotpatch dispatch table.
extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(
    CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount)
        return -1;
    auto& entry = s_hotpatch_entries[chaos_entry_index];
    if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry)
        && !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry))
    {
        uint64_t __chaos_args[4] = {}; uint64_t __chaos_ret[2] = {};
        chaos::il2cpp::runtime_core::InterpreterEntryDirect(
            entry.method_key, __chaos_args, __chaos_ret);
    } else {
        kAotMethods[chaos_entry_index]();
    }
    return 0;
}
```

**删除的模板内容：**
- `kBenchmarkWrappers[]`
- `kSubjectEntryIndices[]`
- `RunNativeAotAll()`
- `RunNativeAotBench()`
- `BenchmarkMethod()`

---

## 5. verification_dispatch.generated.cpp — Python 生成器

新建文件：`build/toolchains/run/testing/foundation_dll/generate_verification_dispatch.py`

```python
"""Generate verification_dispatch.generated.cpp from methods-manifest.json.

Called by family_verification_orchestrator.py stage 1 (codegen) after
chaos-il2cpp convert-to-cpp completes.  The generated .cpp is placed in
the family's native/ directory and compiled into entry.exe.

Generated functions:
  - RunFactAll()              — all-methods fact loop with try/catch
  - RunBenchmark(idx, iters)  — switch-based direct dispatch + timing
  - RunHotpatchAll()          — interpreter-routed all-methods loop
  - RunHotpatchBenchmark(idx, iters) — interpreter-routed timing loop
"""

def generate_verification_dispatch(manifest_path: str, output_path: str) -> None:
    with open(manifest_path) as f:
        manifest = json.load(f)

    methods = manifest["methods"]
    default_string_id = manifest["defaultStringId"]

    lines = []
    lines.append("// verification_dispatch.generated.cpp — auto-generated")
    lines.append(f"// Family: {manifest['familySlug']}, Assembly: {manifest['assemblyName']}")
    lines.append(f"// Methods: {manifest['methodCount']}")
    lines.append('#include <cstdint>')
    lines.append('#include <chrono>')
    lines.append('#include <chaos/native_types.h>')
    lines.append('#include "runtime_core.h"')
    lines.append('')
    lines.append('extern "C" const int kAotMethodCount;')
    lines.append('extern "C" void (*kAotMethods[])();')
    lines.append('extern "C" s_hotpatch_entries[];')
    lines.append('')

    # ── Helper: generate default arguments for a method ──────────────
    # Uses same logic as current kBenchmarkWrappers:
    #   - instance methods → sentinel this pointer
    #   - string params → chaos_make_string_id_value(default_string_id)
    #   - other params → 0

    # ── kSubjectEntryIndices ────────────────────────────────────────
    # Build from subjectIndex field in manifest
    subject_indices = []
    for m in methods:
        si = m.get("subjectIndex", -1)
        if si >= 0:
            while len(subject_indices) <= si:
                subject_indices.append(-1)
            subject_indices[si] = m["index"]
    for i in range(len(subject_indices)):
        if subject_indices[i] < 0:
            subject_indices[i] = i

    lines.append(f"extern \"C\" const int kSubjectEntryCount = {len(subject_indices)};")
    lines.append(f"extern \"C\" const int kSubjectEntryIndices[{len(subject_indices)}] = {{")
    lines.append(", ".join(str(i) for i in subject_indices))
    lines.append("};")
    lines.append('')

    # ── RunFactAll ──────────────────────────────────────────────────
    lines.append('extern "C" CHAOS_IL2CPP_INT32 RunFactAll() {')
    lines.append('    CHAOS_IL2CPP_INT32 result = 0;')
    lines.append('    for (int i = 0; i < kAotMethodCount; i++) {')
    lines.append('        try {')
    lines.append('            RunNativeAot(i);')
    lines.append('        } catch (...) {')
    lines.append('            result |= (1 << i);')
    lines.append('        }')
    lines.append('    }')
    lines.append('    return result;')
    lines.append('}')
    lines.append('')

    # ── RunBenchmark ────────────────────────────────────────────────
    lines.append('extern "C" double RunBenchmark(int entry_index, int iterations) {')
    lines.append('    if (entry_index < 0 || entry_index >= kAotMethodCount)')
    lines.append('        return -1.0;')
    # Sentinel for instance methods
    lines.append('    static CHAOS_IL2CPP_UINT8 s_this_sentinel = 0;')
    lines.append('    auto* __this_ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_this_sentinel);')
    lines.append('    auto* __abi = chaos::il2cpp::runtime_core::GetRuntimeAbiV0();')
    if default_string_id != 0:
        lines.append(f'    auto __default_str = chaos_make_string_id_value({default_string_id}ULL);')
    lines.append('')
    lines.append('    switch (entry_index) {')

    for m in methods:
        idx = m["index"]
        symbol = m["nativeSymbol"]
        params = m.get("params", [])
        is_instance = m.get("isInstance", False)
        
        # Build argument list
        args = []
        if is_instance:
            args.append("__this_ptr")
        for p in params:
            if p.get("isString"):
                args.append("__default_str")
            else:
                args.append("0")
        
        arg_str = ", ".join(args)
        
        lines.append(f'    case {idx}: {{')
        lines.append(f'        auto start = std::chrono::steady_clock::now();')
        lines.append(f'        for (int i = 0; i < iterations; i++) {{')
        if arg_str:
            lines.append(f'            {symbol}({arg_str});')
        else:
            lines.append(f'            {symbol}();')
        lines.append(f'        }}')
        lines.append(f'        auto end = std::chrono::steady_clock::now();')
        lines.append(f'        return std::chrono::duration<double, std::milli>(end - start).count();')
        lines.append(f'    }}')

    lines.append('    default:')
    lines.append('        return -1.0;')
    lines.append('    }')
    lines.append('}')
    lines.append('')

    # ── RunHotpatchAll ──────────────────────────────────────────────
    lines.append('extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll() {')
    lines.append('    CHAOS_IL2CPP_INT32 result = 0;')
    lines.append('    for (int i = 0; i < kAotMethodCount; i++) {')
    lines.append('        auto& entry = s_hotpatch_entries[i];')
    lines.append('        if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry)')
    lines.append('            && !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry))')
    lines.append('        {')
    lines.append('            uint64_t __chaos_args[4] = {}; uint64_t __chaos_ret[2] = {};')
    lines.append('            chaos::il2cpp::runtime_core::InterpreterEntryDirect(')
    lines.append('                entry.method_key, __chaos_args, __chaos_ret);')
    lines.append('        } else {')
    lines.append('            kAotMethods[i]();')
    lines.append('        }')
    lines.append('    }')
    lines.append('    return result;')
    lines.append('}')
    lines.append('')

    # ── RunHotpatchBenchmark ────────────────────────────────────────
    lines.append('extern "C" double RunHotpatchBenchmark(int entry_index, int iterations) {')
    lines.append('    if (entry_index < 0 || entry_index >= kAotMethodCount)')
    lines.append('        return -1.0;')
    lines.append('    auto& entry = s_hotpatch_entries[entry_index];')
    lines.append('    auto start = std::chrono::steady_clock::now();')
    lines.append('    for (int i = 0; i < iterations; i++) {')
    lines.append('        if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry)')
    lines.append('            && !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry))')
    lines.append('        {')
    lines.append('            chaos::il2cpp::runtime_core::InterpreterEntryDirectFast(')
    lines.append('                entry.method_key);')
    lines.append('        } else {')
    lines.append('            kAotMethods[entry_index]();')
    lines.append('        }')
    lines.append('    }')
    lines.append('    auto end = std::chrono::steady_clock::now();')
    lines.append('    return std::chrono::duration<double, std::milli>(end - start).count();')
    lines.append('}')

    with open(output_path, 'w') as f:
        f.write('\n'.join(lines))
```

### 调用时机

在 `family_verification_orchestrator.py` 的 `_stage_codegen()` 中，`run_family()` 完成后：

```python
def _stage_codegen(family_slug, assembly, preflight, *, codegen_mode=None):
    # ... existing run_family() call ...
    
    if result.get("success"):
        # Generate verification dispatch code
        from generate_verification_dispatch import generate_verification_dispatch
        
        family_dir = _VERIFICATION_BASE / assembly / family_slug
        manifest_path = family_dir / "codegen" / assembly / "generated" / "methods-manifest.json"
        output_path = family_dir / "native" / "verification_dispatch.generated.cpp"
        
        if manifest_path.exists():
            generate_verification_dispatch(str(manifest_path), str(output_path))
            # Re-run CMake build to include the new generated file
            _run_cmake_build(family_slug, assembly)
```

---

## 6. runtime-entry.cpp 简化

```cpp
// Extern declarations — only what's needed
extern "C" void* kChaosExternalRuntimeFnTable[];
extern "C" const char* kChaosExternalRuntimeSubjects[];
extern "C" int32_t kChaosExternalRuntimeCount;
extern "C" void ChaosJitRegisterAll();
extern "C" std::int32_t RunNativeAot(std::int32_t);
extern "C" const int kAotMethodCount;

// Verification dispatch — generated by Python
extern "C" double RunBenchmark(int, int);

extern const uint8_t kPatchData[];
extern const size_t kPatchDataSize;
extern const char* const kPatchDataHostClassName;

enum class RunMode { Fact, Benchmark, HotUpdate, HotUpdateAndBenchmark, PatchAndBenchmark };

int main(int argc, char** argv) {
    // ... init (same as current) ...

    switch (mode) {
    case RunMode::Fact: {
        // Fact mode: run all methods via RunNativeAot
        int failed_count = 0;
        for (int i = 0; i < kAotMethodCount; i++) {
            try {
                RunNativeAot(i);
            } catch (...) {
                ++failed_count;
            }
        }
        int passed_count = kAotMethodCount - failed_count;
        printf("Passed: %d/%d\n", passed_count, kAotMethodCount);
        _exit(failed_count);
    }
    case RunMode::Benchmark: {
        // Benchmark: call generated RunBenchmark
        double elapsed_ms = -1.0;
        try {
            elapsed_ms = RunBenchmark(entry_index, iterations);
        } catch (...) {
            elapsed_ms = -1.0;
        }
        // ... JSON output (same format) ...
    }
    case RunMode::HotUpdate: {
        // All-methods hotupdate loop — generated RunHotpatchAll
        auto* patch_ctx = ApplyHotpatchIfAvailable();
        // ... (same as current) ...
    }
    // ... other modes ...
    }
}
```

关键变化：
- `RunNativeAotAll()` 声明删除 → inline loop 已足够
- `RunNativeAotBench()` 声明删除 → 不再需要
- `BenchmarkMethod()` → 重命名为 `RunBenchmark()`，由 Python 生成
- 删除 `kSubjectEntryCount`/`kSubjectEntryIndices` extern 声明

---

## 7. CMakeLists.txt 更新

```cmake
# Add verification dispatch generated file
set(CHAOS_VERIFICATION_DISPATCH
    "${CMAKE_CURRENT_SOURCE_DIR}/verification_dispatch.generated.cpp"
)

set(CHAOS_ENTRY_SOURCES
    "runtime-entry.cpp"
    "runtime-patchdata.cpp"
    ${CHAOS_VERIFICATION_DISPATCH}
    ${CHAOS_NATIVE_STUBS}
    ${CHAOS_CODEGEN_CPP}
)
```

---

## 8. 迁移计划

### 8.1 单 family 验证流程

```
1. chaos-il2cpp convert-to-cpp  →  native-aot.generated.cpp + methods-manifest.json
2. generate_verification_dispatch.py → verification_dispatch.generated.cpp
3. cmake --build . → entry.exe (包含 verification_dispatch.generated.cpp)
4. entry.exe --benchmark N I  →  RunBenchmark() in generated code
5. entry.exe --hotupdate      →  RunHotpatchAll() in generated code
```

### 8.2 迁移 ~50+ families 的步骤

| Step | 内容 | 涉及文件 |
|------|------|---------|
| **S1** | 精简 Scriban 模板，删除 5 块验证代码 | `NativeAot.DispatchEntryCode.cpp.scriban` |
| **S2** | 修改 `BuildDispatchEntryCode()` 仅渲染精简模板 + 输出 manifest | `NativeAotLoweringPlanner.cs` (L2757) |
| **S3** | 创建 `generate_verification_dispatch.py` | `build/.../foundation_dll/generate_verification_dispatch.py` |
| **S4** | 修改 `_stage_codegen()` 调用生成器 + 重编译 | `family_verification_orchestrator.py` |
| **S5** | 简化 `runtime-entry.cpp` | 各 family 的 `runtime-entry.cpp` |
| **S6** | 更新各 family 的 `CMakeLists.txt` | 各 family 的 `CMakeLists.txt` |
| **S7** | 删除 `fact_verifier.py` 中对 `RunNativeAotAll`/`BenchmarkMethod` 的引用 | `fact_verifier.py` |
| **S8** | 验证 threading-tasks-primitives 全流程 | 验证管线 |

### 8.3 向后兼容

- **旧 entry.exe 不含 `verification_dispatch.generated.cpp`**：`RunBenchmark` 符号不存在 → 链接失败。必须所有 family 同时迁移。
- **渐进方案**：S1+S2 先合入（codegen 精简），S3+S4+S5+S6 作为第二波合入。中间状态：旧的 dispatch 代码仍由 Scriban 生成，但不再使用（`runtime-entry.cpp` 仍调用 `BenchmarkMethod()`）。

更推荐的方案是在单个 PR 中完成所有步骤，避免中间状态。

---

## 9. 文件变更汇总

| 文件 | 操作 | 说明 |
|------|------|------|
| `src/managed/.../Templates/NativeAot.DispatchEntryCode.cpp.scriban` | **修改** | 删除 kBenchmarkWrappers, kSubjectEntryIndices, RunNativeAotAll, RunNativeAotBench, BenchmarkMethod |
| `src/managed/.../NativeAotLoweringPlanner.cs` | **修改** | BuildDispatchEntryCode 改为输出 manifest JSON + 精简模板 |
| `build/.../foundation_dll/generate_verification_dispatch.py` | **新建** | Python manifest→C++ 生成器 |
| `build/.../foundation_dll/family_verification_orchestrator.py` | **修改** | _stage_codegen 调用 generate_verification_dispatch |
| `build/.../foundation_dll/fact_verifier.py` | **修改** | 删除对 BenchmarkMethod 的引用 |
| `<family>/native/runtime-entry.cpp` | **修改** | ~50 files — 简化 extern 声明 |
| `<family>/native/CMakeLists.txt` | **修改** | ~50 files — 添加 verification_dispatch.generated.cpp |
| `<family>/codegen/.../methods-manifest.json` | **新建** | codegen 输出（~50 files） |
| `<family>/native/verification_dispatch.generated.cpp` | **新建** | Python 生成器输出（~50 files） |

---

## 10. 风险与缓解

| 风险 | 缓解 |
|------|------|
| **Codegen 输出路径不一致** — `_outputDir` 在 NativeAotLoweringPlanner 中可能不可靠 | 通过 `CodegenOptions.OutputDirectory` 传入 manifest 路径，或从 `native-aot.generated.cpp` 路径推导 |
| **Python 生成器与 C# codegen 不一致** — 默认参数生成逻辑有两份实现 | 验证：对同一 family，Python 生成的 `RunBenchmark` 调用签名与 Scriban 生成的 `BenchmarkMethod` 签名完全一致；通过 diff 现有生成的 benchmark 输出来验证 |
| **CMake 重编译** — 生成 dispatch 后重新 cmake 增加了 stage1 耗时 | verification_dispatch 代码变化不触发完整重编（仅新增 .cpp），cmake --build 增量编译 |
| **kBenchmarkWrappers 被 codegen 其他部分引用** | 搜索确认 kBenchmarkWrappers 仅在 Scriban 模板内引用，核心层不依赖它 |
