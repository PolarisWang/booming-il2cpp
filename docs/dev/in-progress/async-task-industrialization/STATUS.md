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




### 2026-09-08 段 B/C（原 commit 35ab089c0，已 reverted 后 re-applied 为 75f22310c + c9cfec49a）真实边界校准
用 live AOT-IR 提取(AsyncTestAssembly) probe 实证段 C 的 entry 发射**结构早已完整**（建 box+Create+Start+
builder.get_Task 都吐了）——真正的 gap 不是缺 entry emission，而是 **builder 6 操作都落 ChaosExternalRuntimeFallback**。
本 commit 闭合了那部分：给 async.h + ShapeRegistry 接真 native。

> **提交历史**：此段内容先由 `35ab089c0` 提交，后因 CI 门禁问题被 `a9650df30` revert，再由 `75f22310c` 原样 re-apply
> （`c9cfec49a` 补充 R2a 冒烟验证）。HEAD 中 Segment B 代码真实存在，STATUS.md 描述与 HEAD 一致。

确凿（绿测试为证,codegen 2149/2149 段 B/C 增量后测得 + AsyncPipelineTests 5/5;
计数演进:前文 L191 "1986" = 段 A（Step2+3）基线;本段 "2149" = 段 B/C 增量后的 codegen 总数,
为各会话各自实测值，随测试数量递增而不同,非矛盾。当前 HEAD 全量 codegen 实测为 2150）:
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
   GC-heap box(`CHAOS_IL2CPP_NEW_GC`)+ continuation 持 box;否则 UAF。设计见 roadmap 决定(GC 堆非 pinned)。
4. **Task.Yield is_completed 策略**：async.h 现恒 1(即时)。要真跨线程测试需让 yield 在注册 dispatcher 时走
   async_await_yield_resume 挂起路径;同步 smoke 仍可走即时完成(两路并存)。

recommended_next(fresh): R2 把 AsyncTestAssembly codegen 产出接实编译,先证 Start wrapper 真调 MoveNext + entry
真 produce completed=1 Task(不走 ThreadPool);绿后再叠 3/4 跨线程。

### 2026-09-08 R2a（native 冒烟验证）done
**REMAIN 项 R2a 已完成（c9cfec49a）**：async.h native helpers 真人编译验证 + 冒烟。
确凿（native test_async_integration_smoke 17 tests 编译过,SegmentB_*2 新+HandCrafted*1 旧均 PASS）:
- `async_task_builder_start` 驱动手写 MoveNext → Task 同步 completed。（SegmentB_AsyncTaskBuilderStartCompletes）
- `async_await_task_resume` 注册 continuation,从 ThreadPool worker complete Task → MoveNext 重入并 SetResult。
  （SegmentB_AsyncAwaitTaskResumesAndCompletes）
另修 ContinuationRunsOnThreadPoolWorker pre-existing capturing-lambda 不匹配 fn-ptr AsyncContinueFn(改 ctx-ptr)。

REMAIN(2,3,4)未做:
- R3 Task.Yield is_completed: 现恒1(即时)。.NET Task.Yield语义 is_completed=false(恒异步线程池跳)。真让
  codegen-emitted GetOne 挂起跨线程需 is_completed 返回0(需 dispatcher 注册时才如此,保留同步 smoke 路径)。
- R4 e2e: 需把 codegen-emitted GetOne/DoVoid 真实 C++ 接进 native 测试 build 编译链接(全 R2 pipe 的前置)。
- R2b GC-heap box: entry 仍 stack `__chaos_stack_obj` 分配 d__; 跨 await 跨线程挂起需 CHAOS_IL2CPP_NEW_GC。

### 2026-09-08 R3+R4（Task.Yield 跨线程 + e2e round-trip）done
REMAIN 项 3 和 4 已完成（672bcee2a）。确凿（native test_async_integration_smoke）:
- R3: async_yield_get_is_completed 注册 dispatcher 时返 0(挂起);无 dispatcher 返 1(同步,保 HandCrafted 路径)。
- R4: TaskYieldRoundTripAcrossThreadPool —— 完整跨线程往返:
  builder_start → MoveNext(is_completed=0挂起→queue ThreadPool)→ worker fire continuation
  → MoveNext 重入(state=0→SetResult(1)→completed=1,result=1)。4 测试 ALL PASS。

