# 新项目立项：fact-266 残留 stub 专项（Convert.ChangeType / Enum.TryParse / ReadOnlySpan / Nullable 等）

> 继承自 fact-266，但**这是一个全新的独立项目**，需要完整方案设计（dev-brainstorm）后执行。

---

## 1. 问题定义

### 1.1 已知 stub 方法清单（共 8 个方法，4 个真实问题）

| 方法 | 当前行为 | 正确行为 | 难度 |
|------|---------|---------|------|
| `Convert.ChangeType(object, TypeCode)` | 返回 null | 应装箱转换 | 🔴 高 |
| `Convert.ChangeType(object, TypeCode, IFormatProvider)` | 返回 null | 应装箱转换 | 🔴 高 |
| `Enum.TryParse<T>(string, out T)` 2 个变体 | 返回 false | 应解析枚举 | 🔴 高 |
| `ReadOnlySpan<T>.ToArray()` | 返回 null | 应分配数组 | 🟡 中 |
| `Nullable<T>.GetValueRefOrDefaultRef(ref Nullable<T>)` | 返回 &struct | 应返回 value 字段指针 | 🟢 低 |
| `Delegate.Combine/Remove` | 真 native（null→null 正确） | 已实现 | ✅ 无需修 |
| `String.Join` | 真 native（null→"" 正确） | 已实现 | ✅ 无需修 |

### 1.2 ATG 测试缺口

**核心问题**：ATG 探针只对 `default(T)` 输入测了一个值集（Set0），所以：
- `Convert.ChangeType(default(object), default(TypeCode))` → probe 捕获 null → AOT 返回 null → 测试通过
- 但 `Convert.ChangeType(42, Int32)` 从未被 probe → 测试从不验证这个路径

**结果是**：stub-return-0 方法通过了测试，但不是因为测试正确，而是因为**测试覆盖的输入集太窄**。

## 2. 需要讨论的设计问题

### 2.1 测试层面
- **如何让 ATG 对 stub 方法 probe 更多输入**（非 default 输入）？
  - 对 `Convert.ChangeType`：需要 probe `(123, Int32)`、`(true, String)` 等
  - 对 `Enum.TryParse`：需要 probe `("Monday", true, out DayOfWeek)` 等
  - 对 `ReadOnlySpan.ToArray`：需要 probe `new int[] {1,2,3}.AsSpan().ToArray()` 等
  - 这些输入需要 ATG 的 `ValueSet` 生成器扩展，或手动添加测试用例

### 2.2 实现层面
| 问题 | 实现方案（讨论用） |
|------|------------------|
| `Convert.ChangeType` | 需要 TypeCode→IConvertible 分发表，或反射调用。可复用现有 Convert 系列 native |
| `Enum.TryParse<T>` | 需要 EnumMetadata 解析，或 `EnumToString` 的反向操作。需 `EnumMetadataTable` 查表 |
| `ReadOnlySpan<T>.ToArray()` | 需分配数组并拷贝。可简易实现 `new[] { default(T) }` |
| `Nullable<T>.GetValueRefOrDefaultRef` | 返回 struct 内 value 字段地址。需知道 OffsetOf |

## 3. 建议的执行策略

### 阶段 1（最容易，先做验证）
- `Nullable<T>.GetValueRefOrDefaultRef` — 已有 inline `{0}` 回声，但应返回**值字段指针**（offset 4）。这是纯 codegen 修复，可在不扩展 ATG 的情况下正确实现。
- `ReadOnlySpan<T>.ToArray` — 分配 `new T[1] { default(T) }`。native 实现简单，正确性可验证。

### 阶段 2（中等，需 ATG 扩展）
- 扩展 ATG 测试集：对已知 stub 方法添加多值 probe（非 default 输入）
- 跑 pipeline 观察哪些现有修复失败 → 这是真问题暴露的过程

### 阶段 3（最难，需 runtime 基础设施）
- `Convert.ChangeType` — TypeCode 分发表
- `Enum.TryParse` — EnumMetadata 解析

## 4. 下一步

建议先用 `dev-brainstorm` 技能完成完整方案设计，涵盖：
1. ATG 测试缺口根治方案
2. 每个 stub 方法的实现设计
3. 优先级排序
4. 验收标准

## 5. 关联文档

- `docs/dev/in-progress/fact-266-stub-codegen/FACT-REVIEW-2026-08-31.md`（完整 review 报告）
- 已记录 memory：`fact266-remaining-13-hard-wall.md`