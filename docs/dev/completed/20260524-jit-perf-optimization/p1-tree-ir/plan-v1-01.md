# P1: 轻量树 IR 框架 — 实现计划

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development（如果可用子 Agent）或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。本任务带有 `source_relation: roadmap-child`，执行完成时必须先归档当前子任务目录，再同步父任务。

**目标：** 构建单 BB 表达式树 IR 框架，在寄存器分配之后、原生代码生成之前插入 VN/CSE/常量折叠优化阶段。

**架构：** 将 RegisterInstruction 线性序列按基本块边界分割，对每个单 BB 构建 DAG 表达式树，应用值编号消除冗余计算+常量折叠，再线性化回 RegisterInstruction 序列。与现有线性 OptimizeInstructions 共存：树 IR 管线优先，失败时回退线性优化。

**技术栈：** C++17, ankerl::unordered_dense (identity_hash 用于 VN 表), CodeBuffer (现有)

**架构审核模式：** normal（纯 jit 内部新增，不涉及 contracts/codegen/loader 主线）

**结构告警重点：** 5 个新文件的职责边界——tree_builder ≠ vn ≠ mutator ≠ linearizer ≠ optimizer；避免 tree_builder 中混入优化逻辑

**权责图审核主题：** n/a

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: n/a
- capabilityItem: n/a
- ownerSubjectId: n/a
- proofRequired: false
- benchmarkRequired: true
- hotupdateImpact: None
- formalVerificationObjects: []
- requiredGates: []

**设计文档：** `design-v1-01.md`（父任务设计文档）

**问题清零来源：** parent-roadmap（继承父任务清零结果，已确认 blocking_questions: []）

**计划来源：** roadmap-child-auto（P0 已完成，自动推进 P1）

**预期知识沉淀：** n/a（JIT 优化属内部实现细节，不做 wiki 知识沉淀）

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 文件清单

### 新增文件（5 个，11 个源文件）

| 文件 | 职责 | 关键 API |
|------|------|----------|
| `jit_tree_node.h` | 树节点类型定义：ExprNode (基类)、BinaryNode、UnaryNode、LeafNode (常量/变量/字段)、CallNode | `enum NodeKind`, `ExprNode::vn_id()` |
| `jit_tree_builder.h/cpp` | RegisterInstruction[lo..hi] → 表达式树 DAG | `BuildTree(span<RegisterInstruction>, lo, hi) → ExprNode*` |
| `jit_vn.h/cpp` | 值编号表 + CSE 检测 | `VNTable::GetOrCreateVN(op, src1, src2, imm) → uint32_t` |
| `jit_tree_mutator.h/cpp` | 树遍历/替换基类 + CSE Mutator + 常量折叠 Mutator | `Mutator::Visit(node)`, `CSEMutator::Mutate(node)`, `ConstFoldMutator::Mutate(node)` |
| `jit_linearizer.h/cpp` | 优化后树 → RegisterInstruction | `Linearize(root, out_instrs)`, `LinearizeTree(root, collector)` |
| `jit_optimizer.h/cpp` | 优化入口编排 | `Optimizer::Run(bb_instrs, temp_arena) → optimized_instrs` |

### 修改文件

| 文件 | 改动 |
|------|------|
| `src/native/jit/jit_engine.cpp` | Generate() 中插入树 IR 优化阶段（替代部分现有线性优化）；空管线回归 |
| `src/native/jit/CMakeLists.txt` | 注册 5 个新 .cpp 文件 |

---

## 核心设计

### 树节点结构 (jit_tree_node.h)

```
ExprNode (基类)
├── LeafNode       — LdcI4(imm), LdLoc(vreg), LdFld(base, offset), LdArg(idx), LdNull
├── UnaryNode      — Neg, Not, ConvI4, ConvI8, ConvR4, ConvR8, LdLen
├── BinaryNode     — Add, Sub, Mul, Div, Rem, And, Or, Xor, Shl, Shr, Ceq, Clt, Cgt
├── CallNode       — Call(method_token, args[]) — 不内联，只做 CSE 的根
└── StoreNode      — StLoc(vreg, value), StFld(base, offset, value) — 副作用节点
```

