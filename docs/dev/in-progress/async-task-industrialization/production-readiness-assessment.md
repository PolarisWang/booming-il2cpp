# System.Threading.Tasks 生产机品质评估报告

> 评估日期：2026-09-13 | 评估基线：`6afb108fe` (main) | codegen 2252/2252

## 一、结论摘要

**当前状态：Phase 1-3 核心语义生产就绪，BCL 覆盖面存在系统性缺口（65% 调用点仍走解释器），距离完整生产机品质还需约 3-4 个月。**

| 评估维度 | 状态 | 简要证据 |
|----------|------|----------|
| 核心语义（Task.Run/Wait/异常/三态） | ✅ **生产就绪** | 4/4 反例通过，in-place revert 反证 |
| 组合子（WhenAll/Any/Each/ContinueWith） | ✅ **生产就绪** | native 实现 + 反例测试 |
| async 状态机（async/await） | ✅ **生产就绪** | A1-A5 全部落地，2252/2252 |
| async 迭代器（IAsyncEnumerable） | ✅ **可用** | YieldOne 端到端 lowering |
| 取消/上下文（CTS/AsyncLocal/EC/SyncCtx） | 🟡 **接线完成** | Phase 3 注册齐全，未压测 |
| **Task BCL 完整覆盖面** | 🔴 **系统性缺口** | **3015 调用点仍走解释器（65%）** |
| 性能（AOT vs .NET 8） | 🟡 **单 chunk −80%，全量未知** | |
| hotupdate 方法替换 | 🔴 **未验证** | |

---

## 二、已完成部分（可按生产使用）

### 2.1 核心执行语义（Phase 1）

所有 4 项设计验收通过，每项均做过 in-place revert 反证：

| 验收 | 说明 | 反证方式 |
|------|------|---------|
| Task.Run 端到端 | 委托真的在线程池上执行 | 注释掉 `chaos_delegate_object_invoke` → 3 FAIL |
| Wait 阻塞语义 | 阻塞 ≥ 任务时长，不得自旋占核 | 恢复 yield() 自旋 → 157ms CPU/150ms 等待 → FAIL |
| 异常传播 | faulted 任务 Wait()/Result 必须抛 | 恢复 Wait 静默 return 1 → `PayloadLessFaultRaisesFromWaitToo` FAIL |
| 三态区分 | cancelled/faulted/resolved 互斥 | 恢复 `canceled := faulted&&!exception` → 3 FAIL |

### 2.2 组合子（Phase 2）

| API | 实现 | 反例测试 |
|-----|------|---------|
| Task.WhenAll (Task[]) | native aggregate (WhenAllAnyInternal) | 结果集 [1,2,3] + 异步完成 |
| Task.WhenAny (Task[]) | native aggregate | winner index 正确 |
| Task.WhenAll\<TResult\> | 泛型重载通过 array combinator | 移除注册 → 管 FAIL |
| Task.WhenEach | completion-ordered FIFO | completion-order ≠ array-order gtest |
| Task.ContinueWith | native continuation | 不调用委托 → 9 FAIL |
| Task.Factory.StartNew | delegate 版走 ThreadPool | 移除注册 → 管线 FAIL |
| ContinueWhenAll/Any | 组合 aggregate + ContinueWith | 3 tasks, exactly 1 延续触发 |
| Task.Delay (int) | TimerQueue 延迟 | 对应反例通过 |
| TaskCompletionSource | native set_result/exception/cancel | 0-arg ABI 槽匹配缺陷已修 |

### 2.3 async 状态机（A1-A5）

| 层次 | 内容 | 提交 |
|------|------|------|
| A1 | 显式检测 + 诊断（记录而非静默 stub） | `bd77bcd73` |
| A2 | native `AsyncIteratorSourceCore` + 池化 `ValueTask<bool>` | `624c3682f` |
| A3 | codegen 5-op 注册（Create/MoveNext/AwaitOnCompleted/Complete） | `4fdf4a4b9` |
| A4 | await-free 迭代器 lowering + 完成信号 native 接线 | `d0b85bce8`, `bf1d94138` |
| A5 | awaited 迭代器门控解除 | `3e5ac9ef2` |

### 2.4 取消与上下文（Phase 3）

