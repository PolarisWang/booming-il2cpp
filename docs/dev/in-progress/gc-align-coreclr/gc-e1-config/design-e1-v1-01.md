# Design — GC-E1：配置旋钮体系（对齐 CoreCLR + 跨平台 + AOT/JIT）

> 版本：design-e1-v1-01
> 创建：2026-08-10
> 上游：`docs/dev/in-progress/gc-align-coreclr/roadmap-v1-01.md`（Phase 2, GC-E1）
> 对标基准：`D:\OpenSource\dotnet\runtime\src\coreclr\gc\gcconfig.h`（`GC_CONFIGURATION_KEYS` 宏体系）

---

## 1. 对齐 CoreCLR 的核心（gcconfig.h 的宏式旋钮表）

CoreCLR 用**单一宏列表** `GC_CONFIGURATION_KEYS` 声明全部 72 个配置旋钮，每项是：

```cpp
KIND_CONFIG(name, env_key, managed_key, default, doc)
// KIND ∈ {BOOL, INT, STRING}
```

此单一列表驱动三件事：
1. 访问方法 `GCConfig::Get{name}()`（枚举名 → getter）
2. env 变量查找（`gcServer`, `GCHeapHardLimit`...）
3. managed runtimeconfig 开关（`System.GC.Server`...）

**跨平台关键**：CoreCLR 经 `GCToOSInterface::GetEnvironmentVariable(env_key)` 读 env —— 平台无关。

## 2. CRAG 的差异与适配

CRAG **没有** CoreCLR 的 managed-config-loader（`runtimeconfig.json`），所以落地为：

| CoreCLR 源 | CRAG 等价 | 跨平台 |
|-----------|----------|--------|
| env 变量 | `CHAOS_GC_<env_suffix>` 经 `std::getenv` | ✅ std 标准，Win/Linux/macOS/iOS/Android 一致 |
| managed 开关 | native API `chaos_gc_config_set_*`（AOT/JIT codegen 可调用做程序化调参） | ✅ 纯 C 接口，AOT/JIT 都能链 |
| `GCConfig::Get{name}()` | 内部 `GcConfig::Get{name}()`（`gc_config.h`） | ✅ |

**跨平台 / AOT-JIT 约束如何满足**：
- 纯 `std::getenv` + 纯 C 访问接口，无平台特定代码 → **跨平台**。
- AOT（静态 codegen）和 JIT 生成的代码都能调 `chaos_gc_config_*` 原生符号 → **AOT/JIT 适配**（二者不冲突，同一运行时单例）。

## 3. 实现结构

### `src/native/runtime-core/gc/gc_config.h`
CoreCLR 式宏表（首批 ~20 个核心旋钮）：

```cpp
#define CHAOS_GC_CONFIGURATION_KEYS \
    INT_CONFIG(DefaultNurserySize,  "NurserySize",   kDefaultYoungRegionSize, "nursery region size (bytes)") \
    INT_CONFIG(DefaultGen1Size,     "Gen1Size",       0, "gen1 survivor size (0=default)") \
    INT_CONFIG(MaxTlabAlloc,        "MaxTlabAlloc",   kMaxTlabAlloc, "max in-TLAB allocation (bytes)") \
    INT_CONFIG(LohThreshold,        "LOHThreshold",   kLohThreshold, "large-object-heap threshold (bytes)") \
    INT_CONFIG(ParallelMarkWorkers, "ParallelMarkWorkers", kMaxParallelMarkWorkers, "max parallel-mark workers") \
    INT_CONFIG(HeapHardLimitMB,     "HeapHardLimitMB", 0, "hard heap limit (MB, 0=disabled)") \
    INT_CONFIG(HeapSoftLimitMB,     "HeapSoftLimitMB", 0, "soft heap limit (MB, 0=disabled)") \
    ...
```

宏展开为：`GcConfig::Get{name}()` getter + 内部存储 + env 覆盖。

### `gc_config.cpp`
- `GcConfig::Initialize()`：读 `CHAOS_GC_<env_suffix>` 覆盖默认（`std::getenv`，缺失 → 用编译期默认）。
- AOT/JIT 程序化调参：`extern "C" void chaos_gc_config_set_<name>(...)`（经 native_types/chaos API 导出）。

### 接线
- `InitYoungGeneration()`：nursery/gen1 大小、TLAB、硬软限从 `GcConfig` 读，替代硬编码/编译期 `#if`。
- LOH 阈值、parallel-mark workers 同理。

## 4. 验收（exit criteria）

- [ ] `chaos_runtime_core` Debug 编译通过
- [ ] 设 `CHAOS_GC_NurserySize=...` 后 `InitYoungGeneration` 用新值（env 覆盖生效）
- [ ] 设 `CHAOS_GC_HeapHardLimitMB=...` 后续 G_Scheduler().GetHardLimit() 反映（替代编译期 #if）
- [ ] native `chaos_gc_config_set_*` 程序化调参可达（AOT/JIT codegen 可链）
- [ ] GC 确定性单测批无回归

## 5. 备选

若宏展开复杂度高，降级为手写 `struct GcConfig { 字段; Initialize(); }`（无宏），仍是 env + native API 双通道。推荐宏式（对齐 CoreCLR 架构可读性）。
