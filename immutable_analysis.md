# System.Collections.Immutable 问题分析报告

## 一、问题现象

```
Error: Codegen failed: An item with the same key has already been added.
Key: System_Collections_Immutable_ImmutableArray_1_AddRange_0_array
```

当 auto-detect 机制自动将 `System.Collections.Immutable.dll` 加入 `--assembly` 列表时，codegen 在构建方法字典时报**重复键错误**。

## 二、根因分析

### 2.1 数据流

```
namespace-partition.json
  ├── assemblyName: "System.Collections.Immutable"
  ├── chunk: global-ns
  │     ├── methods: 12
  │     └── namespaces: ["System.Collections.Frozen"]
  └── benchmarkMethodIndices: [0..35]
```

- **12 个 chunk 方法**：Frozen 命名空间下的方法
- **36 个 benchmark 方法**：实际测试方法数（含 Subject_N 包装器）
- **Namespace filter**: `System.Collections.Frozen`

### 2.2 冲突链

```
TPG 加载 CombinedSubjects.dll  → 编译它的方法
  └── NativeSymbol: System_Collections_Immutable_ImmutableArray_1_AddRange_0_array

Auto-detect 加入 System.Collections.Immutable.dll  → 编译 ALL 方法
  └── NativeSymbol: System_Collections_Immutable_ImmutableArray_1_AddRange_0_array  ← 相同！

字典 Add() → ArgumentException: 键已存在
```

**原因**：CombinedSubjects（测试包装器程序集）引用了 `System.Collections.Immutable`，它的某些测试方法（如调用 `ImmutableArray.AddRange`）生成了与目标程序集相同的 NativeSymbol。

### 2.3 为什么 namespace filter 没挡住

Namespace filter `System.Collections.Frozen` 是在**方法选取阶段**生效的，而重复键错误发生在更早的**AOT IR 加载阶段**——`_methodsBySubjectId` 字典构建时。两个阶段在 codegen 中是分离的：

```
AOT IR 加载（所有 --assembly 的方法） → 字典构建（这里崩溃！） → namespace filter → 发射
```

## 三、修复方案

### 方案 A：跳过 namespace filter 块的 auto-detect ✅ 已实现

在 TPG 检测到 `--namespace-filter` 时，跳过 auto-detection。

**优点**：简单，零风险  
**缺点**：Immutable chunk 拿不到原生编译的 Frozen 方法（依然走解释器）

### 方案 B：精确加载 — 只加目标 namespace 的方法

不把整个 DLL 作为 `--assembly`，而是解析 DLL 的 IL 元数据，只提取 `System.Collections.Frozen` 命名空间下的方法，单独注入 codegen。

**优点**：避免了 namespace 外的冲突  
**缺点**：
- 需要实现 IL 元数据扫描（方法级过滤）
- 涉及 ManagedMethodModel 的构造，需要打通 TPG → Codegen 的精确方法注入通道
- 改动量大

### 方案 C：Codegen 层去重 — 遇到重复键时跳过而非抛异常

在 codegen 的字典构建点（`_methodsBySubjectId`）用 `TryAdd` 代替 `Add`：

```csharp
// 当前：
_methodsBySubjectId.Add(method.SubjectId, method);

// 改为：
_methodsBySubjectId.TryAdd(method.SubjectId, method);
```

并配合 namespace filter 在后续阶段过滤重复的方法。

**优点**：
- 改动量小（~10 行）
- 对所有 namespace filter chunk 通用
- 不影响现有逻辑（重复方法在 namespace filter 阶段被过滤）

**缺点**：
- 可能掩盖真正的冲突（两个不同方法偶然同 key）
- 需要确认后续阶段能正确处理被跳过的方法

### 方案 D：TPG 级 namespace filter — 加载 DLL 前就过滤

TPG 在解析 metadata 时，根据 `namespace-partition.json` 的 `namespaces` 字段，只把目标 namespace 的方法信息传给 codegen，而不是整个 DLL。

**优点**：最精确，将问题掐死在源头  
**缺点**：
- 需要修改 TPG 的方法提取逻辑
- 需要打通 TPG → Codegen 的过滤管道
- 改动最大

## 四、建议

**推荐方案 C**：改动最小，效果最佳。`TryAdd` 替代 `Add` 配合已有的 namespace filter，让 Frozen 方法进入编译，非 Frozen 方法被过滤掉。对其他 namespace filter chunk 也适用，不会重新引入性能问题。
