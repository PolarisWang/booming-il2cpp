---
task_id: fd-p0-infra
task_name: Phase 0 基础设施搭建
task_type: plan
phase: completed
lifecycle_status: completed
parent_task_id: foundation-dll-industrialization
source_task_id: foundation-dll-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: lightweight-brainstorm
---

# Phase 0: Foundation DLL 验证基础设施搭建

## 验证结论

Brotli/decoder **13/13 ALL PASSED** (100% coverage, 101.5s)

### 各 Stage 结果

| Stage | Status | Detail |
|-------|--------|--------|
| preflight | ✅ | 3 methods |
| codegen AOT | ✅ | entry.exe 2027008 bytes |
| jit_codegen | ✅ | entry-jit.exe passes |
| fact AOT | ✅ | 3/3 |
| fact JIT | ✅ | 3/3 |
| audit | ✅ | 57 methods, audited=57 |
| asm_compare | ✅ | 3/3, expansion=0.73x |
| microbench | ✅ | 3 metrics |
| benchmark | ✅ | native-aot=3/3, native-jit=3/3 |
| hotupdate AOT fact | ✅ | 3/3 |
| hotupdate AOT bench | ✅ | 3/3 |
| hotupdate JIT fact | ✅ | 3/3 |
| hotupdate JIT bench | ✅ | 3/3 |

### 修复项

1. `batch_run_all.py`: 加 `--family` 和 `--skip-stages` 参数
2. `ConvertToCppHandler.cs`: RunMicrobench 恢复为 extern 声明（microbench.cpp 始终编译）
3. `pipeline_native_aot_runner.py`: `_run_convert_to_cpp` 清理逻辑保留 `_subjects_input` 目录

### JIT Codegen 验证

通过 Brotli/decoder 验证了 JIT codegen 通路。适配器通过旧管线 `run_family(codegen_mode="jit")` 生成 entry-jit.exe，然后复制回 testing/ 路径。

### HotUpdate 验证

entry.exe --hotupdate 和 entry.exe --hotupdate-and-benchmark 均正常工作。JIT variant 也通过。

### 已知的非阻塞问题

- dispatch code generation: `manifest not found` warning（不影响管线通过）
- probe build 始终 FAILED（不影响 subjects build 和后续 stages）
