# I-R9: Overflow-check 测试补齐 — 完成

> **task_id**: I-R9
> **parent_task_id**: interpreter-industrialization
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed
> **completed_date**: 2026-05-23

## 完成摘要

为 Interpreter 子系统补齐 Overflow-check 指令的 RegisterExecute 路径测试覆盖，并修复了 ConvOvfI / ConvOvfI8 在寄存器路径下的符号扩展 bug。

### 新增测试（12 个）

在 `interpreter_register_opcode_coverage_test.cpp` 中新增：

| 测试 | 覆盖 | 说明 |
|------|------|------|
| SubOvf | 正常减法 | 500000 - 200000 = 300000 |
| SubOvfOverflow | 溢出 | INT32_MIN - 1 应溢出 |
| MulOvf | 正常乘法 | 100000 * 2000 = 200000000 |
| MulOvfOverflow | 溢出 | 1000000 * 100000 应溢出 |
| ConvOvfI | 正常 | int32(-42) → native int(-42) — 符号扩展验证 |
| ConvOvfI4 | 正常 | int32(42) → int32(42) |
| ConvOvfI8 | 正常 | int32(-42) → int64(-42) — 符号扩展验证 |
| ConvOvfU | 正常 | int32(42) → native uint(42) |
| ConvOvfUNegative | 溢出 | int32(-1) → native uint 应溢出 |
| ConvOvfU4 | 正常 | int32(42) → uint32(42) |
| ConvOvfU4Overflow | 溢出 | int32(-1) → uint32 应溢出 |
| ConvOvfU8 | 正常 | int32(42) → uint64(42) |
| ConvOvfU8Overflow | 溢出 | int32(-1) → uint64 应溢出 |

### Bug 修复

**ir_reg_alloc.cpp: Reg_ConvOvfI / Reg_ConvOvfI8 — 符号扩展错误**

- **症状**：`conv.ovf.i` 和 `conv.ovf.i8` 对负 int32 输入（如 -42）返回 4294967254（零扩展的 uint32 位模式），而非正确的符号扩展到 int64。
- **根因**：`static_cast<uint64_t>(static_cast<uint32_t>(v))` 先通过 uint32 截断，再零扩展到 uint64，丢失了 int32 的符号位。应当 `static_cast<uint64_t>(static_cast<int64_t>(v))` 先通过 int64 符号扩展。
- **修复**：`int32_t v = static_cast<int32_t>(static_cast<uint32_t>(frame.regs.reg(src1)))` 先取回正确的 int32 值，再 `static_cast<uint64_t>(static_cast<int64_t>(v))` 符号扩展到 native int。

### 验证结果

| 测试套件 | 测试数 | 结果 |
|---------|--------|------|
| RegisterVM_OpcodeCoverage | 36（含 12 新增） | 全部 PASS |
| Interpreter_NewFeatures | 9 | 全部 PASS |
| InterpreterEntry | 5 | 全部 PASS |
| InterpreterBridge | 3 | 全部 PASS |
| InterpreterSmoke | 1 | 全部 PASS |

### Exit Criteria

| 标准 | 状态 |
|------|------|
| AddOvf RegisterExecute 测试存在 | ✅ 已有 |
| SubOvf RegisterExecute 测试（正常 + 溢出） | ✅ 新增 |
| MulOvf RegisterExecute 测试（正常 + 溢出） | ✅ 新增 |
| ConvOvfI/I4/I8 RegisterExecute 测试（正常 + 溢出） | ✅ 新增 |
| ConvOvfU/U4/U8 RegisterExecute 测试（正常 + 溢出） | ✅ 新增 |
| 所有测试通过 | ✅ |
| 无回归（既有测试全部 PASS） | ✅ |
