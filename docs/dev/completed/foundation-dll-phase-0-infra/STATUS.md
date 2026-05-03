---
task_id: foundation-dll-phase-0-infra
title: Phase 0 基础设施对齐与 CoreLib 摸底
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-02 01:00:00 +08:00
updated_at: 2026-05-02 03:30:00 +08:00
current_dir: docs/dev/completed/foundation-dll-phase-0-infra
parent_task_id: foundation-dll-full-development
parent_task_phase: phase-0
parent_task_relation: child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# Phase 0 基础设施对齐与 CoreLib 摸底

## 完成结果

- IL Scanner C# 工具已完成 + 构建验证：`src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/ILScanCommand.cs`
- Emitter coverage Python 分析器已完成：`build/toolchains/run/testing/foundation_dll/il_coverage_analyzer.py`
- Emitter known opcodes 常量已完成：`build/toolchains/run/testing/foundation_dll/emitter_coverage_known.py`
- CoreLib (net10) 扫描完成：42,371 methods, 40,082 with body, 197 unique opcodes
- Gap report 已生成：71.1% coverage, ~16 真实缺失 opcodes

## 核心发现

| 类别 | 数量 | 说明 |
|------|------|------|
| 假阳性（IR 归一化） | 16 | ldelem/stelem/ldind/stind 类型变体 |
| 无符号变体 | 3 | bgt.un/ble.un/blt.un |
| 溢出检查变体 | 22 | conv.ovf.* 系列 |
| **真实缺失** | **16** | **constrained/endfinally/rethrow 等高优先级** |

## 完成证据

- IL Scanner: `dotnet run -- il-scan --dll-path <path> --output <output>`
- Gap Analyzer: `python il_coverage_analyzer.py --il-scan <scan> --output <report>`
- Coverage report: `verification/reports/corelib-emitter-coverage-report.md`

## 交付物

| 文件 | 用途 |
|------|------|
| `src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/ILScanCommand.cs` | IL 扫描器（C#） |
| `build/toolchains/run/testing/foundation_dll/emitter_coverage_known.py` | Emitter 已知 opcodes |
| `build/toolchains/run/testing/foundation_dll/il_coverage_analyzer.py` | Gap 分析器（Python） |
| `verification/reports/corelib-il-scan.json` | CoreLib IL 扫描数据 |
| `verification/reports/corelib-gap-report.json` | CoreLib gap 报告 |
| `verification/reports/corelib-emitter-coverage-report.md` | CoreLib 摸底报告 |
