# JIT 性能优化路线 — Roadmap

> **面向执行 Agent：** 必须使用 `dev-subagent-driven-development`（如果可用）或 `dev-executing-plans` 执行本 roadmap。子任务使用复选框（`- [ ]`）语法跟踪。每个子任务完成时，必须先将任务目录物理移出 `docs/dev/in-progress/`，再同步更新父 STATUS.md。

**目标：** 将 Chaos JIT 从基础 x64 代码生成器（无内联、无 CSE、无边界消除）提升到 CoreCLR JIT 级别的性能，同时保持完整的 hot-update 安全性。

**架构：** 以轻量树 IR（单 BB 表达式树 + VN + CSE）替代线性 RegisterMethod 作为优化中表示，通过 Call-Site slot 间接化确保 hot-update 安全，按 P0→P6 串行推进。

**架构审核模式：** critical（涉及 JIT 引擎核心、contracts/native/v0 的 HotpatchEntryV0 修改）

**结构告警重点：** jit_engine.cpp 将在 P0-P2 持续修改，需监控其文件大小和职责膨胀；P1 新增 5 个文件后需确保各自职责边界清晰（tree_builder ≠ tree_mutator ≠ optimizer）。

**权责图审核主题：** P0 修改 `contracts/native/v0/codegen_bridge.h`（HotpatchEntryV0）是合约层文件——需确认新增 version 字段的二进制兼容性约束。

**设计文档：** `design-v1-01.md`

**问题清零来源：** brainstorm-approved STATUS.md（blocking_questions: [], question_clearance: cleared, clearance_confirmed_by_user: true）

**计划来源：** brainstorm → roadmap（7 个阶段，串行推进，跨多次会话）

**预期知识沉淀：** `wiki/03-功能模块/06-il2cpp核心架构/05-JIT引擎/JIT优化路线-设计文档.md`

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 → 测试通过 → 归档 completed → 合并&提交"固定链路。

---

## 范围与边界

### 范围
- JIT 引擎优化：内联、VN/CSE、常量折叠、边界检查消除、循环优化、SIMD
- Hot-update 安全体系：Call-Site Slot 间接化、版本失效追踪、内联失效
- MIR / 轻量树 IR 框架
- Intrinsic 识别与展开
- 寄存器分配质量提升

### 非目标
- 不涉及 ARM64 支持
- 不涉及 Linux SEH/DWARF
- 不涉及调试信息/SOS
- 不做完整 GenTree 体系（只做单 BB 表达式树）
- 不做全局值编号（当前只做单 BB VN）
- 不重构 codegen 管线或 Scriban 模板

## 全局优先级约束

| 优先级 | 评估 |
|--------|------|
| P1 性能最优 | 所有方案选择以性能为第一驱动力——树 IR 方案（C）因提供 VN+CSE 能力优于线性 MIR |
| P2 方案完美性 | 架构对标 CoreCLR（显式 phase + GenTree-like），函数式编排预留 PassManager 迁移路径 |
| P3 热更适配 | Slot 间接化 + ReverseSlotMap + 内联失效构成完整 hot-update safety net |

所有阶段严格执行 P1>P2>P3 裁决。

## 阶段列表

| 阶段 | 名称 | 内容 | 估算 |
|------|------|------|------|
| P0 | Call-Site Slot 间接化 | HotpatchEntryV0 加 version，call `[rip+off]` 发射，ReverseSlotMap，内联失效框架 | 3-4w |
| P1 | 轻量树 IR 框架 | Tree builder + VN + CSE + Mutator + Linearizer + Optimizer 入口 | 4-5w |
| P2 | 热更感知内联器 | 树嫁接式内联展开 + 启发式 + SEH 合并 + callee version snapshot | 3-4w |
| P3 | Intrinsic 识别 | Math.* / Array.Length / GC write barrier 等 inline 替换 | 2-3w |
| P4 | 边界检查消除 | LdLen hoisting + bounds check elimination + NULL check coalescing | 2-3w |
| P5 | 循环优化 | IV hoisting / unrolling / LICM | 3-4w |
| P6 | SIMD / HW intrinsics | Vector128/256 生成 + POPCNT/LZCNT 等 SSE/AVX 指令 | 4-6w |

