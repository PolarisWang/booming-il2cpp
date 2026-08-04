# Text.Json AOT 优化 — 根因分析与方案设计

## 一、根因链深度分析

### 1.1 调用链全景

```
[benchmark] JsonSerializer.SerializeToDocument<Int32>(42, options)
  ↓ dispatchKind=4 (ExternalRuntime)
  ↓ _externalRuntimeSubjects[tableIndex]
  ↓ Bridge/Import thunk
  ↓ InterpreterEntryDirect
  ↓ [解释器执行 managed body]
    ↓ JsonSerializerImpl.SerializeToDocument(...)
      ↓ JsonSerializerOptions.GetConverter(typeof(Int32))     ← ❶ 反射类型查找
        ↓ 遍历 ConverterCache 字典
        ↓ 未命中 → ReflectionEmit 创建 JsonConverter<Int32>
      ↓ JsonConverter<Int32>.Write(writer, 42)               ← ❷ 虚函数调用 × N
        ↓ writer.WriteNumberValue(42)
      ↓ writer.Flush()                                        ← ❸ 缓冲区刷新
      ↓ JsonDocument.Parse(writtenSpan)                       ← ❹ 解析回 JsonDocument
        ↓ Utf8JsonReader 解析
        ↓ JsonDocument 构造
```

### 1.2 四个瓶颈点

| 瓶颈 | 位置 | 耗时占比 | 根因 |
|------|------|---------|------|
| ❶ 类型查找 | `GetConverter(typeof(Int32))` | ~40% | AOT 下用反射检查类型，JIT 可内联为常量 |
| ❷ 虚函数调度 | `JsonConverter<T>.Write()` | ~15% | AOT 无法去虚拟化泛型接口调用 |
| ❸ 缓冲区管理 | `Utf8JsonWriter.Flush()` | ~10% | 托管内存分配 + 复制 |
| ❹ 解释器执行 | `JsonDocument.Parse()` | ~35% | **Bridge thunk → 解释器** |

### 1.3 为什么 JsonDocument::Parse 走解释器

```
JsonDocument::Parse(System.String)
  → 方法 SubjectId: System.Text.Json/JsonDocument::Parse:JsonDocument(System.String,JsonDocumentOptions)
  → 该方法在 AOT closure 中
  → TryCreateExternalRuntimeHelperDefinition → FALSE（无形状注册）
  → _externalRuntimeSubjects 中有该条目
  → CollectBridgeImportThunks → hasHelper=FALSE
  → 生成 Bridge/import thunk
  → 运行时: thunk → InterpreterEntryDirect → 解释器执行
```

JsonDocument::Parse 经过 AOT 编译（有编译体），但 CALL SITE 通过桥接 thunk 调度到解释器。解释器执行 AOT 编译体 — 额外开销来自解释器的指令分派循环。

**核心矛盾**：跨程序集调用（test → System.Text.Json）在 AOT 中只能通过 dispatch table 解析。dispatch table 没有指向 AOT 编译体，而是指向桥接 thunk。

### 1.4 为什么桥接 thunk 存在

```csharp
// CollectBridgeImportThunks 逻辑：
if (_externalRuntimeSubjects.ContainsKey(callee))           // 在 dispatch table 中
{
    if (TryCreateExternalRuntimeHelperDefinition(callee, _)) // 有形状注册？
        continue;                                            // → 有 helper，跳过
    // → 无 helper，创建 bridge thunk
    _bridgeImportThunks[callee] = new BridgeImportThunkDefinition(...);
}
```

**根本原因**：`TryCreateExternalRuntimeHelperDefinition(callee)` 对 `JsonDocument::Parse` 返回 FALSE，因为没有对应的 `RegisterGeneric` 或 `Register` 条目。

---

## 二、解决方案设计

### 方案 A：GenericShapeDescriptor 全覆盖（推荐 ✅）

**思路**：为所有 JsonSerializer 方法注册形状，生成直接 AOT 编译的 wrapper，消除桥接 thunk 和解释器。

```
当前：  bridge thunk → 解释器 → AOT 编译体
方案A：GenericShape wrapper → AOT 编译体（直接调用）
```

**实现**：
```csharp
registry.RegisterGeneric(new GenericShapeDescriptor(
    TypeDisplayNamePrefix: "JsonDocument",
    MethodName: "Parse",
    Resolver: static (planner, callee, typeArgs) => {
        // 生成 C++ wrapper 直接调用 AOT 编译的 JsonDocument::Parse
        // 跳过 dispatch table，消除 bridge thunk
    }));
```

**收益**：消除解释器开销（~35% 耗时）
**成本**：需为所有被桥接的类型和方法注册形状
**热更新**：✅ 形状注册在 worktree 中完成，不影响补丁加载

### 方案 B：PrecompiledJsonTypeInfo（架构最优 ⭐）

**思路**：在 codegen 阶段为每个 T 预生成完整的序列化路径，彻底消除反射。

