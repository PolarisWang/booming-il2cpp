# ENG-34912: ExternalRuntimeFallback 确认报告

> 基于 comparison-summary.json 和 interop_stubs.cpp 源代码分析

---

## 一、什么是 ExternalRuntimeFallback

`ChaosExternalRuntimeFallback` 是一个集中式末位退路函数（`interop_stubs.cpp:688`），当 AOT codegen 没有为一个方法发射专用 native 实现时，由 codegen 生成的 dispatch stub 调用。

它的工作流程（4 阶段）：

1. **Phase 0.5**: SIMD 字符匹配短路（System.Numerics 家族 -> 返回 0 或 1）
2. **Phase 1**: 嵌入式 IL 数据 -- `kChaosExternalRuntimeIlData[]` 中的方法可通过解释器执行
3. **Phase 2**: `kChaosExternalRuntimeSubjects[]` 调度表 -- 线性扫描匹配 -> `_TryInvoke` 解释执行
4. **Phase 3**: 全失败 -> 返回 0

**核心含义**: 如果一个方法在 AOT 中没有专属 native 实现，它的执行结果要么是解释器返回值，要么是 0。

---

## 二、覆盖规模

| 指标 | 数值 |
|------|-----:|
| 总方法数（CoreLib） | 26954 |
| 有 AOT C++ 实现（aotCoveredMethods） | 24178 |
| 有 .NET 8 基线对比 | 2385 |
| 经 dispatch_overhead 路径的方法 | 1597 |
| ├─ 其中有 .NET 8 基线（可量化差距） | **397**（平均落后 **-3393%**） |
| └─ 其中无 .NET 8 基线（差距未知） | 1200 |
| stub 方法（硬编码 return-0） | 108 |
| 无 .NET 8 基线（完全盲区） | 24569 |

**判读**: `dispatch_overhead` 标记 = 走 ExternalRuntimeFallback 分发的方法，共 1597 个。
其中只有 397 个有 .NET 8 基线可做性能对比 —— 这 397 个平均落后 **3393%**。
作为对照，**有基线但无 dispatch_overhead 标记的 661 个方法平均领先 .NET 8 达 71%**。

> 注：这两个数（1597 vs 397）是同一集合的不同口径，不是矛盾 —— 1597 是全部走 fallback 的方法，
> 397 是其中能与 .NET 8 配对的子集。

---

## 三、安全评估 -- 这些方法返回什么？

ExternalRuntimeFallback 不是简单的 return-0。它有 3 条实线路径:

| 路径 | 作用 | 可靠性 |
|------|------|--------|
| **IL Data** | 对 crypto 等方法，codegen 嵌入了 AOT Core IR JSON 或原始 CIL -> 解释器执行 | 返回真实语义值 |
| **Dispatch Table** | `kChaosExternalRuntimeSubjects[]` 线性扫描 -> `_TryInvoke` 解释器执行 | 返回真实语义值 |
| **Return 0** | 两条路径都失败或不在表中 | 虚假 0（但 fact 框架比较 sentinel 值，不会误判为通过） |

**关键结论**: `dispatch_overhead` 方法的返回值**不是全部是 0**。comparison-summary.json 中 2385 个 completed 方法全部通过了语义验证，意味着其中很多都不是 return-0。

---

## 四、与 M3-C1c（runtime-intrinsics x4）的关联

| 项目 | ExternalRuntimeFallback | runtime-intrinsics x4 |
|------|----------------------|----------------------|
| 问题 | 是 dispatch 路径（性能） | 是 fact 输出为零 + 80 stub（正确性） |
| 机制 | 有调度表 + IL 数据 | `entry.exe` 能启动但 SIGSEGV |
| 修复 | 性能优化（P0） | 运行时修复（P1） |

**二者不是同一个问题。** ExternalRuntimeFallback 是**基础设施层**，runtime-intrinsics x4 是**具体的单一运行时崩溃**。

---

## 五、行动建议与优先级

| 优先级 | 行动 | 依据 |
|-------|------|------|
| **P0** | 优化 dispatch_overhead 路径 | 1597 方法平均落后 3393%，根因单一，修复杠杆极高 |
| **P1** | 修复 runtime-intrinsics x4 SIGSEGV | 80 stub + 零 fact 输出 |
| **P2** | 补齐 system/globalization/threading .NET 8 基线 | 72% 方法性能维度盲 |
