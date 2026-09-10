# Codegen 工业化收官 — Roadmap v1.01

> 父任务: docs/dev/in-progress/20260523-codegen-industrialization/
> 上游设计: brainstorm-v1-01.md（已确认, blocking_questions = []）
> 调度模式: hybrid（阶段内三线程并行，阶段间串行）

---

## 1. 目标

将所有 codegen 差距项（G1-G12、T1-T10、V1-V6、D1-D3 + 解释器工业化 4 项 + GC sequel 1 项 = 39 项）全部填平，达到 codegen 子系统工业化就绪状态。

## 2. 范围边界

- **纳入**: 托管 AOT codegen（含 Scriban 模板、NativeAotLoweringPlanner、NativeAotEmitter）+ 原生 T4 codegen（code_generator.cpp, t4_seh_handler, deopt）+ 验证管线（foundation-dll 13 阶段）+ codegen 文档
- **前置依赖纳入**: I-R5（FastFrame 容量）、I-R1（SEH in FastExecute）、I-R2a（tier-up 激活）、I-R2b（Backedge OSR）、A3（verification pipeline 打通）
- **不纳入**: 解释器非 codegen 相关工业化（I-R3/I-R4/I-R8/I-R9/I-R10/I-R12、EventPipe、调试器）、GC 非 codegen 相关工业化（GC 算法优化、GC 压力测试）

## 3. 非目标

- 不重新设计 IL2CPP 整体架构
- 不替换现有 Scriban 模板引擎
- 不引入新的中间语言（IR）
- 不改变 codegen 与运行时的 ABI 合约

## 4. 阶段列表

### Phase 1 — 基础设施 + 核心能力（预估 8-10 周）

**goal**: 补齐解释器基础设施缺失（FastFrame、SEH），打通异常 AOT 生成通路；T4 具备精确 GC 和 tier-up 激活；CI 有基础回归门禁。

**线程任务**:
| 线程 | 人力 | 工作项 | 预估 |
|------|------|--------|------|
| A | FT | A-P1-2: G1 异常 lowering（6 flat fallback 修复，~98% 完成） | 1-2 周 |
| B | HT | B-P1-2: T1 精确 GC（接入活跃度分析 + per-safepoint 过滤） | 2-3 周 |
| C | HT | V4 CI/CD 回归 + V3 快照基线 + D1/D2 文档 | 4-6 周 |

**exit_criteria**:
- flatFallbackCount = 0（修复全部 6 个 EH 相关 flat fallback）
- T4 精确 GC slot map（接入活跃度 + per-safepoint 过滤）
- CI 有 codegen 回归检测门禁
- 快照测试基线覆盖 P0+P1

**deliverables**: G1 flat fallback 修复 PR、T1 精确 GC 增强 PR、CI 配置、快照基线、故障诊断指南

**dependencies**: 无外部依赖（A0/A1/B0/T6 已在解释器工业化中完成）

**resolved_decisions**:
- G1 仅剩 6 个 flat fallback（3 runtime-self-test + 2 snapshot-prover + 1 error-info-basic），由非常规 EH region 结构导致
- T1 已有基础设施（GcSlotMapV0、GcScanPreciseFrame），缺失活跃度过滤和类型推断精度
- A0/B0/T6 已由解释器工业化 I-R5/I-R2a/I-R11 完成

**watch_items**:
- 6 个 flat fallback 是否覆盖了所有已知 EH 模式 → 修复后验证 flatFallbackCount = 0
- T1 图着色分配器中的活跃度分析接口是否可直接复用 → 替代从零实现

### Phase 2 — 生产覆盖 + 性能（预估 8-10 周）

**goal**: AOT 覆盖全 ABI + 虚/接口分发 + 100% 结构化恢复；T4 具备 OSR + 多态 PIC + 内联分配；foundation-dll 50% family 全验证通过。

