# Phase 2 AotCoreIr Semantic Surface Expansion 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。
> 目标：冻结 `ldtoken` 在 `AotCoreIr` 的强类型 `targetReference` contract，并完成局部 compatibility 回归。

Status: in-progress

## 计划清单

- [x] 创建真实 `ldtoken` compatibility 测试，先得到明确 failing surface
- [x] 覆盖 type / field / method 三种 token 形态
- [x] 最小修改 `AotCoreIrLowering.ResolveTargetReference(...)`
- [x] 确认 `aot-core-ir.json` 中三类 `ldtoken` 均带有正确的 `targetReference`
- [x] 运行相邻 `AotCoreIr` compatibility 回归
- [x] 更新父 roadmap / 子任务状态与进展记录

## 涉及文件

- `tests/unit/compatibility/test_phase2_aot_core_ir_ldtoken_target_reference.py`
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- `docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/STATUS.md`
- `docs/dev/in-progress/20260416-04-phase-2-aot-core-ir-semantic-surface-expansion/STATUS.md`

## 验证命令

- `python -m pytest tests/unit/compatibility/test_phase2_aot_core_ir_ldtoken_target_reference.py -q`
- `python -m pytest tests/unit/compatibility/test_phase2_aot_core_ir_ldtoken_target_reference.py tests/unit/compatibility/test_phase1_loader_typed_reference_decode.py tests/unit/compatibility/test_phase1_loader_vararg_decode.py tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/compatibility/test_loader_switch_opcode.py -q`

## 状态维护要求

- 子任务执行期间 `docs/dev/ACTIVE.md` 指向本目录
- 父 roadmap 在 `20260416-04` 激活后要把 `Phase 2` 标记为进行中
- 本批已完成；下一批候选先审计更宽 scalar ABI carrier（真实 `ulong` surface）
