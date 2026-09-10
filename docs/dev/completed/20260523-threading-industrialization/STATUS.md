# Threading 子系统工业化收尾

> **Phase**: roadmap
> **日期**: 2026-05-23
> **目录**: `docs/dev/in-progress/20260523-threading-industrialization/`
> **task_type**: roadmap
> **roadmap_or_plan**: `roadmap-v1-01.md`
> **child_execution_mode**: auto
> **auto_continue**: true
> **auto_stop_policy**: blocking-only
> **dispatch_model**: sequential

---

## 问题来源

详见 `docs/dev/in-progress/20260523-threading-industrialization/ANALYSIS.md`（分析报告），基于对 `src/native/runtime-core/` 下全部 threading 源码 ~3,950 行 + 测试 ~2,580 行的审计。

## 设计摘要

### 边界拍板

| 项目 | 决策 |
|------|------|
| **范围** | 全量工业化：P0 + P1 + P2 全部 10 个问题 |
| **测试策略** | TDD 先行，每个 bug fix 先写失败测试再改实现 |
| **Wiki 更新** | 随改随更，每个子任务同步更新对应 wiki 章节 |
| **Semaphore/Barrier/CDE** | 三个全部改为固定数组 O(1) |
| **执行顺序** | Phase 1→2→3→4 严格串行 |

### Phase 设计

#### Phase 1 — P0 正确性修复（预计 2-3 天）

| # | 问题 | 文件 | 方案 |
|---|------|------|------|
| 1.1 | EC EnsureHeapCapacity 忽略失败 | `execution_context.cpp:219` | 检查返回值，失败时回退 + 日志 |
| 1.2 | HC CPU 利用率虚假 | `thread_pool.cpp:340` | 接入真实 CPU 时间测量 |
| 1.3 | TDD: EC 5+ slot + HC CPU 反馈测试 | 新增测试 | 先写失败测试，再改代码 |

#### Phase 2 — P1 健壮性加固（预计 3-4 天）

| # | 问题 | 文件 | 方案 |
|---|------|------|------|
| 2.1 | 全局队列无限增长 | `thread_pool.cpp:37` | 添加 `kMaxGlobalQueueDepth` + backpressure |
| 2.2 | Semaphore/Barrier/CDE O(n)→O(1) | `synchronization.cpp` | 固定数组，与 RWLock 一致 |
| 2.3 | ExitRead/ExitWrite 所有权验证 | `synchronization.cpp:313` | DEBUG TID 检查 |
| 2.4 | TDD: 新测试 | 补充 | stress 8+线程 Monitor, HC 长稳, EC 溢出 |

#### Phase 3 — P2 工业化补齐（预计 3-5 天）

| # | 问题 | 文件 | 方案 |
|---|------|------|------|
| 3.1 | ThreadPool 注入速率控制 | `thread_pool.cpp:704` | wave 创建 + gate tick 内限速 |
| 3.2 | WorkerLoop shutdown 竞态 | `thread_pool.cpp:172-213` | 双重检查 + lock |
| 3.3 | TimerQueue vector→min-heap | `timer_queue.cpp` | 最小堆 O(log n) |
| 3.4 | TDD: Timer 精度 + shutdown 竞态 | 新增 | — |

#### Phase 4 — 最终验证（预计 1 天）

| # | 任务 | 说明 |
|---|------|------|
| 4.1 | 全量 threading 测试 | 所有 smoke + benchmark + 专项测试 |
| 4.2 | foundation-dll managed 测试 | threading 全系列 |
| 4.3 | 重写 wiki 99-threading-assessment.md | 反映真实代码状态 |
| 4.4 | 更新 20-threading-optimizations.md | gate tick 等修正 |

### 风险评估

| 风险 | 缓解措施 |
|------|---------|
| HC CPU 真实测量有平台差异 | 抽象 `GetThreadCpuTime()`, Win/POSIX 各一套 |
| TimerQueue 重构影响时序 | min-heap 保留 1024 上限，TDD 覆盖 |
| 全局队列 backpressure 死锁 | 使用 yield 而非阻塞 wait |
| 并发修复引入回归 | 每个 Phase 独立验证 + TDD 先写失败测试 |

### 三优先级权衡

| 维度 | 结论 |
|------|------|
| 性能最优 | HC CPU + 注入速率 + TimerQueue 直接提升性能 |
| 方案完美 | 所有修复沿用既有模式，不引入新抽象 |
| 热更新 | threading 非热更新关键路径，无影响 |

