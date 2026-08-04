# P5: 循环优化 — 实现计划

> **面向执行 Agent：** 使用 dev:subagent-driven-development 或 dev:executing-plans 执行。步骤使用复选框（`- [ ]`）跟踪。完成当前子任务目录必须归档到 `docs/dev/completed/` 并更新父 roadmap。

**目标：** 在树 IR 框架上构建循环优化能力 — CFG + 支配树 + 循环检测 + LICM + IV 强度削减 + 简单循环展开。

**架构：** 新增 `jit_cfg.h/cpp` 作为 CFG/支配树/循环检测的基础分析设施。LICM/IV/Unrolling 作为独立 pass 插入优化管线。当前 per-BB 树 IR 管线扩展为 "预分析（CFG分析）→ per-BB 优化（含循环感知）→ 线性化" 的两阶段模型。

**架构审核模式：** normal（CFG + 循环是树 IR 框架的扩展，不改变现有架构模式）

**结构告警重点：** jit_cfg.cpp 可能成为大文件，需确保其职责边界清晰（只做 CFG 分析，不做优化）；LICM/IV/Unrolling 各自独立文件。

**设计文档：** `docs/dev/in-progress/20260524-jit-perf-optimization/design-v1-01.md`（P5 阶段摘要）

**问题清零来源：** parent-roadmap（brainstorm 已确认方案 B）

**计划来源：** roadmap-child-auto（轻量级 brainstorm + 用户确认 1+2+3+4）

**预期知识沉淀：** n/a（P5 是树 IR 框架的扩展，不引入新架构模式）

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 → 测试通过 → 归档 completed → 父 roadmap 同步"固定链路。

---

## 已确认设计决策

| 维度 | 方案 | 说明 |
|------|------|------|
| CFG | 完整 CFG + 支配树（方案 B） | Lengauer-Tarjan O(N log N)，~50 行 |
| 循环检测 | Natural loop：back-edge → loop header → 循环包含的所有 BB | 经典支配树方法 |
| LICM | 只外提纯计算（LdLen、常量表达式） | 不涉及内存访问、不涉及 volatile |
| IV 强度削减 | 检测 `vreg = vreg + const_c` 模式，替换 `vreg * const_k` → 累加 add | 仅 int32，< 2^30 |
| 循环展开 | factor=2/4，仅 trip_count 确定且 < 64 | 深度 ≤ 10 |
| 管线位置 | CFG(预分析) → per-BB: Intrinsic → ConstFold → BCE → (LICM/IV/Unroll) → CSE → Linearizer | LICM/IV/Unroll 在 CSE 前 |

## 架构变更：两阶段模型

当前管线：`BB循环 → TreeBuilder → per-BB passes → Linearizer → 输出`

P5 后管线：
```
第一阶段：预分析
  BB范围分析 → CFG构建 → 支配树 → NaturalLoop检测
  ↓
第二阶段：per-BB 优化（循环感知）
  TreeBuilder → IntrinsicMutator → ConstFold → BCE
    → (若是循环体内BB: LICM → IVReduce → Unroll)
    → CSE → Linearizer
```

## 文件修改汇总

| 文件 | 修改类型 | 说明 |
|------|----------|------|
| `src/native/jit/tree/jit_cfg.h` | **新增** | CFG 分析类 + 支配树 + 循环检测 |
| `src/native/jit/tree/jit_cfg.cpp` | **新增** | Lengauer-Tarjan 支配树 + NaturalLoop 识别 |
| `src/native/jit/tree/jit_optimizer.cpp` | 修改 | 两阶段管线：预分析 + 循环感知 per-BB 优化 |
| `src/native/jit/tree/jit_optimizer.h` | 修改 | 可能需要暴露 LoopAnalysis 结构 |
| `src/native/jit/CMakeLists.txt` | 修改 | 注册 jit_cfg.cpp |

---

## 任务 1：CFG + 支配树 + 循环检测（jit_cfg.h/cpp）

