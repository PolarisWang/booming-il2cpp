# Codegen 代码质量修复记录

> 记录 2026-05 codegen 风险评估报告的修复实施。

## 背景

对 codegen 生成的 native C++ 代码进行综合风险评估（性能、代码清晰度、IL2CPP 最终形态角度），发现多个风险点并逐一修复。

## 修复清单

### P0 — Critical Bugs

| 修复 | 文件 | 说明 |
|------|------|------|
| COM refcount 原子化 | `src/native/runtime-core/com_ccw.h` + `.cpp` | `refcount` 从 `CHAOS_IL2CPP_UINT32` 改为 `std::atomic<CHAOS_IL2CPP_UINT32>`。`AddRef` 使用 `memory_order_relaxed`，`Release` 使用 `memory_order_acq_rel`（确保释放前所有写操作可见）。 |
| TypeInfo type-id 缺失 AppendLine | `NativeAotLoweringPlanner.ObjectModelEmission.cs` | 引用类型 type-id 声明块的 `stringBuilder.AppendLine(ref handler)` 调用缺失，导致 type-id 常量从生成的 C++ 文件中消失。 |

### P1 — High Risk

| 修复 | 文件 | 说明 |
|------|------|------|
| ret_tag 运行时验证 | `src/native/interpreter/fast_dispatch.cpp` | `Handle_Call_DoAotDirect` 中加入 `CHAOS_IL2CPP_ASSERT`，在 CHECK 构建下验证 ret_tag 值范围。ret_tag 是 interpreter 层 CachedCallInfo 预计算的概念，C# codegen 不直接控制。 |
| 枚举元数据碰撞检测 | `EnumMetadataExtractor.cs` | FNV-1a 32-bit hash 碰撞检测（codegen 时 warning，碰撞项退回到 reflection API）。FNV-1a 24-bit hash 碰撞检测（codegen 时 emit `#error`，阻断编译）。32-bit 查询的 strcmp 验证从 `#ifndef CHAOS_IL2CPP_SHIP` 移出，所有构建都执行。 |

### P2 — Medium

| 修复 | 文件 | 说明 |
|------|------|------|
| Store-load DCE 跨 body 保护 | `NativeAotLoweringPlanner.StructuredIR.cs` | `EmitIRSwitch` 在调用 `FilterRedundantStoreReloadPairs` 时，收集所有 case body + default body 的 ldloc 槽位作为 `externallyReferencedLocals`，防止 stloc+ldloc 对中的 stloc 被错误消除。 |

### P3 — Low Risk / Code Quality

| 修复 | 文件 | 说明 |
|------|------|------|
| >8 参数回退 | `src/native/interpreter/fast_dispatch.cpp` | `Handle_Call` 中检查 `ac > 8` 时路由到 `Handle_Call_DoRaw` 而非 `Handle_Call_DoAotDirect`（AOT direct 使用统一 8-arg 签名）。 |
| CHAOS_IL2CPP_ARRAY 注释 | `src/native/common/chaos/native_types.h` | 添加简短注释说明用途。 |

### 不需要修复的项（评估误解）

- **EH 3x 膨胀**: `#if/#elif/#else` 预处理分支，编译器只编译活动路径。
- **FastDispatch.cpp 拆分**: 代码组织问题，非风险。
- **RuntimeHelperShapeRegistry 6581 行**: 架构重构，非当前风险。
- **Shape dispatch 分组**: if-constexpr 链已被编译器转换为高效跳转表；按返回类型分组已经足够。

## 验证

1. `cmake --build build/native --target chaos_runtime_core --config RelWithDebInfo` — 编译通过
2. `cmake --build build/native --target chaos_interpreter --config RelWithDebInfo` — 编译通过
3. `tests/e2e/translation` 全家桶验证（需重新生成 codegen DLL 后运行）

## 经验教训

1. **`std::atomic` 的 memory ordering 选择**: COM refcount 的 AddRef 不需要顺序一致性，relaxed 足够（只保证原子性）。Release 必须用 acq_rel 确保释放前所有写操作对其他线程可见。
2. **FNV hash 碰撞是真实风险**: 虽然概率低，但在 codegen 时检测碰撞并阻断构建（24-bit）或降级到 reflection fallback（32-bit）是必要的安全措施。
3. **跨 body DCE**: 局部优化（如 stloc+ldloc 消除）必须考虑控制流体中其他 body 的隐式依赖。
