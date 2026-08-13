# Step 2.1 调研结论：P/Invoke GC 切换 — 已被 codegen 处理，非 broad gap

> **日期**：2026-08-13 ｜ **来源**：Explore agent 精确定位解释器→unmanaged GC 切换链
> **结论**：原"解释器 P/Invoke 不 preemptive"的假设大部分不成立——AOT 编译的 `[DllImport]` stub 已正确自转。

## 关键事实

1. **AOT P/Invoke stub 已自转**（`MethodEmission.PInvoke.cs`）：`needsGcTransition = !method.IsSuppressGCTransition`（:121），blittable 路径 `:301-304` TO_PREEMPTIVE / `:313-316` TO_COOPERATIVE，带 marshal 路径 `:324-339` 同理。
   → **默认 `[DllImport]` 调用 unmanaged 时已是 preemptive，返回转回 cooperative。**

2. **native 解释器 wrapper 链无一处可加注入**：`abi_reflection.cpp:145 MethodInvoke`、`interpreter_dispatch.cpp:151`、`precache_dispatch_raw.cpp:353`、`fast_dispatch_call.inc`、`interpreter_vm.cpp` 全部无 GC transition，且这些层**无法区分 P/Invoke 与 managed call**——若强加会污染 managed 路径。这是它们当前不转的正确原因。

3. **解释器 entry 已断言 cooperative**（`entry_direct.cpp:652-653`）。

## 残留缺口（窄，非 broad）
- **(a)** `[SuppressGCTransition]` 方法：AOT stub 明确不注入（按设计）→ 调用须自身短，若长跑仍有风险。
- **(b)** 不走 AOT pinvoke stub 的 fallback/解释路径末段。

## 对主线的意义
- **Step 2.1 非 broad gap** → 不该盲目加 transition（会污染 managed 路径）。
- 最有价值的正确动作 = 验证 (a)/(b) 边角（低优先），非主线。
- **建议主线转 Step 3.1（null/越界抛异常）**：真 gap、作用域清晰、前后差异明确（silent → throw）、可直接对 fact/unit 基线 test。
