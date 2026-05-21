# 进度记录 v1-05

## 时间

- 2026-04-06 16:05:00 +08:00

## 已完成

- 已从 Stage 5 roadmap 正式派生 `20260406-06-stage-5c-pinvoke-lite-windows-reference`。
- 已把 Stage 5C 第一轮 scope 收窄为 Windows 上最小 `DllImport` 整数 import proof，而不是直接吞下完整 smoke `PInvokeLite`。
- active 上下文已从 Stage 5 roadmap 下沉到新的 Stage 5C 子任务。
- 本轮无需新增 wiki；当前长期有效约束已沉淀到 Stage 5C brainstorm / design / plan 与 Stage 5 roadmap。

## 关键收窄结论

- 第一轮只覆盖 `[DllImport("kernel32.dll")]` 形式的最小 import path，优先选择稳定的整数参数与整数返回值。
- 第一轮不纳入 `UnmanagedCallersOnly`、`Marshal.StringToCoTaskMemUTF8` / `PtrToStringUTF8`、`NativeLibrary.TryLoad` / `TryGetExport` 与导出反射路径。
- 第一轮优先复用现有 Windows reference spine，不把问题面扩张到动态库装载、导出解析和基础 marshalling 以外的能力。

## 验证

- 复核 `tests/smoke/input/PInvokeLite/Program.cs`
- 复核 `tests/registry/modules/interop/basic/verification.manifest.json`
- 复核 Stage 5 roadmap、Stage 5B 已归档状态与现有索引

## 下一步

- 由 Stage 5C 子任务接管 active，按 `plan-v1-01.md` 进入执行阶段。