**线程任务**:
| 线程 | 人力 | 工作项 | 预估 |
|------|------|--------|------|
| A | FT | G3 ABI 扩展 → G2 虚/接口分发 → G4 flat fallback → G5 元数据闭包 | 6-8 周 |
| B | HT | T2 OSR deopt → T4 PIC 多态 → T3 TLAB 内联分配 | 6-8 周 |
| C | HT | C0(A3)管线打通 → V1 前 50 family → V2 覆盖率门禁 | 6-8 周 |

**exit_criteria**:
- 含虚方法/接口调用的方法可通过 AOT 生成
- T4 具备 OSR 栈上替换 + megamorphic PIC
- 50 family 全 13 阶段验证通过
- 覆盖率门禁设立

**deliverables**: G2/G3/G4/G5 实现 PR、T2/T4/T3 实现 PR、V1-50 验证报告

**dependencies**: C0/V1 依赖 Thread A Phase 1 稳定（G1 合入后）

**resolved_decisions**: 无新增

**watch_items**:
- T2 OSR 实现是否范围膨胀 → 严格限定 backedge 单一场景
- V1 验证失败分类比例（codegen vs runtime vs test framework）

### Phase 3 — 收官硬化（预估 8-10 周）

**goal**: 泛型 AOT + 无 goto 生成 + D3-C 插件化；T4 PGO + 图着色 + 调试信息；96 family 全验证 + 语义等价验证。

**线程任务**:
| 线程 | 人力 | 工作项 | 预估 |
|------|------|--------|------|
| A | FT | G6 泛型/共享 → G7 goto 消除 → G8 D3-C Phase 1 → G9-G12 | 8-10 周 |
| B | HT | T5 PGO 集成 → T7 图着色 → T8-T10 | 6-8 周 |
| C | HT | V1 剩余 46 family → V5/V6 正确性验证 → D3 性能基线 | 8-10 周 |

**exit_criteria**:
- 覆盖异常/分发/ABI/泛型四大场景
- T4 103/103 opcode + PGO + 精确 GC + OSR + 图着色
- 96 family 全部通过 13 阶段验证
- 无 goto 发射、D3-C Phase 1 完成、validator 全绿
- codegen API 文档 + 故障诊断指南 + 性能基线齐备

**deliverables**: G6/G7/G8/G9-G12 实现 PR、T5/T7/T8-T10 实现 PR、V1-96 验证报告、D3 基线报告

**dependencies**: G7 依赖 Phase 2 的 G5 先完成（元数据闭包后 goto 消除更安全）

**resolved_decisions**: 无新增

**watch_items**:
- G6 泛型/共享 lowering 的复杂度（3-4 周估时是否充分）
- V1 剩余 46 family 中是否出现新模式导致 codegen 改动

---

