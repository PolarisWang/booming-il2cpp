# Phase 1 Loader And Typed-IL Input Closure 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。
> 目标：完成 `TypedReference + sizeof` 的第一批 loader widening，并以真实 C# compatibility 固定证据。

Status: completed

## 计划清单

- [x] 创建真实 C# fixture compatibility 测试，先得到 `Mkrefany` / `Sizeof` 的明确失败
- [x] 在 `LoaderStage` 中补齐 `mkrefany` / `refanytype` / `refanyval` / `sizeof` decode
- [x] 确认 `typed-il-ir.json` 保留正确的 opcode / operand / resultType / reference
- [x] 确认 `aot-core-ir.json` 保留相同的指令身份
- [x] 运行相邻 loader compatibility 回归并归档结果

## 涉及文件

- `tests/unit/compatibility/test_phase1_loader_typed_reference_decode.py`
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
- `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/STATUS.md`
- `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/roadmap-v1-01.md`

## 验证命令

- `python -m pytest tests/unit/compatibility/test_phase1_loader_typed_reference_decode.py tests/unit/compatibility/test_phase7_loader_function_pointer_decode.py tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py tests/unit/compatibility/test_loader_switch_opcode.py -q`

## 状态维护要求

- 本 child task 作为 `Phase 1` 的第一批 widening 归档到 `docs/dev/completed/`
- 父 roadmap 继续保持 `in_progress`
- 父 roadmap 的 `next_action` 继续冻结到下一个真实 failing surface，而不是直接跳到 `Phase 2`
