---
name: dev-il2cpp-jit-expert
description: JIT 原生代码生成专家 — T4 自适应分层编译、寄存器分配、指令编码、SEH、unwind info、tier promotion、OSR
---

> ⚠️ **本文件通过 Skill 工具加载，作用是注入领域知识到当前对话上下文。**
> 本文件**不是可执行的 agent**。当前 Agent 需阅读下方"执行流程"作为实现参考。

# dev-il2cpp-jit-expert — JIT 原生代码生成专家

## 领域边界

### 我负责的

**T4 原生代码生成管线**（`src/native/jit/`）：
- 寄存器分配（Linear Scan，参考 CoreCLR 实现）
- x64/ARM64 指令编码（SSE/AVX/NEON）
- SEH（结构化异常处理）：`__try/__except` 的 Win64 / Linux 实现
- Unwind Info（`.pdata` / `.xdata`）：Win64 UNWIND_CODE 编码
- Tier Promotion：call_count 驱动的 0→8 状态机
- OSR（On-Stack Replacement）：T4 deopt → interpreter 回退
- Precise GC：GC root 枚举、safepoint 插入、stack map 生成
- Inline Cache：PIC（Polymorphic Inline Cache）链式查找

### 我不负责的

- **AOT codegen**（C# 翻译管线）→ 超出范围，标记 remaining，原因：需要 CodeGen 域知识
- **Interpreter VM**（FastExecute/RegisterExecute）→ 超出范围，标记 remaining，原因：需要运行时域知识
- **GC 分配/写屏障** → 超出范围，标记 remaining，原因：需要 GC 域知识
- **ARM64 JIT 完整实现**（x64→ARM64 交叉编译）→ 超出范围，标记 remaining，原因：需要平台域知识

---

## 触发条件

- 从 `dev-il2cpp-core-agent` 路由（关键词：JIT、T4、native codegen、register allocator、tier promotion、OSR、SEH、unwind、inline cache、PIC、precode）
- JIT 编译失败、生成的机器码崩溃、unwind info 编码错
- Tier 升级/降级逻辑故障

---

## 执行流程

### Step 1: 加载 JIT 语境

```
1. 读取 wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/27-T4原生代码生成器架构.md
2. 读取 wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/25-JIT-UNWIND-INFO编码修复记录.md
3. 确认当前 JIT 状态:
   - Quick JIT (T1): precode 系统 → ✅ 已完成
   - Full JIT (T3/T4): register allocator + SEH → ✅ 已完成
   - Tier promotion: call_count-based, 0→8 状态机 → ✅ 已完成
   - OSR (On-Stack Replacement): T4 deopt → interpreter → ✅ 已完成
   - ARM64: 🔄 基础支持, QEMU 通过, 性能未优化
```

---

## 知识域

### 核心源文件

| 文件 | 行数 | 职责 | CoreCLR 对标 |
|------|------|------|-------------|
| `jit_engine.cpp` | 270K | JIT 入口、指令发射主循环 | `compiler.cpp` |
| `jit_engine.h` | 8K | CompileConfig、JitMethod、NativeEntry | `compiler.h` |
| `jit_helpers.cpp` | 39K | 运行时 helper（分配、类型转换、thinlock） | `jithelpers.cpp` |
| `jit_precode.cpp` | 34K | precode 桩、tier 升级桩 | `precode.cpp` |
| `jit_reg_alloc.h` | 30K | Linear Scan 寄存器分配器 | `regalloc.cpp` |
| `x64_encoder.h` | 86K | x64 指令编码（SSE/AVX/VEX） | `emitx86.cpp` |
| `arm64_encoder.h` | 69K | ARM64 指令编码（NEON） | `emitarm64.cpp` |
| `X64Encoder.h` / `Arm64Encoder.h` | 20K+29K | IEncoder 接口实现 | — |
| `IEncoder.h` | 15K | 编码器抽象接口 | — |
| `jit_seh.cpp` | 4.5K | SEH 异常处理 | `excep.cpp` |
| `WinSehHandler.cpp` | 40K | Win64 SEH handler | `excep.cpp` |
| `LinuxSehHandler.cpp` | 33K | Linux 信号→SEH 桥接 | `excep.cpp` |
| `jit_unwind.cpp` | 18K | UNWIND_CODE/.eh_frame 生成 | `unwind.cpp` |
| `jit_inline.cpp` | 14K | 方法内联 | `inline.cpp` |
| `jit_deopt.cpp` | 4.5K | OSR deoptimization | `deopt.cpp` |
| `code_buffer.h/cpp` | 8K+4K | 机器码缓冲区 | `emit.cpp` |
| `ArchTraits.h` | 4K | 平台特性（寄存器数量、调用约定） | `target.h` |

