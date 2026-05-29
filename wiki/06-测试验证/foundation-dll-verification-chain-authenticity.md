# Foundation-DLL 验证链路真实性分析

## 概述

本文档追溯 foundation-dll family verification 中 native AOT/JIT 的 fact 和 benchmark 整条验证链路，确认是否真实地测试了 AOT 和 JIT 代码路径。

---

## AOT 验证链路 (entry.exe)

### 代码生成

```
C# Subject → dotnet build → IL DLL
                          → Chaos.IL2CPP.Driver convert-to-cpp --mode aot
                          → AotCoreIR lowering → Scriban C++ 模板发射
                          → native-aot.generated.cpp (完整 C++ 函数体)
```

`Chaos.IL2CPP.Driver` 执行完整 il2cpp 管线：IL 降低 → AotCoreIR → Scriban C++ 模板发射。与生产级 il2cpp 使用相同管道。**无桩无模拟。**

生成的文件如 `testing/foundation-dll/System.Private.CoreLib/garbage-collection/codegen/generated/generated/native-aot.generated.cpp`，AOT 模式下 `ChaosJitRegisterAll()` 为空 no-op：

```cpp
extern "C" void ChaosJitRegisterAll() {}
```

### 构建

MSVC 编译生成的 C++ → `entry.exe`，链接 `chaos_runtime_core`。由 `pipeline_native_aot_runner.py` 中的 `build_entry_executable()` 驱动。

### Fact

`entry.exe` 通过 `RunNativeAot()` 函数调用每个 subject method。该函数解析 AotCoreIR JSON，通过 `InterpreterEntryDirect` 或直接调用 AOT C++ 函数体。Fact 阶段每个方法调用一次，检查退出码与 `Passed: N/M` 输出。

AOT C++ 函数体操作的是 `InterpreterObject`/`InterpreterValue`（解释器数据模型），这是 Chaos IL2CPP 架构的选择（AOT C++ 函数体运行在统一解释器数据模型之上），**不是验证捷径**。与生产级 il2cpp 使用相同的 lowering 和后端。

### Benchmark

`std::chrono::steady_clock` 计时循环调用 AOT C++ 函数体 100000 次，真实 wall-clock 测量。

---

## JIT 验证链路 (entry-jit.exe)

### 代码生成

```
C# Subject → dotnet build → IL DLL
                          → Chaos.IL2CPP.Driver convert-to-cpp --mode jit
                          → AotCoreIR lowering → Scriban C++ 模板发射
                          → native-aot.generated.cpp (含 JitEntry 数组)
```

与 AOT 模式使用完全相同的 `Chaos.IL2CPP.Driver convert-to-cpp` 管线，仅 `--mode` 参数不同。

`NativeAotLoweringPlanner.cs:745-753` 在 JIT 模式下发射 `JitEntry` + `RegisterJitEntryMethods()`：

```csharp
if (_codegenMode == CodegenMode.Jit && methodCount > 0)
{
    globalDeclarations += "\n" + BuildJitMethodRegistration(methodsForLowering, metadataRegistration);
    globalDeclarations += @"
extern ""C"" void ChaosJitRegisterAll() {
    RegisterJitEntryMethods(kChaosJitEntries, kChaosJitEntryCount);
}
";
}
```

生成产物包含 `extern "C" const JitEntry kChaosJitEntries[N]` 数组，每条记录包含 AotCoreIr JSON、token、module_id。**注意：JIT 模式下生成的文件是临时的，不会提交到仓库**——已提交的 `native-aot.generated.cpp` 都来自 AOT 模式运行。

### 构建

```
entry-aot.exe (AOT 构建)
  → orchestrator 保存为 entry-aot.exe
  → 重新 codegen --mode jit
  → MSVC 编译含 JIT 库
  → 链接 chaos_jit.lib
  → entry-jit.exe
  → 恢复 entry.exe = entry-aot.exe
```

新管线 `testing/foundation-dll/verification/stages/codegen.py`（`run_jit_codegen`）：

```python
build_ok = _build_entry_exe(family_slug, ..., output_name="entry-jit.exe", is_jit=True)
# 构建完成后恢复 AOT entry.exe
shutil.copy2(str(aot_exe), str(entry_exe))
```

JIT 构建额外包含：
- `chaos_jit.lib` — 真实 JIT 编译器
- `chaos_debugger.lib` — 调试支持
- `/FORCE:MULTIPLE` — 链接器标志（JIT 与 AOT 符号冲突）

### 运行时入口

`entry-jit.exe` 启动后调用 `ChaosJitRegisterAll()` → `RegisterJitEntryMethods()` 为每个方法创建 `JitPrecode` + `PrecodeArena` 存根。首次调用触发存根 → `JitStubDispatchImpl` → `jit::Compile()` → `direct_ptr` 被修补为编译后的原生代码。详见 `jit_registration.h:30-38`。

### 层级升级系统 (Tiering)

