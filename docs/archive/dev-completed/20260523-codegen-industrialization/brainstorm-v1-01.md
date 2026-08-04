# Codegen 工业化收官 — Brainstorm 设计方案

> 基于 2026-05-23 差距分析报告及三轮边界确认
> 状态: brainstorming, 待用户审查确认

---

## 1. 纳入范围（合并后 Gap 清单）

从解释器工业化拉入 4 项，从 GC 工业化 sequel 拉入 1 项，共计 **39 项差距**：

### Thread A — 托管 AOT Codegen（含解释器基础设施依赖）

| ID | 差距 | 原始来源 | 预估工作量 | 优先级 |
|----|------|----------|-----------|--------|
| **A0** | **I-R5: FastFrame 容量扩展** | 解释器工业化 | 1-2 周 | P0（基础设施） |
| **A1** | **I-R1: SEH in FastExecute** | 解释器工业化 | 2-3 周 | P0（基础设施） |
| G1 | 异常 lowering（catch/finally/filter） | 原始 G1 | 3-4 周 | P0 |
| G2 | 虚/接口分发（callvirt/callinterface） | 原始 G2 | 3-4 周 | P0 |
| G3 | ABI 载体扩展（非 Int32 参数/返回值） | 原始 G3 | 3-4 周 | P0 |
| G4 | 剩余 flat fallback（runtime-self-test 89.3%） | 原始 G4 | 1 周 | P1 |
| G5 | 元数据闭包统一（令牌级） | 原始 G5 | 2-3 周 | P1 |
| G6 | 泛型/共享 lowering | 原始 G6 | 3-4 周 | P1 |
| G7 | goto 发射消除 | 原始 G7 | 2-3 周 | P1 |
| G8 | D3-C 插件化 Phase 1（Family Registry） | 原始 G8 | 1-2 周 | P1 |
| G9 | 桥接/导入 thunk lowering | 原始 G9 | 2 周 | P2 |
| G10 | 不可约 CFG pc-dispatch | 原始 G10 | 2-3 周 | P2 |
| G11 | 多 assembly 编译硬化 Phase 4-5 | 原始 G11 | 4-6 周 | P2 |
| G12 | D3-C Phase 2-4 | 原始 G12 | 4-6 周 | P2 |

### Thread B — 原生 T4 Codegen

| ID | 差距 | 原始来源 | 预估工作量 | 优先级 |
|----|------|----------|-----------|--------|
| **B0** | **I-R2a: 调用站点层级升级激活** | 解释器工业化 | 3 天 | P0（基础设施） |
| T1 | 精确 GC Slot Map | 原始 T1 | 2-3 周 | P0 |
| T2 | 溢出 deopt → 真正 OSR（含 I-R2b） | 原始 T2 + I-R2b | 3-4 周 | P0 |
| T3 | TLAB 内联分配（V3.5） | 原始 T3 | 1-2 周 | P1 |
| T4 | PIC 多态扩展（megamorphic vtable） | 原始 T4 | 2-3 周 | P1 |
| T5 | PGO 集成（含 I-R11 PGO 部分） | 原始 T5 + I-R11 | 2 周 | P1 |
| T6 | T4 失败分类保护 | I-R11 | 1 周 | P1 |
| T7 | 图着色寄存器分配集成 | 原始 T7 | 2-3 周 | P1 |
| T8 | 调试信息（DWARF/CodeView） | 原始 T8 | 4-6 周 | P2 |
| T9 | 跨模块 hotpatch | 原始 T9 | 2-3 周 | P2 |
| T10 | 码率优化 | 原始 T10 | 持续 | P2 |

### Thread C — 测试验证 + 文档

