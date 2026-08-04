# Reflection 工业化补齐

> **task_id**: reflection-industrialization
> **创建日期**: 2026-05-23
> **phase**: roadmap
> **task_type**: roadmap
> **roadmap_or_plan**: roadmap-v1-01.md
> **基于报告**: 前序 Reflection 工业化现状综合报告

## 背景

Reflection 子系统当前处于 Phase 2 中期，核心查询路径可用，但存在大量 stub/占位符返回错误值，以及测试覆盖、工具链、热更新兼容性方面的缺口。目标是达到"完美级工业化标准"——即对标 CoreCLR 反射能力 + 完整的测试 proof 系统 + EventPipe 诊断集成 + 热更新兼容性验证 + 性能基准。

## 设计摘要

### 1. 边界拍板

| 维度 | 结论 |
|------|------|
| 目标水平 | CoreCLR 反射能力 + 完整 proof 测试 + 诊断集成 + 热更新兼容 |
| 平台范围 | Windows x64 优先，不阻塞扩展 |
| 交付节奏 | 4 阶段分批交付 |
| 三优先级 | 性能(1) > 架构(2) > 热更(3) 继续适用 |

### 2. 完美级工业化 5 条标准

1. **API 表面全覆盖** — `remaining_stubs.cpp` 中所有返回 0 的函数实现真实逻辑
2. **测试 Proof 完整** — 每个子能力有独立的 `[CapabilityTest]` + patch data
3. **EventPipe 诊断事件** — 反射关键路径（GetType, GetMethod, Invoke）有埋点
4. **热更新兼容** — 热更新加载新类型后，反射查询可见
5. **性能基线** — 主要查询路径有 benchmark，回归可检测

### 3. 核心架构决策

| 决策项 | 方案 | 理由 |
|--------|------|------|
| BindingFlags 过滤 | 方案 A — Native 侧过滤，codegen descriptor flags + C++ 实现 | 性能最优，P1 驱动 |
| Event API | 方案 A — 扩展 ImageDescriptor，新增 ReflectionQueryEventDescriptor | API 完备性要求 |
| CustomAttribute 多 scope | 方案 A — 统一扩展 blob scope 字段，单入口查询 | 架构一致，P2 驱动 |
| MakeArrayType 等 | 走 `bridge->resolve_or_instantiate_type` 复用泛型引擎 | 复用既有架构 |
| Assembly version | 从 managed metadata 写入 ImageDescriptor 新增 version 字段 | AOT 数据固化，查询零开销 |
| 热更新兼容 | 热更新注册时同步更新 ModuleRegistry type index | 最小改动，统一句柄空间 |
| 测试体系 | 每个子能力独立 proof 套件 | 维持项目惯例 |

### 4. authority 决策

- Field/Method/Property MetadataDescriptor 新增 flags 字段：codegen (`NativeAotLoweringPlanner`) + Scriban 模板
- EventDescriptor 新增：`reflection_query_model.h` 中定义，Scriban 模板生成，C++ 反射 API 消费
- CustomAttribute scope 扩展：ModuleRegistry 的 `custom_attribute_blob` 格式不变，新增 scope_kind 参数
- 热更新反射兼容：`hot_update.cpp` 中热更新模块注册完成后同步刷新 ModuleRegistry
- 测试：`testing/foundation-dll/System.Private.CoreLib/` 下新建 `reflection-*` 套件

### 5. 风险评估摘要

| 风险 | 等级 | 缓解措施 |
|------|------|---------|
| codegen descriptor flags 修改影响全量 project | 中 | 新 flags 字段默认 0，向后兼容 |
| CustomAttribute blob scope 扩展复杂 | 中 | Phase 2 再处理，已有 Type 级可用 |
| 热更新反射兼容涉及 hot_update + runtime_core 两端 | 中 | Phase 3 单独做，有测试闸门 |
| 工作量可能被低估（4 阶段 ~ 6 周）| 低 | 每阶段有 exit criteria 可以调整 |

### 6. 三优先级权衡结论

三优先级（性能 > 架构 > 热更）贯穿全部决策：
- BindingFlags 过滤选择 Native 侧实现在 codegen 级别加 flags（P1 驱动，避免 managed 侧二次分配）
- Event/CustomAttribute 扩展选择统一架构方案（P2 驱动，避免后续重构）
- 热更新兼容放在 Phase 3（P3 驱动，但依赖 Phase 1-2 的元数据扩展完成）
- 所有方案均未出现 P3 强冲突场景

## blocking_questions

- [x] Q1: "完美级工业化"5 条标准定义 — 确认
- [x] Q2: BindingFlags 过滤方案 — Native 侧方案 A
- [x] Q3: Event API 扩展 — 完整 ImageDescriptor 方案 A
- [x] Q4: CustomAttribute scope 扩展 — 统一 blob scope 方案 A
- [x] Q5: MakeArrayType 等 — bridge->resolve_or_instantiate_type 方案 A
- [x] Q6: Assembly version 数据来源 — ImageDescriptor 新增字段
- [x] Q7: 热更新反射兼容 — ModuleRegistry 同步更新
- [x] Q8: 测试 proof 级别 — 每子能力独立

**blocking_questions: []**
**question_clearance: cleared**
**clearance_confirmed_by_user: true**

## 关键文档

- 前序 Reflection 工业化现状综合报告（本会话内产出）
- `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/17-反射查询模型.md`
- `roadmap-v1-01.md`

## 调度状态

- `task_type`: roadmap
- `roadmap_or_plan`: roadmap-v1-01.md
- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy`: blocking-only
- `dispatch_model`: sequential
- `latest_stop_point`: ALL 17 sub-tasks completed. Archive. (2026-05-23)
- `下一步`: 归档 completed