### 树形 IR（tree/ 目录）

| 文件 | 行数 | 职责 | CoreCLR 对标 |
|------|------|------|-------------|
| `jit_tree_node.h` | 13K | 树节点定义（NodeKind 40+ 种） | `gentree.h` |
| `jit_tree_builder.cpp` | 19K | IL→树形 IR 构建 | `fgbranch.cpp` |
| `jit_optimizer.cpp` | 44K | 树优化（常量折叠、死代码消除） | `optimizer.cpp` |
| `jit_cfg.cpp` | 14K | 控制流图构建 | `flowgraph.cpp` |
| `jit_linearizer.cpp` | 16K | 基本块线性化 | `lvalue.cpp` |
| `jit_intrinsics.cpp` | 11K | 硬件 intrinsic lowering | `hwintrinsic.cpp` |
| `jit_bounds_check.cpp` | 3.5K | 边界检查消除 | `rangecheck.cpp` |
| `jit_tree_mutator.cpp` | 6K | 树形 IR 变换 | `morph.cpp` |
| `jit_vn.h` | 5K | Value Numbering | `value.cpp` |

---

## 核心架构

### 四层自适应执行系统

```
T1  FastExecute      栈机 IR 直接解释        首次执行 (cold)
T2  RegisterExecute  寄存器 IR 解释          call_count 达到阈值
T3  RegisterExecute  寄存器 IR + inline 缓存  热点方法
T4  JIT              原生 x64 机器码         最热方法
```

**与 CoreCLR 对比**：
| 维度 | Chaos IL2CPP | CoreCLR |
|------|-------------|---------|
| 层级数 | 4 (T1-T4) | 3 (Quick JIT → Full JIT → Opt) |
| 解释器 | 有（T1/T2） | 无（.NET 1.0 后有但默认关闭） |
| T1→T2 | call_count 达阈值 | — |
| T2→T3 | inline cache 命中率 | — |
| T3→T4 | call_count 再升温 | call_count / 耗时采样 |
| OSR | T4 deopt → interpreter | 分层编译中的 OSR 重编译 |

### Tier Promotion 状态机

```
Tier 0 (Cold)    → T1 FastExecute
Tier 1 (Warm)    → T2 RegisterExecute
Tier 3 (Hot)     → T3 RegisterExecute + PIC
Tier 8 (JIT)     → T4 原生代码
  
  升级: call_count += Interlocked::fetch_add
  降级: PatchMethod::tier_state 标记 → 下次调用走 interpreter
  
  状态机实现: jit_precode.cpp SetupPatchMethod → CAS 0→8
  CoreCLR 对标: CallerCount → count 达到阈值 → 触发 JIT
```

### Precise GC 栈映射

```
JIT 生成的机器码中，每个 GC safepoint 位置记录:
  - 哪些栈槽/寄存器包含 GC 引用
  - 通过 StackMap 表（codegen 输出）传递给 GC

  实现: jit_engine.cpp EmitGcPoll / EmitGcStackMap
  CoreCLR 对标: GCInfo / GcSlotTable
```

### SEH（Structured Exception Handling）

```
Win64:  __try/__except → VEH handler → RtlVirtualUnwind → handler 查找
Linux:  signal → sigsetjmp → SEH handler

  实现: WinSehHandler.cpp / LinuxSehHandler.cpp
  CoreCLR 对标: ExceptionHandling::EmitHandler / excep.cpp

  关键约束:
  - FUNK_PROLOG_LEN + prologue 必须 ≤ 255 (1-byte UNWIND_CODE)
  - UNWIND_CODE 使用 nibble encoding: 4-bit unsaved + 4-bit saved
  - 先压栈的寄存器在 unwind 码数组中排在后面
```

### 寄存器分配（Linear Scan）

```
  实现: jit_reg_alloc.h (30K)
  算法: Linear Scan + live interval splitting
  寄存器数量: x64=16 GPR + 16 XMM, ARM64=31 GPR + 32 V

  与 CoreCLR 对比:
  - CoreCLR 使用 Linear Scan (RS) + 局部优化
  - Chaos JIT 使用纯 Linear Scan，没有图着色
  - 当前瓶颈: 长方法（>500 IL）的分配速度
```

---

## 已知故障模式

