# Tier 升级路径统一 — 设计 v1

## 问题

`TryTierUpgrade` (entry_direct.cpp:373) 和 `TryFastOsrPromotion` (fast_dispatch.cpp:2679) 各自维护了 T1→T2 和 T2→T3 的 CAS+阈值判断逻辑。重复代码可能导致升级时序不一致和双重编译。

```
重复范围：
  T1→T2 CAS ✓ + AllocateRegisters ✓   ← 两边都有
  T2→T3 CAS ✓ + EnqueueOptimization ✓  ← 两边都有
  T3→T4 CAS (空) + Compile+OSR          ← 仅 TryFastOsrPromotion
```

## 方案

抽取共享决策函数 `EvaluateTierPromotion`，让入口和 FastExecute 共用同一套 CAS+阈值逻辑，各自保留执行权。

### PromotionAction 枚举

```cpp
enum class PromotionAction {
    kNone,          // 无升级
    kToT2,          // T1→T2: caller 执行 AllocateRegisters
    kToT3,          // T2→T3: caller 执行 EnqueueOptimization
    kToT4,          // T3→T4: caller 执行 Compile, 设 cached_native_method
    kOsrReady,      // kJitted + OSR entry 可用: caller 执行 OSR 帧迁移
    kNativeReady,   // kJitted, 无 OSR entry
};
```

### EvaluateTierPromotion 流程

```
输入: PatchMethod*, call_count
输出: PromotionAction, tier_state 可能被 CAS 修改

1. tier = pm->tier_state.load(acquire)

2. if tier == kStackInterpreted && call_count >= GetAdaptiveT1Threshold():
     CAS kStackInterpreted → kRegisterLowering
     成功 → return kToT2
     失败 → return kNone

3. if tier == kRegisterMapped && call_count >= GetAdaptiveT2Threshold():
     CAS kRegisterMapped → kOptimizeLowering
     成功 → return kToT3
     失败 → return kNone

4. if tier == kOptimizedRegister && call_count >= kJitThreshold + backoff:
     CAS kOptimizedRegister → kJitted
     成功 → return kToT4
     失败 → return kNone

5. if tier >= kJitted:
     nm = pm->cached_native_method
     if nm && nm->code && nm->osr_entry_offset:
       return kOsrReady
     elif nm && nm->code:
       return kNativeReady
     否则 return kNone

6. return kNone
```

### TryTierUpgrade 新逻辑

```cpp
void TryTierUpgrade(PatchMethod* pm, uint32_t call_count, ...) {
    auto action = EvaluateTierPromotion(pm, call_count);
    switch (action) {
    case kToT2:
        OptimizeToTier2(pm);
        pm->tier_state.store(kRegisterMapped, release);
        break;
    case kToT3:
        TierManager::Get().EnqueueOptimization(pm);
        break;
    case kToT4:
        // 预编译 T4，设置 cached_native_method
        // 等 FastExecute 回边检测到 kOsrReady 后做 OSR
        CompileAndCache(pm);
        break;
    default:
        break;
    }
}
```

### TryFastOsrPromotion 新逻辑

```cpp
bool TryFastOsrPromotion(FastFrame& frame) {
    auto* pm = frame.patch_method;
    auto call_count = pm->call_count.load(relaxed);
    auto action = EvaluateTierPromotion(pm, call_count);
    
    switch (action) {
    case kToT4:
        // 兜底: entry 侧没触发编译, 或编译在另一个线程进行中
        CompileAndCache(pm);
        break;
    case kOsrReady:
        // OSR 帧迁移到 native 代码
        return ExecuteOSR(frame, pm->cached_native_method);
    case kNativeReady:
        // 已 T4 但无 OSR entry, 继续解释执行
        break;
    default:
        break;
    }
    return false;
}
```

### T3→T4 CompileAndCache 封装

当前的 T3→T4 编译逻辑分散在两处，需要提取：

```cpp
static bool CompileAndCache(PatchMethod* pm) {
    auto* rm = pm->cached_optimized_reg_method;
    if (!rm || rm->instructions.empty()) return false;
    
    auto* nm = jit::Compile(*rm, cfg);
    if (!nm) {
        pm->codegen_fail_count++;
        if (pm->codegen_fail_count >= kMaxCodegenFailures) {
            pm->tier_state.store(kJitSkip, release);
        }
        return false;
    }
    
    pm->cached_native_method = nm;
    // 注册 GC slot map + native code section
    GcRegisterSlotMap(nm->code, nm->gc_slot_map);
    RegisterNativeCodeSection(nm->code, nm->code_size, nm);
    return true;
}
```

## 安全考虑

### 先编译后 CAS

EvaluateTierPromotion 返回 kToT4 后，调用者执行 CompileAndCache。但编译成功后到 CAS kJitted 之间可能有另一个线程也触发了编译。处理：

```cpp
// CompileAndCache 内部
uint32_t expected = kOptimizedRegister;
bool won = pm->tier_state.compare_exchange_strong(expected, kJitted);
if (!won) {
    // 另一个线程赢了编译竞赛，释放当前产物
    delete nm;
    return false;
}
// 赢了才设 cached_native_method
pm->cached_native_method = nm;
```

### Domain unload 期间的 OSR

`cached_native_method` 在 domain unload 时可能被清掉。EvaluateTierPromotion 在第 5 步检查 `pm->cached_native_method != nullptr`，如果为空则继续返回 kNone，osr 永远不触发直到下次 entry。足够安全。

## 文件变更

| 文件 | 操作 | 行数 |
|------|------|------|
| `src/native/runtime-core/tier_manager.h` | 新增 PromotionAction enum + EvaluateTierPromotion 声明 | +15 |
| `src/native/runtime-core/tier_manager.cpp` | 实现 EvaluateTierPromotion | +60 |
| `src/native/interpreter/interp_entry/entry_direct.cpp` | TryTierUpgrade 改造 | ~-40 +20 |
| `src/native/interpreter/fast_dispatch.cpp` | TryFastOsrPromotion 改造 | ~-50 +30 |

总变更约 150 行新增/120 行删除，净增 ~30 行。

## 验证

1. **行为等价**：现有 JIT 测试全过（bench_jit_vs_aot、jit_mode_test、jit_abi_test 等）
2. **调试断言**：Tier upgrade 时序不变（触发点在相同位置）
3. **双验证**：entry_direct 和 fast_dispatch 的 promotion 路径在测试中分别覆盖
4. **长期 loop 测试**: foundation-dll parsing benchmark 的热循环在修改后仍能正确触发 OSR
