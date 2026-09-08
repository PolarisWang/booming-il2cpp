# STATUS — System.Threading.Tasks 生产级工程

## 元信息

```yaml
task_id: async-task-industrialization
task_type: roadmap
phase: roadmap
roadmap_or_plan: docs/dev/in-progress/async-task-industrialization/roadmap-v1-01.md
created: 2026-09-07
created_by: user
source: 用户要求 — System.Task 到可生产级质量；brainstorm 确认手工状态机方案，排除 C++20 coroutine
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
```

## Scope

将 C# `System.Threading.Tasks`（Task、async/await、ThreadPool、组合子、Parallel）在 chaos-il2cpp AOT 翻译管线中达到可生产级质量。
核心方案：**IL 状态机 → native C++ 手工状态机**（替换当前 AsyncCoroutineEmitter 的占位 C++20 coroutine）。

## Inputs

- 深度调研：CLR async 无 coroutine，JIT 自建状态机变换（`jit/async.cpp`）
- 现有 native 地基：`src/native/common/chaos/async.h`、`task_runner.cpp`、`thread_pool.cpp`、`monitor.cpp`
- 现有 codegen 占位：`AsyncCoroutineEmitter.cs`（占位 coroutine）、`AsyncAwaiterCatalog.cs`
- Roslyn 生成的状态机 IL：`>d__` struct + `MoveNext()`（switch on state）
- CoreCLR AsyncTaskMethodBuilder 契约：Start/AwaitUnsafeOnCompleted/SetResult/SetException

## 设计摘要（权威输入）

### 边界拍板
- 采用手工状态机翻译，不做 C++20 coroutine reverse-engineering— **决策来源**：brainstorm 深挖 C++20 coroutine 语义，判定其不能满足 P1(perf)+P2(arch)+P3(hotupdate)，详见 `docs/dev/archive/discuss/`（brainstorm 记录）
- 目标：async Task 全家桶（Task/Task<T>/ValueTask 等）+ **performance < .NET8 2×（roadmap 计划目标，非已测量的性能数据；具体 baseline 待 Phase 5 时才产出）** + hotupdate 支持
- 非目标：await foreach、await using、Pipelines、Channel、自定义 TaskScheduler（后置）

### 架构结论
- native 侧 ThreadPool（thread_pool.cpp）是孤儿，需 `ThreadPoolInitialize` 接到 RuntimeInit — **证据**：`async.h`/`thread_pool.cpp` 中的 ThreadPool 类型已独立存在，但既不被 RuntimeInit 调用也不被任何 GC 路径引用，属于"有代码无接入"的孤儿状态；详见 `src/native/runtime-core/thread_pool.cpp` + `src/native/runtime-core/async.h`
- AsyncStateMachineBox native 等价物需用 GC 堆对象（非 pinned）— **证据**：CoreCLR 的 `AsyncStateMachineBox<T>` 是 `Task<T>` 子类、分配在 GC 堆上；pinned 违背 GC 设计（产生碎片、阻止晋升）；详见 `src/coreclr/System.Private.CoreLib/src/System/Runtime/CompilerServices/AsyncTaskMethodBuilder.cs`（CoreCLR 源码）
- 状态机是按 IL `MoveNext` 直译（switch + goto），不多做一次变换 — **证据**：Roslyn 生成的 `>d__` struct 已含完整 switch(state)/goto 状态机，直接翻译即可复用；见 Inputs 段的 Roslyn 状态机 IL

### 阶段切分
Phase 1（运行时基础设施闭环）→ Phase 2（状态机翻译引擎）→ Phase 3（组合子语义）→ Phase 4（Parallel）→ Phase 5（性能优化）→ Phase 6（验证+hotupdate）

## 下一步

- 从 **Phase 1 的 ASYNC-P1-1（ThreadPool 生命周期接入 RuntimeInit）** 开始
- 串行推进。Phase 2 完成后评估是否切 hybrid

## 最近摘要

- 2026-09-07：完成深度调研。确认 CLR async 无 coroutine、JIT 自建状态机。并行了 agent 深挖 C++20 coroutine 语义，判定其不能满足 P1(perf)+P2(arch)+P3(hotupdate)，采用手工状态机方案。

