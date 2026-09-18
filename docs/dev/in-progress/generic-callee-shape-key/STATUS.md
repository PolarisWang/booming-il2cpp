# 泛型 callee 查表键失配 — 立项与方案

> **task_id**: `generic-callee-shape-key`
> **类型**: 缺陷（**系统性**，单 chunk 38 处）
> **创建**: 2026-09-18
> **发现路径**: threading chunk `CompareExchange_7_int_int_int` 残余失败调查

## 〇、结论速览

**显式泛型实例化的 callee，其 method 名带 `<T>`，与注册键不匹配 → 永不命中 → 落 catch-all 返 0。**

- **不是架构缺陷** —— `TryMatchGenericShape` **已支持 `<...>` 格式**（`RuntimeHelperShapeRegistry.cs:375-377`）
- **是注册缺口** —— 相关类型**没有注册 generic descriptor**
- **修法明确**：补 descriptor（有先例，改动局部）

## 一、根因（实测，非推断）

```
callee: System.Threading/Interlocked::CompareExchange<System.Int32>:System.Int32(System.Int32&,System.Int32,System.Int32)
GetMethodNameFromSubjectId → "CompareExchange<System.Int32>"
TryMatchShape 注册键       → "CompareExchange"
→ BuildCanonicalKey 不等 → 永不命中 → 零参 catch-all → 返回 0
```

**实测对照（同 chunk，其余全通 —— 排除替代解释）**：

| 方法 | 结果 |
|---|---|
| `CompareExchange(sbyte|short|byte|ushort|uint|ulong|float|double|long|object)` ×2 | ✅ `real` |
| **`CompareExchange(int,int,int)` ×2** | ❌ **`failed`** |

**关键反证**：`AppendFormatted<T>` **有** descriptor → 15 个 wrapper **全部真转发**，
仅 1 个 catch-all（且那是开放泛型 `AppendFormatted\`1(...!!0)`，属 L3 的 `!!0` 问题）。
**说明「有 descriptor 就通、没有就不通」—— 机制本身工作正常。**

## 二、影响面

单 chunk **38 处**，横跨 6 个类型族：

| 类型 | 方法 | 数量 |
|---|---|---:|
| `SubjectInstanceFactory` | `Create<T>` | 16 |
| `DefaultInterpolatedStringHandler` | `AppendFormatted<T>` | 13 ✓已有 descriptor |
| `ThreadPool` | `QueueUserWorkItem<T>` / `UnsafeQueueUserWorkItem<T>` | 2 |
| `Interlocked` | `CompareExchange<T>` / `Exchange<T>` | 2+ |
| `LazyInitializer` | `EnsureInitialized<T>` | 2 |
| `Volatile` | `Read<T>` / `Write<T>` | 1+ |

> ⚠️ `AppendFormatted` 已有 descriptor 却仍在清单里 —— 说明该清单是
> **「IR 中出现过 `<T>` 拼写」**，不等于「全部失配」。立项时需逐族核实。

## 三、与既有项目的关系（避免重复）

| 项目 | 层 | 关系 |
|---|---|---|
| `generic-instantiation-materialization` L2 | Loader（demand 未物化） | **不同层** |
| `generic-instantiation-materialization` L3 | ABI（`CreateLegacyAbiSlot` 类型降级） | **不同层** |
| **本项目** | **codegen shape registry 查表键** | 独立 |

**边界已验证**：L3 处理「ABI slot 类型解析」（`System.Char`/`!!0` → INTPTR），
本项目处理「method 名 `<T>` 与注册键不匹配」。两者可同时存在且互不覆盖。

**与并行会话的关系**：`Create<T>` 正被 L2 线（`e9e1612ce` 等）处理。
**本项目应避开 `Create<T>`**，先做无冲突的族（`Interlocked` / `LazyInitializer` / `Volatile`）。

## 四、方案选项

### 选项 A —— 补 generic descriptor（**推荐**）

为失配的类型族各注册一个 `GenericShapeDescriptor`，仿既有先例
（`Task::GetAwaiter`、`JsonSerializer::Serialize` 等）。

- ✅ 机制已验证（`AppendFormatted` 即此路径，15/15 真转发）
- ✅ 改动局部、可逐个族验证
- ✅ 不改匹配层，零回归风险
- ✅ 可避开与 L2 线冲突的 `Create<T>`

### 选项 B —— 改 `GetMethodNameFromSubjectId` 剥离 `<...>`（**不推荐**）

- ❌ **会使 `<Int32>` 与 `<Int64>` 坍缩到同一 key** → 错配
  （前者 carrier Int32、后者 Int64）→ 重演 canonical 共享体那类错误
- ❌ 6 个调用方全部受影响，需逐个评估
- ❌ 治「名不匹配」但没解决「不同实例化需可区分」这个真问题

**结论：选 A。** 若将来 descriptor 数量爆炸，再考虑 B 的变体
（剥离 `<...>` 得方法名 + 把类型实参并入 key）。

## 五、实施计划（分批，每批独立可验）

### 批次 1 —— Interlocked（最小验证）

- `CompareExchange<T>` / `Exchange<T>`
- native 已存在（`ChaosInterlockedCompareExchangeInt32` 等）
- 修法：注册 descriptor，按 `typeArgs[0]` 选 native 符号
- **验收**：`CompareExchange_7_int_int_int_{0,3}` 转 `real`

### 批次 2 —— LazyInitializer / Volatile

- `EnsureInitialized<T>` / `Read<T>` / `Write<T>`
- 需确认 native 是否齐备（`EnsureInitialized` 可能要新建）

### 批次 3 —— ThreadPool（**先确认是否属诚实边界**）

- `QueueUserWorkItem<T>` 在 `managed_primitive_entries.h` 有**书面决定不接线**
  （只导出 `(callback, state)` 两参形态）→ **可能不应修**，需先判定

### 明确不做 —— `Create<T>`

与 L2 并行线冲突，**不在本项目范围**。

## 六、执行纪律（memory 血泪）

1. **改 codegen 后必须重建 TPG**（`tpg-bundles-stale-generator-dll`）
   — pipeline 用 `bin/Debug`，不是 Release
2. **跑 pipeline 前查并发**（`concurrent-pipeline-shares-artifacts-root`）：
   `tasklist | grep -iE "MSBuild|cl\.exe"` 应为 0
3. **判据用生成物**：wrapper 是**零参**且体为 `ChaosExternalRuntimeFallback`
   → 无 shape；有 shape 的形态是**带参 + `return ChaosXxx(arg)`**
4. **反向验证**：新测试须能**在修复前失败**，否则测的是恒真
5. **native 测试放 `tests/contracts/native/`**（`testing/` 未跟踪）

## 七、验收标准

- 批次 1 完成后：threading chunk `failed` 11 → 9
- 每批次独立提交，commit message 含 `root_cause` / `fix_strategy` / `regression_check`
- 新增 native 回归测试覆盖各 carrier 宽度（仿
  `interlocked_arithmetic_test.cpp`）

## 八、风险与回退

| 风险 | 缓解 |
|---|---|
| `typeArgs` 解析格式不稳（`<System.Int32>` vs `` `1 ``） | 先打印实测 callee 再写 resolver |
| descriptor 前缀匹配过宽（误匹配 `List<T>::Sort`） | 已有类型前缀校验（`:383`），沿用 |
| 与并行会话改同一文件 | 每批开工前 `git log -1`，冲突则让行 |
