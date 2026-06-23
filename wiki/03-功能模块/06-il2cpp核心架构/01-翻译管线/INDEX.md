# 01-翻译管线 INDEX

> 描述 Chaos IL2CPP 从 IL 到 C++ 的完整翻译流程。阅读顺序：按编号从 `01` 到 `31`。
>
> **编号说明**：部分编号存在文件前缀重叠（如 28、29、30 在"运行时子系统"和"模式选择"两个分类中均有使用），以实际文件名区分为准。两个分类的文档彼此独立，阅读时按分类标题导航即可。

## 文档

### 核心翻译管线（01-12）
| 文档 | 说明 |
|------|------|
| [`01-总体流程与五层职责.md`](./01-%E6%80%BB%E4%BD%93%E6%B5%81%E7%A8%8B%E4%B8%8E%E4%BA%94%E5%B1%82%E8%81%8C%E8%B4%A3.md) | Loader → SemanticWorld → Linker → CodeGen → Emitter |
| [`02-输入合约模型.md`](./02-%E8%BE%93%E5%85%A5%E5%90%88%E7%BA%A6%E6%A8%A1%E5%9E%8B.md) | AotCoreIrArtifact 结构树、引用与类型信息 |
| [`03-求值栈与ABI调用模型.md`](./03-%E6%B1%82%E5%80%BC%E6%A0%88%E4%B8%8EABI%E8%B0%83%E7%94%A8%E6%A8%A1%E5%9E%8B.md) | 求值栈值表示、12种 ABI carrier 映射 |
| [`04-NativeAotLoweringPlanner文件布局.md`](./04-NativeAotLoweringPlanner%E6%96%87%E4%BB%B6%E5%B8%83%E5%B1%80.md) | 20+ partial 文件分布（Planning / Emission / RuntimeSupport） |
| [`04-NativeAotPlanner-API.md`](./04-NativeAotPlanner-API.md) | 公共 API 文档（NativeAotEmitter、LoweringPlanner、TemplateModel、模板目录） |
| [`05-异常处理架构.md`](./05-%E5%BC%82%E5%B8%B8%E5%A4%84%E7%90%86%E6%9E%B6%E6%9E%84.md) | 5种 EH shape 的 C++ 模式 |
| [`06-字符串ID系统.md`](./06-%E5%AD%97%E7%AC%A6%E4%B8%B2ID%E7%B3%BB%E7%BB%9F.md) | FNV-1a 64-bit hash、碰撞处理、AOT baked table |
| [`07-对象模型与相等性.md`](./07-%E5%AF%B9%E8%B1%A1%E6%A8%A1%E5%9E%8B%E4%B8%8E%E7%9B%B8%E7%AD%89%E6%80%A7.md) | C++ struct 定义、类型身份辅助函数、相等性 7 级优先级 |
| [`08-泛型执行模型.md`](./08-%E6%B3%9B%E5%9E%8B%E6%89%A7%E8%A1%8C%E6%A8%A1%E5%9E%8B.md) | GenericContextShapeKind、Gen-2 Handle 双路径架构（AOT 快路径 + Interpreted Fallback）、RuntimeInstantiationEngine |
| [`09-运行时Helper生态.md`](./09-%E8%BF%90%E8%A1%8C%E6%97%B6Helper%E7%94%9F%E6%80%81.md) | 16 类 helper 及其文件分布 |
| [`10-静态初始化模型.md`](./10-%E9%9D%99%E6%80%81%E5%88%9D%E5%A7%8B%E5%8C%96%E6%A8%A1%E5%9E%8B.md) | std::call_once 模式 |
| [`11-CPP运行时模块分层.md`](./11-CPP%E8%BF%90%E8%A1%8C%E6%97%B6%E6%A8%A1%E5%9D%97%E5%88%86%E5%B1%82.md) | 8 模块 DAG、禁止包含列表、命名约定、验证层级 |
| [`12-关键合约.md`](./12-%E5%85%B3%E9%94%AE%E5%90%88%E7%BA%A6.md) | codegen_bridge.h / runtime_abi.h / engine contracts / 共享 ABI 合约 / 版本兼容性 / 功能支持状态 |

### C++ 运行时子系统（13-21）
| 文档 | 说明 |
|------|------|
| [`13-MemoryDomain系统.md`](./13-MemoryDomain%E7%B3%BB%E7%BB%9F.md) | 内存域分配隔离、3 种堆策略、TLS 域栈 |
| [`14-VTable注册表.md`](./14-VTable%E6%B3%A8%E5%86%8C%E8%A1%A8.md) | B2+ 虚方法分派架构：Per-Object VTable + 继承链 slot 自动编号 + iface_map 接口分派 |
| [`15-泛型上下文运行时.md`](./15-%E6%B3%9B%E5%9E%8B%E4%B8%8A%E4%B8%8B%E6%96%87%E5%8C%BA%E8%BF%90%E8%A1%8C%E6%97%B6.md) | Handle-based API、ModuleGenericRegistrationV0 注册束、token→handle 解析注册、by_module hotupdate 清理、RuntimeInstantiationBridgeV0 |
| [`16-字符串表运行时.md`](./16-%E5%AD%97%E7%AC%A6%E4%B8%B2%E8%A1%A8%E8%BF%90%E8%A1%8C%E6%97%B6.md) | 运行时字符串解析、AOT 二分查找、动态注册 |
| [`17-反射查询模型.md`](./17-%E5%8F%8D%E5%B0%84%E6%9F%A5%E8%AF%A2%E6%A8%A1%E5%9E%8B.md) | 不可变反射描述符、指针标记句柄、AOT const 数据 |
| [`18-热更新架构.md`](./18-%E7%83%AD%E6%9B%B4%E6%96%B0%E6%9E%B6%E6%9E%84.md) | 包加载/卸载、方法替换表、MemoryDomain 关联 |
| [`19-解释器VM架构.md`](./19-%E8%A7%A3%E9%87%8A%E5%99%A8VM%E6%9E%B6%E6%9E%84.md) | IR opcode、栈机执行模型、EH 支持 |
| [`20-Chaos公共库.md`](./20-Chaos%E5%85%AC%E5%85%B1%E5%BA%93.md) | 15 个头文件、native_types 宏、指针标记、async/await 基元 |
| [`21-引导程序与引擎桥接.md`](./21-%E5%BC%95%E5%AF%BC%E7%A8%8B%E5%BA%8F%E4%B8%8E%E5%BC%95%E6%93%8E%E6%A1%A5%E6%8E%A5.md) | BootstrapState、codegen 注册、engine C++ 实现 |
| [`22-Planner设计模式.md`](./22-Planner%E8%AE%BE%E8%AE%A1%E6%A8%A1%E5%BC%8F.md) | CustomAttribute物化、反射元数据、RVA字段、委托组合、签名解码、符号命名 |

