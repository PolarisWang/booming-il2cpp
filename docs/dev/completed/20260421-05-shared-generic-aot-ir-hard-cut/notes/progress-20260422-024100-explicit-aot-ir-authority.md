# 2026-04-22 02:41:00 +08:00 进展记录

## 本轮完成

- 为 `AotCoreIrMethodArtifact` / `AotCoreIrReferenceArtifact` 新增顶层 authority 字段。
- `AotCoreIrLowering` 现在把 loader demand graph authority 显式写入这些字段。
- generic layout proof 与 loader authority contract 回归已验证通过。

## 尚未完成

- planner/emitter 仍然没有把这些顶层 authority 变成真实执行 authority。
- 下一刀将落在 invocation planning / invocation ABI / method emission。
