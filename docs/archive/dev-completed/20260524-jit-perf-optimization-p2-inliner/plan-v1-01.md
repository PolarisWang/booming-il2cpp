# P2: 热更感知内联器 — 实现计划

> **面向执行 Agent：** 使用 dev:subagent-driven-development 或 dev:executing-plans 执行。步骤使用复选框（`- [ ]`）跟踪。完成当前子任务目录必须归档到 `docs/dev/completed/` 并更新父 roadmap。

**目标：** 在 P1 树 IR 框架上实现内联展开 + 热更感知失效

**架构：** 树嫁接（tree grafting）方式：BuildTree(caller) → BuildTree(callee) → GraftTrees(vreg_remap) → Optimizer::Run(CSE+ConstFold) → Linearize。热更安全通过 callee version snapshot + dispatch-time stale 检查实现。

**架构审核模式：** critical（修改 JIT 核心管线 data flow 和 slot/hotupdate 基础设施）

**结构告警重点：** jit_inline.h/cpp 新增文件的职责边界（不能与 jit_optimizer 职责重叠）；jit_engine.cpp 管线扩展的侵入度控制

**设计文档：** n/a（父 roadmap design-v1-01.md 已覆盖架构决策）

**问题清零来源：** 轻量级 brainstorm 确认拍板（1C, 2C, 3C, 4C），记录于 p2-inliner/STATUS.md

**Plna来源：** lightweigh-brainsorm -> plan

**预期知识沉淀：** wiki/03-功能模块/06-il2cpp核心架构/05-JIT引擎/JIT内联器设计.md

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 → 测试通过 → 归档 completed → 父 roadmap 同步"固定链路。

---

## 已确认设计决策

| 维度 | 方案 | 说明 |
|------|------|------|
| 内联启发式 | 方案 C — CoreCLR 多因子模型 | callee 节点≤25, loop bonus, depth≤2, return-used bonus |
| SEH 策略 | 方案 C — Phase 1 只内联无 SEH 方法 | 80% 收益，避免 SEH 合并复杂度 |
| vreg 重映射 | 方案 C — 按需映射 + 位图 | 连续区间基线 + bitmap 跟踪实际使用 |
| 入口点 | 方案 C — BuildTree→Graft→Optimizer | 职责最干净，复用现有 pass 框架 |

---

## 任务 1：TokenToPrecodeMap — 全局 token→JitPrecode 映射

**文件：**
- 新增: `src/native/jit/jit_inline.h` — TokenToPrecodeMap 类 + 全局声明
- 新增: `src/native/jit/jit_inline.cpp` — 实现 + `g_token_to_precode` 定义
- 修改: `src/native/jit/jit_precode.h` — JitPrecode 添加 `uint32_t method_token` 字段
- 修改: `src/native/jit/jit_precode.cpp` — RegisterJitEntryMethods 中注册到 map
- 修改: `src/native/jit/CMakeLists.txt` — 注册 jit_inline.cpp

**关键实现：**
- TokenToPrecodeMap: `ankerl::unordered_dense::map<uint32_t, JitPrecode*>` （查找表，key = metadata token）
- 在 `RegisterJitEntryMethods()` 中每个 JitPrecode 创建后调用 `g_token_to_precode.Register(token, precode)`
- 查找：`auto* precode = g_token_to_precode.Lookup(callee_token)` → 获取 RegisterMethod

**验证：** build + 已知 token 查找返回正确 precode

## 任务 2：Linearizer 修复 — kCall/kCallVirt 线性化

**文件：**
- 修改: `src/native/jit/tree/jit_tree_builder.h/cpp` — 添加 `BuildWithCallCache()` overload，正确处理 method_token
- 修改: `src/native/jit/tree/jit_linearizer.cpp` — 添加 kCall/kCallVirt cases

**关键实现：**
- TreeBuilder 的 `BuildWithCallCache()`: 在构建 kCall 节点时从 call_cache（CachedCallInfo[]）读取真实的 method_token / module_id，替代当前 `reinterpret_cast<uintptr_t>(ri.imm.ptr)` 的截断方式
- LinearizeNode() 新增 kCall/kCallVirt 分支: 从 tree node 的 method_token/module_id 字段读取，发射 Call/CallVirt RegisterInstruction，参数 vreg 从 call-site 上下文获取（连续 vreg 协议）

**验证：**
- 非内联 kCall/kCallVirt → Linearizer 发出正确 Call/CallVirt 指令
- 回归: fact 18/18 不变

