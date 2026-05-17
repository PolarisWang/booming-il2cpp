---
task_id: hotupdate-hotpatch-dispatch
title: HotUpdate Hotpatch 双层分派架构实现
task_type: plan
lifecycle_status: complete
phase: complete (string-char-text-core 全链路验证通过)
created_at: 2026-05-06 12:00:00 +08:00
updated_at: 2026-05-10 13:15:00 +08:00
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# HotUpdate Hotpatch 双层分派架构

## 设计摘要

为 foundation-dll 验证体系实现真实 hotupdate 路径：patch 代码以托管 DLL 形式存在，构建期提取为 `.patchdata`，运行时通过 Hotpatch Entry Table + Interpreter 执行 IL，替代当前静态 3-TU C++ 预编译模式。

### 架构
Hotpatch 双层分派（原名 D3）：每个模块生成静态 Entry Table，call site 采用模式感知分支 — AOT→AOT 零额外开销，patch 时走 InterpreterEntryDirect → Lazy IL→IR 降低 → Interpreter 执行。PatchMetadataCache 本地自洽，不注册全局类型系统。

### 决策要点
- 方法匹配：运行时 Token 重映射（名称匹配 + NameIndex）
- Dispatch 机制：Hotpatch 双层 ABI — dispatch table + direct call
- Dispatch Table：Per-module 平面静态表，构建期确定
- 数据格式：FlatBuffer 风格 .patchdata
- IL→IR 降低：Lazy（首次调用降低）
- 卸载：逐方法恢复，第一阶段无 RCU

## 关键文档
- plan: `plan-v1-01.md`
- 设计决策: `wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/18-HotUpdateDispatchHotpatch-决策.md`

## 当前完成状态

### 已完成 Steps 1-8（全部完成）

| Step | 名称 | 状态 | 关键文件 |
|------|------|------|---------|
| 1 | NameIndex + Token→Slot emit | 完成 | `NativeAotLoweringPlanner.ModuleRegistration.cs` |
| 2 | 运行时 NameIndexRegistry + DispatchTable | 完成 | `hotpatch_table.h/.cpp` |
| 3 | 构建期 patch data 提取 | 完成 | `PatchDataExtractor.cs`, `DriverEntry.cs` |
| 4 | 运行时 PatchLoader | 完成 | `patch_loader.h/.cpp` |
| 5 | InterpreterEntryDirect | 完成 | `interpreter_entry.h/.cpp`, `interpreter_entry_test.cpp` |
| 6 | Codegen 模式感知分支 | 完成 | `ExceptionEmission.cs`, `ObjectModelEmission.cs` |
| 7 | HotUpdateTest 改造 | 完成 | `generate_hotupdate_test.py` → `HotUpdateTest.cpp` |
| 8 | O(1) AotCoreIr 索引 + patchdata 嵌入 | 完成 | `PatchDataExtractor.cs` (BuildAotCoreIrSection), `patch_loader.cpp` (GetAotCoreIr), `batch_hotupdate_runner.py` (--aot-core-ir) |

## 注意

- **已验证全链路的 family：** string-char-text-core（20 方法，baseline→patch→dispatch→revert 全通过，MSVC 编译错误已修复）、convert-char、guid-random-hashcode、interface-dispatch
- **patchdata 兼容性问题已解决：** 所有 31 个 family 的 `.patchdata` 已通过 `batch_hotupdate_runner.py` 重新生成并嵌入 AotCoreIr JSON（O(1) uint32_t 索引数组）。PatchDataHeader 统一为 112 字节（28 × uint32_t），C# 与 C++ 布局一致。
- **AotCoreIr 匹配修复：** `BuildAotCoreIrSection()` 改为按 method name 匹配（而非 TypeName::MethodName），兼容 NativeEntry ↔ PatchEntry 的命名差异。
- **CMake 构建：** `chaos_runtime_core` 库编译通过。HotUpdateTest 单独 target（`chaos_hotupdate_<slug>`）使用 `REPO_ROOT` 相对路径（而非 `CMAKE_SOURCE_DIR`），`link_directories` 替换为完整 `.lib` 路径，`RelWithDebInfo` 配置匹配预构建库。
- **MSVC 编译错误修复：** genuine-fixed 中 struct 成员缺失分号（`header{}` → `header{};`）、`/FI` 路径使用 `CMAKE_SOURCE_DIR` → `REPO_ROOT` 修复、fmt include 路径补全、RuntimeLibrary 匹配（RelWithDebInfo vs Debug）。

## 当前状态

**string-char-text-core 全链路验证通过。** 20/20 methods passed with full dispatch lifecycle verification (baseline→patch→dispatch→revert→restore)。MSVC genuine-fixed 编译错误已修复，exe 成功输出 JSON 验证报告。可以作为其他 family 的参考。

## 架构映射

### Dispatch 闭环
codegen emits dispatch tables + mode-aware call sites → runtime registers modules via RegisterHotpatchModule() → PatchLoader sets kHotpatchActive on dispatch entry → next invocation hits InterpreterEntryDirect → lazy IR deserialization → InterpreterVM executes

### 关键集成点
- **HotpatchNameRegistry**: Per-module flat tables (types, methods, slots), bsearch-based lookup
- **InterpreterEntryDirect**: Entry point for patched methods — AOT bridge setup, lazy IR lowering (DCLP), signature parsing, ExecutionFrame, InterpreterVM::Execute()
- **PatchMetadataCache**: Self-contained metadata access for patch data, no global type system registration
- **ResolveSubjectId**: Maps subjectId strings from AotCoreIr JSON to runtime call_target pointers

## 待解决

无。全链路验证已通过。
