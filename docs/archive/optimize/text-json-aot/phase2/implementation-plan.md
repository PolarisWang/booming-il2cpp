# Phase 2: PrecompiledJsonTypeInfo 实现计划

## 核心思路

在 codegen 阶段，对于每个 `JsonSerializer::Serialize<T>()` / `Deserialize<T>()` 调用：
1. 提取类型参数 T
2. 枚举 T 的 public 可序列化属性
3. 为每个属性生成直接 getter/setter C++ 代码（非反射）
4. 生成预编译的 `JsonTypeInfo` 初始化代码
5. 通过 GenericShapeDescriptor 路由到预编译实现

## 实现步骤

### Step 2.1 — AOT IR 类型参数提取器（当前任务）
在 codegen 阶段扫描所有 reachable methods 的指令，收集 JsonSerializer 调用中的 T。

### Step 2.2 — 属性枚举器
给定类型 T 的 SubjectId，查询 managed method dictionary 找到其属性 getter/setter。

### Step 2.3 — 代码生成器
为每个 T 生成 C++ 代码：
- 预构建的 JsonPropertyInfo 数组
- 直接属性访问 lambda
- JsonConverter 包装

### Step 2.4 — GenericShapeDescriptor 集成
将生成的代码注册到 shape registry。

## 目标类型
从 Phase 1 分析，benchmark 中出现的类型：
- 原始值类型：Int32, Int64, Boolean, String（Phase 1 已完成）
- DOM 类型：JsonDocument, JsonElement, JsonNode（Phase 2 目标）
- 复杂类型：需要在 benchmark subject 中出现的 POCO 类型
