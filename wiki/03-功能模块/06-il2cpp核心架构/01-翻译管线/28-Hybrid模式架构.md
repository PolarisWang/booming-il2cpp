# Hybrid 模式架构

> Hybrid（AOT↔JIT 混合编译模式）是 Chaos IL2CPP 三模式编译体系中的第三种模式，兼顾 AOT 的冷启动性能与 JIT 的热点优化能力。方法启动时直接执行 AOT 原生代码，热点计数达到阈值后自动触发后台 JIT 编译，编译完成后无锁切换执行路径。

---

## 整体架构

### 定位

Hybrid 模式在性能最优（P1）和 HotUpdate 支持（P3）之间提供平衡选择：

- **冷执行**：直接调用 AOT 编译的 C++ 函数体，零解释器开销（与纯 AOT 模式同等性能）
- **热点升级**：方法调用计数达到阈值后，自动触发 StackInterpreted JIT 编译（图着色寄存器分配），编译完成后 atomic swap 函数指针
- **热更兼容**：AOT 函数体保留原址，可通过 hotupdate.patchdata 替换整个方法

### 三模式对比

| 维度 | AOT | JIT | Hybrid |
|------|-----|-----|--------|
| CLI 选项 | `--mode aot`（默认） | `--mode jit` | `--mode hybrid` |
| 冷执行 | AOT C++ 函数 | JitPrecode trampoline（触发 Quick JIT） | AOT C++ 函数 |
| 热点执行 | AOT C++ 函数（不变） | StackInterpreted 优化编译 | StackInterpreted 优化编译 |
| 启动性能 | 最快（零运行时开销） | 最慢（每方法首次需 JIT） | 快（AOT 直接执行） |
| 峰值性能 | 较好（AOT） | 最好（PGO+Tier 1） | 最好（Tier 1 同 JIT） |
| 热更方式 | C++ 替换 | 代码指针替换 | AOT 保留 + 代码指针替换 |
| 内存占用 | 最低 | 方法越多占用越大 | 中等（AOT+JIT 双输出） |

### 数据流

```
┌──────────────┐    首次调用     ┌──────────────────┐
│  AOT C++     │ ←──────────── │  HybridPrecode    │
│  函数体      │                │  call_counter=N   │
└──────────────┘                │  aot_entry=@AOT   │
                                └────────┬─────────┘
                                         │
                                counter fetch_sub
                                (每调用递减1)
                                         │
                          ┌──────────────┴──────────────┐
                          │ old==1 ?                    │
                          │ (计数器归零触发)             │
                          └──────────────┬──────────────┘
                                         │ yes
                                         ▼
                          ┌──────────────────────────────┐
                          │  Compile(ir, config)         │
                          │  → JitMethod                 │
                          │  → CAS: Uncompiled→Compiled  │
                          │  → direct_ptr = compiled.code│
                          └──────────────┬───────────────┘
                                         │
                          ┌──────────────▼───────────────┐
                          │  后续调用经由 direct_ptr      │
                          │  直接执行 JIT 编译代码        │
                          └──────────────────────────────┘
```

---

## HybridPrecode 结构体

定义在 `src/native/jit/jit_precode.h`：

```cpp
struct HybridPrecode {
    // ── 状态控制 ──
    std::atomic<uint32_t> state;        // kUncompiled / kCompiling / kCompiled / kFailed
    std::atomic<uint32_t> call_counter; // 递减计数器，初始化为 kT4Threshold(30)

    // ── AOT 路径 ──
    void* aot_entry;                    // AOT 函数入口（始终保留）
    HotpatchEntryV0* entry;             // HotpatchEntryV0（含 direct_ptr）
    void* trampoline;                   // Hybrid trampoline（15 bytes RWX）

    // ── JIT 路径 ──
    RegisterMethod ir;                  // 寄存器分配 IR（编译输入）
    CompileConfig config;               // 编译配置
    JitMethod* compiled;                // JIT 编译产物（nullptr=未编译）
};
```

