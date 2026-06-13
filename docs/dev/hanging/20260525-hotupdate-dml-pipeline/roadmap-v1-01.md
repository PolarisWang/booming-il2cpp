# Roadmap: Foundation-DLL HotUpdate DML 管线修复

## 目标

让 foundation-dll 验证管线的 hotupdate 阶段（fact + benchmark）真正走解释器路径，而非当前的假 AOT native 路径。

## 问题链

```
runtime-patchdata.cpp 是空桩 (kPatchDataSize=0)
  → ApplyHotpatchIfAvailable() 不做任何事
    → kHotpatchKeepNative (0x02) 阻止 interpreter 路由
      → ChaosDispatchMethod 永远走 thunks[index]()
        → benchmark 和 fact 和 AOT 一模一样的 native 代码
```

## 架构

```
patch DLL (managed/patch/*.cs)
  │ dotnet build
  ▼
patch.dll
  │ emit-patch-data CLI
  ▼
subjects.patchdata (二进制 blob)
  │ _generate_runtime_patchdata_cpp()
  ▼
runtime-patchdata.cpp (kPatchData/kPatchDataSize/kPatchDataHostClassName)
  │ cmake --build (仅此文件变化)
  ▼
entry.exe (含真实 patch data)
  │ --hotupdate / --hotupdate-and-benchmark
  ▼
ApplyHotpatchIfAvailable() → SetPatchedBySlot()
  │ 清除 kHotpatchKeepNative + 设置 kHotpatchActive
  ▼
ChaosDispatchMethod() → HotpatchIsActive() && !HotpatchShouldKeepNative()
  → InterpreterEntryDirect()  ← 真正走解释器
```

---

## 阶段划分

```
Phase 1: Foundation-DLL 管线接入 emit-patch-data
    │ 新增 hotupdate_runner.py: _ensure_patch_data()
    │ 依赖: project_pipeline_adapter.py 中的 4 个工具函数
    ▼
Phase 2: 修复 RunHotupdateBenchmarkMode 未 apply patch
    │ runtime-entry.cpp: ApplyHotpatchIfAvailable() before 计时
    ▼
Phase 3: SetPatchedBySlot 清除 kHotpatchKeepNative
    │ hotpatch_table.cpp: ~kHotpatchKeepNative 在 OR kHotpatchActive 时
    ▼
Phase 4: 验证 convert-char 全流程
    │ 修复 Program.cs → 全链运行 → 确认 interpreter 路径 + 回归
```

---

## Phase 1: Foundation-DLL 管线接入 emit-patch-data

### 改动文件

`testing/foundation-dll/_core/python/hotupdate_runner.py`

### 改动内容

1. **import 四个工具函数**（从 `project_pipeline_adapter.py` 复用，或内联到 foundation-dll 的 toolchain 路径）：

   ```python
   from _core.python.project_pipeline_adapter import (
       _build_patch_dll,
       _run_emit_patch_data,
       _generate_runtime_patchdata_cpp,
       _write_sentinel_patchdata,
   )
   ```

   **注意**：`project_pipeline_adapter.py` 在 `testing/project-test/_core/python/` 下，foundation-dll 的 `hotupdate_runner.py` 在 `testing/foundation-dll/_core/python/` 下。需要确认路径可达性，或在 foundation-dll 下独立复制这四个函数。

2. **新增 `_ensure_patch_data(ctx)` 编排函数**：

   ```python
   def _ensure_patch_data(ctx: FamilyContext) -> bool:
       """构建 patch DLL → emit-patch-data → 生成 runtime-patchdata.cpp → 重编 entry.exe"""
       family_dir = ctx.family_dir
       native_dir = family_dir / "native"
       
       # 1. Build patch DLL
       dll = _build_patch_dll(ctx)
       if not dll:
           return False
       
       # 2. Run emit-patch-data CLI
       patchdata = native_dir / "subjects.patchdata"
       aot_core_ir = native_dir / "ConvertCharSubjects" / "generated" / "aot-core-ir.json"
       ok = _run_emit_patch_data(str(dll), str(patchdata), aot_core_ir_path=str(aot_core_ir))
       if not ok:
           return False
       
       # 3. Generate runtime-patchdata.cpp
       # 需要确定 host_class_name — convert-char 的 AOT 类型名可能是 "ConvertCharSubjects"
       host_class_name = _detect_host_class(native_dir)
       _generate_runtime_patchdata_cpp(patchdata, native_dir / "runtime-patchdata.cpp", host_class_name)
       
       # 4. Rebuild entry.exe (only runtime-patchdata.cpp changed)
       subprocess.run(["cmake", "--build", str(native_dir / "build")], check=True)
       return True
   ```