每个节点包含：
- `NodeKind kind` — 节点类型枚举
- `uint32_t vn_id` — 值编号（由 VNTable 分配，0 = 未编号）
- `TypeTag type` — 值类型（Int32, Int64, Float32, Float64, ObjectRef, Void）
- 子节点指针（`ExprNode* src1`, `ExprNode* src2` 等）

### 值编号 (jit_vn.h/cpp)

```
VNTable:
  - GetOrCreateVN(opcode, vn1, vn2, imm) → vn_id
  - IsRedundant(vn_id) → bool  // 同一 VN 已被计算且未失效
  - MarkComputed(vn_id)       // 标记该 VN 已计算（用于 CSE 替换）
```

- Key 结构：`(opcode_kind, vn1, vn2, imm64)` → 64-bit hash
- 使用 `ankerl::unordered_dense::map` 配合 `identity_hash`（key 是 uint64_t）
- 单 BB 生命周期：每次 BuildTree 创建新 VNTable，BB 结束后销毁

### 树构建 (jit_tree_builder)

构建算法（单 BB 内）：

1. 从 BB 末尾开始反向扫描：对每个有 dst 的指令，构建表达式节点
2. 对 dst vreg 建立 `vreg → ExprNode*` 映射
3. 对 src vreg：递归查找 `vreg → ExprNode*`（已构建则复用节点 = 隐式 CSE）
4. 遇到以下情况停止反向追踪：call（side-effect 边界）、branch（BB 边界）、store（副作用边界）、vreg 被重新定义

```
Input:  [LdcI4 1→r0] [LdcI4 2→r1] [Add r0+r1→r2] [Call r2]
Tree:   CallNode
          └── BinaryNode(Add)
                ├── LeafNode(LdcI4, 1)
                └── LeafNode(LdcI4, 2)
```

### CSE 实现 (jit_vn + jit_tree_mutator)

CSE 分两步：
1. **构建期（VNTable 被动检测）**：BuildTree 时，对每个表达式节点调用 `VNTable::GetOrCreateVN()`。如果 VN 已存在且已计算，标记为冗余
2. **消除期（CSEMutator）**：后序遍历树，对冗余节点替换为 `Dup(vn_reg)` 引用

### 常量折叠 (jit_tree_mutator)

`ConstFoldMutator` 前序遍历：对 BinaryNode/UnaryNode，如果两个子节点都是 LeafNode(LdcI4)，直接计算替换为 LeafNode。

### 线性化 (jit_linearizer)

DFS 后序遍历树 → RegisterInstruction 序列：
1. 对 BinaryNode：先 linearize src1 → 分配 vreg，再 linearize src2 → 分配 vreg，再 emit Add dst,src1,src2
2. 对 LeafNode(LdcI4)：emit LdcI4 dst, imm
3. 对 LeafNode(LdLoc)：emit LdLoc dst, vreg
4. 对 StoreNode：先 linearize value → vreg，再 emit StLoc/StFld
5. vreg 分配：顺序分配 (r0..rN)，每个节点产出唯一 dst vreg

### Optimizer 编排 (jit_optimizer)

```
Optimizer::Run(instructions, bb_ranges) → optimized:
  for each BB [lo, hi):
    if BB is trivial (≤2 instrs): skip
    tree = TreeBuilder::Build(instrs[lo..hi], vn_table)
    tree = ConstFoldMutator::Mutate(tree)
    tree = CSEMutator::Mutate(tree, vn_table)
    Linearize(tree, out_instrs)
  return out_instrs
```

### Integration in jit_engine.cpp

在 Generate() 中，替换现有 `OptimizeInstructions()` 调用：

```cpp
// ── Optimize instructions (tree IR) ──────────────────────
auto opt_instrs = rm_.instructions;
if (!is_tier0_ && config_.enable_optimizer) {
    if (rm_.seh_clauses.empty()) {
        // 无 SEH：使用树 IR 优化管线（单 BB 优化）
        OptimizeWithTreeIR(opt_instrs);
    } else {
        // 有 SEH：回退到现有线性优化
        std::vector<uint8_t> removed_mask;
        OptimizeInstructions(opt_instrs, removed_mask, true);
    }
}
```