## latest_stop_point

创建 roadmap-v1-01.md + STATUS.md。下一步执行 ASYNC-P1-1。

## 关键文档

- roadmap-v1-01.md

## 调度状态

```yaml
# dispatch_model 权威声明在顶部元信息 yaml（本文件 L19）；此处不重复维护以避免漂移。
active_batches: []
completed_batches: []
```
## 执行进度

### Phase 1 收尾（实际进度）

* ASYNC-P1-1 ✅ a5f6ec141: ThreadPool→RuntimeInit
* ASYNC-P1-2 ✅ ef440e3de: AsyncTask 续列(box resumption)契约 + 5测试
* ASYNC-P1-3 ✅ a46d4be49: 续列经 ThreadPool 派发 + 桥接注册 + 2测试
* ASYNC-P1-4: EC infra **已存在且已消费**(execution_context.cpp 318行 + task_runner/thread_pool 已用), execution_context_smoke 11/11 绿 → 视为 pre-existing 满足, 不再重复建。跨 await 自动 EC 流属 P3-4。
* ASYNC-P1-5: async_integration_smoke + execution_context_smoke 已覆盖 → 满足

**Phase 1 net**: ThreadPool 活、Task 能续列派发、EC 贯穿。Phase 2 是翻译引擎(接 AsyncCoroutineEmitter 占位 → 真状态机翻译)。

## Phase 2 on-ramp（ASYNC-P2-1, 下一会话入口）
Async codegen 现状(MethodEmission.cs:206-263)：
- Complex async → interpreter-dispatch stub(不执行状态机)
- 非 Complex async → GenPromise+GenCoro 占位(co_await std::suspend_always{} 不做任何事) →
  NativeSymbol wrapper 返回 handle
⇒ 两态都不真正执行 MoveNext 的 IL。

P2-1 正确入口(最小可验证翻译器)：
1. 在 translator 用一个手工迷你 async Task subject(如 async Task<int> One(){await Task.Yield();return 1;})，
   锁机其 >d__ IL：switch(state)+builder+awaiter。
2. 新增实现："MoveNext IL → C++ 状态机 struct+成员函数" (非 C++20 coroutine)，绕过现 GenCoro 占位。
3. 复用 Phase 1 的 native 续列契约(async_task_on_completed/builder) 接线 MoveNext 重入。
4. 跑通端到端(entry AOT 态)后，再扩通用正常方法 lowering(translateNormalBody 复用+state 处理)。

警告：AsyncCoroutineEmitter/MethodEmission 是 Multi-session 增量built，
替换须在清醒完整会话谨慎做，避免在状态机 IL → C++ 翻译误埋 latent 错。
Phase 1 infra (ThreadPool/续列/EC) 已真落地可用，是 P2-1 的稳固地基。

## 执行进度（2026-09-07 会话 2 续）

### ASYNC-P2-1 前导：真实 async subject + 全管线提取 ✅ committed `f04d4af34`
用户拍板 scope=段 A+B 一次做、基座=**真实 async subject + 完整翻译管线**(非手搓 synthetic)。
达成的第一可交付：**现有基础内并无任何真实 async C# subject**，故先建真实 subject 源 + 用真管线提取。

* `tests/unit/managed/codegen/AsyncTestAssembly/{AsyncMethods.cs,AsyncTestAssembly.csproj}`:
  含 `async Task<int> GetOne()`(await Task.Yield+return 1)与 `async Task DoVoid()`，
  编译产真 `<GetOne>d__0::MoveNext` / `<DoVoid>d__0::MoveNext` value-type 状态机。
* `tests/unit/managed/codegen/AsyncPipelineTests.cs`: 用 `new PipelinePlan().Execute()`(
  FullAssemblyClosure) 驱动 **Loader→SemanticWorld→Linker→MetadataWriter→CodeGenStage**
  全管线对该 dll 提取 → **PASSED**。断言:subject surface 出 `AsyncMethods::GetOne` +
  `>d__::MoveNext`,且 MoveNext body 的 callee 含 `AsyncTaskMethodBuilder`(SetResult /
  AwaitUnsafeOnCompleted / get_Task)。
