---
task_id: 20260521-crt-codegen-foundation
task_type: roadmap
phase: execution
roadmap_or_plan: docs/dev/in-progress/20260521-crt-codegen-foundation/roadmap-v1-01.md
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
created_at: 2026-05-21
updated_at: 2026-05-21
---

## 设计摘要

三个工作流：CRT 链接错误修复 → Codegen 基础测试 → Foundation-DLL 扩展。
串行推进，从 crt-fix 开始。

## 最近摘要

Phase A (crt-fix) 完成。根因：chaos 预编译库用 Debug 配置构建但测试项目链接 release CRT。修复：统一用 RelWithDebInfo 配置重建所有 chaos libs。验证：test_interpreter_smoke 及全部 66/73 测试通过（7 个失败为预存问题，与本 phase 无关）。同时修复 engine-bridge 测试的 include path 缺失。

## 当前状态

- 子任务 crt-fix: completed
- 子任务 codegen-tests: completed
- 子任务 foundation-expand: planned

## 下一步

Phase B-1 完成。新增 test_codegen_il_smoke (16/16 PASS) + test_codegen_abi (31/31 PASS)。
codegen 模块基础覆盖率已补充。

Phase B-2: 选取下一个 foundation-DLL family 并通过验证管线。

## 调度状态

active_child: codegen-tests (completed)
latest_stop_point: codegen-tests 通过验证（16 + 31 测试全部 PASS）
recommended_next_child: foundation-expand
completed_children: [crt-fix, codegen-tests]
