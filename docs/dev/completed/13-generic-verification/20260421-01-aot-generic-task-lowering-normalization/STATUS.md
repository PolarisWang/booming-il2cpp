# 20260421-01 AOT 通用泛型 Task Lowering

## 当前状态

- 状态: completed
- 完成阶段: Step 7 CLOSEOUT
- 当前目标: `20260421-01` 已完成，正式向 `20260421-07-dispatch-reflection-hotupdate-cutover` 交接

## 当前结论

- runtime-skeleton async task factory / get-result 已不再依赖 `SupportedAsyncTaskResultShapes` 这类 `int/bool/string` 结果类型白名单
- async family 现在由 `Task<T>` 返回签名、真实参数签名与 carrier 解析共同决定：
  - `Task<T>` builder / awaiter / get-result callee 由 `T` 推导
  - factory stub 参数 C++ carrier 由方法参数类型推导，不再跟着结果类型走
  - get-result wrapper 的 literal 验证改为基于 producer 方法参数，而不是基于结果类型猜测
- 新增了真实生成物回归：`Task<string>(int)` 这种“结果类型与参数 carrier 脱钩”的 case 现在会在 runtime-skeleton generated page 中强制出现 `int32_t arg0`
- native-aot async family 现有通用 raw carrier 路径保持成立；本轮没有回退到 `Task<int>` 专用 runtime prelude
- get-result stub 对引用返回值不再把 `nullptr` 视为模板层内部错误，从而避免把 `Task<T>` 的泛化能力重新绑死到个别字符串 proof 假设上

## 本轮修改文件

- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeReferenceProof.RuntimeSkeleton.AsyncTaskFactoryStub.cpp.scriban`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeReferenceProof.RuntimeSkeleton.AsyncGetResultStub.cpp.scriban`
- `tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py`
- `tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_validators.py`
- `tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py`
- `docs/architecture/managed-native-hotupdate-test-pipeline.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`
- `wiki/06-测试验证/INDEX.md`

## 架构审视

- runtime-skeleton async family 现在采用“family shape + managed type shape”的两层抽象：
  - `AsyncTaskFamilyShape` 负责 `Task<T>` / builder / awaiter 的 family contract
  - `AsyncManagedTypeShape` 负责参数与返回值的 carrier / literal 规则
- 这次 cutover 修掉了之前最危险的结构债：`Task<string>(int)` 会被错误地按字符串参数 carrier 生成 stub
- source contract 与 generated artifact contract 现在同时锁住“参数 carrier 必须来源于方法签名”这一点，后续 07/08 不容易再从 emitter 侧退回结果类型白名单
- native-aot 侧 `Task<T>` / `TaskAwaiter<T>` / `AsyncTaskMethodBuilder<T>` 的通用 raw carrier prelude 未被破坏；runtime-skeleton 与 native-aot 对 async generic family 的抽象方向现在一致

## 验证

- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_validators.py -q`
  - 结果: `16 passed`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -q`
  - 结果: `6 passed`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_core.py tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_validators.py tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py tests/unit/compatibility/test_phase3_async_task_builder_native_aot.py tests/unit/compatibility/test_phase3_generic_sharing_boundary_native_aot.py tests/unit/execution/test_subject_workers_build_native_aot_contracts_core.py tests/unit/execution/test_subject_workers_build_native_aot_entry_routing.py tests/unit/execution/test_subject_workers_runtime_observe_native_aot.py -q`
  - 结果: `40 passed`
- `python -m pytest tests/unit/compatibility -k "native_aot" -q`
  - 结果: `133 passed, 290 deselected`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果: `0 Error(s)`

## 残余风险

- runtime-skeleton 当前明确覆盖的 async carrier 仍以 `int32-like / int64 / float32 / float64 / string / object` 为主，尚未把任意自定义值类型/引用类型参数都纳入统一 carrier 规则
- `ValueTask<T>` family 在 native-aot owner proof 中已具备通用 raw carrier 路径，但 runtime-skeleton 侧尚未单独扩张到 `AsyncValueTaskMethodBuilder<T>` 的 factory/get-result wrapper 发射
- 仓库里仍存在其他与 `System.Int32` / `System.String` 绑定的 reference-proof string family / forwarder / constructor chain 逻辑；这些不属于本轮 async family cutover，本应留给 `20260421-08-legacy-generic-path-purge`

## 下一步

- 在父级 roadmap 中把 `20260421-01` 标记为已完成
- 启动 `20260421-07-dispatch-reflection-hotupdate-cutover`
- 优先审 dispatch / reflection / HotUpdate 是否仍依赖 `subjectId`、字符串 slot 或旧 registry authority

## 父级路线对齐

- 父级 roadmap: `docs/dev/in-progress/20260421-02-canonical-generic-runtime-kernel-roadmap/STATUS.md`
- 在父级路线中，本任务是 `planner-emitter-runtime-cutover` 阶段的首个 family 级子任务，现已完成收口
- 下一阶段是 `dispatch-reflection-hotupdate`，要求把 `InstantiationStubId + generation` authority 向 dispatch / reflection / HotUpdate bridge 继续推进
