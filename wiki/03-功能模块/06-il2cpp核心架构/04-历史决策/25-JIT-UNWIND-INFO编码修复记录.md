# 25 — JIT UNWIND_INFO 编码修复记录

> 来源：`MEMORY.md > jit-unwind-info-nibble-fix.md`

## root_cause

`EmitUnwindInfo()` 在 `jit_unwind.cpp` 中以 nibble 反转的顺序发射 UNWIND_CODE 第二字节 — UnwindOp 在高 4 位、OpInfo 在低 4 位。但 Win64 `_UNWIND_CODE` 结构期望 UnwindOp 在低 4 位 (bits 0-3)、OpInfo 在高 4 位 (bits 4-7)。

这导致 OS unwinder 将 PUSH_NONVOL 解码为 ALLOC_SMALL、SET_FPREG 解码为 PUSH_NONVOL(RCX)，在 JIT 代码崩溃后的 SEH unwind 过程中产生垃圾展开状态和 `STATUS_INVALID_CRUNTIME_PARAMETER (0xC00000FF)`。

## fix_strategy

- 将 6 处 `buf.EmitByte()` 调用从 `(UWOP << 4) | opInfo` 改为 `(opInfo << 4) | UWOP`
- 设置 `SizeOfProlog = 255` 以容纳 ALLOC_LARGE 的 raw size 字节在 slot 1，避免验证失败
- 移除调试期添加的 `#pragma optimize("", off)` 守卫和诊断 fprintf

## regression_check

`entry-jit.exe --benchmark-all 1` — 不再出现 0xC00000FF 崩溃。预存的 0xC0000005 (ACCESS_VIOLATION) 是另一问题（编译代码中的空指针解引用）。

## TRIGGER

JIT 编译后的 SEH unwind 崩溃、0xC00000FF 异常、或修改 jit_unwind.cpp 时。
