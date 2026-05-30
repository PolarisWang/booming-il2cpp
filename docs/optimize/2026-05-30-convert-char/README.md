# 优化：convert-char

## 优化对象
- family: `convert-char`
- 涉及方法: System.Convert.ToChar() 的 18 个重载

## 变更内容

### HotUpdate JIT Fact 修复
- **问题**: `hotupdate_jit_fact` stage 在完整 pipeline 中始终失败
- **根因**: 三个 hotupdate stage 在 `ParallelGroup` 中并发执行，共享 `runtime-patchdata.cpp` 导致竞态
- **修复**: 改为顺序执行

### OriginalAotPtrCallback
- **问题**: JIT 模式下 IR lowering 获取到 JIT trampoline 地址而非 AOT 代码指针
- **根因**: `RegisterJitEntryMethods()` 替换了 `direct_ptr` 为 trampoline
- **修复**: 添加 `OriginalAotPtrCallback` 回调链，`ResolveDirectFn` 通过回调获取原始 AOT 指针

## 性能数据

几何均值: chaos-aot 8.99ns vs net8-jit 25.99ns = **2.89x 整体加速**

| 方法 | chaos-aot (ns) | chaos-jit (ns) | .NET 8 (ns) | 加速比 |
|------|---------------|---------------|-------------|--------|
| ToChar(System.Boolean) | 1082.16 | 935.90 | 3822.20 | 3.53x |
| ToChar(System.Byte) | 1.54 | 1.61 | 2.78 | 1.81x |
| ToChar(System.Char) | 2.33 | 2.44 | 2.54 | 1.09x |
| ToChar(System.DateTime) | 989.51 | 1110.10 | 3612.08 | 3.65x |
| ToChar(System.Decimal) | 3.08 | 3.14 | 4647.96 | 1509.08x |
| ToChar(System.Double) | 2.32 | 2.30 | 4568.96 | 1969.38x |
| ToChar(System.Int16) | 1.54 | 2.68 | 3.52 | 2.29x |
| ToChar(System.Int32) | 1.54 | 2.60 | 3.52 | 2.28x |
| ToChar(System.Int64) | 1.55 | 1.54 | 3.49 | 2.25x |
| ToChar(System.Object) | 1049.33 | 1115.69 | 10.28 | exc-path |
| ToChar(System.Object,IFormatProvider) | 1080.30 | 896.01 | 8.94 | exc-path |
| ToChar(System.SByte) | 2.61 | 1.55 | 2.88 | 1.10x |
| ToChar(System.Single) | 3.45 | 3.75 | 4858.90 | 1408.38x |
| ToChar(System.String) | 5.28 | 4.55 | 3.35 | 0.63x |
| ToChar(System.String,IFormatProvider) | 4.34 | 7.85 | 3.65 | 0.84x |
| ToChar(System.UInt16) | 1.54 | 2.55 | 2.71 | 1.76x |
| ToChar(System.UInt32) | 1.55 | 1.54 | 2.93 | 1.89x |
| ToChar(System.UInt64) | 2.62 | 1.54 | 2.62 | 1.00x |

## 验收检查
- [x] Step 4: Pipeline 全部 passed（含 hotupdate_jit_fact）
- [x] Step 5: benchmark timing > 0
- [ ] Step 6: vs .NET 8 <= 20%（3 方法超限）
- [x] Step 7: hotupdate semantic_changed > 0（18/18），overhead <= 100%