### 2026-09-08 R2-full（真 codegen 产物实编译+驱动）状态
真 codegen 产物已实编译+链接+运行到 native exe（artifacts/r2full/asyncgen/r2full_asyncgen.exe）。
确凿证据（运行输出）:
- ChaosRuntimeHost 初始化 + threadpool dispatcher armed(g_async_dispatch_continuation_fn!=0)。
- codegen GetOne entry(AOT 非手写)被调用;Start wrapper 执 async_task_builder_get_task + 驱动 MoveNext。
- MoveNext suspend 路径 engaged: async_yield_get_is_completed armed==1 → 0(override 文件生效) → 走 AwaitUnsafeOnCompleted。
- AwaitUnsafeOnCompleted(YieldAwaiter) wrapper: g_async_dispatch_continuation_fn!=0 时 new ContinuationData + queue 到 threadpool。

REMAIN 缺陷（只差最后一步）: task 完成但 result==0 非 1。MoveNext resume 重入(state==0)未正确跳到
SetResult(1) 尾(或 result local 未填 1)。看 native-aot.generated.cpp ~1438+ probe + ~1623 SetResult tail:
MoveNext 结构是 if(state==0){await setup} else{resume},但 Tail(CHAOS_EH_END 后)无条件 SetResult + 重
置 state=-2。可能 resume 重入写 state 分支/result local(chaos_locals[1]) 用到了初值 0。
命令复现: cd artifacts/r2full/asyncgen && cmake --build build --config Debug && ./build/Debug/r2full_asyncgen.exe
(cmake 需 -DCHAOS_SDK_ROOT 指向 source-coherent ref preset libs,见 CMakeLists)。
下一步: 修 MoveNext resume 语义 — 确证 resume 时 MoveNext 读到 state==0 应 goto 尾(SetResult)而非重跑
await setup。或改 codegen switch 使 resume(非首跑)分支进 tail。

### HANDSIGN: 2026-09-09 会话 3 — dispatch 极性 + suspend return + 预存 C++ 编译缺陷全部修复, R2-full 最终 block 为 async box ABI 间接层

#### 2026-09-09 已推送修复 (3 commits, 均 `origin/main`)

| Commit | 内容 | 域 | 验证 |
|--------|------|----|------|
| `2b3fd2511` | **async MoveNext resume-state**: (1) `BuildIfThenElse` trampoline 链追逐,解决 dispatch 极性颠倒（state==0 → resume 分支; state!=0 → setup 分支）; (2) `AppendSuspendReturnsForAsyncMoveNext` 在 `AwaitUnsafeOnCompleted` 后插 `IRReturn`, 阻止首次进入 fall-through 到尾 `SetResult(0)` | codegen | 2153/2153 PASS + 新回归测试 |
| `8e41cceda` | **header extern `{;`**: `BuildTypeDeclarationsCode` 取 Source 首行作 extern 声明时去掉尾 `{` (async helper 多行 body 源残 `{` → C2598/C1075) | codegen | 2153 PASS |
| `31ad4a921` | **gc_dirty_card_dst_ref cast**: second arg reinterpret_cast<const void*> (1de5c36ec LEAF barrier 后 intptr_t 不再隐式转换) | codegen | 2153 PASS; R2-full 能编译链接 |

#### 修复后 R2-full (真 codegen 产物) 运行行为

```c++
// 现已编译+链接+启动通过。emitted MoveNext(for GetOne):
//   if (state == 0) {   ← RESUME path (correct!)
//       reload u__1 awaiter; GetResult; chaos_locals[1] = 1;
//   } else {             ← SETUP path (correct!)
//       yield_create; get_awaiter; get_is_completed;
//       if (!is_completed) state=0; AwaitUnsafeOnCompleted(...); return;  ← SUSPEND + RETURN
//       // ^^^ suspend no longer falls through to tail SetResult(0)
//   }
//   tail: SetResult(chaos_locals[1])
```

运行输出到 2× `[override:is_completed] armed=1 → 0` 后**挂起**．不 segfault、不报错，task 不在合理时限内完成。

