# GC 工业化极致计划 — STATUS

## Phase
`roadmap`

## task_type
`roadmap`

## roadmap_or_plan
`docs/dev/in-progress/gc-industrialization/roadmap-v1-01.md`

## dispatch_model
`hybrid`（batch-1 并行 / batch-2 串行）

## 调度状态

dispatch_doc: DISPATCH.md
dispatch_model: hybrid
active_batches: []
completed_batches: []
pending_batches: [batch-1, batch-2]
terminals_active: []

## 子任务执行策略
- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`

## 最近摘要
### batch-1（已完成 10/11，2 项 deferred/scope-adjusted）
- 🟡 T-A1（0b1777e8d）：per-PR 并发 stress 门禁（SCALE=50）。功能就位但 CI 首绿待验证。flaky 容忍量化策略：允许 ≤3 次重试后标记为 known flaky 并记录到 known-fail 数据库；单次 stress 运行 ≥1/1000 FAIL 即触发人工审查；若连续 3 次 CI 运行均出现同一 flaky，降级为 blocker 并停止门禁。
- 🟡 T-A2（84ae5f621）：ASAN per-PR 门禁。功能就位但 CI 首绿待验证。
- 🟡 T-A3（b8e6da61a+b841b745b）：**Linux x64 GC CI 已新增** + TSAN 构建框架已就绪。TSAN 在 Windows 上不可用（MSVC 不支持），Linux x64 基线已先行加入 CI。**Linux GC 原生单测首次在 CI 运行**（可能有不绿风险，需先修基线再叠 TSAN 为 per-PR 门禁）。功能就位但 CI 首绿待验证。TSAN 全量 per-PR 门禁延后到 P4（平台覆盖阶段）。
- ✅ T-B1：CoreCLR safepoint 研读 + A3 Hybrid 设计（跨平台 ✅）
- ✅ T-B2：单 region 分配器设计
- ✅ T-B3：LEAF barrier 设计（mode switch 绑定）
- ✅ T-B4：codegen/JIT/interpreter 写屏障契约（S+B+[D] 序列）
- ✅ T-B5：forbid_suspend 强化为互斥护栏（ForbidSuspendThreadHolder 设计）
- ⏸️ T-A4（READY-FOR-CI，**不阻塞 G-P1 gate**——需真实 CI 手动 dispatch 填充基线，属性能基线基建非安全关键；详见 roadmap 子任务映射）。T-A4 性能基线首捕为建议项（non-blocking），不影响 G-P1 启动判定；但建议在 G-P1 过程中尽早完成，否则 P2 启动时缺少性能回退依据。T-A5/A6（deferred，不阻塞 G-P1 gate）

### P1 E0+ — A3 Hybrid safepoint 实现（已完成核心）
- ✅ `7c7872659`：PohAllocate std::mutex → GcSpinLock + ScopedPreemptiveMode（修 HIGH 死锁）。验证：test_gc_poh 9/9
- ✅ `1d1a82d31`：pal_suspend.h PAL 抽象（Windows SuspendThread + POSIX stub）
- ✅ `073809dc3`：ForbidSuspendThreadHolder（跨线程互斥护栏，对齐 CoreCLR m_dwForbidSuspendThread）
- ✅ `007ab4262`：A3 Hybrid safepoint 核心（全局 trap 标志 + EnterCooperativeMode rendezvous）
- ✅ `b77c2b2d5`：A3 hard-suspend drive（PalSuspendThread 集成到 safepoint 超时兜底，CoreCLR 挂起→redirect→立即resume 模式）

## P1 剩余（开放项）
- ❌ **jit_helpers.cpp 静态字段补 dirty_card**：**判断修正为不需要**。审计 CodegenStSFld 发现 g_static_fields 是解释器 sandbox 的非 GC 向量（InterpreterValue），注释明确警告 BgcRecordRootChange 在 BGC re-mark 会损坏内存。它不是托管 AOT 静态字段，不过 Young GC card 扫描，**补 dirty_card 反而危险**。T-B4 契约此条为误判，撤销。
- ⏸️ 单 region 分配器整合（大重构，数周，需真实 CI 护网确认后实施）
- ⏸️ CI 护网验证（ASAN/TSAN/stress 实际 CI 首次绿）

## 结论
P1 的 A3 Hybrid safepoint 骨架已完整实现（trap + rendezvous + hard-drive + forbid 护栏 + pal_suspend），所有对应 GC 单测通过。单 region 分配器与 CI 护网验证是长期开放项。

## latest_stop_point
A3 Hybrid safepoint 骨架全部落地（6 commit）。jit 静态 dirty_card 已撤销（误判）。剩单分配器（大重构）+ CI 护网验证。

## 下一步
- 单 region 分配器整合（大重构，需架构级确认 + 真实 CI 护网）
- 或先跑真实 CI 验证护网首次绿（T-A1/A2/A3）

## 目标
超越 CoreCLR WKS 工业化成熟度，消除所有已知并发正确性缺陷。A3 深度对齐。

## 边界拍板
- 对标: CoreCLR WKS
- 深度: A3 完全对齐（修正后 = Hybrid：软主路径 + 硬驱赶兜底 + mode switch 保证 store+barrier，非全线程物理挂起）
- 验证先行: CI 护网/ASAN/TSAN 就绪再改核心
- 差异化: 热更新 MemoryDomain 保持优势，每 Phase ≥1 维护 commit

## Authority 决策
- A2b → 结构性消除（mode switch 保证 + 强制驱赶兜底，非测试断言）
- A3 架构修正（T-B1 研究结论）：软协作主路径 + SuspendThread(Windows)/SIGUSR2(Linux) 兜底驱赶，根扫描在 rendezvous 做，绝不在挂起态扫根（OS 寄存器不可靠）
- store+barrier 一致性 by mode switch，非原子指令对
- forbidden-suspend → **保留并强化**为互斥护栏（修正原"废弃"方向）
- 三套分配系统 → 统一单 region 分配器
- L1 卡表 → 恒定卡表或 RCU
- BGC root-scan → 根因定位，非归档
- ↕ 跨平台验证结论：方案跨平台可行，现有 PAL 已具备 trampoline/PalPreemptRequest/PalCaptureReliable，唯一新增 pal_suspend.h 三接口+Windows 实现

## 当前结论
并行双轨：Track A（验证基建）+ Track B（A3 设计）→ 汇合后 Phase 1-5。
- **执行节奏**：每半天切换 Track（上午 Track A / 下午 Track B），以周为周期轮换主导方向。
  详见 DISPATCH.md 的并行策略说明。

### Phase 时间线
- **Track A/B**: 验证基建 + A3 设计（并行，2-4周）
- **Phase 1**: A2b A3 实现（ASAN 护网下，4-6周）
- **Phase 2**: 残余并发 bug（含 30% 未知 bug 缓冲，3-5周）
- **Phase 3**: 诊断/配置完备（3-4周）
- **Phase 4**: 性能/平台对齐（4-6周）
- **Phase 5**: 差异化优势（3-4周）

## 风险评估摘要
见 `design-v1-01.md`。核心缓解：Track A 绿后合入 Phase 1；双轨固定节奏；Phase 2 预置缓冲。

## 三优先级权衡结论
性能(中高) / 架构(高) / 热更新(高)。性能排在并发正确性之后。

## 问题清零
- `blocking_questions: []`
- `question_clearance: cleared`
- `clearance_confirmed_by_user: true`

## 关键文档
- `docs/dev/in-progress/gc-industrialization/design-v1-01.md`

## 下一步入口
→ 分流 `roadmap`（需用户确认）