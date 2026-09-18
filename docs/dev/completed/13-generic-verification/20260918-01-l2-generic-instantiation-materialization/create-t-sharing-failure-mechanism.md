# Create<T> 共享体失效机制 — 实测补充（T7 调查，2026-09-18）

> 来源：threading-tasks chunk 87 项 factoryGap 根因（workflow wf_9a1b7d61-600 F5 族 + 本会话代码追踪）。
> 本文是 `design-l2-ondemand-v1-01.md` 的**实测证据补充**，证明 L2 之前的**当前共享机制**对
> 类型身份敏感方法不健全 — 与 L2 要解决的问题同根。

## 失效链条（全部 file:line 实证）

1. `BuildGenericSharingCanonicalMap`（Emission/NativeAotLoweringPlanner.GenericSharing.cs:51）
   按 OpenDefinitionSubjectId 分组，11 个 `SubjectInstanceFactory::Create<T>` 实例化
   全部是引用类型实参 → 全部可共享；`PickCanonicalInstantiation` 无 Object 实参兜底时
   **按 SubjectId 字典序取第一 = `Create<System.Action>`**。
2. 其余 10 个实例化的 stub（Emission/NativeAotLoweringPlanner.MethodEmission.cs:82
   `EmitGenericInstantiationStub` → GenericSharing.cs:253 `ResolveStubTargetNativeSymbol`）
   全部转发到 Action 的 canonical 体。
3. canonical 体在 lowering 时**把 T=Action 烧死**：硬编码
   `chaos_mt_Chaos_TestFramework_Sdk_System_Action.AsTypeInfoHot()` +
   `ChaosRuntimeHelpersGetUninitializedObject` → 任何 T 的 receiver 都是 Action 实例。
4. `chaos_generic_context` 参数虽然在 ABI 上存在并被转发，但 canonical 体**不消费它**做
   类型解析 — 这正是 typeof-fold ↔ 对象模型错配（memory:
   typeof-fold-vs-object-model-mismatch）在泛型共享面上的表现。

## 实测影响

- threading-tasks chunk：87 项 factoryGap（receiver=Action 或 null），跨 6 族。
- system chunk 的 Enum.Parse/Array.CreateInstance 等亦依赖托管 Type 对象，同源。

## 与本线 L2 设计的关系

- L2（按需加载）解决"泛型定义跨程序集物化"；本文证明**即使物化正确**，现有
  canonical 共享仍会把身份敏感方法烧死到字典序第一的实例化上。
- **两个修法不互斥**：(a) 短期 — `OpenDefinitionUsesGenericTypeParameter`（开放定义
  指令流含 `!!` 泛型参数拼写）时该组禁用共享，回退每实例化真体（共享只是优化，
  禁用零语义风险）；(b) 长期 — canonical 体真正消费 chaos_generic_context 做类型
  解析（= typeof-fold coerce 方案的一部分）。
- (a) 已在本 worktree 之外定位完毕未实施，等待与 L2 线协调归属。

## 附：demand graph 数据（实证 11 个实例化 demand 全部正确）

`generic-instantiation-demand-graph.json`：140 demands，其中 11 个 Create 实例化，
subjectId 均正确拼写 `Create<T>:T()`（per-instantiation，无折叠）— 缺陷不在 demand
收集，纯在 canonical 共享映射。
