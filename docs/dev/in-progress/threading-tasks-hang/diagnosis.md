# threading-tasks fact 挂死 — 完整诊断（2026-09-20）

> 症状：`entry.exe --fact-json` 在 **si=8** 永久挂起，其后 448 个 subject 全跑不到。
> 影响：**threading-tasks 的 fact 阶段完全不可用** → 整条 thread 线验证受阻。

## 一、挂死点（精确定位）

`kSubjectSubjectIds[si]`（生成代码里的 slot→subject 权威映射）给出：

```
si=8 → TaskTests::Wait_7__0
```

生成体（`native-aot.page-0009.cpp`）：

```cpp
const auto chaos_result = chaos_stub_definition_..._SubjectInstanceFactory__Create_1___0___type____method__System_Threading_Tasks_Task();
_s0 = chaos_result;
if (chaos_arg_0 == 0) raise_null_reference_exception();
ChaosAsyncTaskWaitInfinite(chaos_arg_0);   // ← 无限等待
```

## 二、根因链

```
Create<Task>()  → chaos_stub_definition_..._System_Threading_Tasks_Task()
                → Chaos_TestFramework_Sdk_..._Create_System_Threading_Tasks_Task__generic()
                   [kHotpatchKeepNative —— TPG 发射的 native body]
                → 返回 async_task_create()
                   = 一个【活的、永不完成的】AsyncTask
ChaosAsyncTaskWaitInfinite(handle)
                → park_until_completed(task, nullptr)   // async.h:115
                → deadline=nullptr → wait_cv.wait() 无超时
                → 该 task 永不 completed
                → notify_task_completed 永不触发
                → 【永久 park】
```

**stderr 的最后一行即此**：
```
[PARK] task=23d7f0992d8 completed=0 deadline=0
```

## 三、⚠️ 关键判断：**运行时行为符合 .NET 语义**

`.NET 8 实测`：
```csharp
var t = new Task(() => {});     // Status = Created（未启动）
t.Wait();                        // → 永久阻塞（实测确认，非推断）
```

而 ATG 的 factory（`ProbeEmitter.cs:410-418`）对 `Create<Task>()` 走
`Activator.CreateInstance<Task>()` → 返回**未启动**的 Task。

**⇒ 未启动的 Task 上 `Wait()` 死锁，是 .NET 的【正确】语义。**

因此这**不是运行时缺陷**，而是 **probe 生成了一个会自我死锁的测试**：

```csharp
// ProbeEmitter 生成的 Wait_7__0
SubjectInstanceFactory.Create<System.Threading.Tasks.Task>().Wait();  // 必然死锁
return 42L;
```

## 四、方向 2（补线程池/延续投递）的正确性评估

**方向 2 不需要做，也不应该做** —— 原因：

1. **语义上无解**：未启动的 Task 就是要永久阻塞。补线程池**不会**让 `new Task(()=>{}).Wait()` 返回，反而会**破坏 .NET 语义**。
2. **运行时是对的**：`park_until_completed` 的无限等待正是 `Wait()` 的正确实现。
3. **真正的缺陷在 ATG 层**：`Create<Task>()` 返回未启动任务，而 probe 直接对它 `Wait()`。

## 五、正确的修法方向（转向 ATG / fixture）

| 方案 | 内容 | 归属 |
|---|---|---|
| **A（推荐）** | ATG 识别「subject 会对 factory 产物立即 `Wait()`」的组合，让 `Create<Task>()` 返回**已完成**任务（或该 probe 标 unassertable） | ATG 线 |
| **B** | `Create<Task>()` 一律返回 `Task.CompletedTask` 等价的已完成句柄 | 需论证是否偏离 .NET 语义 |
| **C** | 在 fact runner 层加超时（如 60s/subject），把挂死降级为单项失败 | 验证基础设施 |

## 六、与既有工作的一致性

- `object_stubs.cpp:189-194` 已对 **GUO 路径**做了同样处理：
  ```cpp
  if (taskLike) return async_task_from_result(0);   // 已完成任务
  ```
  但 `Create<Task>()` 走的是 **另一条 `__generic` 入口**，**不经过 GUO** → 未受益。
- `e9e1612ce` / `bffe43c7b` 都在处理「receiver 供给」面，本问题是**同一族的第三个入口**。

## 七、立即可做的止血

**方案 C**（runner 超时）与方向无关、成本最低，且能**立刻解封 fact 阶段** ——
因为死锁的是**单个 subject**，超时后可继续跑完剩余 447 个。

> ⚠️ 但要注意：`park_until_completed` 用的是 `wait_cv.wait()`，**超时无法中断它** ——
> 需要 runner 层的**进程级/线程级**超时（如 watchdog + ExitThread），或改为
> 对 `Wait()` 无参调用注入有限超时。**这需要在实施前验证可行性。**