* `Chaos.IL2CPP.CodeGen.Tests.csproj` 加 `AsyncTestAssembly.csproj` ProjectReference
  (ReferenceOutputAssembly=false 仿 StubAssembly)。异质 dll `bin/Debug/net8.0/AsyncTestAssembly.dll`。
* Driver.cs(snapshot FixtureAssembly)那处 async 尝试**已 revert** —— snapshot 88-fixture 是
  手写 aot-core-ir.json,不经真提取;AsyncTestAssembly 才是真提取主体,避免污染共享 snapshot。

→ **结论(根因定版)**: 真 async 经全管线确实能让 `>d__::MoveNext` 进 AOT core IR 且带
builder/awaiter callee;管线能承载、不跳过 compiler-generated 类型。TDD 基座可用。

### Step 2 设计要点(权威,续做前必读)
真实 `MoveNext` = 值类型 `>d__` 上的实例方法,IL 含:
- `ldfld/stfld <>1__state (int)`
- `ldflda/ldfld <>t__builder` — `AsyncTaskMethodBuilder<int>` value 字段(嵌套 struct)
- `call builder.AwaitUnsafeOnCompleted<YieldAwaiter,d__>(ref awaiter, ref this)`
- `call builder.SetResult` / `SetException`(SetException 在 catch 区)
- **EH : `try{...} catch { builder.SetException}`** + state machine switch(brfalse/branch)

发射器未变(MethodEmission.cs:206-254)前,该 MoveNext 走的是:
- `IsAsyncStateMachineMoveNext` → `ClassifyAsyncMethod`
- 非 Complex → `AsyncCoroutineMethodCount++; GenPromise+GenCoro(c++20 co_return 假 coroutine)`+NativeSymbol wrapper。
  GenCoro **丢弃 abody**,不执行真 IL → 状态机不真跑。

改造落点(MethodEmission.cs:206-263 async 分支):把非-Complex 从 `GenPromise/GenCoro` 改为
**值类型 struct-this 结构化发射** —— 复用现有 lower path:
- struct: 由 ObjectModelEmission 发 `struct chaos_valuetype_...>d__N { field_<>1__state int / field_<>t__builder b / ... }`
  (需 `>d__` 进 `valueTypeSubjectIds`+`_fieldsByDeclaringType`;ObjectModel 的 field 扫描在
  reachableMethods 上独立于 method emission,见 ObjectModelEmission.cs:280-304 已含 Field
  TargetReference → ValueType 判定 → 加 declaring type)
- MoveNext body: 走 Emit 普通 value-this(ldarg.0 / stfld value / EH 异常体 → native builder.SetException)。
- 段 B: 在其 body 里把 `call builder.SetResult/GetTask/AwaitUnsafeOnCompleted` resolve 到
  async.h native extern(SetResult/GetResult/is_completed 已有;Start/AwaitUnsafeOnCompleted 待加)。

风险/顺序护栏:优先无 EH、无真挂起(单 await 恒完成)最简样例先绿,再叠 EH SetException 与
resume 语义;理由与 roadmap `watch_items`(异常传播) 一致。真 MoveNext 的 EH 区域让"一步到位
value-this 发射"最难,故先走最简。
Codegen emission 单测/文字断言的最高保真基座 = AsyncPipelineTests(全管线提取)+ 对该
`>d__::MoveNext` artifact 的 emission 文字断言;`SNAPSHOT_UPDATE` 风格全发射可后续再铺。

### PART 1/2 DONE（Step 2+3）✅ committed `60b1c8b06`
把 async MoveNext 从 C++20 coroutine 占位切到真 state-machine 结构化发射。

* MethodEmission.cs async 分支:移除 GenPromise/GenCoro(+ NativeSymbol forward wrapper
  只管返回 int64 handle)发射。非-Complex async MoveNext 现在直接**fall-through 到现有普通
  结构化发射**:EmitAbiArgumentInitialization + EmitViaStructuredIR(含 EH 5-shape /
  线性 emit), 不特殊内联。
