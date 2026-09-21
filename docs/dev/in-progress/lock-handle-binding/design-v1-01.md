# design-v1-01 — Create&lt;T&gt; 对引用类型改为真构造

> task_id: `lock-handle-binding`
> 日期：2026-09-20
> 类型：跨域设计（运行时 native + CodeGen + ATG/测试基建）
> 上游：`threading-receiver-plan`（Phase A receiver 注入，已完成）、
>       `threading-tasks-hang`（同族 ATG 侧挂死诊断，已归档）

---

## 一、架构映射

本设计触及四层中的三层，改动集中在一层：

| 层 | 文件 | 是否改动 | 职责 |
|---|---|---|---|
| **ATG** | `src/tools/.../AutoTestGenerator/` | ❌ 不改 | 生成 subject；本设计不动其产出 |
| **CodeGen** | `src/managed/Chaos.IL2CPP.Generator/` | ❌ 不改 | 把 `Create<T>()` 按托管语义降低（自动跟随源码变化） |
| **SDK/fixture** | `src/reference/Chaos.TestFramework.Sdk/SubjectInstanceFactory.cs` | ✅ **主改点** | 决定「引用类型如何获得实例」 |
| **运行时 native** | `src/native/runtime-core/reflection/invoke.cpp` | ✅ **补一处** | `ChaosReflectionCreateInstance` 无参分支补 ctor 调用 |

**关键结构事实（实测）**：`Create<T>()` 是**按实例化降低的托管代码**
（`Create_ReaderWriterLockSlim__generic` 是真实函数体，非 stub）。
所以改 C# 源码即可，codegen 自动跟随——**不需要**新增 shape 或改 lowering。

---

## 二、问题陈述

`SubjectInstanceFactory.Create<T>()` 对引用类型统一返回
`RuntimeHelpers.GetUninitializedObject(typeof(T))`：**零初始化、不跑构造函数**。

实测规模（`CombinedSubjects.cs` 中 `Create<` 出现次数 / 不同类型数）：

| chunk | 调用点 | 类型数 |
|---|---:|---:|
| system | 1065 | 65 |
| threading-tasks | 629 | — |
| threading | 307 | 32 |
| reflection | 33 | — |

即：**这不是 14 个 subject 的局部问题**，而是「ATG 如何为引用类型造测试对象」
的统一机制问题。它建立在隐含假设「零初始化对象对该类型的方法调用是安全的」上；
对**持有不变量/资源的类型**（锁、事件、需要 ctor 建立内部状态的任何东西）
该假设必然不成立。

---

## 三、证据：三类失败，语义各不相同

| 类 | 代表 subject | 机制 | 运行时行为 |
|---|---|---|---|
| **A** | `Create<ReaderWriterLockSlim>().TryEnterReadLock(0)`（6 项，si=211,212,214,215,217,218） | handle 字段=0 → `BoundHandle` 返 0 → 答「未获取」→ 断言 `true` 失败 | **对象不成立**（锁从未被创建） |
| **B** | `Create<SpinLock>().Enter(ref taken)`（4 项，si=261..264） | `SpinLock` 是 **struct** → 走 `default(T)`，**非** GUO；槽是**合法**零锁字 | 对象成立；被**我方一个过时的对象头守卫误判**而拒绝 |
| **C** | `Create<ManualResetEventSlim>().Wait()` | 新建非信号事件 + 无限等待 | **符合 .NET 语义**（真 .NET 同样永久阻塞） |

**B 类更正了本轮早先的判断**：`SpinLock` 的失败**不是** `Create<T>` 的问题，
而是 `LooksLikeObjectHeaderNotLockWord` 守卫（提交 `25e454f24`）在 receiver
注入（`aa55b5e87`）之后**已过时**——它假设接收器可能是对象指针，现在传的是
真实值类型槽，守卫在**误伤合法输入**。

**C 类与已归档结论同族**：`threading-tasks-hang/diagnosis.md` 对 `Wait_7__0`
的判定是「运行时行为符合 .NET 语义 …… 正确修法在 ATG/fixture 侧」。

---

## 四、边界拍板（已与用户确认）

