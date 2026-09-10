# Bridge 架构综合审阅与风险评估报告

## 1. 当前架构总览

```
┌─────────────────────────────────────────────────────────────┐
│                    Subject AOT (主管线)                       │
│  Loader → SemanticWorld → Linker → MetadataWriter → CodeGen  │
│  ↓                                                           │
│  NativeAotLoweringPlanner (6000+ 行 C#, 50+ scriban 模板)      │
│  ↓                                                           │
│  native-aot.generated.cpp (全功能: GC, 异常, 泛型, ABI 映射)  │
└─────────────────────────────────────────────────────────────┘
                            │
                   ExternalRuntime dispatch (callee 不在 subject 中)
                            ↓
┌─────────────────────────────────────────────────────────────┐
│                Bridge 管线（新增）                             │
│  BridgeAotCompiler → BridgeMethodBodyEmitter (~500 行)       │
│  ↓                                                           │
│  bridge-redirect.generated.cpp (简化发射器)                   │
│  ↓                                                           │
│  Phase 2: 注册到 kChaosExternalRuntimeFnTable[]              │
└─────────────────────────────────────────────────────────────┘
```

## 2. 核心问题分析

### 问题 1: 双发射器维护成本

| 维度 | 主管线 (NativeAotLoweringPlanner) | Bridge 发射器 (BridgeMethodBodyEmitter) |
|:----|:---------------------------------|:--------------------------------------|
| 代码量 | 6000+ 行 C# + 50+ scriban 模板 | ~500 行 C# |
| opcode 覆盖 | 完整（含 EH、泛型、委托等） | 50+ 基础 opcode |
| ABI 支持 | 完整（含 float/double/value type） | 仅 INTPTR 简化 |
| GC 栈映射 | ✅ 精确 GC | ❌ 无 |
| 异常处理 | ✅ try/catch/finally | ❌ 跳过 |
| 泛型共享 | ✅ | ❌ |
| 维护同步 | 基准 | 需要**两处修改** |

**风险评级：高**

每次有 opcode 变更、ABI 调整、或新特性加入，都需要同步修改两套发射器。Bridge 发射器与主管线的差异会随时间逐渐增大，最终导致 bridge 编译产出的代码质量远低于主管线。

### 问题 2: ExternalRuntime Dispatch 损耗

当前 bridge 函数的调用路径：

```
Subject AOT code → call chaos_external_X (未命名, 通过 kChaosExternalRuntimeFnTable[i])
                   ↓
               kChaosExternalRuntimeFnTable[i] (数组索引 + 函数指针解引用)
                   ↓
               chaos_bridge_0_TryGetInt32() (编译后的函数体)
```

vs 理想 Direct 调用：

```
Subject AOT code → call chaos_bridge_0_TryGetInt32() (直接符号调用)
```

ExternalRuntime dispatch 引入的损耗：
1. **数组索引：** `kChaosExternalRuntimeFnTable[i]`（~1 条指令）
2. **函数指针调用：** 无法内联
3. **编译器优化屏障：** 编译器无法跨函数指针调用做优化

每层约 2-5 条额外指令，对简单方法（如 `GetInt32`，只有几条 IL）影响比例可能达 30-50%。

**风险评级：中**

### 问题 3: 缺少运行时元数据

Bridge 函数体无法访问：
| 操作 | 当前实现 | 正确做法 |
|:----|:--------|:---------|
| `newobj` | `ObjectNew(typeHash, 0)` | 需要 TypeInfo 指针 + GC 分配 |
| `ldsfld` | `return extern _sfld_HASH` | 需要静态字段地址 |
| `box` | `BoxValueObject(hash, &v)` | 需要类型描述符 + 分配信息 |
| `isinst` | `chaos_isinst(obj, hash)` | 需要完整类型层级信息 |
| `stfld` | `*reinterpret_cast<INTPTR*>(obj)` | 需要字段偏移 + GC 写屏障 |

**结果：** bridge 函数体虽然被调用，但行为不正确（分配 0 字节、读静态字段返回 0）。即使 Phase 2 注册生效，**benchmark 结果不可信**。

**风险评级：高**

### 问题 4: C2733 符号冲突（根因）

当初引入独立发射器的根因：把 bridge 方法加入 `aotCoreIr.Methods` 后，emitter 生成 `chaos_stub_definition_*` 重复符号导致 C2733。

**根因更深层分析：**
- `stub_definition` 符号由 `InvocationPlanning.cs` 的 `TryCreateExternalRuntimeHelperDefinition()` 生成
- 当 bridge 方法引用了同一个 callee，而该 callee 已有 `GenericShapeDescriptor` 时，生成相同的 stub 符号
- 两个定义在不同 TU page 上有不同签名 → C2733

**这个根因仍存在**——没能根本上解决。独立发射器只是绕过了它。

**风险评级：高**

---

## 3. 方案对比分析

### 方案 A：维持现状（独立发射器 + Phase 2 注册）

| 维度 | 评分 |
|:----|:----:|
| 实现复杂度 | 低（已实现） |
| **维护成本** | **高**（两套发射器） |
| **函数体质量** | **低**（无元数据） |
| **性能上限** | **低**（ExternalRuntime 间接层） |
| 增量可用 | ✅ |
| 近期风险 | 低 |
| 长期风险 | 高 |