**目标：** 构建 CFG 基础设施，从 BB 范围列表生成前驱/后继图，计算支配树，识别 natural loop。

### jit_cfg.h — 数据结构

```cpp
#pragma once
#include "tree/jit_tree_builder.h"  // BBRange
#include <cstdint>
#include <vector>

namespace chaos::il2cpp::jit::tree {

/// A single node in the control flow graph.
/// Maps 1:1 to a basic block from FindBasicBlocks.
struct CfgBlock {
    uint32_t      lo;             // instruction index (same as BBRange)
    uint32_t      hi;
    uint32_t      id;             // block index in the blocks_ array
    uint32_t      loop_depth = 0;
    int32_t       idom = -1;      // immediate dominator (-1 = no idom / entry)
    
    std::vector<uint32_t> preds;  // predecessor block IDs
    std::vector<uint32_t> succs;  // successor block IDs
    bool visited = false;         // for DFS / loop detection
};

/// A natural loop identified by back-edge analysis.
struct NaturalLoop {
    uint32_t              header;           // loop header block ID
    uint32_t              back_edge_from;   // source of back-edge
    std::vector<uint32_t> blocks;           // all blocks in the loop
    uint32_t              depth = 0;        // nesting depth (0 = outer)
};

/// Full CFG analysis result: graph + dominator tree + natural loops.
struct LoopAnalysis {
    std::vector<CfgBlock>  blocks;
    std::vector<NaturalLoop> loops;
    std::vector<uint32_t>  dom_tree_parent;  // [block_count]: parent in dom tree
    bool                   has_loops = false;
    
    bool IsInLoop(uint32_t block_id, uint32_t& out_loop_idx) const noexcept;
    bool IsLoopInvariant(uint32_t block_id, uint32_t vreg,
                          const std::vector<uint32_t>& def_blocks) const noexcept;
};

/// Build CFG from basic block ranges.  Scans terminators of each BB range
/// to determine successors (Br → target, BrTrue/BrFalse → target & next,
/// Ret/Throw → none).
LoopAnalysis BuildCfg(const std::vector<BBRange>& bbs,
                      const interpreter::RegisterInstruction* instrs) noexcept;

}  // namespace chaos::il2cpp::jit::tree
```

### jit_cfg.cpp — 实现细节

**BuildCfg 算法：**
1. 为每个 BBRange 创建一个 CfgBlock，设置 lo/hi/id
2. 扫描每个 BB 最后一条指令（terminator）确定 succs：
   - `Br, BrTrue, BrFalse, Leave` → 按 operand_index / imm.ptr 确定目标 BB
   - `Ret, Throw, Rethrow` → 无后继
   - `Switch` → 扫描 switch table 确定所有目标
   - 无条件流向下一个 BB 的指令（非 terminator 结尾的 BB）
3. 从 succs 反向填充 preds

**Lengauer-Tarjan 支配树算法：**
```
DFS 编号（semi-dominator 计算）→ Link / Eval（路径压缩）→ IDOM 分配
```
- 输入：CFG entry block (id=0)
- 输出：每个 block 的 idom（immediate dominator）
- 数据结构：parent(DFS tree)、semi(semi-dominator)、label、ancestor、best

**NaturalLoop 识别：**
```
对于每个 back-edge (a → b, 其中 a 是 b 在 dom tree 中的后代):
  1. header = b, 从 a 向上遍历 idom 链到 b 的所有 block
  2. 这些 blocks 构成 natural loop
  3. 计算 loop depth：如果 loop header 在另一循环内，depth = parent_depth + 1
```

**IsLoopInvariant：**
判断某个 vreg 的定义是否在循环外——通过比较 vreg 的定义所在 block 和循环 header block 的 idom 关系。

**验证：** 单一 BB 方法返回 `has_loops = false`；含循环的简单多 BB 方法正确识别循环 header/body/back-edge。

---

## 任务 2：LICM（循环不变量外提）

