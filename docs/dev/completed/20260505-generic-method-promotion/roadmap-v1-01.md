# 泛型运行时方法提升到 AOT — Roadmap

> **面向执行 Agent：** 子任务使用 `dev-writing-plans` → `dev-executing-plans` 或 `dev-subagent-driven-development` 执行。完成时子任务目录物理移入 `docs/dev/completed/`。

**目标：** 将泛型方法（method + type generics）的封闭实例化提升到 AOT 编译，运行时通过 per-module AOT map 快速分发。

**架构：** Codegen 端新增 Detector + Emitter 识别泛型实例化并发射 `method_aot_entries` 表；C++ 端新增 `GenericMethodAotEntryV0` 结构体和 `QueryAotMethod` 二分查找；runtime_instantiation.cpp 形成 fast-path (AOT) + slow-path (interpreter) 的分发对。

**技术栈：** C# codegen emitter, Scriban 模板, C++17 (codegen_bridge.h, runtime_instantiation.cpp)

**架构审核模式：** critical（触及 AOT / IL2CPP / codegen / contracts 四条主线）

**结构告警重点：** codegen 与 Scriban 分层（检测逻辑放在 C# emitter，渲染逻辑放在 Scriban）；codegen_bridge.h 不混入运行时逻辑

**权责图审核主题：** codegen Detector 职责边界（哪些泛型实例化由 detector 决定 AOT 编译）；Scriban 只做渲染不做语义决策

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: generic-runtime
- capabilityItem: generic-method-aot-promotion
- ownerSubjectId: IL2CPP-runtime-core
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: Smoke
- formalVerificationObjects: [QueryAotMethod unit test, codegen emission contract test, full-path integration test]
- requiredGates: collector → registry → workspace

**设计文档：** `docs/dev/in-progress/20260505-generic-method-promotion/STATUS.md`（设计摘要）

**问题清零来源：** brainstorm-approved STATUS.md

**计划来源：** direct-design-to-roadmap

**预期知识沉淀：** `wiki/03-功能模块/06-il2cpp核心架构/02-翻译路径参考/` 下新增泛型 AOT 分发翻译表

---

## 目标

将 Interpreter → AOT bridge 中目前全部走解释器的泛型方法分发，扩展为"优先查 AOT map → hit 走 AOT native 代码 → miss 回退解释器"的混合分发路径。

## 范围边界

- 方法泛型（method generics）和类型泛型（type generics）的封闭实例化
- Codegen 端识别并发射 AOT entries
- 运行时端 QueryAotMethod 二分查找
- Fallback 到 ResolveOrInstantiateMethod 解释器路径

## 非目标

- 不做泛型共享（shared generics）
- 不做 JIT 优化
- 不做条件分支后的尾调用优化
- 不修改现有的 MethodInvoke 和 FindInvokerPointer 基础设施

## 阶段列表

### Phase 1: Contract + Runtime（基础层）

| 字段 | 内容 |
|------|------|
| goal | 定义 AOT map 的 C++ 数据结构，实现 QueryAotMethod 查询函数 |
| exit_criteria | `GenericMethodAotEntryV0` 定义就绪；`ModuleGenericRegistrationV0` 新增字段；`QueryAotMethod` 二分查找实现通过单元测试 |
| deliverables | codegen_bridge.h 变更；runtime_instantiation.cpp 新增 QueryAotMethod |
| dependencies | 无 |
| resolved_decisions | 方案 A × A 数据结构和查找策略已确认 |
| watch_items | per-module entries 排序约束 |

### Phase 2: Codegen Emission（发射层）

| 字段 | 内容 |
|------|------|
| goal | Codegen Detector 识别需 AOT 编译的泛型实例化；Emitter 写入 method_aot_entries；Scriban 渲染 |
| exit_criteria | C# Detector + Emitter 逻辑实现；Scriban 模板渲染 method_aot_entries 数组；codegen 输出验证通过 |
| deliverables | C# emitter 变更；Scriban 模板变更 |
| dependencies | Phase 1 完成（依赖 GenericMethodAotEntryV0 定义） |
| resolved_decisions | 独立 method_aot_entries 表，不复用 generic_methods |
| watch_items | Detector 完备性（R1）；排序约束（R3） |

### Phase 3: Integration + Full Path Test（集成层）

| 字段 | 内容 |
|------|------|
| goal | 端到端验证：codegen emit → runtime load → QueryAotMethod → AOT dispatch |
| exit_criteria | 全路径集成测试通过；hotupdate 场景测试通过；interpreter fallback 测试通过 |
| deliverables | 集成测试代码 |
| dependencies | Phase 1 + Phase 2 完成 |
| resolved_decisions | — |
| watch_items | 跨 module token 解析 |

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|------------|----------|-------------|--------------|---------------|----------------|-----------------|
| contract-runtime | 1 | planned | terminal-1 | GenericMethodAotEntryV0 结构体 + ModuleGenericRegistrationV0 扩展 + QueryAotMethod 实现 | 无 | batch-1 | managed proof + native proof + hotupdate smoke | codegen_bridge.h, runtime_instantiation.cpp | 单元测试通过，struct 定义就绪 | contracts/native/v0/, runtime-core/ | 中 |
| codegen-emission | 2 | planned | terminal-1 | Detector + Emitter + Scriban 模板 | contract-runtime | batch-1 | managed proof + codegen contract test | C# emitter, Scriban 模板 | codegen 输出验证通过 | src/codegen/, src/templates/ | 中 |
| integration-test | 3 | planned | terminal-1 | 全路径端到端集成测试 | codegen-emission | batch-1 | managed proof + hotupdate smoke | 集成测试文件 | 全路径测试通过 | tests/ | 小 |

## 依赖

```
contract-runtime → codegen-emission → integration-test
```

纯串行依赖。

## 风险

（同 STATUS.md 风险评估摘要，不再重复）

## 备选路径

如 Detector 完备性长期不足，可补充运行时记录（recording mode）：首次解释执行时记录泛型实例化，后续自动触发重新编译。当前不做，作为未来优化项。

## 当前建议推进顺序

串行：contract-runtime → codegen-emission → integration-test

## 子任务执行策略

```yaml
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: contract-runtime
```
