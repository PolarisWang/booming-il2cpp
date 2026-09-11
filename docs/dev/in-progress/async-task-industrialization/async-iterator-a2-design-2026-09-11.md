# ASYNC-P2-8 A2 设计 — native AsyncIteratorBuilder + 池化 ValueTask&lt;bool&gt; source

> 状态：**已决策 — 采纳"版本化池化 source"方案**
> 前置：A1 已落地（`bd77bcd73`，显式检测 + `kUnsupportedAsyncIterator*` 信号通道）
> 上游文档：`async-iterator-recon-2026-09-11.md`（§1 事实、§5 三处假绿缺陷）
> 全局优先级：**P1（性能最优）> P2（方案完美性）> P3（HotUpdate）**。池化是 P1 硬约束，非可选。

---

## 0. 本轮新增的 ground truth（反射探针，.NET 9.0.0 CoreLib）

A1 的教训是"问运行时，不要从源码语义推断 API"。本轮对 A2 要落地的四个类型重新做了探针
（`%TEMP%/aqprobe/ValueTaskProbe.cs`），结果直接改变了设计取舍：

### 0.1 `ValueTask<T>` 的真实形状（是 **struct**，不是 class）

```
### System.Threading.Tasks.ValueTask`1  IsValueType=True
  interfaces: IEquatable`1
  field  Object  _obj          ← 多态载体：null=内联结果 / Task<T> / IValueTaskSource<T>[+token]
  field  TResult _result
  field  Int16   _token
  field  Boolean _continueOnCapturedContext
```

🔴 **关键**：单个 `_obj` 字段承担三种身份（inline value / `Task<T>` / `IValueTaskSource<T>`），
靠 `_token` 消歧。这与 `AsyncTask` 单一 handle 模型不同 —— `ValueTask<T>` **必须**能表达
"我其实是个 boxed `IValueTaskSource<T>`"。整个池化设计都围绕这一点。

### 0.2 `IValueTaskSource<T>` 的接口面（3 个方法）

```
GetStatus(Int16 token) -> ValueTaskSourceStatus
OnCompleted(Action`1 continuation, Object state, Int16 token, ValueTaskSourceOnCompletedFlags flags)
GetResult(Int16 token) -> TResult
```

**`token` 是每个方法的显式形参** —— 这不是装饰，是池化的正确性核心：池会复用同一个 source
对象，靠 token/version 拒绝"上一轮生命周期的 awaiter 在本轮被回调"。**丢掉 token = 复用出
错值**，属于静默错误结果，比崩溃更糟。

### 0.3 `ManualResetValueTaskSourceCore<T>` —— .NET 自己的实现，**是 struct**

```
### ...ManualResetValueTaskSourceCore`1  IsValueType=True
  field  Action`1 _continuation
  field  Object   _continuationState
  field  Object   _capturedContext
  field  ExceptionDispatchInfo _error
  field  TResult  _result
  field  Int16    _version     ← token 的来源
  field  Boolean  _completed
  field  Boolean  _runContinuationsAsynchronously
  Void Reset() / SetResult(TResult) / SetException(Exception)
  Int16 get_Version()
  ValueTaskSourceStatus GetStatus(Int16 token)
  TResult GetResult(Int16 token)
  Void OnCompleted(Action`1, Object, Int16, ValueTaskSourceOnCompletedFlags)
  Void ValidateToken(Int16 token)     ← token 校验的落点
```

.NET 把这套逻辑放在一个**可嵌入的 struct**里，由外层池化类型按值持有一段**池化数组**，
`Reset()` 递增 `_version` 后归还池位。A2 照抄这个结构 —— 它已经被验证过，且 `ValidateToken`
的存在说明"token 校验"是一等公民而非可选项。

### 0.4 `ConfiguredValueTaskAwaiter<T>`（迭代器状态机消费的那个 awaiter）

```
### System.Runtime.CompilerServices.ConfiguredValueTaskAwaitable`1 IsValueType=True
  ConfiguredValueTaskAwaiter GetAwaiter(...)
  --- nested ConfiguredValueTaskAwaiter
      Boolean get_IsCompleted(...)
      TResult GetResult(...)
      Void OnCompleted(...)
      Void UnsafeOnCompleted(...)
