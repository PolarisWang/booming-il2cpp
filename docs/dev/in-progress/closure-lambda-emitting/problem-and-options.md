# 问题展开 + 方案综合分析（含性能）

> task_id: `closure-lambda-emitting`
> 日期：2026-09-20
> 面向：需要完整理解问题与方案取舍的读者

---

# 第一部分：问题是什么

## 1.1 背景：什么是 display class 和 lambda body

C# 编译器在遇到 lambda 时，会生成一个**隐藏的辅助类**（叫 display class，
名字形如 `<>c`），把这个 lambda 变成一个**静态方法**挂在该类上：

```csharp
// 你写的 C#：
Task.WaitAll(tasks)          // 内部：LazyInitializer.EnsureInitialized(..., () => default(int))

// 编译器生成的（示意）：
class <>c {
    public static readonly <>c <>9 = new <>c();    // 单例 (.cctor 负责 new)
    public <>c() {}                                 // .ctor
    public int <EnsureInitialized_3...>b__14_0()    // ← lambda body
        => 0;                                        //    (ldc.i4 0; ret)
}
```

所以一个 display class 会带来**三类**方法：

| 类别 | 名字形态 | IL | 作用 |
|---|---|---|---|
| **单例构造器** | `<>c::.cctor` | `newobj <>c; stsfld <>9; ret` | 建单例 |
| **实例构造器** | `<>c::.ctor` | `ldarg.0; call Object::.ctor; ret` | 基类构造 |
| **lambda body** | `<>c::<Enclosing>b__N` | `ldc.i4 0; ret` 等 | 真正的逻辑 |

## 1.2 缺陷是什么

`MethodEmission.cs:169` 有一条**按名字前缀**的规则：

```csharp
if (method.SubjectId.Contains("<>c::"))     // 只要名字里有 "<>c::"
{
    // 就放弃发射真实 body，改发这个：
    (void)ChaosExternalRuntimeFallback("<subject id>");
    return {};
}
```

这条规则**本意**只针对前两类构造器（注释说它们的 `newobj` 结构化发射器处理不了），
但它匹配的是**整个前缀**，把**第三类 lambda body 也一并拦下**。

**关键**：被拦下的方法，IR 是**正确的**（`has-canonical-body`），
只是**发射阶段被换成了 fallback 假体**。

## 1.3 后果：运行时发生了什么

被拦的方法运行时执行的是：

```cpp
(void)ChaosExternalRuntimeFallback("...b__14_0...");
```

`ChaosExternalRuntimeFallback` 的四个阶段：

| 阶段 | 行为 |
|---|---|
| Phase 0.5 | SIMD 特判（不匹配） |
| Phase 1 | 查 IL 数据表（**死代码**，表从未发布） |
| Phase 2 | **线性扫描 278 条分派表 + 逐条 `strcmp`**（不匹配） |
| Phase 3 | **抛 `NotImplementedException`** |

实测 stderr：

```
Phase 3 catch-all: ...b__14_0 ... — unresolvable subject raises NotImplementedException
[SEH-FAULT] code=0xe0000001 si=141 ...EnsureInitialized_3_int_bool_object_System_Funcint_3
```

**即：本该是 "return 0" 的方法，变成了"线性扫描 278 条 + 抛 C++ 异常"，
异常穿透到外层被 `catch`，测试判定失败。**

## 1.4 规模（实测，非估算）

| chunk | `.ctor/.cctor`（规则本意） | **lambda（被误伤）** |
|---|---:|---:|
| system | 4 | 54 |
| threading | 6 | 12 |
| threading-2 | 6 | 12 |
| threading-tasks | 8 | **122** |
| **合计** | **24** | **200** |

**224 个方法**（24 构造器 + 200 lambda）全部走 fallback 假体。

被误伤 lambda 的性质：**IL 全部 ≤2 条指令**（`{1:46, 2:76}`），
`bodyAvailability` 全为 `has-canonical-body`。没有一个有 lower 难度。

---

# 第二部分：性能分析（重点）

## 2.1 单次调用的代价差异

