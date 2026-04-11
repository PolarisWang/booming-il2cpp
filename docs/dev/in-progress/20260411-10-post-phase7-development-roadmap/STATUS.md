---
task_id: 20260411-10-post-phase7-development-roadmap
title: Post Phase-7 Development Roadmap (Phase A-E)
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-11 21:30:00 +08:00
updated_at: 2026-04-12 00:04:39 +08:00
current_dir: docs/dev/in-progress/20260411-10-post-phase7-development-roadmap
parent_task_id: "20260409-10-total-solution-and-ios-hot-update-analysis"
source_task_id: "20260409-10-total-solution-and-ios-hot-update-analysis"
source_relation: successor_roadmap
active: true
---

## 关键文档

- parent_roadmap: docs/dev/completed/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md
- phase_architecture: docs/dev/completed/20260409-10-total-solution-and-ios-hot-update-analysis/phase-architecture-v1-01.md
- roadmap: docs/dev/in-progress/20260411-10-post-phase7-development-roadmap/roadmap-v1-01.md

## 当前判断

- current_focus: 父 roadmap `20260409-10` 已归档完成；本 roadmap 现为新的 active 主线，准备从 Phase A 开始推进。
- why_now: Phase 8 已收口，上一轮 proof-first total-solution baseline 已完成，需要把后续 GC / Benchmark / HotUpdate / Mobile / CI 路线从设计切到执行。
- done_definition: Phase A-E 全部子任务完成，三方 Benchmark 报告在各平台可运行

## 5 Phase 概览

| Phase | 目标 | 核心产出 | 依赖 |
| --- | --- | --- | --- |
| A | GC 集成 + AOT 运行时关键补完 | BDWGC 接入、vtable、完整 EH/泛型/委托、IL opcode 扩展 | Phase 7 完成 |
| B | 三方 Benchmark 报告体系 | C# vs AOT vs HotUpdate 对比报告 + HTML 可视化 | A (中期可启动) |
| C | 热更端到端 | Package -> Interpreter -> AOT interop 闭环 | A |
| D | 多平台真机验证 | Android/iOS 真机 + 跨平台 Benchmark | A + C |
| E | 生产化 + CI/CD | GitHub Actions + 版本兼容 + Stack Trace + 文档 | All |

## 关键决策

- GC 方案: BDWGC 分阶段策略 (BDWGC -> Arena 补充 -> 数据驱动精确 GC 迁移)
- Benchmark: 三方对比 (C# .NET / AOT Generated C++ / HotUpdate Interpreter)
- 性能目标: AOT >= 2x faster than C#, HotUpdate <= 50x slower than C#

## 最近摘要

- 2026-04-12 00:04:39 +08:00: 父 roadmap `20260409-10` 已按用户指令跳过最后两个 blocker 并归档完成；本 successor roadmap 正式成为当前 active 入口。
- 2026-04-11 21:30:00 +08:00: 完成 Phase A-E 全量实现方案设计，包含 5 Phase / 35 sub-task / 文件级实现步骤

## 下一步

- next_action: 派生 `Phase A` child plan，固定第一批实现范围: BDWGC 接入、runtime-core GC / vtable / EH / generic / delegate 关键缺口与验收路径。
- trigger: 父 roadmap 已 completed，进入执行入口

## 风险

### risks

- AOT codegen 从 proof 到 production 的 IL opcode 覆盖工作量可能超预期 (A.11)
- BDWGC 保守式扫描的假性存活可能在移动端造成内存问题
- Interpreter 当前仅 int32，扩展到 float/object/array 工作量大 (C.1)
- Mobile 真机环境不可控 (D.1, D.2)

### blockers

- 当前无 immediate blocker；Phase D 的 Android/iOS 真机环境约束保留到对应子任务处理。
