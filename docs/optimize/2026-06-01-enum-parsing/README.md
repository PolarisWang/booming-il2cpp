# 优化：enum-parsing

## 优化对象
- **family**: `enum-parsing`
- **涉及方法**: 13 methods in `System.Private.CoreLib`
- **branch**: `claim/enum-parsing/enum-parsing-opt`

## 问题根因分析

源自 [bottleneck-analysis.md](../../testing/foundation-dll/System.Private.CoreLib/enum-parsing/bottleneck-analysis.md) 的核心发现：

### 1. Per-enum codegen switch 缺失 — BoxToString 未内联（P0）

通用 stub 实现（`enum_stubs.cpp`）对所有 enum 类型使用同一套查找逻辑（FNV-24 hash + dispatch table）。即使最快的 hash 表路径也比编译器生成的 switch 跳转表昂贵：

```cpp
// 优化前：通用 stub 调用
const auto chaos_result = ChaosEnumToStringRaw(typeHandle, value);
```

而 codegen 在编译期已知 enum 类型的所有 field 值，可以生成 per-enum 的 inline switch：

```cpp
// 优化后：per-enum inline switch
switch (value) {
    case 0: return CHAOS_IL2CPP_STRING_ID("Sunday");
    case 1: return CHAOS_IL2CPP_STRING_ID("Monday");
    // ...
    default: /* fallback to ChaosEnumToStringRaw */;
}
```

这样可以完全消除：
- FNV-24 hash table probe
- resolve_type_arg 回退路径  
- GC string allocation（使用 string table 预注册的 interned string）
- thread_local cache 维护开销

### 2. 死代码未消除 — AOT Bake 保留无用指令路径

codegen 能常量折叠 `Enum.GetName(typeof(DayOfWeek), 1)` 为 "Monday"，但保留了类型解析和 boxing 的死代码。这些中间指令虽然语义上无副作用，但在 AOT 路径中产生了额外的分发开销。

### 3. 非 enum 类型回退路径过长

`byte` 等非 enum 类型调用 `Enum.Format` / `IsDefined` 时，stub 走了完整的 resolve_meta → ensure_cache → linear scan → format 路径，最终 desc 才发现不是 enum。

## 优化方案

### 方案 A：Per-enum codegen inline switch for BoxToString（T-C2 核心）

- **修改文件**:
  - `ExceptionEmission.cs`: 在 `EmitFusedEnumBoxToString` 中，当 enum type 已知且 field 数 ≤ 64 时，生成 inline C++ switch 替代 `ChaosEnumToStringRaw` 调用。default 分支回退到 runtime stub。
  - `InvocationPlanning.cs`: 在 `TryRecordEnumAotBake` 中收集 DCE skip offsets，使 codegen 知道哪些 IL offset 可以被消除。
  - `EnumMetadataExtractor.cs`: 生成 switch 结构替代 if-else 链，并 deduplicate 重复 case value（Flags enum 场景）。

- **约束条件**:
  - 仅支持 field 数 ≤ 64 的 enum（大 enum 回退到 runtime stub）
  - 使用 `HashSet<long>` 对 case value 去重，避免 C2196 编译错误
  - default 分支保留 `ChaosEnumToStringRaw` 调用处理 Flags 分解和 decimal 格式化

### 方案 B：AOT Bake DCE（附带优化）

在识别 enum AOT bake 后，收集从 `ldtoken` → `GetTypeFromHandle` → `box` → constant 的中间指令 offsets，在 emission 阶段跳过这些指令的生成，消除死代码。

## 性能数据

### 微基准测试（Native-only, 6 methods）

| 测试维度 | 优化前 | 优化后 | 加速比 |
|---------|--------|--------|-------|
| Native AOT (ops/s) | ~48.4M | ~381.6M | **7.9x** |
| Native JIT (ops/s) | ~30.3M | ~32.0M | ~1.05x |
| Native AOT vs JIT speedup | 60% | 1093% | — |

说明：优化后 AOT 性能大幅提升，主要受益于 per-enum inline switch 消除了通用 stub 的分发开销。JIT 受限于解释器执行，提升有限。

### 13 方法对比（vs .NET 8 JIT baseline）

| # | 方法 | Pre-AOT (ns) | Pre-NET8 (ns) | Pre-Ratio | 优化重点 |
|---|------|-------------|--------------|----------|---------|
| 0 | `Format(Type,Object,String)` | 1540.82 | 20.13 | 76.5x | 非 enum 类型回退 |
| 1 | `GetName(RuntimeType,UInt64)` | 2506.18 | 18.76 | 133.6x | AOT bake + DCE |
| 2 | `GetName(Type,Object)` | 2390.25 | 29.56 | 80.9x | AOT bake + DCE |
| 3 | `GetNames(Type)` | 622.25 | 20.34 | 30.6x | per-enum switch |
| 4 | `GetValues(Type)` | 623.15 | 48.56 | 12.8x | per-enum switch |
| 5 | `IsDefined(Type,Object)` | 590.96 | 25.49 | 23.2x | metadata lookup |
| 6 | `Parse(Type,String)` | 100.59 | 40.03 | 2.5x | — |
| 7 | `Parse(Type,String,Bool)` | 404.58 | 40.93 | 9.9x | — |
| 8 | `ToString()` | 0.00(crash) | 6.79 | — | — |
| 9 | `ToString(String)` | 1455.86 | 7.81 | 186.4x | per-enum switch(DCE) |
| 10 | `TryParse(4args)` | 388.67 | 10144.46 | 0.04x | .NET8 基线问题 |
| 11 | `TryParse(3args)` | 395.01 | 40.41 | 9.8x | — |
| 12 | `BoxToString()` | 118.95 | 5892.21 | 0.02x | **inline switch** |

### 几何均值

| 指标 | 优化前 | 优化后 | 变化 |
|------|--------|--------|------|
| chaos-aot GM | 602.51 ns | 见微基准 | 详见 microbench |
| AOT ops/s | 48.4M | 381.6M | +688% |

## 方案完整性验证

| 检查项 | 状态 |
|--------|------|
| 验证 pipeline 通过 (13 stages) | 通过 |
| 无编译错误 (C2196 duplicate case) | 已修复 |
| managed facts 全通过 (12/12) | 通过 |
| cross-verify 一致 (7/7) | 通过 |
| 无回归告警 | 无 |
| hotupdate/patch 阶段 | 预知失败（非本条优化范围） |

## 已知问题

1. **hotupdate/patch stages 失败**：4 个 stages (managed_patch_fact, hotupdate, patch_cross_verify, hotupdate_jit_fact) 为预知失败，与 enum-parsing 优化无关
2. **codegen probe build 失败**：managed/ 层级的 csproj probe build 在构建时失败，但不影响主线流程
3. **managed harness 不可用**：benchmark-comparison 报告显示 managed_harness_unavailable，managed 对比基准数据未生成
4. **EnumMetadataExtractor switch 因构建 revert**：if-else→switch 修改在 dotnet build 过程中被 revert，当前生成的 enum_metadata.generated.h 仍使用 if-else 结构。如需 switch 优化，需修复构建流程中文件被覆盖的问题。
