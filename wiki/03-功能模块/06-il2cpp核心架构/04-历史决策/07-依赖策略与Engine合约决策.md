# 依赖策略、Emitter 边界与引擎合约决策

**来源**: `docs/discuss/il2cpp-mainline/20260407-07-*` ~ `20260407-11-*`

## 依赖策略合约（20260407-07）

**决策**：采用两阶段依赖合约。

- `SemanticWorld` 输出 capability hints
- `Linker` 仅将其映射到 managed external dependencies
- `CodeGen/LoweringPlan` 单独处理 runtime/host binding
- `LinkedWorld.Dependencies` 仅限外部 managed dependencies，不含 runtime roles 或 host bindings

## Emitter 模板边界（20260407-08）

**决策**：采用 `LoweringFamily` 作为 Emitter 的正式输入，`TemplateKind` 作为内部概念。

- Emitter 仅限于 plan-completeness 和 backend-invariant 验证
- Emitter 不得做 lowering discovery、shape inference 或 semantic re-detection

## Emitter 边界实现步骤（20260407-09）

**决策**：两阶段迁移：

1. 从 Emitter 提取 lowering-planning 逻辑为独立 planner
2. 使 `CodeGenStage` 成为 lowering plan 的唯一生产者，Emitter 转换为纯消费者

执行 7 步完成迁移。

## 引擎 API 与主机绑定边界（20260407-10）

**决策**：将 engine binding 提升为 Phase A mainline 内的一等合约边界。

4 层显式区分：managed external dependency、runtime/helper/icall、engine binding、host/platform binding。engine API 需求必须在 `LoweringPlan` 中声明，不在 `LinkedWorld.Dependencies` 中。

## 引擎绑定 LoweringPlan 合约（20260407-11）

**决策**：在 lowering plan 中引入 `EngineBindings` 和 `HostBindings` 结构化层。

```
Plan Skeleton: CommonHeader → SubjectRoles → RuntimeRoles 
               → EngineBindings → HostBindings 
               → LiteralBindings → OutputContract → FamilyExtension
```

## 关键约束

- Emitter 仅消费 LoweringPlan，不产生任何 lowering 决策
- Engine bindings 不属于 Linker 的输出范围
- Dependency 只记录 managed external 引用
