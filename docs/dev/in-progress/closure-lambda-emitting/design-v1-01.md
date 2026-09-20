# 闭包 lambda 被误判为 display-class 构造器 — 设计（dev-brainstorm）

> task_id: `closure-lambda-emitting`
> 类型：design（brainstorm 产出）
> 日期：2026-09-20
> 上游：`il-data-table-not-registered/closure-body-rootcause.md`（并发会话独立坐实根因）
> 状态：**设计已定稿**（用户拍板，见 §9 清零）

## 一、问题陈述

`NativeAotLoweringPlanner.MethodEmission.cs:169` 用一条按 `<>c::` 的**前缀规则**
决定"要不要放弃发射真实 body、改发 `ChaosExternalRuntimeFallback` 假体"：

```csharp
if (method.SubjectId is not null && method.SubjectId.Contains("<>c::", StringComparison.Ordinal))
{
    // emit ChaosExternalRuntimeFallback(...) stub
}
```

该规则**本意**是给 display-class 构造器（`<>c::.ctor` / `.cctor`）兜底 ——
它们的 `newobj` 结构化发射器处理不了。但它**无差别匹配了整个 `<>c::` 前缀**，
把**同前缀下本可正常翻译的 lambda body（`<>c::<Enclosing>b__N`）**也一并降级。

**后果**：lambda 的 IR 是正确的（`has-canonical-body`，IL 1-2 条），
但发射出来是 fallback 假体 → 运行时走 Phase 3 → `NotImplementedException`
→ 外层 `caught` → 测试失败。

## 二、定量（本设计的立论基础，全部实测）

### 2.1 误伤规模（CoreLib 4 个相关 chunk）

| chunk | ctor（正目标） | **lambda（被误伤）** |
|---|---:|---:|
| system | 4 | **54** |
| threading | 6 | **12** |
| threading-2 | 6 | **12** |
| threading-tasks | 8 | **122** |
| **合计** | **24** | **200** |

**200 个 lambda 被降级**，其中 threading-tasks 占 122。

### 2.2 被误伤 lambda 的可翻译性

| 判据 | 值 |
|---|---|
| `bodyAvailability` | 全部 `has-canonical-body`（122/122） |
| IL 指令数 | `{1: 46, 2: 76}` —— **全部 ≤2 条** |
| 指令数 >10 的 | **0** |

即：`() => {}` / `() => default(T)` 级别的平凡 body，**无任何 lower 难度**。

### 2.3 `b__` 判据的完备性

在 CoreLib 7 个 chunk 中检索编译器生成形态：

| 形态 | 命中数 |
|---|---:|
| `b__`（lambda） | 200 |
| `g__`（局部函数） | **0** |
| `<...>d__N`（状态机） | **0** |
| `<>c__DisplayClass` | **0** |

**当前 chunk 集合中 `<>c::` 家族恰好只有 ctor 与 lambda 两类**，
`b__` 判据在本集合上充分。⚠️ 但这是**现状**，不是**契约** —— 见 §5 风险。

## 三、现状：修复已写、未提交、验证受阻

| 项 | 状态 |
|---|---|
| 修复代码 | 在**工作区**（`MethodEmission.cs` 的 `M`），**未提交** |
| 修复逻辑 | `<>c::` 且非 `b__` → fallback；`b__` → 正常发射 |
| 独立验证 | ✅ 逻辑正确（在 `b__14_0` 上逐步验证过） |
| 产物验证 | ❌ **未完成** —— threading 产物仍是 09-18 21:08 的旧行为 |

**验证受阻原因（并发会话已定论）**：另一会话的 `1c830ad24` WIP
（`object_stubs.cpp` 的 `GetUninitializedObject`）会让 fact 阶段**挂死**
（`si=218` 忙等）。其隔离实验证明**与本 lambda 修复无关**。

## 四、方案

### 方案 A：收窄判据（**推荐**）

**做什么**：把 `Contains("<>c::")` 改为「`<>c::` **且** 不是 lambda body」。

```csharp
var _isDisplayClassCtor = method.SubjectId is not null
    && method.SubjectId.Contains("<>c::", StringComparison.Ordinal)
    && !IsLambdaBodySubjectId(method.SubjectId);
```

**判据选型**（本方案的关键决策）：

| 备选判据 | 评价 |
|---|---|
| `tail.Contains("b__")`（WIP 采用） | 简单；对当前集合充分；但**只认 lambda** |
| `tail.StartsWith(".")` 反向判定 | **更稳**：ctor 是唯一点号开头的成员；其余一律正常发射 |
| 白名单 `.ctor`/`.cctor` | 显式、可读；与"正目标只有这两类"的事实一致 |

**✅ 用户拍板：沿用 `b__` 正向判定**（即并发会话 WIP 的写法）。

理由（记录取舍）：对**当前** CoreLib 集合 `b__` 判据充分
（实测 `<>c::` 家族恰好只有 ctor 与 lambda 两类，无 `g__`/`d__`）。
改动最小、与既有 WIP 一致、可立即验证。

⚠️ **接受的代价**：这是**正向白名单**，未来若出现局部函数 `g__`、
状态机 `d__` 等新形态，会再次落入 fallback。已登记为 §8 的后续加固项
（「改为反向判定」），不在本任务范围。

**代价**：小（一个判定 + 一个 helper）。
**收益**：解封 200 个 lambda 的真实 body。
**风险**：低（不改 ctor 行为；不新增发射路径）。

### 方案 B：按「发射能力」判定（治本，工作量大）

不做前缀/名字启发式，改为**问结构化发射器能不能 lower**：
- `newobj` 在 eval 栈上的形态 → 无法结构化 → 走 fallback
- 否则 → 正常发射

