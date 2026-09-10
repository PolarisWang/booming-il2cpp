# PrecompiledJsonTypeInfo 架构设计

## 问题边界

JsonSerializer 的慢路径链：
```
JsonSerializer.Serialize<T>(value, options)
  → options.GetConverter(typeof(T))        // JsonSerializerOptions 内部
    → JsonConverter<T>.ctor()              // 反射创建转换器
    → JsonPropertyInfo<T>.Create()        // 运行时属性发现
      → PropertyInfo.GetValue/SetValue     // 反射属性访问
    → converter.Write(writer, value)       // 多态调度
```

AOT 下无法消除上述反射链。PrecompiledJsonTypeInfo 在 **codegen 阶段** 预生成等价代码。

## 核心设计

### 1. 运行时代码结构

```cpp
// codegen 阶段为每个 T 生成的预编译转换器
// 存储在 entry.exe 的只读数据段
struct PrecompiledConverter {
    CHAOS_IL2CPP_INTPTR (*Serialize)(CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR options);
    CHAOS_IL2CPP_INTPTR (*Deserialize)(CHAOS_IL2CPP_INTPTR json, CHAOS_IL2CPP_INTPTR options);
};

// 注册表：codegen 阶段生成，全局可见
extern "C" PrecompiledConverter g_precompiledConverters[];
extern "C" const char* g_precompiledConverterKeys[];
extern "C" int32_t g_precompiledConverterCount;
```

### 2. 调用路径

```
JsonSerializer.Serialize<Int32>(42, options)
  → 优化的 codegen 路径（dispatchKind=Direct，非 ExternalRuntime）
    → 查找 g_precompiledConverters["System.Int32"]
      → 调用 converter.Serialize(42, options)
        → snprintf(buffer, "%d", value)
        → ChaosStringCreateFromUtf8(buffer)
        → 返回 managed string
```

### 3. 类型覆盖范围

| 类型 | 序列化方法 | 复杂度 |
|------|-----------|--------|
| Int32/Int64 | snprintf | 低 |
| String | 直接返回 | 低 |
| Boolean | "true"/"false" | 低 |
| Single/Double | snprintf | 低 |
| JsonDocument | 转发到 managed body | 高 |
| JsonElement | 转发到 managed body | 高 |
| JsonNode | 转发到 managed body | 高 |
| Array<T> | 循环元素 | 高 |
| POCO | 属性遍历 + 逐个序列化 | 最高 |

### 4. Managed 侧的改动

```csharp
// RuntimeHelperShapeRegistry.cs
registry.RegisterGeneric(new GenericShapeDescriptor(
    TypeDisplayNamePrefix: "JsonSerializer",
    MethodName: "Serialize",
    Resolver: static (planner, callee, typeArgs) => {
        var tArg = ExtractTypeArg(callee);
        var converter = FindOrCreatePrecompiledConverter(tArg);
        if (converter == null) return null; // fallback
        return new GenericShapeResolution(
            source: converter.CppSource,
            symbol: converter.Symbol,
            parameterAbis: converter.ParameterAbis,
            returnAbi: converter.ReturnAbi,
            rawArgumentIndices: converter.RawArgIndices,
            DirectNativeSymbol: converter.NativeFnName);
    }));
```

### 5. ChaosStringCreateFromUtf8

需要在 C++ runtime 层导出一个安全的字符串创建函数：

```cpp
// runtime-core 中导出
extern "C" CHAOS_IL2CPP_INTPTR ChaosStringCreateFromUtf8(const char* utf8, int32_t length);
```

内部调用 `StringNewUtf8(GetCurrentRuntimeState(), GetCurrentThreadState(), utf8, length)`。

## 实施依赖

1. **ChaosStringCreateFromUtf8 导出** — 让 interop stubs 能创建 managed string
2. **AOT Core IR 扫描** — 收集 JsonSerializer 调用的 T 参数
3. **PrecompiledConverter 注册表** — 预生成的转换器函数
4. **GenericShapeDescriptor 路由** — 将调用分发到预编译转换器

## 工作量估算

| 模块 | 工时 | 风险 |
|------|------|------|
| ChaosStringCreateFromUtf8 | 2 天 | 低 |
| AOT Core IR 扫描 | 3 天 | 中 |
| PrecompiledConverter 注册表 | 5 天 | 中 |
| GenericShapeDescriptor | 3 天 | 低 |
| POCO 属性生成 | 7 天 | 高 |
| 测试 + Benchmark 验证 | 5 天 | 中 |
| **总计** | **~25 天** | |