### C++ 运行时子系统（23+）
| 文档 | 说明 |
|------|------|
| [`23-托管线程模型与GC协作.md`](./23-%E6%89%98%E7%AE%A1%E7%BA%BF%E7%A8%8B%E6%A8%A1%E5%9E%8B%E4%B8%8EGC%E5%8D%8F%E4%BD%9C.md) | 托管线程标识、Monitor Thin Lock、Hybrid GC Safepoint、ThreadPool 基础 |
| [`24-全量IL2CPP编译器架构.md`](./24-%E5%85%A8%E9%87%8FIL2CPP%E7%BC%96%E8%AF%91%E5%99%A8%E6%9E%B6%E6%9E%84.md) | 多程序集输入、每程序集C++输出、跨程序集调用决议、CMake构建、Phase 0-5路线图 |
| [`25-Marshal平台ABI与互操作体系.md`](./25-Marshal%E5%B9%B3%E5%8F%B0ABI%E4%B8%8E%E4%BA%92%E6%93%8D%E4%BD%9C%E4%BD%93%E7%B3%BB.md) | Marshal 方法 Alloc/Free、String、Struct、Delegate、COM Interop |
| [`26-CodeGen度量与结构化恢复.md`](./26-CodeGen%E5%BA%A6%E9%87%8F%E4%B8%8E%E7%BB%93%E6%9E%84%E5%8C%96%E6%81%A2%E5%A4%8D.md) | codegen-metrics 格式、flat-fallback 消除、4 种模式、100% structuredRecoveryRate |
| [`27-JIT原生代码生成器架构.md`](./27-T4%E5%8E%9F%E7%94%9F%E4%BB%A3%E7%A0%81%E7%94%9F%E6%88%90%E5%99%A8%E6%9E%B6%E6%9E%84.md) | JIT 自适应分层、NativeCodeGenerator IR→x64 翻译、5 阶段 V3 能力、全部 103 opcode 支持 |
| [`28-AssemblyManager与ALC.md`](./28-AssemblyManager%E4%B8%8EALC.md) | AssemblyLoadContext 生命周期、shared_mutex 锁策略、静态字段管理、加载/卸载流程 |
| [`28-MetadataWriter架构.md`](./28-MetadataWriter%E6%9E%B6%E6%9E%84.md) | MetadataWriter 模型与增量/全量 emit 流程 |
| [`29-ModuleRegistry运行时参考.md`](./29-ModuleRegistry%E8%BF%90%E8%A1%8C%E6%97%B6%E5%8F%82%E8%80%83.md) | 模块注册/查找/墓碑/回收、TypeInfoHandle、并发安全 |
| [`30-PatchLoader运行时参考.md`](./30-PatchLoader%E8%BF%90%E8%A1%8C%E6%97%B6%E5%8F%82%E8%80%83.md) | .patchdata 加载验证、PatchContext/PatchMetadataCache 生命周期、溢出保护 |

### 模式选择与切换
| 文档 | 说明 |
|------|------|
| [`29-模式选择决策树.md`](./29-%E6%A8%A1%E5%BC%8F%E9%80%89%E6%8B%A9%E5%86%B3%E7%AD%96%E6%A0%91.md) | AOT/JIT 两种编译模式的适用场景对比、决策树流程图、典型场景推荐 |
| [`30-模式间切换指南.md`](./30-%E6%A8%A1%E5%BC%8F%E9%97%B4%E5%88%87%E6%8D%A2%E6%8C%87%E5%8D%97.md) | 构建期 --mode 配置、运行时模式检测、跨模式 dispatch 路由、平台约束 |
| [`31-故障诊断指南.md`](./31-%E6%95%85%E9%9A%9C%E8%AF%8A%E6%96%AD%E6%8C%87%E5%8D%97.md) | 按模式分类的常见故障诊断、排查框架、定位工具使用 |
| [`32-codegen-2026-06-修复链总结.md`](./32-codegen-2026-06-%E4%BF%AE%E5%A4%8D%E9%93%BE%E6%80%BB%E7%BB%93.md) | C2660/C2061/SEH 修复汇总、架构要点、15 个 commit、关键文件索引、已知限制 |