## 任务 3：InlineHeuristics — 内联决策引擎

**文件：**
- 修改: `src/native/jit/jit_inline.h` — 添加 InlineConfig、InlineDecision、EvaluateInline()
- 修改: `src/native/jit/jit_inline.cpp` — 实现 EvaluateInline()

**关键实现：**
- InlineConfig: max_callee_nodes=25, max_inline_depth=2, loop_bonus=-3, return_used_bonus=5
- EvaluateInline(call_node, depth, return_used, cfg, callee_rm, token, module_id, entry):
  1. precode = g_token_to_precode.Lookup(token) — 不存在 → false
  2. callee_rm.seh_clauses 非空 → false (Phase 1)
  3. cost = callee_instr_count + depth * 3 - loop_bonus
  4. return_used → cost -= return_used_bonus
  5. should_inline = (cost ≤ max_callee_nodes)
  6. Version snapshot: 如果内联，读取 `precode->entry->version`

**验证：** 小 callee(5 instr, 无 SEH) → true; 大 callee(50 instr) → false; 有 SEH → false

## 任务 4：Inliner — 树嫁接核心逻辑

**文件：**
- 修改: `src/native/jit/jit_inline.h` — 添加 Inliner 类（InlineRoots, TryInline）
- 修改: `src/native/jit/jit_inline.cpp` — 实现 Inliner::InlineRoots + TryInline

**关键实现：**
- Inliner 不是树 Mutator（不是 1:1 节点替换），而是操作 BB 的根数组
- TryInline(call_node, call_instr, instr_idx):
  1. 通过 g_token_to_precode 获取 callee RegisterMethod
  2. 创建独立 TreeBuilder 构建 callee 树
  3. **参数重映射**：将 callee 树中的 LdArg(i) 替换为 caller 的 vreg（src1_reg + i），使用一个简单的 ArgReplacer（继承 Mutator，只替换 LdArg 节点）
  4. **vreg 重映射**：callee 的 LdLoc/StLoc vreg 偏移 caller_max_vreg + 1，用 bitmap 跟踪实际使用
  5. **合并**：callee 的 return 值替换原 kCall 节点；callee 的 side-effect roots（StLoc 等）追加到 extra_roots[]
  6. 更新 caller_max_vreg
  7. 检查深度（max=2），防止递归内联

**限制：** Phase 1 只内联 kCall（直接调用），不处理 kCallVirt（虚调用需要 PIC 解析）

**验证：** 合成测试：caller 调用 add(a,b) callee → 内联后树语义等价

## 任务 5：版本快照 + 失效机制

**文件：**
- 修改: `src/native/jit/jit_method.h` — JitMethod 添加 InlinedCallee[] + stale 标志
- 修改: `src/native/jit/jit_inline.h` — 添加 InlineReverseMap 类
- 修改: `src/native/jit/jit_inline.cpp` — 实现 InvalidateCallers()
- 修改: `src/native/jit/jit_precode.cpp` — SlotUpdateCallback 扩展为同时失效内联调用者

**关键实现：**
```cpp
// jit_method.h
struct InlinedCallee {
    uint32_t callee_token;
    uint32_t snapshot_version;  // HotpatchEntryV0.version 在 inline 时
};
// JitMethod 添加:
InlinedCallee* inlined_callees = nullptr;
uint32_t inlined_callee_count = 0;
bool stale = false;
```

- InlineReverseMap: `token → vector<JitMethod*>`，在 Inliner 内联时记录
- 热更新时 SlotUpdateCallback 调用 `g_inline_reverse_map.InvalidateCallers(token, entry)`
- InvalidateCallers: 遍历所有内联了该 token 的 JitMethod，比较 snapshot_version 与 entry->version，不匹配则设置 stale=true
- stale 检查：JitStubDispatchImpl 中 `precode->compiled->stale` → 触发重编译

**验证：** 内联后 callee 被 hotpatch → caller 的 JitMethod.stale = true

## 任务 6：管线集成 — Wire into jit_engine.cpp

**文件：**
- 修改: `src/native/jit/jit_engine.h` — CompileConfig 添加 enable_inlining 等
- 修改: `src/native/jit/tree/jit_optimizer.h` — OptimizeWithTreeIR 添加 inline 参数
- 修改: `src/native/jit/tree/jit_optimizer.cpp` — Build 后插入 Inliner::InlineRoots
- 修改: `src/native/jit/jit_engine.cpp` — Generate() 传递 call_cache + 启动内联

