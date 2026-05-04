# 04-历史决策 INDEX

> 从 `docs/discuss/` 设计讨论中提炼的最终决策结论。只保留确定后的方案和约束。

## 文档

| 文档 | 说明 |
|------|------|
| [`01-管线五层职责决策.md`](./01-%E7%AE%A1%E7%BA%BF%E4%BA%94%E5%B1%82%E8%81%8C%E8%B4%A3%E5%86%B3%E7%AD%96.md) | Loader/SemanticWorld/Linker/CodeGen/Emitter 职责和禁止事项 |
| [`02-SemanticWorld与Linker边界决策.md`](./02-SemanticWorld%E4%B8%8ELinker%E8%BE%B9%E7%95%8C%E5%86%B3%E7%AD%96.md) | SemanticWorld 6块输出、CanonicalSubjects、Linker output contract、StableMetadataLookup |
| [`03-Lowering与Emission决策.md`](./03-Lowering%E4%B8%8EEmission%E5%86%B3%E7%AD%96.md) | LoweringFamily vocabulary、LoweringPlan role contract、EmissionOrder、SemanticShapes、CapabilityBundles、MethodSemanticBody |
| [`04-性能优先治理决策.md`](./04-%E6%80%A7%E8%83%BD%E4%BC%98%E5%85%88%E6%B2%BB%E7%90%86%E5%86%B3%E7%AD%96.md) | Phase A 8项正式决策，OptimizationFacts contract，性能优先原则 |
| [`05-测试框架与报告决策.md`](./05-%E6%B5%8B%E8%AF%95%E6%A1%86%E6%9E%B6%E4%B8%8E%E6%8A%A5%E5%91%8A%E5%86%B3%E7%AD%96.md) | Subject framework、Reporting schema、Engine binding、Phase A/B 边界 |
| [`06-实现层优化决策.md`](./06-%E5%AE%9E%E7%8E%B0%E5%B1%82%E4%BC%98%E5%8C%96%E5%86%B3%E7%AD%96.md) | 代码优化审计（缓存、索引、消除 O(n) 扫描等） |
| [`07-依赖策略与Engine合约决策.md`](./07-%E4%BE%9D%E8%B5%96%E7%AD%96%E7%95%A5%E4%B8%8EEngine%E5%90%88%E7%BA%A6%E5%86%B3%E7%AD%96.md) | 依赖策略、Emitter 模板边界、Engine binding 合约 |
| [`08-Linker输出与Reason决策.md`](./08-Linker%E8%BE%93%E5%87%BA%E4%B8%8EReason%E5%86%B3%E7%AD%96.md) | Linker 输出合约、Reason 分类法、Reason 词汇表、EmissionOrder、诊断合约 |
| [`09-CodeGen输入与Lowering选择决策.md`](./09-CodeGen%E8%BE%93%E5%85%A5%E4%B8%8ELowering%E9%80%89%E6%8B%A9%E5%86%B3%E7%AD%96.md) | CodeGen 输入合约、LoweringFamily 选择面与规则形状 |
| [`10-Loader与操作流决策.md`](./10-Loader%E4%B8%8E%E6%93%8D%E4%BD%9C%E6%B5%81%E5%86%B3%E7%AD%96.md) | Loader/SemanticWorld 职责、操作流粒度、操作词汇表、Payload 合约 |
| [`11-ValueRef与SemanticBody决策.md`](./11-ValueRef%E4%B8%8ESemanticBody%E5%86%B3%E7%AD%96.md) | ValueRef 模型、LiteralRef 池化、SpanId 合约、MethodSemanticBody 合约 |
| [`12-诊断与CanonicalSubjects决策.md`](./12-%E8%AF%8A%E6%96%AD%E4%B8%8ECanonicalSubjects%E5%86%B3%E7%AD%96.md) | SW 诊断、CapabilityBundle 词汇表、SemanticShapes、CanonicalSubjects、StableMetadataLookup |
| [`13-泛型注册Gen2Handle方案决策.md`](./13-%E6%B3%9B%E5%9E%8B%E6%B3%A8%E5%86%8CGen2Handle%E6%96%B9%E6%A1%88%E5%86%B3%E7%AD%96.md) | Gen-2 Handle 双路径架构、Interpreted Fallback、方案对比评估、值类型布局与虚方法决策 |
| [`14-B2-ModuleRegistry-TwoTier-决策.md`](./14-B2-ModuleRegistry-TwoTier-%E5%86%B3%E7%AD%96.md) | B2 Module Registry + Two-Tier 反射元数据架构，替代 TLS constexpr 指针方案，解决跨 DLL 类型身份、Binary 膨胀和 HotUpdate 问题 |
| [`15-B2+VTable虚方法分派决策.md`](./15-B2+VTable%E8%99%9A%E6%96%B9%E6%B3%95%E5%88%86%E6%B4%BE%E5%86%B3%E7%AD%96.md) | B2+ 虚方法分派架构：Per-Object VTable + 继承链 slot 自动编号 + iface_map 接口分派 |
| [`16-RuntimeCentricCrossDllDispatch决策.md`](./16-RuntimeCentricCrossDllDispatch%E5%86%B3%E7%AD%96.md) | Runtime-Centric 跨 DLL 分派 + STRING_ID 完整架构：method_table、StringId-first ldstr、TypeInfo stable_id 双路径比较 |