* emit 决策复用(不重写): `EmitManagedMethod` 共享主路径;body 内的 stfld/ldfld value-this
  走 `chaos_resolve_managed_value_pointer`,`call AsyncTaskMethodBuilder::SetResult/
  AwaitUnsafeOnCompleted/* async_yield_get_*` 等已被**既有 LinearEmitCall/ExternalRuntime
  helper resolution** 解析为 C++ extern。当前 catalog 已能解析这些符号为 extern 声明（C++ 编译
  通过），但 symbol 尚未在运行时注册表中登记（运行时链接阶段可能解析失败 —— 此属 R1 范围）。无需
  新 segment-B resolver。`chaos_external_runtime_...` + async_yield_* 路径已 cover。
* AsyncPipelineTests 两 emission 断言(用 real 全管线 artifacts + NativeAotLoweringPlanner
  Create 生 MoveNext 函数源)。

真产出(<DoVoid>d__1::MoveNext)经实际查看:
```
extern "C" void ...MoveNext(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) {  // async_yield_*,
  ... state switch, ldfld/stfld field_...1__state / t__builder / u__1,
  ... async_yield_create/get_awaiter/get_is_completed/get_result,
  ... chaos_external_runtime_...AsyncTaskMethodBuilder...AwaitUnsafeOnCompleted(...),
  ... SetException(在 CHAOS_EH_CATCH_BEGIN), SetResult,
  ... }  // 无 co_await / suspend_always / AsyncPromise_ / _Coro();
```
不再空壳。segment-B call 已真解析到 native(dataset:await 挂起/续列重入在 MoveNext 源码里
经 get_is_completed==0 分支写 state 并交给 ThreadPool 续列——但 **codegen 未发显式
scheduler/continuation 注册**:当前产出只是把 await 当"get_is_completed 为真才继续,否则返回"
的表达;真正 resume 注册(spike 的 awaiter.UnsafeOnCompleted→async_task_on_completed/
ThreadPool re-MoveNext)是 Step 5 需要接的 native 半边,见下)。

> **两层语义澄清**（文本层已解析 ≠ 运行时语义完整）:上述"解析到 native / 无显式注册"并不矛盾
> —— extern 调用仅保证 C++ 编译通过（call 语句能落成合法 extern 声明并 emitted），真正
> 挂起→续列重入的注册语义（MoveNext 何时/如何被 ThreadPool 重新拉起）依赖 R1 的
> continuation_cb 注册,未在本 session 落地。

回归门:codegen 单元 1986/1986 green(含 98 stub emission/shape 测试);snapshot
88-fixture **pre-existing 已挂 85**(与本次改动无关 —— clean baseline 也挂,待 repo-cleanliness
track)。Driver.cs async 已 revert;AsyncTestAssembly 是 async subject 唯一 home。

**关键门复现命令**：
- codegen 单元(回归门): `dotnet test tests/unit/managed/codegen/Chaos.IL2CPP.CodeGen.Tests.csproj`
- snapshot(88-fixture 预挂的已知门): `dotnet test tests/unit/managed/snapshot/Chaos.IL2CPP.CodeGen.SnapshotTests.csproj` (经统一入口 = `python tests/runner/test_driver.py --layer unit --group snapshot`)
- async_integration_smoke(native CTest): `tests/unit/runtime-native/runtime-core/threading/async_integration_smoke_test.cpp`

### REMAIN（Step 4-5 及残余,续做入口）
Step 3 输出已把 `MoveNext` 体真发射 + builder/awaiter call 解析到 native,已达"段 A+B 的
**codegen 文本层**判据。但完整异步运行语义(真正跨 await 挂起→续列重入)需下述 native 半边,
未在本次 session(已很长 + 仓库多 agent 争用 index.lock)做:

