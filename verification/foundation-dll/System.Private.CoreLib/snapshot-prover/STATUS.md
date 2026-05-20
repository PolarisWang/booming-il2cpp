# Snapshot-Prover Family: Status & Findings

## 架构映射

| Layer | Component | Role |
|-------|-----------|------|
| Managed | `SnapshotProverSubjects` | 10 probe methods exercising IL patterns (arithmetic, callvirt, boxing, loops, generics, ldftn*) |
| Codegen (C++) | `chaos_generated_module.cpp` | A1 typed dispatch table (`kFunctions`, `kFunctionsFlat[]`) + A2 proxy wrappers |
| Runtime | `runtime-entry.cpp` | Entry point with `ChaosRuntimeHost` + verification dispatch |
| Patch | `SnapshotProverPatchEntry` | Hotupdate patch DLL with custom entry for ldftn workaround |

## 验证状态

| Stage | Status | Details |
|-------|--------|---------|
| Preflight | ✅ | 10 methods, 1 custom entry (index 8 — ldftn gap) |
| Codegen | ✅ | entry.exe builds, patch DLL builds |
| Fact AOT | ✅ | 157/157 |
| AsmCompare | ✅ | 10/10 methods, IR expansion 0.35x |
| HotUpdate AOT | ⚠️ | 148/158 — 10 methods fail due to ldftn codegen gap (Subject_8) |
| Benchmark | ⚠️ | Skipped — no managed harness |

## HotUpdate 148/158 分析

### 背景
Old verification tested only 29/157 patched methods (`allMethodsRevertVerified: false` in legacy report). A1+A2 expanded coverage to all 157 dispatch table entries. Fact mode passes 157/157, confirming A1+A2 dispatch works correctly.

### 失败根因
Subject_8 使用 `ldftn` IL 指令（`LdftnHelper.GetFnPtr`）。`StructuredIR` planner 在完整程序模式下对此指令处理不正确（嵌套 if-then-else 中的 slot stack underflow）。9 个 patch 变体方法因此回退到 `InterpreterEntryDirect`，其中 6 个因解释器缺少完整的 ldftn 支持而失败。另有 6 个从 Subject 入口级联失败。

### 当前缓解
Subject_8 被标记为 `customEntry`，使用自定义 no-op 替代以保障其余 9 个 subjects 的正常验证。未来需要在 codegen IR 层面完整支持 `ldftn` 以消除该 gap。

## 已知问题

1. **Ldftn codegen gap**: Subject_8 uses ldftn IL instruction — IR planner doesn't handle it
2. **entry-jit.exe not built**: JIT codegen path not configured for this family
3. **Benchmark harness**: No managed harness output — auto-benchmark skipped
