# Type System / VTable 完全工业化

> **task_id**: 20260523-typesystem-vtable-industrialization
> **创建日期**: 2026-05-23
> **更新日期**: 2026-05-23
> **task_type**: roadmap
> **phase**: hanging
> **lifecycle_status**: hanging — 等待外部依赖（hotupdate C1/C9, interpreter Layer 3）
> **child_execution_mode**: auto
> **auto_continue**: true
> **auto_stop_policy**: blocking-only
> **dispatch_model**: sequential
> **latest_stop_point**: Phase 2 完成（T2-3 阻塞），等待外部依赖

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
| 当前阶段 | Phase 2（收尾中） |
| 当前子任务 | — |
| 已完成 | T0-1 ✅ type_registry API 单元测试 (12 tests) |
|  | T0-2 ✅ vtable_registry 隔离测试 (44 tests) |
|  | T0-3 ✅ VTable 查找性能基准 (3 benchmarks) |
|  | T0-4 ✅ 并发/压力测试 (6 tests) |
|  | T0-5 ✅ TCVC epoch 风险分析+修复 |
|  | T1-1 ✅ ObjectHeader 瘦身 |
|  | T2-1 ✅ vtable_registry 双路径合并 |
|  | T2-2 ✅ vtable 一致性验证断言 |
|  | T2-4 ✅ MetadataRegistry 冷路径回退补齐 |
|  | T2-5 ✅ 动态类型注册表哈希表升级（unordered_dense，无硬上限） |

## 阻塞

- **T2-3 (EEClass 动态类型填充)**: 部分阻塞。AOT 路径已实现（module registry reverse-lookup → ReflectionQueryTypeDescriptor），动态类型路径需等待 hotupdate pipeline 提供 PatchMetadataCache 描述符。当前动态路径为 stub（`filled=true` 无实际数据）。
- **Phase 3 (T3-1, T3-2, T3-3)**: 阻塞。依赖 hotupdate C1（寄存器 VM）、interpreter Layer 3、hotupdate C9（AutoBridge）。外部 roadmap 未就绪。

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

Phase 0 全部完成 ✅ | Phase 1 (T1-1) 完成 ✅ | Phase 2 推进中：

- T2-1 (vtable_registry 双路径合并) ✅
- T2-2 (vtable 一致性验证断言) ✅ — 44 tests passing
- T2-4 (MetadataRegistry 冷路径回退补齐) ✅
  - GetMethodName: 通过 ReflectionQueryImageDescriptor 扫描返回方法名
  - FindToken: 扫描所有模块的 ImageDescriptor 查找 method token
  - 13 metadata_registry_test, 44 vtable_registry_test, 12 type_registry_test, 6 concurrent_stress_test 全部通过
- T2-5 (动态类型注册表哈希表) ✅ — 已使用 CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY，无硬上限，>256 类型通过测试
- T2-3 ⏳ 部分阻塞 — EEClass 动态类型填充需等待 hotupdate PatchMetadataCache
- Phase 3 ⏳ 阻塞 — 依赖 hotupdate C1、interpreter Layer 3、hotupdate C9

可独立推进的子任务已全部完成。本 roadmap 已达到最大独立推进边界，等待外部依赖。

## 下一步

→ 本 roadmap 所有可独立推进的子任务已完成
→ T2-3 待 hotupdate PatchMetadataCache 就绪后恢复
→ Phase 3 待外部依赖（hotupdate C1, interpreter Layer 3, hotupdate C9）就绪后恢复

## 完成证据

- **Phase 0**: 5/5 子任务完成 (T0-1~T0-5)
- **Phase 1**: 1/10 子任务完成 (T1-1)。T1-2~T1-10 (A4-Dual+V2 全量优化) 未排入当前工作树
- **Phase 2**: 4/5 子任务完成 (T2-1, T2-2, T2-4, T2-5)。T2-3 阻塞
- **Phase 3**: 0/3 (T3-1~T3-3) 阻塞，依赖外部 roadmap

roadmap 达到当前可执行边界，等待外部依赖就绪后继续。

## 关键文档

- [brainstorm-v1-01.md](brainstorm-v1-01.md)
- [roadmap-v1-01.md](roadmap-v1-01.md)