| | 真实 body（如 `ldc.i4 0; ret`） | fallback 假体 |
|---|---|---|
| 指令数 | ~2 条（`_s = 0; return`） | 见下 |
| 函数调用 | 0（内联可能） | `ChaosExternalRuntimeFallback(...)` |
| 字符串操作 | 0 | `strstr` ×1（Phase 0.5）+ 278 × `strcmp` |
| 异常 | 0 | **构造 + 抛 + 展开 C++ 异常** |
| 量级估计 | **~1 ns** | **~微秒级**（278 次 strcmp + 异常） |

**差异约 3 个数量级。**

## 2.2 为什么异常特别贵

Phase 3 走的是 `RaiseManagedException("System.NotImplementedException", ...)`——
这是 **C++ 异常**。即使有 `try/catch` 包着：

1. 构造异常对象（含字符串拼接）
2. `throw` —— MSVC 的异常抛出在 Windows 上走 SEH，**每次约 1-10 μs**
3. 栈展开到 `catch`（`EmitManagedExceptionCatch` 生成的帧）

**对照**：这些方法本来的工作是 `return 0`。

## 2.3 这段代码在什么路径上

**多数在冷路径**（测试初始化、静态构造、一次性 delegate 建立），
所以**对 benchmark/profile 指标影响有限**。

但有两类例外：

| 场景 | 影响 |
|---|---|
| **lambda 被放在循环里调用** | 每次迭代 ~1μs 而非 ~1ns → 可直接拖垮 benchmark |
| **`GcAllocateFast` 等热路径经 delegate 调用** | 同上 |

**结论：修复在性能上是净收益**（把 3 个数量级的开销消除），
但**收益大小取决于这些方法被调用的频率** —— 冷路径上无所谓，
热路径上影响显著。当前证据不足以断言具体有多少热路径，
**建议：修复后跑 benchmark 对比 `Parallel` chunk 的 hot 方法**。

## 2.4 编译期 / 产物体积代价

修复的**反向**代价：多发射 224 个真实 body。

| 项 | 估算 |
|---|---|
| 每个 body 体积 | 假体 ~200 字节 → 真实 body ~600-1500 字节（含 try/catch） |
| 总增量 | 224 × ~1KB ≈ **~220 KB** |
| 对照 | 单个 chunk 的 `chaos_codegen.lib` 是 **60 MB** |
| 占比 | **~0.4%**，可忽略 |
| 编译时间 | 224 个平凡函数，预计 < 1% 增量 |

**结论：编译期与体积代价可忽略。**

## 2.5 运行时内存

真实 body 含 `CHAOS_IL2CPP_ARRAY(..., 1) chaos_args{}` 等**栈**数组，
无堆分配。fallback 路径反而**每次构造异常对象**（堆分配 + 消息字符串）。

**修复后堆分配减少。**

---

# 第三部分：方案

> 所有方案共同的前置：**必须先做 spike**（见第四部分）。
> 因为方案选择完全取决于「ctor 到底能不能正常发射」。

## 方案 A：收窄判据（只救 lambda）

**做什么**

```csharp
var _isDisplayClassCtor = method.SubjectId is not null
    && method.SubjectId.Contains("<>c::", StringComparison.Ordinal)
    && !IsLambdaBodySubjectId(method.SubjectId);   // 新增：b__ 判定
```

**收益**：解封 **200** 个 lambda。构造器（24）仍走 fallback。
**代价**：1 个 helper + 1 行改动。
**风险**：低 —— 不动构造器行为。
**性能**：消除 200 个方法的假体开销。
**架构**：仍是**名字启发式**。未来出现 `g__`（局部函数）/`d__`（状态机）
会**再次**误伤，需再改一次。

## 方案 B：按「发射能力」判定

**做什么**：不问名字，问发射器"这个 body 能不能 lower"。

**收益**：根治整类问题 —— 未来任何新形态自动正确。
**代价**：需要一个能力探测机制，且要与 StructuredIR 的既有假设对齐。
**风险**：中 —— 探测与发射器实际行为若不符，会引入**新的一类静默错误**。
**性能**：同 C（224 全解封）。
**架构**：最好。

## 方案 C：消除整条特例（治本）

**做什么**：让 `<>c::*` 一视同仁走正常发射，删除整条 guard。

**收益**：解封 **224**（ctor + lambda）。
**代价**：**最小 —— 删代码**。
**风险**：**完全依赖 spike** —— 若构造器确实发射不了，此路不通。
**性能**：同 B，且是**唯一能消除 24 个构造器假体**的方案。
**架构**：消除整类特例。

### C 的子变体

