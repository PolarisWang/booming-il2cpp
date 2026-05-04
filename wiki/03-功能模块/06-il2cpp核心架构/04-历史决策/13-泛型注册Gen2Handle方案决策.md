# 13-泛型注册 Gen-2 Handle 方案决策

## 时间

2026-05-04

## 背景

基于现有 TypeInfoHandle 系统的泛型注册方案设计。需求：性能最优、il2cpp 架构最完美、hotupdate 高度适配、泛型功能完整。

## 问题

`MetadataRegistrationV0.generic_types / generic_methods` 是 null 占位符。需要决定 codegen 发射什么样的数据、bootstrap 如何注册、运行时 MakeGenericType 如何支持。

## 备选方案

### 方案 A：静态数组 + Bootstrap 批量注册

codegen 发射纯数据数组，bootstrap 单次循环完成注册。类型参数以 token 传递。

优点：最简、性能最优（数据在 .rdata）、hotupdate 自然增量。
缺点：token 不跨模块，方法/类型 API 不一致。

### 方案 B：Codegen 发射注册调用

codegen 直接发射 `RegisterGenericInstantiation()` 调用语句。

优点：灵活。
缺点：代码膨胀、I-cache 压力、架构代码/数据混合。

### 方案 C：类型静态数组 + 方法内联注册

类型用方案 A，方法用方案 B。

优点：两者优点兼顾。
缺点：两套机制不一致。

### Gen-2 Handle（选定方案）

统一 handle-based API，token→handle 解析在 bootstrap 时完成。

优点：handle 跨模块安全、O(1) 指针比较、统一 API。见下方完整评估。

### Gen-2 Handle + Interpreted Fallback（选定方案）

Gen-2 Handle + MakeGenericType 时 registry 未命中走解释器。

## 选定方案

**Gen-2 Handle + Interpreted Fallback**

## 决策理由

### 按四条标准的加权评分

```
权重: 性能最优(p0)  架构完美(p0)  HotUpdate(p1)  泛型完整(p2)

Gen-2 + Interpreted: 9×10 + 9×4 + 10×2 + 10×1 = 156 ← 最高
Gen-2 AOT only:      9×10 + 9×4 + 10×2 + 5×1  = 151
PreGenerate Hints:    9×10 + 9×4 + 10×2 + 4×1  = 150
Unity IL2CPP:         6×10 + 7×4 +  0×2 + 10×1 = 98
```

### 关键考量

1. **性能最优（p0）**：Gen-2 快路径不受影响，90% 的泛型调用零解释器开销
2. **架构完美（p0）**：模块隔离、handle 统一、数据/逻辑分离、代码极简
3. **HotUpdate（p1）**：`by_module` 索引 + 模块级注册/清理，第一等公民
4. **泛型完整（p2）**：割舍运行时实例化性能（解释器慢 10-50x），但功能完整

### 三条确认决策

1. **解释器 IL 子集**：初始版本只支持最简子集，以实际 MakeGenericType 用例驱动扩展
2. **值类型布局**：RuntimeInstantiatedType 从第一天包含 value_size 和 field_offsets，ComputeValueTypeSize/ComputeFieldOffsets 是必需组件
3. **虚方法派发**：vtable registry 需要支持运行时分配的 token，与 Gen-2 同时实现

## 架构要点

- 双路径：快路径（AOT 注册 handle，O(1) 查询） + 慢路径（解释器 fallback）
- 统一 TypeInfoHandle 指针作为全局类型身份
- token→handle 解析在模块注册时完成
- 扁平数据数组 codegen 发射，无执行代码
- by_module 索引支持 hotupdate 清理

## 关联文档

- `08-泛型执行模型.md`：完整双路径架构
- `15-泛型上下文运行时.md`：注册 API、ModuleGenericRegistrationV0、Registry 内部结构
- `19-解释器VM架构.md`：现有解释器 VM
- `contracts/native/v0/runtime_instantiation.h`：RuntimeInstantiationBridgeV0
- `plan: generic-gen2-handle-full-plan.md`：5 阶段实现计划
