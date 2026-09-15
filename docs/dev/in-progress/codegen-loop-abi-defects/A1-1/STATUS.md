# A1-1 — 标记指针经 INT64 返回槽漏出

```yaml
task_id: A1-1
task_type: plan
phase: implementation
parent_task_id: codegen-loop-abi-defects
source_task_id: codegen-loop-abi-defects
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-executing-plans
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
lifecycle_status: in-progress
```

## Scope

定位 6 项 passing 记录的 `value` 是标记指针（`0x80007ff7...`）的根因层：
ATG 返回类型推断 vs codegen 返回槽处理。

## Inputs

- 归因来源：`FAILURE-TRIAGE-2026-09-14.md`
- 现象：si=27/29/33/159/170/174

## Exit Criteria

- 定位到层（ATG 或 codegen）—— **已达成**，见下
- 修复后 6 项 `value` 正常

---

## ✅ 定位结论：**codegen 层**（IR→C++ 的槽位映射）

以 `si=159 ModuleTests::IsResource_1__0` 为取证样本（该样本最干净：
C# 返回 **bool**，`return result_1_0 ? 1L : 0L` —— 一个 bool 逻辑上
不可能产出托管指针，故缺陷必在下游）。

### IR 是**正确**的（`aot-core-ir.json`）

```
16 callvirt  Module::IsResource:System.Boolean()
21 stloc.0            ← result_1_0
22 ldloc.0
23 brtrue 29
25 ldc.i4.0 ; 26 conv.i8 ; 27 br 31      ← 假臂
29 ldc.i4.1 ; 30 conv.i8                 ← 真臂
31 stloc.1            ← ?: 结果 → local 1
32 br 34
34 ldloc.1            ← 取 local 1
35 ret
```

IR 明确：`ret` 取 **local 1**（?: 的结果）。**IL→IR 无缺陷。**

### emit 是**错的**（`native-aot.page-0006.cpp`，函数体 43-45 行）

```cpp
chaos_locals[1] = ChaosStoreInt64(_s1);   // ← 写 _s1
_i1 = ChaosLoadInt64(chaos_locals[1]);
return _i1;
```

三处错误：

| # | 表现 |
|---|---|
| 1 | **源错**：`stloc.1` 写的是 `_s1`（第 26 行 `GetModule()` 的接收者句柄），而非 `?:` 的结果 |
| 2 | **结果被丢弃**：`_i2` 在真/假两臂都被 `ChaosStoreInt64(...)` 赋值，**之后从未被读取** |
| 3 | **栈/local 混淆**：假臂 `ChaosLoadInt64(_s2)` 读的是 **slot `_s2`**（刚被置 0），而 IR 意图是读栈上的值 |

`_s1` 是托管 `Module` 句柄 → 正是观测到的 `0x80007ff7...` 标记指针。

### 6 项**形态完全一致**

随机抽样 3 项（`GetLoadedModules_24__0` / `GetMethods_9__0` / `GetFields_13__0`）
的 emit 尾部**逐字相同**：

```cpp
_i2 = ChaosStoreInt64(...);                 // ?: 结果算出
chaos_locals[1] = ChaosStoreInt64(_s1);     // ← 却存 _s1
_i1 = ChaosLoadInt64(chaos_locals[1]);
return _i1;
```

⇒ 单一根因，非逐项差异。

### 归因层裁决

**codegen**。依据：
1. IR 正确（`ret` 取 local 1），缺陷在 IR→C++
2. `stloc` / `ldloc` 的 instruction handler **本身正确**
   （`EmitInstruction.cs:468-490` 的 `case "stloc"` 正确使用 `ConsumeEvalStackValueExpression()`）
3. ⇒ 缺陷在**结构化槽位上下文的 push/pop 与 local↔slot 映射**
   （`ActiveStructuredSlotContext.PopValue()` / `EmitEvalStackPush`），
   或 `brtrue/br` 合并点的栈深度记账

### 次要发现（可能同源）

`Module::IsResource` 等 6 个 API 走 **catch-all 兜底**
（`chaos_external_runtime_..._Module__IsResource_System_Boolean__()`，**零参数**——
`this` 未传）。这与已修的 `Type::get_Module` 是同一类「漏注册」问题。
但它**不是**本缺陷的根因：即便兜底返回 0，正确的 emit 也会返回 `0L` 而非标记指针。

⇒ 可作为 A2 的并行清扫项登记（不阻塞 A1-2）。

## 下一步（A1-2）

修 `stloc.1` 写入错误源的问题。切入点候选（择一，需先测量）：
- `ActiveStructuredSlotContext` 的 local→slot 映射表
- `EmitEvalStackPush` 在 `brtrue/br` 合并点后的栈深度
- 结构化体的 slot 分配器（`AllocateEvalStackTargetExpression`）

**测量方法**：加诊断打印 IR 的 `stloc.N` 与 emit 选中的 slot 名，对照定位错配点。
（诊断须**直接跑 TPG**，见 roadmap 陷阱 1。）