1. **R1【最重要】codegen 尚未发 resume 续列注册**。真实 move-next 的 await 分支目前只:
   `if(!get_is_completed) { 存 awaiter; /* 没有 UnsafeOnCompleted 续列注册 */ return; }`
   —— 需要类似 spike 的
   `async_task_on_completed(task_handle, continuation_cb=MoveNext, ctx=this)`(awaiter 里取出
   Task handle)+ 把 MoveNext 地址当 continuation_cb。此即"挂起后 ThreadPool 重入 MoveNext"。
   落地:在 `call AwaitUnsafeOnCompleted`/`async_*` 的 emission 映射处,或给 await 分支补
   continuation_cb(= MoveNext native symbol FN) + ctx(=this) 注册调用;并确认 `d__` box(经
   builder.get_Task → 实际 `AsyncTask` handle)不 GC。参考 Phase1 async.h:`async_task_on_completed`
   / `finish_async_task` 已实现。
2. **确定 emitted C++ 可编译**。AsyncTestAssembly 的异步方法现在 emit 的 path 需要真正
   编译链接(C++ macro/field struct/`chaos_type_...d__` 声明存在与否)。当前 codegen 单测
   只断言文本非空壳,未做编译链接。需要能接 foundation-dll 或 native smoke 里:
   thread_pool initialized + ThreadPool init + `async_task_run` 注册;test 驱动 AsyncHelper 的
   entry(冒烟 pattern)验证端到端(One→yield→ThreadPool→set_result(1)→completed=1)。
3. **async entry(段 C)** 由真实 `async` 方法(非 MoveNext)建 box+`builder.Start`+返回 Task:
   codegen 对 `AsyncMethods::GetOne/DoVoid` 本体(返回 Task/空 GetAwaiter stub)还没真做
   MoveNext box 起动——R1/R2 打通后自然能验。

recommended_next（下一 clean session）: 从 **R2(把 real MoveNext emitted C++ 接进
async_integration_smoke 同型链路编译/跑通)+ R1(补 continuation_cb 注册)** 二择一起桥,先
R1 最小(只补 resume 注册 + MoveNext continuation) 让它能在已 verify 的 Phase1 native smoke
spike(手写 AsyncStateMachine_One) 等价路径里 codegen 真跑。

已澄清作业要求:不 C++20 coroutine(hotupdate P3 不可丢);performance AOT<2x .NET8;
Priority P1>P2>P3;Go production-grade 不催。

### 2026-09-07 会话末再校准（真实边界：段 A 结构发射 done；B+C+R1+R2 REMAIN）
补一次诚实校准：60b1c8b06 完成的只是**段 A 结构发射**（移除 coroutine 占位 → MoveNext 经现有
普通结构化发射产出真 C++ 函数体），且已验证 **dormant-safe**：当前无任何 codegen/真实 subject
会调用 MoveNext（async entry 段 C 未建 → 没人 box+Start → MoveNext 不可达），故不会有 silent
运行期错。

确凿事实（用 real <DoVoid>d__1::MoveNext 生成源当证据，~180 行）：
- MoveNext 产出真 extern "C" 函数：state 初始化、switch(state)、`<>1__state/<>u__1/<>t__builder`
  字段读写走 `reinterpret_cast<chaos_type_<...>d__1*>`（reference-this —— Roslyn 把 >d__ box 进
  AsyncStateMachineBox<T> 堆对象首成员存 >d__，故 `reinterpret_cast<chaos_type_...>` 取 box 内
  >d__ 是**语义正确**的），EH CHAOS_EH_TRY/CATCH (SetException)，末尾 SetResult。
- call `builder.SetResult/SetException/* async_yield_*` 已被既有 callee→native resolve 成
  `async_*` extern 或 `chaos_external_runtime_System_PublicCoreLib_...AsyncTaskMethodBuilder*
  SetResult/SetException/AwaitUnsafeOnCompleted...()`。

但**真 async/await 语义 REMAIN**（下一步必须是它，不是已在"完成"里）：
- `call ...AwaitUnsafeOnCompleted...()` 现在落成 extern-runtime-stub 调用（对它来说要真做
  resume 注册 = box+Start+AsyncTaskMethodBuilder 真 class + async_task_on_completed 注册
  MoveNext 重入）。这需要 **native runtime 提供真 AsyncTaskMethodBuilder.Start / box** +
  **段 C**（async entry 建 >d__ box 驱动 MoveNext）。codegen 不能独自闭链。
