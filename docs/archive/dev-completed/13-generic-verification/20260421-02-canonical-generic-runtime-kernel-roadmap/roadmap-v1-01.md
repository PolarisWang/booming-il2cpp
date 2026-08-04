# Canonical Generic Runtime Kernel Roadmap v1.01

Date: 2026-04-21 23:02:19 +08:00
Status: in-progress

## 1. 目标

建立一条 `loader/IR-first` 的 canonical generic runtime kernel 主线，把当前以闭合实例复制和字符串特判为核心的 generic codegen，迁移为：

- canonical generic identity
- instantiation demand graph
- shared generic AOT IR
- runtime generic context
- thin instantiation stub

成功标准不是“再过几条 proof”，而是：

- loader 不再把闭合实例复制当成 generic 主路径
- AOT IR 能稳定承载 shared-generic 语义
- planner/emitter/runtime 消费统一 IR，而不是继续扩字符串特判
- `Task<T>`、dispatch、reflection、hotupdate generic boundary 都进入统一验证矩阵

## 2. 范围边界

本 roadmap 负责：

- generic contracts / naming 统一
- loader instantiation graph 重构
- AOT core IR shared-generic 语义
- planner/emitter/runtime 对新 IR 的接入
- generic matrix、IR gates、native/hotupdate/benchmark 正式验证
- legacy generic 主路径清理

本 roadmap 不负责：

- 一次性接管所有 BCL generic family
- 动态代码生成相关 generic synthesis
- 与 generic kernel 改造无关的普通 codegen 清理

## 3. 非目标

- 不继续把 `Task<int> / Task<bool> / Task<string>` 这类 family-level 归一化当成总路线终点
- 不接受“保留旧 clone 路径，外面再包一层 generic 外观”作为完成口径
- 不把 hotupdate generic 风险推迟到 roadmap 结束后再处理

## 4. 阶段列表

### Phase 0: Canonical Generic Contract Freeze

- goal:
  - 冻结 canonical generic identity 与 specialization policy
- exit_criteria:
  - 明确 `type/method/type+method generic` 的 identity
  - 明确 `ref/value/byref/pointer/nested` 的 specialization policy
  - 明确本轮首批 shared-generic family
- deliverables:
  - `brainstorm-v1-01.md`
  - `design-v1-01.md`
  - `policy notes` 写入后续 child task
- dependencies:
  - none
- open_questions:
  - `ValueTask<T>`、pointer/byref 是否纳入第一轮

### Phase 1: Contracts And Naming Unification

- goal:
  - 统一 generic parser、identity、runtime context contract
- exit_criteria:
  - 仓库只保留一套 generic parser
  - `GenericInstantiationKey` / `RuntimeGenericContextArtifact` 合同稳定
- deliverables:
  - contracts 更新
  - parser consolidation
  - unit/contracts tests
- dependencies:
  - Phase 0
- open_questions:
  - `GenericContextArtifact` 与新 runtime context 的共存边界

### Phase 2: Loader Instantiation Graph Hard Cut

- goal:
  - 把 loader generic 主路径改成 instantiation demand graph
- exit_criteria:
  - `src/managed/Chaos.IL2CPP.Loader/LoaderStage.GenericMaterialization.cs` 不再以文本替换物化闭合实例为主路径
  - `src/managed/Chaos.IL2CPP.Loader/LoaderStage.CrossAssemblyInstantiation.cs` 改成 instantiation demand 收集
- deliverables:
  - loader 重构
  - cross-assembly demand graph
  - loader gates
- dependencies:
  - Phase 1
- open_questions:
  - 旧 metadata-only consumer 是否需要兼容层

### Phase 3: Shared-Generic AOT IR

- goal:
  - 让 AOT IR 显式表达 `shared body + runtime context + thin stub`
- exit_criteria:
  - 闭合 generic subject 不再默认等价于一整份 body
  - call site 可以映射到 `shared body + context/stub`
- deliverables:
  - AOT IR contract 更新
  - IR shape tests
  - generated artifact gates
- dependencies:
  - Phase 1
  - Phase 2
- open_questions:
  - `RuntimeContextId` 挂在 method、reference，还是单独 registry

### Phase 4: Generic Matrix And IR Gates

- goal:
  - 用矩阵证明 generic 语义与 IR shape，而不是只靠 proof case
- exit_criteria:
  - 覆盖 type/method/type+method generic
  - 覆盖 ref/value/nested/byref/pointer
  - 覆盖 `Task<T>/builder/awaiter`
  - 覆盖 interface/virtual/constraint dispatch、reflection、cross-assembly、hotupdate
- deliverables:
  - `generic matrix`
  - IR gates
  - size/memory benchmark baseline
- dependencies:
  - Phase 1
  - Phase 3
- open_questions:
  - 是否对每个 family 单独出 growth budget

### Phase 5: Planner / Emitter / Runtime Cutover

- goal:
  - 让 planner/emitter/runtime 正式消费 shared-generic IR
- exit_criteria:
  - `Task<T>`、`AsyncTaskMethodBuilder<T>`、`TaskAwaiter<T>` 不再依赖按闭合结果类型分裂的完整 body
  - family registry 替代 scattered prefix/pattern logic
- deliverables:
  - planner/emitter/runtime 改造
  - async family cutover
  - owner native proof
- dependencies:
  - Phase 3
  - Phase 4
- open_questions:
  - shared body 与 thin stub 的 naming/diagnostics 口径

### Phase 6: Dispatch / Reflection / HotUpdate Cutover

