# 异步迭代器（IAsyncEnumerable / AsyncIteratorMethodBuilder）翻译路径 设计讨论

- 日期: 2026-09-11
- 状态: **已决策 — 采纳方案 A（完整实现异步迭代器）**
- 发起: ASYNC-P2-8（Task.WhenEach 前置依赖）
- 相关: `docs/dev/in-progress/async-task-industrialization/STATUS.md`

## 0. 决策记录（2026-09-11）

用户决策：**坚持先在 Phase 2 内做完整异步迭代器**（方案 A），理由是
`Task.WhenEach` 属于锁定的 full-TPL-surface 范围，缺口不应绕过。

据此，方案 C（登记缺口）**不作为替代**，但其中「显式检测 + 诊断」部分**仍要保留**：
它保证在异步迭代器实现完成之前，未支持的形态是**显式失败**而非静默返 0。

P1（性能）约束按 §4 处理：`ValueTask<bool>` 的池化（`IValueTaskSource`）
**列为硬性验收点**，不是可选项 —— 否则每轮 MoveNextAsync 一次分配，
违反 CLAUDE.md 的 P1>P2>P3 优先级。

## 1. 背景与触发

ASYNC-P2-8 原计划实现 `Task.WhenEach`。架构查询发现它**不是**一个可独立接线的方法：

```
System.Threading.Tasks.Task::WhenEach(params Task[]) -> IAsyncEnumerable<Task>
System.Threading.Tasks.Task::WhenEach<TResult>(params Task<TResult>[]) -> IAsyncEnumerable<Task<TResult>>
```

BCL 实现（`ilspycmd` 反编译 .NET 9 System.Private.CoreLib）：

```csharp
public static IAsyncEnumerable<Task> WhenEach(params Task[] tasks)
    => WhenEachState.Iterate<Task>(WhenEachState.Create(tasks));

private static async IAsyncEnumerable<T> Iterate<T>(
    WhenEachState waiter,
    [EnumeratorCancellation] CancellationToken cancellationToken = default) where T : Task
```

即 WhenEach 的全部语义都落在 **`async IAsyncEnumerable<T>` 方法**上。没有异步迭代器支持，
WhenEach 无法实现。

**关键事实（已核实）**：

| 事实 | 证据 |
|------|------|
| `Task.WhenEach` 仅存在于 .NET 9+ | `ilspycmd` 在 8.0.11 CoreLib 中 `grep -c WhenEach` = **0**；9.0.0 中命中 |
| 当前 fixture 是 net8.0 | `tests/unit/managed/codegen/AsyncTestAssembly/AsyncTestAssembly.csproj` |
| 管线**零**异步迭代器支持 | `grep -rn "AsyncIteratorMethodBuilder" src/managed/` = 0 |
| wiki **零**相关路径记录 | `grep -rln "IAsyncEnumerable\|MoveNextAsync" wiki/` = 0 |

因此这是一个**全新翻译路径**，按 `dev-architecture-first-development` 步骤 1.4，
必须先完成设计讨论再实现。

## 2. 与现有 async 路径的差异

现有 Phase 2 的 async 支持建立在 `AsyncTaskMethodBuilder` 之上：

| 维度 | 现有（AsyncTaskMethodBuilder） | 新增（AsyncIteratorMethodBuilder） |
|------|------------------------------|-----------------------------------|
| 生成者 | `async Task/Task<T>` 方法 | `async IAsyncEnumerable<T>` 方法 |
| 状态机结构 | 单次 `MoveNext()` 执行到 `SetResult`/挂起 | `MoveNext()` **每次 `yield return` 返回一次**，循环复用 |
| builder 调用 | `Create` / `Start` / `SetResult` / `AwaitUnsafeOnCompleted` | `Create` / `MoveNext` / `AwaitOnCompleted` / `Complete` |
| 返回对象 | `Task` 句柄（native `AsyncTask`） | **`IAsyncEnumerable<T>` 对象**（需实现 `GetAsyncEnumerator`） |
| 消费侧 | `await` → `TaskAwaiter` | `await foreach` → `IAsyncEnumerator<T>::MoveNextAsync` → `ValueTask<bool>` |

**最大的结构差异**：现有状态机是「跑到底或挂起」，迭代器状态机是
「每 yield 一次就退出、下次 MoveNext 从 state 恢复继续跑」。R2-full 里为
`AwaitUnsafeOnCompleted` 补的 `AppendSuspendReturnsForAsyncMoveNext`
（NativeAotLoweringPlanner.StructuredIR.Emit.cs:1685）只处理**挂起后返回**，
不处理 **yield 后返回且需要保留续行状态**。

## 3. 方案候选

### 方案 A：完整实现异步迭代器（native 对象模型 + codegen 路径）

- native 侧：`AsyncIterator<T>` 对象（实现 `IAsyncEnumerable<T>` + `IAsyncEnumerator<T>`），
  持有状态机 box + `MoveNext` 函数指针 + `current`；`MoveNextAsync` 返回 `ValueTask<bool>`。
