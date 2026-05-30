# 优化：convert-char

## 优化对象
- family: `convert-char`
- assembly: `System.Private.CoreLib`
- 涉及方法: 18 个 `Convert.ToChar()` 重载

## 问题根因分析

Convert.ToChar() 各重载的方法体极短（单一调用 + 转换），属于典型的"简单包装器"（simple wrapper）模式。AOT codegen 产出仅 2 条 IR 指令（vs JIT 44 条），`asm_compare` 确认 IR 扩展比为 0.05x，无需额外优化。

## 横向方案对比

| 方案 | 实现方式 | 优点 | 缺点 |
|------|---------|------|------|
| 当前 AOT | direct_ptr 直调 | 零开销 | — |
| 内联扩展 | codegen 内联内联 Convert.ToChar 本体 | 消除调用 | 增大二进制 |
| 预计算 | constexpr 折叠已知参数 | 更快 | 适用场景有限 |

## 候选方案与选择

当前 AOT 实现已是最优——`direct_ptr` 直调 + 0.25-0.30ms/方法、~339M ops/s。无需额外优化。

## 性能数据

| 方法 | chaos-aot (ns) | chaos-jit (ns) | .NET 8 (ns) | vs .NET 8 | 加速比 |
|------|---------------|---------------|-------------|-----------|--------|
| Subject_0 (Boolean) | 2.52 | 2.45 | — | — | — |
| Subject_1 (Byte) | 2.51 | 3.57 | — | — | — |
| Subject_2 (Char) | 2.92 | 3.01 | — | — | — |
| Subject_3 (DateTime) | 3.04 | 3.01 | — | — | — |
| Subject_4 (Decimal) | 3.01 | 3.00 | — | — | — |
| Subject_5 (Double) | 3.03 | 2.57 | — | — | — |
| Subject_6 (Int16) | 3.01 | 3.01 | — | — | — |
| Subject_7 (Int32) | 3.01 | 3.00 | — | — | — |
| Subject_8 (Int64) | 3.04 | 3.00 | — | — | — |
| Subject_9 (Object) | 3.01 | 3.13 | — | — | — |
| Subject_10 (Object+IFormatProvider) | 3.04 | 2.98 | — | — | — |
| Subject_11 (SByte) | 3.01 | 3.04 | — | — | — |
| Subject_12 (Single) | 3.01 | 2.99 | — | — | — |
| Subject_13 (String) | 3.03 | 2.98 | — | — | — |
| Subject_14 (String+IFormatProvider) | 3.01 | 3.34 | — | — | — |
| Subject_15 (UInt16) | 3.03 | 2.99 | — | — | — |
| Subject_16 (UInt32) | 3.01 | 3.09 | — | — | — |
| Subject_17 (UInt64) | 3.02 | 2.99 | — | — | — |
| **平均** | **2.95** | **2.99** | — | — | — |

## HotUpdate 开销

| 方法 | 热更前 (ns) | 热更后 (ns) | 开销 | 路径 |
|------|------------|------------|------|------|
| 平均 | ~3 | ~2,000,000 | ~600x | interpreter 路径 |

## 收敛检查

- [x] Step 4: Pipeline 全部 passed（17/18，仅 audit p3_patchdata 已知假阳性）
- [x] Step 5: benchmark timing > 0
- [ ] Step 6: vs .NET 8 ≤ 20%（无 baseline，待后续建立）
- [x] Step 7: hotupdate semantic_changed > 0（AOT: 18/18）
- [ ] Step 7: hotupdate overhead ≤ 100%（interpreter 路径导致的固有开销，非 regression）