**目标：** 识别循环体内的不变量表达式（LdLen、常量算术），将其外提到循环前的 pre-header。

**设计决策：**
- 只在 BB 树 IR 层面做：在 TreeBuilder 构建树后，识别树中所有不变量子树的根，将其从循环体内 root 列表中移除，记为"hoisted"
- Pre-header 创建：如果 header BB 没有 pre-header（即它有多个前驱），插入一个新的空 pre-header BB
- Hoisted 表达式在 pre-header 的树上被重新构建

**关键实现思路：**
LICM 在 per-BB 树 IR 优化时，需要知道哪些表达式是不变量。
1. TreeBuilder::Build() 为循环体内的 BB 构建树后
2. 扫描 root 列表，对每个 root，递归检查子节点是否为不变量
3. 不变量判定：节点所有 vreg 引用的定义都在循环外（通过 IsLoopInvariant 判断）
4. 如果 root 是不变量：从当前 BB 的 root 列表中移除，挂起到 LoopAnalysis 的 hoist_list
5. 当处理到 pre-header BB 时：将 hoist_list 中的节点插入 pre-header 的 root 列表

**简化实现（P5 范围）：**
- 只外提 `kLdLen` 节点（最常见的循环不变量）
- 只外提 `kLdcI4` / `kLdcI8` 常量节点（恒定值）
- 不变量条件：数组 vreg 的定义在循环外
- 外提位置：将 kLdLen 节点转化为 LdLoc 引用（vreg 引用），在 pre-header 中提前计算

**伪代码：**
```cpp
void LicmHoist(TreeBuildResult& result, const LoopAnalysis& analysis,
               uint32_t bb_id, uint32_t loop_idx, TreeBuilder& builder,
               uint8_t*& arena_pos, uint8_t* arena_end) {
    auto& loop = analysis.loops[loop_idx];
    
    for (uint32_t ri = 0; ri < result.root_count; ) {
        ExprNode* root = result.roots[ri];
        // Check if root is hoistable (kLdLen with loop-invariant array)
        if (root->kind() == kLdLen && IsHoistableLdLen(root, analysis, bb_id, loop)) {
            // Record for pre-header insertion
            // Remove from current BB's roots (or replace with LdLoc reference)
            result.roots[ri] = /* LdLoc referencing the hoisted vreg */;
            // ... (may need to create a new vreg for the hoisted value)
        } else {
            ri++;
        }
    }
}
```

**验证：** 循环体内的 LdLen 被外提到 pre-header；不可外提的节点不受影响。

---

## 任务 3：IV 强度削减

**目标：** 识别简单的归纳变量（`vreg_n = vreg_{n-1} + const_c`），将 `vreg * const_k` 乘法替换为累加加法。

**设计决策：**
- 只在单 BB 循环内检测 IV（跨 BB 的 IV 太复杂，P5 不做）
- 检测模式：BB 内存在 `Add(OldVreg, ConstC) → NewVreg` 的定义链
- 替换模式：`Mul(IvVreg, ConstK)` → 用 Add(Accumulator, ConstK_per_iter) 替代

**简化实现（P5 范围）：**
- 检测：在 TreeBuilder::Build() 后扫描 vreg 定义链，寻找 `vreg = Add(old_vreg, const)` 模式
- 替换：Mutator 在 PostVisit 中将 `Mul(iv_vreg, const_k)` 替换为新的累加序列
- 需要为累加器分配新的 vreg

**伪代码：**
```cpp
void IvStrengthReduce(TreeBuildResult& result, uint32_t bb_id,
                      const LoopAnalysis& analysis, uint32_t max_vreg) {
    // 1. Scan all Add nodes: if child0 is LdLoc(vreg) and child1 is LdcI4(const)
    //    and the vreg is defined by a similar Add → linear IV chain
    // 2. For each IV, scan all Mul nodes: if child0 is LdLoc(iv_vreg)
    //    → replace Mul with a new accumulator add sequence
    // 3. The accumulator is initialized as iv_vreg * const_k before the loop
    //    and incremented by const_k_per_iter each iteration
}
```

