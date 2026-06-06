# Stage B 策略分析

## 核心约束

```
需要编译的方法: JsonDocument::Parse(string) ~300 行 IL
如果加载全部 CoreLib: ~30,000 方法 × ~500 行 IL = 15,000,000 行
codegen 时间: 2min → ~30min (15x)
```

## 方案对比

### 方案 A：按需懒加载（推荐 ✅）

**原理**：只在方法被桥接时才加载其 IL 体。

```
管线流程：
  1. 正常跑完当前管线（subjects 全量编译）
  2. CollectBridgeImportThunks 收集所有桥接方法
  3. 对每个桥接方法：
     a. 从对应 DLL 中加载 IL 体（PEReader.Open）
     b. 注入 AOT IR（含 TypedIL）
     c. 尝试 codegen 编译
     d. 成功 → 替换 dispatch table → 消除桥接
     e. 失败 → 保留桥接（无恶化）
```

| 维度 | 评估 |
|------|------|
| **最终方案？** | ✅ 是。不依赖 allow-list，任何被桥接的方法都会被尝试编译 |
| **codegen 时间** | 🟢 只增加 ~5%（处理几十个桥接方法 vs 正常数千 subjects） |
| **正确性** | 🟢 保留失败方法为桥接，不破坏现有行为 |
| **实现复杂度** | 🟡 中等。需在管线中新增独立阶段，加载 IL + 注入 IR + 重试编译 |
| **风险** | 🟢 低。隔离在桥接收集之后，不影响现有管线 |

### 方案 B：Allow-list 选择性加载

只在 subjects + 指定 allow-list 程序集中加载 IL 体。

```
allow-list = [CombinedSubjects, Chaos.TestFramework.Sdk, System.Text.Json]
→ 额外加载 System.Text.Json ~3,000 方法
→ codegen 时间 +15%
```

| 维度 | 评估 |
|------|------|
| **最终方案？** | ❌ 否。新程序集需手动加入 allow-list |
| **codegen 时间** | 🟡 每个 allow-list 程序集加 ~5-15% |
| **正确性** | 🟢 同 A |
| **实现复杂度** | 🟢 低。只需改 Loader 的 assemblyPaths |
| **风险** | 🟡 需确认 codegen 能编译这些方法，如失败则浪费编译时间 |

### 方案 C：AOT 预编译模块

将 System.Text.Json 单独编译成 AOT 模块，主管线链接该模块。

```
1. 单独编译 System.Text.Json → chaos_text_json.aot
2. 主管线编译 CombinedSubjects → 引用 chaos_text_json.aot
3. JsonDocument::Parse → 从 AOT 模块直接解析符号
```

| 维度 | 评估 |
|------|------|
| **最终方案？** | ✅ 是。可推广到任意框架程序集 |
| **codegen 时间** | 🟢 预编译模块只做一次，后续增量编译 |
| **正确性** | 🟢 独立编译不冲突 |
| **实现复杂度** | 🔴 高。需设计 AOT 模块格式、链接器、运行时加载 |
| **风险** | 🔴 高。需要 AOT 模块 ABI 标准化 |

### 方案 D：分层编译（A+B 混合）

桥接方法按复杂度分层处理：
- **L1（快速）**：参数/返回值都是基础类型 → 用 GenericShape + C++ stub（Phase 1 已实现）
- **L2（中等）**：含托管类型参数/返回值 → 按需加载 IL + codegen 编译（方案 A）
- **L3（复杂）**：async/异常 → 保留桥接（当前兜底）

```
管线：
  Phase 1 形状注册 → L1 方法直接消除 ✅
  Phase 2 按需加载 → L2 方法编译消除 ✅
  Phase 3 桥接保留 → L3 方法 fallback ✅
```

| 维度 | 评估 |
|------|------|
| **最终方案？** | ✅ 是。三层覆盖全部场景 |
| **codegen 时间** | 🟢 最少。L1 几乎零开销，L2 只处理几十个方法 |
| **正确性** | 🟢 三层各有 fallback |
| **实现复杂度** | 🟡 中。L1 已有，L2 需按需加载，L3 是当前行为 |
| **风险** | 🟢 低。可分层交付 |

## 综合评估

```
                   最终方案  codegen时间  复杂度  风险
方案A 按需懒加载     ✅✅       🟢5%      🟡中    🟢低
方案B Allow-list     ❌        🟡15%     🟢低    🟡中
方案C AOT模块        ✅✅       🟢0%      🔴高    🔴高
方案D 分层编译       ✅✅       🟢5%      🟡中    🟢低
```

## 建议

**推荐方案 D（分层编译）**，理由：
1. L1 已有 Phase 1 形状注册基础，可直接使用
2. L2 按需加载是最终方案的核心——不依赖 allow-list，不加载全部 CoreLib
3. L3 保留桥接确保零退化
4. 可分层交付：先做 L1（已完），再做 L2（当前），L3 自然兜底

**L2 的具体实现路径**：
```
在 CollectBridgeImportThunks 之后，新增独立阶段 BridgeAotCompiler：
  1. 遍历 _bridgeImportThunks
  2. 对每个桥接方法，从 DLL 加载 IL 体
  3. 创建 AotCoreIrMethodArtifact + TypedIlMethodArtifact
  4. 注入 codegen 尝试编译
  5. 成功 → 替换桥接为编译体；失败 → 保留桥接
```
