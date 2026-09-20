# 方案 C 实施方案 — 让 display-class 构造器也能正常发射（根除 `<>c::` 特例）

> task_id: `closure-lambda-emitting`
> 类型：design（dev-brainstorm 产出，方案 C 深度展开）
> 日期：2026-09-20
> 上级：`closure-lambda-emitting/design-v1-01.md`（方案 A/B/C 对比）
> 状态：**Spike 已证实 C1 成立**（见文末附录）

## 〇、为什么现在展开 C

用户要求看 C 的实施方案。深入验证后发现 **C 的可行性比 design-v1-01 假设的更高**，
关键证据如下。

### C 立论修正（推翻 design-v1-01 的两处假设）

| design-v1-01 假设 | 实测 | 影响 |
|---|---|---|
| ctor 的 `newobj` 无法结构化发射（畸形 C++ `auto chaos_value = return`） | `EmitLinearNewObject` **已支持** 无参 reference-type newobj（`Linear.cs:529-535`）；"畸形代码"只在注释里，代码中无对应产物 | **该假设不成立** —— `<>c::.cctor` 可能本就可发射 |
| 24 个 ctor 拿回真实 body 风险高 | `.ctor`（`ldarg/call/ret`）与普通测试类 ctor **IR 相同**，那些现在正常发射 | **风险比预期低** —— 两个形态都已具备发射路径 |

### 建立"可行性断言必须先验证"（Spike 先行）

**不能直接据上述推断就删 guard** —— 注释所述失败可能来自陈旧的某条路径。
必须先做一个**最小 spike**：临时允许 `<>c::.cctor` 走正常发射，构建 threading chunk，
看是否产生畸形代码 / 编译是否通过。

**Spike 判据**：
- ✅ 12 个 lambda + 6 个 ctor（含 cctor）全部拿到真实 body，编译通过、link 成功
  → C 完全可行 → 走 C1
- ⚠️ ctor 可、cctor 挂（newobj-stsfld 组合有隐藏问题）→ 走 C2（.cctor 单独留 fallback）
- ❌ 两者都挂 → 走 C3 / 退回 A

---

## 一、目标

**消除 `MethodEmission.cs` 里整条 `<>c::` 特例**，让 display-class 构造器
（`.ctor` + `.cctor`）与 lambda body 一样走正常结构化发射。

端到端收益：**224 个方法**（24 ctor + 200 lambda）全部拿到真实 body，
不再有任何 `<>c::` 方法落 `ChaosExternalRuntimeFallback` 假体。

## 二、变体

### C1：整条删除 `<>c::` guard（治本，推荐先行 Spike）

```csharp
// 删除整个 if (method.SubjectId.Contains("<>c::")) {...} 块
```
让 `EmitManagedMethod` 对 `<>c::*` **一视同仁**走正常发射。

- **前提**：Spike 证明两种形态（newobj+stsfld / ldarg+call）都能正常 lower
- **代价**：最小 —— 删代码，不加新逻辑
- **收益**：最大 —— 224 个全部解封；整类"名字启发式"消失
- **风险**：依赖 Spike 结论；若 `stsfld` 写 `<>9` 静态字段在此路径有隐藏问题则挂

### C2：只留 `.cctor`（newobj）特例，`.ctor` + lambda 解封

```csharp
var _isDisplayClassCtor = method.SubjectId is not null
    && (method.SubjectId.EndsWith("::.ctor")  || method.SubjectId.EndsWith("::.cctor"))
    && IsStaticCtorWithNewobj(...);  // 精确判定
```

- **适用**：C1 spike 发现 cctor 的 newobj-stsfld 组合确实挂，但 .ctor 没问题
- **代价**：中 —— 保留特例但精确化（只留真正需要的形态）
- **收益**：226-24（至少 .ctor+lambda）= 解封 .ctor + 200 lambda
- **风险**：中 —— 仍需精确判定"哪种 ctor 形态不能发射"

### C3：修 .cctor 的 stsfld 发射路径（真正治本，不删 guard 也可行）

若 Spike 定位到 `.cctor` 的 `newobj→stsfld` 具体挂点（比如 `stsfld` 的
eval-stack 消费顺序），**修那个发射点**，然后整体删除 guard。