- goal:
  - 把 dispatch、reflection、hotupdate generic boundary 接到新 kernel
- exit_criteria:
  - interface/virtual/constraint dispatch 走新 runtime context
  - reflection generic construction 走新 canonical model
  - hotupdate generic boundary proof 全绿
- deliverables:
  - dispatch cutover
  - reflection cutover
  - hotupdate proof
- dependencies:
  - Phase 5
- open_questions:
  - patch 侧动态 instantiation 的最终支持边界

### Phase 7: Legacy Generic Path Purge

- goal:
  - 删除 legacy generic clone/materialization 主路径
- exit_criteria:
  - loader text substitution 主路径删除
  - scattered prefix/pattern 主路径删除
  - 过时测试断言和 helper 删除
- deliverables:
  - cleanup commit set
  - verification closeout
  - final documentation updates
- dependencies:
  - Phase 6
- open_questions:
  - 是否保留极短 emergency rollback 分支，不进入主线

## 5. 每阶段完成定义

- Phase 1 完成定义：
  - 新旧 parser 不再并存
  - contracts tests 固定新 identity 口径
- Phase 2 完成定义：
  - loader 可以输出 instantiation graph
  - 旧 `SubstituteText` / `SubstituteMethodBody` 不再为主路径
- Phase 3 完成定义：
  - `aot-core-ir.json` 能区分 shared body / runtime context / thin stub
- Phase 4 完成定义：
  - generic matrix 可稳定验证 IR shape 和 family 行为
- Phase 5 完成定义：
  - async generic family 切到新 IR 主路径
- Phase 6 完成定义：
  - dispatch/reflection/hotupdate 不再挂在旧 generic 执行模型上
- Phase 7 完成定义：
  - legacy generic 主路径彻底移除

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260421-03-canonical-generic-contracts-and-parser-unification` | `contracts-and-naming` | `ready` | `codex` | 统一 generic parser、identity、runtime context contract | `20260421-02` |
| `20260421-04-loader-instantiation-graph-hard-cut` | `loader-instantiation-graph` | `planned` | `codex` | 把 loader generic 主路径改成 instantiation demand graph | `20260421-03-canonical-generic-contracts-and-parser-unification` |
| `20260421-05-shared-generic-aot-ir-hard-cut` | `shared-generic-ir` | `planned` | `codex` | 在 AOT IR 中引入 shared body / runtime context / thin stub | `20260421-03-canonical-generic-contracts-and-parser-unification`, `20260421-04-loader-instantiation-graph-hard-cut` |
| `20260421-06-generic-matrix-and-ir-gates` | `verification-and-gates` | `planned` | `codex` | 建立 generic matrix、IR gates、size/memory baseline | `20260421-03-canonical-generic-contracts-and-parser-unification`, `20260421-05-shared-generic-aot-ir-hard-cut` |
| `20260421-01-aot-generic-task-lowering-normalization` | `planner-emitter-runtime-cutover` | `in-progress` | `codex` | 作为首个 family 级切入口，把 `Task<T>/TaskAwaiter<T>/AsyncTaskMethodBuilder<T>` 接到新 generic kernel | `20260421-05-shared-generic-aot-ir-hard-cut`, `20260421-06-generic-matrix-and-ir-gates` |
| `20260421-07-dispatch-reflection-hotupdate-cutover` | `dispatch-reflection-hotupdate` | `planned` | `codex` | 接管 dispatch、reflection、hotupdate generic boundary | `20260421-01-aot-generic-task-lowering-normalization` |
| `20260421-08-legacy-generic-path-purge` | `legacy-purge` | `planned` | `codex` | 删除 legacy clone/materialization 与 scattered prefix/pattern logic | `20260421-07-dispatch-reflection-hotupdate-cutover` |

## 7. 依赖

- `docs/archive/architecture/managed-native-hotupdate-test-pipeline.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`
- 现有 `Loader / Contracts / CodeGen / NativeAotEmitter / HotUpdate` 主线
- generic owner subject 和现有 native/hotupdate proof 资产

## 8. 风险

- Phase 2/3 会形成中间态：IR 先变，runtime 后变，必须依赖 gates 把住边界。
- 如果 contracts/IR 设计失误，Phase 5 以后返工成本很高。
- async family 是第一爆点，但不是唯一爆点；dispatch/reflection/hotupdate 往往迟爆。
- 若 Phase 4 不先建立 matrix/gates，后续会出现“proof 过了但泛化能力没建立”的假阳性。
- 值类型 specialization 误判会直接造成 ABI/layout 错误。

## 9. 备选路线

### 备选路线 A：验证先行硬切

- 更稳，但架构推进偏慢。

### 备选路线 B：runtime kernel 先行硬切

- 更快看到执行效果，但 contract/IR 返工风险最高。

### 备选路线 C：loader/IR 先行硬切

- 当前推荐路线。
- 原因：先统一 generic 语义中枢，再切执行主路径，整体风险最平衡。

## 10. 当前建议推进顺序

1. 创建并执行 `20260421-03-canonical-generic-contracts-and-parser-unification`
2. 创建并执行 `20260421-04-loader-instantiation-graph-hard-cut`
3. 创建并执行 `20260421-05-shared-generic-aot-ir-hard-cut`
4. 创建并执行 `20260421-06-generic-matrix-and-ir-gates`
5. 重新对齐 `20260421-01-aot-generic-task-lowering-normalization` 到新 IR
6. 再推进 `20260421-07-dispatch-reflection-hotupdate-cutover`
7. 最后执行 `20260421-08-legacy-generic-path-purge`