`OptimizeWithTreeIR()` 是新函数，负责：
1. 分析 BB 边界（branch 指令分割点）
2. 对每个 BB 调用 `Optimizer::Run()`
3. 拼接优化后指令序列

---

## 任务分解

### 任务 1: jit_tree_node.h — 树节点类型定义

- 定义 `ExprNode` 基类（`kind`, `vn_id`, `type`）
- 定义 `LeafNode`（LdcI4/imm, LdLoc/vreg, LdFld/base+offset, LdArg/idx, LdNull）
- 定义 `UnaryNode`（Neg, Not, ConvI4, ConvI8, ConvR4, ConvR8, LdLen）
- 定义 `BinaryNode`（Add, Sub, Mul, Div, Rem, And, Or, Xor, Shl, Shr, Ceq, Clt, Cgt）
- 定义 `CallNode`（method_token, args[]）
- 定义 `StoreNode`（StLoc/vreg+value, StFld/base+offset+value）
- 使用 `chaos::il2cpp::jit::tree` 子命名空间
- 验证：编译通过，无 runtime 依赖

### 任务 2: jit_vn.h/cpp — 值编号表

- `VNTable` 类：
  - 内部 `unordered_dense_map<uint64_t, uint32_t>`（hash(key) → vn_id）
  - `GetOrCreateVN(opcode, src1_vn, src2_vn, imm64) → uint32_t vn_id`
  - `IsComputed(vn_id) → bool`
  - `SetComputed(vn_id)`
- Key 编码：`(opcode<<48 | src1_vn<<32 | src2_vn<<16 | imm_low16)` 拼为 uint64_t
- 验证：单元测试——相同表达式返回相同 VN，不同表达式返回不同 VN

### 任务 3: jit_tree_builder.h/cpp — RegisterMethod → 表达式树

- `TreeBuilder` 类：
  - `Build(span<RegisterInstruction>, lo, hi, VNTable&) → ExprNode* root`
  - 内部 `vreg_to_node_[kGPRegisters]` 映射
  - 反向扫描 BB 构建 DAG
  - 对每指令：创建节点 → 设置 src1/src2 为 `vreg_to_node_[src_reg]` → 注册 dst_reg → 编号
- 调用 VNTable::GetOrCreateVN 为每节点分配 VN
- 处理边界：call（新 root）、store（StoreNode → 继续）、branch（返回 root）
- 验证：3 个 fact 方法（无分支）的树构建正确

### 任务 4: jit_tree_mutator.h/cpp — 树遍历器 + CSE + 常量折叠

- `Mutator` 基类：`Visit(node)` 前序遍历，`PostVisit(node)` 后序遍历
- `ConstFoldMutator`：
  - `PostVisit`: 对 BinaryNode 检两个子节点是否都是叶常量 → 替换为 LeafNode
- `CSEMutator`：
  - `Visit`: 对每节点检查 `VNTable::IsComputed(vn_id)` → 若已计算则替换为 `Dup` 叶
  - `PostVisit`: `SetComputed(vn_id)`
- 验证：已知常量表达式正确折叠，重复表达式正确消除

### 任务 5: jit_linearizer.h/cpp — 树 → RegisterInstruction

- `Linearizer` 类：
  - `Linearize(ExprNode* root, vector<RegisterInstruction>& out)` → 分配 vreg 并 emit 指令
  - vreg 分配器：简单递增计数器 `next_vreg_ = kMaxReservedVReg`（使用高位 vreg 避免与现有 vreg 冲突）
  - DFS 后序遍历：先 emit 子节点，再 emit 当前节点
  - `LeafNode(LdcI4)` → `LdcI4 dst, imm`
  - `LeafNode(LdLoc)` → `LdLoc dst, vreg`
  - `BinaryNode` → `emit src1` → `emit src2` → `emit Add dst, src1, src2`
  - `StoreNode` → `emit value` → `emit StLoc vreg, value`