## 每阶段完成定义

### P0 — Call-Site Slot 间接化

| 字段 | 内容 |
|------|------|
| **goal** | JIT 生成的 call 指令通过 slot 表间接寻址，hotpatch 时只需更新 slot 指针；内联失效框架可检测版本变化并触发重编译 |
| **exit_criteria** | (1) entry-jit.exe 全 18 方法 fact 通过 (2) slot 表 RX 或独立分配，运行时可见 (3) ReverseSlotMap 在 hotpatch 测试中正确更新 slot (4) 版本号 bump 后内联调用者正确失效重编译 |
| **deliverables** | 修改: codegen_bridge.h, jit_engine.h/cpp, jit_precode.cpp, hotpatch_table.cpp, jit_registration.h. 新增: slot_map.h |
| **dependencies** | 无 |
| **resolved_decisions** | call `[rip+off]` slot 间接化（方案 2） |
| **watch_items** | slot 表的内存保护（RX 页面 vs 独立可写页面）；ReverseSlotMap 的线程安全性 |

### P1 — 轻量树 IR 框架

| 字段 | 内容 |
|------|------|
| **goal** | 在 AllocateRegisters 之后、NativeCodeGenerator 之前插入树 IR 优化阶段，支持 VN/CSE/常量折叠 |
| **exit_criteria** | (1) 空优化管线（不做任何变换）通过所有测试 (2) 常量折叠：纯算术折叠正确 (3) CSE：重复表达式消除正确 (4) LdFld 合并：连续 LdFld 合并正确 (5) 线性化后 RegisterMethod 语义等价 |
| **deliverables** | 新增: jit_tree_builder.h/cpp, jit_vn.h/cpp, jit_tree_mutator.h/cpp, jit_linearizer.h/cpp, jit_optimizer.h/cpp. 修改: jit_engine.cpp, CMakeLists.txt |
| **dependencies** | P0 |
| **resolved_decisions** | 单 BB 表达式树（方案 C），函数式 pass 编排（方案 B） |
| **watch_items** | 树构建和线性化可能导致额外开销——需监控 compile 时间增加；VN 表哈希冲突 |

### P2 — 热更感知内联器

| 字段 | 内容 |
|------|------|
| **goal** | 在树 IR 上实现内联展开，callee 版本变化时触发重编译 |
| **exit_criteria** | (1) convert-char 中简单方法（ToChar_Char/ToChar_Byte）正确内联 (2) 内联后正确性通过 fact (3) hotpatch 后内联版本触发失效并重编译 (4) 有内联深度限制防止栈爆炸 (5) SEH 子句合并正确 |
| **deliverables** | 修改: jit_tree_mutator.h/cpp, jit_optimizer.h/cpp, jit_engine.h (inlined_method_tokens 使用) |
| **dependencies** | P0, P1 |
| **resolved_decisions** | 树嫁接方式展开 vreg 重映射；caller list 记录 inlined callee |
| **watch_items** | SEH 子句坐标调整（内联展开后指令索引偏移）；多级内联的复杂度 |

### P3 — Intrinsic 识别

| 字段 | 内容 |
|------|------|
| **goal** | 识别常见 BCL 方法模式并替换为 inline x64 指令序列 |
| **exit_criteria** | (1) Math.Abs → `and rax, mask` (2) Math.Min/Max → `cmp; cmov` (3) Array.Length → inline field load (4) GC write barrier 在 StFld 中内联 (5) 所有 intrinsic 通过 convert-char fact |
| **deliverables** | 新增或修改: jit_intrinsics.h/cpp（或合并入 jit_engine.cpp） |
| **dependencies** | P1, P2 |
| **resolved_decisions** | Pattern-match based on subjectId + 树节点匹配 |
| **watch_items** | CoreCLR 的 intrinsic 列表很庞大，只做当前 benchmark 热点的 subset |

