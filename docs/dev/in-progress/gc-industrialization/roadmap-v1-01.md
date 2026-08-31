# GC 工业化极致计划 — Roadmap

## 1. 目标

超越 CoreCLR WKS 的工业化成熟度。消除所有已知并发正确性缺陷（A2b、L1 卡表竞态、BGC root-scan 挂起、CodegenStSFld 三路不一致），通过「验证先行 + A3 深度对齐」在 CI 护网/ASAN/TSAN 保障下结构性重写安全关键路径，最终达到可背书生产且对标 CoreCLR WKS 的 GC。

## 2. 范围边界

- **包含**：GC 核心（safepoint/写屏障/region 分配器）、codegen 写屏障指令序列、JIT 寄存器根扫描、interpreter 帧扫描、CI 门禁加固、ASAN/TSAN、性能基线、诊断完备（ETW/GCMemoryInfo/GCNotification）、平台覆盖（ARM64/Server GC）、差异化优势（MemoryDomain/GcLayout/NUMA）
- **对标**：CoreCLR WKS

### 深度分级（A1 / A2 / A3）

全文使用 A1/A2/A3 表示「对 CoreCLR WKS 的对齐深度」，为避免与正文中的 bug 代号（A2b）混淆，此处显式定义：

- **A1 ｜ 关键段对齐**：仅对写屏障关键路径 + safepoint 握手采用硬 STW（SuspendThread/GetThreadContext），主体保留现有 handshake 软路径。消除绝大部分 A2b 窗口，接受残余极窄竞态窗口。用于「硬 STW 全线程开销不可接受」时的回退档。
- **A2 ｜ 混合对齐**：软路径（rendezvous handshake）为主 + 硬驱动双螺旋兜底，仅对确证需原子的 store+barrier 对切换模式。
- **A3 ｜ 完全对齐**：**Hybrid**（软主路径 + 硬驱赶兜底，**非**全线程物理挂起停留）。全局 trap 标志 + 事件排队（软，主路径），`SuspendThread`（Windows）/ `SIGUSR2`（Linux）仅兜底驱赶，根扫描在 rendezvous 中做（**绝不在挂起态扫描**——CoreCLR 已证伪 OS 寄存器一致性），store+barrier 一致性由 **mode switch 保证**（非原子指令对）。+ 单 region 分配器 + LEAF 汇编 barrier + 全链路安全。本路线图默认目标深度。**修正来源**：T-B1 CoreCLR 源码研究（threadsuspend.cpp:3111）证明「全线程物理挂起停留」是 OS 寄存器不可靠的证伪路径。

> ⚠️ 注意：A2b 是具体并发 bug 代号（cross-gen 写屏障竞态），与深度档 A1/A2/A3 无关。凡提到「A1 深度/A3 深度」均指上表对齐档位。

## 3. 非目标

- 不重写 full GC 的并行 mark-sweep 为区别于现有架构的全新设计（沿用现有并行 work-stealing 架构，仅修死锁/瓶颈）
- 不做 CoreCLR 完整对标（如 2000 测试、SOS 调试器扩展）——il2cpp 场景不强制需要
- 不实现非分代回退模式（对标 Go 的 <1ms 无分代路径）——本项目是分代 GC
- 架构上不强行统一为 single-file 或全 lock-free（现有 GcSpinLock + RAII 模式已合理，只修已知缺陷）

## 4. 阶段列表

| Phase | 名称 | 类型 | 依赖 | 预估 |
|:------|:-----|:----:|:-----|:----:|
| T-A | 验证基础设施（CI 门禁 / ASAN / 性能基线） | 并行准备 | 无 | 2-4周 |
| T-B | A2b A3 架构设计 | 并行准备 | 无 | 2-4周 |
| P1 | A2b A3 实现 | 串行 | T-A 绿 + T-B 设计完成 | 4-6周 |
| P2 | 残余并发 bug 修复 | 串行 | P1 | 3-5周 |
| P3 | 诊断/配置完备 | 串行 | P2 | 3-4周 |
| P4 | 性能/平台对齐 | 串行 | P3 | 4-6周 |
| P5 | 差异化优势 | 串行 | P4 | 3-4周 |

**全局优先级约束**：性能(1) > 架构(2) > 热更新(3)。本计划在「并发正确性」语境下对优先级做了领域化裁决：**可靠性和安全性高于纯性能**（记录的裁决——并发正确性缺失会直接导致崩溃/UAF，属于必须优先于性能的硬约束）。性能优化（P4）排在并发正确性修复（P1/P2）之后。热更新优势（P5）有独立阶段，不挤占资源。

