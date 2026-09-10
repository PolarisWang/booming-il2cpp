# `vector_t_boxing` 修复方案 Brainstorm

## 问题描述

`capabilities.json` 中 `vector_t_boxing = false`，原因是 Vector\<T\> 返回类型在 AOT
编译时产生 LNK2001（未解析的 `chaos_mt_Vector_1_System_Int32_` 符号）。

## 根因分析

### Vector\<T\> 的 AOT 编译链路

```
IL: call Vector<int>.Zero
  → codegen IR: stloc Vector<int> (value type, 32 bytes on x64)
  → 如果返回 object: box Vector<int>
    → 需要 chaos_boxed_type_Vector_1_System_Int32_ { PureTypeHeader + value }
    → 需要 chaos_mt_Vector_1_System_Int32_ (MethodTable for GC layout)
    → 需要 chaos_type_Vector_1_System_Int32_ (TypeInfo for type identity)
  → 如果 JIT fallback: external runtime dispatch
```

### 当前断点

1. **`hashSet3` 不包含 Vector\<T\>** — instruction scanner 只添加非泛型 value type
   到 boxed type 集合（`hashSet3`）。泛型 value type（含 `[[` 的 SubjectId）被跳过。

2. **Vector\<T\> size 动态** — sizeof(Vector\<T\>) 取决于元素类型和硬件平台
   （x64: Vector256=32B, Vector512=64B）。codegen 已有 `AotCoreIrLowering.Resolve.cs:245`
   计算 size 的逻辑，但 boxed type struct 定义需要编译期常量 size。

3. **MethodTable 符号缺失** — `chaos_mt_Vector_1_System_Int32_` 符号在链接时未定义。
   只有非泛型 value type 的 MethodTable 被注册。

4. **泛型实例化矩阵** — `Vector<T>` 需要为所有被引用的元素类型（int, float, double,
   long, short, byte, etc.）生成独立的 boxed type struct + MethodTable。

### 影响范围

| 元素类型 | sizeof | 使用场景 |
|---------|--------|---------|
| System.Int32 | 32 | Vector<int>, Vector<int>.Zero |
| System.Single | 32 | Vector<float>, matrix ops |
| System.Double | 64 | Vector<double> |
| System.Int64 | 64 | Vector<long> |
| System.UInt16 | 16 | Vector<ushort> |
| System.Byte | 16 | Vector<byte> |

## 修复方案

### 方案 A：完整 AOT 支持（推荐）

#### Phase 1: 泛型 value type 的 boxed type 注册

**文件**: `NativeAotLoweringPlanner.ObjectModelEmission.cs`

```csharp
// 在 instruction scanner 中（line ~315-325 区域）
// 当前逻辑只对非泛型 value type 添加 hashSet3
// 需要扩展以包含 Vector<T> 等泛型 value type

if (targetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType)
{
    valueTypeSubjectIds.Add(targetReference.SubjectId);
    // 新增: 泛型 value type 也需要 boxed type struct
    if (targetReference.SubjectId.Contains("System.Numerics.Vector`1<"))
    {
        hashSet3.Add(targetReference.SubjectId);
    }
}
```

#### Phase 2: Vector\<T\> 的 struct size 编译期确定

**文件**: `NativeAotLoweringPlanner.ObjectModelEmission.cs`

boxed type struct 需要 `sizeof(chaos_valuetype_Vector_1_T)` 在 C++ 编译期确定。
Vector\<T\> 在 x64 Windows 上的大小：
- `sizeof(Vector<byte>)` = 16 (Vector64)
- `sizeof(Vector<int>)` = 32 (Vector256) 
- `sizeof(Vector<double>)` = 64 (Vector512)

方案：在 C++ header 中用 `alignas(X) uint8_t data[X]` 表示 Vector\<T\>：

```cpp
// 生成的 header 中：
struct chaos_valuetype_System_Numerics_Vector_1_System_Int32_ {
    alignas(32) uint8_t __data[32] = {};
};
struct chaos_boxed_type_System_Numerics_Vector_1_System_Int32_ {
    PureTypeHeader header{};
    chaos_valuetype_System_Numerics_Vector_1_System_Int32_ value{};
};
```

这需要 codegen 的 `MapFieldTypeToCppType` 或 struct 发射器支持基于 size 的 opaque struct。

#### Phase 3: MethodTable + TypeInfo 注册

**文件**: `NativeAotLoweringPlanner.ModuleRegistration.cs`

Vector\<T\> 的 MethodTable 注册与普通 value type 相同，但需要：
1. `chaos_mt_Vector_1_System_Int32_` 符号的 extern 声明
2. 在 `EmitGcTypeLayoutRegistration` 中将 Vector\<T\> 的 MethodTable 注册为 GC-tracked type

#### Phase 4: 现有 external runtime fallback 保留

对于无法 AOT 编译的 Vector\<T\> 方法（如硬件 intrinsic），保留现有的
`ChaosExternalRuntimeFallback` 路由，作为降级保障。

### 方案 B：最小修复（只修复 boxing 不修复 intrinsic）

只解决 boxing 导致的 LNK2001，不要求所有 Vector\<T\> 方法都能 AOT 编译。
Vector\<T\> 方法仍走 external runtime fallback，但 boxing 操作本身不崩溃。

**优势**：改动小，快速解决 `vector_t_boxing = false`
**劣势**：Vector\<T\> 性能仍受限于 interpreter fallback

### 方案 C：完整 intrinsic 路由（含 SIMD JIT）

在方案 A 基础上，为 Vector\<T\> 的热点方法（`op_Addition`, `Dot`, `Zero`, `One` 等）
添加 native SIMD intrinsic 实现（通过 `_TryExecuteViaSimdStub`）。

**优势**：Vector\<T\> 性能最优
**劣势**：改动最大（涉及 SIMD stub 编写、所有平台的 intrinsic 映射）

## 推荐路线

**Phase 1 (方案 A)** → 解决 LNK2001 → 验证 `vector_t_boxing = true`
  → 预期工时: 2-3 天
  → 验证: System.Numerics.Vectors.Tests 的 + `vector_t_boxing` capability

**Phase 2 (方案 C, 可选)** → SIMD intrinsic stubs
  → 预期工时: 1-2 周
  → 验证: Vector\<T\> benchmark 性能对比

## 风险

| 风险 | 影响 | 缓解 |
|------|------|------|
| Vector\<T\> size 在不同硬件上不同 | sizeof 错 → 内存损坏 | 用 alignas + 最大 size |
| 泛型 value type 的 TypeInfo 层次 | is-a 关系错误 | 所有 Vector\<T\> 继承同一 Object TypeInfo |
| MethodTable 符号名包含 `<` `>` | C++ 非法符号 | SanitizeSubjectId 已处理 |
| GC layout 中 Vector\<T\> 不含 GC 引用 | 扫描器误判 | 注册为 pointer-free type |
