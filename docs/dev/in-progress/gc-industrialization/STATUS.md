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
batch-1 已全部处理（11/11）：
- ✅ T-A1（0b1777e8d）：per-PR 并发 stress 门禁（SCALE=50）
- ✅ T-A2（84ae5f621）：ASAN per-PR 门禁
- ✅ T-A3（b8e6da61a+b841b745b）：Linux x64 GC CI + TSAN per-PR
- ✅ T-B1：CoreCLR safepoint 研读 + A3 Hybrid 设计（跨平台 ✅）
- ✅ T-B2：单 region 分配器设计
- ✅ T-B3：LEAF barrier 设计（mode switch 绑定）
- ✅ T-B4：codegen/JIT/interpreter 写屏障契约（S+B+[D] 序列，补 jit 静态路径）
- ✅ T-B5：forbid_suspend 强化为互斥护栏（ForbidSuspendThreadHolder 设计）
- ⏸️ T-A4（READY-FOR-CI）：性能基线基建全就绪，需真实 CI 跑 green run 填充
- ⏸️ T-A5（deferred）：known-fail expiry 量小风险高，暂缓
- ⏸️ T-A6（deferred）：测试框架 singleton reset，延到 P2

## batch-1 收口判定
batch-1 的持续交付件（CI 变更 + A3 设计文档）全部完成。T-A4（CI 依赖）/A5/A6（deferred）不阻塞 G-P1 gate。

**G-P1 gate 检查**：
- T-A 绿：✅（ASAN per-PR + TSAN per-PR + stress per-PR 已就位；Linux 基线未首次绿=已知风险）
- T-B 设计完成：✅（a3-safepoint/allocator/leaf-barrier/contract/forbid-suspend 5 件全套）

## 设计文档清单（T-B 系列，供 P1 消耗）
- `a3-safepoint-design.md`：A3 Hybrid 架构 + 跨平台验证
- `a3-allocator-design.md`：单 region 分配器
- `a3-leaf-barrier-design.md`：LEAF barrier + mode switch
- `a3-contract-design.md`：写屏障 S+B+[D] 契约
- `a3-forbid-suspend-design.md`：硬驱赶互斥护栏

## latest_stop_point
**CHECKPOINT-1（batch-1 收口，等真实 CI 撑稳护网后启动 P1）**

batch-1 全部交付完成（CI 变更 + A3 设计 5 件套）。G-P1 gate 条件已满足，但按「验证先行」原则，P1 动手改 safepoint/分配器前需先确认护网真实绿。用户已确认选 A（暂停等 CI）。

## 下一步
先推进「真实 CI 验证清单」（见下），全部绿后启动 G-P1。

## G-P1 启动前置（真实 CI 验证清单）
1. **T-A3 Linux TSAN 首次绿**—新 Linux x64 GC CI 首跑；Linux GC 单测从未在 CI 验证，可能现未绿需先修基线（最大的未知风险）
2. **T-A2 ASAN per-PR 首跑**—`gc-asan` job 在 GC PR 上首跑是否绿
3. **T-A1 stress 门禁首跑**—per-PR SCALE=50 stress 是否绿（已知 SCALE=50 单独绿，需确认并行 -j4 下稳定）
4. **T-A4 性能基线首捕**—手动 dispatch `gc-stress-nightly` + `update_baseline=true` 填充 gc.perf.yaml
5. **回归**—现有 Windows GC 单元门禁（gc-ci.yml）在以上改动提交后仍绿

以上 1-5 全绿 → G-P1 可启动。

## recommended_next_child
- G-P1（A2b A3 实现）——待真实 CI 清单全绿

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