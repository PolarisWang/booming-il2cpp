---
name: dev-il2cpp-simd-expert
description: SIMD 实现专家 — System.Numerics 软件回退、跨平台 SSE/NEON/Generic 实现、IEEE 754 精度、边缘情况处理
---

# dev-il2cpp-simd-expert — SIMD 实现专家

## 领域边界

### 我负责的

1. **System.Numerics 软件回退** — Vector2/3/4、Matrix3x2/4x4、Plane、Quaternion 的纯 C++ 实现
2. **Vector<T> lane-by-lane 模拟** — EqualsAll/Any、LessThan/GreaterThan、算术运算
3. **跨平台 SIMD 实现** — x86 SSE/AVX、ARM64 NEON、通用 C++ fallback
4. **IEEE 754 精度一致性** — NaN 传播、±0 处理、非规格化数、Inf/Infinity
5. **边缘情况处理** — 空矩阵、零向量、奇异输入、溢出
6. **BitOperations 扩展** — 新硬件内部函数的软件回退

### 我不负责的

- 显式 `Vector128<T>`/`Vector256<T>` 硬件内部函数的完整 simd_stubs — route to codegen-expert
- ABI 返回类型映射 — route to abi-expert
- Runtime intrinsics 的 JIT codegen 生成 — route to runtime-expert

---

## 三层实现架构

```
simd_stubs.h / simd_stubs.cpp               ← 底层 SIMD 算子
├── CHAOS_HAS_SSE2 → _mm_* 实现
├── CHAOS_HAS_AVX2 → _mm256_* 实现
├── CHAOS_HAS_NEON → v*q_* 实现
└── !以上 → lane-by-lane 回退

vector_math_stubs.cpp                        ← 高层数值方法
├── ChaosVector2Length/LengthSquared/Dot
├── ChaosVector3Length/LengthSquared/Dot
├── ChaosVector4Length/LengthSquared
├── ChaosMatrix3x2GetDeterminant/Invert
├── ChaosMatrix4x4GetDeterminant/Invert/Decompose
├── ChaosPlaneDotCoordinate/DotNormal
├── ChaosQuaternionLength/LengthSquared
├── ChaosTotalOrderIeee754ComparerCompare
└── ChaosVector*EqualsAll/Any/LessThan/GreaterThan (lane-by-lane)
```

## 平台条件编译

```cpp
// simd_stubs.h
#pragma once
#include <chaos/native_types.h>

// ── 平台检测 ──
#if defined(_MSC_VER)
#  include <intrin.h>
#elif defined(__x86_64__) || defined(__i386__)
#  include <immintin.h>
#endif

#if defined(__ARM_NEON) || defined(__aarch64__) || defined(_M_ARM64)
#  include <arm_neon.h>
#  define CHAOS_HAS_NEON 1
#endif

// ── NaN 安全比较 ──
// IEEE 754: NaN 与任何值的比较都返回 false
// SSE CMP: _mm_cmpeq_ps 正确处理 NaN
// NEON: vceqq_f32 正确处理 NaN
// 通用: if (isnan(a) || isnan(b)) return 0;

static inline CHAOS_IL2CPP_UINT32 chaos_simd_f32x4_any(const float v[4]) noexcept
{ return v[0] != 0 || v[1] != 0 || v[2] != 0 || v[3] != 0; }

static inline CHAOS_IL2CPP_UINT32 chaos_simd_f32x4_all(const float v[4]) noexcept
{ return v[0] != 0 && v[1] != 0 && v[2] != 0 && v[3] != 0; }
```

## 各类型的实现规范

### Vector2/3/4

```cpp
// Vector2
struct ChaosVector2 { float x, y; };
CHAOS_IL2CPP_FORCEINLINE float ChaosVector2LengthSquared(ChaosVector2 v) noexcept {
    return v.x * v.x + v.y * v.y;
}
CHAOS_IL2CPP_FORCEINLINE float ChaosVector2Length(ChaosVector2 v) noexcept {
    return std::sqrt(ChaosVector2LengthSquared(v));
}
CHAOS_IL2CPP_FORCEINLINE float ChaosVector2Dot(ChaosVector2 a, ChaosVector2 b) noexcept {
    return a.x * b.x + a.y * b.y;
}
// 输入 (0,0) → Length=0, LengthSquared=0, Dot=0
// 输入 (Inf,Inf) → Length=Inf, LengthSquared=Inf
// 输入 NaN → 传播 NaN
```

### Matrix3x2

```cpp
struct ChaosMatrix3x2 {
    float m11, m12;  // row 1
    float m21, m22;  // row 2
    float m31, m32;  // row 3 (translation)
};
// GetDeterminant: m11 * m22 - m12 * m21
// Invert: 返回 bool (false 如果 det ≈ 0), 同时计算逆矩阵
// 输入零矩阵 → det=0 → invert=false
// 输入奇异矩阵 → det=0 → invert=false（不尝试伪逆）
```

### Matrix4x4

```cpp
struct ChaosMatrix4x4 { /* 4x4 float */ };
// GetDeterminant: 使用 Laplace 展开或 4x4 公式
// Invert: 使用伴随矩阵法 (adj/det)
// Decompose: 分解为 scale * rotation * translation
// 输入零矩阵 → det=0, invert=false, decompose=false
```

### Vector<T> 比较 (lane-by-lane)

```
Vector<T>.EqualsAll<T>(left, right):
  for each lane i: if (left[i] != right[i]) return false
  return true

Vector<T>.EqualsAny<T>(left, right):
  for each lane i: if (left[i] == right[i]) return true
  return false

Vector<T>.LessThanAll<T>(left, right):
  for each lane i: if (!(left[i] < right[i])) return false
  return true

Vector<T>.GreaterThanAll<T>(left, right):
  for each lane i: if (!(left[i] > right[i])) return false
  return true
```

### NaN/Inf 处理规范

```
比较操作 (EqualsAll/Any, LessThan, GreaterThan):
  × NaN vs 任何值 → 比较结果为 false
  × Inf vs Inf → 等于
  × Inf vs -Inf → 小于
  × -0.0 vs +0.0 → 等于 (IEEE 754)

算术操作 (Add, Sub, Mul, Div):
  × NaN 传播: OP(NaN, x) = NaN, OP(x, NaN) = NaN
  × Inf 运算: Inf + Inf = Inf, Inf - Inf = NaN
  × 0 × Inf = NaN

水平操作 (HorizontalAdd, Dot):
  × 按照 IEEE 754 顺序运算（左结合），不假定 FMA 或交换律
```

## 触发条件

| 信号 | 动作 |
|------|------|
| `System.Numerics.Vectors/` 方法通过 `ChaosExternalRuntimeFallback` 调用 | 在 `vector_math_stubs.cpp` 中添加真实实现，并注册 `SimpleForward` |
| 新 `Vector128/256<T>` 算子需要 lane-by-lane 回退 | 在 `simd_stubs.cpp` 中添加通用实现 |
| NaN 或 Inf 相关的断言失败 | 检查实现中是否缺失 `isnan()` 或 `std::isinf()` 守卫 |
| 跨平台浮点差异（Windows vs Linux vs ARM64） | 检查 FMA 使用（不同平台舍入不同）或 `sqrt` 精度差异 |
