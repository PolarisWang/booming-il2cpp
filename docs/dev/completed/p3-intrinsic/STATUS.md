---
task_type: plan
phase: completed
lifecycle_status: completed
parent_task_id: 20260524-jit-perf-optimization
source_task_id: p3-intrinsic
source_relation: roadmap-child
roadmap_or_plan: pending
dispatch_model: sequential
child_execution_mode: auto
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
completed: 2026-05-24
---

# P3: Intrinsic 识别与展开 — STATUS

> 父任务: 20260524-jit-perf-optimization (JIT 性能优化路线)

## Scope

在树 IR 框架上识别常见 BCL 方法模式并替换为内联 x64 指令序列。Phase 1 覆盖当前 benchmark 热点：Math.Abs/Min/Max、Array.Length、GC write barrier。

## Summary

已完成所有 8 个实现任务：

1. **IR Opcode 扩展** — Abs(100), Min(101), Max(102), StFldBarrier(103)
2. **新 NodeKind** — kAbs(52), kMin(53), kMax(54) 在 jit_tree_node.h Gap 3
3. **IntrinsicMutator** — 识别表 + token 匹配 + 节点替换（Inliner 后、ConstFold 前）
4. **Linearizer** — 新 NodeKind → IROpCode 映射
5. **jit_engine.cpp 代码生成**:
   - Abs: cdq + xor + sub 序列
   - Min/Max: cmp + cmovcc (Min=cmovg, Max=cmovl)
   - StFldBarrier: 内联 `g_bgc_is_marking` 检查 + 条件分发到 CodegenStFldNoBarrier / CodegenStFld
6. **管线集成** — IntrinsicMutator 在 Inliner 后、ConstFold 前插入
7. **CMakeLists.txt** — jit_intrinsics.cpp 注册
8. **验证** — build 通过, fact 18/18, benchmark 30M ops/sec

## 文件变更

- 新增: src/native/jit/tree/jit_intrinsics.h, jit_intrinsics.cpp
- 修改: jit_tree_node.h, jit_linearizer.cpp, jit_optimizer.cpp, jit_engine.cpp, CMakeLists.txt
- 修改: jit_helpers.h — InitTlsTlabInfo namespace 修复

## Exit Criteria 满足情况

1. ✅ Math.Abs(int) → cdq + xor + sub 序列正确生成
2. ✅ Math.Min/Max → cmp + cmov 序列正确生成
3. ✅ Array.Length → 复用 kLdLen 内联加载
4. ✅ GC write barrier — StFldBarrier 内联 g_bgc_is_marking 检查 + 条件 barrier
5. ✅ convert-char fact 18/18 通过
