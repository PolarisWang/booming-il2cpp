# Review Notes — Primitive Numeric Conversions

## AI 生成的测试 (2026-05-11)

### 审核摘要
- simple（带断言）: 25 个
- mixed（冒烟）: 0 个
- needs-manual: 0 个

### 逐方法审核

#### Convert.ToBoolean(String)
- **分类**: simple
- **测试目的**: 验证 "true"/"false" 字符串正确转换为 bool
- **输入**: "true", "false"
- **预期**: true / false
- **风险**: 边界格式（大小写、空白）未覆盖

#### Convert.ToByte(String)
- **分类**: simple
- **测试目的**: 验证字符串到 byte 的转换
- **输入**: "42", "255"
- **预期**: 42, 255
- **风险**: 越界字符串未覆盖（预期抛出异常）

#### Convert.ToInt16(String)
- **分类**: simple
- **测试目的**: 验证字符串到 short 的转换，含负数
- **输入**: "42", "-32768"
- **预期**: 42, short.MinValue

#### Convert.ToInt32(String)
- **分类**: simple
- **测试目的**: 验证字符串到 int 的转换，含负数
- **输入**: "42", "-42"
- **预期**: 42, -42

#### Convert.ToInt64(String)
- **分类**: simple
- **测试目的**: 验证字符串到 long 的转换，含大数
- **输入**: "42", "9223372036854775807"
- **预期**: 42, long.MaxValue

#### Convert.ToSingle(String)
- **分类**: simple
- **测试目的**: 验证字符串到 float 的转换
- **输入**: "42.5"
- **预期**: 42.5f

#### Convert.ToDouble(String)
- **分类**: simple
- **测试目的**: 验证字符串到 double 的转换
- **输入**: "42.5"
- **预期**: 42.5

#### Convert.ToDecimal(String)
- **分类**: simple
- **测试目的**: 验证字符串到 decimal 的转换
- **输入**: "42.5"
- **预期**: 42.5m

#### Convert.ToString(Int32)
- **分类**: simple
- **测试目的**: 验证 int 到 string 的转换，含负数
- **输入**: 42, -42
- **预期**: "42", "-42"

#### Convert.ToString(Double)
- **分类**: simple
- **测试目的**: 验证 double 到 string 的转换
- **输入**: 42.5
- **预期**: "42.5"

#### Convert.ToDecimal(Double)
- **分类**: simple
- **测试目的**: 验证 double 到 decimal 的转换
- **输入**: 42.5
- **预期**: 42.5m

#### Convert.ToInt32(Double)
- **分类**: simple
- **测试目的**: 验证 double 到 int 的截断转换，含负数
- **输入**: 42.5, -42.5
- **预期**: 42, -42

#### Int32.Parse(String)
- **分类**: simple
- **测试目的**: 验证字符串解析为 int，含负数
- **输入**: "42", "-42"
- **预期**: 42, -42

#### Int64.Parse(String)
- **分类**: simple
- **测试目的**: 验证字符串解析为 long，含大数
- **输入**: "42", "9223372036854775807"
- **预期**: 42, long.MaxValue

#### Double.Parse(String)
- **分类**: simple
- **测试目的**: 验证字符串解析为 double，含负数
- **输入**: "42.5", "-42.5"
- **预期**: 42.5, -42.5

### 未覆盖的场景
- 抛出异常的输入（null, 格式错误, 越界）
- IFormatProvider 重载
- 十六进制格式字符串
- 带空白的输入字符串
- CultureInfo 相关行为

### 审核结论
- [x] 所有 simple 测试断言合理
- [x] 无需 mixed 或 needs-manual 方法（全部为简单类型转换）
- [x] 25 个 Fact 方法覆盖全部 15 个 methodSubjectIds
- [ ] benchmark/hotupdate 已存在单独的 benchmark harness 和 hotupdate 验证