| 组件 | 类型 | 内容 |
|------|------|------|
| CancellationToken | codegen 注册 | IsCancellationRequested / CanBeCanceled / ThrowIfCancellationRequested |
| CancellationTokenSource | codegen 注册 | Create / Cancel / Dispose / get_Token |
| AsyncLocal\<T\> | native + codegen | thread-local set/get |
| ExecutionContext | codegen 注册 | SuppressFlow / RestoreFlow / IsFlowSuppressed |
| SynchronizationContext | native + codegen | thread-local Current, Post/Send placeholder |

### 2.5 ValueTask / 扩展（Phase 4）

| API | 状态 |
|-----|------|
| ValueTask state queries (4) | ✅ |
| ValueTask.AsTask | ✅ |
| ValueTask.GetAwaiter | ✅ |
| TaskExtensions.Unwrap | ✅ |
| TaskToAsyncResult | ❌ APM 模式，AOT 不可能 |
| ConcurrentExclusiveSchedulerPair | ❌ 自定义 TaskScheduler，AOT 不可能 |

### 2.6 性能优化（Phase 6，部分）

**实测验证（Parallel chunk, 107 benchmark 方法）：**

| 指标 | 优化前 | 优化后 | 变化 |
|------|--------|--------|------|
| Hot 方法数 (>10μs) | 8 | 4 | **−50%** |
| Hot 总耗时 | 1127μs | **215μs** | **−81%** |
| 全 chunk 总耗时 | ~1175μs | **233μs** | **−80%** |

**分项收益：**
- E3 (`IDisposable::Dispose` → no-op)：40μs → <1μs
- E1 (委托 .ctor 原生化)：31–62μs → <1μs
- E2 (`Parallel.For` 原生分区)：585μs (52%) → 不可测量
- B (eval_stack 按需分配)：每个方法 −~10KB 零初始化
- GC ownership (`CHAOS_IL2CPP_NEW_GC`)：AsyncTask 对 GC 可见

---

## 三、未完成部分（🔴 系统性缺口）

### 3.1 核心问题：BCL 覆盖面

`threading-tasks` chunk 的 native 构建失败暴露了 **3015 个走解释器的调用点**：

| 分类 | 调用点数 | 占比 | 性质 |
|------|---------|------|------|
| 测试夹具 (ATG 生成) | 968 | 32% | 不影响运行时性能，但阻碍 chunk 构建 |
| **Task 家族（影响运行）** | **1965** | **65%** | **真实 lowering 缺口** |
| 其他 BCL | 82 | 3% | 零散的简单 API |

**Task 家族缺口分解（TOP 10）：**

| 缺口 | 调用点数 | 分析 |
|------|---------|------|
| `Task::GetAwaiter` + 泛型变体 | 177+138 | 已有注册但 callee 拼写走外部助手路径 → resolver 返回 null |
| `TaskAwaiter::GetResult` 变体 | 180+136 | 同上 |
| `Task::ContinueWith` 额外重载 | 196 | 已有 `chaos_task_continue_with`，需要增加 arity 匹配 |
| `Task::WaitAll` / `Task::WaitAny` | 163 | 走解释器（~1μs/调用 × 多次迭代）|
| `TaskFactory::StartNew` 额外变体 | 135 | 已有注册仅覆盖 delegate-only 重载 |
| `Task::Run` 额外变体 | 56 | CancellationToken 变体未覆盖 |
| `Task::Delay` 额外变体 | 46 | TimeSpan/CancellationToken 变体未覆盖 |
| `Task::WhenAny` 变体 | 46 | IEnumerable 重载未覆盖 |
| `TaskToAsyncResult` | 12 | APM 模式，AOT 不可能实现 |
| `TaskCompletionSource` 额外变体 | 34 | CancellationToken 参数变体 |

### 3.2 根本原因

**这不是"还有 X 个 API 没注册" —— 这是注册策略的分水岭问题：** 现有的 `Register()` + `GenericShapeDescriptor` 模式只覆盖了最简单的直通调用（SimpleForward）。当 IL 的 callee 拼写带有复杂的泛型参数嵌套（`Task`1[[System.Int32]]` 等）时，resolver 返回 null → 退化为外部运行时助手 → `chaos_external_runtime_*` → C3861。

**3.2.1 现有注册表 vs 实际 BCL 面的差距**

| 维度 | 现有 | 需要 |
|------|------|------|
| 注册条目数 | 86 | ~320+ |
| 覆盖率 | ~27% | 100% |
| 泛型变体处理 | 有但遗漏了 1 个 backtick | 完整覆盖 `Task\`1` / `TaskAwaiter\`1` 等 |