**关键实现：**
```cpp
// OptimizeWithTreeIR pipeline (per BB):
// 1. TreeBuilder::Build(bb)        ← 构建调用者树
// 2. Inliner::InlineRoots          ← 内联展开（新步骤，在 ConstFold 前）
// 3. ConstFoldMutator              ← 常量折叠（合并内联后的常数）
// 4. CSEMutator                    ← CSE（消除内联后的重复子表达式）
// 5. Linearizer::LinearizeRoots    ← 线性化
```

Generate() 修改点（lines 3338-3359）：
```cpp
if (!is_tier0_ && config_.enable_optimizer && rm_.seh_clauses.empty()) {
    tree::OptimizeWithTreeIR(opt_instrs, tree_opt, false, max_vreg,
                              config_.call_cache, config_.call_cache_count,
                              config_.method_token, /*enable_inlining=*/true);
    // 之后：对于非内联调用，线性化器已通过 Task 2 修复
}
```

内联后 slot 计数更新：重新扫描 opt_instrs 中 Call/CallBridge 的数量，更新 slot_count_。

**验证：** 
- 含可内联 callee 的方法：内联后指令流正确
- `--no-opt` 关闭优化后不内联（回归基线）

## 任务 7：Recalc Slot 计数 + JitMethod 生成

**文件：**
- 修改: `src/native/jit/jit_engine.cpp` — 树 IR 优化后重算 slot 数 + 填充 JitMethod 的 inlined_callees

**关键实现：**
- 树 IR 后重算 slot_count_（内联减少了 call 数量）
- JitMethod 构造时从 Inliner 获取 inlined_callees 信息并填充

**验证：** 内联后 slot 表大小正确；JitMethod.inlined_callees 包含正确条目

## 任务 8：验证

**验证命令：**
```bash
cd testing/foundation-dll/System.Private.CoreLib/convert-char/native/build-jit
cmake -S . -B . && cmake --build . --config Debug
./Debug/entry.exe --fact all        # 期望 18/18
./Debug/entry.exe --benchmark 1 10000  # 期望内联生效
```

**检查项：**
1. build 通过 ✅
2. fact 18/18 通过 ✅
3. 空内联管线（disable_inlining）回归基线不变 ✅
4. 简单 getter/setter 内联后指令减少 ✅

---

## 文件修改汇总

| 文件 | 修改类型 | 说明 |
|------|----------|------|
| `src/native/jit/jit_inline.h` | **新增** | TokenToPrecodeMap, InlineConfig, InlineDecision, EvaluateInline, Inliner, InlineReverseMap |
| `src/native/jit/jit_inline.cpp` | **新增** | 全部实现 |
| `src/native/jit/jit_method.h` | 修改 | InlinedCallee[], stale, inlined_callee_count 字段 |
| `src/native/jit/jit_precode.h` | 修改 | JitPrecode 添加 method_token |
| `src/native/jit/jit_precode.cpp` | 修改 | TokenToPrecodeMap 注册 + SlotUpdateCallback 扩展 |
| `src/native/jit/jit_engine.h` | 修改 | CompileConfig 可选 inline 控制 |
| `src/native/jit/jit_engine.cpp` | 修改 | 传递 call_cache + enable_inlining; 重算 slot 计数; JitMethod inlined_callees |
| `src/native/jit/tree/jit_tree_builder.h/cpp` | 修改 | BuildWithCallCache() overload |
| `src/native/jit/tree/jit_linearizer.cpp` | 修改 | kCall/kCallVirt 线性化 |
| `src/native/jit/tree/jit_optimizer.h/cpp` | 修改 | 传递 inline 参数 + Inliner 步骤 |
| `src/native/jit/CMakeLists.txt` | 修改 | 注册 jit_inline.cpp |

---

## 注意事项

1. **method_token 问题**：当前 TreeBuilder 将 `ri.imm.ptr` 作为 method_token（函数指针截断），需要 `BuildWithCallCache()` 从 CachedCallInfo 读取真 token
2. **Phase 1 不处理 kCallVirt**：虚调用需要 PIC 信息，留到后续阶段
3. **vreg 范围保护**：remap 后超过 kGprCount(64) 时跳过内联
4. **Arena 共享**：callee 树节点从 caller 的 arena 分配（每个 inline ≤600 字节 = 25 nodes × 24 bytes）
5. **深度检查**：max_inline_depth=2，防止级联膨胀
