# 进度记录 v1-01

## 时间

- 2026-04-06 16:05:00 +08:00

## 已完成

- 已从 Stage 5 roadmap 正式派生 `20260406-06-stage-5c-pinvoke-lite-windows-reference`。
- 已把 Stage 5C 第一轮 scope 收窄为 Windows 上最小 `DllImport` 整数 import proof，而不是直接吞下完整 smoke `PInvokeLite`。
- 已完成 Stage 5C 的 `brainstorm-v1-01.md`、`design-v1-01.md` 与 `plan-v1-01.md`。
- active 上下文已切到本子任务。

## 关键收窄结论

- 第一轮 proof 输入优先使用 `kernel32.dll` 上稳定的整数 import 结果，例如 `MulDiv(6, 7, 3) -> 14`。
- 第一轮只验证 import path，不纳入 `UnmanagedCallersOnly`、`NativeLibrary`、导出符号解析与 UTF-8 marshalling。
- 第一轮优先把改动留在 descriptor materialization 与 generated import glue，不为最小 proof 提前扩张 runtime/bootstrap。

## 验证

- 复核 `tests/smoke/input/PInvokeLite/Program.cs`
- 复核 `tests/registry/modules/interop/basic/verification.manifest.json`
- 复核 Stage 5 父 roadmap、Stage 5B 归档状态与 `docs/dev/ACTIVE.md` / 索引同步关系

## 下一步

- 进入执行阶段，先新增 `tests/proof/input/PInvokeLiteDllImportMinimal/` 与 Stage 5C RED 测试。
