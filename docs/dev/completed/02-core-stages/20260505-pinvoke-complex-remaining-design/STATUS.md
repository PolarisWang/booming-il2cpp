# P/Invoke Complex Remaining — Architecture Design

**phase:** executing
**task_type:** roadmap
**created:** 2026-05-05
**updated:** 2026-05-05
**roadmap_or_plan:** roadmap-v1-01.md

## 设计确认

- 方案选择: **C — 混合分层 (Hybrid Layered)** — 静态 descriptor 发射 + 运行时反射兜底
- 三优先级权衡: P1(性能) = 静态 descriptor 零开销热路径; P2(架构) = codegen 拥有 struct 布局 authority; P3(热更) = 反射兜底支持动态类型
- blocking_questions: []
- question_clearance: cleared
- clearance_confirmed_by_user: true

## 当前阶段状态

| 阶段 | 状态 | 备注 |
|------|------|------|
| P1 (Blittable struct) | ✅ completed | AOT 内联 memcpy |
| P2 (String-in-struct) | ✅ completed | AOT 逐字段展开 + string helper |
| P3 (Delegate marshalling) | ✅ completed | GetFunctionPointerForDelegate + GetDelegateForFunctionPointer |
| P4 (Reverse P/Invoke) | ✅ completed | UnmanagedCallersOnly stub + bootstrap |
| P5 (COM RCW) | ✅ completed | EmitComVtableCall + com_abi.h + AotCoreIrLowering |
| **P6 (Complex struct)** | ✅ **completed** | P6a-e 全部实现 — field kind 扩展、descriptor 反射、codegen 发射、wrapper 引用、runtime stubs |
| P7 (验证) | ✅ completed | smoke all 5/5 pass, managed build 0 errors |

## 架构映射

- il_feature: [pinvoke / struct-marshalling / com-rcw]
- translation_path: [complex-struct-descriptor-driven-marshalling / com-vtable-dispatch]
- planner: [AotCoreIrLowering.cs / MethodEmission.cs]
- emitter: [MethodEmission.cs / ObjectModelEmission.cs]
- runtime_support: [struct_marshal.cpp / runtime_stubs.cpp / com_abi.h]
- constraints_followed: [五层职责边界 / 合约桥接 / 性能最优]
- new_constraints: [无]
- consistency_check: [ok]
- wiki_entry: [needs-update: 翻译路径参考表新增 complex struct 编组 + COM RCW 条目]

## 完成证据

- arch_review: n/a
- authority_review: n/a
- verification:
  - canonicalCommand: run.cmd test smoke all
  - formalObject: smoke/PInvokeLite
  - summaryPath: artifacts/logs/tests/20260505-204317-windows-fdaf/summary.json
  - eventsPath: artifacts/logs/tests/20260505-204317-windows-fdaf/events.jsonl
  - consolePath: artifacts/logs/tests/20260505-204317-windows-fdaf/console.log
- test_result: passed (smoke all 5/5)
- wiki: n/a
- next: merge

## 下一步

归档 completed → 合并&提交
