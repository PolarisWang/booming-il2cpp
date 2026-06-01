# Blocker: JIT ToChar(String) .NET 8 对比超 20%

## 触发步骤
Step 6 — .NET 8 对比检查

## 失败项
| 方法 | JIT (ns) | .NET 8 (ns) | 差距 |
|------|----------|-------------|------|
| ToChar(String) JIT | 10.14 | 3.349 | **+202.8%** ❌ |
| ToChar(String,IFormatProvider) JIT | 7.85 | 3.649 | **+115.1%** ❌ |

## 根因分析
JIT 的 InlineShapeDescriptor 是纯 AOT 概念。JIT 路径有自己独立的内联基础设施（`jit_inline.h` / `jit_inline.cpp`），其 `EvaluateInline()` 当前是**始终返回 `false` 的存根**。这导致每个 Subject_N JIT 调用都走完整的分发 → 解释器 → kernel 路径。

.NET 8 JIT 可以将 `Convert.ToChar(string)` 编译成几条原生指令直接执行，而 chaos JIT 的存根内联导致每次调用都要通过解释器分派。

## 已尝试的修复
1. **AOT 优化（成功）**: ToChar(String) AOT 从 5.28ns → 3.00ns（-43%），vs .NET 8 = **-10.4% ✅**
   - 修改 `string_table.h`: 暴露 `ResolveWithGlobalCache` 为 inline 变量
   - 修改 `RuntimeHelperShapeRegistry.cs`: GenericShapeDescriptor 为 String 重载 emit 内联 `ResolveWithGlobalCache` 代码
   - 修改 `convert.cpp`: 简化 `chaos_convert_tochar_string` 使用 `ResolveWithGlobalCache`

2. **JIT 优化（不可行）**: 修复 JIT 内联需要实现 `jit_inline.cpp` 的完整内联引擎（IR 树拼接、启发式预算、热补丁失效跟踪），超出本优化战役范围。

## 修复建议
实现 `jit_inline.cpp:EvaluateInline()` 以支持 `Convert.ToChar(string)` 方法调用的内联。具体而言：
- 为 `Convert.ToChar(string)` 注册内联策略
- 当 JIT 遇到 `call Convert.ToChar(string)` 时，将 IL 调用替换为 inline `ResolveWithGlobalCache` + 首字节返回
- 这需要 JIT IR 支持外部 C++ 函数调用的内联展开

## 附：AOT 优化成效（独立于 blocker）
| 方法 | 优化前 | 优化后 | .NET 8 | vs .NET 8 |
|------|--------|--------|--------|-----------|
| ToChar(String) AOT | 5.28ns | 3.00ns | 3.35ns | **-10.4% ✅** |
| ToChar(SByte) AOT | — | 3.75ns | 3.09ns | **+21.2%** |
| ToChar(UInt16) AOT | — | 3.00ns | 4.62ns | **-35.1% ✅** |
| ToChar(UInt64) AOT | — | 2.92ns | 4.37ns | **-33.2% ✅** |
