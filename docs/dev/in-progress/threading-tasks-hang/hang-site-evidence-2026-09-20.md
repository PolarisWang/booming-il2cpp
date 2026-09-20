# threading-tasks 挂死 — 挂点实证与根因裁决（2026-09-20 第 4 轮）

> 上游：`diagnosis.md`（两轮）、`handoff-box-unwrap-breaks-task-handle.md`、
> `verification-2026-09-20.md`
> 本文：用**实测挂死现场 + 生成物静态证据**裁决三条互相矛盾的根因判断。

## 一、方法：用上一轮的超时修复换取现场

上一轮修复（`47dab8dff`）让 `entry.exe` 超时后**保留已产出的 subject 结果**，
不再整批丢弃。本轮直接跑现成的 `entry.exe`（90s 上限，不白等 600s），
立刻拿到挂死前的现场：

```
si=0  ConcurrentExclusiveSchedulerPairTests::Complete_0__0        passed  value=42
si=1  TaskTests::RunSynchronously_0__0                            passed  value=42
si=2  TaskTests::RunSynchronously_1_TaskScheduler_0               passed  value=42
si=3  TaskTests::Dispose_2__0                                     passed  value=42
si=4  TaskTests::GetAwaiter_3__0                                  passed  value=1
si=5  TaskTests::ConfigureAwait_4_bool_0                          passed  value=1
si=6  TaskTests::ConfigureAwait_5_ConfigureAwaitOptions_0         passed  value=1
si=7  TaskTests::Yield_6__0                                       passed  value=1
si=8  ← 挂死（stdout 中无记录）
```

stderr 末两行：
```
[WAIT] handle=23cf8a2f608 completed=0 timeout=-1
[PARK] task=23cf8a2f608 completed=0 deadline=0
```

**8 个 subject 跑完并全部 passed，第 9 个（si=8 `Wait_7__0`）永久 park。**
（修复前这一切都被丢成 `passed:0/total:0`。）

## 二、根因裁决（三条判断，实测裁决）

此前存在三条互相矛盾的判断：

| # | 来源 | 主张 |
|---|---|---|
| 1 | `diagnosis.md` 前文 | ATG 生成了自我死锁的 probe（`new Task(()=>{}).Wait()`） |
| 2 | `diagnosis.md` 补充 + 交接文档 | codegen 的 `EmitLinearUnbox` box 拆解破坏了 Task 句柄 |
| 3 | `bffe43c7b` WIP 自述 | stable_id 特判生效，但「第 9 个 subject 挂点**不经过 GUO**」 |

### 裁决 1：判断 3 **被推翻**

si=8 的 `Wait_7__0` 生成体（`native-aot.page-0008.cpp:3639`）：

```cpp
const auto chaos_result =
    chaos_stub_definition_..._SubjectInstanceFactory__Create_1___0___type____method__System_Threading_Tasks_Task();
_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
if (chaos_arg_0 == 0) { ...raise_null_reference_exception(); }
ChaosAsyncTaskWaitInfinite(chaos_arg_0);
```

它**恰恰走的就是 `Create<Task>()` 路径**。「挂点不经过 GUO」不成立 ——
GUO 在 `Create<T>` 体内被调用（见裁决 2），只是**不在 `Wait_7__0` 自己的函数体里**，
这可能是上一轮误判的来源。

### 裁决 2：判断 2 **坐实**（生成物实证）

`Create<T>` 的泛型定义体（`native-aot.page-0003.cpp:1053` 起的 11 处之一）：

```cpp
const auto chaos_result = ChaosRuntimeHelpersGetUninitializedObject(chaos_arg_0);
_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
chaos_locals[0] = _s2;
// ...
auto* chaos_boxed = reinterpret_cast<chaos_boxed_type_..._T*>(_s2);
if (chaos_boxed == nullptr) { CHAOS_IL2CPP_FAIL_FAST(); }
_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_boxed->value);   // ★ 取 box 的 value 字段
```