- **代价**：中-大 —— 触及 StructuredIR 的 stsfld 处理
- **收益**：与 C1 同（224 全解封），且**不依赖"本来就能发射"的假设**
- **风险**：中 —— stsfld 在 eval-stack 路径上可能牵连其他模式

### C4：保留 guard 但改为「发射失败才回落」（能力感知）

```csharp
try { EmitNormalBody(...); }
catch (NotSupportedException) { EmitFallbackStub(...); }   // 精确到这条方法
```

- **代价**：中 —— 需要 `EmitNormalBody` 可安全重试 / 可预测抛错
- **收益**：**自愈式** —— 未来任何新形态如果发射不了，自动回落到 fallback
  （而非像现在这样"名字匹配就全拦"）
- **风险**：高 —— 发射器抛异常可能在中间状态（builder 已写了一半）；需确保可回滚

---

## 三、深度分析

### 3.1 为什么 C1 现在比 design-v1-01 时可行（关键证据）

`EmitLinearNewObject` 的三个分支，恰好覆盖 `.cctor` 的 newobj 所需：

```csharp
// 无参 reference-type 构造器（.cctor 的目标 <>c::.ctor 就是无参）
if (TryResolveDirectInvocationTarget(instruction.Callee) is { } constructorTarget)
{
    if (constructorTarget.ParameterAbis.Count == 0)
    {
        // NEW_GC + 设 type_info + push —— 已是完全支持的生产路径
    }
}
```

这条路径**已有生产用例**（注释自己写 `e.g. ArrayList::.ctor()`）。说明
`newobj` 结构化发射并非 display-class 专属难点 —— 它是发射器普遍支持的。

`stsfld` 同理：`EmitInstruction.cs:1209` 有完整 case，且 `EnsureInitialized`
生成体里已出现 `chaos_static_...___9__14_0` 的写入（那正是 `stsfld` 产物）。

**所以 `.cctor` 的 IR（newobj→stsfld→ret）在现有发射器里既有对应分支。**

### 3.2 残余未知（正是 Spike 要回答的）

| 未知 | 为什么可能是坑 |
|---|---|
| comment 所述 `auto chaos_value = return` 到底来自哪段历史 | 无对应代码 → 可能早已修好，也可能来自**另一条路径**（如 JIT 而非 AOT 发射） |
| `stsfld` 的 eval-stack 消费顺序是否对 `newobj` 压入的对象正确 | `stsfld` 要消费栈顶值 —— newobj 压入后 stsfld 消费。需确认 structured 路径的栈平衡 |
| `<>c::.cctor` 被谁调用 | `chaos_generated_module.cpp:6381` 显示 `try { ..._c_cctor(); } catch` —— cctor 是被**模块初始化**调用，含 catch。若 cctor 正常发射后这里行为变化需复核 |

### 3.3 风险 vs 收益 vs 成本（与 A/B 对比）

| | A（收窄判据） | C1 | C2 | C3 | C4 |
|---|---|---|---|---|---|
| 解封方法数 | 200 lambda | **224** | 212+ | **224** | 224 |
| 改动量 | 小 | 最小（删代码） | 中 | 中-大 | 中 |
| 新代码 | 1 helper | 0 | 1 helper | 修 1 点 | 重试机制 |
| 依赖 spike | 否 | **是** | 是 | 是 | 是 |
| 根治"名字启发式误伤" | ❌ | ✅ | ⚠️ 部分 | ✅ | ✅ |
| 未来新形态自愈 | ❌ | ✅ | ⚠️ | ✅ | ✅ |

### 3.4 三优先级

| P1 性能 | P2 架构 | P3 热更 |
|---|---|---|
| C1/C3：多发射 224 个 body（运行更快）| C1/C3：**消除整类特例**，最符合 P2 | 皆中性 |

P1/P2 **方向一致支持 C1/C3**（比 A 更好 —— A 只是收窄，仍留启发式）。

### 3.5 与 A 的关系

**C 是 A 的超集**：A 只解 lambda，C 解 lambda + ctor。若 C1 spike 成功，
A 变得多余（guard 整条删除）。故**推荐顺序：先跑 C1 spike，再在 A 与 C1 间定夺**。

---

## 四、实施步骤（C1 路线）

1. **Spike（必须先做，独立于方案选择）**：
   临时把 `MethodEmission.cs:169` guard 的 `Contains("<>c::")` 改为 `false`，
   构建 threading chunk，记录：
   - 6 个 ctor（含 cctor）是否拿到真实 body
   - 编译是否通过（有无 `auto chaos_value = return` 畸形代码）
   - link 是否成功