### 核心常量

| 常量 | 值 | 说明 |
|------|-----|------|
| `kT4Threshold` | 30 | AOT→JIT 升级阈值。前 29 次调用走 AOT 路径，第 30 次触发 JIT 编译 |
| `kPrecodeUncompiled` | 0 | 初始状态，AOT 路径活跃 |
| `kPrecodeCompiling` | 1 | 正在编译（CAS 保护，仅一个线程进入编译） |
| `kPrecodeCompiled` | 2 | JIT 编译完成，后续调用走 JIT 路径 |
| `kPrecodeFailed` | 3 | 编译失败，可重试 |

---

## Dispatch 状态机

`HybridStubDispatchImpl`（`src/native/jit/jit_precode.cpp`）使用两级快路径优先的 CAS 状态机：

```
入口
  │
  ├─ 检查 state == kPrecodeCompiled?
  │   ├─ YES → return compiled->code      ← 快路径（memory_order_acquire）
  │   └─ NO  → 继续
  │
  ├─ call_counter.fetch_sub(1, relaxed)
  │   │
  │   ├─ old != 1? (计数器未归零)
  │   │   └─ entry 有效? → return entry->direct_ptr (trampoline)
  │   │   └─ entry 无效? → return aot_entry
  │   │
  │   └─ old == 1? (计数器归零, 触发编译)
  │       │
  │       ├─ CAS state: Uncompiled → Compiling
  │       │   ├─ 成功? → 执行 Compile(ir, config)    ← 编译线程
  │       │   │          ├─ 编译成功 → state = Compiled, direct_ptr = compiled->code
  │       │   │          └─ 编译失败 → state = kFailed, counter = 1 (可重试)
  │       │   └─ 失败? → 另一线程已进入编译，自旋等待 state
  │       │              (spin loop load acquire)
  │       │
  │       └─ 返回 AOT entry（编译线程）或 trampoline（等待线程）
```

### 关键设计决策

1. **计数器先递减后判断**：`fetch_sub` 在 CAS 之前，确保计数器归零的信号不会被并发丢失
2. **CAS 保护编译入口**：只有拿到 `Uncompiled→Compiling` 转换的线程执行编译，其他线程自旋等待
3. **失败恢复**：编译失败后 `state = kFailed`，`counter = 1`，下次调用重新触发编译（先置 IR 再调用）
4. **entry/direct_ptr 模式**：当 `entry` 非空时，dispatch 返回 `entry->direct_ptr`（trampoline），由 trampoline 再次进入 dispatch；当 `entry` 为空时直接返回 `aot_entry`

### Trampoline 布局

Hybrid trampoline（15 bytes，PrecodeArena 分配的 RWX 内存）：

```asm
mov r10, imm64      ; 10 bytes: 加载 HybridPrecode* 地址
jmp shared_entry     ; 5 bytes: 跳转到共享分派入口
```

共享入口（每个 `PrecodeArena` 实例一个）保存 GPR/FPR 参数寄存器，调用 `HybridStubDispatchImpl`，恢复参数寄存器，tail-call 到返回的地址。

---

## 性能特征

### 基线数据（2026-05, Windows x64 Debug 构建）

| 指标 | P50 | P99 | 说明 |
|------|-----|-----|------|
| 升级延迟 | 15.3 µs | 1284 µs | 从第 1 次调用到 JIT 编译完成（含 30 次 AOT 调用 + 编译） |
| 纯编译时间 | 16.1 µs | 27.0 µs | Compile() 本身耗时（不含 dispatch 开销） |
| AOT dispatch 开销 | 0.02 µs/调用 | — | 计数器递减 + trampoline 跳转 + AOT 返回 |
| 编译后 dispatch 开销 | 0.013 µs/调用 | — | atomic load + 直接跳转到 JIT 代码 |
| Trampoline 分配 | 0.0 µs | 2.6 µs | PrecodeArena 内从预分配 RWX 页分配 |

### 多方法批量升级

