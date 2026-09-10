# Text.Json AOT Optimization

## 目标
将 `JsonSerializer::Serialize<T>()` / `Deserialize<T>()` 的 benchmark 耗时从 320μs 降至 <20μs（vs .NET 8 <20% overhead），消除 56 个 >100μs 的方法。

## 根因
JsonSerializer 内部使用 `JsonTypeInfo<T>` 运行时构建 + `PropertyInfo.GetValue/SetValue` 反射。AOT 无法内联反射链。

## 方案：PrecompiledJsonTypeInfo

AOT codegen 阶段预生成类型元数据，替代运行时反射。

### 架构
```
当前（慢）：
  JsonSerializer.Serialize<T>(obj)
    → GetTypeInfo(typeof(T))            ← 运行时反射
      → CreateJsonTypeInfo<T>()         ← 运行时代码生成
        → PropertyInfo.GetValue()       ← 慢
        → JsonConverter<T>.Write()      ← 虚函数

优化后（快）：
  JsonSerializer.Serialize<T>(obj)
    → PrecompiledJsonTypeInfo<T>        ← AOT 预编译
      → DirectPropertyAccessor.Get()     ← 直接函数调用
      → DirectJsonConverter.Write()      ← 直接函数调用
```

### Phase 1：PrecompiledJsonTypeInfo 框架（2周）

| Step | 内容 | 交付物 |
|------|------|--------|
| 1.1 | AOT Core IR 扫描 JsonSerializer 调用，提取泛型参数 T | `JsonSerializer-reflect-sites.json` |
| 1.2 | 设计 `IPrecompiledJsonConverter<T>` 接口 | 接口定义 |
| 1.3 | 实现基础类型（Int32/String/Boolean）的预编译转换器 | C++ stub |
| 1.4 | 实现 GenericShapeDescriptor 路由到预编译转换器 | 注册代码 |
| 1.5 | Benchmark 验证 | 320μs → <50μs |

### Phase 2：属性访问器生成（2周）

| Step | 内容 | 交付物 |
|------|------|--------|
| 2.1 | Codegen 阶段枚举类型属性 | PropertyAccessor 生成器 |
| 2.2 | 为每个属性生成直接 getter/setter C++ 代码 | 生成的访问器 |
| 2.3 | 多态支持（派生类型、接口、泛型） | 类型注册表 |
| 2.4 | Benchmark 验证 | 50μs → <20μs |

## 文件布局
```
src/managed/Chaos.IL2CPP.Generator/
├── RuntimeSupport/
│   └── PrecompiledJsonTypeInfo.cs         ← 新增
src/native/runtime-core/runtime_stubs/
├── interop_stubs.h                        ← 添加 JSON stubs 声明
└── interop_stubs.cpp                      ← 添加 JSON stubs 实现
```

## 收敛标准
- [ ] JsonSerializer::Serialize<Int32> < 20μs
- [ ] JsonSerializer::Deserialize<Int32> < 20μs
- [ ] >100μs 方法数：56 → 0
- [ ] 全量 benchmark 无退化
