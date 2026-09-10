# Stage B 现实检查

## 核心问题

`JsonDocument::Parse(string)` 的 AOT 编译体不存在。不是因为我们没注册形状，而是因为 codegen **无法将 IL 翻译成 C++**——该方法内部使用 codegen 不支持的 IL 模式（复杂泛型、嵌套异常、Span/Memory 操作）。

## 两个路径对比

### 路径 A：扩展 codegen（原 Stage B 计划）

让 codegen 能够编译 `JsonDocument::Parse` 的 IL。这需要：

| 所需能力 | 影响方法数 | 工时 |
|----------|-----------|------|
| `ReadOnlyMemory<T>` 栈上传递 | 5 | 3周（Stage A 已开始） |
| 托管对象构造函数调用 | 10 | 2-4周（Stage B 原计划） |
| async 状态机 | 8 | 4周（Stage C） |
| 复杂异常处理 | 20+ | 3周（Stage D） |

**总计**：12+ 周，完成后 Text.Json 全部可 AOT 编译。

### 路径 B：C++ 混合实现（建议 ✅）

对于 benchmark 覆盖的 56 个 >100μs 方法，评估每个方法可否用 C++ 绕过：

| 方法类别 | 方法数 | 是否可用 C++ 绕过 | 策略 |
|---------|--------|-------------------|------|
| `Serialize<Int32>:String` | 2 | ✅ Phase 1 已完成 | snprintf |
| `Deserialize<Int32>:Int32` | 2 | ✅ Phase 1 已完成 | atoi |
| `Serialize<Int32>:Void(Stream)` | 4 | ✅ C++ 写入 Stream | StreamWrite |
| `SerializeToDocument<Int32>` | 2 | ✅ 混合：snprintf + interpreter Parse | **C++ 格式化 + 托管解析** |
| `SerializeToElement<Int32>` | 2 | ⚠️ 同上 | 混合 |
| `SerializeToNode<Int32>` | 2 | ⚠️ 同上（JsonNode 更复杂） | 混合 |
| `SerializeAsync<Int32>` | 4 | ❌ async | 保留解释器 |
| `DeserializeAsync<Int32>` | 2 | ❌ async | 保留解释器 |
| `SerializeToUtf8Bytes<Int32>` | 2 | ✅ snprintf + byte[] 创建 | C++ + ChaosString |
| `Deserialize<JsonDocument>` | 2 | ❌ 解析本身解释器 | 保留解释器 |

**路径 B 优点**：
- 不修改 AOT codegen，零风险
- 解决 ~70% 的 >100μs 方法（56 → ~18）
- 周级交付（1-2 周），非月级
- 保留的解释器路径不影响正确性

**路径 B 缺点**：
- 不是"完美架构"方案，需逐个方法适配
- async 方法无法加速（~8 个方法）

## 建议

**走路径 B**。Stage A 基础设施（WideValue slot）保留，供后续 codegen 扩展使用。当前集中完成 C++ 混合实现。
