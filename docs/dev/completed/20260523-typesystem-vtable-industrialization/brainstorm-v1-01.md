# Type System / VTable 完全工业化 — Brainstorm

> **task_id**: 20260523-typesystem-vtable-industrialization
> **创建日期**: 2026-05-23
> **phase**: brainstorming

## 范围边界

### 覆盖差距清单

| 编号 | 类别 | 差距项 | 当前状态 |
|------|------|--------|---------|
| #1 | P0-架构 | A4-Dual+V2 ObjectHeader 优化（56B→~9.6B） | 已决策未排期 |
| #2 | P0-架构 | vtable_registry 与 vtable_array 双路径合并 | 未规划 |
| #3 | P1-功能 | 运行时 MakeGenericType（Gen-2 Handle）完整实现 | 决策完成、实现未完成 |
| #4 | P1-功能 | 跨程序集调用走运行时桩（非 vtable_array） | 未规划 |
| #5 | P2-测试 | type_registry API 零测试覆盖 | 0 测试 |
| #6 | P2-测试 | 缺少 vtable_registry 专用隔离测试 | 无隔离测试目标 |
| #7 | P2-测试 | 缺少 VTable 查找性能基准测试 | 无 |
| #8 | P2-测试 | 缺少并发/压力测试 | 无 |
| #9 | P3-冷路径 | EEClass 动态类型填充 stub | 仅设 filled=true |
| #10 | P3-冷路径 | MetadataRegistry 冷路径回退 stub | GetMethodName→nullptr |
| #11 | P3-冷路径 | 动态类型注册表 256 上限 | 平数组 |
| #12 | 风险 | TCVC 全局 epoch 缓存行震荡 | 未分析 |
| #13 | 风险 | EEClass stub 影响反射 API | 未修复 |

### 非目标

- 不覆盖 iOS 合规签名体系
- 不覆盖 Unity 编辑器集成
- 不覆盖完整 AppDomain 级别代码隔离
- 不覆盖解释器 Layer 4 调试器（归 interpreter-industrialization）
- 不覆盖 HotUpdate C4 工具链/C10 静态变量/C11 COM（归 hotupdate-industrialization）

## authority 决策

| 决策项 | 方案 | 理由 |
|--------|------|------|
| 本 roadmap 定位 | 独立并行 roadmap，与 hotupdate/interpreter roadmap 有 cross-reference | 范围独立于热更新和解释器 |
| 与 hotupdate P3 关系 | 本路 Phase 1-2 完成后启动 hotupdate P3 | C5 测试覆盖可复用本路 Phase 0 产出 |
| A4-Dual+V2 时序 | 等测试安全网就绪后执行 | 避免无安全网的大重构 |

## 三优先级权衡

| 优先级 | 在本 roadmap 中的体现 |
|--------|---------------------|
| 性能最优 (P1) | A4-Dual+V2 是核心性能项；vtable 统一消除间接跳转 |
| 方案完美 (P2) | 双路径合并、冷路径补齐提升架构完整性 |
| HotUpdate (P3) | Gen-2 Handle、跨 DLL vtable、动态类型扩容直接服务热更新 |

**裁决**：A4-Dual+V2 虽然对架构有破坏性（全量 re-gen），但性能收益巨大，符合 P1 优先。冷路径和测试属于 P2/P3 范畴，但不与 P1 冲突，可并行推进。

## 风险评估摘要

| 风险 | 等级 | 触发条件 | 缓解措施 |
|------|------|---------|---------|
| Phase 0 测试在 A4-Dual+V2 后需调整 | 中 | A4-Dual+V2 改变对象头布局和 TypeInfo 结构 | 测试用例使用抽象访问器而非硬编码偏移 |
| A4-Dual+V2 全量 re-gen 回归 | 高 | codegen 输出与运行时预期不匹配 | Phase 0 测试安全网 + 分步验证 |
| Gen-2 Handle 依赖 C1 延迟 | 中 | hotupdate C1 寄存器 VM 延期 | Phase 3 依赖外部 roadmap，可降级为"设计 + 部分实现" |

## 问题清零

blocking_questions:
- [x] 范围边界：全量覆盖
- [x] 时序策略：先测试后重构（混合同步）
- [x] 与现有路线图的关系：先本路再 hotupdate P3
- [x] 整体方案选型：方案 A（四阶段串行）
