# AOT 通用泛型 Task Lowering 与完整正确性回归执行计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 把当前按 `Task<int> / Task<bool> / Task<string>` 等具体返回类型分叉的 codegen 路径收敛成通用泛型 lowering，并完成一轮覆盖 codegen 合约、emit 产物、AOT 生成与执行工作流的完整正确性回归。

**架构：** 先冻结 `Task<T> / TaskAwaiter<T> / AsyncTaskMethodBuilder<T>` 的通用 shape contract，再把 runtime-skeleton 与 native-aot 两条 codegen 路径中的按具体类型分叉逻辑折叠为同一组“泛型 shape + 类型参数/元数据描述 + 单模板族”实现。验证上分为四层：源码契约、emit 产物、AOT compatibility 回归、subject worker/native-aot 工作流回归。

**技术栈：** C# / .NET 8、Scriban、pytest、`emit-native-aot`、`emit-native-reference`

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: AOT codegen generic-sharing normalization
- capabilityItem: generic `Task<T>` / `TaskAwaiter<T>` / `AsyncTaskMethodBuilder<T>` lowering unification
- ownerSubjectId: `SolutionCorePack`
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects:
  - `tests/unit/compatibility/test_phase3_async_task_builder_native_aot.py`
  - `tests/unit/compatibility/test_phase3_generic_sharing_boundary_native_aot.py`
  - `tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_core.py`
  - `tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py`
  - `tests/unit/execution/test_subject_workers_build_native_aot_contracts_core.py`
  - `tests/unit/execution/test_subject_workers_build_native_aot_entry_routing.py`
  - `tests/unit/execution/test_subject_workers_runtime_observe_native_aot.py`
- requiredGates: `collector -> registry -> workspace -> owner managed proof -> owner native proof`

**设计文档：** `docs/dev/in-progress/20260421-01-aot-generic-task-lowering-normalization/design-v1-01.md`

**预期知识沉淀：**
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/managed-native-hotupdate-test-pipeline.md`
- `wiki/06-测试验证/INDEX.md`
- 如新增长期规则，再补对应 AOT / codegen wiki

**收尾约束：** 执行完成后必须进入“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”固定链路。

---

## 现状结论

- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs` 当前仍保留 `AsyncTaskIntResultShape`、`AsyncTaskBoolResultShape`、`AsyncTaskStringResultShape` 以及配套 `TryBuildAssemblyBoundAsyncTaskInt/Bool/StringFactoryStub`、`ValidateAsyncTaskInt/Bool/StringFactoryShape` 之类专用分支。
- `src/managed/Chaos.IL2CPP.CodeGen/ReferenceProof/NativeReferenceProofCatalog.cs` 仍保留 `RuntimeSkeletonAsyncTaskIntFactoryStubTemplateRelativePath`、`...Bool...`、`...String...` 以及对应 `AsyncGetResult` 模板族。
- 当前已经存在 AOT 侧异步与 generic-sharing 回归入口：
  - `tests/unit/compatibility/test_phase3_async_task_builder_native_aot.py`
  - `tests/unit/compatibility/test_phase3_generic_sharing_boundary_native_aot.py`
- 当前“完整 AOT 正确性”仍是分散的能力点回归，尚未围绕“通用泛型 Task lowering cutover”做一轮统一收口。

## 目标文件与职责

- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs`
  - 移除按 `int/bool/string` 分叉的 runtime-skeleton async task lowering 分支。
  - 引入通用 `Task<T>` shape 描述、泛型模板模型、以及必要的 metadata/type 参数抽象。
- `src/managed/Chaos.IL2CPP.CodeGen/ReferenceProof/NativeReferenceProofCatalog.cs`
  - 折叠异步 task factory / get-result 模板目录常量与加载入口，避免每个返回类型一份 catalog API。
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeReferenceProof.RuntimeSkeleton.AsyncTask*.scriban`
  - 收敛为通用模板族，模板输入改成“泛型结果类型 + builder/task/awaiter subject/token 绑定”。
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeReferenceProof.RuntimeSkeleton.AsyncGetResult*.scriban`
  - 同上，收敛为通用模板族。
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/**` 与 `NativeAotEmitter.cs`
  - 审计 native-aot 路径中是否还存在按具体 `Task<int>` 直译的特殊化；如果存在，改为与 generic-sharing 一致的抽象。
- `tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py`
  - 为通用泛型 task lowering 增加 RED/GREEN 合约测试，锁定“不能再新增 `AsyncTaskInt/Bool/String` 专用模板族”。
- `tests/unit/compatibility/test_phase3_async_task_builder_native_aot.py`
  - 从 `System.Int32` 单一断言扩展为通用 family 断言，至少覆盖 `int / bool / string`，并为后续新增 `Task<T>` 结果类型保留可扩展结构。
- 新增或扩展 `tests/unit/compatibility/test_phase3_*native_aot.py`
  - 补充一个“通用 generic task result shape”回归对象，避免只在 `int` 上通过。
- `docs/dev/in-progress/20260421-01-aot-generic-task-lowering-normalization/STATUS.md`
  - 执行阶段逐步记录基线、切换点、验证结果、剩余风险。

## 执行步骤

- [ ] 步骤 1：冻结“通用泛型 Task lowering”契约与边界
  - 审计 runtime-skeleton 与 native-aot 两条 codegen 主线，列出所有 `Task<int> / Task<bool> / Task<string>` 专用路径、模板、catalog 常量、验证函数。
  - 明确本轮 cutover 的最小统一对象：
    - `Task<T>`
    - `TaskAwaiter<T>`
    - `AsyncTaskMethodBuilder<T>`
    - 对应 factory / await / get-result shape
  - 明确本轮暂不覆盖对象：
    - `ValueTask<T>` 如当前实现路径仍未并入 `Task<T>` family，则作为后续任务单列，不和本轮混做。