## 5. 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|------------|----------|--------------|--------------|----------------|-----------------|
| A-P1-1 | P1 | planned | FT | A0 FastFrame 容量扩展 + A1 SEH in FastExecute | 无 | P1A | 解释器 FastFrame 容量验证 + SEH 异常传播 | src/native/interpreter/fast_frame_pool.h, fast_dispatch.cpp | src/native/interpreter/ | 3-4 周 |
| A-P1-2 | P1 | planned | FT (after A-P1-1) | G1 异常 lowering（catch/finally/filter AOT 生成） | A-P1-1 | P1A | AOT 生成含 try-catch-finally 的 C++，通过 fact 验证 | NativeAotLoweringPlanner.ExceptionEmission.cs, Scriban 模板 | src/managed/Chaos.IL2CPP.Generator/ | 3-4 周 |
| B-P1-1 | P1 | planned | HT | B0 tier-up 激活 + T6 T4 失败分类保护 | 无 | P1B | 调用站点层级升级 T1→T4 通路、失败分类+非永久跳过 | ir_reg_alloc.cpp, code_generator.cpp | src/native/codegen/, src/native/interpreter/ | 2 周 |
| B-P1-2 | P1 | planned | HT (after B-P1-1) | T1 精确 GC Slot Map | B-P1-1 | P1B | 活跃度分析→精确 slot mask，GC 点生成精确映射 | code_generator.cpp, deopt_runtime.cpp | src/native/codegen/ | 2-3 周 |
| C-P1-1 | P1 | planned | HT | V4 CI/CD PR 级 codegen 回归门禁 | 无 | P1C | CI 集成 codegen 回归测试、快照测试自动触发 | .github/workflows 或 CI 配置 | .github/ | 2-3 周 |
| C-P1-2 | P1 | planned | HT | V3 快照测试基线扩增（P0+P1） | 无 | P1C | 新增快照测试 fixture 覆盖 P0+P1 模式 | test fixtures + baselines | testing/snapshots/ | 2-3 周 |
| C-P1-3 | P1 | planned | HT | D1 Codegen API 文档 + D2 故障诊断指南 | 无 | P1C | API 文档覆盖 NativeAotLoweringPlanner 公共方法、故障排查流程 | wiki/ 文档 | wiki/ | 2 周 |
| A-P2-1 | P2 | planned | FT | G3 ABI 载体扩展 + G4 flat fallback 清零 | A-P1-2, A-P1-1 | P2A | 非 Int32 参数/返回值 ABI、runtime-self-test 100% 结构化 | NativeAotLoweringPlanner.MethodEmission.cs, Scriban 模板 | src/managed/Chaos.IL2CPP.Generator/ | 3-4 周 |
| A-P2-2 | P2 | planned | FT | G2 虚/接口分发 lowering | A-P2-1 | P2A | callvirt + callinterface + 桥接 thunk AOT 生成 | NativeAotLoweringPlanner 虚方法相关, Scriban | src/managed/Chaos.IL2CPP.Generator/ | 3-4 周 |
| A-P2-3 | P2 | planned | FT | G5 元数据闭包统一 | A-P2-2 | P2A | 令牌级元数据闭包、反射/运行时元数据统一消费 | MetadataSupport 相关 | src/managed/Chaos.IL2CPP.Generator/ | 2-3 周 |
| B-P2-1 | P2 | planned | HT | T2 OSR deopt（含 I-R2b Backedge OSR） | B-P1-2 | P2B | 真正 OSR 栈上替换，Backedge 触发 | deopt_runtime.cpp, code_generator.cpp | src/native/codegen/ | 3-4 周 |
| B-P2-2 | P2 | planned | HT | T4 PIC 多态扩展 + T3 TLAB 内联分配 | B-P2-1 | P2B | megamorphic vtable 分发、NewObj/Box 内联 bump | codegen_helpers.cpp, code_generator.cpp | src/native/codegen/ | 3-4 周 |
| C-P2-1 | P2 | planned | HT | C0 (A3) codegen verification pipeline 打通 | A-P1-2 稳定 | P2C | 验证管线全 13 阶段可运行 | pipeline_adapter.py, verification 配置 | testing/foundation-dll/ | 1-2 周 |
| C-P2-2 | P2 | planned | HT | V1 前 50 family 全 13 阶段验证 | C-P2-1 | P2C | 50 family 每 family 基准确认通过 | unified-verification-report.json | testing/foundation-dll/ | 4-6 周 |
| C-P2-3 | P2 | planned | HT | V2 代码覆盖率门禁 | C-P2-2 部分 | P2C | coverlet 门禁 + 原生代码覆盖率基线 | 覆盖率配置 | testing/ | 2-3 周 |
| A-P3-1 | P3 | planned | FT | G6 泛型/共享 lowering | A-P2-3 | P3A | 泛型上下文载体、泛型实例化 codegen 闭包 | NativeAotLoweringPlanner.GenericSharing.cs, GenericRegistration.cs | src/managed/Chaos.IL2CPP.Generator/ | 3-4 周 |
| A-P3-2 | P3 | planned | FT | G7 goto 消除 + G8 D3-C Phase 1 | A-P3-1 | P3A | 无 chaos_ip_* goto 发射、Family Registry Generator | StructuredIR.cs, MethodEmission.cs, 代码生成器 | src/managed/Chaos.IL2CPP.Generator/ | 3-4 周 |
| A-P3-3 | P3 | planned | FT | G9-G12 剩余差距 | A-P3-2 | P3A | 桥接/导入 thunk、不可约 CFG pc-dispatch、多 assembly、D3-C Phase 2-4 | 各相关模块 | src/managed/Chaos.IL2CPP.Generator/ | 4-6 周 |
| B-P3-1 | P3 | planned | HT | T5 PGO 集成 | B-P2-2 | P3B | Profile-guided 调用站点 tier-up | code_generator.cpp, 调用计数基础设施 | src/native/ | 2 周 |
| B-P3-2 | P3 | planned | HT | T7 图着色寄存器分配集成 | B-P3-1 | P3B | 图着色分配器接入 codegen 流水线 | reg_alloc_graph_coloring.h | src/native/codegen/ | 2-3 周 |
| B-P3-3 | P3 | planned | HT | T8-T10 调试信息/跨模块 hotpatch/码率优化 | B-P3-2 | P3B | DWARF/CodeView、跨模块 HotpatchNameRegistry、码率基线 | code_generator.cpp, t4_seh_handler.cpp | src/native/codegen/ | 4-6 周 |
| C-P3-1 | P3 | planned | HT | V1 剩余 46 family 全验证 | C-P2-2 | P3C | 96 family 全部通过 13 阶段验证 | unified-verification-report.json | testing/foundation-dll/ | 4-6 周 |
| C-P3-2 | P3 | planned | HT | V5 语义等价性验证 + V6 内存/GC 正确性验证 | C-P3-1 部分 | P3C | IL→C++ 语义等价框架、GC 安全独立验证 | 验证框架代码 | testing/ | 6-8 周 |
| C-P3-3 | P3 | planned | HT | D3 性能基线 | 无严格依赖 | P3C | 生成 C++ 编译时间/码体积量化基线 | 性能基线报告 | wiki/ | 2 周 |

