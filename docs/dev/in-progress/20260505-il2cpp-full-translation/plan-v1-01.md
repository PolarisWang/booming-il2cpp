# Native AOT 无 Label/Goto 发射改造实现计划

> **面向执行 Agent：** 必须使用 `dev-executing-plans` 或 `dev-subagent-driven-development` 执行本计划。步骤使用 `- [ ]` 复选框语法跟踪。

**目标：** 从 `NativeAotLoweringPlanner` 生成器源头移除 `chaos_ip_*` label 与 `goto chaos_ip_*` 发射模型，在保持语义正确性的前提下，把可约 CFG 固定为纯结构化 C++，把不可约 CFG 统一收口到无 `goto` 的 `pc-dispatch` fallback。

**架构：** 继续沿用现有 `CFG -> Structured IR -> C++` 主路径，不把整条 Native AOT 主线退化成解释器式状态机。可约控制流保留 `if/else/switch/while/do-while` 输出；不可约控制流与异常续流统一走局部 `pc` continuation / dispatch 模式。

**技术栈：** `C#/.NET 8`、`Chaos.IL2CPP.CodeGen`、Native AOT generated C++、`unittest/pytest` 治理测试、owner subject proof/native/hotupdate 验证链。

**架构审核模式：** critical

**结构告警重点：** 结构化控制流恢复与 fallback 不能继续混在同一发射路径；异常续流不能再以 label/goto 特判补洞；测试门禁不能继续正向依赖 `chaos_ip_*` 文本。

**权责图审核主题：** `il2cpp/codegen/test_governance` 三条主线在“无 goto 发射”上的 authority 对齐：CodeGen 拥有输出形状 authority，Test Governance 拥有 formal gate authority，SolutionCorePack 拥有 canonical proof/native proof authority。

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: `il2cpp-codegen`
- capabilityItem: `native-aot-no-goto-emission`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: true
- benchmarkRequired: true
- hotupdateImpact: Proof
- formalVerificationObjects: `tests/unit/compatibility/*NativeAot*`、`tests/unit/performance/test_native_aot_workload_entry_bundle.py`、`SolutionCorePack` canonical managed proof、canonical native proof、hotupdate proof、representative benchmark families
- requiredGates: `collector -> registry -> workspace`

**设计文档：** n/a

**问题清零来源：** `docs/dev/in-progress/il2cpp-full-translation/STATUS.md` 中已清零的 blocking questions + 本轮用户直接确认“从生成器源头去掉 label/goto 模型，做开发计划”

**计划来源：** direct-plan

**预期知识沉淀：**
- `wiki/03-功能模块/06-il2cpp核心架构/02-翻译路径参考/09-结构化控制流恢复翻译表.md`
- `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/04-NativeAotLoweringPlanner文件布局.md`
- `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/05-异常处理架构.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`

**收尾约束：** 执行完成后必须进入“结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交”固定链路。本计划只冻结实现方案，不自动切换 `docs/dev/ACTIVE.md`。

---

## 涉及文件与职责

- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.StructuredControlFlow.cs`
  负责 CFG 构建、可约性判定、结构恢复边界；本轮需要明确“Structured IR 主路径”和“pc-dispatch fallback”分界。
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.StructuredIR.cs`
  负责 Structured IR 节点模型与递归发射；本轮需要移除 `IRGoto`、label 收集和 `goto chaos_ip_*` 发射。
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs`
  负责普通指令、分支、`EmitInstructionRange` 旧链路；本轮需要把分支/顺序续流切换到结构化或 `pc-dispatch`。
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs`
  负责 catch/filter/finally shape 发射；本轮需要移除所有 `goto chaos_ip_*` 异常入口与尾续流。
- `tests/unit/compatibility/test_il2cpp_codegen_structure_governance_native_aot_emission.py`
  负责 codegen 结构治理；本轮需要增加“禁止 label/goto”门禁。
- `tests/unit/performance/test_native_aot_workload_entry_bundle.py`
  当前仍显式断言 `chaos_ip_30`；本轮需要改成行为与结构双重断言，不再绑定旧文本。
- `tests/unit/compatibility/*NativeAot*`
  负责 planner/emitter 约束；本轮需要补 coverage，锁住 reducible 与 irreducible 两类路径。
- `verification/foundation-dll/**`
  不直接手改 generated 产物，但作为代表性回归输入，用于验证 `goto` 清零、proof/hotupdate/benchmark 不回退。

## 架构与治理约束

- [ ] 所有生成器主路径不得再发射 `chaos_ip_*` label 或 `goto chaos_ip_*`。
- [ ] 可约 CFG 仍必须优先输出结构化 C++，不得一刀切退化为全量状态机。
- [ ] 不可约 CFG 允许 fallback，但 fallback 必须是无 `goto` 的 `pc-dispatch`，且只作用于方法局部。
- [ ] 异常处理续流必须与普通 CFG 续流统一到 continuation/dispatch 语义，不能保留异常专用 label 跳转旁路。
- [ ] 测试治理必须先锁住“禁止 goto”与“不可约 fallback 仍可编译执行”两类门禁，再动实现。
- [ ] 相关长期规则一旦落地，需要同步更新 wiki authority 文档与 AOT 自测规范。

## 分阶段任务

- [ ] Phase A：冻结现状与门禁
  产出当前 `goto` 基线统计，定位仍生成 `goto chaos_ip_*` 的源码路径，补充/修订 unit tests 与 governance tests。
  完成标准：测试层不再正向依赖 `chaos_ip_*`，并新增“generated cpp 不得包含 `goto chaos_ip_` / `chaos_ip_:`”断言。