#### 真实根因(下一 session 入口精确说明)

**box 指针传递多了一层间接** —— 这是从段 A 出厂时就潜伏的根本 ABI 缺陷, 被旧的"无 suspend return → 永远 fall through 到尾 SetResult"掩盖了。因为我修好了 suspend return, 真实挂起走了第一次, 暴露了此缺陷。

**证据（生成 C++ artifacts/r2full/asyncgen/native-aot.generated.cpp）**:

GetOne 入口 (async entry, `AsyncTestAssembly_AsyncMethods_GetOne`):
```
15: auto* box = CHAOS_IL2CPP_NEW_GC(chaos_type_...d__0, {});  // GC 堆分配
16: box->header.type_info = ...;
19: chaos_locals[0] = (intptr_t)box;  // 栈槽 [0] = box_ptr
...
44: chaos_arg_0 = &box->t__builder;          // OK: builder_ref = &(box内的 builder 字段)
45: chaos_arg_1 = reinterpret_cast<INTPTR>(&chaos_locals[0]); // BUG: &stack-slot, NOT box_ptr
     //                    ^^^^^^^^^^^^^^^^^
     // 传的是"持 box ptr 的栈槽地址", 不是 box_ptr 本身
46: Start<SM>(chaos_arg_0, chaos_arg_1);     // Start wrapper 收到 chaos_arg_1 = &chaos_locals[0]
```
Start wrapper (同一文件):
```
extern "C" ... Start<SM>(... chaos_arg_1) {
    async_task_builder_get_task(chaos_arg_0);
    AsyncTestAssembly...MoveNext(chaos_arg_1);  // 此处 chaos_arg_1 是 &chaos_locals[0] (栈槽地址)
}
```

所以 MoveNext 收到的 `chaos_fn_arg_0` = `&chaos_locals[0]` (调用者的栈帧上的一个 INTPTR 槽).  
Sync 路径碰巧能工作: 因为 &slot 在调用者栈帧仍存活, `reinterpret_cast<chaos_type_d__0*>(chaos_fn_arg_0)` 读出的是栈槽内容(box ptr), 然后通过这个内容访问字段。这个"通过栈槽位置读取 box 指针"的模式本身就不稳定,但在同步单帧路径中存活足够久。

跨线程续列时彻底崩: 
```
AwaitUnsafeOnCompleted wrapper (chaos_arg_2 = &chaos_locals[4]):
244: auto* __data = new AsyncStateMachineContinuationData{ MoveNext_fn, reinterpret_cast<void*>(chaos_arg_2) };
     // chaos_arg_2 = &chaos_locals[4] (调用者栈帧的另一个 slot)
     // ContinuationData.sm_box = dead stack slot address
...
// worker 线程 fire continuation → AsyncStateMachineContinuationCallback:
data->move_next(reinterpret_cast<INTPTR>(data->sm_box));
// data->sm_box = 已消亡调用者栈的地址 → 读 garbage → 挂起
```

**这就是 STATUS REMAIN#3(GC-heap box + continuation 需持 box)的根本子**。陈旧的分析说 'stack `__chaos_stack_obj`' 是错的 — 实际代码已经是 `CHAOS_IL2CPP_NEW_GC`(2b3fd2511 前的 17ded8c5f 已经改了)。但 codegen 对 `ref this` 参数的 ABI 解析始终走的 `ldloca`(取局部槽地址) 而不是传递 box 的 GC 稳定地址。

#### 修法（给下 session 的精确入口）

**本质**: async MoveNext 是 value-type 实例方法, Roslyn IL 内 `ldarg.0` = `ref this` = 指向 **box GC 对象** 内含 >d__ 首位置的指针。当前 codegen 的 ABI 解析把这个 `ldarg.0` (参 0, type=value-type-by-ref) 映射成了 `chaos_args[0]` → 而 **EmitAbiArgumentInitialization** 对于 value-type this 填入的是 `&chaos_locals[i]` (持 box 的栈槽地址), 不是 box 值本身。

**修复目标**: 对于 async state machine MoveNext (或其他确认 GC-heap 盒化值类型的实例方法), 传递给 Start/AwaitUnsafeOnCompleted 的 `ref state_machine` 应该传 GC 对象的稳定指针(interior pointer to box->>d__), 而不是 `&chaos_locals[i]`。