### 方案 B：修复 C2733 + 合入主管线

修复 C2733 的方案：

```
方案 B1: 独立 TU page
为 bridge 方法分配独立的 page 文件，与 subject 方法不在同一 TU，
stub 符号自然不会冲突。

方案 B2: 在 IsSubjectMethod 阶段排除 bridge 方法
bridge 方法不经过 kSubjectSlotMap，callee 重复的 stub 定义只保留一个。

方案 B3: DispatchKindCode 设为 Direct
bridge 方法对 callee 的调用使用 Direct dispatch（指向已编译的 chaos_bridge_* 符号），
不用 ExternalRuntime，不生成 stub_definition 符号。
```

**最佳选择：B1 + B3 组合**
- B3：bridge 方法内部对 **已 bridge 编译的 callee** 用 Direct dispatch
- B1：bridge 方法放在独立 page，避免与 subject 方法冲突
- 对于未编译的 callee，走 ExternalRuntime（但不会生成 duplicate stub，因为 bridge 方法在独立 page 上）

| 维度 | 评分 |
|:----|:----:|
| **维护成本** | **低**（单一发射器） |
| **函数体质量** | **高**（完整的 ABI、GC、元数据） |
| **性能上限** | **高**（Direct dispatch + 全优化） |
| 实现复杂度 | 中（需要修改 page 分配逻辑） |
| 近期风险 | 中 |
| 长期风险 | 低 |

### 方案 C：彻底消除 ExternalRuntime 间接层

在方案 B 的基础上，对 bridge 方法的调用也改为 Direct dispatch：

```
1. Bridge 编译完成后，在 aotCoreIr.Methods 中用 bridge 方法的 NativeSymbol 更新
2. 扫描所有 subject 方法，将指向 bridge 方法的 callee 从 ExternalRuntime 改为 Direct
3. 主管线正常处理，Direct dispatch 无间接层
```

这需要 bridge 方法编译完成后回填 subject 方法的指令，侵入性较大。

| 维度 | 评分 |
|:----|:----:|
| **维护成本** | **低**（单一发射器） |
| **函数体质量** | **高** |
| **性能上限** | **最高**（零间接层） |
| 实现复杂度 | 高（需要修改 AOT IR 后处理） |
| 风险 | 中高 |

---

## 4. 综合风险评估矩阵

| 风险 | 概率 | 影响 | 当前方案 B | 方案 B | 方案 C |
|:----|:----:|:----:|:---------:|:-----:|:-----:|
| 双发射器不同步 | 高 | 高（编译错误/正确性） | ⚠️ 必然发生 | ✅ 消除 | ✅ 消除 |
| 函数体质量不足 | 高 | 高（错误结果） | ⚠️ 已有（0字节分配） | ✅ 主管线 | ✅ 主管线 |
| 性能提升不足 | 中 | 中 | ⚠️ ExternalRuntime+简化体 | ✅ Direct+完整体 | ✅ 零间接 |
| ExternalRuntime 损耗 | 中 | 低中 | ⚠️ 查表+指针调用 | ✅ Direct dispatch | ✅ 消除 |
| C2733 复发 | 低 | 高 | ⚠️ 被绕开未解决 | ✅ 根治 | ✅ 根治 |
| hotupdate 兼容 | 低 | 中 | ⚠️ 未验证 | ✅ | ✅ |
| 维护人力成本 | — | — | **高** (2 套发射器) | **低** (1 套) | **低** (1 套) |

---

## 5. 建议

### 短期（1-2 周）：修复 C2733，合入主管线

**核心工作：**
1. 修复 `InvocationPlanning.cs` 中 `TryCreateExternalRuntimeHelperDefinition` 的 stub 重复生成逻辑——当 callee 已有注册的 ExternalRuntime 条目时，跳过 stub 生成
2. 把 bridge 方法放在独立 page 上（修改 page 分配逻辑或利用现有的 type-group 机制）
3. 移除 `BridgeMethodBodyEmitter.cs` 和 `FullAssemblyEmitter.GenerateBridgeRedirectCpp`

**预期结果：**
- 单一发射器维护
- bridge 方法获得完整的 ABI/GC/异常支持
- 无需担心函数体质量

### 中期（2-4 周）：Direct dispatch + 消除 ExternalRuntime

在 C2733 修复的基础上：
1. bridge 方法编译完成后，patch subject 方法的指令（`DispatchKindCode → Direct`，`TargetSymbol → chaos_bridge_X`）
2. bridge 方法内部对已编译的 callee 也用 Direct dispatch
3. 最终调用链：`Subject AOT → call chaos_bridge_X`（无间接层）

---

## 6. 结论

当前独立发射器方案是用**短期快速完成**换**长期维护债务**。核心权衡：

| | 独立发射器（当前） | 合入主管线（建议） |
|:--|:---------------:|:----------------:|
| 实现时间 | 1 周 | 1-2 周 |
| 当前功能 | 可用但不完整 | 需额外工作 |
| **长期维护** | **每月新增同步工作** | **零额外工作** |
| **函数体正确性** | **不可靠** | **可靠** |
| **性能上限** | **有天花板** | **无天花板** |

**建议放弃独立发射器，投入 1-2 周修复 C2733 后合入主管线。** 当前独立发射器的 500 行代码在修复 C2733 后可以完全删除，长期来看节省的维护成本远超短期投入。
