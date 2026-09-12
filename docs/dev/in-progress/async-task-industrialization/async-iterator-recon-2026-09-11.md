# ASYNC-P2-8 异步迭代器 — 侦察结论与范围重估

- 日期: 2026-09-11
- 状态: **侦察完成，范围需要重新决策**
- 前置: `docs/discuss/20260911-async-iterator-translation-path.md`（设计讨论，已采纳方案 A）
- 方法: 4 agent 多维侦察（runtime / translation / wiring）+ **独立反射探针**对拍 .NET 9 CoreLib

## 0. 为什么要重估

设计讨论（§5）与一个 agent 都建议「先做方案 D + C」。用户决策为**坚持方案 A**。
本轮侦察的目的是把方案 A 的真实成本量化 —— 结论是：**真实成本显著超出设计文档给出的画像**。

设计文档 §3 把方案 A 描述为「约等于再造一遍 Phase 2 的工作量」。侦察发现这个类比
**低估了**，因为它忽略了**消费侧**：

| 被忽略的部分 | 证据 |
|------------|------|
| 迭代器状态机实现 **`IValueTaskSource<bool>`** | 反射: `<Iterate>d__15`1` 接口表含 `IValueTaskSource<Boolean>` |
| 迭代器状态机实现 **`IAsyncDisposable`** | 反射: 同上，含 `DisposeAsync`；SM 有 `<>w__disposeMode` 字段 |
| 消费侧 `await foreach` 需要 `MoveNextAwaiter` | `MoveNextAsync()` 返回 `ValueTask<bool>`，当前 `AsyncAwaiterCatalog` 无此形态 |
| WhenEach 的 `Iterate<T>` 真实体依赖 | 见 §3 —— 需要 Monitor / WaitAsync / CancellationTokenSource / ValueTask.AsTask |

即：方案 A 不是「再造一遍 Phase 2」，而是**Phase 2 + Phase 3 的合并**再加上
`IAsyncEnumerable` 对象模型与 `IValueTaskSource` 池化两处**全新**机制。

## 1. 已核实的事实（反射探针，.NET 9.0.0 CoreLib）

探针源码: `%TEMP%/aqprobe/Program.cs`。这是 ground truth，不是从源码语义推断。

### 1.1 AsyncIteratorMethodBuilder 的真实 API 面（5 个方法，非 4）

```
static AsyncIteratorMethodBuilder Create()
       Void MoveNext<TStateMachine>(TStateMachine& stateMachine)
       Void AwaitOnCompleted<TAwaiter,TStateMachine>(TAwaiter& awaiter, TStateMachine& stateMachine)
       Void AwaitUnsafeOnCompleted<TAwaiter,TStateMachine>(TAwaiter& awaiter, TStateMachine& stateMachine)
       Void Complete()
```

🟡 **修正**：计划 step 3 写「注册 4 个 op（Create/MoveNext/AwaitOnCompleted/Complete）」——
**漏了 `AwaitUnsafeOnCompleted`**。它与 `AwaitOnCompleted` 同时存在，必须都注册
（指向同一 native 实现）。计划内部的 `Q1` 已经提出这个矛盾但未落到 step 3 的文案里。

### 1.2 接口方法数（vtable 布局的输入）

```
IAsyncEnumerable`1  → 1 个方法: IAsyncEnumerator`1 GetAsyncEnumerator(CancellationToken)
IAsyncEnumerator`1  → 2 个方法: ValueTask`1 MoveNextAsync() / T get_Current()
```

这比计划 step 12 暗示的要简单 —— 接口 surface 本身很小。

### 1.3 真实迭代器状态机的形状（`Task+WhenEachState+<Iterate>d__15`1`）

```
IsValueType = False            ← class，不是 valuetype（与 async Task 形态不同）
interfaces: IAsyncDisposable, IValueTaskSource<Boolean>, IValueTaskSource, IAsyncStateMachine
fields:
  Int32 <>1__state
  AsyncIteratorMethodBuilder <>t__builder
  ManualResetValueTaskSourceCore`1 <>v__promiseOfValueOrEnd
  T <>2__current
  Boolean <>w__disposeMode
  CancellationTokenSource <>x__combinedTokens
  Int32 <>l__initialThreadId
  ConfiguredValueTaskAwaiter <>u__1