| ID | 差距 | 原始来源 | 预估工作量 | 优先级 |
|----|------|----------|-----------|--------|
| **C0** | **A3: codegen verification pipeline 打通** | GC 工业化 sequel | 1-2 周 | P0 |
| V1 | foundation-dll 全部 96 family 全 13 阶段验证 | 原始 V1 | 8-12 周 | P1 |
| V2 | 代码覆盖率门禁 | 原始 V2 | 2-3 周 | P1 |
| V3 | 快照测试基线扩增（P0+P1+P2） | 原始 V3 | 3-4 周 | P1 |
| V4 | CI/CD PR 级 codegen 回归 | 原始 V4 | 2-3 周 | P1 |
| V5 | 语义等价性验证框架 | 原始 V5 | 4-6 周 | P2 |
| V6 | 内存/GC 正确性验证 | 原始 V6 | 3-4 周 | P2 |
| D1 | Codegen API 文档 | 原始 D1 | 1-2 周 | P1 |
| D2 | 故障诊断指南 | 原始 D2 | 1 周 | P1 |
| D3 | 性能基线（编译时间/码体积追踪） | 原始 D3 | 2 周 | P1 |

---

## 2. 依赖关系图

```
T0 (基础设施层，三线程前提)
├── A0 (FastFrame 容量) ──→ A1 (SEH in FastExecute)
│                              │
│                              ▼
│                         Thread A 主线
│                         ├── G1 异常 lowering
│                         ├── G3 ABI 载体扩展
│                         ├── G2 虚/接口分发
│                         ├── G5 元数据闭包
│                         └── G6 泛型/共享
│                              │
│                              ▼ (A 第一阶段稳定后)
│                         C0 A3 验证管线打通
│                         V1 96 family 全验证
│
├── B0 (tier-up 激活) ──→ Thread B 主线
│                         ├── T1 精确 GC Slot Map
│                         ├── T2 OSR deopt
│                         ├── T4 PIC 扩展
│                         ├── T5 PGO 集成
│                         └── T6 失败分类保护
│
└── Thread C 独立主线（无代码依赖）
    ├── V4 CI/CD 回归（立即启动）
    ├── D1-D3 文档（立即启动）
    ├── V3 快照测试基线（立即启动）
    ├── V2 覆盖率门禁（Phase 2）
    ├── V5 语义等价（Phase 3）
    └── V6 内存正确性（Phase 3）
```

**关键依赖标记：**
- A0 → A1（串行：FastFrame 先于 SEH）
- A0-A1 → Thread A 主线（解释器基础设施完成前不能安全生成异常/分发代码）
- Thread A Phase 1 → C0/V1（验证管线需要稳定 AOT 输出）
- B0 → T2/T5（tier-up 激活是 OSR 和 PGO 的前提）
- V4/V3/D 与所有 Thread 无代码依赖，可立即并行

---

## 3. 三阶段推进计划

### Phase 1 — 基础设施 + 核心能力（8-10 周）

| 线程 | 工作项 | 时间 | 产出 |
|------|--------|------|------|
| **A (FT)** | A0 FastFrame 容量 → A1 SEH in FastExecute → G1 异常 lowering | 6-8 周 | 解释器支持异常、AOT 生成含 EH 的 C++ |
| **B (HT)** | B0 tier-up 激活 → T1 精确 GC Slot Map → T6 失败分类 | 4-6 周 | T4 代码可被主动激活、精确 GC、失败不永久跳过 |
| **C (HT)** | V4 CI/CD 回归门禁 → V3 快照基线扩增 → D1/D2 文档 | 4-6 周 | PR 级回归检测、基线覆盖 P0+P1 |

**Phase 1 退出标准（Milestone: 核心可用）：**
- 有 EH 的基本方法可通过 AOT 正确生成
- T4 具备精确 GC + tier-up 激活
- CI 有 codegen 回归门禁

### Phase 2 — 覆盖扩展 + 性能（8-10 周）

