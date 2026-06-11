# 29 — SIMD Overload 重命名挂起记录

> 来源：`MEMORY.md > simd-overload-pending.md`

## 问题

`simd_stubs.h` 中 `extern "C"` 块内有 3 个 `ChaosSimd_V256_Mul_I32` 重载，MSVC `extern "C"` 不允许同名重载，触发 C2733 错误。

## root_cause

```cpp
ChaosSimdV256 ChaosSimd_V256_Mul_I32(ChaosSimdV256 a, ChaosSimdV256 b) noexcept;
ChaosSimdV256 ChaosSimd_V256_Mul_I32(ChaosSimdV256 a, CHAOS_IL2CPP_INT32 b) noexcept;  // C2733
ChaosSimdV256 ChaosSimd_V256_Mul_I32(CHAOS_IL2CPP_INT32 a, ChaosSimdV256 b) noexcept;  // C2733
```

## fix_strategy

将标量重载分别重命名为 `_VS` (vector-scalar) 和 `_SV` (scalar-vector) 后缀，避免 `extern "C"` 内的名称冲突。

## 挂起原因

修复已应用 3 次，但每次被 linter (task-archiver) 还原。需要先禁用 task-archiver 或使用 `git update-index --skip-worktree` 后再应用。

## 影响范围

- `collections-generic` (CoreLib)
- `runtime-intrinsics` (CoreLib)
- `System.Security.Cryptography/security-cryptography`

提交后需重建全部 3 个 chunk + 重建 Hephaestus 缓存。

## TRIGGER

C2733 编译错误、SIMD stub 修改、或涉及 simd_stubs.h/cpp 时。
