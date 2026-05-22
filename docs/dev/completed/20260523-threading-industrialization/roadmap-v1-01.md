# Threading 子系统工业化收尾 — Roadmap

> **日期**: 2026-05-23
> **父任务**: `docs/dev/in-progress/20260523-threading-industrialization/`
> **设计输入**: `STATUS.md`（设计摘要）、`ANALYSIS.md`（分析报告）
> **brainstorm 结论**: `blocking_questions: []`, `question_clearance: cleared`, `clearance_confirmed_by_user: true`

---

## 1. 目标

将 threading 子系统的 10 个已知问题（P0×2, P1×3, P2×3, 测试补全, wiki 重写）全部修复，达到 CoreCLR 对等的工业化水平。

## 2. 范围边界

| 项目 | 边界 |
|------|------|
| **范围内** | `src/native/runtime-core/` 下 threading 源码修复、TDD 测试补充、wiki 同步更新 |
| **范围外** | 新增 managed 端 threading 特性（Task<T>, ValueTask, async/await 状态机等）；`src/managed/` 代码修改；跨进程 WaitHandle；NUMA 亲和性 |
| **不改的架构** | GC Worker Pool 和 ETW 事件不改（已在 P3 评估为低风险） |

## 3. 非目标

- 不引入新的跨平台 abstraction layer
- 不改动 GC safepoint 协议的核心设计（generation-based handshake 设计合理）
- 不改动 Monitor thin lock/inflation 的核心路径
- 不新增 managed 端 C# API

## 4. 阶段列表

### Phase 1: P0 正确性修复

| 字段 | 内容 |
|------|------|
| **goal** | 修复 EC heap 越界和 HC CPU 虚假两个正确性问题 |
| **exit_criteria** | EC 5+ slot 测试通过；HC CPU 真实测量接入；现有全量 threading 测试通过 |
| **deliverables** | `execution_context.cpp` fix + `thread_pool.cpp` fix + 新增/补充测试 |
| **dependencies** | 无（起点） |
| **resolved_decisions** | HC CPU 测量通过 `GetThreadCpuTime()` 抽象接口，Win/POSIX 各一套 |
| **watch_items** | GetThreadCpuTime 的平台兼容性 |

### Phase 2: P1 健壮性加固

| 字段 | 内容 |
|------|------|
| **goal** | 修复全局队列 OOM、同步原语 O(n) 查找、所有权验证缺失 |
| **exit_criteria** | 所有 O(1) 查找测试通过；queue backpressure 验证通过；全量测试通过 |
| **deliverables** | `thread_pool.cpp` fix + `synchronization.cpp` 重构 + 新增 stress 测试 |
| **dependencies** | Phase 1 completed |
| **resolved_decisions** | Semaphore/Barrier/CDE 三个全部改为固定数组 |
| **watch_items** | O(1) 重构是否会遗漏边界条件（已有 RWLock 模式可参考） |

### Phase 3: P2 工业化补齐

| 字段 | 内容 |
|------|------|
| **goal** | ThreadPool 注入速率控制、shutdown 竞态修复、TimerQueue min-heap |
| **exit_criteria** | 新测试通过；shutdown 竞态测试通过；Timer 精度测试通过 |
| **deliverables** | `thread_pool.cpp` fix + `timer_queue.cpp` 重构 + 新增测试 |
| **dependencies** | Phase 2 completed |
| **resolved_decisions** | TimerQueue 保留 1024 上限；注入速率使用 gate tick 内限速 |
| **watch_items** | 注入速率控制与现有 HillClimbing 是否协调 |

### Phase 4: 最终验证

| 字段 | 内容 |
|------|------|
| **goal** | 全量测试 + wiki 重写 + 最终验证 pass |
| **exit_criteria** | 所有 threading 测试 pass；foundation-dll managed 测试 pass；wiki 更新完成 |
| **deliverables** | 更新后的 wiki 文档 |
| **dependencies** | Phase 3 completed |
| **resolved_decisions** | wiki 随改随更，Phase 4 做最终一致性检查 |
| **watch_items** | 无 |

## 5. 子任务映射