- [ ] 步骤 2：先补 RED 测试，再改 emitter / catalog / templates
  - 在 compatibility tests 中加入源码级 RED：
    - 不再允许新增 `AsyncTaskIntResultShape / AsyncTaskBoolResultShape / AsyncTaskStringResultShape` 这种按具体结果类型切分的公共结构。
    - 不再允许 `RuntimeSkeletonAsyncTaskIntFactoryStubTemplateRelativePath` 这类模板族继续扩张。
    - 必须存在通用模板/通用 shape 描述入口。
  - 保持测试先红后绿，避免在大文件里盲改。

- [ ] 步骤 3：实现 runtime-skeleton 的通用泛型 task lowering
  - 把 `NativeReferenceProofEmitter.cs` 中当前 `AsyncTaskResultShape` 及 `TryBuildAssemblyBoundAsyncTask*` / `ValidateAsyncTask*` 的三套分支折叠为单一通用 family。
  - 模板参数化为：
    - 结果类型显示名/subject id
    - builder/task/awaiter 相关 callee 前缀
    - 参数个数约束
    - 返回值 carrier / literal op 要求
  - 删除已废弃的具体类型模板和 catalog 加载入口。

- [ ] 步骤 4：审计并统一 native-aot 路径
  - 审核 `src/managed/Chaos.IL2CPP.CodeGen/Emission/**` 与 `NativeAotEmitter.cs` 中对 async / generic sharing 的处理。
  - 如果 native-aot 侧对 `Task<int>` 等存在专门 lowering 或硬编码类型判断，改为复用同一套通用泛型判定与模板模型。
  - 如果 native-aot 当前没有同类分叉，也要补齐 contract test，明确“native-aot 只能消费通用 task shape，而不是新增具体 T 的特殊分支”。

- [ ] 步骤 5：扩展验证对象，建立“泛型 task family”回归矩阵
  - 扩展现有 async builder proof，使其至少显式覆盖：
    - `Task<int>`
    - `Task<bool>`
    - `Task<string>`
  - 在 generic-sharing boundary proof 中补充与 async task family 的交叉用例，确认：
    - 闭包/状态机类型仍走 generic-sharing
    - builder / awaiter / get-result 不因结果类型不同而复制出新的 codegen 分支
  - 如现有 proof subject 不足，新增最小 subject，而不是在生产 emitter 中用临时样例“脑补通过”。

- [ ] 步骤 6：完成完整 AOT 正确性回归
  - 第一层：源码/模板契约
    - `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_core.py -q`
    - `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py -q`
  - 第二层：owner async/generic-sharing proof
    - `python -m pytest tests/unit/compatibility/test_phase3_async_task_builder_native_aot.py -q`
    - `python -m pytest tests/unit/compatibility/test_phase3_generic_sharing_boundary_native_aot.py -q`
  - 第三层：AOT compatibility sweep
    - `python -m pytest tests/unit/compatibility -k "native_aot" -q`
  - 第四层：workspace / subject worker
    - `python -m pytest tests/unit/execution/test_subject_workers_build_native_aot_contracts_core.py -q`
    - `python -m pytest tests/unit/execution/test_subject_workers_build_native_aot_entry_routing.py -q`
    - `python -m pytest tests/unit/execution/test_subject_workers_runtime_observe_native_aot.py -q`
  - 第五层：构建入口
    - `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release`
  - 如任一 `dotnet build/test/msbuild` 出现编译崩溃，立即按 `dev:systematic-debugging` 处理，不得跳过。

- [ ] 步骤 7：清理与归档
  - 删除已废弃模板、catalog 常量、临时脚本、试验目录。
  - 保持仓库无新增无用 `.tmp`/脚本噪音。
  - 在 `STATUS.md` 记录：
    - 基线 special-case 清单
    - cutover 后删除的专用族
    - 全量验证命令与结果
    - 未纳入本轮的 `ValueTask<T>` / 其他 async family 风险

## 关键技术约束

- 不允许把 `Task<int>` 专用模板简单重命名为“generic”后继续保留 `int/bool/string` 分叉实现。
- 通用方案必须以“一个 family + 参数化输入”的方式落地，而不是再为每个 `T` 扩增一套 `TryBuild...` / `Validate...` / `TemplateRelativePath`。
- 需要保持当前已落地的 runtime-skeleton `method_id`、descriptor-driven page helpers、status taxonomy 不回退。
- 需要兼容现有 dirty worktree，不回滚用户未要求的改动。

## 风险与预案

- 风险 1：`Task<T>` family 看似统一，但 `builder/start/await/get-result` 在 IL shape 上仍有细微差异。
  - 预案：把差异放进 shape descriptor，不回退到按 `int/bool/string` 分叉。
- 风险 2：native-aot 与 runtime-skeleton 的 generic abstraction 演化不一致。
  - 预案：先冻结共享 contract，再分别接入两条 emitter。
- 风险 3：compatibility sweep 数量大、执行时间长。
  - 预案：先跑 owner proofs 和 contract RED/GREEN，再跑 `-k "native_aot"` 全量回归；执行期间持续记录失败分层。

## 执行期文档维护要求

- 开始执行前创建 `STATUS.md`，记录基线和步骤切换点。
- 每完成一个阶段，都要更新：
  - 当前已删除/替换的 special-case family
  - 新增验证对象
  - 已通过命令
  - 未解决阻塞
- 如本轮形成长期规则，再补 `design-v1-01.md` 和 wiki 沉淀。
