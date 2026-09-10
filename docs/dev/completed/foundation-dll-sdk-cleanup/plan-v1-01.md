# Foundation-DLL SDK 输出清理与 Python 验证框架精简计划

> **面向执行 Agent：** 必须使用 `dev-executing-plans` 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。执行完成时必须先归档当前子任务目录，再提交。

**目标：** 将测试代码从 SDK 输出中完全移除，由 Python 脚本直接生成 `runtime-entry.cpp`，消除所有 `_fix_*` 后处理函数，精简全部 Python 验证脚本。

**架构：** (1) C# Driver 不再生成 `runtime-entry.cpp`，SDK 输出只含生产 C++；(2) Scriban 模板只保留纯数据数组，移除所有测试/dispatch 函数体；(3) 热修补分发逻辑下沉到 runtime library header `hotpatch_dispatch.h`；(4) Python 在 `native/` 下直接生成 `runtime-entry.cpp`，生成前清理目录；(5) 修复 codegen bug 以消除全部 `_fix_*` 后处理。

**技术栈：** C# (Driver/Generator), Scriban (C++ 代码生成模板), C++ (runtime library header), Python (验证框架)

**架构审核模式：** critical

**结构告警重点：** codegen 与 Scriban 分层、Python 脚本职责拆分、重复消除

**权责图审核主题：** codegen 输出 authority（生产 vs 测试的边界）、Python 验证框架 authority

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: System.Private.CoreLib (convert-char 作为验证家族)
- capabilityItem: foundation-dll 全家族
- ownerSubjectId: n/a
- proofRequired: true
- benchmarkRequired: true
- hotupdateImpact: Smoke
- formalVerificationObjects: [convert-char AOT fact verification, convert-char JIT fact verification, convert-char hotupdate verification]
- requiredGates: collector -> registry -> workspace

**设计文档：** n/a（brainstorm 会话中确认的方案C，无独立设计文档）

**问题清零来源：** brainstorm 会话中用户直接确认

**计划来源：** direct-plan

**预期知识沉淀：** n/a（架构规则将通过 CLAUDE.md 或 wiki 沉淀）

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 任务列表

### 任务 1：修改 C# Driver — 停止生成 runtime-entry.cpp

**文件：** `src/managed/Chaos.IL2CPP.Driver/ConvertToCpp/ConvertToCppHandler.cs`

**改动：**
- 移除 `GenerateRuntimeEntryCpp()` 方法的调用（第 130-131 行和第 209-210 行）
- 从 `extraSources` 列表中移除 `"runtime-entry.cpp"`（第 126 行和第 204 行）
- `GenerateRuntimeEntryCpp()` 方法本身保留（可被 Python 用作模板引用，或标记为废弃）

**产出：** SDK 输出不再包含 `generated/runtime-entry.cpp`

### 任务 2：修改 Scriban 模板 — 只保留纯数据数组

**文件：** `src/managed/Chaos.IL2CPP.Generator/Templates/NativeAot.DispatchEntryCode.cpp.scriban`

**改动：**
- 保留：
  - `kMethodTable`（原名 `kAotMethods`）— 纯函数指针数组
  - `kDefaultArgThunks`（原名 `kBenchmarkWrappers`）— 纯 lambda 包装器数组
  - `kSubjectSlotMap`（原名 `kSubjectEntryIndices`）— 纯 subject→slot 映射数组
  - `__g_null_instance`（原名 `__g_benchmark_this_sentinel`）
- 移除（整个函数体删除）：
  - `RunNativeAot()` — 单方法分发
  - `RunNativeAotAll()` — 全方法循环
  - `RunNativeAotBench()` — 快速 benchmark 分发
  - `BenchmarkMethod()` — benchmark 计时循环
- 符号重命名：
  - `kAotMethods` → `kMethodTable`
  - `kBenchmarkWrappers` → `kDefaultArgThunks`
  - `kSubjectEntryIndices` → `kSubjectSlotMap`
  - `__g_benchmark_this_sentinel` → `__g_null_instance`

**产出：** codegen 输出的 `native-aot.generated.cpp` 只包含纯数据，无测试/分发函数体

### 任务 3：更新 NativeAotLoweringPlanner.cs — 适配新模板

**文件：** `src/managed/Chaos.IL2CPP.Generator/NativeAotLoweringPlanner.cs`

**改动：**
- 更新第 711-720 行的注释，反映新的纯数据语义
- `BuildDispatchEntryCode()` 方法中的 model 属性名做对应更新（如需要）
- 更新 `__g_benchmark_this_sentinel` → `__g_null_instance` 的引用（第 19 行对应 Scriban 参数）
- `verification_dispatch_manifest.json`（`methods-manifest.json`）保持不变（Python 仍需要方法元数据）
- `extern "C" const int kAotMethodCount` 声明保留（runtime-entry.cpp 通过 Python 生成后仍需要）

