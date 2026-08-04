---
task_id: 20260522-hotupdate-jit-fact-seh
task_type: fix
status: completed
created_at: 2026-05-22
completed_at: 2026-05-22
---

## HotUpdate JIT Fact 验证 + SEH 路由改进

### A: hotupdate_jit_fact 全家族验证

- 编写 `run_hotupdate_jit_fact_all.py` 批量运行 47 个 family
- 初筛: 46/47 PASS, 1 FAIL (snapshot-prover 156/157)
- C.1 修复后: snapshot-prover 升至 **157/157 PASS**

### B: 预存问题修复

| 项 | 家族 | 修复 | 结果 |
|----|------|------|------|
| B1 | garbage-collection | Subjects 5/6 非确定性断言改为确定性检查 | fact AOT 7/7, JIT 6/6 |
| B2 | compound family 目录 | 删除残留 | pipeline 不再报错 |
| B3 | thread-sync 目录 | 删除残留 | pipeline 不再报错 |

### C: Reg_Div/Reg_Rem SEH 路由

#### C.1 — 异常传播 (`entry_direct.cpp`)
- 移除 `InterpreterEntryDirect` 和 `InterpreterEntryDirectFast` 的 `noexcept`
- RegisterExecute 返回 false 且 `threw_exception=true` 时：
  - 无 SEH clauses: `throw chaos_managed_exception{}`
  - 有 SEH clauses: fall through 到 Step D (InterpreterVM)
- **效果**: 顺带修复了 snapshot-prover hotupdate_jit_fact 的 156→157（之前的 RegisterExecute 异常被静默吞掉）

#### C.2 — 异常对象哨兵 (`ir_reg_alloc.cpp`)
- Reg_Div/Reg_Rem 零除时设置 `frame.exception_obj` 和 `frame.pc = 9999`

#### C.3 — 硬件 SEH 包装 (`entry_direct.cpp` + `ir_reg_alloc.cpp`)
- 移除 `ir_reg_alloc.cpp` 中 Reg_Div/Reg_Rem 的 `if (r == 0)` 软件检查
- 在 `entry_direct.cpp` Step B 用 `CHAOS_EH_TRY`/`CHAOS_EH_CATCH` (`__try/__except`) 包裹 `RegisterExecute` 调用
- 硬件 `EXCEPTION_INT_DIVIDE_BY_ZERO` 被 SEH filter 捕获 → 设置 `threw_exception=true` → C.1 传播机制处理
- VEH handler (`T4VectoredExceptionHandler`) 对非 T4 代码返回 `EXCEPTION_CONTINUE_SEARCH`，不会干扰

### Collateral 修复

- **`gc_layout.h`**: 补回 `#include <chaos/unordered_dense.h>`（本地修改误删，导致 entry build 失败）
- **`bgc_stubs.cpp`**: 删除 `snapshot-prover/native/bgc_stubs.cpp`（chaos_runtime_core.lib 已提供 `BgcRecordRootChange`）
- **`family_entrypoint_generator.py`**: 移除 `_SKIP_AUDIT` 未使用导入
- **`test_code_generator.py`**: 补回 stash 中丢失的 `_build_call_expr_with_ref_locals`、`_get_skip_reason`、`_cast_return_to_int` 函数

### 架构变更

- **SEH 层**: interpreter 层 Step B 现在有硬件异常保护，不再依赖软件零除检查
- **异常传播**: 非 SEH 方法的 interpreter 异常通过 `throw chaos_managed_exception` 向上传播，SEH 方法 fall through 到 InterpreterVM (Step D)
- **Step D 约束**: v2 IR placeholder 仍不支持完整 SEH dispatch（`findCatchHandler`/`setupFinallyUnwind`），需要 IR v2 拥有完整 IL body

### 验证结果

| family | stage | result |
|--------|-------|--------|
| snapshot-prover | fact_jit | 157/157 PASS |
| snapshot-prover | hotupdate_jit_fact | 157/157 PASS |
| garbage-collection | fact | 7/7 PASS |
| garbage-collection | fact_jit | 6/6 PASS |
