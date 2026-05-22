# Threading 子系统深度分析报告

> 日期: 2026-05-23
> 分析范围: `src/native/runtime-core/` threading 源码 ~3,950 行 + 测试 ~2,580 行 + 托管端 foundation-dll 测试

---

## 总体评估

| 维度 | 评分 | 说明 |
|------|------|------|
| **实现完整性** | ~72% | 骨架完整，精细度有差距 |
| **代码质量** | 7.5/10 | 整体良好，存在若干 bug 和设计缺陷 |
| **测试覆盖** | 6.5/10 | 基础功能有覆盖，压力/边界/并发安全不够 |
| **生产就绪度** | ⚠️ 不可直接用于生产 | 存在正确性问题 |
| **与已有评估差异** | 显著 | wiki 评估部分过时/不准确，实际代码已有改进 |

## 已实现但 wiki 评估中标记为缺失的改进

| 项目 | wiki 说 | 实际代码 | 文件:行 |
|------|---------|---------|---------|
| **ForbidSuspendScope** | "缺失" | 已实现，完整 RAII guard + 嵌套深度 + debug 告警 | `forbid_suspend.h:41-81` |
| **ReaderWriterLockSlim 重构** | "std::mutex 封装" | Interlocked 实现 — 读路径 InterlockedIncrement，CAS 写锁 | `synchronization.cpp:32-583` |
| **Monitor 链式 PulseAll** | "可能唤醒风暴" | 链式唤醒 — pulse_count + notify_one 接力 | `monitor.cpp:298-313` |
| **ExecutionContext 动态 slot** | "4 slot 硬限制" | 动态映射 — 4 slot inline + heap 扩容至 64，SuppressFlow 完整 | `execution_context.cpp:31-66` |
| **HillClimbing V2** | "5-state, 均值比较" | 9-state + Goertzel 滤波器 + SigmoidGain | `thread_pool.cpp:309-597` |
| **Gate thread tick** | "500ms tick" | 15ms tick（代码实际值） | `thread_pool.cpp:285` |

## 代码审查发现的关键问题

### P0 — 正确性风险

#### 1. ExecutionContextRun 中 EnsureHeapCapacity 失败后继续执行
- 文件: `execution_context.cpp:219-231`
- EnsureHeapCapacity 返回值被忽略，heep 扩容失败时 ValueAt 访问越界内存
- 触发条件: 线程上有 5+ AsyncLocal 值的情况下发生 EC flow

#### 2. HillClimbing CPU 利用率基于虚假启发式
- 文件: `thread_pool.cpp:340-341`
- `busy_ratio = min(1.0f, completed_count * 0.01f)` — 完全虚构
- 导致 Goertzel CPU filter 和 Saturating 状态决策不可靠

### P1 — 健壮性风险

#### 3. Semaphore/Barrier/CDE O(n) 线性扫描
- 文件: `synchronization.cpp:89-94`
- 批量创建同步原语时性能退化

#### 4. 全局队列无限增长
- 文件: `thread_pool.cpp:37`
- 生产者远快于消费者时 OOM

#### 5. ExitRead/ExitWrite 无所有权验证
- 文件: `synchronization.cpp:313`
- 任何线程可解别人的锁

#### 6. WorkerLoop shutdown 竞态
- 文件: `thread_pool.cpp:172-213`
- shutdown 标志在 wait 返回后、lock 获取前设置会导致延迟退出

### P2 — 工业化差距

#### 7. ThreadPool 注入速率无控制
- 文件: `thread_pool.cpp:704-709`
- 并发入队造成 worker 创建风暴

#### 8. TimerQueue vector 线性扫描
- 文件: `timer_queue.cpp`
- O(n) vs 理想 O(log n)

#### 9. 测试覆盖缺口
- 空闲回收、GC safepoint 协作、8+线程 Monitor 压力、EC 5+ slot、ForbidSuspend+GC、TimerQueue 精度均无自动化测试

#### 10. Wiki 文档严重过时
- `99-threading-assessment.md` 多个结论与代码不符

## 改进路线

详见 STATUS.md：4 个 Phase 严格串行，TDD 先行。