| 批量大小 | 总耗时 | 每方法平均 |
|----------|--------|-----------|
| 10 | 156 µs | 15.6 µs |
| 50 | 844 µs | 16.9 µs |

升级延迟线性扩展，多方法升级无额外串行化开销。

---

## 验证体系

### 测试套件

Hybrid 工业化交付了 4 个专用测试套件（共 41 个测试用例）：

| 套件 | 文件 | 用例数 | 覆盖范围 |
|------|------|--------|----------|
| test_jit_hybrid_mode | `jit_hybrid_mode_test.cpp` | 19 | 核心验证：阈值边界、计数器初始值、aot_entry 为 null、PGO 交互、编译失败恢复、direct_ptr 更新、null dispatch |
| test_jit_hybrid_e2e | `jit_hybrid_e2e_test.cpp` | 8 | 端到端集成：AOT→Upgrade→JIT 三阶段、多方法、计数器变化、失败恢复、并发 AOT+JIT、aot_entry 持久性 |
| test_jit_hybrid_race | `jit_hybrid_race_test.cpp` | 7 | 竞争测试：ABA 模式、并发 reset+dispatch、并发编译+读取、状态转换竞争、内存序压力、trampoline 分配器竞争 |
| bench_hybrid | `bench_hybrid.cpp` | 7 | 性能基准：升级延迟 P50/P99、纯编译时间、dispatch 开销、trampoline 分配延迟、批量升级吞吐量 |

### Foundation-DLL 集成

Hybrid 验证通过 Foundation-DLL stage 14 集成管线，使用 `hybrid_verification_runner.py` 自动完成三阶段验证：

```
阶段 1 (AOT)      → 验证 29 次 AOT 调用返回 AOT 结果 (42)
阶段 2 (Upgrade)  → 第 30 次调用触发 JIT 编译
阶段 3 (JIT)      → 验证后续调用返回 JIT 编译结果 (77)
```

验证报告输出到 `hybrid-verification-report.json`。

---

## 文件清单

### 生产代码 (`src/native/jit/`)

| 文件 | 职责 |
|------|------|
| `jit_precode.h` | HybridPrecode / JitPrecode 结构体定义、PrecodeArena RWX 页管理器 |
| `jit_precode.cpp` | HybridStubDispatchImpl / JitStubDispatchImpl 实现、PrecodeArena::AllocateHybridTrampoline |
| `jit_engine.h/cpp` | Compile() 入口、CompileConfig / CompileTier 定义 |
| `jit_method.h/cpp` | JitMethod 编译产物结构体 |

### 测试代码 (`testing/src/native/jit/`)

| 文件 | 职责 |
|------|------|
| `jit_hybrid_mode_test.cpp` | HybridPrecode 核心验证（19 tests） |
| `jit_hybrid_e2e_test.cpp` | 三阶段端到端集成测试（8 tests） |
| `jit_hybrid_race_test.cpp` | 多线程竞争测试（7 tests） |
| `bench_hybrid.cpp` | 性能基准（7 benchmarks） |

### Foundation-DLL (`testing/foundation-dll/`)

| 文件 | 职责 |
|------|------|
| `System.Formats.Asn1/hybrid/hybrid_verification_runner.py` | 三阶段验证运行器 |
| `System.Formats.Asn1/hybrid/capability-family-contract.json` | 能力族合约定义 |
| `System.Formats.Asn1/hybrid/native/CMakeLists.txt` | 族内混合测试构建 |

---

## 已知限制

1. **单 Arena 非线程安全**：`PrecodeArena::AllocateHybridTrampoline` 非线程安全，多线程并发分配需要外部加锁
2. **编译线程阻塞 dispatch**：编译期间（~16 µs），其他线程调用同一方法时自旋等待 state 转换
3. **失败重试需外部驱动**：编译失败后 state 切换到 `kFailed`，需要调用方先设置 `ir` 再触发重新编译
4. **仅 x64**：HybridPrecode trampoline 和 dispatch 实现仅支持 x64 架构
