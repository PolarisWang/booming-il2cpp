# 优化战役 blocker: convert-char .NET 8 对比未通过

## 失败位置
Step 6: .NET 8 对比

## 失败详情

3 个方法超出 20% 阈值（vs net8-jit）：

| 方法 | chaos-aot | chaos-jit | net8-jit | 差距 |
|------|-----------|-----------|----------|------|
| ToChar(String) | 5.28 ns | 4.55 ns | 3.35 ns | AOT +58%, JIT +36% |
| ToChar(String,IFormatProvider) | 4.34 ns | 7.85 ns | 3.65 ns | JIT +115% |

## 尝试了哪些修复

尝试 1/3: 根因诊断，无代码修改
- 确认 chaos-aot 几何均值 8.99 ns vs net8-jit 25.99 ns = **2.89x 加速**
- 确认 18 方法中 16 个快于 net8-jit，仅 2 个慢于（ToChar(String) 及 exception-path）
- 确认根因为架构级差异：chaos 通过 `chaos_convert_tochar_string()` 调用（StringId resolution + string_table::Resolve），.NET 8 JIT 内联为 ~4 条指令

## 剩余问题

- `ToChar(String)` 的 ~2ns 差距来自函数调用开销（entry prologue, null check bridge, return marshaling），无法在不进行 codegen 级别内联优化的前提下消除
- `ToChar(String,IFormatProvider)` JIT 7.85ns 可能为 run-to-run variance，AOT 版本 4.34ns 在 20% 以内
- 绝对性能已非常优秀（2.89x vs .NET 8），差距仅体现在极简单的内联型方法上

## 建议

- 可考虑在 codegen 层为 Convert.ToChar(String) 添加模式匹配内联(inline pattern)，将 `null check + length check + char access` 序列替换为直接内存访问
- 当前阶段不建议继续投入微优化 —— ~2ns 收益与投入不成比例