**具体修改范围**:
1. **`EmitAbiArgumentInitialization`** (MethodEmission.cs 或其调用的 ABI 格式化代码): 识别 async state machine 场景, 把 `ldarga.s 0`/`ldloca 0`(value-this) 解析为 `chaos_args[0]`(直接传 box 值) 而非 `&chaos_locals[0]`。  
   **关键**: codegen 目前的 value-type this 通用处理是 `chaos_resolve_managed_value_pointer(...)` 或者 `&chaos_locals[0]`, 两者都产生栈间接 — 需要改为直接传持久指针。

2. **MoveNext 本身**: 现在 MoveNext 读 `chaos_args[0]`(Input: `&chaos_locals[0]`, 栈槽地址) → 用 `reinterpret_cast<type*>(chaos_args[0])` 读 box 内容 → 实际上读的是栈槽里存的 box ptr, 继续字段访问。如果改成直接传 `chaos_locals[0]`(box ptr), 链路上的 `reinterpret_cast<type*>(chaos_args[0])` 仍然正确(box ptr reinterpret 为 type* 访问 box 头部 >d__ 字段)。

3. **Start/AwaitUnsafeOnCompleted 的 chaos_arg_2(持 box 值的字段)**: 现在这些调用点传的是 `&chaos_locals[4]` (局部栈槽), 需要改为直接传可用于 cross-thread 的持久 box 指针。

**风险**: `ldloca 0`/`ldarga.s 0` 是 value-type 实例方法共享的全域 ABI 路径。直接改全局影响所有 value-type 方法。必须加 async state machine 专属守卫,或者确认不改变其他 value-type 实例方法的语义(它们通常不和跨线程续列相关)。

**推荐方案**: 在 `EmitManagedMethod` 的 async MoveNext 分支(MethodEmission.cs:232-249)或 `EmitAbiArgumentInitialization` 中插入: 若 `IsAsyncStateMachineMoveNext`, 不映射 `arg.0` → `chaos_args[0]` 然后 `&chaos_locals[0]` 转储, 改为**直接从 GC 对象传稳定指针**:
```
intptr_t this_box = CHAOS_IL2CPP_NEW_GC(...)/*已在 entry 分配*/;
// MoveNext 收到的 chaos_args[0] = this_box (box ptr, stable for cross-thread)
// 现有 reinterpret_cast<chaos_type_d__0*>(chaos_args[0]) 已在 MoveNext body 中运行, 读 box 头部的 >d__ 字段。
```

#### 当前架构决策(不要推翻)
1. **手工状态机翻译, 不做 C++20 coroutine**
2. **MoveNext 当普通 value-this 方法发射** — 复用现存结构化发射路径
3. **GC 堆 box(CHAOS_IL2CPP_NEW_GC)而非栈** — 已在 17ded8c5f 完成, 不需重做
4. **保障代码: 入口 SetResult/SetException 进 native async.h 路径**(段B)
5. **跨线程 dispatcher 注册(register_async_dispatch_continuation_fn)** — 已实现在 runtime init

#### 复现命令
```bash
# 1. Regenerate codegen artifacts from current codegen unit test
dotnet test tests/unit/managed/codegen/Chaos.IL2CPP.CodeGen.Tests.csproj \
  --filter "FullyQualifiedName~R2Full_RealCodegen_EmitsGetOneAndMoveNext"

# 2. Recompile native exe
cd artifacts/r2full/asyncgen
cmake --build build --config Debug

# 3. Run
./build/Debug/r2full_asyncgen.exe

# 4. 观察: 如修复正确, 应输出 task->result = 1 + PASS
```

#### 门禁
- codegen 全量: `dotnet test tests/unit/managed/codegen/Chaos.IL2CPP.CodeGen.Tests.csproj` (2153+, 无新增 FAIL)
- AsyncPipelineTests 7/7 PASS (包括 `MovenextEmittedSource_SuspendReturnsAfterAwaitUnsafeOnCompleted`)
- native smoke: `tests/unit/runtime-native/runtime-core/threading/async_integration_smoke_test.cpp` (17 tests, 无关改动不改它, 仅验证 async.h 语义完整)