| 模式 | 症状 | 根因 | 修复方向 |
|------|------|------|---------|
| JIT 编译崩溃 | entry-jit.exe AV | 寄存器分配溢出错误 | 检查 live interval 计算 |
| `STATUS_INVALID_CRUNTIME_PARAMETER` | entry.exe 崩溃 | UNWIND_CODE nibble 编码反转 | 修复 EmitByte 顺序 |
| C2733（extern C 重载） | simd_stubs.cpp 编译错 | CRLF 行尾 + noexcept | LF 化 + .gitattributes |
| Tier 不升级 | 方法始终 T1 | call_count fetch 未加 | 改为 Interlocked::fetch_add |
| OSR segfault | deopt 后崩溃 | jmp_buf 未初始化 | 修复 g_chaos_fail_hook |
| JIT 生成代码慢 | 首次调用延迟 >100ms | 长方法寄存器分配 O(n²) | 优化 live interval 计算 |
| ARM64 JIT 信号崩溃 | QEMU SIGABRT | ARM64 特有的 LdcI4 zero-extend | ARM64 编码器修复 |

---

## 性能基线

| 指标 | 当前值 | CoreCLR 参考 | 说明 |
|------|--------|-------------|------|
| JIT 编译时间 | ~5µs/方法 | ~2µs/方法 | 简单方法；含优化的较长 |
| 生成代码质量 | ~0.8x native | 1.0x (baseline) | 相比 GCC -O2 |
| Tier 升温阈值 | call_count=8 | call_count=30 | 更激进的热点检测 |
| Inline 深度 | 3 层 | 5 层 | 保守策略，可调 |
| 寄存器分配 | Linear Scan | Linear Scan + 局部优化 | 缺少图着色阶段 |
| SEH 异常捕获 | ~200ns | ~100ns | 信号→seh 桥接开销 |
| OSR deopt | ~500ns | ~300ns | jmp_buf 重建 |

---

## 架构约束

1. **Tier 8 是最终状态** — JIT 编译成功后不再降级回 interpreter（除非 PatchMethod 标记）
2. **Inline 缓存使用 PIC 链** — 先查 VTable slot，再查直接函数指针
3. **Precise GC 精度** — 所有 GC 引用必须通过 StackMap 登记，不降级为保守扫描
4. **SEH 与 AOT 不同** — JIT SEH 使用 `__try/__except`，AOT 使用 CHAOS_EH_TRY 宏
5. **x64 调用约定** — Windows: Microsoft x64（rcx/rdx/r8/r9）; Linux: System V（rdi/rsi/rdx/rcx/r8/r9）
6. **ARM64 调用约定** — x0-x7 参数寄存器，x8 间接结果指针，x19-x28 callee-saved

---

## 与 CoreCLR 的对标参考

| Chaos IL2CPP JIT | CoreCLR 对应 | 差异 |
|-----------------|-------------|------|
| T1 FastExecute | — | CoreCLR 无解释器 |
| T4 JIT (jit_engine.cpp) | `Compiler::compCompile` | Chaos 更简单，无 IL 重写阶段 |
| Linear Scan (jit_reg_alloc.h) | `LinearScan::doLinearScan` | 算法类似，但 Chaos 缺少溢出权重计算 |
| x64_encoder.h | `Emitter::emitIns` / emitx86.cpp | 编码器架构不同：Chaos 用 header-only 函数 |
| SEH (WinSehHandler) | ExceptionHandling::EmitHandler | Chaos 支持 Win64/Linux 双平台 |
| Unwind Info (.pdata) | `UNWIND_INFO` / `funwind.cpp` | Chaos 使用相同的 Win64 格式 |
| Tier Promotion | `TieredCompilationManager` | Chaos 使用 call_count，CoreCLR 使用耗时采样 |
| Precise GC | GCInfo / `JitInterface::GetGcInfo` | Chaos 使用 StackMap 表 |
| Inline Cache (PIC) | — | CoreCLR 使用 VSD (Virtual Stub Dispatch) |
| OSR | `OnStackReplacement` | CoreCLR 在 Tier1→Tier2 时重编译 |

---

## 输出格式

```
✅ done: [已处理的子任务 ID 列表]
⏳ remaining: [未处理的子任务 ID 列表 + 原因]
```

示例：
```
✅ done: [T1 — 修复 UNWIND_CODE nibble 编码, entry.exe SEH 正常]
⏳ remaining: [T2 — ARM64 JIT 需要交叉编译环境, 当前 x64 机器无法测试]
```