## 5. 每阶段完成定义

### T-A 验证基础设施

- `goal`: 建立可背书生产改造的 CI 护网与验证闭环
- `exit_criteria`: stress 测试进 CI 门禁（无 `-LE` 排除、无 `|| true`）；ASAN/TSAN 接入 per-PR；Release 性能基线建立（P99/阶段分解/对比 CoreCLR）；known-fail 数据库带 expiry
- `deliverables`: 修改的 CI 配置、ASAN/TSAN workflow、性能报告 `gc-baseline-release.md`、known-fail db schema
- `dependencies`: 无
- `resolved_decisions`: A1 用 LABEL 白名单而非黑名单排除；ASAN/TSAN 作为 per-PR 门禁（非仅 nightly）；性能基线对比 CoreCLR WKS
- `watch_items`: stress 测试移除 `|| true` 后暴露的真实失败数量（可能超出预期，需分类）

### T-B A2b A3 架构设计

- `goal`: 完成 A3 深度对齐的前置设计（修正后：Hybrid 软主路径 + 硬驱赶兜底）
- `exit_criteria`: CoreCLR WKS safepoint 机制研读（已定稿）；单 region 分配器接口/数据流设计；LEAF barrier 设计（mode switch 绑定）；codegen/JIT 接口契约；forbid_suspend 强化方案
- `deliverables`: `a3-safepoint-design.md`（已定稿，含跨平台验证）、`a3-allocator-design.md`、`a3-leaf-barrier-design.md`、`a3-contract-design.md`
- `dependencies`: 无（可读 CoreCLR 源码，不需 CI）
- `resolved_decisions`: 分配器统一为单 region owner；A3 采用 Hybrid **非**全线程物理挂起；根扫描在 rendezvous 做 **非**挂起态；store+barrier 一致性 by mode switch **非**原子指令对；forbid_suspend **保留并强化**为互斥护栏（不废弃；此点修正了原 B5 的废弃方向）
- `watch_items`: CoreCLR 源码细节复杂度；Apple/Android 纯软等待对 latency 的影响；Windows SuspendThread GetThreadContext 识别安全点位置的一致性风险；全局 trap 与现有 epoch 机制是否需统一

### P1 A2b A3 实现

- `goal`: A3 Hybrid 深度对齐落地，结构性消除 A2b
- `exit_criteria`: `gc_region_barrier_stress_test` 0/1000 挂起；ASAN/TSAN 满 CI 绿；统一分配器通过全部现有测试；LEAF barrier 通过 barrier 单元测试；三路写屏障契约统一
- `deliverables`: 重写后的 safepoint（Hybrid）、分配器、barrier、forbid 互斥护栏源码
- `dependencies`: T-A（CI 绿）、T-B（设计完成，5 件已齐）
- `resolved_decisions`: A3 Hybrid 主路径（软 + 硬驱赶兜底，**非**全线程物理挂起）；根扫描在 rendezvous 做；mode switch 保证 store+barrier；forbid_suspend 保留并强化（不废弃）
- `watch_items`: Hybrid 引入的性能开销；`pal_suspend.h` Windows `SuspendThread` GetThreadContext 识别安全点位置的一致性风险；全局 trap 与现有 epoch 机制是否需统一

### P2 残余并发 bug 修复

- `goal`: 消除所有已知并发正确性缺陷
- `exit_criteria`: L1 卡表恒定/RCU；BGC root-scan 根因定位修复；CrossPageCompact 提前退出；CodegenStSFld 三路统一；chaos_is_gc_pointer 盲点修复；全部并发测试绿
- `deliverables`: 各 bug 修复 commit
- `dependencies`: P1
- `resolved_decisions`: L1 卡表永不 realloc（恒定）或 RCU；BGC root-scan 根因定位非归档
- `watch_items`: 预置 ~30% 缓冲吸收未知并发 bug；ASAN/TSAN 新报错需分流（真 bug vs 工具墙）

### P3 诊断/配置完备

