---
task_id: 32byte-abi-fix
task_type: task
phase: implementation
lifecycle: in-progress
created: 2026-06-07
blocking_questions:
  - Layer 3 Extern 声明 emission crash — BRIDGE-AOT 注入符号命名不一致（UInt32 vs System.UInt32）
question_clearance: partial
clearance_confirmed_by_user: false
---

# 32-byte Value Type ABI Fix

## 状态
| Layer | 状态 | 说明 |
|-------|------|------|
| Layer 1 ABI 分类 | ✅ 完成 | Vector<T> >16byte → ByRefToValueType |
| Layer 2 Opcode 归一化 | ✅ 完成 | ldarg.N/ldc.i4.N/br.s 已处理 |
| Layer 3 Extern 声明 | 🔴 阻塞 | BRIDGE-AOT 注入符号命名不一致 → 需统一 DummySigProvider |
| Layer 4 Runtime stubs | ⏳ 待定 | Layer 3 之后 |

## 活着吗？
**是，但受阻** — Layer 1+2 已改但被 linter 恢复，Layer 3 阻塞在 DummySigProvider 命名一致性。需统一修复后重新提交。
