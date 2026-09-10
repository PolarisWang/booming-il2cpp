---
task_id: M1-arch-review
lifecycle_status: completed
---

# M1-arch-review: MetadataWriterStage 架构审视

> **子任务:** M1-arch-review (Phase 1)
> **状态:** completed

---

## 审视结论

### 1. 职责拆分分析

MetadataWriterStage.cs（339 行）当前混入 5 类职责：

| 职责 | 方法 | 行数 | 是否应拆分 |
|------|------|------|-----------|
| **入口编排** | `Write()` | 1-47 | 保持 |
| **AotManifest 构建** | `BuildAotManifest()` | 49-92 | 可保持（44 行） |
| **MetadataRegistration 构建** | `BuildMetadataRegistration()` | 94-191 | 可保持（98 行，最大块） |
| **SupplementalTemplate 构建** | `BuildSupplementalTemplate()` + `BuildGenericDemandLookup()` + `ResolveRuntimeGenericContext()` + `ResolveGenericDiagnostic()` | 193-317 | 需要关注（125 行，含泛型逻辑） |
| **泛型辅助方法** | `EnsureEquivalentDemand()` + `AreEquivalentInstantiationKeys()` + `SequenceEqual()` | 319-369 | 应拆分到 `GenericDemandHelpers` 或类似 helper 类 |

**建议：**
- `BuildMetadataRegistration()` 已接近 100 行，如果继续增长应考虑拆分出 `MetadataRegistrationBuilder` 类
- `EnsureEquivalentDemand()` + `AreEquivalentInstantiationKeys()` + `SequenceEqual()` 是纯辅助逻辑，当前 50 行且与主流程无关，建议独立为内部 `GenericDemandValidator` 类
- Authority 决策（STATUS.md 中的边界拍板）规定：控制在 500 行以内，超过则拆分。当前 339 行，暂无拆分压力但已接近临界区

### 2. 错误处理细化评估

当前错误码及触发条件：

| 错误码 | 触发条件 | 细化程度 |
|--------|---------|---------|
| `METADATA_WRITER_DEMAND_CONFLICT` | InvalidOperationException("conflicting generic instantiation") | **死代码**（见下文） |
| `METADATA_WRITER_INVALID_INPUT` | ArgumentNullException | 覆盖面合理 |
| `METADATA_WRITER_INTERNAL_ERROR` | 所有其他 Exception | 兜底，合理 |

**发现：`EnsureEquivalentDemand` 是死代码**

`BuildGenericDemandLookup()` 在遇到重复 SubjectId 时执行 `continue` 跳过，**从未调用** `EnsureEquivalentDemand()`。这意味着：
- `METADATA_WRITER_DEMAND_CONFLICT` 错误码理论上**无法被触发**
- 重复的泛型需求被静默忽略（first-wins 语义）
- `EnsureEquivalentDemand()` 及其调用的 `AreEquivalentInstantiationKeys()` 和 `SequenceEqual()` 共 50 行代码处于死亡状态

**建议：**
- 明确 first-wins 是故意设计还是待实现。如果是故意设计，应删除 `EnsureEquivalentDemand` 及相关死代码并移除 `METADATA_WRITER_DEMAND_CONFLICT` 错误码。如果是待实现，应集成到 `BuildGenericDemandLookup()` 中
- 当前架构审查建议：**明确 first-wins 语义，删除死代码**，作为短期措施

### 3. 文件拆分决策

按照 Authority 决策，当前 339 行 < 500 行阈值，**不拆分**。记录以下决策：

- 当 `MetadataWriterStage.cs` 超过 500 行时：将 `BuildMetadataRegistration()` 拆到 `MetadataRegistrationBuilder` 类，将泛型辅助方法拆到 `GenericDemandValidator` 类
- 新文件放在 `src/managed/Chaos.IL2CPP.MetadataWriter/` 目录下
- 辅助类标记为 `internal`，保持在同一命名空间

### 4. 其他发现

| 编号 | 发现 | 严重度 | 建议 |
|------|------|--------|------|
| A-1 | `BuildGenericDemandLookup` 使用 `StringComparer.Ordinal` 作为字典比较器，与 SubjectId 比较语义一致 | 无 | 保持 |
| A-2 | `SequenceEqual` 与 `System.Linq.Enumerable.SequenceEqual` 功能重复 | 低 | 如果删除死代码则一并删除；否则替换为 Linq 调用 |
| A-3 | 所有构建方法都是 `static`，无实例状态，设计合理 | 无 | 保持 |
| A-4 | `Write()` 方法使用 `try-catch` 包裹全部逻辑，但 `catch(Exception)` 粒度过粗，可能掩盖编码错误 | 中 | 建议在开发阶段配置 `CHAOS_IL2CPP_FAIL` 或类似机制捕获意外异常 |
| A-5 | `PipelineResult<T>` 的 `Error.Exception` 未被下游消费用于日志或诊断（仓库中无搜索到的消费者） | 低 | 保留结构以备将来集成追踪系统 |

### 5. 总体评分

| 维度 | 评分 | 说明 |
|------|------|------|
| 职责内聚 | B | 基本内聚，但有 50 行死代码和即将到阈值的最大方法 |
| 错误处理 | C | 死代码产生不可达错误码 + 兜底 catch 粒度过粗 |
| 可测试性 | A | Write() 入口设计清晰，私有方法可通过 Write() 间接测试 |
| 可维护性 | B | 单文件 339 行尚可，但 BuildMetadataRegistration 98 行偏大 |
