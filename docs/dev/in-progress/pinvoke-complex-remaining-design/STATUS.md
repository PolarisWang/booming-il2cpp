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
| P5 (COM RCW) | ❌ not started | COM vtable 调用生成 |
| **P6 (Complex struct)** | **🔄 in-progress** | 非 blittable struct 编组 + StructureToPtr/PtrToStructure/DestroyStructure |
| P7 (验证) | ❌ not started | 综合端到端测试 |

## 架构映射

- il_feature: [pinvoke / struct-marshalling]
- translation_path: [complex-struct-descriptor-driven-marshalling]
- planner: [AotCoreIrLowering.cs::ClassifyValueTypeFields / EmitStructMarshallingDescriptors]
- emitter: [MethodEmission.cs::EmitPInvokeMethod]
- runtime_support: [struct_marshal.cpp / runtime_stubs.cpp]
- constraints_followed: [五层职责边界 / 合约桥接 / 性能最优]
- new_constraints: [无]
- consistency_check: [ok]
- wiki_entry: [needs-update: 翻译路径参考表新增 complex struct 编组条目]

## 下一步

1. P6a: StructFieldKind 扩展 + runtime marshaller 扩展
2. P6b: ResolveStructMarshallingDescriptor + 反射兜底
3. P6c: Codegen 发射 StructMarshallingDescriptorV1
4. P6d: P/Invoke wrapper 引用 descriptor
5. P6e: Marshal.StructureToPtr / PtrToStructure / DestroyStructure
