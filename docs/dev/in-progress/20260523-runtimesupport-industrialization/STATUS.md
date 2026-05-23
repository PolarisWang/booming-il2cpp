# RuntimeSupport 全面工业化

> **task_id**: 20260523-runtimesupport-industrialization
> **创建日期**: 2026-05-23
> **更新日期**: 2026-05-23 (t4-2 completed → t4-4 regression)
> **task_type**: roadmap
> **phase**: Phase4-t4-4
> **lifecycle_status**: in-progress
> **child_execution_mode**: auto
> **auto_continue**: true
> **auto_stop_policy**: blocking-only
> **dispatch_model**: sequential
> **blocking_questions**: []
> **question_clearance**: cleared
> **clearance_confirmed_by_user**: true
> **clearance_source**: brainstorm-conversation
> **latest_stop_point**: t4-2 (LOG_WARN) done — added LOG_WARN to exception_stubs.cpp, stream_stubs.cpp, char_stubs.cpp, object_stubs.cpp. Next: run regression to verify changes.
> **recommended_next_child**: t4-4-regression

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
- **2026-05-23**: T0-1 完成 — string_stubs 19 API 共 77 测试全部通过，修复 JoinSs tagged-pointer copy bug。
- **2026-05-23**: T0-2 完成 — collection_stubs 14 API 共 34 测试全部通过。
- **2026-05-23**: T0-3 完成 — enum_stubs 12 API 共 16 测试全部通过。
- **2026-05-23**: T0-4 完成 — datetime/guid/math 57 测试全部通过。
- **2026-05-23**: Phase 1a/b 完成 — convert/stream/varargs/exception/object/misc/collection 所有 stubs 工业化；ConsoleWriteLine/StackTrace/Delegate 生产级实现；Culture LOG_WARN 标识。
- **2026-05-23**: Phase 2 完成 — COM Interop ANALYSIS.md、测试、P0/P1 修复（malloc→DomainFreeTagged、LOG_WARN 入口标识）、wiki 文档。CCW 28/28 通过（含 aggregation），Connection Point 15/15 通过，RCW 11+6skip/17 通过。
- **2026-05-23**: T3-1 layout_engine 完成 — 33/33 测试通过；ScopedEnter 条件 Leave 修复；ResolveCcw address-distance 安全检查；VerifyModuleLayouts LOG_WARN stub。
- **2026-05-23**: T3-2 generic_context 完成 — 23/23 测试通过（含6个新增）；修复 P1 bug（DoLazyResolveOpenType moved-from type_args）；修复 P2 data race（owner_shard 写操作移入 shard_mutex 保护域）。
- **2026-05-23**: COM CCW 测试从 23/28 提升至 28/28 全通过（refcount start=1 防止 stack-allocated guard 在 Release 时被 free）；Connection Point 15/15 通过（ComCcwOld 兼容层）。
- **2026-05-23**: COM RCW 测试新增 QueryInterfaceCachedCacheFullNoAdd 等 6 个非 Win32 测试，17/17 全通过。修复 SEH crash（FindOrCreateRcw identity_unknown 非空保护）。
- **2026-05-23**: T3-3 aot_core_ir_reader 完成 — 22/22 测试通过；修复 JSON 非空终止字符串 bug（SEH kind 比较、catch type 解析、targetReference/targetSymbol 非 call opcode 路径、direct_ctx guard）。
- **2026-05-23**: T3-4 assembly_manager+domain_unloader 完成 — 17/17 测试通过；提供代码生成符号 extern "C" stubs，添加 UnloadDomain(0) guard 测试。

## 下一步

启动 T3-5 (convert+parse_convert audit+test)。
