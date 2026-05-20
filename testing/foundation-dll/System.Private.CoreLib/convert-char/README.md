# Convert.Char Conversions

- familyId: `family/System.Private.CoreLib/convert/char`
- assembly: `System.Private.CoreLib`
- 方法数: 18（11 非 throwing + 7 throwing）
- 覆盖类型: Boolean, Byte, SByte, Int16, UInt16, Int32, UInt32, Int64, UInt64, Char, Single, Double, Decimal, DateTime, String, Object, Object+IFormatProvider

## 实现

所有 18 个方法通过 auto-generate 实现，无需 handwrite：
- **值类型**（Byte, SByte, Int16 等 10 个）— inline expression 优化，C++ 生成 `static_cast<CHAOS_IL2CPP_UINT16>(_s0)`，零函数调用开销
- **throwing**（Boolean, DateTime, Decimal, Double, Single 共 5 个）— external runtime helper 自动生成 `throw InvalidCastException`
- **string**（String, String+IFormatProvider 共 2 个）— external runtime helper 生成带 FormatException 检查的解析
- **Object**（Object, Object+IFormatProvider 共 1+1 个）— 拆箱后截断

## Pipeline 验证

| 阶段 | 结果 |
|------|------|
| Fact | 18/18 passed |
| Audit | false_passing=0 |
| L5 xUnit | 16 tests pass |

## 迁移状态

导入到新框架：2026-05-21
