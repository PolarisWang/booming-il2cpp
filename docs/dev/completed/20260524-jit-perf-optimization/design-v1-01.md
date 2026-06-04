# JIT 性能优化路线 — 设计方案

> 目标：从当前 JIT（真 x64 代码生成器，但无内联/无 CSE/无边界消除）逐步达到 CoreCLR JIT 级别的性能，同时保持 hot-update 安全性。

## 范围与边界

### 范围
- JIT 引擎优化：内联、VN/CSE、常量折叠、边界检查消除、循环优化、SIMD
- Hot-update 安全体系：Call-Site Slot 间接化、版本失效追踪、内联失效
- MIR / 轻量树 IR 框架
- Intrinsic 识别与展开
- 寄存器分配质量提升

### 非目标
- 不涉及 ARM64 支持（属于已有 JIT 工业化 roadmap）
- 不涉及 Linux SEH/DWARF（属于已有 JIT 工业化 roadmap）
- 不涉及调试信息/SOS（属于已有 JIT 工业化 roadmap）
- 不做完整 GenTree 体系（只做单 BB 表达式树）
- 不做全局值编号（当前只做单 BB VN）
- 不重构 codegen 管线或 Scriban 模板

## 架构决策

### D1 — 调用间接化策略
**选择：Call-Site Slot 间接化（方案 2）**

JIT 编译时对每个 call 发射 `call [rip + offset_to_slot_table + slot_index * 8]`，替代当前 `mov rax, imm64; call rax`。slot 表附着在 JitMethod 代码缓冲区后。

理由：
- Hotpatch 时只需更新 slot 表中的指针，调用者不需要重编译
- 内联失效时仍需要调用者重编译（内联展开无法通过 slot 更新修复）

### D2 — 版本失效机制
每个 `HotpatchEntryV0` 新增 `uint32_t version`，每次 hotpatch 时 atomic_fetch_add。
JIT 编译时快照 callee version；hotpatch 时通过 ReverseSlotMap 更新 slot 指针，并通过内联者列表（inlined_method_tokens）标记 stale。

### D3 — IR 优化框架
**选择：轻量树 IR（方案 C）**

不做完整 GenTree 体系。只对单基本块内构建表达式树，进行 VN、CSE、常量折叠、LdFld 合并，然后线性化回 RegisterInstruction。

新文件：`jit/jit_tree_builder.h/cpp`、`jit/jit_vn.h/cpp`、`jit/jit_tree_mutator.h/cpp`、`jit/jit_linearizer.h/cpp`、`jit/jit_optimizer.h/cpp`。

### D4 — Pass 编排方式
**选择：函数式（方案 B），预留 PassManager 迁移**

优化 pass 以显式函数调用的方式在 `Optimizer::Run()` 中编排。未来 pass 数量达到 10+ 时，可零成本迁移到 PassManager。

## 阶段划分

| 阶段 | 内容 | 依赖 | 估算 |
|------|------|------|------|
| P0 | Call-Site Slot 间接化 + ReverseSlotMap + Version + 内联失效 | 无 | 3-4w |
| P1 | 轻量树 IR（表达式树 + VN + CSE + 线性化器） | P0 | 4-5w |
| P2 | 热更感知内联器（树嫁接 + 内联启发式） | P0+P1 | 3-4w |
| P3 | Intrinsic 识别 + 展开（Math.*, Array.Length, GC barrier） | P1+P2 | 2-3w |
| P4 | 边界检查消除 + NULL 消除 | P1 | 2-3w |
| P5 | 循环优化（IV hoisting, unrolling） | P1+P4 | 3-4w |
| P6 | SIMD / HW intrinsics | P2+P3+P4 | 4-6w |

## 三优先级权衡结论

| 优先级 | 评估 |
|--------|------|
| P1 性能最优 | 方案 C（树 IR + VN + CSE）在内联前提供额外优化机会，优于线性 MIR。核心权衡正确。 |
| P2 架构完美 | 函数式编排 + 树 IR 与 CoreCLR 做法一致（显式 phase + GenTree），架构对标行业方案。 |
| P3 热更适配 | Slot 间接化 + caller tracking + 内联失效构成完整的 hot-update safety net。 |

所有决策均通过 P1>P2>P3 优先级审查。

## 关键文件新增/修改

### P0
| 文件 | 改动 |
|------|------|
| `contracts/native/v0/codegen_bridge.h` | HotpatchEntryV0 加 version 字段 |
| `src/native/jit/jit_engine.h` | JitMethod 加 call_site_slots/inlined_tokens/version 字段 |
| `src/native/jit/jit_engine.cpp` | EmitCallWithSpill 改 RIP-relative slot 发射 |
| `src/native/jit/jit_precode.cpp` | ReverseSlotMap 注册/CAS/失效回调 |
| `src/native/jit/jit_precode.h` / `slot_map.h` | ReverseSlotMap 定义 |
| `src/native/runtime-core/hotpatch_table.cpp` | Patch 成功后触发 slot update callback |
| `src/native/runtime-core/jit_registration.h` | RegisterReverseSlot / UnregisterMethodSlots API |

### P1
| 文件 | 改动 |
|------|------|
| `src/native/jit/jit_tree_builder.h/cpp` | NEW — RegisterMethod → 表达式树 |
| `src/native/jit/jit_vn.h/cpp` | NEW — 值编号表 + CSE |
| `src/native/jit/jit_tree_mutator.h/cpp` | NEW — 树遍历/替换基类 |
| `src/native/jit/jit_linearizer.h/cpp` | NEW — 优化后树 → RegisterInstruction |
| `src/native/jit/jit_optimizer.h/cpp` | NEW — 优化入口编排 |
| `src/native/jit/jit_engine.cpp` | Compile() 中插入 Optimizer::Run() |
| `src/native/CMakeLists.txt` | 新增源文件注册 |

### P2-P6
P2-P6 的具体文件级设计在各自子任务的 plan 中展开。
