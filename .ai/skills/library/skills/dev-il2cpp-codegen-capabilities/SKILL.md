---
name: dev-il2cpp-codegen-capabilities
description: capabilities.json schema、ATG 特性过滤、codegen 能力反馈机制
---

> 加载说明(⚠️ Skill 工具注入/当前Agent做不做): 见 `_shared/expert-loading-notice.md`。
>

# dev-il2cpp-codegen-capabilities — codegen 能力反馈机制专家

## 领域边界

### 我负责的
- **capabilities.json schema**：codegen 构建产出的能力描述 JSON 的 schema 设计与维护
- **ATG 特性过滤**：DllScanner 的黑名单方法列表与 capabilities.json 联动
- **能力反馈机制**：codegen 新增能力时自动更新 capabilities.json，ATG 据此放行方法
- **GenericCapabilityMatrix**：泛型参数的约束条件矩阵（unmanaged, new, max_generic_params 等）
- **TypeCapability 注册**：CodeRegistration 中的 type_capabilities 条目维护
- **Capability taxonomy**：capability family 的定义和分类体系

### 我不负责的（超出以下范围 → 标记 remaining，回 Dispatcher 重新分发）

- **ATG 核心逻辑**（Probe 机制、SubjectId 名称生成）→ 超出范围，标记 remaining，原因：需要 ATG 域知识
- **CodeGen 模板引擎**（Scriban 渲染、emitter 管线）→ 超出范围，标记 remaining，原因：需要 CodeGen 域知识
- **运行时能力检测**（runtime feature detection）→ 超出范围，标记 remaining，原因：需要运行时域知识

## 输出格式（Dispatcher 回读用）

每个 Expert 处理完任务后，必须在当前上下文中输出：

```
✅ done: [已处理的子任务 ID 列表]
⏳ remaining: [未处理的子任务 ID 列表 + 原因]
```

---

## 触发条件

- 从 `dev-il2cpp-core-agent` 路由到本 expert
- `capabilities.json` schema 需要修改或扩展
- ATG 需要根据 codegen 能力过滤方法
- codegen 新增了需要同步 capabilities 的特性

---

## 知识域

### 关键源文件

| 文件 | 说明 |
|------|------|
| `src/managed/Chaos.IL2CPP.Generator/ConvertToCpp/ConvertToCppHandler.cs` | capabilities.json 生成入口 |
| `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/DllScanner.cs` | ATG 方法过滤（UnprobableMethods） |
| `src/managed/Chaos.IL2CPP.Generator/CodeGenStage.cs` | MethodCapabilities 传递 |
| `src/managed/Chaos.IL2CPP.Generator/GenericCapabilityMatrixBuilder.cs` | 泛型约束矩阵 |
| `wiki/03-功能模块/06-il2cpp核心架构/capability-taxonomy.md` | Capability taxonomy 设计文档 |

### capabilities.json schema（v1）

```json
{
  "version": 1,
  "features": {
    "async_methods": false,
    "vector_t_boxing": false,
    "generic_constraints": ["unmanaged", "new"],
    "max_generic_params": 4
  },
  "unsupported_method_patterns": [
    ".*::Invoke\\(System\\.Object,.*",
    ".*::BeginInvoke\\(System\\.Object,.*"
  ]
}
```

### Capability Family 分类

| Family | Flag | 说明 |
|--------|------|------|
| Async | async_methods | async/await 支持 |
| Vector SIMD | vector_t_boxing | Vector<T> boxing 支持 |
| Generic Constraints | generic_constraints | 泛型约束类型 |
| Generic Arity | max_generic_params | 最大泛型参数数量 |

---

## 执行流程

### Step 1：加载上下文
1. 读取当前 capabilities.json schema
2. 检查 DllScanner.UnprobableMethods 黑名单
3. 确认 codegen 当前支持的能力范围

### Step 2：实现
1. 修改 capabilities.json schema（如需扩展）
2. 同步更新 DllScanner 的过滤逻辑
3. 更新 GenericCapabilityMatrix（如需新增约束）

### Step 3：验证
1. ATG 构建产出 capabilities.json 格式正确
2. 黑名单方法在对应能力标注为 true 后出现在测试集中

### Step 4：输出
```
✅ done: [已处理的子任务 ID 列表]
⏳ remaining: [未处理的子任务 ID 列表 + 原因]
```

## 集成点

| 上游 | 本 skill | 下游 |
|------|----------|------|
| `dev-il2cpp-core-agent` → codegen 路由 | **dev-il2cpp-codegen-capabilities** | `dev-il2cpp-atg-expert` |
| ATG 黑名单变更 | | `dev-project-test-governance` |
| codegen 新特性交付 | | `dev-project-wiki-maintenance` |


