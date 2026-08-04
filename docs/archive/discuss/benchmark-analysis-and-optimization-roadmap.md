# Benchmark Data Analysis & Optimization Roadmap

## 一、Benchmark 数据正确性审查

### 1.1 数据采集方法

| 技术 | 来源 | 迭代策略 | 归一化 |
|------|------|---------|--------|
| **chaos-aot** | AOT entry.exe --benchmark-all | 自适应校准 (目标 ~50ms) | ✅ 已除以 iterations |
| **chaos-jit** | JIT entry-jit.exe --benchmark-all | 自适应校准 | ✅ 已除以 iterations |
| **net8-jit** | dotnet exec BenchmarkRunner, 1000 iters | 固定 1000 | ✅ 已除以 iterations |
| **net10-jit** | 同上 | 固定 1000 | ✅ 已除以 iterations |

### 1.2 已知数据质量问题

| 问题 | 严重程度 | 状态 |
|------|---------|------|
| Brotli -326528% outlier | 高 | 数据未被 managed_benchmark 刷新 → 旧基线残留 |
| Asn1 -2246% (6 methods) | 中 | 真实差距 (value-type struct 方法) |
| CoreLib -138% (3083 methods) | 低 | 真实聚合值 |
| Parallel -306% (62 methods) | 低 | 真实差距 (Task 调度) |
| System.Net.Http 无数据 | 高 | CombinedSubjects net8.0 构建失败 |

### 1.3 数据可靠性评估

```
✅ 可信 (归一化+MAX elapsed):
  System.Linq -112.5%, 339 methods
  System.Private.CoreLib -138.1%, 3083 methods  
  System.Collections.Immutable -230.8%, 4 methods
  System.Runtime.InteropServices -874.8%, 26 methods
  System.Threading.Tasks.Parallel -306.2%, 62 methods

⚠️ 部分可信 (基线覆盖不完整):
  System.Formats.Asn1 -2246.6%, 6 methods (样本太小)
  System.IO.Compression.Brotli -326528.9% (未刷新)

❌ 不可信 (无基线):
  System.Net.Http, System.Security.Cryptography, System.Net.Sockets, 等
```

---

## 二、性能瓶颈根本原因分析

### 2.1 AOT vs JIT 架构差异

```
┌────────────────────────────────────────────────────────┐
│                    .NET JIT (CoreCLR)                  │
│                                                        │
│  IL → JIT Compiler → Native Code (直接生成机器码)       │
│  ├── Tier 0: Quick JIT (minimal optimization)          │
│  ├── Tier 1: Full JIT (optimized)                      │
│  └── Tier 3/4: Optimized + intrinsics (SIMD, LZCNT)   │
│                                                        │
│  ✅ 热路径自动识别                                      │
│  ✅ 内联决策在 JIT 阶段                                 │
│  ✅ 直接生成 SIMD 指令                                  │
│  ✅ P/Invoke 直接 call, 无中间层                         │
└────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────┐
│              Chaos IL2CPP AOT (当前)                   │
│                                                        │
│  IL → Codegen → C++ → NativeC Compiler → Machine Code  │
│  ├── AOT 编译所有可达方法                                │
│  ├── 外部运行时表 (kChaosExternalRuntimeFnTable)          │
│  ├── 解释器 (FastExecute / RegisterExecute)              │
│  └── Tiered JIT (T1→T4, 有限的内置)                     │
│                                                        │
│  ❌ 跨 assembly 调用走外部运行时表                        │
│  ❌ SIMD 方法走解释器或 sentinel 返回值                   │
│  ❌ P/Invoke 有 ~64ns 分派开销                           │
│  ❌ C++ 编译器看不到 IL 级优化信息                        │
└────────────────────────────────────────────────────────┘
```

### 2.2 三层性能损耗链

```
第一层: ABI 间接 (已修复 P0-1-A/B)
  问题: abi->object_get_type_info() + vtable_resolve()
  修复: inline chaos_object_get_type_info() + vtable[slot]
  收益: ~5-15ns/call

第二层: SEH 开销 (已修复 P0-1-C)
  问题: catch(...) 触发 MSVC SEH, 每方法 50-100 指令
  修复: 跳过无 callee 方法的 try/catch; catch(managed_exception&)
  收益: subject wrapper 5x (51ms→10ms)

第三层: 外部运行时表分派 (部分修复)
  问题: O(n) strcmp + PalTryCallNoExcept + 解释器 fallback
  修复: O(1) hash 查找 (已做); 原生桩函数 (IsPow2 已做)
  收益: 11,800x (IsPow2); Marshal 方法待修复
```

### 2.3 各 assembly 根因归类

| 类别 | Assembly | 根因 | 修复策略 |
|------|----------|------|---------|
| Subject wrapper | runtime-interop, globalization, numerics | try/catch + SEH + 解释器 fallback | P0-1-C + SubjectFactory (已做) |
| P/Invoke | Marshal, Interop | 外部运行时表分派 (hash 已做, 原生函数注册待做) | 注册更多原生桩函数 |
| SIMD | Vector128, Vector256, BitOperations | 无 AOT SIMD → 解释器 fallback | 原生桩函数 (IsPow2 已做) |
| 跨 assembly | CoreLib text, reflection | 方法在外部运行时表中无对应 AOT 函数 | hotpatch_resolve 注册 |

---

## 三、横向参考 CoreCLR / Unity IL2CPP

### 3.1 CoreCLR (dotnet/runtime) 方案

