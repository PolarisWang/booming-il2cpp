# Codegen 工业化收官

> 父任务: 无（顶级 roadmap）
> 创建日期: 2026-05-23
> 上游设计: brainstorm-v1-01.md（已确认）
> 下游执行: roadmap-v1-01.md, DISPATCH.md

---

## Phase

`roadmap`

## Type

`roadmap`

## Task Type

`roadmap`

## Roadmap or Plan

`roadmap-v1-01.md`

## 关键文档

- [Brainstorm 设计方案](brainstorm-v1-01.md) — 差距清单、依赖分析、三优先级权衡
- [Roadmap 路线图](roadmap-v1-01.md) — 阶段定义、子任务映射、退出标准
- [调度总纲](DISPATCH.md) — 并行批次、文件冲突检测、合并协议

## 设计摘要

### 边界拍板

- **范围**: 39 项差距全覆盖（G1-G12 + T1-T10 + V1-V6 + D1-D3 + 解释器工业化 4 项 + GC sequel A3）
- **非目标**: 不重新设计架构、不替换 Scriban、不引入新 IR、不改 ABI 合约
- **纳入**: I-R5/FastFrame、I-R1/SEH、I-R2a/tier-up、I-R2b/OSR、A3/verification pipeline

### Authority 决策

- 三线程分工: Thread A（托管 AOT, FT）、Thread B（T4, HT）、Thread C（验证+文档, HT）
- 三阶段: Phase 1 核心可用 → Phase 2 生产就绪 → Phase 3 收官硬化
- 三优先级: 收官期无严重冲突，P1 性能 > P2 架构 > P3 hotupdate

### 当前结论

- **Phase 1 全部完成 ✅** — 所有 7 个子任务已进入终态
- A-P1-1（A0 FastFrame + A1 SEH）✅ 由解释器工业化 I-R5 + I-R1 完成
- B-P1-1（B0 tier-up + T6 failure classification）✅ 由解释器工业化 I-R2a + I-R11 完成
- A-P1-2（G1 异常 lowering）✅ 新增 MultipleCatchExceptionMethodShape 6th detector，6 flat fallback 清零
- B-P1-2（T1 精确 GC）✅ 活跃度分析接入 + RecordGcPoint per-safepoint 过滤 + LdFld/Call 类型推断改进
- C-P1-1（CI/CD）✅ GitHub Actions codegen-regression.yml 创建
- C-P1-2（快照测试）✅ 新增 8 EH fixtures，总 fixture 88 / 基线 85
- C-P1-3（文档）✅ 故障诊断指南 + API 文档
- 当前可执行: Phase 2（A-P2-1, A-P2-2, A-P2-3, B-P2-1, B-P2-2, C-P2-1, C-P2-2, C-P2-3）

### 风险评估摘要

- G1 异常设计复杂度超预期（中概率/高影响）→ 已解决 ✅
- T1 精确 GC 实现复杂度超预期（中概率/高影响）→ 已解决 ✅
- V1 96 family 逐个验证失败（高概率/中影响）
- 人力波动（HT 被抽走，高概率/中影响）
- 缓解策略详见 brainstorm-v1-01.md

### 三优先级权衡结论

收官期补课性质为主，不出现同类方案的三优先级冲突。如 goto 消除 vs 码体积出现冲突，P1 性能优先。

---

## 最近摘要

2026-05-23: Phase 1 启动。三个并行调研 Agent 返回，关键发现：

1. **G1 (异常 AOT lowering)** — 已基本完成: `EmitIRExceptionRegion` 覆盖全部 5 种 EH shape，`CHAOS_EH_TRY/CATCH/END` 宏完备。仅剩余 6 个 flat fallback（3 runtime-self-test + 2 snapshot-prover + 1 error-info-basic）由非常规 EH region 结构导致。A-P1-2 实际完成度 ≈ 98%
2. **T1 (精确 GC Slot Map)** — 基础设施已完备（GcSlotMapV0、GcScanPreciseFrame、RecordGcPoint），但不精确：无 per-safepoint 过滤、类型推断保守（LdFld/Call 默认 ObjectRef）、活跃度分析未接入。B-P1-2 实际完成度 ≈ 40%
3. **C-P1-1 (CI)** — 绿地任务，无任何 CI 基础设施
4. **C-P1-2 (快照测试)** — 已成熟: 80 fixtures / 77 baselines
5. **C-P1-3 (文档)** — ✅ 已完成: 故障诊断指南 (codegen-troubleshooting-guide.md) + API 文档 (04-NativeAotPlanner-API.md)

2026-05-23 (续): C-P1-3 完成，进入剩余 Phase 1 任务并行推进阶段。4 个并行 Agent 分别在 G1 flat fallback 修复、T1 精确 GC、快照 EH fixtures、CI 门禁 四个方向同步实现。

