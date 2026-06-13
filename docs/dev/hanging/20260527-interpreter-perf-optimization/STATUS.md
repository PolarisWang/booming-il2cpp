# Interpreter Hotpatch 性能优化规划

## 阶段
`writing-plans`

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
| P0 验证 | 用现有 `entry.exe --hotupdate-and-benchmark` + `_postprocess.py` |
| P1-A GC 侵入 | `void* flat_data` 零 GC 侵入 — ArrayStorage 在 domain heap，GC 不扫描 |
| P1-B direct_fn 失效 | 统一走 `patched_entry_ptr` |
| 执行顺序 | P0 → P1 → P2 串行推进 |

## blocking_questions
- [x] P0 的改动是否需要新的 benchmark 流程来验证提升？ → 用现有的
- [x] P1-A typed array: `flat_data` 在 GC scan 是否被误扫？ → ArrayStorage 不在 GC 堆，零侵入
- [x] P1-B direct_fn: hotpatch 后如何使 direct_fn 失效？ → 统一走 patched_entry_ptr
- [x] P2 的 InterpreterValue deep copy 消除是否要动数据结构？ → 不动，B1 按 tag 分路径即可

## question_clearance
`cleared`

## 风险评估摘要
- P0 改动量小风险极低
- P1 typed array 需要 IR lowering 配合传递 element type
- P2 pool 分配需注意 domain 生命周期
- P2 deep copy 消除可能引入 use-after-free

## 三优先级权衡结论
性能最优(1) > 方案完美性(2) > HotUpdate支持(3)。P0/P1/P2 所有推荐方案均以性能为首位，同时保持架构清晰。

## 实施计划

### Phase 1: P0 (当前)
1. **P0-A**: Box type_token — 1 line in fast_dispatch.cpp
2. **P0-B**: NoChk opcodes dispatch — 3 new handler + 3 case entries

### Phase 2: P1
3. **P1-B**: 扩大 direct_fn — TryResolveDirectFn + IR lowering 集成
4. **P1-A**: Typed array flat buffer — IRInstruction + ArrayStorage + handlers

### Phase 3: P2
5. **P2-A**: NewObj TLS pool
6. **P2-B**: Scalar field bypass deep copy
7. **P2-C**: Dispatch overhead 条件编译