- [ ] Phase B：普通结构化控制流主路径去 goto
  在 `StructuredIR.cs` 与 `MethodEmission.cs` 中移除 `IRGoto`、`CollectGotoTargets`、`_emittedLabels`、`AppendGotoNext` 主路径依赖。
  完成标准：reducible CFG 只发射 `if/else/switch/while/do-while/break/continue/return/throw`，不再残留 label/goto。

- [ ] Phase C：不可约 CFG fallback 切换为 `pc-dispatch`
  保留 `IsCfgReducible` 判定，但 `false` 时不再走 `EmitInstructionRange()` 的 flat goto 模式，改成方法局部 `pc` 状态机。
  完成标准：交叉分支、复杂循环、无法结构恢复的方法仍可编译执行，且 generated cpp 中不出现 `goto`。

- [ ] Phase D：异常路径去 goto
  在 `ExceptionEmission.cs` 中重写 catch/filter/finally 入口与尾续流模型，把 `leave/endfinally/rethrow` 收口到 continuation/dispatch。
  完成标准：异常 shape 发射不再出现 `goto chaos_ip_*`，异常 proof 与 hotupdate proof 行为不回退。

- [ ] Phase E：回归、性能与体积评估
  对代表性 family 和 workload 执行 proof/benchmark/hotupdate 回归，比较 `goto` 数量、生成文件体积、编译时间与运行结果。
  完成标准：代表性 generated cpp `goto` 清零，correctness 全绿，benchmark 无显著负回归。

## 详细执行步骤

- [ ] Step 1：建立基线与失败测试
  新增或修改针对 `NativeAotLoweringPlanner` 的 unit tests，覆盖：
  `reducible CFG -> 无 goto`
  `irreducible CFG -> 无 goto 的 pc-dispatch fallback`
  `exception shapes -> 无 goto`
  `workload bundle -> 不再断言 chaos_ip_*`

- [ ] Step 2：抽离发射后端
  明确 `StructuredEmitter` 与 `PcDispatchEmitter` 两套后端职责，避免继续在同一 emitter 上叠加条件分支。
  要求：结构化主路径不感知 label 集合；fallback 路径不污染结构化节点模型。

- [ ] Step 3：清理 Structured IR 中的 goto 模型
  删除 `IRGoto` 及其发射、target 收集与 label 去重机制。
  把原先跨块跳转改写为：
  循环内 `break/continue`
  方法级 continuation/pc 赋值
  直接 `return/throw`

- [ ] Step 4：替换旧的 `EmitInstructionRange()` fallback
  把不可约 CFG 和无法结构恢复的残余路径收口到 `pc-dispatch`，不再回退到“每条 IL 一段 label + goto-next”。
  保留必要的 eval-stack/slot 映射，但不得重新引入 `goto-next` 语义。

- [ ] Step 5：重做异常 shape 续流
  把 `EmitCatchOnlyExceptionMethodBody`、`EmitFilterOnlyExceptionMethodBody`、finally 续流逻辑与 `leave/endfinally` 行为改成 continuation/dispatch 驱动。
  要求：异常处理路径与普通控制流路径共享“无 goto”发射约束。

- [ ] Step 6：清理残留治理与调试噪音
  删除调试输出、更新文档与测试辅助，确保没有新的正向依赖绑定旧实现细节。

- [ ] Step 7：代表性 formal 验证
  选择以下代表性输入做回归：
  `System.Runtime.InteropServices/native-memory-pointers`
  `System.Private.CoreLib/reflection-*`
  `System.Linq/*`
  `test_native_aot_workload_entry_bundle`
  需要覆盖 managed/native/hotupdate/proof/benchmark 视角。

## 验证命令

- [ ] 运行 unit/contracts 治理测试
  `pytest tests/unit/compatibility -k NativeAot`
  `pytest tests/unit/performance/test_native_aot_workload_entry_bundle.py`

- [ ] 运行代表性 native/contracts 回归
  `dotnet test` 或仓库既有 native proof 入口，至少覆盖一个普通控制流 proof、一个异常 proof、一个 hotupdate proof。

- [ ] 运行 foundation-dll 代表性 family 验证
  至少验证 `System.Runtime.InteropServices/native-memory-pointers`、`System.Private.CoreLib/reflection-*`、`System.Linq/*`。

- [ ] 运行 formal 刷新
  如果本轮修改触及 formal report/projection contract，则执行：
  `run verify verification-v1 --json`

## 验收标准

- [ ] 生成器源码主路径中不再保留 `goto chaos_ip_*` / `chaos_ip_*:` 发射逻辑。
- [ ] 代表性 generated cpp 文件中的 `goto` 数量为 `0`。
- [ ] reducible CFG 仍保持结构化输出，不退化为全量状态机。
- [ ] irreducible CFG 仍可编译、可执行、可验证。
- [ ] 异常 catch/filter/finally 行为与改造前保持语义等价。
- [ ] owner subject managed/native/hotupdate proof 全部通过。
- [ ] benchmark 无显著负回归；若有回退，必须记录裁决理由并给出后续优化入口。
- [ ] wiki 与测试治理文档同步更新完成。

## 风险与裁决

- [ ] 风险：异常路径语义回归
  裁决：异常路径单独阶段推进，不与普通 CFG 一次性混改。

- [ ] 风险：全量状态机化导致性能下降
  裁决：只允许不可约 CFG 进入 `pc-dispatch` fallback，可约 CFG 必须继续结构化。

- [ ] 风险：治理测试仍接受旧 `chaos_ip_*`
  裁决：Phase A 先改测试门禁，再改实现。

- [ ] 风险：generated cpp 体积虽去掉 goto，但因 dispatcher 扩张反而膨胀
  裁决：Phase E 必须输出体积与编译时间对比，必要时追加 shard/helper 抽取优化任务。