| 变体 | 内容 | 适用 |
|---|---|---|
| C1 | 整条删 guard | spike 证明两种形态都能发射 |
| C2 | 只留 `.cctor`（newobj 那个） | spike 证明 `.ctor` 行、`.cctor` 不行 |
| C3 | 修 `.cctor` 的具体挂点后删 guard | spike 定位到确切挂点 |

## 方案 D：能力感知重试（自愈式）

**做什么**

```csharp
try { EmitNormalBody(...); }
catch (NotSupportedException) { EmitFallbackStub(...); }
```

**收益**：未来任何新形态自动回落，**永不误伤**。
**代价**：中-高 —— 需保证 `EmitNormalBody` 失败时 builder 可回滚
（否则写到一半的畸形代码留在里面）。
**风险**：高 —— 回滚语义难以保证。
**定位**：**未来加固项**，不建议现在做。

---

# 第四部分：我建议的决策路径

## 4.1 为什么必须先 spike

方案选择**完全取决于一个未验证的事实**：构造器能不能正常发射？

- 我做了**静态代码分析**，发现 `EmitLinearNewObject` **已支持**
  无参 reference-type newobj（`:529-535`），`stsfld` 也有完整 case
- 且 `<>c::.ctor` 的 IR 与**正在正常发射的普通测试类 ctor 完全相同**
- **但**那条 guard 的注释声称会产出畸形 C++ —— 而代码里**找不到对应产物**

我看的是代码，注释说的是历史。**两者矛盾，必须实测。**

## 4.2 Spike（约 30 分钟）

临时把 guard 判定改成 `false`，构建 threading chunk，观察：

| 观察项 | 判据 |
|---|---|
| 6 个构造器是否有真实 body | 是/否 |
| 是否出现 `auto chaos_value = return` 畸形代码 | 有/无 |
| 编译是否通过 | 是/否 |
| link 是否成功 | 是/否 |

## 4.3 分流

| Spike 结果 | 采用 |
|---|---|
| 全部通过 | **C1**（删 guard）—— 224 全解封 |
| 只有 `.ctor` 行 | **C2** |
| 定位到具体挂点 | **C3** |
| 都不行 | **A**（保底，仍解 200） |

## 4.4 为什么这个顺序

**C1 是 A 的超集**（224 vs 200），且是**唯一根治**的方案。
如果 spike 半小时就能证实，没有理由先做 A 再回头做 C。

**若 spike 失败**，A 仍是可靠的保底 —— 所以 spike 不是赌博，
而是**用半小时把 A 的收益从 200 提升到 224**。

---

# 第五部分：风险汇总

| 风险 | 等级 | 说明 | 缓解 |
|---|---|---|---|
| spike 证明构造器确实发射不了 | 🟡 | C 不成立，退 A | spike 本身就是探测 |
| 修复后部分测试**仍不绿** | 🟡 | 解封只是拿回 body；下游可能有独立缺口（如 `QueueUserWorkItem` 3 参无 native） | **不预设全绿**，逐项记录 |
| 并发会话在改 codegen，撞车 | 🟡 | 工作区有他人 WIP | 切干净 worktree 隔离 |
| 验证被 GUO 挂死阻塞 | 🔴 | `1c830ad24` 的 WIP 使 fact 阶段挂死 | 用**不含该 WIP** 的 commit 切 worktree |
| 224 个 body 增大产物 | 🟢 | 实测估算 +220KB / 60MB = 0.4% | 可忽略 |
| 名字启发式的未来误伤 | 🟡 | A/B 都有；C1 无 | C1 根治 |

---

# 第六部分：一句话总结

**问题**：一条按 `<>c::` 前缀的规则想救 24 个构造器，却误伤了 200 个
本可正常编译的 lambda，把它们从 `return 0` 变成"线性扫描 278 条 + 抛异常"。

**性能**：单次调用差约 **3 个数量级**；冷路径无感，热路径显著。
修复在性能、体积（0.4%）、内存上**都是净收益**。

**方案**：A（收窄，救 200）／B（能力判定，治本但风险中）／
C（删特例，救 224，代价最小）／D（自愈，成本高，留作未来）。

**决策**：先花 30 分钟 spike 验证"构造器能否正常发射"，
用实测在 A 与 C 之间定夺。**C1 若能成立，它是 A 的超集且根治。**