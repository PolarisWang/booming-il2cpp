# Type System / VTable 完全工业化

> **task_id**: 20260523-typesystem-vtable-industrialization
> **创建日期**: 2026-05-23
> **更新日期**: 2026-05-23
> **task_type**: roadmap
> **phase**: roadmap
> **lifecycle_status**: question_clearance = cleared, clearance_confirmed_by_user = true
> **child_execution_mode**: auto
> **auto_continue**: true
> **auto_stop_policy**: blocking-only
> **dispatch_model**: sequential
> **recommended_next_child**: T0-2

## 背景

基于 Type System / VTable 综合评估报告识别出的 11 项差距（P0×2, P1×2, P2×4, P3×3）和 2 项风险，全面补齐至工业化标准。

## 边界拍板

| 维度 | 决策 |
|------|------|
| 范围 | 全量覆盖（架构 + 功能 + 测试 + 冷路径） |
| 与 hotupdate/interpreter 关系 | 独立并行 roadmap，交叉引用依赖边界 |
| 与 hotupdate P3 关系 | 本路 Phase 1-2 完成后启动 hotupdate P3 |
| A4-Dual+V2 时序 | 先测试安全网，再执行重构 |
| 整体方案 | 方案 A：四阶段串行推进 |

## authority 决策

- 本任务独立为 roadmap，不并入 hotupdate/interpreter
- 测试基建创建独立测试目标（`vtable_registry_test`、`type_registry_test` 等）
- A4-Dual+V2 遵循 `19-A4-Dual+V2-ObjectHeader优化决策.md` 已批准的 3 子阶段方案
- vtable 统一不引入新抽象层，复用现有 `vtable_array[slot]` 路径

## 调度状态

| 项目 | 值 |
|------|-----|
| 调度模式 | sequential |
| 当前阶段 | Phase 0 |
| 当前子任务 | T0-2（即将启动） |
| 已完成 | T0-1 ✅ type_registry API 单元测试 (12 tests) |

## 风险评估摘要

| 风险 | 等级 | 缓解措施 |
|------|------|---------|
| Phase 0 测试在 A4-Dual+V2 后需调整 | 中 | 测试用例使用抽象访问器 |
| A4-Dual+V2 全量 re-gen 回归 | 高 | Phase 0 测试安全网 + 分步验证 |
| Gen-2 Handle 依赖 C1 延迟 | 中 | Phase 3 可降级为设计 + 部分实现 |

## 三优先级权衡结论

- 性能最优 (P1)：A4-Dual+V2 核心性能项；vtable 统一消除间接路径
- 方案完美 (P2)：双路径合并 + 冷路径补齐提升架构完整性
- HotUpdate (P3)：Gen-2 Handle + 跨 DLL vtable + 动态类型扩容

## 最新摘要

T0-1 (type_registry API 单元测试) ✅ 已完成并归档：
- 新增 `testing/src/native/runtime-core/type_registry_test.cpp`
- 12 个测试用例覆盖正常注册/查找/多类型/接口追加/幂等性/空值/容量溢出
- CHECK 配置下全部通过 (12/12)
- 修复关键问题：共享 domain 避免 registry dangling pointer（使用 SetUpTestSuite/TearDownTestSuite）

## 下一步

→ 启动 T0-2：vtable_registry 隔离测试（TCVC、IOC、继承链走行、幂等性、epoch 传播）

## 关键文档

- [brainstorm-v1-01.md](brainstorm-v1-01.md)
- [roadmap-v1-01.md](roadmap-v1-01.md)