2026-05-23 (Phase 1 完成): 全部 7 个 Phase 1 子任务进入终态：
- **A-P1-2** ✅ 新增 MultipleCatch 第 6 shape detector，6 flat fallback 全部清零
- **B-P1-2** ✅ 活跃度分析接入 NativeCodeGenerator::Generate() → RecordGcPoint 精确过滤
- **C-P1-1** ✅ GitHub Actions CI workflow (codegen-regression.yml)
- **C-P1-2** ✅ 8 个 EH fixture 新增（try-finally, try-fault, try-filter, multiple-catch, catch-finally-nested, filter-finally, nested-try-catch, fault-finally）
- **C-P1-3** ✅ 已归档

Phase 1 exit criteria 全部满足，进入 Phase 2 生产覆盖阶段。

## 调度状态

```yaml
dispatch_doc: DISPATCH.md
dispatch_model: hybrid
active_batches: [P2A, P2B, P2C]
completed_batches: [P1A, P1B, P1C, P1-merge]
terminals_active: {}
pending_batches: [P2-merge, P3A, P3B, P3C]
recommended_next_child: A-P2-1
```

## 子任务状态

| task_id | phase | status | owner | purpose |
|---------|-------|--------|-------|---------|
| **A-P1-1** | P1 | **completed** | FT | A0 FastFrame + A1 SEH ✅ (解释器工业化已归档) |
| **B-P1-1** | P1 | **completed** | HT | B0 tier-up + T6 failure classification ✅ (解释器工业化已归档) |
| **A-P1-2** | P1 | **completed** | FT | G1 Exception lowering ✅ (新增 MultipleCatch 6th shape detector) |
| **B-P1-2** | P1 | **completed** | HT | T1 Precise GC slot map ✅ (活跃度分析+per-safepoint过滤) |
| **C-P1-1** | P1 | **completed** | HT | V4 CI/CD regression gate ✅ (.github/workflows/codegen-regression.yml) |
| **C-P1-2** | P1 | **completed** | HT | V3 Snapshot baseline expansion ✅ (8 EH fixtures 新增) |
| **C-P1-3** | P1 | **completed** | HT | D1+D2 API docs + troubleshooting ✅ (已归档) |
| **A-P2-1** | P2 | **ready** | FT | G3 ABI + G4 flat fallback |
| **A-P2-2** | P2 | **planned** | FT | G2 Virtual/interface dispatch |
| **A-P2-3** | P2 | **planned** | FT | G5 Metadata closure |
| **B-P2-1** | P2 | **ready** | HT | T2 OSR deopt (溢出→OSR) |
| **B-P2-2** | P2 | **planned** | HT | T4 PIC + T3 TLAB inline |
| **C-P2-1** | P2 | **ready** | HT | C0(A3) verification pipeline |
| **C-P2-2** | P2 | **planned** | HT | V1 first 50 families |
| **C-P2-3** | P2 | **planned** | HT | V2 Coverage gate |
| A-P3-1 | P3 | planned | FT | G6 Generics/sharing |
| A-P3-2 | P3 | planned | FT | G7 Goto elimination + G8 D3-C P1 |
| A-P3-3 | P3 | planned | FT | G9-G12 remaining gaps |
| B-P3-1 | P3 | planned | HT | T5 PGO integration |
| B-P3-2 | P3 | planned | HT | T7 Graph coloring allocator |
| B-P3-3 | P3 | planned | HT | T8-T10 debug/hotpatch/codesize |
| C-P3-1 | P3 | planned | HT | V1 remaining 46 families |
| C-P3-2 | P3 | planned | HT | V5+V6 semantic/memory correctness |
| C-P3-3 | P3 | planned | HT | D3 Performance baseline |

## 执行策略

```yaml
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
```

## Latest Stop Point

Phase 1 全部完成。Phase 2 启动。

## 下一步

Phase 2 当前可启动子任务:

| 子任务 | 前置依赖 | 预估工作量 | 描述 |
|--------|---------|-----------|------|
| **A-P2-1** (G3+G4 ABI + flat fallback) | A-P1-2 | 3-4 周 | 非 Int32 参数/返回值 ABI、runtime-self-test 100% 结构化 |
| **B-P2-1** (T2 OSR deopt) | B-P1-2 | 3-4 周 | 真正 OSR 栈上替换，Backedge 触发 |
| **C-P2-1** (C0/A3 验证管线) | A-P1-2 稳定 | 1-2 周 | 验证管线全 13 阶段可运行 |

## 入口

`entry_skill` = 按子任务类型分配（详见 roadmap-v1-01.md §10）

## Question Clearance

```yaml
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

## Child Execution Mode

`auto`

## Clearance Source

`parent-roadmap`

## Recommended Next Child

`A-P2-1`
