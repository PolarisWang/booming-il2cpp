---
task_id: t4jit-B
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-t4jit-industrialization
source_task_id: 20260523-t4jit-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
preflight_review: pass
preflight_summary: OSR null deref 修复完成，Test_OsrEntry 通过。OsrPromote/OsrRepromotion 为预存问题（在 stash 验证中确认），不在本阶段范围内。
---

# t4jit-B — OSR segfault 修复（已完成）

## 完成证据

- **改动：**
  - `codegen_helpers.cpp:619-621` — `OsrResolveLoopHeader()` 增加 `instr_offsets==nullptr` 和 `instr_offset_count==0` 提前返回 nullptr
  - `code_generator.cpp:3543-3546` — OSR 入口生成代码增加 `test rax,rax; jne; ret; jmp rax` 空指针守卫
- **构建:** ✅ `test_codegen_native` 编译通过
- **测试:** ✅ Test_OsrEntry 通过（之前 segfault）
  - 64/65 测试通过，仅预存 `DeoptEntry_Registration` 失败
- **预存问题确认:** Test_OsrPromote (exit code 3) 在 stash 回退验证中也失败，确认为预存问题
- **arch_review:** ok — 纯防御性改动，不改变正常路径行为