- codegen 侧：
  1. `AsyncIteratorMethodBuilder::Create/MoveNext/AwaitOnCompleted/Complete` 四族解析
  2. `yield return`（不是 IL 指令，而是 `SetResult(false)` + `stfld current` + `ret` 的模式）识别
  3. 状态机 box 的 GC 分配（复用 P2-7 的 Pattern A'）
  4. `[EnumeratorCancellation]` → `GetAsyncEnumerator(CancellationToken)` 的 token 传递
- 范围：大（约等于再造一遍 Phase 2 的工作量）

**风险**：`IAsyncEnumerable<T>` 是**接口**，需要在 native 侧实现完整 vtable
（`GetAsyncEnumerator`），并让 codegen 的 `callvirt` 能派发到它。这是对象模型层的扩展，
不只是翻译路径。

### 方案 B：只做 codegen 识别，运行时走解释器

- 识别 `async IAsyncEnumerable<T>` 状态机，但 builder 调用仍路由到
  `ChaosExternalRuntimeFallback`。
- **否决**：这正是本 Phase 要消灭的「假绿」。返回值会是 null，`await foreach` 直接崩。

### 方案 C：降级为「显式不支持」，登记为已知缺口

- 在 codegen 中显式检测 `AsyncIteratorMethodBuilder` / `IAsyncEnumerable<T>` 状态机，
  发出明确的编译期诊断（而非静默 fallback），并在 STATUS 中登记为 Phase 4+ 缺口。
- WhenEach 一并登记为 blocked-on-异步迭代器。
- 范围：小（1 个检测器 + 诊断）
- **收益**：把「静默返 0」转成「显式失败」，符合反假绿原则；为将来实现留下准确的路标。

### 方案 D：先做 Phase 2 剩余的可达工作，异步迭代器单独立项

- `ContinueWhenAll` / `ContinueWhenAny`（net8 可达，32 个重载）先做完，收尾 Phase 2。
- 异步迭代器 + WhenEach 作为**独立特性线**（Phase 4 或独立 roadmap），先出设计文档。

## 4. 依赖与冲突分析

- **P1 性能**：方案 A 的 `AsyncIterator<T>` 每次 `MoveNextAsync` 分配
  `ValueTask<bool>`；.NET 自己用 `IValueTaskSource` 复用（见 `WhenEachState : IValueTaskSource`）。
  若照抄简单实现会引入每次迭代分配 —— 与 P1 冲突，需池化。
- **P2 完美性**：方案 A 才完整；方案 C 不完整但诚实。
- **P3 HotUpdate**：异步迭代器状态机是新的 codegen 输出形态，hotupdate 侧需要同步登记
  （`AsyncIteratorMethodBuilder` 符号进入热更分发表）。
- **与 P2-7 的关系**：方案 A 的步骤 3 直接复用刚落地的 Pattern A' + prologue 分配。
  P2-7 是异步迭代器的**前置依赖，已完成**。

## 5. 建议

**分两步走（方案 D + C 组合）**：

1. **立即**：实现方案 C 的显式检测 + 诊断，把当前的静默 fallback 转为显式失败。
   这是纯收益且工作量小，符合反假绿原则。
2. **同时**：把 Phase 2 收尾工作（`ContinueWhenAll/Any`）做完 —— 这些在 net8 上可达，
   不依赖异步迭代器。
3. **异步迭代器**（方案 A）作为独立特性线立项：它是 IAsyncEnumerable 的**对象模型 + 翻译路径**
   双重扩展，不适合塞进 Phase 2 的尾巴。

**不建议**在 Phase 2 直接开做方案 A：它会把 Phase 2 的收尾无限期推迟，且需要先把
`IAsyncEnumerable<T>` 的对象模型（接口 vtable）设计清楚 —— 那本身就该有一份设计文档。

## 6. 待决策问题

- Q1: 采纳「方案 D + C 组合」，还是坚持先在 Phase 2 内完成方案 A？
- Q2: 异步迭代器特性线的优先级 —— Phase 4 尾部，还是独立 roadmap？
- Q3: 方案 A 若做，`ValueTask<bool>` 的池化（P1）是否作为硬性验收点？

## 附录：已核实的 BCL 事实

```
.NET 8.0.11 System.Private.CoreLib: Task.WhenEach 出现次数 = 0
.NET 9.0.0  System.Private.CoreLib:
  Task::WhenEach(params Task[])                       -> IAsyncEnumerable<Task>
  Task::WhenEach<TResult>(params Task<TResult>[])     -> IAsyncEnumerable<Task<TResult>>
  Task::WhenEach(IEnumerable<Task>)                   -> IAsyncEnumerable<Task>
  Task::WhenEach<TResult>(IEnumerable<Task<TResult>>) -> IAsyncEnumerable<Task<TResult>>
  Task/WhenEachState : Queue<Task>, IValueTaskSource, ITaskCompletionAction
  Task/WhenEachState::Iterate<T>(...) : async IAsyncEnumerable<T>  [EnumeratorCancellation]
```