### P4 — 边界检查消除 + NULL 消除

| 字段 | 内容 |
|------|------|
| **goal** | 消除数组/字符串访问的冗余边界检查 |
| **exit_criteria** | (1) `for (i in arr) { use arr[i] }` 模式中边界检查提升到循环外 (2) 常量索引的边界检查在编译期消除 (3) NULL check 在非空路径消除 (4) fact 全部通过 |
| **deliverables** | 修改: jit_optimizer.h/cpp, jit_engine.cpp（发射边界检查的路径） |
| **dependencies** | P1 |
| **resolved_decisions** | 在树 IR 阶段分析，在代码发射阶段消除 |
| **watch_items** | 多线程场景下数组长度变化的正确性（边界消除基于编译时快照） |

### P5 — 循环优化

| 字段 | 内容 |
|------|------|
| **goal** | 循环不变量外提 + 归纳变量优化 |
| **exit_criteria** | (1) LdLen 等不变量外提出循环 (2) IV 强度削减（mul→add） (3) 简单循环 unrolling (factor=2/4) (4) fact 全部通过 |
| **deliverables** | 修改: jit_optimizer.h/cpp，可能涉及 loop detection |
| **dependencies** | P1, P4 |
| **resolved_decisions** | 循环检测基于 BB CFG 中的 back-edge |
| **watch_items** | 循环 unrolling 后的代码膨胀 |

### P6 — SIMD / HW intrinsics

| 字段 | 内容 |
|------|------|
| **goal** | 支持 Vector128/256 生成 + 硬件 intrinsic 指令 |
| **exit_criteria** | (1) 向量加/乘运算生成 SSE/AVX 指令 (2) POPCNT/LZCNT 等识别 (3) 基本 Vector\<T\> 操作通过 fact |
| **deliverables** | 修改: x64_encoder.h（如有缺失的 SSE/AVX 编码）, jit_engine.cpp, jit_intrinsics.h/cpp |
| **dependencies** | P2, P3, P4 |
| **resolved_decisions** | P6 是性能优化的最终目标——只有前面的基础设施就绪后才可进行 |
| **watch_items** | SSE/AVX 状态切换开销；Vector\<T\> 的对齐要求 |

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|------------|----------|--------------|---------------|----------------|----------------|-------------------|
| p0-call-site-slot | P0 | ready | main | Call-Site Slot 间接化 + Version + ReverseSlotMap | — | batch-1 | native proof + hotupdate proof | codegen_bridge.h, jit_engine.h/cpp, jit_precode.cpp, hotpatch_table.cpp, jit_registration.h, slot_map.h | entry-jit.exe fact 通过, ReverseSlotMap 正确更新 slot | contracts/native/v0/, src/native/jit/, src/native/runtime-core/ | 3-4w |
| p1-tree-ir | P1 | planned | main | 轻量树 IR 框架 | p0-call-site-slot | batch-1 | native proof | jit_tree_builder.h/cpp, jit_vn.h/cpp, jit_tree_mutator.h/cpp, jit_linearizer.h/cpp, jit_optimizer.h/cpp, jit_engine.cpp | 常量折叠 + CSE 通过, 空管线通过 | src/native/jit/ | 4-5w |
| p2-inliner | P2 | planned | main | 热更感知内联器 | p1-tree-ir | batch-1 | native proof + hotupdate proof | jit_tree_mutator.h/cpp, jit_optimizer.h/cpp, jit_engine.h | convert-char 简单方法内联, hotpatch 触发失效重编译 | src/native/jit/ | 3-4w |
| p3-intrinsic | P3 | planned | main | Intrinsic 识别与展开 | p2-inliner | batch-1 | native proof + benchmark | jit_intrinsics.h/cpp (new) | Math.Abs/Min/Max, Array.Length, GC barrier 识别 | src/native/jit/ | 2-3w |
| p4-bounds-check | P4 | planned | main | 边界检查消除 + NULL 消除 | p1-tree-ir | batch-1 | native proof + benchmark | jit_optimizer.h/cpp, jit_engine.cpp | 循环内边界检查提升, 常量索引编译期消除 | src/native/jit/ | 2-3w |
| p5-loop-opt | P5 | planned | main | 循环优化 | p4-bounds-check | batch-1 | native proof + benchmark | jit_optimizer.h/cpp | LdLen hoisting, IV 削减, unrolling | src/native/jit/ | 3-4w |
| p6-simd | P6 | planned | main | SIMD + HW intrinsics | p3-intrinsic + p4-bounds-check | batch-1 | native proof + benchmark | x64_encoder.h, jit_engine.cpp, jit_intrinsics.h/cpp | SSE/AVX 向量操作通过 | src/native/jit/ | 4-6w |