**验证：** 乘法被替换为加法，在 benchmark 中体现性能提升；不可削减的场景保持原乘法。

---

## 任务 4：循环展开（Unrolling）

**目标：** 对 trip_count 确定的简单循环展开 factor=2/4。

**设计决策：**
- 仅在循环有确定 trip_count 时展开
- Trip_count 确定条件：循环的结束条件是 `vreg < const_c`，且 vreg 是 IV
- 展开因子：默认 2，仅当 trip_count >= 8 时用 4
- 不需要 remainder 处理（trip_count % factor != 0 时不展开，避免复杂 epilogue）

**简化实现（P5 范围）：**
- 在 Linearizer 之后、阶段输出之前，对线性化的 RegisterInstruction 序列做展开
- 展开：复制循环体内容 N 次，调整 vreg 编号
- 不展开：trip_count < 4、trip_count 不能被 factor 整除、循环有复杂控制流

**伪代码：**
```cpp
bool UnrollLoop(const NaturalLoop& loop, uint32_t factor,
                std::vector<RegisterInstruction>& io_instrs,
                uint32_t bb_starts[], uint32_t bb_ends[]) {
    // 1. Determine trip_count from loop condition
    // 2. If trip_count % factor != 0 → bail
    // 3. Copy loop body instructions factor times
    // 4. Adjust vreg mapping for each copy (rename vregs)
    // 5. Replace loop back-edge with fall-through
    // 6. update bb_starts/bb_ends for new blocks
}
```

**验证：** 展开后的指令序列长度是原来的 factor 倍；正确性通过 fact 验证。

---

## 任务 5：管线集成（jit_optimizer.cpp）

**目标：** 将 CFG 预分析 + LICM + IV + Unrolling 集成到优化管线。

### jit_optimizer.cpp 修改

```cpp
#include "tree/jit_cfg.h"

bool OptimizeWithTreeIR(...) noexcept {
    // ... existing preamble ...
    
    auto bbs = FindBasicBlocks(instrs.data(), n);
    if (bbs.empty()) return false;
    
    // [P5] Phase 1: Build CFG + dominator tree + detect loops
    LoopAnalysis loop_analysis = {};
    if (bbs.size() > 1) {
        loop_analysis = BuildCfg(bbs, instrs.data());
    }
    
    // [P5] Prepare loop hoist buffers
    // (keyed by loop_idx → list of hoisted expression fragments)
    
    bool any_optimized = false;
    
    for (uint32_t bi = 0; bi < bbs.size(); ++bi) {
        const auto& bb = bbs[bi];
        uint32_t bb_len = bb.hi - bb.lo;
        
        // [P5] Check if this BB is a loop header (pre-header insertion point)
        // If so, emit any hoisted expressions from LICM before the loop body
        
        // ... existing TreeBuilder + per-BB passes ...
        
        // [P5] LICM: if this BB is inside a loop, hoist invariants
        uint32_t loop_idx = UINT32_MAX;
        if (loop_analysis.has_loops && loop_analysis.IsInLoop(bi, loop_idx)) {
            LicmHoist(result, loop_analysis, bi, loop_idx, builder, ...);
        }
        
        // [P5] IV strength reduction: if this BB is in a loop
        if (loop_analysis.has_loops && loop_analysis.IsInLoop(bi, loop_idx)) {
            IvStrengthReduce(result, bi, loop_analysis, max_vreg);
        }
        
        // ... CSE → Linearizer ...
    }
    
    // [P5] Post-phase: loop unrolling (on linearized instructions)
    if (loop_analysis.has_loops) {
        // Analyze each loop for unrolling opportunity
        // Apply unrolling to the linearized instruction stream
    }
    
    return any_optimized;
}
```

### 管线排序

