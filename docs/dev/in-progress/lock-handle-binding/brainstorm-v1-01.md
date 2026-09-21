# brainstorm-v1-01 — Create&lt;T&gt; 对「无参可构造引用类型」应返回什么

> task_id: `lock-handle-binding`
> 日期：2026-09-20
> 状态：brainstorming（blocking_questions 未清零）
> 范围：**跨域** — 运行时(native) + CodeGen + ATG/测试基建

## 〇、为什么升级为跨域

最初的问题看起来是「threading 的 14 个锁类 subject 失败」。
追查后发现根因是一个**更通用的机制问题**，它的影响面远超 threading：

`SubjectInstanceFactory.Create<T>()` 对**引用类型**统一返回
`RuntimeHelpers.GetUninitializedObject(typeof(T))` —— 零初始化、**不跑构造函数**。

实测调用规模（`CombinedSubjects.cs` 中 `SubjectInstanceFactory.Create<` 出现次数）：

| chunk | 调用点 | 涉及的不同类型 |
|---|---:|---:|
| system | 1065 | 65 |
| threading-tasks | 629 | — |
| threading | 307 | 32 |
| reflection | 33 | — |

即：**这不是 14 个 subject 的问题，是「ATG 如何为无参可构造的引用类型造测试对象」的
统一机制问题。** 每一条 `Create<SomeClass>()` 都建立在一个隐含假设上：
"零初始化的对象对该类型的方法调用是安全的"。

对**纯数据容器**（如 `List<int>`）该假设大致成立；对**持有不变量/资源的类型**
（锁、事件、数据库连接、需要 ctor 建立内部状态的任何东西）该假设**必然不成立**。

## 一、问题陈述（可证伪的形式）

> 对 `T` 为引用类型且 `T` 有无参构造函数时，
> `Create<T>()` 返回的对象**不具备 `T` 的不变量**，
> 导致 `T` 的实例方法在"对象存在"与"对象可用"之间的语义鸿沟里给出错误结果，
> 而这些错误结果**被记成 subject 失败**，掩盖了"这个 subject 从一开始就
> 不可能通过"这一事实。

## 二、证据（实测，非推断）

### 2.1 三类表现，语义各不相同

| 类 | 代表 subject | `Create<T>` 后发生了什么 | 运行时行为是否正确 |
|---|---|---|---|
| **A** | `Create<ReaderWriterLockSlim>().TryEnterReadLock(0)` | handle 字段=0 → `BoundHandle` 返 0 → 答「未获取」→ 断言 `true` 失败 | **对象不成立**（锁从未被创建） |
| **B** | `Create<SpinLock>().Enter(ref taken)` | `SpinLock` 是 **struct** → 走 `default(T)` 分支，非 GUO；槽是**合法**零锁字 | 对象成立；但被 native 侧一个**过时的对象头守卫误判**而拒绝 |
| **C** | `Create<ManualResetEventSlim>().Wait()` | 新建非信号事件 + 无限等待 | **符合 .NET 语义**（真 .NET 也永久阻塞）→ subject 本身不可满足 |

> B 类是本轮新认知：`SpinLock` 的失败**不是** `Create<T>` 的问题，而是我方
> 一个在 receiver 注入后已过时的防御守卫在误伤合法输入。

### 2.2 同族先例已被判给 ATG（重要边界信号）

main 已归档的 `threading-tasks-hang/diagnosis.md` 对 `TaskTests::Wait_7__0`
挂死的结论是：

> **关键判断：运行时行为符合 .NET 语义** …… 正确修法在 ATG/fixture 侧

C 类与它是同一族。**即：并非所有此类失败都该在运行时侧修。**

### 2.3 家长 roadmap 的边界裁决

`threading-receiver-plan/roadmap-v1-01.md:35` 的「不做」清单：

> **`SubjectInstanceFactory.Create<T>` 泛型降级 → 非 threading 域**

本议题若严格按此边界，**不在** threading receiver-plan 范围内。这是本设计
要重新裁决的第一个点。

## 三、核心待决问题

见 `STATUS.md` 的 `blocking_questions`。核心是：

**Q1. `Create<T>()` 的契约到底是什么？** 三种候选语义：
  - (a) "零初始化的 T 实例"（现状）
  - (b) "T 的可用实例"（须真正构造）
  - (c) "T 的可用实例，无法构造时显式失败"

**Q2. 修复责任在哪一层？** ATG（不生成此类 subject）/ CodeGen（生成真实 ctor 调用）/
     native（按类型补状态载体）——三者成本与正确性差异很大。

**Q3. 对无法满足的 subject（C 类）如何**如实**处理？** 不能让它挂着不回，
     也不该伪造成"通过"。

## 四、方案对比、风险评估、三优先级权衡

> **已收敛**：Q1–Q3 拍板与完整方案对比 / 风险评估 / 三优先级权衡
> 见 `design-v1-01.md`（本文件只保留立项时的背景与问题界定，
> 不重复设计结论，避免两处口径漂移）。

三个问题的最终裁决：

| 问题 | 裁决 |
|---|---|
| Q1 `Create<T>()` 的契约 | **(c) 真构造 + 无法构造则显式失败** |
| Q2 修复责任层 | **改 SDK 的 `SubjectInstanceFactory` 走真构造** |
| Q3 无法构造如何处理 | **维持现有 `throw`**（落 `factoryGap`） |

