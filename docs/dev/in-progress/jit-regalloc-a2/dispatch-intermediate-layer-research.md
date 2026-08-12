# 消除中间层 dispatch — 符号前置调研（Task 6）

> **日期**：2026-08-12
> **类型**：调研文档（用户选择：不落未验证代码，交付前置分析）
> **目的**：定跨 assembly 调用的 2.5-30× 落后真因，给出「消除中间层」的正确修法方向。
> **关联**：`cross-platform-unify-plan.md` §9.3、roadmap `jit-regalloc-a2-continuation-roadmap.md` §3.1、memory `[[perf-line-benchmark-credibility-and-dispatch]]`

---

## 1. 一句话结论（真因已确认）

**2.5-30× 的根源不是「间接调用」的 1-3 cyc 开销，而是跨 assembly / 无 shape 的 callee 在 `kChaosExternalRuntimeFnTable` 路由后，落到了解释器执行（`InterpreterEntryDirect`），根本没有 native AOT 码。** `kChaosExternalRuntimeFnTable` 是症状，病灶是**这些 callee 没有原生 codegen**。

---

## 2. 代码链路（已逐层核实）

### 2.1 codegen 侧：何时 callee 进外部表（`InvocationPlanning.cs:211-276`）
`CollectExternalRuntimeDispatchEntries` 决定每个 callee 的路由方式：

```csharp
bool _isAot = _methodsBySubjectId.TryGetValue(callee) && _e.Instructions.Count > 0;
if (_isAot)                                      → 直接 AOT 调用（不进表）        // :242
if (TryCreateExternalRuntimeHelperDefinition())  → ShapeRegistry 处理（进表，有 C++ shape）// :252
if (同 assembly 实例化 stub)                     → 直接调用                    // :264
else                                             → 路由进 kChaosExternalRuntimeFnTable // :270
```

**进表的三种 callee**：① 不在编译闭包内（跨 assembly）② 不在 ShapeRegistry 且无 helper 定义 ③ 跨 assembly 泛型方法（无 stub 定义）。

### 2.2 codegen 侧：call site 发射（`ExceptionEmission.Linear.cs:717/721` + `Utilities.cs:342/346`）
进表的 callee，call site 生成：

```cpp
reinterpret_cast<Ret(*)(Args)>(kChaosExternalRuntimeFnTable[idx])(params);  // 间接表调用
```

代码注释（`Linear.cs:709`）明说："callee **无 shape-matching ExternalRuntimeHelper 定义，函数符号链接期不存在**，故经表调用而非直接符号。"

### 2.3 运行时侧：表条目解析 + 执行（确认解释器 fallback）
- `InvocationPlanning.cs:333-336` 注释：**"bridge thunks use the external runtime helper symbol as the target (resolved at startup by the interpreter's table). Future enhancement: resolve the actual native symbol at compile time for InternalCall methods."**
- `entry_direct.cpp:1224` `InterpreterEntryDirectFast(method_key)` → `InterpreterEntryDirect(...)` → **解释器执行 IL**。

**∴ 结论**：进表的 callee 在运行时执行的是**解释器**（乃至 RegisterExecute/transpile），不是 native 码。这才是 2.5-30×。

---

## 3. 为什么「改成直接调用」是错误方向（关键判断）

| 假设修法 | 为什么不对 |
|---|---|
| 「间接调用慢，改成 `TargetSymbol(params)` 直接调用」| 治标：间接调用只 ~1-3cyc，改成直接调用最多省几 cyc，与 2.5-30× 差两个量级。**且这些 callee 根本没有可链接的 native 符号**（`TargetSymbol` 链接期不存在），硬改会链接失败。 |
| 「压榨 table 查找 / 紧凑化索引」| 同样治标，表查找是 O(1) 启动期已解析，非每次调用开销。 |

**正确方向 = 让这些 callee 拥有 native AOT 码**，而非省间接调用的皮。

---

## 4. 正确修法方向（三个可叠加，按收益/成本排序）

### 方向 A（首选，代码注释已标注为 future enhancement）：编译期解析 native 符号
`InvocationPlanning.cs:334-336` 已明写意图。对 **InternalCall**（native runtime 方法，managed 签名 + GC 协作模式）：
- 现状：表条目目标 = InterpreterEntryDirect（解释器）。
- 改法：在**编译期**把 InternalCall 解析成真实 native 符号（与 `ChaosMarshalGetHRForLastWin32Error` 等既有 internal 解析对齐），call site 直接调用，不走表/解释器。
- **收益最大**，且作用域清晰（codegen 单域）。
- 难点：需给 InternalCall 提供真实 C++ 函数体（`ConvertToCppHandler` / 既有 internal stubs）。

### 方向 B：扩编译闭包，让跨 assembly callee 进 `_methodsBySubjectId`
让「跨 assembly 但 shape 可推理」的 callee 进入 AOT 编译闭包（`_isAot` 分支），生成真 native 体，call site 直接 `TargetSymbol`。
- 收益大，但可能扩大编译体积（闭包膨胀）。
- 需要跨 assembly 的 method artifact 关联（是 codegen 主链改动）。

### 方向 C：扩 ShapeRegistry，提供更多 native `ExternalRuntimeHelper` 定义
增大 `TryCreateExternalRuntimeHelperDefinition` 覆盖的 shape 集合（GenericShapeDescriptor / SimpleForward），让更多 cross-assembly 方法有 C++ shape，call site 进 `:252` 分支（有 shape）而非 `:270`（落解释器）。
- 收益中等，作用域集中在 ShapeRegistry（单域，小而可控）。
- 与 §4.1 的既有 5/75 覆盖塌方（CoreLib 只 lowering 子集）同源 —— shape 未覆盖。

---

## 5. 建议（供 next session 采用，非本次实施）

1. **先量**：chaos-aot vs net8 的 benchmark_report `bottleneck` 分类会给出 `dispatch_overhead`（时间比 >2）并列出具体 methodSubjectId。用它对每种进表 callee（跨 assembly / generic / InternalCall）**分类统计**，确定 A/B/C 哪个方向覆盖最多代价方法。
2. **优先方向 A**（InternalCall 编译期符号解析）：它是代码注释预定的未来增强，作用于最热的跨 assembly runtime 调用，且单域、可增量落地。
3. **验证**：方向 A 落地后，`test_jit_native` + 相关 AOT 测试全绿 + benchmark_report 中原本 `dispatch_overhead` 的 methodSubjectId 变为直接调用（`chaosAotVsNet8Pct` 显著改善）。
4. 依赖：需 foundation-dll chunk pipeline 可跑（产生 benchmark profile 真因数据）。fd-verification 线推进后回到此任务。

---

## 6. 本调研范围外

- 不落代码（用户选择文档交付）。
- 未改 `_methodsBySubjectId` / ShapeRegistry / call-site 发射逻辑。
- 未跑真实 profile（环境依赖 chunk pipeline）。