2. **按 Spike 结果分流**：
   - ctor+cctor 都行 → C1（删 guard）
   - 只有 ctor 行 → C2（.cctor 留特例 + 精确判定）
   - 找到具体挂点 → C3（修那个点）
3. **C1 落地**：删 guard，跑 threading + threading-tasks 回归
4. **记录**：224 方法解封的 fact 前后对比（不预设全绿 —— 解封后下游独立缺口仍在）

## 五、明确不做

- ❌ 不在本任务做 C4（能力感知重试）—— 它是"未来加固"方向，成本高
- ❌ 不修 `object_stubs.cpp` GUO 挂死 —— 并发会话
- ❌ 不追 `QueueUserWorkItem` 3 参无 native —— 独立缺口

## 六、问题清零（blocking_questions: []）

| # | 问题 | 结论 |
|---|---|---|
| 1 | 先跑 C1 spike 再决 A vs C1？ | ✅ 已跑 —— **C1 成立**（见附录） |
| 2 | 与并发会话撞车？ | ✅ 已用隔离 worktree 规避 |
| 3 | C1 成功后 A 作废？ | ✅ **是** —— C1 是 A 的超集 |

**question_clearance: cleared**

---

# 附录：Spike 实测结论（2026-09-20）

## 结论：**C1 成立** —— 整条 guard 可以删除

### 实验设置

隔离 worktree `.claude/worktrees/spike-c1`（从 `bdf0630ba` 切出，**不含**
并发会话的 GUO 挂死 WIP），临时把 `MethodEmission.cs:169` 的 guard 判定
改为 `false && ...`，跑 `threading` chunk 的 build。

### 实测结果（全部判据通过）

| 判据 | 旧行为 | **Spike 结果** |
|---|---|---|
| display-class stub 数 | 18 | **0** ✅ |
| 编译错误 | — | **0** ✅ |
| 畸形代码 `auto chaos_value = return` | — | **未出现** ✅ |
| link | — | **成功**（`1/1 passed`，724 subjects）✅ |
| `.cctor` body | fallback 假体 | **真实 body**：`NEW_GC` 分配 + 设 `type_info` + 调 `.ctor` ✅ |
| lambda `b__14_0` body | fallback 假体 | **真实 body**：`_s0 = 0; return (INT32)_s0` ✅ |

### 关键发现

1. **那条 guard 的注释理由已过时**。注释称 ctor 的 `newobj`
   会被发射成畸形 C++（`auto chaos_value = return`）——
   **Spike 中该现象完全未出现**。`EmitLinearNewObject` 的无参
   reference-type 分支（`Linear.cs:529-535`）已能正确处理
   `<>c::.cctor` 的 `newobj`，`stsfld` 亦有完整支持。

2. **`<>c::.ctor` 本就不是问题** —— 其 IR（`ldarg/call/ret`）与
   正在正常发射的普通测试类 ctor 完全相同。

3. **产物体积反而更小**：
   `entry.exe` 14,131,200 vs 旧 14,404,096（**−273 KB**）。
   原因：`fallback 桩 + try/catch + 异常路径` 比
   `平凡 body` 更大。**印证了性能分析中"修复是净收益"的判断。**

### 分流决定

| 结果 | 采用 |
|---|---|
| ✅ **全过** | **C1 —— 删除整条 guard** |

C2（只留 `.cctor`）与 C3（修挂点）**均不需要** —— 没有任何形态发射失败。

### 落地范围

- `MethodEmission.cs`：删除 `:169-200` 的整段 `<>c::` 特例
- 连带清理：`IsLambdaBodySubjectId` helper（若由并发会话 WIP 引入则一并处理）
- 预期解封：**224 个方法**（24 ctor + 200 lambda）

### ⚠️ 遗留验证（未做，Spike 只跑 build）

- **fact 阶段未跑** —— Spike worktree 缺 chunk 运行环境
- 需在落地后于干净树跑 `build,fact`，确认：
  - (a) 那 12/122 个 lambda 相关 subject 的 fact 结果变化
  - (b) **不预设全绿** —— 解封后下游独立缺口仍在
    （如 `QueueUserWorkItem` 3 参无 native）
