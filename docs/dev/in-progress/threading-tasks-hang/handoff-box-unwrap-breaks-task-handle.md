# threading-tasks fact 挂死 — 交接（三：box 拆解破坏 Task 句柄）

> 交接日期：2026-09-20 13:30
> 上游诊断：`docs/dev/in-progress/threading-tasks-hang/diagnosis.md`（两轮）
> 交接给：解决 ctor 句柄 ABI（`5184a0e59` `CtorReturnsNativeHandle`）的执行者
> 必要背景：`CtorReturnsNativeHandle` 机制与本单位同族 —— 都是「native 返回句柄、
> 通用生成路径当对象指针」。

## 一、一句话根因

**`RuntimeHelpers.GetUninitializedObject(Task)` 在运行时被改为返回 `AsyncTask` 句柄
（`object_stubs.cpp:189-194`，为解 87 个 factoryGap），但 codegen 的 `unbox` 降低
把它当托管对象指针、取 `&boxed->value`，破坏了句柄。**

## 二、症状

- threading-tasks `entry.exe --fact-json` **卡在 si=8**（`TaskTests::Wait_7__0`）
- stderr 末行：`[PARK] task=23cfbdf1318 completed=0 deadline=0`
- `entry.exe` 进程 **CPU=0**（真阻塞，非忙等）
- 影响：**fact 阶段完全不可用**，其后 448 个 subject 全跑不到

## 三、完整链条（生成物实证）

```
Create<Task>() → chaos_stub_definition_..._System_Threading_Tasks_Task()
  → ..._Create_System_Threading_Tasks_Task__generic()   [per-instantiation，非共享]
      {
          _s1 = Chaos_mt_Task.AsTypeInfoHot();
          _s1 = ChaosRuntimeHelpersGetUninitializedObject(_s1);
                 // ↑ object_stubs.cpp:193 —— taskLike 命中，返回【已完成的 AsyncTask 句柄】
          auto* chaos_boxed = reinterpret_cast<chaos_boxed_type_..._Task*>(_s1);
                 // ↑ ★ 把【句柄】当【boxed 对象指针】重新解释
          _s1 = &chaos_boxed->value;                    // ★ 返回 box 的 value 字段地址
          return _s1;                                    // Wait() 收到的是地址，不是句柄
      }
  → ChaosAsyncTaskWaitInfinite(那个地址)
  → park_until_completed(伪指针, deadline=nullptr) → 永久 park
```

## 四、关键证据

| 证据 | 值 |
|---|---|
| `[GUO] ... taskLike=1` | 日志 8 次 —— 特判命中 ✅ |
| `object_stubs.cpp:193` | `async_task_from_result(0)` —— 正确的完成句柄 ✅ |
| `__generic` 生成体 | `reinterpret_cast<chaos_boxed_type_Task*>(句柄)` + `&->value` ❌ |
| `[WAIT] handle=... completed=0` | Wait 收到的 handle ≠ GUO 返回的句柄 |
| `entry.exe` CPU=0 | 真阻塞，与 `wait_cv.wait()` 一致 |

## 五、发射点（已定位）

`EmitLinearUnbox`（`Emission/NativeAotLoweringPlanner.LinearEmission.cs:319-335`）：

```csharp
builder.AppendLine($"auto* chaos_boxed = reinterpret_cast<{GetNativeBoxTypeSymbol(...)}*>({ConsumeEvalStackValueExpression()});");
builder.AppendLine("if (chaos_boxed == nullptr) { CHAOS_IL2CPP_FAIL_FAST(); }");
EmitEvalStackPush(..., "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_boxed->value)");
```

它**无条件**把 eval 栈值当 boxed 指针取 `&->value`。

## 六、与 `CtorReturnsNativeHandle` 的同族关系

你刚修的 `5184a0e59` 机制：

> **引用类型 newobj 的 native 工厂返回旁表 slot 句柄，但 `EmitLinearNewObject`
> 恒推托管对象地址上栈 → 句柄被当堆指针 → `Resolve()` 返 nullptr。**

**本单位的形态完全相同** —— 只不过句柄来源是 `GetUninitializedObject`（运行时特判），
不是 `.ctor` 工厂（shape 标记）。两者都是「**通用生成路径假定引用=对象指针**，
而 native 契约返回句柄」的冲突。

你的机制（`CtorReturnsNativeHandle` 标记 + 发射点旁路）**应该能直接套用**：
关键在于让「这个调用返回的是句柄」这个信息从 shape/定义处传到 `EmitLinearUnbox`，
然后像你 C4 那样**旁路**而不是当对象。

## 七、候选修法（按与你机制的对齐度排序）

| 方案 | 内容 | 对齐度 |
|---|---|---|
| **Y5（推荐）** | 仿 `CtorReturnsNativeHandle`：给「接受句柄返回的 `GetUninitializedObject` 调用」打标记，`unbox` 时旁路 | 高 —— 复用你的机制骨架 |
| **Y3** | 句柄表登记 + unbox 前查标记 | 中 —— 新机制 |
| **Y1** | codegen 对 Task 特判 | 低 —— 违反通用规则 |

**推荐 Y5**：它与你已落地的 `CtorReturnsNativeHandle` 是**同一条设计线**，
改起来是「再加一个标记」而非「新发明」。

## 八、影响面

`Create<T>` 家族 **11 个 instantiation**（含 Task / Task\<T\> / TCS / TCS\<T\> /
ValueTask / TaskFactory / ConcurrentExclusiveSchedulerPair）**全部**受此冲突。
这就是 threading-tasks 大批 `factoryGap` 的来源 —— 修好本单位，不止解挂死。

## 九、我（诊断者）的状态标记

- ✅ 已定位到 `EmitLinearUnbox`
- ❌ 未动手修 —— 因玩具案已越过「三次修复规则」，且并发会话同在 codegen 层
- ⏳ `Create<T>` 降低的可插入点已确认存在（GenericSharing 的 `OpenDefinitionUsesGenericTypeParameter` 已禁用共享 → 每实例化独立 body），
  Y5 的切入点应是**给该 per-instantiation body 的 GetUninitializedObject 调用打标记**