- 顺序：段 C（async entry : Create box + builder.Start(box) → move-next 起跑）→ 真 builder
  Start → R2(实编译/原生跑通 One→yield→ThreadPool→set_result(1)→completed=1)。
结论：60b1c8b06 是正确但**仅段 A**。Rest status 表不要标 ASYNC-P2-* "完成"直到 Step5(R1+R2+段C)e2e
真跑通 codegen 产出的状态机。recommended_next fresh session = 由接入真 native AsyncTaskMethodBuilder
runtimeless builder(段 C 入口 + Start) 起步,复证 spike 往返。
线程并发注意:async.h / async_integration_smoke 有并行 agent(已 committed d5342d7e6 补 finish_async
顺带 + smoke 线程 identity 断言);commit 前核对暂存区。




### 2026-09-08 段 B/C（commit 35ab089c0）真实边界校准
用 live AOT-IR 提取(AsyncTestAssembly) probe 实证段 C 的 entry 发射**结构早已完整**（建 box+Create+Start+
builder.get_Task 都吐了）——真正的 gap 不是缺 entry emission，而是 **builder 6 操作都落 ChaosExternalRuntimeFallback**。
本 commit 闭合了那部分：给 async.h + ShapeRegistry 接真 native。

确凿（绿测试为证,codegen 2149/2149 + AsyncPipelineTests 5/5）:
- **Entry**(GetOne/DoVoid) `Create`→`chaos_async_task_builder_create()`、`get_Task`→`async_task_builder_get_task()`
  已 native;不再走 interpreter fallback。（新测试 CreateAndGetTaskRouteToNativeAsyncBuilder）
- **MoveNext body** `SetResult`→`async_task_builder_set_result_raw/_void`、`SetException`→`async_task_builder_set_exception`
  已 native。（新测试 SetResultAndSetExceptionRouteToNativeAsyncBuilder）
- **Start<SM>/AwaitUnsafeOnCompleted<A,SM>** ShapeRegistry resolver 能解析每-SM 的 MoveNext native symbol
  (probe log 实证 `mnSym=AsyncTestAssembly_AsyncMethods__GetOne_d__0_MoveNext`; 修复了非泛型 builder parse)。
  native async.h 已备 `async_task_builder_start(move_next,sm_box)` + 跨线程 continuation
  (`AsyncStateMachineMoveNextFn`/`AsyncStateMachineContinuationData`/`async_await_task_resume`/`async_await_yield_resume`)。

REMAIN（下一 session 入口,codegen emission 未做 end-to-end runtime 验证）:
1. **R2【关键】Start<SM> wrapper 的 emitted C++ 实编译**：resolver 生成的 wrapper(symbol=CppSource 带 {mnSym} 调用)
   以 `chaos_external_runtime_...Start...` 名发射，需接 async_integration_smoke/foundation-dll 实编译链接验证它真的
   触发 MoveNext。此前探针显示 entry 的 Start 处仍是 `chaos_external_runtime_*` 符号 —— 需确证该 wrapper body
   (含 mnSym 调用) 真被 emit 且能链接，而非 interpreter 0-return。
2. **AwaitUnsafeOnCompleted wrap 上亦同理**（GetOne 的 yield 路径是 TaskAwaiter 还是 YieldAwaiter 边界待实测确认）。
3. **跨线程线程/GC box 生命周期**：entry 用 stack `__chaos_stack_obj` 分配 d__，真跨 await 挂起跨线程 resume 需改
   GC-heap box(CH_AOS_IL2CPP_NEW_GC)+ continuation 持 box;否则 UAF。设计见 roadmap 决定(GC 堆非 pinned)。
4. **Task.Yield is_completed 策略**：async.h 现恒 1(即时)。要真跨线程测试需让 yield 在注册 dispatcher 时走
   async_await_yield_resume 挂起路径;同步 smoke 仍可走即时完成(两路并存)。

recommended_next(fresh): R2 把 AsyncTestAssembly codegen 产出接实编译,先证 Start wrapper 真调 MoveNext + entry
真 produce completed=1 Task(不走 ThreadPool);绿后再叠 3/4 跨线程。