3. **修改 `run_hotupdate()`**：在 `_run_hotupdate_fact()` 前调用 `_ensure_patch_data()`，`finally` 块调用 `_write_sentinel_patchdata()`

4. **修改 `run_hotupdate_aot_bench()`**：同上

### 风险

| 风险 | 影响 | 缓解 |
|------|------|------|
| project_pipeline_adapter.py 路径不可达 | import 失败 | 直接复制四个函数到 foundation-dll 工具链 |
| patch DLL build 依赖 NuGet | 构建失败 | patch DLL csproj 必须自包含（无外部包依赖） |
| emit-patch-data CLI 找不到 | 流程中断 | try/except 包围，失败时 fallback 到 sentinel |

---

## Phase 2: 修复 RunHotupdateBenchmarkMode

### 背景

`runtime-entry.cpp` 的 `RunHotupdateMode()`（line 113–174）**已经**调用了 `ApplyHotpatchIfAvailable()`——这是 fact 路径。但 `RunHotupdateBenchmarkMode()`（line 187–200）没有调用，直接在未 apply patch 的状态下跑计时。

### 改动文件

`testing/foundation-dll/System.Private.CoreLib/convert-char/native/runtime-entry.cpp`

### 改动内容

在 `RunHotupdateBenchmarkMode()` 的计时循环前插入 patch apply：

```cpp
static int RunHotupdateBenchmarkMode(int entry_index, int iterations) {
    ApplyHotpatchIfAvailable();  // ← 新增
    double elapsed_ms = RunHotpatchBenchmark(entry_index, iterations);
    // ... 其余不变
}
```

### 影响范围

仅 `--hotupdate-and-benchmark` 路径。`--benchmark` 路径不受影响（走 `RunBenchmarkMode`，不调用此函数）。

---

## Phase 3: SetPatchedBySlot 清除 kHotpatchKeepNative

### 背景

codegen 对所有 AOT 模式的方法标记了 `kHotpatchKeepNative = 0x02`。即使 patch 被成功 apply，`ChaosDispatchMethod()` 检查 `HotpatchShouldKeepNative(entry)` 返回 true，跳过 interpreter 回到 native thunks。

### 改动文件

`src/native/runtime-core/hotpatch_table.cpp`

### 改动内容

`SetPatchedBySlot()` 中，在设置 `kHotpatchActive` 的同时清除 `kHotpatchKeepNative`：

```cpp
// hotpatch_table.cpp, SetPatchedBySlot(), "if (patched)" 分支
// release: method_key visible before flags
uint8_t old_flags = _InterlockedOr(reinterpret_cast<volatile long*>(&entry->flags),
                                   kHotpatchActive | ~kHotpatchKeepNative);
```

**注意**：需要改为两步操作——先通过 `_InterlockedAnd` 清除 `kHotpatchKeepNative`，再 `_InterlockedOr` 设置 `kHotpatchActive`。或者如果原子语义允许，用 CAS 循环一次完成。

推荐实现：

```cpp
// Clear kHotpatchKeepNative first (relaxed ordering — no reader depends on this)
_InterlockedAnd(reinterpret_cast<volatile long*>(&entry->flags),
                static_cast<long>(~(unsigned long)kHotpatchKeepNative));
// Then set kHotpatchActive with release ordering
_InterlockedOr(reinterpret_cast<volatile long*>(&entry->flags), kHotpatchActive);
```

### 影响

