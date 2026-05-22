# I-R11: T4 Native Codegen Hardening — 完成

> **task_id**: I-R11
> **parent_task_id**: interpreter-industrialization
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed
> **completed_date**: 2026-05-23

## 完成摘要

对 T4 x64 native codegen (`code_generator.cpp` / `code_buffer.*`) 进行了 5 项加固，修复了 1 个栈缓冲区溢出 bug，提升了内存分配失败时的安全性，并增加了输入验证。

### 变更文件

- **`src/native/codegen/code_buffer.h`** — 添加 OOM 安全防护：
  - 新增 `bool failed_` 标记，内存分配失败后抑制后续写入
  - 所有 `EmitByte`/`Emit16`/`Emit32`/`Emit64`/`EmitBytes`/`Reserve` 入口检查 `failed_`，失败时立即返回无操作
  - 新增 `failed()` 公开查询方法

- **`src/native/codegen/code_buffer.cpp`** — 增强分配失败处理：
  - `Ensure()` 检查 `Grow()` 返回值，失败时设置 `failed_ = true`（原代码忽略返回值，导致后续写入越界）
  - `Seal()` 检查 `failed_` 标记，失败时返回 nullptr
  - 移动构造/赋值操作同步转移 `failed_` 状态

- **`src/native/codegen/code_generator.cpp`** — 主要加固项：
  - **修复栈缓冲区溢出**：`prologue_push_offsets_[6]` 和 `push_reg_nums_[6]` → `[8]`。Graph coloring 可分配最多 5 个 callee-saved 寄存器，加上 rbp/rbx/rsi 共需 8 个槽位，原数组 [6] 导致 WriteBeyond 栈
  - **新增 NativeCodeGenerator::CheckFailed()**：在 Prologue 后、指令循环后、冷段后、SEH/Branch resolve 后、OSR stub 后检查缓冲区状态，OOM 时提前返回 nullptr
  - **SEH clause 索引越界验证**：生成时跳过 malformed clause 并发出 LOG_WARN，同时修正 clause count 以反映实际成功写入数
  - **CanGenerateNativeCode 加固**：新增 SEH clause 索引范围验证，拒绝无效 clause 的 method
  - **跨关键代码段添加 CheckFailed() 守卫**：prologue/主循环/deopt_return/冷段/SEH 表/OSR stub/Seal 前

### Exit Criteria

| 标准 | 状态 |
|------|------|
| chaos_codegen.lib 编译通过（0 errors） | ✅ |
| prologue 数组溢出修复（[6]→[8]） | ✅ |
| CodeBuffer OOM 安全（Ensure 检查 Grow 返回值） | ✅ |
| SEH clause 索引越界验证 | ✅ |
| CheckFailed() 跨关键段守卫 | ✅ |