| 问题 | 裁决 | 理由 |
|---|---|---|
| **Q1** `Create<T>()` 对可构造引用类型的契约 | **(c) 真构造 + 无法构造则显式失败** | 契约是"T 的可用实例"，不是"零初始化的 T 实例" |
| **Q2** 修复责任层 | **改 SDK 的 `SubjectInstanceFactory`，走真构造** | 复用既有 `ChaosReflectionCreateInstance`；`Create<T>` 是托管降低，改源码即可 |
| **Q3** 无法构造时如何失败 | **维持现有 `throw`**（落 `factoryGap`） | 见 §五，源码已有血泪注释证明返 null 会 `ud2`→SIGILL→整 chunk 归零 |
| **推进方式** | **先探针 threading 一个 chunk** | system chunk 65 类型/1065 点，先量数字再全量 |

### 与上游 roadmap 的边界关系（重要）

`threading-receiver-plan/roadmap-v1-01.md:35` 的「不做」清单写着
「`SubjectInstanceFactory.Create<T>` 泛型降级 → 非 threading 域」。

**本设计不与之冲突**：upstream 排除的是"在 threading 任务里顺手改泛型降级"，
而本设计把该机制**独立立项**为 `lock-handle-binding`，范围显式覆盖
runtime + codegen + ATG 三层。upstream 的 receiver 注入（Phase A）**已完成且
是本设计的必要前提**——没有 receiver，真构造出来的对象也送不进实例方法。

---

## 五、Q3 的依据（源码血泪注释，非推断）

`SubjectInstanceFactory.cs` 原文：

> The null check below is **load-bearing**, not defensive noise. The cast used
> to carry a null-forgiving `!` …… the AOT codegen emits that as a real `ud2`
> trap after the call. Under AOT, `GetUninitializedObject` returns null for any
> type the codegen could not give a native body …… so the trap is genuinely
> reachable: it **kills the process with SIGILL mid-fact-loop, which truncates
> the streaming factResults JSON and reports the whole chunk as "0/0 passed"**
> instead of failing one subject.
>
> **Throwing instead makes the failure land where the harness can attribute it**

**这与本轮实测吻合**：我的 MRE 懒创建挂死时，chunk 正是
`0/0 passed / timed_out` 全空。故 **Q3 维持 throw，不改为返回 null**。

分类器核查（实测 `si=24`）：`factoryGap` 记录形态为
`caught=true, assertFailed=false` —— 靠 `assertFailed=false` 与"真断言失败"
区分，与抛异常**不冲突**。

---

## 六、方案对比

### 6.1 主方案对比（Q2 已裁决，保留记录）

| 方案 | 改什么 | 成本 | 正确性 | 裁决 |
|---|---|---|---|---|
| **Q2-a** | 改 SDK factory → `Activator.CreateInstance(typeof(T))`；补 native 无参 ctor | 小 | 从根上实现 Q1(c) | ✅ **采纳** |
| Q2-b | 只补 native `ChaosReflectionCreateInstance` 无参 ctor | 小 | **不解决问题**（factory 不调 Activator） | ❌ |
| Q2-c | native 按类型补状态载体（懒创建） | 中，逐类型 | 治标；MRE 会挂（实测） | ❌ |

### 6.2 文件落地方案对比（架构评估要求）

| 方案 | 落点 | 架构优劣 | 推荐 |
|---|---|---|---|
| **F1** | 只改 `SubjectInstanceFactory.cs` + `invoke.cpp` 无参分支 | 最小面；复用既有层；不新增抽象 | ✅ |
| F2 | 新增一个 `IPostConstructionHook` 抽象，让各类型注册"构造后初始化" | 引入新抽象层，为 14 个 subject 造框架——**YAGNI** | ❌ |
| F3 | 在 codegen 侧特判 `Create<T>` 的调用点 | 违反「codegen 不特判具体 subject」的既有分层 | ❌ |

**架构影响评估**：F1 **不改变**任何现有模块职责边界，不引入新依赖方向，
不破坏分层。`ChaosReflectionCreateInstance` 本就是"真构造"语义的既有归属，
补其无参分支是**补全**而非新建。

---

## 七、风险评估