```
Phase 1（预分析）:
  CFG构建 → 支配树 → NaturalLoop识别
  ↓
Phase 2（per-BB 循环）:
  对每个 BB:
    TreeBuilder::Build()
    IntrinsicMutator
    ConstFoldMutator
    BoundCheckEliminator
    [如果在循环内] LICM → IV 强度削减
    CSEMutator
    Linearizer
  ↓
Phase 3（post-BB）:
  循环展开（在已线性化的指令序列上操作）
```

---

## 任务 6：CMakeLists.txt

**文件：** `src/native/jit/CMakeLists.txt`

```cmake
add_library(chaos_jit STATIC
    ...
    tree/jit_cfg.cpp              # 新增
    ...
)
```

---

## 任务 7：验证

**验证命令：**
```bash
cd testing/foundation-dll/System.Private.CoreLib/convert-char/native/build-jit
cmake -S . -B . && cmake --build . --config Debug
./Debug/entry.exe --fact all        # 期望 18/18
./Debug/entry.exe --benchmark 1 10000  # 期望无退化
./Debug/entry.exe --fact all --no-opt  # --no-opt 仍正确
```

**检查项：**
1. build 通过 ✅
2. fact 18/18 通过 ✅
3. `--no-opt` 下所有 fact 仍正确 ✅
4. 多 BB 方法中循环检测正确（通过日志验证）
5. LICM 循环体内 LdLen 被外提（profile 或 trace 验证）
6. 无性能退化

### 新增测试

P5 最关键的是需要多 BB 循环场景的测试 subjects。当前 convert-char 全部是单 BB。建议至少添加：

- `SimpleLoopSumSubject` — `for (int i = 0; i < arr.Length; i++) sum += arr[i]`
- `SimpleLoopConstSubject` — `for (int i = 0; i < 10; i++) sum += arr[i]`
- `NestedLoopSubject` — 双层循环（验证 loop depth 检测）

这些 subjects 需要：
1. 在 convert-char 的 codegen 模板中添加对应的 IL 序列
2. 在测试套件中注册

如果 codegen 模板修改成本太高，至少有能触发多 BB 路径的 test 用例（通过 `--test` 或类似机制）。

---

## 任务关系

```
Task 1 (jit_cfg) → Task 2 (LICM)  [LICM 依赖 CFG + 循环检测]
Task 1 (jit_cfg) → Task 3 (IV)     [IV 依赖 CFG + 循环检测]
Task 1 (jit_cfg) → Task 4 (Unroll) [Unrolling 依赖 CFG + 循环检测]
Task 2 + Task 3 + Task 4 → Task 5 (管线集成)
Task 5 → Task 6 (CMakeLists.txt)
Task 5 → Task 7 (验证)
```

Task 2/3/4 可以部分并行（它们对 CFG 的分析是只读的），但按串行实现以降低复杂度。

---

## 注意事项

1. **CFG 构建的 terminator 扫描是关键难点**：需要正确解析每种 branch 类指令的目标 BB。当前 `FindBasicBlocks` 已经识别了 terminator，但目标 BB 的判定需要指令数据（operand_index / imm.ptr / switch table）。

2. **Pre-header 插入的指令索引偏移**：插入新的 BB（pre-header）会改变后续所有指令的索引，需要精确的偏移计算。建议在线性化的 instruction vector 上操作，而不是在索引数组上。

3. **LICM 的复杂度控制**：目前只做 LdLen hoisting，不做更复杂的不变量外提。控制在 100 行以内。

4. **IV 强度削减只做最简单的模式**：`vreg = vreg + 1`（步长为 1 的 IV）+ `vreg * const` 替换。不做 `a * b` 中只有一个是 IV 的部分削减。

5. **循环展开不做 remainder**：trip_count 不能被 factor 整除时，直接跳过不展开，避免产生复杂的 remainder BB。

6. **回归保护**：所有循环 pass 必须保证纯分析不影响正确性——如果检测失败（不确定是否可优化），保持原样输出。`no-opt` 模式不跑任何循环 pass。
