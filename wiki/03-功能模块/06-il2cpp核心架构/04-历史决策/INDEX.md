# 04-历史决策 INDEX

> 从 `docs/discuss/` 设计讨论中提炼的最终决策结论。只保留确定后的方案和约束。
>
> 每条决策包含 **TRIGGER 条件**，帮助你判断"什么情况下需要看这个决策"。

## 文档

| 文档 | 说明 | TRIGGER |
|------|------|---------|
| [`01-管线五层职责决策.md`](./01-%E7%AE%A1%E7%BA%BF%E4%BA%94%E5%B1%82%E8%81%8C%E8%B4%A3%E5%86%B3%E7%AD%96.md) | Loader/SemanticWorld/Linker/CodeGen/Emitter 职责和禁止事项 | **任何涉及翻译路径选择的任务，或需要理解 pipeline 各层职责时** |
| [`02-SemanticWorld与Linker边界决策.md`](./02-SemanticWorld%E4%B8%8ELinker%E8%BE%B9%E7%95%8C%E5%86%B3%E7%AD%96.md) | SemanticWorld 6块输出、CanonicalSubjects、Linker output contract、StableMetadataLookup | **修改 SemanticWorld 或 Linker 的输出合约，或增加新 subject 类型时** |
| [`03-Lowering与Emission决策.md`](./03-Lowering%E4%B8%8EEmission%E5%86%B3%E7%AD%96.md) | LoweringFamily vocabulary、LoweringPlan role contract、EmissionOrder、SemanticShapes、CapabilityBundles、MethodSemanticBody | **新增 LoweringFamily、修改 EmissionOrder、或改变 CodeGen 输入合约时** |
| [`04-性能优先治理决策.md`](./04-%E6%80%A7%E8%83%BD%E4%BC%98%E5%85%88%E6%B2%BB%E7%90%86%E5%86%B3%E7%AD%96.md) | Phase A 8项正式决策，OptimizationFacts contract，性能优先原则 | **所有性能优化任务的必读文档，或任何需要在性能与其他因素之间做权衡时** |
| [`05-测试框架与报告决策.md`](./05-%E6%B5%8B%E8%AF%95%E6%A1%86%E6%9E%B6%E4%B8%8E%E6%8A%A5%E5%91%8A%E5%86%B3%E7%AD%96.md) | Subject framework、Reporting schema、Engine binding、Phase A/B 边界 | **修改 subject 框架、reporting schema、或测试引擎绑定时** |
| [`06-实现层优化决策.md`](./06-%E5%AE%9E%E7%8E%B0%E5%B1%82%E4%BC%98%E5%8C%96%E5%86%B3%E7%AD%96.md) | 代码优化审计（缓存、索引、消除 O(n) 扫描等） | **进行代码优化审计或消除性能热点时** |
| [`07-依赖策略与Engine合约决策.md`](./07-%E4%BE%9D%E8%B5%96%E7%AD%96%E7%95%A5%E4%B8%8EEngine%E5%90%88%E7%BA%A6%E5%86%B3%E7%AD%96.md) | 依赖策略、Emitter 模板边界、Engine binding 合约 | **修改模块间依赖关系或 Engine 绑定时** |
| [`08-Linker输出与Reason决策.md`](./08-Linker%E8%BE%93%E5%87%BA%E4%B8%8EReason%E5%86%B3%E7%AD%96.md) | Linker 输出合约、Reason 分类法、Reason 词汇表、EmissionOrder、诊断合约 | **修改 Linker 输出或引入新的 Reason 分类时** |
| [`09-CodeGen输入与Lowering选择决策.md`](./09-CodeGen%E8%BE%93%E5%85%A5%E4%B8%8ELowering%E9%80%89%E6%8B%A9%E5%86%B3%E7%AD%96.md) | CodeGen 输入合约、LoweringFamily 选择面与规则形状 | **修改 CodeGen 输入合约或新增 LoweringFamily 选择逻辑时** |
| [`10-Loader与操作流决策.md`](./10-Loader%E4%B8%8E%E6%93%8D%E4%BD%9C%E6%B5%81%E5%86%B3%E7%AD%96.md) | Loader/SemanticWorld 职责、操作流粒度、操作词汇表、Payload 合约 | **修改 Loader 输入或操作流定义时** |
| [`11-ValueRef与SemanticBody决策.md`](./11-ValueRef%E4%B8%8ESemanticBody%E5%86%B3%E7%AD%96.md) | ValueRef 模型、LiteralRef 池化、SpanId 合约、MethodSemanticBody 合约 | **修改 ValueRef/SemanticBody 模型或 SpanId 合同时** |
| [`12-诊断与CanonicalSubjects决策.md`](./12-%E8%AF%8A%E6%96%AD%E4%B8%8ECanonicalSubjects%E5%86%B3%E7%AD%96.md) | SW 诊断、CapabilityBundle 词汇表、SemanticShapes、CanonicalSubjects、StableMetadataLookup | **修改诊断合约、CapabilityBundle 或 CanonicalSubjects 时** |
| [`13-泛型注册Gen2Handle方案决策.md`](./13-%E6%B3%9B%E5%9E%8B%E6%B3%A8%E5%86%8CGen2Handle%E6%96%B9%E6%A1%88%E5%86%B3%E7%AD%96.md) | Gen-2 Handle 双路径架构、Interpreted Fallback、方案对比评估、值类型布局与虚方法决策 | **涉及泛型注册、Gen2 Handle、或泛型 Interpreted Fallback 时** |
| [`14-B2-ModuleRegistry-TwoTier-决策.md`](./14-B2-ModuleRegistry-TwoTier-%E5%86%B3%E7%AD%96.md) | B2 Module Registry + Two-Tier 反射元数据架构 | **涉及模块注册、跨 DLL 类型身份、反射元数据，或消除 TLS constexpr 指针时** |
| [`15-B2+VTable虚方法分派决策.md`](./15-B2+VTable%E8%99%9A%E6%96%B9%E6%B3%95%E5%88%86%E6%B4%BE%E5%86%B3%E7%AD%96.md) | B2+ 虚方法分派架构：Per-Object VTable + 继承链 slot 自动编号 + iface_map 接口分派 | **修改虚方法分派逻辑、VTable 布局、或接口分派时** |
| [`16-RuntimeCentricCrossDllDispatch决策.md`](./16-RuntimeCentricCrossDllDispatch%E5%86%B3%E7%AD%96.md) | Runtime-Centric 跨 DLL 分派 + STRING_ID 完整架构 | **涉及跨 DLL 方法分派、method_table、StringId ldstr、TypeInfo 比较时** |
| [`17-GenericContextRegistry-B+SegmentedShard-决策.md`](./17-GenericContextRegistry-B%2BSegmentedShard-%E5%86%B3%E7%AD%96.md) | GenericContextRegistry B+ 重构 | **涉及泛型上下文注册、Per-Module Shard、热更新卸载 arg_buffer 泄漏时** |
| [`18-HotUpdateDispatchHotpatch-决策.md`](./18-HotUpdateDispatchHotpatch-%E5%86%B3%E7%AD%96.md) | HotUpdate Hotpatch 双层分派架构 | **涉及热更新分派、patchdata 提取、或 PatchMetadataCache 时** |
| [`19-A4-Dual+V2-ObjectHeader优化决策.md`](./19-A4-Dual%2BV2-ObjectHeader%E4%BC%98%E5%8C%96%E5%86%B3%E7%AD%96.md) | A4-Dual+V2 ObjectHeader 优化：分型 header 方案 | **涉及 ObjectHeader 布局、对象头大小优化、或薄锁实现时** |
| [`20-工程规范统一决策.md`](./20-%E5%B7%A5%E7%A8%8B%E8%A7%84%E8%8C%83%E7%BB%9F%E4%B8%80%E5%86%B3%E7%AD%96.md) | 命名空间、Include 风格、clang-format、extern "C" 规范 | **修改命名空间、include 风格、或格式化配置时** |
| [`21-MarshalABI平台能力决策.md`](./21-MarshalABI%E5%B9%B3%E5%8F%B0%E8%83%BD%E5%8A%9B%E5%86%B3%E7%AD%96.md) | GenericShapeDescriptor、Descriptor-Driven 结构体、分配器选择 | **涉及 Marshal/COM 互操作、结构体封送、或委托 Thunk 时** |
| [`22-结构化恢复度量与FlatFallback消除决策.md`](./22-%E7%BB%93%E6%9E%84%E5%8C%96%E6%81%A2%E5%A4%8D%E5%BA%A6%E9%87%8F%E4%B8%8EFlatFallback%E6%B6%88%E9%99%A4%E5%86%B3%E7%AD%96.md) | 100% StructuredRecoveryRate 硬契约 | **涉及结构化控制流恢复、FlatFallback 消除、或恢复率度量时** |
| [`23-LCAC跨程序集编译决策.md`](./23-LCAC%E8%B7%A8%E7%A8%8B%E5%BA%8F%E9%9B%86%E7%BC%96%E8%AF%91%E5%86%B3%E7%AD%96.md) | LCAC 替代 BridgeAOT 的跨程序集编译方案 | **涉及跨程序集编译、新增 DLL 支持、或 LCAC 翻译路径时** |
| [`24-Crypto解释器路由方案决策.md`](./24-Crypto%E8%A7%A3%E9%87%8A%E5%99%A8%E8%B7%AF%E7%94%B1%E6%96%B9%E6%A1%88%E5%86%B3%E7%AD%96.md) | Crypto 方法 IL 嵌入 + Interpreter 路由 | **涉及 crypto 执行路径、ExternalRuntimeFallback、interpreter routing 时** |
| [`25-JIT-UNWIND-INFO编码修复记录.md`](./25-JIT-UNWIND-INFO%E7%BC%96%E7%A0%81%E4%BF%AE%E5%A4%8D%E8%AE%B0%E5%BD%95.md) | Win64 UNWIND_CODE nibble 编码修复 | **JIT 编译后 SEH unwind 崩溃、0xC00000FF、jit_unwind.cpp 修改时** |
| [`26-NonCoreLib构建修复决策.md`](./26-NonCoreLib%E6%9E%84%E5%BB%BA%E4%BF%AE%E5%A4%8D%E5%86%B3%E7%AD%96.md) | 非 CoreLib 程序集的 3 个构建修复 | **非 CoreLib 构建失败、LNK2019、C2713/C2712 编译错误时** |
| [`27-外部运行时表Null检查架构限制.md`](./27-%E5%A4%96%E9%83%A8%E8%BF%90%E8%A1%8C%E6%97%B6%E8%A1%A8Null%E6%A3%80%E6%9F%A5%E6%9E%B6%E6%9E%84%E9%99%90%E5%88%B6.md) | 跨程序集调用 callvirt → linkedWorld 缺失的架构限制 | **跨程序集调用失败、callvirt 分派到未注册方法时** |
| [`28-并行Codegen构建竞争修复.md`](./28-%E5%B9%B6%E8%A1%8CCodegen%E6%9E%84%E5%BB%BA%E7%AB%9E%E4%BA%89%E4%BF%AE%E5%A4%8D.md) | 并行 AOT+JIT cmake 构建竞争导致二进制损坏 | **codegen.py 构建步骤、AOT/JIT 二进制损坏时** |
| [`29-SIMD-Overload重命名挂起记录.md`](./29-SIMD-Overload%E9%87%8D%E5%91%BD%E5%90%8D%E6%8C%82%E8%B5%B7%E8%AE%B0%E5%BD%95.md) | extern "C" SIMD 重载 C2733 修复（挂起） | **C2733、SIMD stub 修改、simd_stubs.h/cpp 修改时** |
| [`30-Entry-SEH捕获缺口修复.md`](./30-Entry-SEH%E6%8D%95%E8%8E%B7%E7%BC%BA%E5%8F%A3%E4%BF%AE%E5%A4%8D.md) | __try/__except 替代 C++ catch 捕获 AOT AV | **AOT 方法 SEH 异常、entry.cpp 异常处理修改时** |
| [`31-Shutdown-Abort-Longjmp修复.md`](./31-Shutdown-Abort-Longjmp%E4%BF%AE%E5%A4%8D.md) | SIGABRT + longjmp 恢复中止的验证流程 | **0xC0000409 ShutdownAV、abort 杀死验证进程时** |
