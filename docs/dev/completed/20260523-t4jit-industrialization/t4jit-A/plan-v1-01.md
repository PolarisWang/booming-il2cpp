# t4jit-A — GC 精确槽映射 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 完成 T4 JIT codegen 的 GC 精确槽映射，使 NativeMethod 的 gc_slot_map 字段正确设置、slot_map 在 NativeMethod 析构时正确注销、移除 TODO(Phase 3d)。

**架构审核模式：** critical

**结构告警重点：** 确认 slot_map 生命周期一致性、验证 slotmap 注册/注销配对、确认 liveness 分析已在各调用点启用

---

## 分析结论

经过代码审计，**GC slot map 的基础设施已基本完整**：

- `GcSlotMapV0` 数据的序列化已在 `Generate()` 中完成（`slot_map_entries_` → `slot_map_data`）
- `nm->slot_map_data` 已在 `entry_direct.cpp` 通过 `GcRegisterSlotMap` 注册到 GC 注册表
- GC 扫描器（`thread_state.cpp:734`）通过 `FindT4CodeByAddress` → `nm->slot_map_data` 读取 slot map
- `enable_liveness = true` 已在全部四条 T4 晋升路径中设置（`entry_direct.cpp:647`、`entry_direct.cpp:806`、`fast_dispatch.cpp:2169`、`ir_reg_alloc.cpp:2276`）

**剩余待修复问题：**

| # | 问题 | 严重程度 | 
|---|------|---------|
| 1 | `nm->gc_slot_map` 始终为 nullptr（`Generate()` 未设置该字段） | 低 — 扫描器不依赖此字段 |
| 2 | `~NativeMethod()` 未调用 `GcUnregisterSlotMap()`，析构后注册表存在 dangling pointer | 中 — 潜在 use-after-free |
| 3 | `native_method.h:48` `TODO(Phase 3d)` 未移除 | 低 — 文档债务 |

---

## 改动清单

### 1. code_generator.cpp — Generate() 设置 gc_slot_map

在 `Generate()` 的 slot map 序列化段（L3616）之后追加：

```cpp
nm->gc_slot_map = reinterpret_cast<GcSlotMapV0*>(nm->slot_map_data);
```

### 2. code_generator.cpp — ~NativeMethod() 调用 GcUnregisterSlotMap

在 `~NativeMethod()` 中，`CHAOS_IL2CPP_FREE(slot_map_data)` 之前增加：

```cpp
if (code != nullptr) {
    GcUnregisterSlotMap(code);
}
```

需要添加 include：`#include <gc_root_scanner.h>`（或者对应的头文件）。

### 3. native_method.h — 移除 TODO(Phase 3d)

将 `GcPoint` 结构体中的注释从：
```
// TODO(Phase 3d): precise slot map; for Phase 3b, use conservative scanning.
```
改为：
```
// Slot map entries for precise root scanning (GcSlotMapV0).
```

### 4. 验证

- 构建通过：`cmake --build build/testing --target chaos_codegen`
- 现有测试通过：`ctest --test-dir build/testing -R test_codegen_native --output-on-failure`
- 确认 nm->gc_slot_map 不再为 nullptr（通过测试或断言）

---

## 验证方式

```bash
# 构建
cmake --build build/testing --target chaos_codegen

# 运行已有 codegen 测试
ctest --test-dir build/testing -R test_codegen_native --output-on-failure
ctest --test-dir build/testing -R test_t4_seh_handler --output-on-failure
```

---

## 结构告警

- 无重复代码风险 — 改动集中在单文件
- `gc_slot_map` 和 `slot_map_data` 两个字段共存的设计原因是：`slot_map_data` 是 `void*` 序列化 blob（用于释放），`gc_slot_map` 是 `GcSlotMapV0*` 类型化指针（用于消费者）。两者指向同一内存，无冗余问题
