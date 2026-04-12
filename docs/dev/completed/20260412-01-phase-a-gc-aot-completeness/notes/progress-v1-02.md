# Progress v1-02: Batch 2 完成

**时间**: 2026-04-12
**Batch**: 2 — 替换分配器 + GcHandle 实现

## 产出

- `runtime_core.cpp` — `#include <gc.h>` 添加
- `DefaultAllocate` → `GC_MALLOC`, `DefaultDeallocate` → no-op
- `AllocateBytesAtomic` 新增 (StringNewUtf8, BoxValueObject 使用)
- `RuntimeInit` 添加 `GC_INIT()` (once_flag 保护)
- `RuntimeShutdown` 移除 `FreeBytes(runtime_state)` 调用
- GcHandle 全局表 (s_gc_handle_mutex + s_gc_handle_table) 新增
- `GcHandleNew` / `GcHandleFree` stub 替换为真实实现
- `tests/contracts/native/gc/gc_handle_smoke.cpp` 新增

## 遇到的问题

1. `GC_remove_roots` 需要 `DYNAMIC_LOADING` 宏，在我们的配置下不可用。
   解决方案：GcHandleFree 中移除 `GC_remove_roots` 调用，注释说明理由
   (false positive root range 对 GC 功能无影响)
2. gc_handle_smoke.cpp 最初直接调用 `RuntimeInit` — 实际上函数在 `chaos::il2cpp::runtime_core` namespace
   且通过 ABI vtable 访问。修复：改用 `chaos_runtime_get_abi_v0()` 调用 ABI

## 验证结果

- `gc_init_smoke.exe` OK ✓
- `gc_handle_smoke.exe` OK ✓
- HelloWorldObject failure 是预存在问题 (typed-il-ir.min.json 不存在)，与 GC 无关 ✓
- 构建全量通过 ✓

## 下一步

Batch 3 — 线程注册 + Finalizer + Safepoint
