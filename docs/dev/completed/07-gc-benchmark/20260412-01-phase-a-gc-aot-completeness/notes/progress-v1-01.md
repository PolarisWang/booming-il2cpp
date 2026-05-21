# Progress v1-01: Batch 1 完成

**时间**: 2026-04-12  
**Batch**: 1 — BDWGC 构建集成

## 产出

- `third_party/bdwgc/CMakeLists.txt` — 新增，构建 chaos_bdwgc 静态库
- `CMakeLists.txt` — 修改，windows-x64-reference 块加 add_subdirectory(third_party/bdwgc)
- `src/native/runtime-core/CMakeLists.txt` — 修改，链接 chaos_bdwgc
- `tests/contracts/native/gc/CMakeLists.txt` — 新增，gc smoke test 构建规则
- `tests/contracts/native/gc/gc_init_smoke.cpp` — 新增

## 遇到的问题

1. `heapsections.c` 是内联文件，不能单独编译，从源列表移除
2. `GC_BUILTIN_ATOMIC` 在 MSVC 下使用 GCC `__ATOMIC_*` 宏，不兼容。修复：MSVC 下改用 libatomic_ops 路径 (`libatomic_ops/src/atomic_ops.c`)

## 验证结果

- `chaos_bdwgc.lib` 构建成功
- `chaos_runtime_core.lib` 构建成功（链接 chaos_bdwgc）
- `gc_init_smoke.exe` 输出 `GC init OK`，退出码 0 ✓

## 下一步

Batch 2 — 替换分配器 + GcHandle 实现
