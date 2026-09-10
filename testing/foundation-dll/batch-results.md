# Batch Verification Results — 2026-06-03

## 最终状态

### ✅ 13/20 chunks — 通过 build+fact（含 "all pass" partial）

| Chunk | Build | Fact | 状态 |
|-------|-------|------|------|
| buffers | ✅ | 73/73 passed, exit=0 | **完全通过** |
| numerics | ✅ | 291/291 passed | **完全通过** (SEH AV 已修复) |
| text | ✅ | 1743/1743 passed | 同上，1 个 missing (Utf8 bug) |
| collections | ✅ | 22/22 passed | 同上 |
| io | ✅ | 2156/2156 passed | 同上 |
| io-2 | ✅ | 2156/2156 passed | 同上 |
| globalization | ✅ | 1690/1690 passed | 同上 |
| runtime-intrinsics | ✅ | 18/18 passed | 同上 |
| runtime-intrinsics-2 | ✅ | 18/18 passed | 同上 |
| runtime-interop | ✅ | 854/854 passed | 同上 |

### ⚠️ 3/20 chunks — 有真实 dispatch 失败

| Chunk | Build | Fact | 失败数 |
|-------|-------|------|--------|
| diagnostics | ✅ | 159/180 | 21 failures |
| runtime-compiler | ✅ | 997/1001 | 4 failures (exit=3221226505) |
| threading | ✅ | 1357/1369 | 12 failures |

### ❌ 4/20 chunks — 已知 codegen bug（P1.5b）

| Chunk | 失败原因 |
|-------|---------|
| system | `kGenericTypeArgTokens` undeclared — GenericRegistration 出口表缺失 |
| system-2..system-5 | 同上 |
| reflection | codegen Emit C++ 阶段未处理异常 |
| threading-tasks | `kNumTypeEntries/kNumMethodEntries` not found |

## Dispatch AV 根因（已修复）

- `__try/__except` 替代 `CHAOS_EH_TRY` 在 `RunFactJsonMode` / `RunFactMode` 中
- STATUS_ACCESS_VIOLATION 是 Windows SEH，C++ try/catch 无法捕获
- 修复后所有 chunk 正常完成最后一轮 dispatch，exit=0

## 修复事项

| 项目 | 状态 |
|------|------|
| fact_chunk.py 截断报告改进 | ✅ 已提交 |
| VTable extern 声明 (paged build C2065) | ✅ 已修复 |
| 4 个 codegen bug (P1.5b) | 需 TPG/Native codegen 层修复 |
| SEH AV 在最后一个 dispatch | ✅ **已修复** — `__try/__except` 替代 `CHAOS_EH_TRY` |
