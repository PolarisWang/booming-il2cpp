# Interpreter 工业化补齐 — Roadmap

> **task_id**: interpreter-industrialization
> **创建日期**: 2026-05-23
> **dispatch_model**: sequential（单终端全量执行）
> **child_execution_mode**: auto
> **auto_continue**: true
> **auto_stop_policy**: blocking-only

## 1. 目标

修复 Interpreter 子系统的 15 项差距（R-1 ~ R-13），4 层架构从核心引擎到诊断全部工业化，成熟度从 ~35% 提升至 ~85%+。

## 2. 范围边界

- **Layer 1**: FastExecute 核心缺陷修复 — SEH/Cpblk/InitBlk/CallVirtConstrained/Calli 实现 + 容量扩展
- **Layer 2**: Tier 升级 — 混合 OSR（Call-site + Backedge）+ T4 Native Codegen 加固
- **Layer 3**: 交叉验证 — GC 交互审计 + Stress/Soak/Benchmark/Overflow 测试
- **Layer 4**: 诊断 — 自定义 IL-level 调试协议 + EventPipe 核心层

## 3. 非目标

- 跨平台支持（Windows x64 only）
- COM ICorDebug 兼容
- `dotnet-trace` 原生协议兼容（自定义协议 + 自定义 receiver）
- InterpreterVM 整体重写（FastExecute 已覆盖主流路径）
- 完整 EventPipe 多路复用和 session 管理

## 4. 阶段列表

| Phase | 内容 | 子任务数 | 依赖 | 预估工期 |
|-------|------|---------|------|---------|
| 1 | Layer 1: 核心引擎加固 | 6 | 无 | ~2 周 |
| 2 | Layer 2: Tier 升级与自适应 | 3 | Phase 1 | ~3.5 周 |
| 3 | Layer 3: 交叉验证 | 4 | 部分依赖 Phase 1+2 | ~2 周 |
| 4 | Layer 4: 诊断与可观测 | 2 (含子Phase) | 无 | ~4-6 周 |

推进顺序：Phase 1 → Phase 2 串行；Phase 3 可随 Phase 1+2 并行启动但最终验收依赖 Phase 1+2 完成；Phase 4 完全独立可随时启动。

## 5. 每阶段完成定义

### Phase 1: 核心引擎加固

| 字段 | 内容 |
|------|------|
| **goal** | 所有 FastExecute 降级路径消除，FastFrame 容量可承载真实世界方法 |
| **exit_criteria** | SEH/Cpblk/InitBlk/CallVirtConstrained/Calli 全部在 FastExecute 中实现而非降级；FastFrame stack=64/locals=32/tracked=32；Tracked 溢出触发安全 fallback；全部 integration test 通过 |
| **deliverables** | `fast_dispatch.cpp` SEH handlers + Cpblk/InitBlk/Calli/CallVirtConstrained 实现；`fast_dispatch.h` FastFrame 扩容；tracked 溢出 fallback 逻辑 |
| **dependencies** | 无 |
| **resolved_decisions** | FastFrame 采用方案 B 扩容（stack=64, locals=32, tracked=32）；Tracked 溢出采用 fallback 到 InterpreterVM（方案 2） |
| **watch_items** | SEH 嵌套复杂度可能导致边界 bug；FastFrame 扩容的 TLS 内存增量（~600 bytes → ~1.4KB） |

### Phase 2: Tier 升级与自适应

| 字段 | 内容 |
|------|------|
| **goal** | 热方法自动升级到更快的执行 tier，循环热方法通过 backedge OSR 当前帧升级 |
| **exit_criteria** | Call-site tier upgrade 激活（T1→T2→T3→T4）；Backedge OSR Core (FastExecute→RegisterExecute) 完成；T4 codegen 失败原因分类 + 非永久跳过；全部测试通过 |
| **deliverables** | `entry_direct.cpp` OSR 激活策略；`ir_reg_alloc.cpp` backedge counter；OSR Capture→Restore 全路径验证；T4 codegen 失败分类 |
| **dependencies** | Phase 1 完成 |
| **watch_items** | OSR 转换正确性（Capture→Restore 一致性）；deopt 防护是否充分 |