- `goal`: 诊断与配置达到对标级
- `exit_criteria`: ETW 事件 9→40+（BGC 阶段火点接入）；GCMemoryInfo BCL 类型；GCNotification 托管 API；配置 knobs 24→~75；BGC servo 闭环调速
- `deliverables`: 诊断模块、配置扩展、servo 控制
- `dependencies`: P2
- `resolved_decisions`: BGC 阶段事件补 fire 点；servo 闭环基于堆增长率调 mark budget
- `watch_items`: GCMemoryInfo/GCNotification 与 BCL 兼容性；servo 收敛稳定性

### P4 性能/平台对齐

- `goal`: 性能与平台覆盖达到对标级
- `exit_criteria`: Full GC 100MB ≤ CoreCLR 1.5x（Release）；Page decommission/FindPage 索引化；ARM64 全 CI 绿；Server GC 全量回归（per-PR）；跨平台诊断（EventPipe/Linux）；大页/CPU 亲和
- `deliverables`: 性能优化、平台适配、CI 矩阵扩展
- `dependencies`: P3
- `resolved_decisions`: 性能目标 1.5x（非 1.0x，避免过度优化）；ARM64 作为一等 CI 平台
- `watch_items`: 性能优化不要引入正确性回归（严格在 ASAN 护网下验证）；大页/亲和的可移植性

### P5 差异化优势

- `goal`: 固化并深化 CRAG 区别于外部 GC 的独特能力
- `exit_criteria`: MemoryDomain 热更新加固+文档；精确 GcLayout/寄存器根强化；NUMA 动态 heap count；域卸载 O(region) 成体系；GC 形式化验证可行性探针报告
- `deliverables`: 差异化能力、文档体系、探针报告
- `dependencies`: P4
- `resolved_decisions`: 热更新优势保持为显式目标；形式化验证作为可行性探针（spike），非承诺交付
- `watch_items`: MemoryDomain 加固与 A3 单分配器的一致性；探针是否暴露更深架构问题

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | est |
|---|---|---|---|---|---|---|---|---|---|---|---|
| T-A1 | T-A | planned | terminal-1 | Stress 测试 `-LE`→LABEL 白名单，移除 `\|\| true` | - | batch-1 | CI 门禁含 stress | ci-file | stress 测试阻断构建 | `tests/**`,`ci/*.yml` | 2-3天 |
| T-A2 | T-A | planned | terminal-1 | ASAN per-PR 门禁 | - | batch-1 | ASAN 入 CI | asan-workflow | asan 绿 | `ci/*.yml` | 1周 |
| T-A3 | T-A | planned | terminal-1 | TSAN 并发测试 CI | - | batch-1 | TSAN 入 CI | tsan-workflow | tsan 绿 | `ci/*.yml` | 1周 |
| T-A4 | T-A | planned | terminal-1 | Release 性能基线（P99/阶段/对比 CoreCLR） | - | batch-1 | 可复现基准 | `gc-baseline-release.md` | 基线落档 | `tests/**` | 1-2周 |
| T-A5 | T-A | planned | terminal-1 | known-fail 数据库带 expiry | - | batch-1 | expiry 机制 | known-fail-db | 过期自动剔除 | `tests/**` | 3天 |
| T-A6 | T-A | planned | terminal-1 | Cross-test 全域态污染检测 | - | batch-1 | 污染枚举+修 | 污染报告 | singleton 归零 | `tests/**` | 1周 |
| T-B1 | T-B | planned | terminal-1 | 读 CoreCLR WKS safepoint 源码 | - | batch-1 | 研读+摘要 | `a3-safepoint-design.md` | 机制总结 | `docs/**` | 1-2周 |
| T-B2 | T-B | planned | terminal-1 | 设计单 region 分配器 | - | batch-1 | 接口/数据流 | `a3-allocator-design.md` | 设计定稿 | `docs/**` | 1周 |
| T-B3 | T-B | planned | terminal-1 | 设计 LEAF 汇编写屏障（x64+ARM64） | - | batch-1 | 两套汇编设计 | `a3-leaf-barrier-design.md` | 设计定稿 | `docs/**` | 1周 |
| T-B4 | T-B | planned | terminal-1 | 设计 codegen/JIT 接口契约 | - | batch-1 | store+barrier 原子窗口 | `a3-contract-design.md` | 契约定稿 | `docs/**` | 1周 |
| T-B5 | T-B | planned | terminal-1 | 设计 forbid_suspend ack-and-continue 废弃方案 | - | batch-1 | 废弃路径 | `a3-contract-design.md` | 方案定稿 | `docs/**` | 3天 |
| G-P1 | P1 | planned | terminal-1 | A2b A3 实现（safepoint/分配器/barrier 重构） | T-A1,T-B1..B5 | batch-2 | ASAN 下实现 | gc 源码 | stress 0/1000 + asan 绿 | `src/**` | 4-6周 |
| G-P2 | P2 | planned | terminal-1 | 残余并发 bug 修复 | G-P1 | batch-2 | 全并发测试绿 | gc 源码 | P2 exit | `src/**` | 3-5周 |
| G-P3 | P3 | planned | terminal-1 | 诊断/配置完备 | G-P2 | batch-2 | ETW 40+/BCL/servo | diag 源码 | P3 exit | `src/**`,`runtime/**` | 3-4周 |
| G-P4 | P4 | planned | terminal-1 | 性能/平台对齐 | G-P3 | batch-2 | 1.5x + ARM64 | perf 源码 | P4 exit | `src/**`,`ci/*.yml` | 4-6周 |
| G-P5 | P5 | planned | terminal-1 | 差异化优势 | G-P4 | batch-2 | MemoryDomain 等 | 差异化 | P5 exit | `src/**`,`docs/**` | 3-4周 |