## 依赖图

```
p0-call-site-slot
  └── p1-tree-ir
        ├── p2-inliner
        │     └── p3-intrinsic
        ├── p4-bounds-check
        │     └── p5-loop-opt
        └── p6-simd  ← 等待 p3 + p4
```

注：P4 边界消除只依赖 P1（树 IR 框架），不依赖内联器。P6 SIMD 需要 P2 内联器（展开后识别模式）+ P3 Intrinsic（展开为 SSE/AVX）+ P4 边界消除（向量化前的循环分析），所以放在最后。

## 风险

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|----------|
| P0 slot 表 RX 内存布局问题 | 中 | 高 | slot 表独立分配为 RW 页面而非接在 RX code 后 |
| P1 树→线性化语义不一致 | 低 | 高 | 空管线（不做变换）作为回归基线 |
| P2 SEH 子句合并错误 | 中 | 高 | 先只内联无 SEH 的方法，再逐步支持有 catch/finally 的方法 |
| P3 Intrinsic 覆盖率不足 | 低 | 中 | 只覆盖 benchmark 热点，非目标 coverage |
| P4 边界消除多线程正确性 | 中 | 中 | 只在编译时快照分析，不做运行时假设 |
| P5 循环 unrolling 代码膨胀 | 低 | 中 | 设置 max_unroll_factor |
| P6 SIMD 对齐要求导致 crash | 中 | 高 | 先用对齐版本的 movdqa，不对齐则退化为标量 |

## 备选路径

如果 P1 树 IR 框架发现线性化后的性能回归（慢于纯线性），回退到 **方案 A 回退路径**：
- 只做函数式 inline（二次 vreg 映射，不做表达式树）
- VN/CSE 通过线性扫描实现（简化版，只在单 basic block 内）

如果 P2 内联器发现 SEH 合并过于复杂，分两步：
- 阶段 2a：只内联无 SEH 方法的 call sites（覆盖大部分简单 getter/setter）
- 阶段 2b：支持有 SEH 方法的 inlining，使用 `deopt on exception` 策略（内联后不合并 SEH，exception 时直接 deopt 到 interpreter）

## 当前建议推进顺序

1. **p0-call-site-slot** — 所有后续阶段的基础
2. **p1-tree-ir** — 优化框架
3. **p2-inliner** — 性能收益最大
4. **p4-bounds-check** — 可与 p3 并行（只依赖 p1）
5. **p3-intrinsic** — 依赖内联后的上下文
6. **p5-loop-opt** — 依赖边界消除
7. **p6-simd** — 依赖内联 + intrinsic + 边界消除

## 子任务执行策略

```yaml
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: p0-call-site-slot
```

每个子任务进入前做最小 `preflight self-review`：
- 确认前置依赖已满足
- 确认冲突文件无需当前独占访问
- 确认没有阻塞性问题
- `pass` → 自动继续；`warn` → 记录到 STATUS.md 后继续；`stop` → 停止并等待用户确认