**产出：** C# Generator 与 Scriban 模板一致

### 任务 4：创建 runtime library header — hotpatch_dispatch.h

**文件：** `src/native/runtime-core/hotpatch_dispatch.h`（新建）

**内容：**
- `ChaosDispatchMethod(int32_t entry_index)` — 单方法分发函数：
  - AOT 模式：查询 `s_hotpatch_entries[entry_index]` → 检查 hotpatch 状态 → native 调用或 interpreter fallback
  - JIT 模式：通过 `s_hotpatch_entries[entry_index].direct_ptr` 直接调用
- `ChaosDispatchMethodAll()` — 循环全部分发
- `ChaosDispatchMethodBench(int32_t entry_index)` — 快速分发（无 setjmp）
- 使用 `CHAOS_EH_TRY` / `CHAOS_EH_CATCH_BEGIN` / `CHAOS_EH_END` 宏替代原始 try/catch
- 引用 `s_hotpatch_entries`、`kMethodTable`、`kDefaultArgThunks`、`kAotMethodCount`（这些通过 extern 声明）

**接口设计：**
```cpp
namespace chaos::il2cpp::runtime_core {

// Single-method dispatch: hotpatch check → native call or interpreter fallback.
// Returns 0 on success, -1 on invalid index.
int32_t ChaosDispatchMethod(int32_t entry_index) noexcept;

// Loop dispatch over all methods. Returns count of failures.
int32_t ChaosDispatchMethodAll() noexcept;

// Fast dispatch for benchmark (no setjmp, inline slot access).
int32_t ChaosDispatchMethodBench(int32_t entry_index) noexcept;

}  // namespace chaos::il2cpp::runtime_core
```

**注意：** 这个 header 的实现在 AOT vs JIT 模式下有不同路径，通过 `#ifdef` 或编译单元区分。

### 任务 5：修复 codegen bug — 消除 _fix_* 后处理

**目标：** 消除 `pipeline_native_aot_runner.py` 中的全部 13 个 `_fix_*` / `_patch_*` / `_ensure_*` 函数

| # | 函数 | 根因 | 修复位置 |
|---|------|------|---------|
| 1 | `_fix_runtime_entry` | runtime-entry 模板 bug + 功能缺失 | **消除** — Python 直接生成，不需要修复模板 |
| 2 | `_fix_native_aot_bridge_thunks` | Codegen 未 emit bridge thunk 的静态符号 | C# `NativeAotLoweringPlanner.cs` — 在 codegen 输出中添加静态 stub 定义 |
| 3 | `_fix_t4_jit_include` | 缺少 `#include "jit_registration.h"` | Scriban 模板 — 在 JIT 模式下自动添加 include |
| 4 | `_fix_aot_chaos_jit_register_all` | AOT 模式下 ChaosJitRegisterAll 非空 | C# — AOT 模式直接 emit 空函数体 |
| 5 | `_fix_eeclass_strings` | EEClass 初始化中字符串字段未引号包裹 | Scriban 模板 — 修正字符串字段的 emit 格式 |
| 6 | `_fix_eeclass_registration` | TypeInfoV0→MethodTable 类型不匹配 | C# — 修正 CodeRegistration 结构体生成 |
| 7 | `_fix_forward_declarations` | 缺少泛型分发包装器的前向声明 | C# — 在生成代码开头添加必要的前向声明 |
| 8 | `_fix_page_file_decls` | 分页文件 include 路径错误 + 缺少前向声明 | C# + Scriban — 修正 include 路径和 extern 声明 |
| 9 | `_fix_dispatch_externs` | verification_dispatch.generated.cpp 中重复定义 | C# — 确保生成代码使用 extern 声明而非定义 |
| 10 | `_patch_generated_files` | 0xC0000409 /GS 栈 cookie 绕过 | 评估：与 runtime-entry.cpp 的 EH 策略相关，消除 runtime-entry.cpp 后不再需要 |
| 11 | `_patch_enum_dispatch_guard` | enum_metadata.generated.h 缺少 #ifndef guard | Scriban 模板 — 添加 header guard |
| 12 | `_ensure_microbench_source` | microbench.cpp 自动生成 | 保留简化版 — 由 Python 直接生成 |
| 13 | `_ensure_cmakelists` | CMakeLists.txt 自动生成 | 保留简化版 |

**产出：** Python 脚本中不再包含 `_fix_*` / `_patch_*` 调用

### 任务 6：重构 Python 验证脚本

**文件：** `build/toolchains/run/testing/foundation_dll/`

#### 6a: `family_entrypoint_generator.py` — 新增 runtime-entry.cpp 生成函数