methods:
  MoveNext / SetStateMachine
  IAsyncEnumerable<T>.GetAsyncEnumerator
  IAsyncEnumerator<T>.MoveNextAsync / get_Current
  IValueTaskSource<bool>.GetResult / GetStatus / OnCompleted
  IAsyncDisposable.DisposeAsync
```

**与 `async Task` 的形态差异（P2-7 的假设**不能**照搬）**：

| 维度 | async Task（P2-7） | async iterator |
|------|-------------------|----------------|
| 状态机类型 | `valuetype` 局部，无 `newobj` | **class**，有 `newobj` |
| box 识别 | 需要 Pattern A'（无 newobj 形态） | **Pattern A 原生适用**（有 newobj） |
| GC 可达性 | 必须显式上 GC heap（P2-7 修的） | class `this` 天然 GC 可达 |

🟢 **好消息**：`IdentifyAsyncBoxPointerLocalSlots` 的 Pattern A 对迭代器**碰巧适用**，
计划 step 5 担心的 box 缺口比想象中小。但**不能据此认为无需改动** —— 需实测确认
（见 §4 开放问题 R1）。

🔴 **坏消息**：`IsAsyncStateMachineMoveNext` 的两个判据（`>d__` + `::MoveNext`）
对迭代器同样成立，因此迭代器会被**误判为 async Task 的 MoveNext** 走完整条
structured-IR 路径 —— 这正是「部分覆盖的假绿」：测试可能绿，但 yield 语义全错。

## 2. 与计划的矛盾（已解决 / 未解决）

| # | 矛盾 | 状态 |
|---|------|------|
| C1 | AsyncIteratorMethodBuilder 是 4 个还是 5 个方法 | ✅ **已判**: 5 个（§1.1）。计划 step 3 文案需修正 |
| C2 | 是否调 `AsyncIteratorMethodBuilder.MoveNext` 传给 RegisterAsyncTaskBuilder 那种「callee 拼写」 | ✅ **已判**: 需验证 SM 的 `MoveNextAsync` 是否**内联**调 `builder.MoveNext(ref this)` |
| C3 | `ManualResetValueTaskSourceCore` 能否 codegen 直接表达 | ⚠️ **未解决**（见 R2） |
| C4 | 迭代器内部 await 用 `ConfiguredValueTaskAwaiter` | ✅ **已证实**（字段 `<>u__1` 类型即它）。`AsyncAwaiterCatalog` 只有 4 条映射，**不含**它 → 需补 |
| C5 | `disposeMode` 字段 | ✅ **已证实存在**（`<>w__disposeMode`）|

### C2 的验证（关键）

计划 step 3 假定 `AsyncIteratorMethodBuilder::MoveNext<T>(ref T)` 会以**可解析的 callee
拼写**出现在 entry 的 IL 里。但反射显示消费侧方法是
`IAsyncEnumerator<T>.MoveNextAsync()`（在**状态机自己**身上），而不是
`AsyncIteratorMethodBuilder::MoveNext`。

**推论（待 IL 确认）**：`MoveNextAsync` 体内调 `<>t__builder.MoveNext<T>(ref this)`，
即 builder 调用**发生在状态机内部**而非 entry。这与 `AsyncTaskMethodBuilder::Start`
由 entry 调用的形态**是不同的接线点**，step 3 的注册策略需要据此调整。

## 3. 消费侧的真实依赖（范围重估的核心）

`Task+WhenEachState+Iterate<T>` 的**完整**反编译体（ilspycmd，已核实的输出）:

```csharp
public static async IAsyncEnumerable<T> Iterate<T>(
    WhenEachState waiter,
    [EnumeratorCancellation] CancellationToken cancellationToken = default) where T : Task
{
    int num = -1;
    if (!(waiter?.TryStart() ?? false)) yield break;          // 早期 yield break
    while (waiter.Remaining > 0)
    {
        ValueTask valueTask = default(ValueTask);
        bool lockTaken = false;
        Task result;
        try
        {
            Monitor.Enter(waiter, ref lockTaken);              // lock
            waiter._waitForNextCompletedTask.Reset();
            if (!waiter.TryDequeue(out result))
                valueTask = new ValueTask(waiter, waiter._waitForNextCompletedTask.Version);
        }
        finally
        {
            if (num == -1 && lockTaken) Monitor.Exit(waiter);   // finally
        }
        if (result != null)
        {
            cancellationToken.ThrowIfCancellationRequested();
            waiter.Remaining--;
            yield return (T)result;                             // yield return
            num = -1;
        }
        else
        {
            if (cancellationToken.CanBeCanceled && !valueTask.IsCompleted)
                valueTask = new ValueTask(valueTask.AsTask().WaitAsync(cancellationToken));
            await valueTask.ConfigureAwait(continueOnCapturedContext: false);
        }
    }
}
```

**要让这段代码真正跑起来**（不只是 emit 出符号），需要以下全部到位：

| 依赖 | 现状 | 域 |
|------|------|-----|
| `Monitor.Enter/Exit` | 需查证 native 实现 | runtime |
| `WhenEachState.TryStart/TryDequeue/Remaining` | 需 codegen 出真实对象模型 | codegen |
| `ManualResetValueTaskSourceCore<T>.Reset/Version` | **无 native 表示**（R2） | runtime |
| `ValueTask.AsTask()` / `WaitAsync(CancellationToken)` | 需查证 | runtime |
| `CancellationTokenSource`（`<>x__combinedTokens`，合并 token 时） | 需查证 | runtime |
| `ConfiguredValueTaskAwaiter` | catalog 缺 | codegen |
| 迭代器 `DisposeAsync` / `<>w__disposeMode` | **完全未实现** | runtime+codegen |

🔴 **这是范围重估的核心**：即使异步迭代器基础设施全部做完，要让 `Task.WhenEach`
**端到端跑通**（而不只是「emit 出 native 符号」），还需要 Monitor / WaitAsync /
CancellationTokenSource / ValueTask.AsTask 四条独立的运行时能力线。

## 4. 实现前必须先验证的开放问题

按优先级排序。**每一个都必须先验证再动手** —— P2-5/P2-7 的教训是
「诊断结论必须在真实管线上复核」「形状假设必须对拍真实 IL」。

| # | 问题 | 验证方法 | 阻塞对象 |
|---|------|---------|---------|
| **R1** | 迭代器 SM 是 class，Pattern A（`newobj >d__`）是否真的适用？ | 对 fixture 编译产物跑 `IdentifyAsyncBoxPointerLocalSlots` 的诊断输出 | step 5 |
| **R2** | `ManualResetValueTaskSourceCore<T>` 有无 native 表示？能否 codegen 直接表达？ | grep native 侧；pipeline 试跑看是否落 fallback | step 1/12 |
| **R3** | 迭代器内部 await 是否走 `ConfiguredValueTaskAwaiter`？catalog 要补哪些条目？ | 反射已证实类型；需确认 codegen 侧 callee 拼写 | step 6 |
| **R4** | `IAsyncEnumerable`/`IAsyncEnumerator` 在 native 侧的 stable_id 与 vtable slot 顺序 | `ComputeStableTypeId` 同算法核对；slot 错位 = callvirt segfault | step 12 |
| **R5** | net9.0 fixture 对现有 2188 测试的影响面 | 新建独立 csproj（**不动** net8 的 AsyncTestAssembly）后全量跑 | step 10 |
| **R6** | `IRAwait` 节点当前是否接线？`AsyncCoroutineEmitter.BuildAsyncStructuredBody` 是不是死路？ | `Debug.Fail` 探针 + 跑全量 async 单测看是否触发 | step 9 |
| **R7** | Python preflight gate 的挂点路径 | 参考 `benchmark_chunk.py` | step 13 |
| **R8** | `Monitor.Enter/Exit` / `ValueTask.AsTask` / `WaitAsync` / `CancellationTokenSource` 现状 | grep + 管线试跑 | §3 全部 |

## 5. 计划的三个「反假绿」缺陷（必须修正后才可实施）

### D1 — 计划 step 12 的反例**不可执行**

原文：

> 反例: vtable slot 偏移错位 … → **bounded compile 不能捕获 → runtime segfault**
> → 单测不会通过 R2-full emission 检查但 runtime FAIL。注意: 此反例验证需要 runtime 端到端 …
> **codegen 单元测试不能替代此任务**: bounded compile 中加静态断言 slot 号。

自相矛盾：既说「单测不能替代」，又说「bounded compile 加静态断言」。**静态断言只能证明
自己写下的 slot 号等于自己写下的期望值** —— 如果期望值本身就是错的，断言照样绿。
这是 P2-7「断言不具鉴别力」的同类缺陷。

**修法**：slot 号的 oracle 必须来自**外部**（反射探针读 `GetMethods()` 返回顺序，
或 dotnet/runtime 源码），而不是手抄进 C++ 断言里。

### D2 — 「不含 `ChaosExternalRuntimeFallback`」不足以证明「不是假绿」

计划 step 11/13 的核心断言是 `DoesNotContain("chaos_external_runtime_")`。
但 P2-4 的教训是：**单测绿、管线 null**。不落 fallback 只说明「认出来了」，
不说明「发出了正确的调用」。必须同时断言**正向的 native 符号**（计划部分做到了）。

更进一步：`DoesNotContain` 类断言在**符号名漂移**时会静默失效
（改了前缀 → 断言仍然通过，因为原来也不含）。需要用**双向**断言（P2-7 教训）。

### D3 — `FakeWhenEach(return null)` 作为反例**强度不足**

计划 step 10/11 用一个「返回 null 的假 WhenEach」作为反假绿构造。但 `return null`
是一个**明显**的伪造，任何「非 null」检查都能抓。真正会漏掉的是**语义伪造**：
emit 出全部正确符号、但 yield 的**顺序/次数/值**是错的。

**修法**：反例应当是「**符号全对、语义错**」的构造（例如 yield 顺序倒置），
才能证明 gate 检测的是语义而非形状。

## 6. 建议

侦察结论**支持**设计讨论 §5 的原始建议（方案 D + C），而不支持直接全量方案 A。
但用户已明确决策方案 A，因此本文件**不重复反对**，只做两件事：

1. **量化**：给出 §3 的消费侧依赖表 —— 方案 A 的真实边界比设计文档大。
2. **给出可执行的替代切分**（见 §7），让「坚持方案 A」也能在有限轮次内产出**可验证**增量。

## 7. 建议的增量切分（供决策）

把方案 A 拆成**四个可独立验证**的增量，每个都有反例：

| 增量 | 内容 | 验收（含反例） | 预估 |
|------|------|--------------|------|
| **A1** | 显式检测 + 诊断：识别 `AsyncIteratorMethodBuilder`/`IAsyncEnumerable` 状态机，**显式失败**而非静默返 0 | 反例: 去掉检测 → 未支持形态静默返 0（gate 必须变红） | 小 |
| **A2** | native 侧 `AsyncIteratorBuilder` + 池化 `ValueTask<bool>` + 单测 | 反例: 去掉池化 → 每轮分配（P1 gate 变红） | 中 |
| **A3** | codegen: registry 5 op + classify + await 侧 catalog | 反例: 删注册 → emit 落 fallback | 中 |
| **A4** | yield IR + 接口 vtable + `await foreach` 消费侧 | 反例: slot 错位（oracle 来自反射）→ runtime 失败 | 大 |

**A1 是纯收益且立即可做**，符合反假绿原则：它把当前的「静默返 0」转成「显式失败」，
且**不依赖** A2-A4 的任何工作。

## 附录：探针可复现命令

```bash
# 反射探针（ground truth，非源码语义推断）
cd "%TEMP%/aqprobe" && dotnet run

# IL 反编译（注意：-t 对本 DLL 不稳定，会 dump 整个模块；用 sed 定位区间）
ilspycmd -t "System.Threading.Tasks.Task+WhenEachState+<Iterate>d__15\`1" \
  "C:/Program Files/dotnet/shared/Microsoft.NETCore.App/9.0.0/System.Private.CoreLib.dll"
```
