---
task_id: 20260505-generic-method-promotion
title: 泛型运行时方法提升到 AOT — Generic Method Promotion
task_type: roadmap
lifecycle_status: roadmap
phase: roadmap
created_at: 2026-05-05
updated_at: 2026-05-05

child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
clearance_source: brainstorming-approval

roadmap_or_plan: roadmap-v1-01.md

key_documents:
  - roadmap-v1-01.md
---

# 泛型运行时方法提升到 AOT — Generic Method Promotion

## 设计摘要

### 范围

- **范围**：泛型方法（method generics）的封闭实例化在编译期识别并 AOT 编译为 native 代码，运行时通过 per-module AOT map 查找直接调用 AOT 版本，miss 时 fallback 到解释器执行
- **范围也覆盖**：类型泛型（type generics）的方法实例化——当封闭泛型类型的方法被调用时，走同一套 AOT map
- **非目标**：不做泛型共享（shared generics，每个封闭实例化独立）；不做 JIT；不做条件分支后的尾调用优化

### 边界拍板

1. **加载策略**：延迟按需加载（lazy on-demand），仅在方法首次分发时解析 AOT map
2. **AOT Map 结构**：`(module_id, open_token, type_args[]) → closed_token`，per-module 二分查找 + type_args 线性扫描
3. **发射方案**：新增 `method_aot_entries` 独立表，不复用 `generic_methods` 字段
4. **Fallback 策略**：AOT map miss → `ResolveOrInstantiateMethod` 解释执行（安全降级）

### Authority 决策

| 决策项 | 决策 | 依据 |
|--------|------|------|
| AOT Map key 格式 | `(open_token, type_args[]) → closed_token` | 与 `GenericTypeRegistrationEntryV0` 一致的模式；P1 性能达标 + P2 架构最干净 |
| Codegen 发射方法 | 新增独立 `method_aot_entries` 表 | 职责分离，不与 `generic_methods` 语义混淆；P2 + P3 显著优于复用方案 |
| 文件组织 | `QueryAotMethod` 放入 `runtime_instantiation.cpp` | 逻辑简单（~30 行），紧邻 `ResolveOrInstantiateMethod` 形成 fast-path/slow-path 对 |

### 风险评估摘要

| 风险 | 等级 | 缓解策略 |
|------|:----:|---------|
| R1 Detector 不完备 | 🟡 中 | fallback 到解释器安全降级，后续增量完善 |
| R2 编译产物膨胀 | 🟡 中 | 编译期仅检测到的实例化才发射；不做泛型共享 |
| R3 Per-module 排序约束 | 🟢 低 | codegen 发射排序 + assert 验证 |
| R4 TypeArgs 跨 module token | 🟢 低 | 现有 `TypeInfoHandle` 编码已支持 |
| R5 AOT map ↔ GenericRegistry 互斥 | 🟢 低 | 先查 AOT map → miss 才调 ResolveOrInstantiateMethod |
| R6 Hotupdate 注册时序 | 🟢 低 | module 独立注册，生命周期管理已就绪 |
| R7 内存占用 | 🟢 极低 | ~400KB 总量级 |

### 三优先级权衡结论

| 优先级 | 方案 A × A 评价 |
|--------|----------------|
| P1 性能最优 | 🟢 Per-module 二分 O(log n + m)，~11 次整数比较，对总分发延迟占比 < 1% |
| P2 方案完美性 | 🟢 与 `GenericTypeRegistrationEntryV0` 完全一致，职责分离，无新抽象 |
| P3 HotUpdate 支持 | 🟢 每个 module 独立发射 entries，module 生命周期 + tombstone 继续适用 |

## 架构映射

```
Codegen (C#) Detector → Emitter
  │ method_aot_entries[]
  ▼
Scriban 模板 → s_method_aot_entries[] 数组
  ▼
ModuleGenericRegistrationV0::method_aot_entries 注册
  ▼
运行时: QueryAotMethod (per-module 二分查找)
  ├─ hit → closed_token → FindInvokerPointer → AOT MethodInvoke
  └─ miss → ResolveOrInstantiateMethod → 解释器执行
```

## 子任务映射

| task_id | phase | status | purpose | depends_on | estimated_effort |
|---------|-------|--------|---------|------------|-----------------|
| contract-runtime | 1 | **ready** | GenericMethodAotEntryV0 + ModuleGenericRegistrationV0 扩展 + QueryAotMethod | 无 | 中 |
| codegen-emission | 2 | planned | Detector + Emitter + Scriban 模板 | contract-runtime | 中 |
| integration-test | 3 | planned | 全路径端到端集成测试 | codegen-emission | 小 |

## 下一步

启动子任务 **contract-runtime**。串行推进。

## 最近摘要

2026-05-05：Roadmap 创建。方案 A × A 已获设计批准。3 个子任务串行推进。

## latest_stop_point

| task_id | phase | status |
|---------|-------|--------|
| contract-runtime | 1 | ready — 即将启动 |
| codegen-emission | 2 | planned |
| integration-test | 3 | planned |