- 新增 `generate_runtime_entry(native_dir, methods_manifest, codegen_mode)` 函数
- 生成的 `runtime-entry.cpp` 包含：
  - `main()` 入口和参数解析（Fact/Benchmark/HotUpdate/Microbench）
  - 引用 `ChaosDispatchMethod()` 来自 `hotpatch_dispatch.h`
  - 使用 `CHAOS_EH_TRY` / `CHAOS_EH_CATCH_BEGIN` / `CHAOS_EH_END` 宏
  - HotUpdate 语义验证逻辑（baseline → patch → verify → unpatch → revert）
  - FillExternalRuntimeStubs() 逻辑（从 C# 模板迁移）
- 生成前清理 `native/` 目录（除 build/ 等缓存目录）

#### 6b: `pipeline_native_aot_runner.py` — 精简主流程

- 删除所有 `_fix_*`、`_patch_*`、`_ensure_*` 函数（共 13 个）
- 主流程简化为：
  1. 生成 subjects DLL
  2. 运行 `convert-to-cpp --sdk-out`
  3. Sync 生成的 C++ 到 `native/`（清理后）
  4. 调用 `generate_runtime_entry()` 生成 `native/runtime-entry.cpp`
  5. 生成/更新 CMakeLists.txt
  6. 生成 verification_dispatch.generated.cpp
  7. 运行 cmake build
  8. 运行 entry.exe 验证
- 清理 `native/` 目录的语义：
  - 删除所有之前同步的 `.cpp` / `.h` 文件
  - 保留 cmake 构建缓存（build/ 目录）
  - 保留 enum_stubs 等手工维护的辅助文件

#### 6c: `family_verification_orchestrator.py` — 适配新流程

- 适配新的 Python 函数签名（不再引用 `_fix_*`）
- 确保批量验证流程与新 pipeline 一致

#### 6d: `fact_verifier.py` — 最小更新

- 如有需要，适配 Fact 模式的输出解析（输出格式不变，Passed N/M）

**产出：** Python 脚本总行数从 ~7769 行大幅缩减，职责清晰，无后处理 hack

### 任务 7：验证 — convert-char 家族全模式验证

**测试步骤：**

1. **SDK 输出验证：**
   ```bash
   TMPDIR="D:/temp/chaos-sdk-test"
   rm -rf "$TMPDIR"
   dotnet exec Chaos.IL2CPP.Driver.dll convert-to-cpp --assembly <dll> --sdk-out "$TMPDIR" --full-closure --mode Aot
   ls "$TMPDIR/generated/runtime-entry.cpp"  # 应不存在
   ```

2. **AOT Fact 验证：**
   ```bash
   cd build/toolchains/run/testing/foundation_dll
   python pipeline_native_aot_runner.py System.Private.CoreLib convert-char
   # 预期: 13 阶段全部 Passed
   ```

3. **JIT Fact 验证：**（如有 JIT 能力）
   ```bash
   python pipeline_native_aot_runner.py System.Private.CoreLib convert-char --mode Jit
   ```

4. **HotUpdate 验证：**
   ```bash
   # 通过 family_verification_orchestrator.py 运行
   ```

5. **回归验证：** 选择 3-5 个代表性家族验证不破坏现有流程

---

## 执行顺序

```
任务 4 (hotpatch_dispatch.h)         ← 其他任务依赖此 header
  ↓
任务 2 (Scriban 模板) + 任务 3 (C# Generator)    ← 可并行
  ↓
任务 1 (C# Driver)
  ↓
任务 5 (codegen bug 修复)           ← 需要先确认模板正确
  ↓
任务 6 (Python 脚本重构)            ← 依赖以上全部
  ↓
任务 7 (验证)
```

## 风险

1. **codegen bug 修复范围可能超出预期** — 13 个 `_fix_*` 函数中有些是深层 bug，可能无法全部一次性修复。**缓解**：如果某个 bug 修复难度过大，可在 Python 层保留最小 workaround 并明确标记为"待 codegen 修复"。
2. **hotpatch_dispatch.h 的 EH 宏兼容性** — 需要确保 `CHAOS_EH_TRY/CATCH_BEGIN/END` 在所有目标平台（Win32 SEH / iOS setjmp / macOS CPP_THROW）上正确工作。**缓解**：参考 `eh.h` 中已有实现，充分测试 AOT/JIT 两种模式。
3. **Python 生成的 runtime-entry.cpp 符号一致性** — 必须与 codegen 输出的符号名精确匹配。**缓解**：Python 从 `methods-manifest.json` 读取符号名和计数，确保一致。
4. **回归风险** — 33+ 个 foundation-dll 家族可能有个别因重构而失败。**缓解**：选择 convert-char 作为主要验证家族，完成后再对 3-5 个代表性家族做回归验证。
