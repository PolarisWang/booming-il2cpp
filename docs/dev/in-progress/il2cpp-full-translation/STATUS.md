# STATUS - 传统 IL2CPP 全量翻译

## Meta
- task_id: il2cpp-full-translation
- lifecycle: in-progress
- phase: planning
- task_type: roadmap
- roadmap_or_plan: docs/dev/in-progress/il2cpp-full-translation/roadmap-v1-01.md

## 当前计划状态
- active_plan: docs/dev/in-progress/il2cpp-full-translation/plan-v1-01.md
- plan_focus: 从 Native AOT 生成器源头移除 `chaos_ip_*` label / `goto chaos_ip_*` 发射模型
- active_note: 本轮实现与验证已完成；当前任务仍保留在 roadmap 主线下，不迁出 `il2cpp-full-translation`

## 最近摘要
原有 `roadmap-v1-01.md` 解决的是“全量 assembly 转 C++”的大路线。
本轮 `plan-v1-01.md` 已执行完成，落地结果如下：
- 普通 Native AOT 分支续流不再发射 `chaos_ip_*` label / `goto chaos_ip_*`
- `EmitInstructionRange()` 已切换为无 `goto` 的局部 `pc-dispatch`
- `IRExceptionRegion` 仍是异常统一入口；异常分区内部必要时可局部降级为 `pc-dispatch`
- `StructuredIR` 中的 `IRGoto`、label 跟踪、trace 输出已清理
- 相关治理与回归测试已更新到当前输出 contract
- 继续完成了生成代码质量收口：
  - `ExceptionEmission` 的 `stfld` placeholder 已替换为真实字段写入
  - `StructuredIR` 不再输出 `stack depth` 调试注释
  - `NativeAotLoweringPlanner` 不再额外注入一层重复 reachable method forward declarations
  - `pc-dispatch` 每个 case 末尾从不可达 `CHAOS_IL2CPP_ABORT();` 降为 `break;`，减少噪音
  - `StructuredIR` 主路径已切到 `_sN` 局部 slot lower，旧 `IRGoto` / label 跟踪 / 死 slot 后处理旧模型已删除
  - delegate `callvirt` 已切到 structured slot direct emission
  - `box/unbox/unbox.any/castclass/isinst/initobj/newarr/ldobj/stobj/cpobj/ldelema/localloc` 已切到 structured slot direct emission
  - `ldind/stind/ldelem/stelem` 已切到 structured slot direct emission
  - 仍未完成真实 lowering 的 opcode 不再以 placeholder/no-op 形式冒充 direct success，统一退回 residual `pc-dispatch`
  - exception shape detection 已从 `ExceptionEmission.cs` 拆到独立 planning authority
  - finally-only multiple handlers 已补 native AOT 专项覆盖，确认走嵌套 `IRExceptionRegion(TryFinally)` 路径
  - wiki 已同步当前 `B+` 路径：structured 主路径用局部 slot，residual partition 才退回局部 `pc-dispatch`

## 设计摘要
详见 `roadmap-v1-01.md` 与 `plan-v1-01.md`。
当前阶段不改变全量主路线 authority，只在 CodeGen、Exception、Test Governance 三条链上完成“无 label/goto 发射”收口，generated cpp 主路径已脱离 `chaos_ip_*` 模型。

## 关键文档
- roadmap: roadmap-v1-01.md
- plan: plan-v1-01.md

## blocking_questions
- blocking_questions: []
- question_clearance: cleared
- clearance_confirmed_by_user: true

## 架构映射
- il_feature: structured-control-flow + irreducible-cfg fallback + exception continuation
- translation_path: structured_cfg_recovery + pc_dispatch_fallback
- planner: `Emission/NativeAotLoweringPlanner.StructuredControlFlow.cs::BuildControlFlowGraph/RecoverStructure`
- emitter: `Emission/NativeAotLoweringPlanner.StructuredIR.cs` + `Emission/NativeAotLoweringPlanner.MethodEmission.cs` + `Emission/NativeAotLoweringPlanner.ExceptionEmission.cs`
- runtime_support: n/a
- constraints_followed:
  - `wiki/03-功能模块/06-il2cpp核心架构/02-翻译路径参考/09-结构化控制流恢复翻译表.md`
  - `wiki/03-功能模块/06-il2cpp核心架构/03-架构约束/01-翻译层硬性约束.md`
  - `wiki/03-功能模块/06-il2cpp核心架构/03-架构约束/05-全局优先级约束.md`
  - `wiki/06-测试验证/AOT新Feature接入自测规范.md`
- new_constraints:
  - generated cpp 主路径不得再出现 `chaos_ip_*` label 或 `goto chaos_ip_*`
  - reducible CFG 不得退化为全量状态机
  - exception continuation 必须与普通 CFG 一致遵守无 `goto` 发射约束
- consistency_check: ok
- wiki_entry: needs-update: `04-NativeAotLoweringPlanner文件布局.md`, `05-异常处理架构.md`
- design_discussion: n/a

## 下一步
回到 `roadmap-v1-01.md` 主线，继续推进全量 IL2CPP 翻译后续阶段；本轮 `native-aot-no-goto-emission` 子计划已完成。

## 完成证据

- arch_review: ok
- authority_review: ok
- verification:
  - canonicalCommand:
    - `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
    - `dotnet build-server shutdown`
    - `pytest tests/unit/compatibility/test_exception_catch_native_aot.py tests/unit/compatibility/test_dispatch_virtual_native_aot.py tests/unit/compatibility/test_generic_layout_native_aot.py tests/unit/performance/test_native_aot_workload_entry_bundle.py tests/unit/compatibility/test_il2cpp_codegen_structure_governance_native_aot_emission.py -q -k "structured_ir_source_has_no_label_or_goto_model or exception_emission_source_has_no_chaos_ip_goto or NativeAotWorkloadEntryBundleTests or ExceptionCatchNativeAotTests or DispatchVirtualNativeAotTests or GenericLayoutNativeAotTests"`
  - formalObject:
    - `tests/unit/compatibility/test_exception_catch_native_aot.py`
    - `tests/unit/compatibility/test_dispatch_virtual_native_aot.py`
    - `tests/unit/compatibility/test_generic_layout_native_aot.py`
    - `tests/unit/performance/test_native_aot_workload_entry_bundle.py`
    - `tests/unit/compatibility/test_il2cpp_codegen_structure_governance_native_aot_emission.py`
- verification_notes:
  - 串行执行通过；涉及 `dotnet build` 的 pytest 用例若并行启动，易触发 `VBCSCompiler`/`obj` 文件锁竞争
  - 推荐继续沿用 `dotnet build-server shutdown` + 串行 pytest 的回归方式
- test_result: passed
- wiki: updated: `wiki/03-功能模块/06-il2cpp核心架构/02-翻译路径参考/09-结构化控制流恢复翻译表.md`
- next: resume-roadmap