## 7. 依赖

- **T-A / T-B 互相独立**，可并行（batch-1）
- **P1 依赖 T-A 绿 + T-B 设计完成**（gate）
- **P2 → P3 → P4 → P5 串行**，各自顺序依赖

## 8. 风险

| 风险 | 触发 | 后果 | 缓解 |
|:----|:-----|:----:|:-----|
| A3 实现引入新并发回归 | T-A 未就绪即改 | 回归不可见 | 严格 gate：T-A 绿后合入 G-P1 |
| 单人长期执行疲劳 | 6+ 月 | 质量下降 | 月粒度阶段，每阶段明确完成信号 |
| CoreCLR 源码过深 | 读不完 | 设计延迟 | 只读 safepoint/barrier/分配器核心路径 |
| 双轨切换认知开销 | 每日切换 | 效率降 | 固定节奏：上午 Track A/下午 Track B |
| 未知并发 bug | ASAN 满 CI | 计划外 | P2 预置 30% 缓冲 |
| 硬 STW 性能超标 | Physical Suspend 开销 | 性能不达标 | P1 中对比基线，超预算回退 handshake 混合 |
| 热更新 MemoryDomain 退化 | 长期无维护 | 差异化丧失 | 每阶段 1 个小维护 commit |

## 9. 备选路径

- **P1 硬 STW 性能超标** → 回退为「硬 STW 仅屏障关键段 + handshake 主路径」混合（A1 深度，参见上节深度分级），仍消除主要 A2b 窗口，接受残余极窄窗口
- **T-B 设计发现 MemoryDomain 无法直接套 CoreCLR** → MemoryDomain 相关 barrier 用 CRAG 自有方案，不影响全局单分配器
- **ASAN/TSAN 工具墙阻塞** → 沿用已知的隔离目录 + 抑制清单策略（记忆中有 ASan 工具墙破除先例）

## 10. 当前建议推进顺序

1. **✅ batch-1 已完成**（T-A1/A2/A3 + T-B1..B5；T-A4 待真实 CI 捕获，T-A5/A6 deferred）
2. **⏸️ CHECKPOINT-1**：真实 CI 验证清单全绿后启动 G-P1（见 STATUS「G-P1 启动前置」）
   - T-A3 Linux TSAN 首绿（最大风险：Linux GC 单测首次 CI 验证）
   - T-A2 ASAN per-PR 首跑
   - T-A1 stress 门禁首跑
   - T-A4 性能基线首捕（手动 dispatch）
   - 现有 Windows GC 门禁回归
3. **P1（G-P1）**：A3 Hybrid 实现，4 域 Workflow 委托
4. 之后 P2 → P3 → P4 → P5 串行

## 11. 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- **继承上下文**：batch-1 各子任务边界/接口已在本文档 T-A/T-B 的每阶段完成定义中明确 → 子任务可用 `lightweight-brainstorm` + `dev-writing-plans`，不重复全流程
- **preflight self-review**：自动启动前执行最小 preflight（边界一致性 / authority / 依赖满足 / 风险）

## 12. 调度策略

见 `DISPATCH.md`。采用 `hybrid`：batch-1 并行（11 子任务独立无文件冲突）、batch-2 串行（G-P1..G-P5 强依赖）。