---

## 6. 依赖总图

```
P1 ──────────────────────────────┬──────────────────────────────┐
                                 │                              │
A-P1-1 → A-P1-2                B-P1-1 → B-P1-2             C-P1-1, C-P1-2, C-P1-3
  (FastFrame → SEH → G1 EH)      (tier-up → GC Slot Map)       (CI → Snapshot → Docs)
         │                              │
         ▼                              ▼
P2 ──────┼──────────────────────────────┼──────────────────────────┐
         │                              │                          │
A-P2-1 → A-P2-2 → A-P2-3             B-P2-1 → B-P2-2           C-P2-1 → C-P2-2 → C-P2-3
  (ABI → Dispatch → Metadata)         (OSR → PIC+TLAB)          (Pipeline → 50 fam → Gate)
         │                              │                          │
         ▼                              ▼                          ▼
P3 ──────┼──────────────────────────────┼──────────────────────────┘
         │                              │
A-P3-1 → A-P3-2 → A-P3-3             B-P3-1 → B-P3-2 → B-P3-3
  (Generics → Goto/D3C → Rest)        (PGO → RegAlloc → Debug)
                                      C-P3-1 → C-P3-2, C-P3-3
                                      (46 fam → Semantic/Mem)
```

**跨阶段关键依赖**:
- A-P1-2（G1 异常）稳定的 → C-P2-1（验证管线打通）
- A-P2-3（G5 元数据闭包）→ A-P3-2（G7 goto 消除基于元数据闭包）
- B-P1-2（T1 精确 GC）→ B-P2-1（T2 OSR 依赖 GC 信息）
- C-P2-2（V1 前 50）→ C-P3-1（V1 剩余 46）
- C-P2-2（V1 前 50）→ C-P3-2（V5/V6 依赖验证基础设施稳定）

---

## 7. 风险