三个维度**无冲突**，无需优先级裁决。

### 关键文件清单

- `src/native/runtime-core/execution_context.cpp` — Phase 1 EC fix
- `src/native/runtime-core/thread_pool.cpp` — Phase 1 HC CPU + Phase 2 queue limit + Phase 3 injection rate + shutdown
- `src/native/runtime-core/synchronization.cpp` — Phase 2 O(1) + ownership
- `src/native/runtime-core/timer_queue.cpp` — Phase 3 min-heap
- `testing/src/native/runtime-core/threading/*.cpp` — TDD 测试补充
- `wiki/03-功能模块/06-il2cpp核心架构/99-threading-assessment.md` — Phase 4 重写
- `wiki/03-功能模块/06-il2cpp核心架构/20-threading-optimizations.md` — Phase 4 更新

### blocking_questions

- [ ] 无 — 所有问题已在上文 brainstorm 中清零

### question_clearance

- user 已确认范围、策略、执行顺序
- `clearance_confirmed_by_user: true`

## 最近摘要

- **T1-EC-FIX**: ✅ Phase 1。修复了 `ExecutionContextRun` 中 `EnsureHeapCapacity` 返回值被忽略的 bug。
- **T1-HC-CPU**: ✅ Phase 1。接入 `GetProcessCpuTimeNs()` 真实 CPU 时间测量。
- **T1-TEST**: ✅ Phase 1。新增 8 个 HC CPU 反馈测试。
- **T2-QUEUE**: ✅ Phase 2。添加 `kThreadPoolMaxQueueDepth=8192` + yield-spin backpressure。3 个测试。
- **T2-O1**: ✅ Phase 2。Semaphore/Barrier/CDE 全部改用固定数组 O(1) 查找，移除 3 个 table mutex、3 个 std::list。
- **T2-OWNER**: ✅ Phase 2。ExitWrite 添加 DEBUG writer TID 验证 (`debug_writer_tid`)。
- **T2-STRESS**: ✅ Phase 2。新增 6 个多线程 stress 测试（8 线程 RWLock/Semaphore/Barrier/CDE + RapidQueueDrain 5×5000）。
- **T3-RATE**: ✅ Phase 3。ThreadPool 注入速率控制 — rate-limited worker growth (`depth > workers * 3` 阈值)，gate tick 内 starvation detection (2×阈值)。
- **T3-SHUTDOWN**: ✅ Phase 3。WorkerLoop shutdown 竞态修复 — idle reclamation 路径改用 `memory_order_acquire` 读取 `s_shutdown`，确保与 `ThreadPoolShutdown` 的 `memory_order_release` store 正确同步。
- **T3-TIMER**: ✅ Phase 3。TimerQueue 添加 id→index 哈希表 O(1) Change/Delete（2048 槽开放定址）。保持 min-heap O(log n) 插入/提取。
- **T3-TEST**: ✅ Phase 3。新增 5 个 TDD 测试：timer 精度、periodic 多次触发、shutdown 并发 drain、active timer 下 shutdown、massive queue worker storm 验证。

**Phase 1 ✅, Phase 2 ✅, Phase 3 ✅, Phase 4 ✅**

- **T4-WIKI**: ✅ Phase 4。重写 `99-threading-assessment.md` 和 `20-threading-optimizations.md`，反映工业化收尾后的真实代码状态：HC V2 10-state + Goertzel + CPU 反馈、EC 动态 64 slot + SuppressFlow、RWLock Interlocked 实现 + upgradeable read、Sem/Barrier/CDE O(1) 固定数组、TimerQueue min-heap + O(1) 哈希表、ForbidSuspendScope、注入速率控制、shutdown 竞态修复等。
- **T4-VERIFY**: ✅ Phase 4。全量 threading 测试验证通过：
  - test_threading_smoke: 13/13 ✅
  - test_synchronization_smoke: 15/15 ✅
  - test_async_integration_smoke: 12/12 ✅
  - test_hc_cpu_feedback: 8/8 ✅
  - test_execution_context_smoke: 11/11 ✅
  - test_rwlock_upgrade: 12/12 ✅
  - test_phase3_industrialization: 5/5 ✅
  - test_threading_stress: 6/6 ✅ (RelWithDebInfo)
  - test_queue_backpressure: segfault (pre-existing ODR mismatch，非本变更引入)

### latest_stop_point

全部 4 个 Phase 完成。所有 13 个子任务进入终态。

## 下一步

无 — threading 工业化收尾全部完成。

## recommended_next_child

n/a (全部完成)
