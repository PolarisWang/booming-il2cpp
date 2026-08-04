---
task_type: plan
phase: planning
parent_task_id: 20260524-jit-perf-optimization
source_task_id: p2-inliner
source_relation: roadmap-child
roadmap_or_plan: pending
dispatch_model: sequential
child_execution_mode: auto
entry_skill: lightweight-brainstorm
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
created: 2026-05-24
---

# P2: 热更感知内联器 — STATUS

> 父任务: 20260524-jit-perf-optimization (JIT 性能优化路线)

## Scope

在 P1 树 IR 框架上实现内联展开：识别 inline 候选方法、将 callee 的树 IR 嫁接（graft）到 caller 树中、vreg 重映射、内联启发式决策、callee version snapshot 用于 hotupdate 失效检测。

## 非目标（继承父级）

- 不做跨方法全局 VN/CSE（P2 只做内联展开本身）
- 不涉及 intrinsic 识别（P3 阶段）
- 不修改 codegen 管线或 Scriban 模板
- 不做多 BB 内联（当前树 IR 是单 BB 范围）

## Inputs

- P1 树 IR 框架：jit_tree_node.h, jit_tree_builder.h/cpp, jit_vn.h/cpp, jit_tree_mutator.h/cpp, jit_linearizer.h/cpp, jit_optimizer.h/cpp
- jit_engine.cpp 中的 Compile() 管线入口
- 父设计中的 D3（树 IR）/ D4（函数式 pass）决策
- RegisterMethod / RegisterInstruction 数据结构

## Expected Outputs

- 修改: jit_tree_mutator.h/cpp — 内联嫁接（graft）逻辑
- 修改: jit_optimizer.h/cpp — inline candidate 识别 + 启发式 + 编排
- 修改: jit_engine.h — inlined_method_tokens 字段
- 新增或修改: jit_engine.cpp — inline 触发入口（Compile 中）

## Exit Criteria

1. convert-char 中简单方法（ToChar_Char/ToChar_Byte）正确内联
2. 内联后正确性通过 18/18 fact 测试
3. hotpatch 后内联版本触发失效并重编译
4. 内联深度限制防止栈爆炸
5. SEH 子句合并正确（至少无 SEH 方法的 call sites）

## 父级已拍板设计决策（直接继承）

- **内联方式**：树嫁接（tree grafting），callee 树 IR 复制到 caller 树中，vreg 重映射
- **热更安全**：caller list 记录 inlined callee，callee version bump 时触发 caller 重编译
- **依赖**：P1 树 IR 框架 → inline 展开在树 IR 上进行，线性化后无需额外处理

## 拍板设计决策（轻量级 brainstorm 确认）

| 维度 | 方案 | 理由 |
|------|------|------|
| 内联启发式 | **方案 C — CoreCLR 多因子模型** | 一步到位，适合当前目标 |
| SEH 策略 | **方案 C — 分阶段：先无 SEH → 后有 SEH** | Phase 1 先拿低垂果实，后续低风险扩展 |
| vreg 重映射 | **方案 C — 按需映射 + 位图复用** | 紧凑灵活 |
| 入口点 | **方案 C — BuildTree 后、Optimizer 前** | 职责最干净，内联后 CSE 自然消除重复子表达式 |

### 内联启发式（CoreCLR 多因子模型初版）

在 P2 中实现的 CoreCLR 多因子模型减配版：
- **callee 大小**：树节点数 ≤ 25（硬限制，防止膨胀）
- **调用模式**：call site 在循环体中时提高内联倾向
- **返回值使用**：返回值直接被后续指令使用（非存储后闲置）时提高倾向
- **内联深度**：max depth = 2（防止级联膨胀）
- **callee 引用类型参数**：无泛型特化问题可内联

### SEH 分阶段策略

- **Phase 1**（P2 范围内）：只内联无 SEH 子句的 callee 方法
- **Phase 2**（后续）：对含 SEH 方法启用 deopt-on-exception

### vreg 重映射（按需映射 + 位图复用）

- 分配连续区间作为 callee vreg 的基线偏移
- 用 bitmap 跟踪 callee 实际使用的 vreg
- 释放未用槽位，但保留连续区间简化调试
- 按位图选择性声明实际映射关系

### 入口点

```
BuildTree(caller) → BuildTree(callee) → GraftTrees(vreg_remap) → 
Optimizer::Run(CSE → ConstFold) → Linearize
```

- `GraftTrees()` 作为独立函数，调用 vreg 重映射后拼接树
- 不修改 Optimizer 或 TreeBuilder 职责

## 下一步

进入 exec 阶段。执行 plan-v1-01.md 的 8 个任务：TokenToPrecodeMap → Linearizer 修复 → InlineHeuristics → Inliner 核心 → 版本失效 → 管线集成 → Slot 计数 → 验证。

- SEH 子句合并复杂度高（父 roadmap 已识别，备选路径：先只内联无 SEH 方法，exception 时 deopt）
- 内联后代码膨胀可能导致 compile 时间增加
- 内联启发式参数需要 benchmark 调优

## 下一步

进入轻量级 brainstorm 确认待澄清问题后，编写 plan 并执行。