**代价**：中-大（需要一个能力探测，且要与 StructuredIR 的既有假设对齐）。
**收益**：彻底消除这类"名字启发式误伤"的整族问题。
**风险**：中（能力探测若与发射器实际行为不符，会引入新的一类静默错误）。

### 方案 C：让 ctor 也能正常发射

修掉"ctor 的 newobj 无法结构化"这个**根因**，然后整条 `<>c::` 特例消失。

**代价**：大（触及 StructuredIR 的核心假设）。
**收益**：最大 —— 24 个 ctor 也拿回真实 body。
**风险**：高（`auto chaos_value = return` 那类畸形代码说明这条路径很脆）。

## 五、风险（逐项）

| 风险 | 等级 | 说明 | 缓解 |
|---|---|---|---|
| **验证被并发 WIP 阻塞** | 🔴 高 | `1c830ad24` 的 GUO 挂死使本树无法产出干净 threading 基线 | ✅ 已定：**切不含该 WIP 的干净 worktree** 验证 |
| `b__` 判据不完备（未来出现 `g__` 等） | 🟡 中 | 当前集合无此类，但 C# 还会生成局部函数 `g__`、状态机 `d__` | **用户已接受该代价**；加固项登记于 §8 |
| 修复后编译/运行变慢 | 🟡 中 | 多发射 200 个真实 body | 实测 build 耗时与包体大小 |
| 解封后**下游仍有独立缺口** | 🟡 中 | 并发 doc 已指出：`QueueUserWorkItem` 3 参重载本身无 native | 预期**不全部转绿**，逐项记录 |
| 与并发会话撞车 | 🟡 中 | 修复是对方 WIP | 开工前确认归属 |

## 六、三优先级权衡

| 优先级 | 方案 A | 方案 B | 方案 C |
|---|---|---|---|
| **P1 性能最优** | ➖ 中性（多发射 200 个平凡 body，运行更快） | ➖ 中性 | ➖ 中性 |
| **P2 架构完美** | ⚠️ 仍是名字启发式 | ✅ 按能力判定 | ✅ 消除特例 |
| **P3 热更适配** | ➖ 中性（不触碰 hotpatch 路径） | ➖ 中性 | ➖ 中性 |

**无 P1/P3 冲突。** P1 是最高优先级，三个方案在它上面等价，因此
**架构完美性（P2）成为实际决策依据** —— 但方案 B/C 的**风险**高于 A。

**推荐 A**：在 P1 等价的前提下，A 的成本/风险最低且**立刻解封 200 个 body**；
B/C 的架构收益应作为**后续独立任务**（见 §8）。

## 七、落地路径（方案 A）

1. ~~确认归属~~ → ✅ 用户拍板：**本任务直接接手实现并提交**
2. ~~判据定稿~~ → ✅ 用户拍板：**沿用 `b__` 正向判定**
3. ~~验证环境~~ → ✅ 用户拍板：**切不含 `1c830ad24` 的干净 worktree 验证**
4. **端到端验证**：threading chunk 上
   - (a) 18 个 display-class stub → 6 个（只余 ctor）
   - (b) 12 个 lambda 出现真实 body
   - (c) 对应 subject 的 fact 结果变化（记录实际数字，**不预设全绿**）
5. **回归**：至少再跑 1 个非 threading chunk（threading-tasks，122 个 lambda）

**验收判据**：不是"12 个全绿"，而是
**(a) ctor 仍走 fallback 且数量正确；(b) lambda 拿到真实 body**。

## 八、明确不做（非目标）

- ❌ 不在本任务修 ctor 的 newobj 结构化发射（方案 C）—— 独立任务
- ❌ 不做"发射能力探测"的架构改造（方案 B）—— 独立任务
- ❌ 不修 `object_stubs.cpp` 的 GUO 挂死 —— 属并发会话
- ❌ **不把判据改成反向判定/白名单** —— 用户已拍板沿用 `b__`；
  该加固作为后续项登记（见下）

### 后续加固项（登记，不在本任务）

`b__` 是**正向白名单**，只覆盖 lambda。若未来出现：
- `g__`（C# 局部函数）
- `<...>d__N`（async/iterator 状态机）
- 其它 `<>c::` 下新增的编译器生成成员

则会**再次**落入 fallback。加固方向：改为**默认正常发射 + 显式白名单 ctor**。
触发条件：观察到新的 `<>c::` 成员形态落 fallback。

## 九、问题清零（blocking_questions: []）

| # | 问题 | 结论 |
|---|---|---|
| 1 | 判据选型 | ✅ **沿用 `b__` 正向判定**（与 WIP 一致，改动最小） |
| 2 | 归属 | ✅ **本任务直接接手实现并提交** |
| 3 | 验证环境 | ✅ **切不含 `1c830ad24` 的干净 worktree 验证** |

**question_clearance: cleared**

### 接受的代价（明确登记）

- `b__` 是正向白名单 → 未来新闭包形态会再次误伤 → §8 后续加固项
- 验证期间不碰并发会话的 GUO WIP

## 十、方法学教训（沉淀）

1. **「符号没有 body」要分两种**：真无 body vs **body 是 fallback stub**。
   grep 主体文件会漏 page 文件 → 误报"无 body"。**判据要跨文件**
   （声明 vs `{` 后非 decl 的体）。
2. **前缀规则会静默误伤子类**：写 `Contains("<>c::")` 时，
   先问"正目标长什么样"，再问"还有谁匹配这个前缀"。
3. **并发工作树下跑 pipeline，不能把结果归因给自己的改动** ——
   归因前必须先隔离变量（并发会话的隔离实验是范例）。
