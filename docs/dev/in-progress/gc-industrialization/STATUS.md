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
- T-A1 ✅ 完成（0b1777e8d）：per-PR 并发 stress 门禁（SCALE=50）
- T-A2 ✅ 完成（84ae5f621）：ASAN per-PR 门禁
- T-A3 ✅ 完成（b8e6da61a + b841b745b）：Linux x64 GC CI + TSAN per-PR
- T-B1 ✅ 完成：CoreCLR WKS safepoint 研读 + A3 Hybrid 设计
- T-B2 ✅ 完成：单 region 分配器设计
- T-B3 ✅ 完成：LEAF barrier 设计（mode switch 绑定版）

batch-1 完成 6/11。progress: T-A1 ✅ T-A2 ✅ T-A3 ✅ T-B1 ✅ T-B2 ✅ T-B3 ✅

## 待同步
- roadmap 的 A3 档定义需同步为 Hybrid 语义（非纯硬 STW，T-B1 已修正）

## latest_stop_point
batch-1 推进至 6/11。下一轮换：T-A4（Release 性能基线）+ T-B4（codegen/JIT 契约）。

## 下一步
- 继续 batch-1：T-A4（Release 性能基线）+ T-B4（codegen/JIT 契约）
- 之后 T-A5/A6 + T-B5

## recommended_next_child
- T-A4（Release 性能基线，P99/阶段/对比 CoreCLR）
- T-B4（codegen/JIT 接口契约，store+barrier 一致性）

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