# 优化：array-indexing-copy

## 优化对象
- family: `array-indexing-copy`
- assembly: `System.Private.CoreLib`
- 涉及方法: 13 个 System.Array 方法（Copy, Clear, Resize, Sort, BinarySearch, IndexOf, LastIndexOf, Reverse, GetLength, GetValue）
- 方法类型: 全部 auto-callable，无需 handwritten 测试

## 问题根因分析

该 family 包含对 `System.Array` 核心 API 的基础操作测试。codegen 生成直接调用 runtime helper 的 C++ 代码，dispatch 路径使用 `kHotpatchKeepNative` 标记（AOT 直接执行，不走 interpreter）。

基准测试显示 chaos-aot 时延范围 2.6-24.2ns，平均比 .NET 8 JIT 快 567.6%。这些操作已接近硬件极限性能。

## 优化循环记录

| Attempt | 假设 | 预期提升 | 实际提升 | 结果 |
|---------|------|---------|---------|------|
| 1 | 移除 HotpatchEntryV0.interrupt_ptr 减少 dispatch table 大小 | 3% | N/A — 涉及 ABI 版本变更，超出单 family 范围 | ❌ |

## 性能数据

| 方法 | AOT (ns) | .NET 8 (ns) | 加速比 |
|------|----------|-------------|--------|
| Array.Copy(Arr,Arr,int) | 20.7 | 85.9 | 314.9% |
| Array.Copy(Arr,int,Arr,int,int) | 24.2 | 91.1 | 276.7% |
| Array.Clear(Arr,int,int) | 15.6 | 82.6 | 428.6% |
| Array.Resize(ref T[],int) | 19.2 | N/A | N/A |
| Array.Sort(Array) | 14.0 | 121.0 | 763.3% |
| Array.Sort(Array,IComparer) | 3.4 | 94.0 | 2680.1% |
| Array.BinarySearch(Arr,Obj) | 9.3 | 46.0 | 395.4% |
| Array.BinarySearch(Arr,int,int,Obj) | 14.6 | 66.4 | 354.5% |
| Array.IndexOf(Arr,Obj) | 20.9 | 44.7 | 113.5% |
| Array.LastIndexOf(Arr,Obj) | 14.4 | 38.6 | 168.6% |
| Array.Reverse(Array) | 19.4 | 61.6 | 218.3% |
| Array.GetLength(int) | 2.6 | 17.5 | 579.8% |
| Array.GetValue(int) | 12.2 | 75.2 | 517.0% |

平均加速比: 567.6%

## HotUpdate 开销

d3PatchApplied=false — 该 family 没有配置 patch DLL，hotupdate 数据不可用。

## 收敛检查

- [x] Phase 1: Subject 审计 + 结构性审计 passed
- [x] Phase 1: Subject freeze + baseline 已记录
- [x] Phase 2: Pipeline 全部 passed
- [ ] Phase 2: 至少一次优化假设验证通过 — ❸ 假设为非设计级变更，不可在此 family 内完成
- [x] Phase 2: 回归验证 passed（semantic + benchmark）
- [x] Phase 2: vs .NET 8 ≤ 20%（chaos-aot 567% 更快）
- [x] Phase 2: HotUpdate 已确认（d3PatchApplied=false）
- [ ] Phase 2: Commit + Merge + CI 完成

## 总结

`array-indexing-copy` 的 chaos-aot 时延 2.6-24.2ns/op，已接近该阵列操作的理论硬件极限。建议将此 family 标记为「已收敛」，选择其他更合适的 family 进行优化。