- 验证：线性化后的 IR 与原始 IR 语义等价（空管线回归基线）

### 任务 6: jit_optimizer.h/cpp — 优化编排 + BB 分析

- `BBRange` 结构：`{uint32_t lo, hi}`（指令索引范围）
- `FindBasicBlocks(instructions) → vector<BBRange>`
  - 遍历指令：遇到分支/Call/Ret/Throw 标记 BB 边界
- `Optimizer` 类：
  - `Run(instructions, VNTable&) → vector<RegisterInstruction>`
  - 对每个 BB：
    1. TreeBuilder::Build → tree
    2. ConstFoldMutator::Mutate → folded_tree
    3. CSEMutator::Mutate → cse_tree
    4. Linearizer::Linearize → bb_out
  - 拼接各 BB 输出 + 插入分支指令
- `OptimizeWithTreeIR(instructions)` 顶层函数（可在 jit_engine.cpp 中或 optimizer 中）

### 任务 7: jit_engine.cpp 集成

- 在 `Generate()` 中：
  - 将现有 `OptimizeInstructions(opt_instrs, removed_mask, has_seh)` 替换为条件分发
  - 无 SEH → `OptimizeWithTreeIR(opt_instrs)`
  - 有 SEH → 保留现有线性优化
  - 空管线（enable_optimizer=false）保持原样
- 保留 `removed_mask` 机制兼容性
- 验证：空管线 → 语义等价；常量折叠开启 → 正确折叠

### 任务 8: CMakeLists.txt 更新

- 在 `chaos_jit` 库中添加：
  - `jit_tree_builder.cpp`
  - `jit_vn.cpp`
  - `jit_tree_mutator.cpp`
  - `jit_linearizer.cpp`
  - `jit_optimizer.cpp`
- 验证：build 通过

### 任务 9: 验证与测试

1. **空管线回归**：`enable_optimizer=false` → entry-jit.exe 18/18 fact 通过, 15/18 benchmark 通过（与 P0 基线一致）
2. **常量折叠**：含常量算术的 fact 方法执行正确
3. **CSE**：重复表达式消除后结果正确
4. **有 SEH 回退**：SEH 方法通过现有线性优化，行为不变
5. **性能监控**：优化后 compile 时间不超过原始 2x

---

## 验证命令

```bash
# 构建
python build/check_foundation_dll_pipeline.py --family convert-char --stage jit-build --config RelWithDebInfo

# Fact 测试（18/18 通过）
build/testing/entry-jit/RelWithDebInfo/entry-jit.exe --fact

# Benchmark 测试（15/18 通过，3 个预存崩溃）
build/testing/entry-jit/RelWithDebInfo/entry-jit.exe --benchmark

# 单独验证常量折叠方法
build/testing/entry-jit/RelWithDebInfo/entry-jit.exe --fact --filter ToChar_Byte
```

---

## 执行顺序

```
任务 1 (jit_tree_node.h) → 任务 2 (jit_vn) → 任务 3 (jit_tree_builder)
  → 任务 4 (jit_tree_mutator) → 任务 5 (jit_linearizer) → 任务 6 (jit_optimizer)
  → 任务 7 (jit_engine.cpp 集成) → 任务 8 (CMakeLists.txt) → 任务 9 (验证)
```

依赖关系：
- 任务 2 依赖任务 1
- 任务 3 依赖任务 1, 2
- 任务 4 依赖任务 1, 2
- 任务 5 依赖任务 1
- 任务 6 依赖任务 1, 2, 3, 4, 5
- 任务 7 依赖任务 6
- 任务 8 依赖任务 3, 4, 5, 6（.cpp 文件存在）
- 任务 9 依赖任务 7, 8

---

## 状态维护

- `STATUS.md` 是主记录 —— 每次任务完成后更新 `phase`、`最新摘要`、`下一步`
- `notes/progress-*.md` 只在阻塞、重要决策或验证失败时追加
- 终态：物理移动目录 → `docs/dev/completed/` → 回写父任务 STATUS.md