### Phase 3: 交叉验证

| 字段 | 内容 |
|------|------|
| **goal** | Interpreter 的测试覆盖从 ~40% 提升至 ~85%+，建立回归防护 |
| **exit_criteria** | GC×Interpreter 交互审计并修复 barrier 缺口；Stress 测试 4/8/16 线程 50000 iters 通过；Soak 8h 无崩溃；Benchmark 回归基线建立；Overflow-check 9 个 opcode 专项测试通过 |
| **deliverables** | GC root 扫描审计报告；`interpret_stress_test.cpp`；`bench-compare` 工具；Overflow-check 测试套件 |
| **dependencies** | 部分测试需要 Layer 1+2 完成后才能跑完整路径 |
| **watch_items** | GC barrier 缺口可能在审计中发现需要修复，届时需要同时修复 FastExecute 和 InterpreterVM |

### Phase 4: 诊断与可观测

| 字段 | 内容 |
|------|------|
| **goal** | 解释器具备 IL-level 调试能力和运行时事件发布能力 |
| **exit_criteria** | Debugger Phase 1-4（FastExecute+RegisterExecute 断点/单步/栈展开/变量查看）完成；EventPipe Phase 1-3（IPC + 3 类事件）完成；自定义 receiver CLI 可用 |
| **deliverables** | `src/native/diagnostics/debugger/` 模块；`src/native/diagnostics/eventpipe/` 模块；`src/native/diagnostics/tools/chaos-diag/` CLI |
| **dependencies** | 无（完全独立） |

## 6. 子任务映射

### Phase 1: 核心引擎加固

| task_id | phase | status | purpose | depends_on | conflict_scope | estimated_effort |
|---------|-------|--------|---------|-----------|---------------|-----------------|
| I-R1 | 1 | ready | SEH in FastExecute — 消除 50% 方法 200x 降级 | 无 | `fast_dispatch.cpp`, `fast_dispatch.h`, `interpreter_vm.h` | 1 周 |
| I-R3 | 1 | ready | Cpblk/InitBlk FastExecute 实现 | I-R1 可选 | `fast_dispatch.cpp` | 0.5 天 |
| I-R4 | 1 | ready | CallVirtConstrained FastExecute 实现 | I-R1 可选 | `fast_dispatch.cpp` | 0.5 天 |
| I-R10 | 1 | ready | Calli 间接函数指针调用实现 | I-R1 可选 | `fast_dispatch.cpp` | 0.5 天 |
| I-R5 | 1 | ready | FastFrame 容量扩展 (stack=64, locals=32, tracked=32) | 无 | `fast_dispatch.h` | 1 天 |
| I-R12 | 1 | ready | Tracked 对象溢出触发安全 fallback | I-R5 可选 | `fast_dispatch.h`, `fast_dispatch.cpp` | 0.5 天 |

### Phase 2: Tier 升级与自适应

| task_id | phase | status | purpose | depends_on | estimated_effort |
|---------|-------|--------|---------|-----------|-----------------|
| I-R2a | 2 | planned | Call-site tier upgrade 激活 (T1→T2→T3→T4) | Phase 1 | 3 天 |
| I-R2b | 2 | planned | Backedge OSR (RegisterExecute→T4) | I-R2a | 2 周 |
| I-R11 | 2 | planned | T4 Native Codegen 加固 (失败分类+deopt 防护+PGO) | I-R2a | 1 周 |

### Phase 3: 交叉验证

| task_id | phase | status | purpose | depends_on | estimated_effort |
|---------|-------|--------|---------|-----------|-----------------|
| I-R6 | 3 | planned | GC×Interpreter 交互审计 + Write Barrier 验证 | 可并行 | 5 天 |
| I-R7 | 3 | planned | Stress (4/8/16 线程) + Soak (8h) 测试 | 可并行，部分依赖 Phase 1+2 | 5 天 |
| I-R8 | 3 | planned | Benchmark 回归管线 (JSON 输出 + bench-compare) | 无 | 3 天 |
| I-R9 | 3 | planned | Overflow-check 9 opcode 专项测试 | 无 | 1 天 |