`InterpreterEntryDirect` 中的层级阈值 (`interp_entry/entry_direct.cpp`)：

| 层级 | 触发条件 | 执行模式 |
|-----|---------|---------|
| T1 | 0 calls | Stack-interpreted |
| T2 | 100 calls | Register-mapped |
| T3 | 500 calls | AOT C++ |
| T4/JIT | 2000 calls | JIT-compiled x64 |

### Fact JIT

单次调用（1 iteration）→ 通过 `InterpreterEntryDirect` → **停留在 T1（栈解释执行）**。不触发真正 JIT 编译。实际结果是"通过解释器调度的 AOT C++"。

**结论：JIT Fact 不体现 JIT 编译。** 这是架构限制——单次调用无法触发层级升级。

### Benchmark JIT

100000 次迭代 → `InterpreterEntryDirect`：
- ~0-1999 次：T1→T2→T3，AOT C++
- ~2000 次之后：层级升级到 T4 → 调用 `jit::Compile()` → **真实 JIT x64 机器码**
- ~98000 次迭代：由 JIT 编译的机器码执行

**结论：~2% AOT C++，~98% 真实 JIT x64 机器码。部分真实，但对 benchmark 目的足够。**

### JIT 引擎真实性

`chaos_jit.lib` 是真实 JIT 编译器 (`src/native/jit/jit_engine.cpp`)：
- Chaitin-Briggs 图染色寄存器分配
- x64 机器码发射（覆盖 LdcI4/Add/Sub/Call/CallVirt/NewObj/Box 等指令）
- 精确 GC 槽映射
- Win64 `.pdata/.xdata` 展开信息（OS 级注册）
- 层级升级引擎 `InterpreterEntryDirect` 中的 T4 路径真实调用 `jit::Compile()` → `nm->code(args_buf, ret_buf)`

---

## 真实性评定汇总

| 阶段 | entry.exe (AOT) | entry-jit.exe (JIT) |
|------|----------------|-------------------|
| **Fact** | ✅ 真实 AOT C++ | ❌ 非真正 JIT。单次调用走栈解释/AOT C++ |
| **Benchmark** | ✅ 真实 AOT C++，数据模型为 InterpreterObject | ⚠️ 部分真实。~2% AOT，~98% 真实 JIT x64（层级升级后） |
| **Codegen** | ✅ 完整 IL→IR→C++ 管线 | ✅ 完整 IL→IR→JitEntry，使用新接口 |

---

## JitEntry 迁移状态

| 接口 | 状态 | 用途 |
|------|------|------|
| `JitMethodEntry` + `RegisterJitMethods()` | **已废弃** | 旧 JIT codegen，路由到解释器调度。保留向后兼容 |
| `JitEntry` + `RegisterJitEntryMethods()` | **当前使用** | 新 JIT codegen，precode 存根 → 真正 JIT 编译 |
| `HybridEntry` + `RegisterHybridMethods()` | **当前使用** | Hybrid 模式，计数器升级 AOT→JIT |

**结论：codegen C# 源已经使用新接口**（`NativeAotLoweringPlanner.cs:745-753`），`--mode jit` 发射 `JitEntry` + `RegisterJitEntryMethods()`。**无需代码修改。**

---

## 现存差距

1. **JIT Fact 不体现 JIT 编译** — 单次调用无法触发层级升级。如需真正的 JIT-only fact，需要在 fact 前增加预热阶段（预热到 T4 之后重置状态再执行 fact）。

2. **外部调用存根化** — `FillExternalRuntimeStubs()` 用无害值填充外部运行时表，可能掩盖路径差异（见 `runtime-entry.cpp`）。

3. **Fact 只检查崩溃** — 如果方法返回错误值但不崩溃，会被误判为"通过"。

---

## 关键文件路径

| 职责 | 路径 |
|------|------|
| 验证编排 | `testing/foundation-dll/verification/orchestration/engine.py` |
| Native AOT runner | `testing/foundation-dll/verification/stages/pipeline_native_aot_runner.py` |
| Codegen JIT 发射 | `src/managed/Chaos.IL2CPP.Generator/NativeAotLoweringPlanner.cs` |
| JitEntry 注册表发射 | `src/managed/Chaos.IL2CPP.Generator/NativeAotLoweringPlanner.ModuleRegistration.cs` |
| JIT 注册头文件 | `src/native/runtime-core/jit_registration.h` |
| JIT 编译器引擎 | `src/native/jit/jit_engine.cpp` |
| 层级升级入口 | `src/native/interpreter/interp_entry/entry_direct.cpp` |
| Driver 配置 | `src/managed/Chaos.IL2CPP.Driver/ConvertToCpp/ConvertToCppConfig.cs` |

---

## 历史

- 2026-05-24: 完成验证链路真实性分析，确认 AOT 链路完全真实、JIT 链路部分真实且 codegen 已使用新 JitEntry 接口