| # | 风险 | 触发条件 | 后果 | 缓解 |
|---|---|---|---|---|
| R1 | **大量 subject 行为改变** | factory 改真构造后，所有引用类型 subject 真跑 ctor | 现在"过"着的可能变红（如 ctor 抛异常/有副作用） | **先探针 threading 一个 chunk**，量出转绿/转红比；不对就不全量铺开 |
| R2 | **性能退化（P1）** | 真构造 vs `GetUninitializedObject` | 每个 `Create<T>` 多一次 ctor 调用 | 仅在**测试 fixture** 路径，不进生产 codegen 热路径；且 subject 数量级小 |
| R3 | **无参 ctor 不存在的类型** | 抽象类 / 无 public 无参 ctor | 报错 | 落 `factoryGap`（Q3 已定），**显式失败而非静默坏对象** |
| R4 | **ctor 副作用** | 如 `Timer` 启动计时、`Mutex` 占系统资源 | 资源泄漏 / 行为差异 | 探针阶段统计；必要时该类转 ATG 侧排除 |
| R5 | **C 类仍阻塞** | MRE 等：构造成功但 Wait 永久阻塞 | chunk 挂起 → 全 0 | `47dab8dff` 已修（超时保留已完成结果）；且 C 类归 ATG |
| R6 | **主树并发度高** | 当前有 9 个其他会话未提交改动 + 2 条 pipeline | 归因错乱 | 在 worktree 隔离实施；跑前核对 `git status` |

---

## 八、三优先级权衡

| 优先级 | 本方案表现 | 说明 |
|---|---|---|
| **P1 性能最优** | ⚪ 中性 | 改动只在**测试 fixture** 路径（`Chaos.TestFramework.Sdk`），**不进生产 codegen 热路径**。真构造的额外开销限于 fact/benchmark 的 subject 建立，不在被测方法本体内 |
| **P2 架构完美** | ✅ 强 | 让契约自洽：`Create<T>()` 名副其实地"创建可用实例"；复用既有 `ChaosReflectionCreateInstance`，不新增抽象 |
| **P3 热更适配** | ⚪ 中性 | 不触及 hotpatch/interpreter 路径 |

**裁决点**：无优先级冲突——P2 改善不以牺牲 P1 为代价（不在热路径）。
若探针发现 R1 导致大面积转红，则**回退到 Q2-c**（按类型补丁），
即用 P2 让位于 P1 的稳定性诉求。

---

## 九、组件与数据流

### 9.1 组件职责

| 组件 | 做什么 | 依赖 | 可独立测试？ |
|---|---|---|---|
| `SubjectInstanceFactory.Create<T>()` | 值类型→`default(T)`；引用类型→**真构造** | `Activator.CreateInstance` | ✅（SDK 单测） |
| `ChaosReflectionCreateInstance` | `class_init` → `object_new` → **找并调 `.ctor`** | runtime ABI | ✅（native 契约测试） |

### 9.2 目标数据流

```
Create<T>() [引用类型分支]
  └─> typeof(T) → Activator.CreateInstance(Type)
        └─> ChaosReflectionCreateInstance(type_handle, args=0)
              ├─ class_init(runtime, type)              // 静态构造
              ├─ object_new(runtime, thread, type)      // 分配
              └─ type_find_method(type, ".ctor", 0)     // ← 当前缺失
                 └─ method_invoke(...)                  // ← 当前缺失
        └─> 失败（无 ctor / ctor 抛）→ throw → factoryGap
```

**当前缺口精确定位**：`invoke.cpp` 的 `if (args != 0)` 分支才找 ctor；
`args == 0` 时**直接 `return obj`**。需补无参分支。

---

## 十、验证方式

1. **SDK 单测**：`Create<T>()` 对有无参 ctor 的类型返回**已构造**实例（可用一个带可观察副作用的类型断言）；对无 ctor 类型抛异常
2. **native 契约测试**：`ChaosReflectionCreateInstance` 无参路径确实调用了 `.ctor`（用带静态计数的测试类型）
3. **探针（关键一步）**：threading chunk 改前后对比
   - 目标：A 类 6 项 `failed` → `real`
   - 观测：R1 风险的实际转红数量
   - 判据：转绿 > 转红 且无新增挂起，才继续全量
4. **全量**：探针通过后跑 system / threading-tasks / reflection 对比

---

## 十一、非目标

| 排除项 | 理由 |
|---|---|
| C 类（MRE 等必然阻塞的 subject） | 运行时行为符合 .NET 语义；归 ATG，与 `Wait_7__0` 同族 |
| B 类撤守卫所需的 SpinLock 值语义调整 | 独立小改动，不属本设计的"真构造"主线，作为**前置清理**单独处理 |
| async/Task 家族 | 上游 roadmap 已排除 |
| 生产路径的 `GetUninitializedObject` 语义 | 本设计只动**测试 fixture** 的 factory |
