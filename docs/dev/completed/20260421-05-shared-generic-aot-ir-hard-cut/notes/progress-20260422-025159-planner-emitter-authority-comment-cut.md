# 2026-04-22 02:51:59 +08:00 进展记录

## 本轮完成

- `NativeAotLoweringPlanner` 开始直接读取 `method.OpenDefinitionSubjectId / SharedGenericBodyId / InstantiationStubId`
- `MethodEmission` 开始直接读取 `targetReference.OpenDefinitionSubjectId / SharedGenericBodyId / InstantiationStubId`
- generated cpp 注释统一改为 `Generic execution authority`

## 结论

- planner/emitter 已经开始消费 AOT IR 的显式 authority 字段，而不是只盯着 `RuntimeGenericContextArtifact`
- 但真实执行路径仍然由 `TargetSymbol` 主导；下一刀要切 invocation planning / invocation ABI