**3.2.2 架构限制**

- `TaskToAsyncResult`（APM Begin/End 模式）—— 此 AOT 运行时没有 `IAsyncResult` 委托泵，**不可能实现**
- `TaskFactory::ContinueWhenAll/Any` 的 IEnumerable 重载 —— 需要任意长数组的 partitioner
- `Task::WaitAll/Any` 的 CancellationToken/TimeSpan 重载 —— 每个变体需独立注册

### 3.3 测试覆盖缺口

| 维度 | 数量 | 评估 |
|------|------|------|
| codegen 单测 | 2252 (全部通过) | ✅ 充分 |
| native threading gtest | 252 | ✅ 充分 |
| ATG 生成事实测试 | 0（未跑） | 🔴 **全缺** |
| BCL 面 end-to-end 测试 | 0 | 🔴 **全缺** |

---

## 四、生产机品质差距量化

| # | 项 | 完成度 | 剩余工作量 | 优先级 |
|---|----|--------|-----------|--------|
| G1 | `threading-tasks` chunk 构建通过 | ~40% | 需注册全部 212 个缺失符号（~3 轮迭代） | **P0** |
| G2 | BCL 面注册补齐 | ~27% | ~320+ 条目，2154 个调用点消除；约需 4-6 轮 | **P0** |
| G3 | foundation-dll fact 覆盖 threading-tasks | 0% | ~50-100 个事实测试，需先解决 G1 | **P1** |
| G4 | GC stress (100k Task, ASan/Valgrind) | 🟡 测试已写 | 需在 asan preset 下运行并确认 | P2 |
| G5 | Performance: AOT < 2× .NET 8 | 🟡 Parallel chunk −80% | 需全量 benchmark + 针对性优化 | P2 |
| G6 | hotupdate 状态机方法替换 | 0% | slot mapping 未决（独立域） | P3 |
| G7 | Parallel 107/107 fact 清理 | 0% | ATG 层面 | P3 |

**乐观估计剩余工期：3-4 个月（按每人 50% 投入算）。**

---

## 五、风险

| 风险 | 等级 | 说明 |
|------|------|------|
| BCL 注册迭代发散 | 🔴 高 | `threading-tasks` chunk 的依赖面逐层暴露（每轮修 18 个，暴露下一层 18 个），不是"补完这堆就好" |
| TaskToAsyncResult 永久不可能 | 已确认 | APM 模式无 AOT 等价物，需在 registry 层显式拒绝 |
| hotupdate slot mapping | 🟡 中 | 热替换状态机方法时 slot 偏移可能漂移，需 framework 层验证 |
| benchmark 平台缺失 | 🟡 中 | windows-x64-reference 预设依赖 MSVC vcvars，在 CI 外不可靠 |
| 迭代器 `ConfiguredValueTaskAwaiter` 缺口 | 🟡 低 | 现有 fixture 用 `YieldAwaiter` 绕过了，WhenEach 真实场景下会暴露 |

---

## 六、建议路径

### 短期（1-2 周）

1. **全量注册缺失符号** —— 用 `GenericShapeDescriptor` 覆盖所有 `Task\`1` / `TaskFactory` / `TaskCompletionSource` 的泛型变体
2. **拿到 `threading-tasks` chunk 的完整 benchmark** —— 消除 C++ 构建错误后跑 `--benchmark-all`
3. **运行 foundation-dll fact 测试** —— 至少覆盖 threading-tasks family 的已注册 surface

### 中期（3-8 周）

4. **Task.WaitAll/Any 超时变体原生实现** —— 扩展 `WhenAllAnyInternal`
5. **TaskFactory 全 74 方法路由** —— 诚实拒绝不可实现的重载，接线可实现的重载
6. **GC stress 完整跑通** —— 在 asan preset 下运行 100k Task 测试
7. **全量 benchmark baseline** —— 跨 22 个 chunk 采集 AOT vs .NET 8 数据

### 长期（3-6 月）

8. **性能收敛到 < 2× .NET 8** —— 按 benchmark 数据逐热点优化
9. **hotupdate 状态机验证** —— 确认 slot mapping 在热替换后正确
10. **foundation-dll fact 覆盖 threading-tasks 全 family**