### Phase 4: 诊断与可观测

| task_id | phase | status | purpose | depends_on | estimated_effort |
|---------|-------|--------|---------|-----------|-----------------|
| I-DBG | 4 | planned | Debugger MVP (Phase 1-4): 断点+单步+栈展开+变量 | 无 | 3 周 |
| I-EVP | 4 | planned | EventPipe 核心层: IPC+GC/TP/Exception 事件 | 无 | 1.5 周 |

## 7. 依赖

```
Phase 1 ──→ Phase 2 ──→ (Phase 3 最终验收需要 Phase 1+2)
   │                        │
   │                        └── Phase 3 部分测试可提前启动
   │
   └── Phase 4 (完全独立，随时可开始)
```

## 8. 风险

| 风险 | 级别 | 缓解 |
|------|------|------|
| SEH 嵌套/重入逻辑复杂 | 🟡 中 | 参考 interpreter_vm.cpp 成熟实现，写专项 SEH 测试 |
| OSR 状态 Capture→Restore 不一致 | 🟡 中 | 每对转换器写 RoundTrip 测试（Capture→Restore→Capture, 验证相等） |
| GC barrier 审计发现缺口 | 🟡 中 | 发现即修，不阻塞 Phase 1+2 启动 |
| T4 codegen 与现有组件耦合 | 🟡 中 | 失败分类 + 非永久跳过策略 |
| Debugger 4 层管线适配复杂 | 🟡 中 | 先用 Phase 1-4 覆盖 95% 路径，Phase 5-6 后续 |
| EventPipe 自定义协议兼容 | 🟢 低 | 明确定义协议头，不绑定 dotnet-trace |

## 9. 备选路径

- **Phase 1 卡在 SEH**：如果 SEH 嵌套实现超出预期，可以先放行非 SEH 方法的 Cpblk/InitBlk/Calli 修复，SEH 单独延后到 Phase 2 前的独立子任务
- **Phase 2 OSR 正确性问题**：如果 Capture→Restore RoundTrip 测试发现不可修复的不一致，退回到 Phase 1（Call-site only，当前帧不升级），OSR 归为长期优化
- **Phase 4 Debugger 量级评估超标**：优先 Phase 1-3（SequencePoint + 断点），Phase 4-6（栈展开+变量+Native）顺延

## 10. 当前建议推进顺序

```
1. Phase 1: I-R5 (FastFrame 扩容) — 前置基础，无依赖
2. Phase 1: I-R1 (SEH in FastExecute) — 最大收益
3. Phase 1: I-R3, I-R4, I-R10, I-R12 (并行，依赖或可选依赖 I-R1)
   ── Phase 1 exit: 所有 integration test 通过 ──
4. Phase 2: I-R2a (Call-site tier upgrade)
5. Phase 2: I-R2b (Backedge OSR)
6. Phase 2: I-R11 (T4 codegen 加固)
   ── Phase 2 exit: OSR + T4 全通 ──
7. Phase 3: I-R9, I-R8 (无依赖，可先做)
8. Phase 3: I-R6 (GC 审计)
9. Phase 3: I-R7 (Stress/Soak)
   ── Phase 3 exit: 所有测试通过 ──
10. Phase 4: I-EVP (EventPipe)
11. Phase 4: I-DBG (Debugger)
   ── Phase 4 exit: 诊断工具可用 ──
```

## 11. 子任务执行策略

- **child_execution_mode**: auto
- **auto_continue**: true
- **auto_stop_policy**: blocking-only
- **recommended_next_child**: I-R5 (FastFrame 扩容)

Phase 1 中的 I-R5 是基础（无依赖），I-R1 是最大收益。启动顺序：I-R5 → I-R1。