而 `object_stubs.cpp:189-194` 对 taskLike 类型返回的是 **AsyncTask 句柄**
（不是零化托管对象）——句柄被当对象指针解引用后取 `&value`，
`Wait` 收到伪地址 → 永不 completed → 永久 park。

### 裁决 3：判断 1 **部分成立但非破坏点**

probe 确实生成了 `Create<Task>().Wait()` 组合。但若仅此而已，
`GetUninitializedObject` 返回**已完成**句柄时 `Wait` 会立即返回
（`si=4 GetAwaiter` / `si=7 Yield` 正是如此通过的）。真正使其死锁的是
**句柄被 box 拆解破坏**。故判断 1 是「诱因」，判断 2 是「破坏点」。

## 三、为什么 si=0..7 通过而 si=8 挂死

`GUO` 只对 **taskLike 类型**（Task / ConcurrentExclusiveSchedulerPair）返回句柄；
其余类型返回**正常的零化托管对象** —— 对后者，box 拆解是**正确**的。

因此该缺陷是 **Task 族专属**。si=0..7 中消费句柄的调用
（`RunSynchronously` / `Dispose` / `GetAwaiter` / `ConfigureAwait` / `Yield`）
**不经过**「把 receiver 当 boxed 对象解引用」这条路径，故通过；
`Wait` 的 receiver **恰好经过**，故挂死。

GUO 调用序列（6 次）与 si 的对应也印证：si=0 用
`stable_id=16910075678292704579`（ConcurrentExclusiveSchedulerPair），
si=1..5 用 `stable_id=13973664129490152178`（Task）。

## 四、影响面

`native-aot.page-0003.cpp` 中 **11 处** `ChaosRuntimeHelpersGetUninitializedObject`
调用点，**每一处**都跟着 `&chaos_boxed->value`：

| 实例化 | 是否 taskLike |
|---|---|
| `Create`（开放泛型） | 视 T 而定 |
| `Create_..ConcurrentExclusiveSchedulerPair` | ✅ 是 |
| `Create_..Task_System_Int32` | ✅ 是 |
| `Create_..Task__generic` | ✅ 是 |
| `Create_..TaskCompletionSource*` | ❌ 否 |
| `Create_..TaskFactory*` | ❌ 否 |
| `Create_..UnobservedTaskException*` | ❌ 否 |
| `Create_..ValueTask*` | ❌ 否 |

**修复必须对所有实例化安全** —— 只对「GUO 返回句柄」的类型旁路 box 拆解，
不能粗暴删除（那会破坏 TaskCompletionSource / TaskFactory / ValueTask 的
正常对象语义）。

## 五、推荐修法（仍未实施，见 §六）

沿用交接文档 §七 的 **Y5**：仿 `CtorReturnsNativeHandle`（`5184a0e59`）——
给「返回值是句柄」的调用打标记，`unbox` 时旁路。

**本轮为 Y5 补充的关键约束**：
- 标记必须**按类型**而非按调用点 —— 同一个 `Create<T>` 定义体对
  Task 与 TaskCompletionSource 行为不同，运行期才可判定
- 因此更可能是**运行时侧**方案：让 `GetUninitializedObject` 的句柄返回
  在 ABI 上可区分（如返回 tagged pointer 或写入旁表），
  使 codegen 的 box 拆解能识别「这不是 box」

## 六、我（本轮执行者）的状态

- ✅ 抓到挂死现场（8/258 部分结果 + 精确 park 点）
- ✅ 裁决三条矛盾根因（推翻 1 条、坐实 1 条、修正 1 条）
- ✅ 定位 11 处调用点与「Task 族专属」的作用域
- ❌ **仍未修复** —— 这是同一问题的第 4 轮，超出「三次修复规则」；
  且 §五 的 Y5 需要对 ABI 语义做设计决策，应由该机制的 owner 定夺

**本轮产出的是可执行的定位，不是补丁。**
