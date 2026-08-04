---
task_type: plan
phase: completed
parent_task_id: 20260524-jit-perf-optimization
source_task_id: p4-bounds-check
source_relation: roadmap-child
roadmap_or_plan: plan-v1-01.md
dispatch_model: sequential
child_execution_mode: auto
entry_skill: lightweight-brainstorm
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
created: 2026-05-24
completed: 2026-05-24
---

# P4: 边界检查消除 — STATUS (完成)

> 父任务: 20260524-jit-perf-optimization (JIT 性能优化路线)

## Scope

在树 IR 框架上消除冗余数组边界检查，覆盖：
1. **常量索引编译期消除** — 常量索引 + NewArr 已知大小传播
2. **LdLen 常量折叠** — NewArr 常量大小在 TreeBuilder::Build() 时折叠为 LdcI4
3. **NULL check coalescing** — BB 内同一数组 vreg 的非空追踪（P4 为信息性）

## 父级已拍板设计决策（直接继承）

- **IR 框架**：单 BB 表达式树 + VN + CSE（D3）
- **Pass 编排**：函数式，预留 PassManager 迁移（D4）
- **优化管线位置**：ConstFold 后、CSE 前插入 BoundCheckEliminator pass

## 实现摘要

| 任务 | 文件 | 状态 |
|------|------|------|
| LdElem/StElem 纳入树 IR | jit_tree_node.h, jit_tree_builder.h/cpp, jit_linearizer.cpp | ✅ |
| NoCheck 辅助函数 | jit_helpers.h/cpp | ✅ |
| IROpCode 扩展 | ir_opcodes.h (LdElemNoChk=104, StElemNoChk=105, LdElemANoChk=106) | ✅ |
| BoundCheckEliminator Mutator | jit_bounds_check.h/cpp (新增) | ✅ |
| Codegen NoCheck 变体 | jit_engine.cpp | ✅ |
| 管线集成 | jit_optimizer.cpp | ✅ |
| CMakeLists.txt | CMakeLists.txt | ✅ |

## 验证结果

- build: ✅
- fact 18/18: ✅
- benchmark 27.1M ops/sec (no regression): ✅
- --no-opt fact 18/18: ✅