```

迭代器 SM 的 `<>u__1` 字段就是 `ConfiguredValueTaskAwaiter<bool>`，`await <>v__promiseOfValueOrEnd`
走这里的 4 个方法。

### 0.5 探针顺带证实的一条依赖关系（影响分期）

`ValueTask<T>` 上有 **`AsTask()` 和 `GetTaskForValueTaskSource(IValueTaskSource<T>)`**。
也就是说 **`ValueTask.AsTask` 不是与池化 source 并列的独立依赖，而是池化路径的一部分**——
两者必须一起做，否则 `Iterate<T>` 里 `...promise.AsTask()` 那一句无解。

---

## 1. 方案选择

### 备选方案

| 方案 | 做法 | 评价 |
|------|------|------|
| A. 不池化，每次 `new` 一个 source | `MoveNextAsync` 每次都堆分配 | ❌ **违反 P1**。`await foreach` 每次迭代一次分配，是 .NET 明确用 pooling 消除的开销 |
| B. 池化但不做 token 校验 | 复用 source，`GetResult` 忽略 token | ❌ 静默错值。旧 awaiter 在新生命周期被回调 → 读到错误的结果，且难以诊断 |
| **C. 版本化池化 source（采纳）** | 复刻 `ManualResetValueTaskSourceCore<T>` 的 `_version`/`ValidateToken` 语义，外加一层池 | ✅ P1 达标；错误 use-after-return 变成显式失败而非静默错值 |

### 采纳 C 的理由（按优先级）

- **P1**：池化消除每次迭代的分配 —— 这是本方案存在的理由，不可退让。
- **P2**：token 校验把"池化最常见的 bug"（陈旧 awaiter 读到复用后的结果）从静默错误
  变成显式失败。P2 允许为正确性增加少量开销：8 个 source 的池在迭代器生命周期内几乎
  零竞争。
- **P3（HotUpdate）**：池化对象若被热更代码持有，跨版本复用需谨慎 —— 但这是 P3，
  且 A2 的池是**每迭代器实例私有**（见 §2），不跨热更边界，故不构成阻碍。

---

## 2. 设计：`AsyncIteratorSourcePool`

### 2.1 结构

```
struct AsyncIteratorSourceCore        // ≙ ManualResetValueTaskSourceCore<bool> 的 native 等价物
{
    bool                  result;             // TResult = bool
    int16_t               version;            // token 来源；Reset() 时递增
    bool                  completed;
    bool                  canceled;           // 与 faulted 互斥（沿用 AsyncTask 的三态纪律）
    CHAOS_IL2CPP_INTPTR   exception;
    AsyncContinueFn       continuation;       // IValueTaskSource.OnCompleted 的 Action+state
    void*                 continuation_state;
    std::atomic<bool>     has_continuation;
    std::atomic<bool>     continuation_fired;

    int16_t GetVersion();
    void    Reset();                          // ++version, completed=false
    void    SetResult(bool);
    void    SetException(CHAOS_IL2CPP_INTPTR);
    int32_t GetStatus(int16_t token);         // token 不匹配 → 显式失败
    bool    GetResult(int16_t token);         // token 不匹配 → 显式失败
    void    OnCompleted(AsyncContinueFn, void* state, int16_t token, int32_t flags);
};

class AsyncIteratorSourcePool              // 每迭代器实例一个
{
    // 池容量取 8：单迭代器并发在途的 MoveNextAsync 通常为 1；8 给出余量而不浪费。
    // 溢出时回退到独立分配 —— 绝不阻塞、绝不返回 0。
    static constexpr int kPoolCapacity = 8;
    AsyncIteratorSourceCore  pool_[kPoolCapacity];
    bool                     in_use_[kPoolCapacity];
    std::mutex               mtx_;            // 只在取/还时持有，绝不在回调时持有

    AsyncIteratorSourceCore* Acquire();       // 满则 heap 分配（记录以便归还时 delete）
    void                     Release(AsyncIteratorSourceCore*);
};
```

### 2.2 token 的生命周期（正确性核心）

```
Acquire()  → 取一个空闲 slot，返回其当前 version（token）
  ... 迭代器 await 期间，awaiter 持有该 token ...
