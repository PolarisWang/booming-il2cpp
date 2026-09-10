# 2026-06-14 变更总结

## 1. ABI 表重建 (V0→V4)

### runtime_abi.h 结构体演化
- V0 基础 (34 fields): runtime_init → generic_context_get_method_arg  
- V1 代码生成器字段 (9 fields): register_module, hotpatch_is_active, hotpatch_should_keep_native, raise_null_reference_exception, external_runtime_fallback, interpreter_entry_direct, register_gc_layouts, register_hotpatch_module, array_empty
- V2 扩展 (20 fields): gc_alloc, gc_alloc_atomic, marshal/COM, delegate, DllImport, PInvoke error, GC finalization, thread state, raise_exception
- 所有 CHAOS_IL2CPP_INTPTR → uintptr_t (C ABI 兼容)

### abi_reflection.cpp 函数表
- RegisterModuleWrapper, HotpatchIsActiveWrapper 等 8 个类型安全桥接函数
- 63 个空指针表项 → 45 项真实填入
- 所有反覆写回 `runtime_core::` 直接调用 (V2 字段暂不通过 ABI 表)

## 2. Pipeline 编译错误修复 (12/12)

| Bug | Root Cause | Fix |
|-----|-----------|-----|
| C2556/C2371 | ExportCollector declaration regex missing «(» | /(;\|)\|(/)pattern |
| C2664 | ModuleDescriptor namespace collision | register_module → const void* |
| C4715 | Structured IR endfinally terminator leaks control flow | return {}; fallback |
| C4068 | #pragma GCC diagnostic unsupported by MSVC | #ifdef GNUC guard |
| C2382 | Assert_Complete noexcept mismatch | selectany weak symbol |
| LNK1107 | 8-byte stub libs | Build real chaos_runtime_core.lib + 10 others |
| C2362 | goto skip declaration+initialization | Separate declaration from assignment in EmitLinearDelegateInvoke |
| C2045 | Label redefinition at same IlOffset | _dinvCounter for unique label names |
| C2705 | goto into __try scope | do{...break;...}while(0) pattern |
| C2660 | Fallback extern «CHAOS_IL2CPP_INTPTR fn() noexcept» without params | Skip fallback extern — definition is self-hosting |
| C3878 | static auto* fn_thunk in non-namespace scope | Remove static from lambda |
| C2065 | Undeclared identifiers in generated code | Remove static lambda + clean interop_stubs |

## 3. 代码生成器改进

- C++ 代理调用标签唯一化 (C2045/C2705)
- 结构体作用域 goto→break 模式
- 声明与赋值的正确分离 (C2362)
- ldftn 中去除非法函数作用域内的 static

## 4. 基础设施

### Hephaestus 缓存分割
- Content hash: subjects DLL + metadata + runtime stubs
- Context fingerprint: 工具 DLL + 模板 (基于 mtime)
- Content match: 1s restore; Context+Content match: instant

### Reporting System
- reporting/models.py — PipelineRunReport, FactFailure, route_for_error()
- stages/reporting.py — Unified reporting stage
- tools/ai_export.py — AI-ready JSONL export
- 25/25 数据正确性测试通过

### Skill System
- 15/15 Expert BEGIN_AGENT_PROMPT blocks
- 3/3 workflow templates (dual, triple, deep-debug)
- CLAUDE.md 阻断门 + hook loaded_expert 验证
- .ai/ 运行时数据 gitignore 清理

### Python Post-Processing Cleanup
- build.py: Post-TPG C++ patching removed
- hotupdate_chunk.py: 6 LINK-HACK patterns eliminated
- managed_benchmark.py: csproj multi-target patching removed
- TPG Scriban templates: weak symbols, no-op assert, bridge redirect clean

## 5. 工具链 / 构建

- cmake /MP flag for MSVC parallel compilation
- Interop stubs 清理 (ChaosExternalRuntimeFallbackDefault 回退)
- 11 individual runtime .lib 组件成功编译

## 6. 人/时维度统计

- Commits: 98+
- Files changed: 200+
- Pipeline bugs resolved: 12 (100% clearance rate)
- Local build time improvement (before/after /MP): ~10min → ~2min
- ABI fields normalized: ~43 (C type-safe + codegen-facing)
