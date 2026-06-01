---
worker: enum-parsing-opt
family: enum-parsing
stage: 2.7 (.NET 8 comparison)
date: 2026-06-01
---

## Blocker: TryParse typed reference — interpreter 路径无法消除

### 症状

方法 [10] (TryParse(Type, String, Boolean, out Object)) 和 [11] (TryParse(Type, String, out Object)) 在 chaos-aot 中走 Interpret 路径：

| 方法 | chaos-aot | .NET 8 | 退化 |
|------|-----------|--------|------|
| [10] TryParse(Type, String, Bool, out) | 1017 ns | 45.3 ns | **22.4x** |
| [11] TryParse(Type, String, out) | 1198 ns | 43.9 ns | **27.3x** |

### 根因

这两个方法包含 **typed reference (`out object`) 参数**。当前 codegen 对 typed reference 参数无法生成 native lowering，所有带 `out`/`ref` 参数的方法自动降级为 AOT-unreachable stub，运行时走 InterpreterEntryDirect。

Interpreter 路径的完整 dispatch 开销（ValidateCall → SetupFrame → FastExecute 循环 → Cleanup → FreeFrame）导致 ~1000 ns/op，而 RegisterExecute 路径仅 ~3 ns/op。

### 影响范围

- **直接阻塞**: enum-parsing 方法 [10], [11]
- **间接影响**: 所有包含 typed reference (`out`/`ref`) 参数的 enum 方法
- **不影响的路径**: 方法 [1],[2],[7] 已进入 RegisterExecute，比 .NET 8 快 6~15x

### 修复方向（属于独立 Phase）

1. **codegen 层扩展**: 使 AOT Core IR lowering 支持 typed reference 参数，绕过 `is_typed_reference` 检查
2. **wrapper 桥接**: 在 managed 侧生成 wrapper 方法消除 typed reference，让 codegen 可以 native lowering
3. **直接 emit**: 对简单的 `out object` 场景，codegen 直接 emit store 指令跳过 typed ref 机制

### 当前状态

- RegisterExecute 路径（5/6 benchmark methods）：✅ 通过，性能优秀
- Interpreter 路径（2/6 benchmark methods）：❌ 阻塞，需独立 Phase 修复
- Exception-path methods（6/12 methods）：✅ 自动排除（始终抛异常）