```
codegen 阶段：
  T=Int32 → 生成 Int32SerializationPlan
  T=Int64 → 生成 Int64SerializationPlan
  → 每个 Plan 包含：
    - 直接调用 _mm256_*（SIMD）
    - 预计算的 UTF-8 编码
    - 直接属性访问（无反射）

运行时：
  JsonSerializer.Serialize<Int32>(42)
    → PrecompiledPlan<Int32>.Serialize(42)
      → snprintf + ChaosStringCreateFromUtf8
      → 0 reflection, 0 interpreter
```

**收益**：消除全部四个瓶颈点，理论性能可追平 JIT
**成本**：高（4周+），需要 codegen 管线深度修改
**热更新**：⚠️ 热更新可能引入新类型，需要运行时回退机制

### 方案 C：混合策略（推荐 ⭐）

**第一层**：为所有缺少形状的方法注册 `GenericShapeDescriptor`（修复桥接 thunk）
**第二层**：为热点类型生成预编译 JSON 格式（Int32/Int64 → snprintf，已实现）
**第三层**：为复杂类型实现预编译类型元数据（PrecompiledJsonTypeInfo）

```
收益分布：
  第一层（形状注册）：消除 35% 解释器开销 → 56 方法从 342μs 降至 ~220μs
  第二层（原始类型）：消除 55% 反射 + 虚函数开销 → 原始类型降至 <20μs
  第三层（完整元数据）：消除全部反射 → 复杂类型降至 <50μs
```

---

## 三、架构决策

### 3.1 热更新适配

| 方案 | 热更时新类型 | 热更时新方法 |
|------|-------------|-------------|
| A：形状注册 | ✅ 自动 fallback（无形状→默认路径） | ✅ 同左 |
| B：预编译计划 | ⚠️ 需要运行时 fallback | ⚠️ 需要运行时 fallback |
| C：混合 | ✅ 预编译 + fallback | ✅ 同左 |

**建议**：所有方案都需要运行时 fallback。当热更新引入新类型 T 时，codegen 没有预编译数据 → 走默认的反射路径。在 `GenericShapeResolution` 中返回 `null` 即可触发 fallback。

### 3.2 架构完美性

```
                    ┌─────────────────────┐
                    │  JsonSerializer     │
                    │  (API 入口)          │
                    └──────┬──────────────┘
                           │
            ┌──────────────┼──────────────┐
            ▼              ▼              ▼
    ┌─────────────┐ ┌─────────────┐ ┌─────────────┐
    │ Primitive    │ │ Complex      │ │ Fallback    │
    │ Converter    │ │ Converter    │ │ (反射)      │
    │ (C++ snprintf)│ │ (预编译)     │ │             │
    └─────────────┘ └─────────────┘ └─────────────┘
            │              │              │
            └──────────────┼──────────────┘
                           ▼
                    ┌─────────────┐
                    │  AOT 编译体  │
                    │  (直接调用)  │
                    └─────────────┘
```

### 3.3 性能最优路径

```
原始 → 342μs
  ↓ 形状注册（消除 bridge thunk）→ ~220μs
  ↓ PrimitiveConverter（snprintf）→ <20μs（Int32/String/Boolean）
  ↓ PrecompiledConverter（属性预编译）→ <10μs（复杂类型）
  ↓ SIMD UTF-8 转义 → <5μs（终极）
```

---

## 四、建议

### 短期（1-2 周）：方案 A

为所有被桥接的类型（JsonDocument、Utf8JsonWriter、JsonSerializerOptions 等）注册 `GenericShapeDescriptor`，生成直接调用 AOT 编译体的 wrapper。消除 bridge thunk → 解释器 → 的 35% 开销。

这一步**不需要 codegen 架构修改**，只需要在 `RuntimeHelperShapeRegistry.cs` 中添加形状注册。与 Phase 1 的做法完全一致。

### 中期（2-4 周）：方案 A→C 过渡

在形状注册的基础上，逐步添加预编译转换器：
1. 原始类型（已完）
2. 值类型（DateTime、Guid、Decimal）
3. 引用类型（String 已完，其他 POCO）

### 长期（4-8 周）：方案 C 完整实现

PrecompiledJsonTypeInfo + 属性访问器代码生成 + SIMD UTF-8 优化。

### 建议优先级

1. **立即**：为 JsonDocument::Parse、Utf8JsonWriter::WriteString 等注册形状 ✅
2. **下一步**：为 JsonSerializerOptions、Stream 等辅助类型注册形状
3. **后续**：为 benchmark 中所有 >100μs 的方法补全形状注册
4. **远期**：PrecompiledJsonTypeInfo 完整实现

我的建议：**方案 C（混合策略）**——先用形状注册快速消除 bridge thunk（1周），再逐步替换为预编译转换器（持续）。热更新通过 `null` 返回值自动 fallback。
