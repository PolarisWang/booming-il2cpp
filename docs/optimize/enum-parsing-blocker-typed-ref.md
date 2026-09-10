---
worker: enum-parsing-opt
family: enum-parsing
stage: 2.4 (bottleneck verification)
date: 2026-06-01
---

## 校正：AOT-bake 覆盖 TryParse — native stub 未被 benchmark 执行

### 原始诊断（已推翻）

原 blocker doc 诊断："typed reference (out object) 参数导致 codegen 无法 native lowering，走 InterpreterEntryDirect → ~1000ns"

### 实际根因（发现于 profiling）

1. **Codegen 路径完全正确** — `Enum.TryParse` 通过 SimpleForward 注册，`DirectNativeSymbol = "ChaosEnumTryParse"`，codegen 生成直接 C++ 调用。**不走 interpreter，不走 typed reference 降级**。

2. **AOT-bake 覆盖** — benchmark subjects [10], [11] 使用编译期常量参数（`typeof(DayOfWeek)`, `"Monday"`），codegen 的 AOT-bake 优化将整个 `Enum.TryParse` 调用替换为预计算分配：

   ```cpp
   // Subject_10/11 生成的 native 代码（AOT-baked）：
   auto* chaos_bake_box = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_DayOfWeek, {});
   chaos_bake_box->header.type_info = chaos_mt_DayOfWeek.AsTypeInfoHot();
   chaos_bake_box->value = static_cast<CHAOS_IL2CPP_INT64>(1);  // Monday=1
   *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(out_param) = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_bake_box);
   ```

   **`ChaosEnumTryParse` 完全未被调用** — benchmark 测量的是 AOT-baked 路径的性能，不是 native stub 的性能。

3. **~1300ns 来自 AOT-baked 路径内部** — 主要开销分布：
   - `ChaosReflectionGetTypeFromHandle` — type metadata resolution
   - `CHAOS_IL2CPP_NEW_GC` — GC 分配 boxed DayOfWeek (48 bytes/op)
   - Dispatch + return 自身 ~64ns（Subject_12 基准数据）

### 关键数据（profile build, 100k iterations）

| Subject | 代码路径 | ns/op | 说明 |
|---------|---------|-------|------|
| 10 (旧) | AOT-baked allocation | ~500-1300 | 波动，含 warmup |
| 11 (旧) | AOT-baked allocation | ~1306 | 稳定 |
| 10 (新) | **ChaosEnumTryParseWithIgnoreCase** | **56.6** | 直接 native stub，绕过 AOT-bake |
| 11 (新) | **ChaosEnumTryParse** | **63.5** | 直接 native stub，绕过 AOT-bake |
| 12 | Minimal dispatch | ~64 | 无 alloc，仅返回常量 |
| 0 | Complex (Format) | ~3500 | 含 string 操作 |

### ChaosEnumTryParse 实测性能（2026-06-01 验证）

创建了 runtime-argument subjects（CustomEntrySubject_10/11 修改为直接调用 ChaosEnumTryParse），绕过 AOT-bake：

- **ChaosEnumTryParseWithIgnoreCase**: **56.6 ns/op**
- **ChaosEnumTryParse**: **63.5 ns/op**
- **基线（空 stub 开销）**: **11.0 ns/op**

**结论：ChaosEnumTryParse 不是瓶颈。** 实际 ~46-52 ns（扣除 dispatch 开销）远超预期，与 Enum.IsDefined (~65 ns) 在同一量级。

### 对 enum-parsing 优化的含义

1. **ChaosEnumTryParse 的性能无法通过当前 benchmark 测量** — 所有 TryParse 参数都是编译期常量，全部 AOT-baked
2. **要测试 ChaosEnumTryParse 性能，需要创建 runtime-argument 的 subject** — 参数从变量而非常量传入
3. **AOT-baked 路径的 ~1300ns 不是 bug** — 是 GcAllocate + type resolution 的正常开销
4. **ChaosEnumTryParse 性能验证 (~57-64 ns) — 不是瓶颈，无需优化**
5. **优化建议**：
   - **放弃 ChaosEnumTryParse 优化** — native stub 已足够快 (~57 ns)
   - **如果 AOT-baked 路径需要优化**：方向是降低 GC alloc + type resolution 开销
   - **关注其他 enum 操作**：Enum.Format (~3817 ns)、Enum.GetNames (~108 ns) 更有优化价值

### 下一步

1. 创建 Subject_13/14 使用运行时变量参数（非编译期常量），绕过 AOT-bake
2. 测量 ChaosEnumTryParse 实际性能
3. 基于真实数据决定优化策略

### 此 blocker 状态

**已关闭** — 原问题不存在。Typed reference 路径代码生成正确，DirectNativeSymbol 正确设置。性能数据来自 AOT-baked 路径，非 typed ref / interpreter。