已在 brainstorm-v1-01.md 中完整记录。此处仅补充 roadmap 特有的调度风险：

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| **Thread B（HT）被打断**导致落后 Thread A 超过 1 个 phase | 高 | 中 | B 的前期任务（B-P1-1/B-P1-2）估时保守，buffer 充分 |
| **Thread C 提前完成**无任务可做 | 中 | 低 | C-P1-1/2/3 后立即接入 C-P2-1 不等待 |
| **P1 某个子任务延期** -> P2 整体阻塞 | 中 | 高 | C 的 P2 初期可做 C-P2-1（独立），B 的 P2 不依赖 A |
| **协作冲突**（A 和 C 同时修改同一个 verification 文件） | 低 | 中 | 通过 conflict_scope 字段声明文件范围，P2+ 注意 review |

---

## 8. 备选路径

| 场景 | 备选 | 触发条件 |
|------|------|----------|
| G1 异常 lowering 超 5 周 | Phase 1 只做 G1 TryCatch 单 shape，finally/filter 移到 Phase 2 | A-P1-2 进度 < 50% 且已用 3 周 |
| T1 精确 GC 超 4 周 | 先合并保守扫描 + 简单活跃度分析，精确全量 GC 移 Phase 2 | B-P1-2 进度 < 60% 且已用 2 周 |
| V1 50 family 验证发现大量阻塞性 codegen bug | 暂停 V1 扩覆盖，集中修 codegen bug 后再继续 | 50 family 中 > 20% 因 codegen bug 失败 |
| 人力变化（HT 被抽走） | B 和 C 串行执行，优先 B（T4 是性能关键） | HT 实际投入 < 50% |

---

## 9. 当前建议推进顺序

**Phase 1 三线程并行启动:**

1. **立即启动（所有线程）**:
   - `A-P1-1`: FT 做 A0 FastFrame + A1 SEH
   - `B-P1-1`: HT 做 B0 tier-up 激活 + T6 失败分类
   - `C-P1-1, C-P1-2, C-P1-3`: HT 做 CI/快照/文档（可并行或顺序推进）

2. **Phase 1 第二阶段**（6-8 周后）:
   - `A-P1-2`: FT 做 G1 异常 lowering（依赖 A-P1-1 完成）
   - `B-P1-2`: HT 做 T1 精确 GC（依赖 B-P1-1 完成）
   - 注意：当 B-P1-1 和 C-P1-1/2/3 都完成后，HT 可集中做 B-P1-2

3. **Phase 1 退出评估** → 进入 Phase 2

---

## 10. 子任务执行策略

```yaml
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: hybrid
recommended_next_child: A-P1-1
```

### 入口 skill 策略

| 子任务类型 | entry_skill | 理由 |
|-----------|-------------|------|
| 架构/设计密集（G1, G2, G3, G6, G7, T5） | `dev-brainstorm`（轻量级） | 需确认方案细节，但父 roadmap 已提供充分上下文 |
| 实现密集（A0, A1, B0, T1, T6, T3, T4） | `dev-writing-plans` | 边界清晰，直接写 plan → 执行 |
| 验证/扩覆盖（C-P1-*, C-P2-*, C-P3-*） | `dev-writing-plans` | 流程明确，写 plan 后执行 |
| 纯工具/CI（V4, D1, D2, D3） | `dev-executing-plans` | 极小且边界清晰，直接执行 |
| T2 OSR（较高风险） | `dev-brainstorm`（轻量级） | OSR 方案需要确认边界 |
| G8 D3-C（有上游 design doc） | `dev-brainstorm`（轻量级） | 继承已完成的设计文档 |

---

## 11. 调度策略

详见 DISPATCH.md。

dispatch_model: hybrid

- **阶段内并行**：P1/P2/P3 内部，Thread A/B/C 三线程并行
- **阶段间串行**：P2 须等 P1 的 Thread A 核心交付稳定；P3 须等 P2 的 Thread A 核心交付稳定
- **同线程内串行**：每个 Thread 内的子任务有先后依赖，必须按顺序执行