| task_id | phase | purpose | depends_on | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|---------|------------|-------------|-------------|---------------|----------------|-----------------|
| T1-EC-FIX | 1 | 修复 EC EnsureHeapCapacity 忽略失败 | 无 | TDD 先写 5+ slot 越界测试，再改代码 | `execution_context.cpp` + 测试 | EC 5+ slot 用例通过，无 segfault | `src/native/runtime-core/execution_context.cpp` | 0.5d |
| T1-HC-CPU | 1 | ✅ completed | HC CPU 利用率真实测量 | 实现 GetThreadCpuTime 抽象 + 替换 heuristic | `thread_pool.cpp` + 测试 | CPU 利用率不再是 fake heuristic | `src/native/runtime-core/thread_pool.cpp` | 1-1.5d |
| T1-TEST | 1 | ✅ completed | 补 Phase 1 TDD 测试 | T1-EC-FIX, T1-HC-CPU | EC 溢出 + HC CPU 反馈 + 回归 | 测试文件 | 所有 Phase 1 测试通过 | `testing/src/native/runtime-core/threading/` | 0.5d |
| T2-QUEUE | 2 | ✅ completed | 全局队列添加 backpressure | kMaxGlobalQueueDepth + yield backpressure | `thread_pool.cpp` + 测试 | 队列超限时正确 backpressure | `src/native/runtime-core/thread_pool.cpp` | 0.5d |
| T2-O1 | 2 | ✅ completed | Semaphore/Barrier/CDE O(1) | 三个全部改为固定数组 | `synchronization.cpp` + 测试 | O(1) 查找验证 + 回归 | `src/native/runtime-core/synchronization.cpp` | 1-1.5d |
| T2-OWNER | 2 | ✅ completed | ExitRead/ExitWrite 所有权验证 | DEBUG 下 TID 检查 | `synchronization.cpp` + 测试 | DEBUG 下非法 exit 触发 assert | `src/native/runtime-core/synchronization.cpp` | 0.5d |
| T2-STRESS | 2 | ✅ completed | 补充压力测试 | 8+ 线程 Monitor stress, HC 长稳, EC 溢出 | 测试文件 | 压力测试 30s 无崩溃 | `testing/src/native/runtime-core/threading/` | 1d |
| T3-RATE | 3 | ✅ completed | ThreadPool 注入速率控制 | Phase 2 | wave 创建 + gate tick 内限速 | `thread_pool.cpp` + 测试 | 批量入队不造成 worker 风暴 | `src/native/runtime-core/thread_pool.cpp` | 1-1.5d |
| T3-SHUTDOWN | 3 | ✅ completed | WorkerLoop shutdown 竞态 | Phase 2 | wait 后双重检查 | `thread_pool.cpp` + 测试 | shutdown 竞态测试通过 | `src/native/runtime-core/thread_pool.cpp` | 0.5d |
| T3-TIMER | 3 | ✅ completed | TimerQueue min-heap + O(1) lookup | Phase 2 | vector→min-heap, id→index 哈希表 | `timer_queue.cpp` + 测试 | O(log n) 验证 + 精度测试 | `src/native/runtime-core/timer_queue.cpp` | 1-1.5d |
| T3-TEST | 3 | ✅ completed | 补 Phase 3 TDD 测试 | T3-RATE, T3-SHUTDOWN, T3-TIMER | Timer 精度 + shutdown 竞态 + 注入速率 | 测试文件 | 所有新测试通过 | `testing/src/native/runtime-core/threading/` | 0.5d |
| T4-VERIFY | 4 | ✅ completed | 全量验证 | Phase 3 | 全量 threading 测试 + foundation-dll | 验证报告 | 所有测试 pass | `testing/` | 0.5d |
| T4-WIKI | 4 | ✅ completed | wiki 重写 | Phase 3 | 重写 99-threading-assessment.md, 更新 20-threading-optimizations.md | wiki 文档 | wiki 与代码状态一致 | `wiki/` | 0.5d |

## 6. 依赖关系

```
Phase 1: T1-EC-FIX → T1-HC-CPU (并行) → T1-TEST
                ↓
Phase 2: T2-QUEUE, T2-O1, T2-OWNER (并行) → T2-STRESS
                ↓
Phase 3: T3-RATE, T3-SHUTDOWN, T3-TIMER (并行) → T3-TEST
                ↓
Phase 4: T4-VERIFY, T4-WIKI (并行)
```

## 7. 风险

| 风险 | 可能性 | 影响 | 缓解 |
|------|--------|------|------|
| GetThreadCpuTime 平台差异 | 低 | 中 | 抽象接口，Win/POSIX 隔离；iOS/macOS 支持 |
| Semaphore O(1) 重构遗漏边界 | 低 | 中 | 已有 RWLock 固定数组模式可参考，TDD 覆盖 |
| 注入速率与 HillClimbing 冲突 | 低 | 低 | HC 的 suggestion 最终决定 worker 数，注入速率只控制创建节奏 |
| 现有测试回归 | 中 | 中 | 每个 Phase 完成后全量运行 baseline 测试 |

## 8. 备选路径

| 场景 | 备选 |
|------|------|
| HC CPU 真实测量在某个平台不可用 | 降级回 heuristic + LOG_WARN 告警，不影响其他修复 |
| TimerQueue min-heap 引入 bug | 保留 vector fallback 开关，使用 ifdef 或编译期常量切换 |

## 9. 当前建议推进顺序

严格按 Phase 1 → 2 → 3 → 4 串行推进。

## 10. 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- `dispatch_model: sequential`
- `recommended_next_child`: T3-RATE