```
CoreCLR JIT:
  1. 方法调用: call/callvirt → JIT 生成直接 call/indirect call
     → 无"外部运行时表"中间层
     → 无 ABI 转换开销
     → P/Invoke: JIT 直接生成 call [DllImport 地址]
  
  2. Intrinsics: JIT 识别方法 token → 生成 CPU 指令
     → BitOperations.PopCount → LZCNT/POPCNT
     → Vector128.Add → vpaddd / vaddps
     → 无需运行时表, 无需 ABI 转换
  
  3. 分层编译 (Tiered JIT):
     T0: 快速生成 (无优化)
     T1: 完全优化 (内联, 常量传播)
     T3/4: 额外优化 (边界检查消除, 循环优化)
```

### 3.2 Unity IL2CPP 方案

```
Unity IL2CPP:
  1. 翻译管线: IL → C++ → 平台编译器
     → 与 Chaos IL2CPP 相同
     → 但 Unity 的 C++ 代码质量更高 (更少中间层)
  
  2. P/Invoke: 直接生成 extern "C" 函数调用
     → 无外部运行时表
     → 链接时直接绑定
  
  3. Generic sharing: 共享泛型代码实例
     → 减少 codegen 方法数
     → 更好的 icache 利用率
  
  4. 对比 Chaos IL2CPP 优势:
     - 无等效 external_runtime_fn_table
     - SIMD: 生成 __m128i C++ 代码直接用
     - P/Invoke: DLL import 在 codegen 阶段绑定
```

### 3.3 差距分析

```
核心差距1: 外部运行时表中间层
  CoreCLR:   call → native code (无中间层)
  Unity IL2CPP: call → C++ function (无中间层)  
  Chaos IL2CPP: call → kChaosExternalRuntimeFnTable → interpreter (2层)

核心差距2: SIMD intrinsic 处理
  CoreCLR:   JIT 直接生成 vpaddd / vaddps
  Unity IL2CPP: 生成 _mm_add_epi32() C++ 调用
  Chaos IL2CPP: 解释器 fallback (100-1000x 慢)

核心差距3: P/Invoke 绑定
  CoreCLR:   JIT 直接 call native addr
  Unity IL2CPP: 编译时 extern "C" 链接
  Chaos IL2CPP: 运行时表 + strcmp 查找
```

---

## 四、最优解决方案

### 4.1 架构目标

```
性能最好:   消除中间层, AOT 直接生成原生代码
架构完美:   清晰的分层, codegen 直接输出最佳 C++
可维护强:   方案集中, 无散落在多个文件中的特殊处理
热更适配:  外部运行时表保留但仅做 fallback, 主力路径避开它
```

### 4.2 分层优化方案

```
Phase 1: 消除代码生成中的外部运行时表依赖 (当前阶段)
  ├── 注册原生桩函数到 kChaosExternalRuntimeFnTable
  │   ├── BitOperations: IsPow2 ✅ → PopCount/LeadingZeroCount/Log2 ✅
  │   └── Vector128: Add/Sub/Mul/And/Or/Xor (simd_stubs.h 已写, 待注册)
  └── 预期收益: 消除解释器 fallback 性能损失

Phase 2: 在 codegen 中内联已知 intrinsic (下阶段)
  ├── codegen 识别 SIMD/Intrinsic 方法签名
  ├── 直接生成编译器 intrinsic C++ 代码
  │   └── _mm_add_ps, __builtin_popcountll 等
  └── 预期收益: 消除外部运行时表分派 (再快 ~64ns/call)

Phase 3: 消除跨 assembly 调用的外部运行时表 (架构升级)
  ├── codegen 为已知方法生成直接 extern "C" 调用
  ├── kChaosExternalRuntimeFnTable 仅做动态加载 fallback
  └── 预期收益: 消除所有 P/Invoke 及跨 assembly dispatch 开销
```

### 4.3 优先级路线图

```
🎯 P0 (当前): 注册更多的原生桩函数
  BitOperations: PopCount ✅, LeadingZeroCount ✅, Log2 ✅
  Vector128: Add/Sub/Mul/And/Or/Xor (simd_stubs.h 已提供)
  Marshal: GetLastPInvokeError ✅, SetLastPInvokeError
  
🎯 P1 (短期): Codegen-level intrinsic 识别
  codegen 匹配方法 token → 直接生成 C++ intrinsic
  无需外部运行时表, 无需 ABI 适配
  
🎯 P2 (中期): 跨 assembly 调用直接绑定
  分析调用图 → 已知目标 → 直接 extern "C" 调用
  
🎯 P3 (长期): 消除 kChaosExternalRuntimeFnTable
  仅保留 interpreter fallback 场景
  主力路径: direct call → C++ → native
```

---

## 五、总结

```
Benchmark 数据可信度: 中高 (归一化后, 23/27 assembly 有基线)
最大性能差距来源:
  1. subject wrapper 开销 (-138% CoreLib 中 ~95% 来自此) — 已修复
  2. 外部运行时表中间层 (~64ns/P/Invoke call)
  3. SIMD/Intrinsic → 解释器 fallback (100-1000x)
  4. 跨 assembly 调用绕行

核心方案: 两阶段消除外部运行时表依赖
  Phase 1: 注册更多原生桩函数 (当前, IsPow2 已验证 11,800x)
  Phase 2: Codegen-level intrinsic 识别 (直接生成 C++ intrinsic)
  
参考:
  CoreCLR:  无外部运行时表, JIT 直接生成
  Unity IL2CPP: 无外部运行时表, C++ 编译器承担
  Chaos IL2CPP 独有: 外部运行时表 + 解释器 fallback
