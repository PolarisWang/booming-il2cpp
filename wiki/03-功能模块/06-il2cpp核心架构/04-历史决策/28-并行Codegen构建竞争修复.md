# 28 — 并行 Codegen 构建竞争修复

> 来源：`MEMORY.md > parallel-codegen-build-race.md`

## 问题

并行 AOT+JIT cmake 构建在共享资源上发生竞争（MSBuild 锁竞争、linker 临时文件），间歇性产生损坏的 AOT 二进制文件（启动时 STATUS_ACCESS_VIOLATION 0xC0000005）。

## root_cause

`codegen.py` 通过 `ThreadPoolExecutor(max_workers=2)` 同时运行 AOT 和 JIT 的 `build_entry_executable`。两个构建使用不同目录 (build/ vs build_jit/)，但共享 native/ 中的源文件，且 MSBuild/linker 进程竞争系统资源。损坏的二进制文件被保存为 entry-aot.exe 备份，jit_codegen 随后恢复损坏的备份到 entry.exe。

## fix_strategy

改为串行执行：AOT 先，JIT 后。完全消除竞争。

## 性能影响

JIT 构建时间约 10s，现在串行执行增加墙钟时间，但为保证正确性可接受。

## TRIGGER

codegen.py 的构建步骤、AOT/JIT 二进制损坏、或需要并行构建优化时。

## 注意

如需恢复并行构建，必须使用完全隔离的源目录（如将源文件复制到各构建的临时目录）。
