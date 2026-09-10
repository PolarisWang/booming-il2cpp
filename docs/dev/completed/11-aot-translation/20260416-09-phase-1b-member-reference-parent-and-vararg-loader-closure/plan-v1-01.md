# Phase 1B Member Reference Parent And Vararg Loader Closure 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。
> 目标：完成 `MemberReference.Parent = MethodDefinition` 与 `arglist` 的第二批 loader widening，并用真实 vararg fixture 固定证据。

Status: completed

## 计划清单

- [x] 创建真实 `__arglist` / vararg compatibility 测试，先得到明确 failing surface
- [x] 为 `MemberReference.Parent = MethodDefinition` 增加 declaring-type / definition-signature 解析
- [x] 在 `LoaderStage` 中补齐 `arglist` decode
- [x] 确认 `typed-il-ir.json` 保留 `arglist` 与 vararg helper 的 call-site / definition 双重身份
- [x] 确认 `aot-core-ir.json` 保留同样的指令身份
- [x] 运行相邻 loader compatibility 回归并归档结果

## 涉及文件

- `tests/unit/compatibility/test_phase1_loader_vararg_decode.py`
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
- `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/STATUS.md`
- `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/roadmap-v1-01.md`

## 验证命令

- `python -m pytest tests/unit/compatibility/test_phase1_loader_vararg_decode.py -q`
- `python -m pytest tests/unit/compatibility/test_phase1_loader_typed_reference_decode.py tests/unit/compatibility/test_phase1_loader_vararg_decode.py tests/unit/compatibility/test_phase7_loader_function_pointer_decode.py tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py tests/unit/compatibility/test_loader_switch_opcode.py -q`

## 状态维护要求

- 本 child task 作为 `Phase 1` 的第二批 widening 归档到 `docs/dev/completed/`
- 父 roadmap 在本批完成后将 `Phase 1` 整体切换为 completed，并把 `next_action` 指向 `20260416-04`