## 执行进度（2026-09-09 会话 4）— Phase 3 P3-1: TaskCompletionSource 落地

### Step 1 ✅（native TaskSource 代理 + 4 CTest）
`src/native/common/chaos/async.h` 新增 `TaskSource` struct + `task_source_create/destroy`：
- `get_task()`：返回底层 AsyncTask handle
- `set_result(value)`/`set_exception(ex)`：完成/置错，fire continuation
- `try_set_result/set_exception/set_canceled()`：CAS gate 首次完成胜利

`async_integration_smoke_test.cpp` 加 4 TEST_F（TaskSource_*，全 PASS）：
- `SetResultCompletesAwaitingSM`：SM await tcs.Task → 外部 SetResult(42) → resume 读 42 完成
- `SetExceptionFaultsAwaitingSM`：SetException fault 读 0xBAD
- `TrySetFiresExactlyOnce`：首次胜利，第二/canceled no-op
- `SetResultFromWorkerThread`：跨线程 SetResult(7)

（注：SequentialAsyncAwaitPattern 超时为 **pre-existing flaky**，与本改动无关——clean baseline 也超时。）

### Step 2 ✅（ShapeRegistry 注册 TCS 完成信号方法）
`RuntimeHelperShapeRegistry.CoreStubs.Part1.S16.cs` 新增 `RegisterTaskCompletionSource`（Part1.cs 派发加入）。
非泛型 + 泛型(`<T>`)实例方法均 SimpleForward/Generic 路由到 native：
- SetResult / TrySetResult / SetException / TrySetException / SetCanceled / TrySetCanceled
- codegen 单测全量 **2165/2165 PASS**。

### Step 3 ✅（真实 TCS async subject + pipeline）
- `AsyncTestAssembly/AsyncMethods.cs` 加 `async Task<int> AwaitTcs()`：`new TaskCompletionSource<int>()` + `await tcs.Task`。
- `AsyncPipelineTests` 加 `FullAssemblyClosure_SurfacesTaskCompletionSourceSubject`（验证 `AsyncMethods::AwaitTcs` 与其 `>d__::MoveNext` 及含 TaskCompletionSource callee 的表面化）。
- 既有 yield 字段断言测试显式指向 `<GetOne>d__`（AwaitTcs 的 MoveNext 不 yield，避免误匹配）。
- AsyncPipelineTests **10/10 PASS**。

### REMAIN（尚未 done — 需独立 codegen-域 session）
1. **TCS `.ctor` + `get_Task`**：TCS 是携带 `m_task:Task` 字段的对象模型类型。此二对象模型原语走完整的 reference-type 发射(ObjectModelEmission 收录 + 字段 + MethodTable)，非完成信号类 extern 直发。此前只 callee 存在、无 canonical 发射体。需在 codegen-域做对象模型注册后才打通(这就是 TCS 泛型收集风险点)。
2. **ObjectModelEmission 泛型收集**: TCS`<T>` 需进 referenceTypeSubjectIds（仿 ObjectModelEmission :415/:445 显式 Track），否则 C++ CHAOS_IL2CPP_NEW_GC 遇 C2027。
3. **跨线程 UAF 防护**: `AsyncTask` 目前在 async.h 是 plain-new 持根。TCS 跨线程被持有需 GC box(Phase 5 perf / GC lifetime)。
4. 现 foundation-dll `threading-tasks` chunk 无真实 SPCoreLib `new TaskCompletionSource` 代码路径（仅 CombinedSubjects test-harness 用 SubjectInstanceFactory::Create 建），故 Step-2 注册给完成信号方法已能让 harness probe 的 `tcs.SetResult/SetException/TrySet*` 直接 native 而非 stub 0-wall；`.ctor/get_Task` 的剩余仍会 fallthrough 到 interpreter，直到 (1)(2) 完成。

→ **P3-1 本 session 落地 = native 代理 + 4 CTest + registry 完成信号路由 + pipeline TCS subject 表面化。** 缺 `.ctor/get_Task` 对象模型 = 已建档 REMAIN，移交 codegen 域。