| 线程 | 工作项 | 时间 | 产出 |
|------|--------|------|------|
| **A (FT)** | G3 ABI 扩展 → G2 虚/接口分发 → G4 flat fallback 清理 → G5 元数据闭包 | 6-8 周 | 全 ABI 支持、虚/接口分发、100% 结构化恢复 |
| **B (HT)** | T2 OSR deopt → T4 PIC 多态 → T3 TLAB 内联分配 | 6-8 周 | OSR 栈上替换、多态分发、内联分配 |
| **C (HT)** | C0(A3)管线打通 → V1 前 50 family → V2 覆盖率门禁 | 6-8 周 | 50% foundation-dll 全 13 阶段通过 |

**Phase 2 退出标准（Milestone: 生产就绪）：**
- 含虚方法/接口的方法可通过 AOT 生成
- T4 具备 OSR + 多态 PIC
- 半数 foundation-dll family 全验证通过

### Phase 3 — 收官硬化（8-10 周）

| 线程 | 工作项 | 时间 | 产出 |
|------|--------|------|------|
| **A (FT)** | G6 泛型/共享 → G7 goto 消除 → G8 D3-C Phase 1 → G9-G12 | 8-10 周 | 泛型支持、无 goto 生成、插件化注册表 |
| **B (HT)** | T5 PGO 集成 → T7 图着色 → T8-T10 | 6-8 周 | PGO 引导、图着色分配、调试信息 |
| **C (HT)** | V1 剩余 46 family → V5/V6 正确性验证 → D3 性能基线 | 8-10 周 | 96 family 全通过、语义等价验证 |

**Phase 3 退出标准（Milestone: 工业化完成）：**
- 覆盖异常/分发/ABI/泛型四大场景
- T4 103/103 opcode + PGO + 精确 GC + OSR + 图着色
- 96 family 全部通过 13 阶段验证
- 无 goto 发射、D3-C Phase 1 完成、validator 全绿

---

## 4. 风险评估

| 风险 | 概率 | 影响 | 缓解策略 |
|------|------|------|----------|
| **A0-A1 阻塞 Thread A 主线进入** | 中 | 高 | A0 1-2 周先做，不阻塞 Thread B/C |
| **G1 异常 lowering 设计复杂度超预期** | 中 | 高 | 先从 TryCatch 单 shape 开始，逐步扩展 |
| **T1 精确 GC 实现复杂度超预期** | 中 | 高 | 保守策略：先支持 simple GC points，逐步扩 |
| **V1 96 family 验证逐个失败（非 codegen 侧问题）** | 高 | 中 | 分类：codegen bug vs 运行时 bug vs 测试框架 bug |
| **跨线程依赖导致等待** | 中 | 中 | C 的前期任务（V4/V3/D）无依赖，可填满等待期 |
| **T2 OSR 实现范围膨胀** | 中 | 中 | 严格限定边界：先支持 backedge 单一场景，不泛化 |
| **人力波动（HT = 半职，可能被其他任务打断）** | 高 | 中 | Phase 规划中 B/C 任务拆为 2-4 周粒度，天然适配半职 |

---

## 5. 三优先级权衡结论

收官阶段的特殊性：**大部分工作在补 P2（方案完美性）和 P1（性能）的债**。

| 优先级 | 在本计划中的体现 |
|--------|----------------|
| **P1 性能** | T1 精确 GC（安全→性能）、T3 TLAB 内联分配、T5 PGO、T7 图着色 |
| **P2 架构完美** | G1-G6 正确性补课（异常/分发/ABI/泛型）、G7 goto 消除、G8 D3-C |
| **P3 HotUpdate** | T9 跨模块 hotpatch（P2 项，低优先级） |

**裁决**：收官期间不出现三优先级冲突——因为补的大部分是"该有却没有"的能力，而不是"同类方案选择"的取舍。如果出现（如 G7 goto 消除 vs 生成代码体积），P2 让位于 P1。

---

## 6. 待决项

`blocking_questions = []`

`question_clearance = cleared`

`clearance_confirmed_by_user = PENDING`