- **正效应**：patch apply 后，方法会走 interpreter（预期的正确行为）
- **副作用**：patch unpatch 后 `kHotpatchKeepNative` 不会恢复——但 unpatch 意味着方法回到 AOT native，`kHotpatchActive` 被清除，`kHotpatchKeepNative` 的值不再影响 dispatch（因为 !HotpatchIsActive 短路了）
- **安全**：仅 patch 状态下的方法受影响，非 patched 方法行为不变

---

## Phase 4: 验证 convert-char 全流程

### 前置步骤

1. **修复 Program.cs**（patch DLL 入口）：

   ```
   testing/foundation-dll/.../convert-char/managed/patch/Program.cs
   问题: Method0-Method17 不存在 → 改为 Subject_0-Subject_17
   ```

### 验证清单

1. **Phase 1+2+3 全链运行**：
   ```
   cd testing/foundation-dll
   python batch_run_all.py --family convert-char
   ```

2. **hotupdate fact 输出确认**：
   - `entry.exe --hotupdate` 输出 JSON 中包含 `"passedMethods": N`
   - N 应该为 18（所有 subject 通过）或预期值
   - 如果 interpreter 还不支持某些 IL，应反映在 fail count 中

3. **hotupdate benchmark 数据确认**：
   - `entry.exe --hotupdate-and-benchmark N I` 返回正值
   - 耗时应比 AOT benchmark 明显更慢（10-100x），因为走 interpreter 路径

4. **回归验证**：
   - AOT fact: `entry.exe` → 通过，数据与之前一致
   - AOT benchmark: `entry.exe --benchmark N I` → 正常，与之前一致
   - 无 patch 时: `entry.exe --hotupdate` → 不 crash，semantic check 通过

### 临时调试辅助

如果全链失败，可分步验证：

```bash
# 1. 独立验证 patch data 构建
dotnet build managed/patch/
dotnet exec Chaos.IL2CPP.Driver.dll emit-patch-data \
  managed/patch/bin/patch.dll native/subjects.patchdata \
  --aot-core-ir native/ConvertCharSubjects/generated/aot-core-ir.json

# 2. 验证 runtime-patchdata.cpp 生成
python _tools/generate_patchdata_cpp.py native/subjects.patchdata native/runtime-patchdata.cpp

# 3. 验证 rebuild
cmake --build native/build

# 4. 手动运行验证
native/entry.exe --hotupdate
native/entry.exe --hotupdate-and-benchmark 0 1000
```

---

## 文件改动总表

| Phase | 文件 | 改动类型 | 改动量 |
|-------|------|----------|--------|
| 1 | `testing/foundation-dll/_core/python/hotupdate_runner.py` | 修改 | ~80 行新增 |
| 1 | `testing/foundation-dll/_core/python/project_pipeline_adapter.py`（如不可复用） | **新建** | 复制四个函数 |
| 2 | `testing/foundation-dll/.../convert-char/native/runtime-entry.cpp` | 修改 | +1 行（函数调用） |
| 3 | `src/native/runtime-core/hotpatch_table.cpp` | 修改 | +2 行（_InterlockedAnd） |
| 4 | `testing/foundation-dll/.../convert-char/managed/patch/Program.cs` | 修改 | MethodN → Subject_N |
| 4 | `testing/foundation-dll/.../convert-char/managed/patch/ConvertCharPatchEntry.cs` | 确认 | ~18 个 Subject_N 函数 |

---

## 执行建议顺序

```
Phase 3 → Phase 2 → Phase 1 → Phase 4
```

理由：
- Phase 3（C++ 原子标志清除）无外部依赖，可独立编译验证
- Phase 2（runtime-entry.cpp 加一行）同样独立
- Phase 1（Python 管线改动）涉及外部 CLI 和 cmake，需确认环境
- Phase 4 是最终验证

---

## 依赖关系

```
Phase 3 ──┐
          ├── Phase 4（验证） ← 必须等前三个 phase 全部完成
Phase 2 ──┘
          │
Phase 1 ──┘
```

Phase 1/2/3 可独立执行互不阻塞，但 Phase 4 需要三者全部就绪。