SetResult() → completed=true，回调 continuation
Release()  → Reset()：completed=false, ++version
```

`GetStatus(token)` / `GetResult(token)` **必须先校验 `token == version`**，不匹配即
显式失败（`CHAOS_IL2CPP_ABORT` + 诊断信息）。这样"上一轮的 awaiter 迟到"不会读到本轮的
结果 —— 这是 B 方案被否掉的正是这个点。

### 2.3 溢出策略

池满时 `Acquire` 退回 `new`。**不阻塞**：迭代器可能在同线程上嵌套等待，阻塞会死锁。
归还时按"是否来自池"决定还池还是 `delete`，用一个标记位区分（不靠指针范围推断 ——
那在 heap 分配落在池地址附近时会误判）。

### 2.4 内存归属（遵守统一内存分配约束）

- 池本体是迭代器状态机对象的**内嵌成员**（按值），随状态机一起由 GC 分配
  （`CHAOS_IL2CPP_NEW_GC`）→ 生命周期天然正确，无需单独释放。
- 溢出的 source 用 `CHAOS_IL2CPP_NEW` / `CHAOS_IL2CPP_DELETE`（raw 域配对），
  **不与 GC 域混用**。
- 状态机 → source 是"持有方 → 被持有方"，池在状态机内，方向单一，无环。

---

## 3. A2 交付范围（本轮）

| # | 交付物 | 反例（决策2=A，定义"完成"） |
|---|--------|---------------------------|
| A2-1 | `AsyncIteratorSourceCore` + `AsyncIteratorSourcePool`（`async_iterator.h`） | 去掉 token 校验 → 陈旧 awaiter 测试读到错误值（必须变红） |
| A2-2 | `AsyncIteratorMethodBuilder` 5 op 的 native 实现（Create/MoveNext/AwaitOnCompleted/AwaitUnsafeOnCompleted/Complete） | 去掉 `MoveNext` → 迭代器不推进（必须变红） |
| A2-3 | `ValueTask<bool>` 的最小 native 表示（承载 inline 值与 source+token 两种身份） | 只做 inline → 池化路径拿不到结果（必须变红） |
| A2-4 | 池化复用测试：同一 slot 跨两个生命周期，第二轮不得读到第一轮结果 | 见 A2-1 反例 |
| A2-5 | 并发测试：多线程同时 Acquire/Release 不丢不重 | 去掉 mutex → 测试必须**确定性变红**（见下方修正） |

### A2-5 反例预测修正（执行后回填）

本表原预测"去掉 mutex → 测试**不稳定**（需多次运行取证）"。实际执行推翻了这条预测，
且推翻方式是本项目最该警惕的那类——**前两版测试根本没有判别力**：

| 版本 | 形态 | 去掉 mutex 后 |
|------|------|--------------|
| 草案 1 | 每线程 acquire→set→get→release，只自校验 | **50/50 通过**（零判别力） |
| 草案 2 | 8 线程各 acquire 一个并跨 barrier 持有 | **40/40 通过**（一次 acquire 太粗，插不进 scan 循环） |
| 草案 3（定稿） | 8 线程 × 4000 轮 acquire/release 循环，发布 `live[t]` 并检测别名 | **6/6 确定性失败** |

结论有两条：

1. **"不稳定"是错误的验收标准**。一个只在竞态窗口偶然失败的测试，无法区分"代码正确"
   与"这次没撞上"。定稿用**计数式 oracle**（同一定义被两个线程同时持有 = 一次违规），
   正确代码确定性通过、错误代码确定性累积违规，与调度无关。
2. **反例必须真的跑**。草案 1/2 写完后看起来是在测并发，实际把 mutex 删掉也全绿；
   若不执行反例，A2-5 会以"绿"的状态交付一个未验证的锁。
   这与 recon 文档 D1/D2 记录的"断言形状缺陷"是同一类问题。

**A2 不做**（属 A3/A4）：registry 注册、classify、yield-return IR lowering、
`IAsyncEnumerable` 接口 vtable、`await foreach` 消费侧。

---

## 4. 架构映射（CLAUDE.md 要求）

| 层 | 路径 | 本轮职责 |
|----|------|---------|
| runtime-core | `src/native/common/chaos/async_iterator.h` | **新增**：source core + pool |
| runtime-core | `src/native/runtime-core/runtime_stubs/async_stubs.{h,cpp}` | 新增 `chaos_async_iterator_builder_*` extern C 入口 |
| codegen | （A3）`RuntimeHelperShapeRegistry.*.S16.cs` | 本轮不动 |
| native 测试 | `src/native/tests/`（或对应 gtest 目录） | 新增 A2-1..A2-5 反例测试 |

**红线检查**：
- ✅ 无全局 `operator new/delete` 重载
- ✅ codegen 输出宏约束（本轮不产 codegen）
- ✅ 跨域分配配对（池内嵌=GC 域；溢出=raw 域，互不混用）
- ✅ 不生成 CMake（TPG 职责）

---

## 5. 未决问题（留给 A3/A4，本轮不阻塞）

| # | 问题 | 为何不阻塞 A2 |
|---|------|--------------|
| R-A | `IAsyncEnumerable<T>`/`IAsyncEnumerator<T>` 的 vtable slot 序号 | 属 A4 对象模型；slot 必须来自反射，不得手写常量（recon D1） |
| R-B | `<>w__disposeMode` / `IAsyncDisposable` 的释放语义 | 属 A4；A2 的 pool 只需支持 `Reset` |
| R-C | `AwaitOnCompleted` 与 `AwaitUnsafeOnCompleted` 是否可指向同一实现 | A2 提供两个入口，可在 A3 注册时决定是否共用（A1 的 recon 已指出计划漏了 `AwaitUnsafeOnCompleted`） |
| R-D | `ValueTaskSourceOnCompletedFlags` 的位语义是否影响调度 | A2 按"忽略 flags、走既有 `g_async_dispatch_continuation_fn`"实现；若测试暴露语义差异再补 |
