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
roadmap 已成稿。hybrid 调度：batch-1（T-A1..A6 验证基建 + T-B1..B5 A3 设计，11 子任务并行就绪）+ batch-2（G-P1..P5 串行实施）。单终端实际按周粒度轮换并行。

## latest_stop_point
roadmap 文档与 DISPATCH 已产出。停滞在 batch-1 派发前，等待用户确认派发执行终端。

## 下一步
- 待用户确认 batch-1 派发启动
- 确认后进入 `dev-executing-plans` / `lightweight-brainstorm` 逐子任务执行

## recommended_next_child
- 串行推荐：T-A1（最轻，先做 CI 门禁）+ T-B1（设计起点，并行）

## 目标
超越 CoreCLR WKS 工业化成熟度，消除所有已知并发正确性缺陷。A3 深度对齐。

## 边界拍板
- 对标: CoreCLR WKS
- 深度: A3 完全对齐（硬 STW + 单 region 分配器 + LEAF barrier + 全链路）
- 验证先行: CI 护网/ASAN/TSAN 就绪再改核心
- 差异化: 热更新 MemoryDomain 保持优势，每 Phase ≥1 维护 commit

## Authority 决策
- A2b → 结构性消除（A3），非测试断言
- 三套分配系统 → 统一单 region 分配器
- 写屏障 → LEAF 汇编（x64+ARM64）
- forbid_suspend.h ack-and-continue → 硬 STW 后废弃
- L1 卡表 → 恒定卡表或 RCU
- BGC root-scan → 根因定位，非归档

## 当前结论
并行双轨：Track A（验证基建）+ Track B（A3 设计）→ 汇合后 Phase 1-5。

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