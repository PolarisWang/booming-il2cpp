# Interpreter Hotpatch 性能优化规划

## 阶段
`brainstorming`

## 问题背景
Hotpatch 后的方法通过 interpreter 执行，postPatchNsPerOp 高达 2600-8900ns，比 native AOT 慢 100-1000x。Dashboard 显示 13 个 performance violation (HU-AOT > 3x NET8)。

## 核心瓶颈 (按影响排序)
1. `call`/`callvirt` DoRaw 路径 — ~2000ns/op
2. `callvirt` MIC miss — ~2000-3000ns/op
3. `newarr` — O(n) element init via `vector<InterpreterValue>::resize`
4. `stfld`/`stelem` — InterpreterValue deep copy + GC barrier
5. `newobj` — 每次 `CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE`
6. `box` 未设 type_token → 后续 virtual dispatch MIC miss
7. Dispatch 固定开销 (histogram + debugger + switch)

## 关键文档
- `brainstorm-v1-01.md` — 完整方案对比与权衡

## 边界拍板
| 项目 | 决策 |
|------|------|
| 优化范围 | fast_dispatch.cpp (FastExecute) 内的所有 handler + dispatch loop |
| 非目标 | interpreter_vm.cpp (InterpreterVM fallback) 不优化 |
| 约束 | 不改 InterpreterValue 数据结构 (避免牵一发动全身) |
| 基准 | postPatchNsPerOp 作为提升衡量标准 |

## blocking_questions
- [ ] P0 的改动是否需要新的 benchmark 流程来验证提升？
- [ ] 如何量化每个优化点的独立提升 (需 per-method 对比)？
- [ ] P2 的 InterpreterValue deep copy 消除是否要动数据结构？

## question_clearance
`pending`

## 风险评估摘要
- P0 改动量小风险极低
- P1 typed array 需要 IR lowering 配合传递 element type
- P2 pool 分配需注意 domain 生命周期
- P2 deep copy 消除可能引入 use-after-free

## 三优先级权衡结论
`pending`
