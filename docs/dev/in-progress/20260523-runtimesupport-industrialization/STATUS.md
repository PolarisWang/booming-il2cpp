# RuntimeSupport 全面工业化

> **task_id**: 20260523-runtimesupport-industrialization
> **创建日期**: 2026-05-23
> **更新日期**: 2026-05-23
> **task_type**: roadmap
> **phase**: roadmap
> **lifecycle_status**: in-progress
> **child_execution_mode**: auto
> **auto_continue**: true
> **auto_stop_policy**: blocking-only
> **dispatch_model**: sequential
> **blocking_questions**: []
> **question_clearance**: cleared
> **clearance_confirmed_by_user**: true
> **clearance_source**: brainstorm-conversation
> **latest_stop_point**: T0-2 完成 — collection_stubs 34 测试全部通过
> **recommended_next_child**: t0-3-enum-stubs-test

## Scope

覆盖 `src/native/runtime-core/runtime_stubs/`、`src/native/support/`、`src/native/runtime-core/com_*.cpp`、`src/native/runtime-core/layout_engine`、`generic_context` 等基础设施的全工业化。

## 边界拍板

| 项目 | 决策 |
|------|------|
| Collection 改造 | unordered_dense 复用（CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY） |
| String Unicode | ASCII-only + 文档约束 |
| 空操作 Stub 处理 | 混合策略：ConsoleWriteLine/Delegate 真实实现，≤⭐⭐ 全部升至 ⭐⭐⭐⭐ |
| COM Interop | 完整工业化（审计+测试+文档） |
| 基础设施 | 完整工业化（ANALYSIS.md→修复→测试→归档） |
| 三优先级 | 无冲突，全部维护 P1>P2>P3 |

## 设计摘要

详见 brainstorm 对话。关键结论：
- Phase 0：测试安全网先行，与 Type System / VTable Phase 0 策略一致
- Phase 1a：所有 ≤⭐⭐ stub 完整实现至 ⭐⭐⭐⭐ 级
- Phase 1b：Stub 功能修复（Culture WARN、Collection 哈希等）
- Phase 2：COM Interop + support 工业化
- Phase 3：基础设施工业化
- Phase 4：硬化 + 文档 + 验证

## 风险评估摘要

| 风险 | 等级 | 缓解措施 |
|------|------|---------|
| unordered_dense rehash 与 GC pointer 稳定性 | 🟡 中 | key/value 为 INTPTR（整数级），rehash 移动安全 |
| Phase 0 测试先行但 stub 是空操作，baseline 需后续更新 | 🟢 低 | Phase 1a 完成后更新 expectation |
| layout_engine audit 可能发现架构级问题 | 🟡 中 | 预留 5 天缓冲 |
| 范围过大，执行周期 ~16.5 周 | 🟡 中 | 6 个 Phase 独立 exit criteria，每个 Phase 完成可独立归档 |

## 关键文档

- [roadmap-v1-01.md](roadmap-v1-01.md)

## 最近摘要

- **2026-05-23**: brainstorm 完成，所有边界拍板和问题清零。
- **2026-05-23**: roadmap 编制完成，6 个 Phase，33+ 子任务。
- **2026-05-23**: T0-1 完成 — string_stubs 19 API 共 77 测试全部通过，同时修复了 JoinSs 的 tagged-pointer copy bug。
- **2026-05-23**: T0-2 完成 — collection_stubs 14 API 共 34 测试全部通过，修复 collection_stubs.h extern "C" 缺失。

## 下一步

启动 Phase 0 下一个子任务 T0-2（collection_stubs 单元测试）。
