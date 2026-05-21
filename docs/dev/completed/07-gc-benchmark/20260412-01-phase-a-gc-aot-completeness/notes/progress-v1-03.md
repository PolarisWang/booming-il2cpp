# Progress v1-03: Batch 3 完成

**时间**: 2026-04-12
**Batch**: 3 — 线程 GC 注册 + Finalizer + Safepoint

## 产出

- `runtime_core.cpp`:
  - `ThreadAttach`: 非 Windows 平台调用 `GC_register_my_thread`
  - `ThreadDetach`: 非 Windows 平台调用 `GC_unregister_my_thread`
  - `EnqueueFinalizer`: 额外注册 `GC_register_finalizer_no_order`
  - `DrainFinalizerQueue`: 额外调用 `GC_invoke_finalizers()`
  - `GcSafepoint`: 调用 `GC_collect_a_little()` 触发增量 GC
- `tests/contracts/native/gc/gc_thread_smoke.cpp` 新增
- `tests/contracts/native/gc/gc_finalizer_smoke.cpp` 新增
- `tests/contracts/native/gc/gc_safepoint_smoke.cpp` 新增

## 遇到的问题

**`GC_register_my_thread` 报错: "Threads explicit registering is not previously enabled"**

原因: Windows `GC_WIN32_THREADS` 模式下，BDWGC 通过 `DllMain`/Win32 hooks 自动注册线程，
不需要也不允许手动 `GC_register_my_thread`。该 API 仅用于 pthread 平台。

修复: 加 `#if !defined(_WIN32) && !defined(_WIN64)` 条件编译

## 验证结果

- `gc_init_smoke`: GC init OK ✓
- `gc_handle_smoke`: gc_handle_smoke OK ✓
- `gc_thread_smoke`: gc_thread_smoke OK ✓ (2 worker threads × 500 allocs)
- `gc_finalizer_smoke`: invoked 5 finalizers ✓
- `gc_safepoint_smoke`: GC ran 1 times ✓

## 下一步

Batch 4 — vtable 虚方法分派
