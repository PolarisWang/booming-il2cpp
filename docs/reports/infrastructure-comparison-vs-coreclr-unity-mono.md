# Chaos IL2CPP 基建对比清单：CoreCLR / Unity IL2CPP / Mono

> 生成日期: 2026-05-17
> 范围: 全项目基础设施模块与三大运行时体系的成熟度对比

---

## 对比基线说明

| 对比对象 | 版本基线 | 核心差异 |
|---------|---------|---------|
| **CoreCLR** | .NET 8+ | 完整 JIT/AOT 混合，生产级 GC，诊断/调试/分析全套 |
| **Unity IL2CPP** | 2023 LTS | 纯 AOT 全量 C++ 代码生成，全平台，无解释器 |
| **Mono** | Mono 6.12+ | 解释器 + JIT 混合，Unity 历史运行时，可调试性好 |
| **Chaos IL2CPP** | main (2026-05) | 自研分代 GC + 解释器 + 部分 AOT，独特热更新能力 |

---

## 一、GC / 内存管理

### 已实现（成熟）
| 组件 | CoreCLR | Unity IL2CPP | Mono | Chaos | 文件 |
|------|---------|-------------|------|-------|------|
| Nursery/Young GC | gen0/1 | Boehm/自研 | SGen | ✅ | gc_young_collector.cpp |
| Old Gen Mark-Sweep | gen2 | Boehm/自研 | SGen | ✅ | gc_old_gen.cpp |
| Parallel Mark | ✅ | ✅ | 部分 | ✅ | gc_parallel_mark.cpp |
| Card Table / Write Barrier | ✅ | ✅ | ✅ | ✅ | gc_card_table.cpp |
| Large Object Heap | ✅ | ✅ | ✅ | ✅ | gc_loh.cpp |
| Bump Cache Per-Thread | ✅ | ✅ | ✅ | ✅ | gc_bump_cache.h |
| GC Stats / Event Callbacks | ✅ | ✅ | ✅ | ✅ | gc_stats.cpp, gc_events.cpp |
| Root Scanner | ✅ | ✅ | ✅ | ✅ | gc_root_scanner.cpp |
| GC Transition (STW guard) | ✅ | ✅ | ✅ | ✅ | gc_transition.h |
| GC Stress Test | ✅ | 自有 | 自有 | ✅ | gc_stress.cpp |
| GC Scheduler (EMA adaptive) | ✅ | ❌ | ❌ | ✅ | gc_scheduler.h |

### 部分实现（待验证/补齐）
| 组件 | Chaos | 风险等级 | 说明 |
|------|-------|---------|------|
| BGC Concurrent Mark | ⚠️ | 🟡 中 | 三阶段代码结构完整（concurrent mark → STW re-mark → concurrent sweep），并发正确性（SATB barrier、ABA、mark stack overflow）未经大规模验证 |
| POH (Pinned Object Heap) | ⚠️ | 🟡 中 | Region 定义了 REGION_POH 类型，但实际 bump-pointer 分配路径是否排除 young GC copy 不可知 |
| CRAG Region Framework | ⚠️ | 🟢 低 | C0.1 阶段完成（RegionManager + 5 域 + Regop），后端还依赖 GC_MALLOC，未替换旧分配路径 |

### 未实现
| 组件 | CoreCLR | Unity | Mono | 必要性 | 影响 |
|------|---------|-------|------|-------|------|
| NUMA Affinitization | ✅ | ❌ | ❌ | 🟢 低 | MVP 不需要 |
| GCNotification API | ✅ | 桥接 | ❌ | 🟢 低 | 可后续添加 |
| HeapCompactionMode 控制 | ✅ | 部分 | ❌ | 🟢 低 | MVP 不阻塞 |
| Write Watch (server GC) | ✅ | ❌ | ❌ | 🟢 低 | 服务器场景扩展 |

### GC 成熟度评估: ~65%
**主要风险**: BGC 并发正确性是大规模并行下的潜在稳定漏洞，建议启动多线程长稳压力测试。

---

## 二、执行引擎

### 已实现
| 组件 | CoreCLR | Unity IL2CPP | Mono | Chaos | 文件/模块 |
|------|---------|-------------|------|-------|----------|
| IL 解释器 (栈式 VM) | N/A | N/A | ✅ | ✅ | fast_dispatch.cpp (68KB) + interpreter_vm.cpp (85KB) |
| IR 寄存器分配 | JIT backend | N/A | JIT backend | ✅ | ir_reg_alloc.cpp (77KB) |
| IR lowering / planning | JIT backend | N/A | JIT backend | ✅ | Lowering/ Planning/ 目录 |
| NativeAot Code Emission | CoreRT | N/A | N/A | ✅ | Emission/NativeAot/ |
| Token / Metadata 解析 | JIT | AOT compile-time | JIT | ✅ | token_resolver.cpp |

### 部分实现
| 组件 | Chaos | 风险等级 | 说明 |
|------|-------|---------|------|
| 全量 AOT IL→C++ 翻译 | ⚠️ | 🔴 高 | CodeGen 管线存在但有 Planner 定义、BuildSystem、Templating、Validation 子目录，**全量编译管线是否完整闭合不可知**。当前明显依赖解释器执行大部分代码 |
| 解释器 ↔ AOT 混合执行 | ⚠️ | 🔴 高 | 两者代码共存但缺少热方法从解释器升级到 AOT 的决策策略（参见 wiki/21-interpreter-optimizations.md） |
| Delegate thunks + calli | ⚠️ | 🟡 中 | delegate_thunks.cpp 存在，但 calli（即 function pointer 调用）路径是否全场景支持不确定 |

### 未实现
| 组件 | CoreCLR | Unity | Mono | 必要性 | 说明 |
|------|---------|-------|------|-------|------|
| Tiered Compilation | ✅ | N/A | ❌ | 🟡 中 | 混合解释器+AOT 场景下，缺少 tier 升级策略意味着热方法永远卡在解释器，性能损失不可接受 |
| OSR (On-Stack Replacement) | ✅ | N/A | ❌ | 🟡 中 | 正在执行的长方法无法切换代码版本，直到返回才生效 |
| RyuJIT / Mini-JIT | ✅ | N/A | ✅ | 🟢 低 | IL2CPP 设计上不需要 JIT |

### 执行引擎成熟度评估: ~45%
**主要风险**: 全量 AOT 管线不完整是最严重的单点风险。如果 codegen 未能覆盖全部 IL 指令序列，当前运行时退化到解释器执行会带来 10-100x 的性能损失。建议 audit codegen 覆盖度。

---

## 三、诊断与调试

### 已实现
| 组件 | CoreCLR | Unity IL2CPP | Mono | Chaos | 文件/模块 |
|------|---------|-------------|------|-------|----------|
| 日志系统 (分级 LOG) | ✅ | ✅ | ✅ | ✅ | chaos/log.h (4 级: DEBUG/INFO/WARN/ERROR) |
| Trace 追踪 (span 树) | ❌ | ❌ | ❌ | ✅ | chaos/trace.h — 独特优势 |
| Profile Scope (RDTSC) | ❌ | ❌ | ❌ | ✅ | chaos/profile.h — 独特优势，热点预埋 |
| GC Event Callbacks | ✅ | ✅ | ✅ | ✅ | gc_events.h — GC_START/MARK_DONE/SWEEP_DONE/GC_END |

### 未实现
| 组件 | CoreCLR | Unity | Mono | 必要性 | 影响分析 |
|------|---------|-------|------|-------|---------|
| **EventPipe** | ✅ | 部分 | ✅ | 🔴 高 | 无法接入 `dotnet-trace`、`dotnet-counters`、SOS。整个 .NET 诊断生态不可用。**这是最严重的单一基础设施缺口** |
| **EventSource (托管事件)** | ✅ | 部分 | ✅ | 🔴 高 | 无运行时事件发布能力（GC 统计、线程池统计、异常统计等） |
| **托管调试器 (ICorDebug / MDB)** | ✅ | Debugger Agent | Soft Debugger | 🔴 高 | 所有 IL 级调试只能通过 native debugger + printf。开发效率极低 |
| **ICorProfiler API** | ✅ | ❌ | ✅ | 🟡 中 | 无第三方性能分析器接入能力 |
| **MiniDump / Crash Dump** | ✅ | 自有 | 自有 | 🟡 中 | 缺乏生产事故的结构化排查手段 |
| **IL-level 断点/单步** | ✅ | 部分 | ✅ | 🔴 高 | 无法单步跟踪解释器或 AOT 生成的代码逻辑 |

### 诊断成熟度评估: ~15%（最薄弱模块）
**主要风险**: 没有 EventPipe = 被排除在 .NET 性能分析/诊断/监控体系之外。这是 P0 级别的缺口。

---

## 四、线程与同步

### 已实现
| 组件 | CoreCLR | Unity IL2CPP | Mono | Chaos | 文件 |
|------|---------|-------------|------|-------|------|
| ThreadPool (work-stealing) | ✅ | ✅ | ✅ | ✅ | thread_pool.cpp (19KB) — 全局队列 + 本地窃取 + gate thread |
| TimerQueue | ✅ | ✅ | ✅ | ✅ | timer_queue.cpp |
| WaitHandle (WaitAny/WaitAll) | ✅ | 部分 | ✅ | ✅ | wait_handle.cpp |
| CancellationToken | ✅ | 桥接 | ✅ | ✅ | cancellation_token.cpp |
| ExecutionContext | ✅ | ✅ | ✅ | ✅ | execution_context.cpp |
| ThreadState / SafepointPoll | ✅ | ✅ | ✅ | ✅ | thread_state.cpp (17KB) |
| Monitor (sync block) | ✅ | ✅ | ✅ | ✅ | core/monitor.cpp + chaos/monitor.h |
| 同步原语 (mutex, CV, RWLock) | ✅ | ✅ | ✅ | ✅ | core/sync_mutex.cpp + synchronization.cpp |
| 线程本地状态 (TLS) | ✅ | ✅ | ✅ | ✅ | core/state_tls.cpp |

### 部分实现
| 组件 | Chaos | 风险等级 | 说明 |
|------|-------|---------|------|
| Task (TAP await 底层) | ⚠️ | 🟡 中 | task_runner.cpp 存在但实现较薄。控制是否完整 support .NET Task 的 await/continuation/spawn |

### 未实现
| 组件 | 必要性 | 影响 |
|------|-------|------|
| 纤程/协程 (Unity 协程模式) | 🟢 低 | 如需嵌入 Unity 生态则是需求 |
| .NET 线程池 Hill-Climbing | 🟡 中 | 当前线程池缺少年代自适应调整的智能算法 |

### 线程成熟度评估: ~80%
**主要风险**: 成熟度较高。Task 的 codegen 层面 lowering 是下一步关注点。

---

## 五、反射与元数据

### 已实现
| 组件 | CoreCLR | Unity IL2CPP | Mono | Chaos | 文件 |
|------|---------|-------------|------|-------|------|
| Type 查询 (GetType/GetMethods/GetFields/GetProperties/GetConstructors) | ✅ | ✅ | ✅ | ✅ | reflection_api.h (30+ API) |
| Custom Attributes | ✅ | ✅ | ✅ | ✅ | reflection/attributes.cpp |
| Reflection Invoke | ✅ | ✅ | ✅ | ✅ | reflection/invoke.cpp |
| 泛型实例化 (GenericContext) | ✅ | ✅ | ✅ | ✅ | generic_context.cpp (31KB) + instantiation_engine.cpp |
| Assembly/Types 枚举 | ✅ | ✅ | ✅ | ✅ | assembly_manager.cpp |
| 类型层次 (BaseType, Interfaces, Subclass) | ✅ | ✅ | ✅ | ✅ | reflection/hierarchy.cpp |
| 成员描述 (Name, Token, Param) | ✅ | ✅ | ✅ | ✅ | reflection/members.cpp |
| Metadata Token 查询 | ✅ | ✅ | ✅ | ✅ | reflection_metadata_impl.h |
| LayoutEngine (对象布局) | ✅ | ✅ | ✅ | ✅ | layout_engine.cpp (34KB) |

### 部分实现
| 组件 | Chaos | 风险等级 | 说明 |
|------|-------|---------|------|
| Type.IsAssignableFrom/IsInstanceOfType | ⚠️ | 🟢 低 | API 导出存在，实现路径在 reflection_stubs 中 |
| 动态 Assembly.Load | ⚠️ | 🟡 中 | AOT 运行时动态加载新 assembly 需要 codegen 或 interpreter 支持新类型注入 |

### 未实现
| 组件 | CoreCLR | Unity | Mono | 必要性 |
|------|---------|-------|------|-------|
| Reflection.Emit | ✅ | ❌ (AOT 不可用) | ✅ | 🟢 低 — AOT 运行时不需要 |
| MemberwiseClone (非托管) | ✅ | ✅ | ✅ | 🟢 低 — 对象模型中有等价物 |
| 完整 TypeLoader 模式 | ✅ | ✅ | ✅ | 🟡 中 — 动态场景待验证 |

### 反射成熟度评估: ~80%
**主要风险**: 静态反射覆盖广泛。动态加载场景在 AOT 运行时是已知难点。

---

## 六、Native Interop (P/Invoke / COM / Marshal)

### 已实现
| 组件 | CoreCLR | Unity IL2CPP | Mono | Chaos | 文件 |
|------|---------|-------------|------|-------|------|
| Marshal ABI 定义 | ✅ | ✅ | ✅ | ✅ | marshal_abi.h — MemoryBlock, String, Struct, Delegate, COM, Variant |
| COM IUnknown Vtbl | ✅ | ✅ | ✅ | ✅ | com_abi.h |
| Delegate ↔ Native FP | ✅ | ✅ | ✅ | ✅ | core/delegate_thunks.cpp |
| Marshal Alloc (HGlobal/CoTaskMem) | ✅ | ✅ | ✅ | ⚠️ | core/marshal_alloc.cpp |
| Marshal String | ✅ | ✅ | ✅ | ⚠️ | core/marshal_string.cpp |
| Struct Layout (Sequential/Explicit) | ✅ | ✅ | ✅ | ⚠️ | core/struct_marshal_descriptors.cpp |
| Marshal API entry | ✅ | ✅ | ✅ | ⚠️ | core/marshal_api.cpp |

### 未实现/极早期
| 组件 | CoreCLR | Unity | Mono | 必要性 | 影响 |
|------|---------|-------|------|-------|------|
| COM CCW (COM Callable Wrapper) | ✅ | ✅ | ✅ | 🔴 高 | Windows 上任何 COM 场景（Office, Shell, DirectX）不可用 |
| COM RCW (Runtime Callable Wrapper) | ✅ | ✅ | ✅ | 🔴 高 | 同上 |
| COM Activation (CoCreateInstance) | ✅ | ✅ | ✅ | 🔴 高 | 同上 |
| WinRT Interop | ✅ | ✅ | ❌ | 🟡 中 | Windows 原生 API 访问受限 |
| P/Invoke DllImport 完整解析链 | ✅ | ✅ | ✅ | 🟡 中 | 名称解析、缓存、调用路径待 audit |
| ICustomMarshaler | ✅ | 部分 | ✅ | 🟡 中 | 高级自定义封送场景 |

### Interop 成熟度评估: ~20%
**主要风险**: COM 是最大的操作性缺口。未实现 COM 意味着任何 Windows 原生 COM 组件（包括 DirectX, Media Foundation, Shell API）都无法从托管代码调用。游戏引擎钩子也需要 COM。

---

## 七、异常处理

### 已实现
| 组件 | CoreCLR | Unity IL2CPP | Mono | Chaos | 文件 |
|------|---------|-------------|------|-------|------|
| SEH/VEH 底层 (setjmp/longjmp) | ✅ | ✅ | ✅ | ✅ | exception_jmp.h |
| 异常传播 (unwinding) | ✅ | ✅ | ✅ | ✅ | exception_helpers.cpp |
| 托管异常 API | ✅ | ✅ | ✅ | ✅ | exception_api.cpp |
| Codegen: try/catch/finally | JIT | C++ try/catch | JIT | ✅ | StructuredControlFlow codegen |
| EH 表验证 | ✅ | ✅ | ✅ | ✅ | fact-verify-exception-handling.md |

### 成熟度评估: ~85%
**主要风险**: 相对成熟，有专项验证文档。桩函数 (exception_stubs.cpp) 对异常类型的覆盖度需要 audit。

---

## 八、平台可移植性

### 已实现
| 平台 | CoreCLR | Unity IL2CPP | Mono | Chaos | 验证状态 |
|------|---------|-------------|------|-------|---------|
| Windows x64 | ✅ | ✅ | ✅ | ✅ | 主开发平台，实际运行验证 |
| CMakePresets (3 档) | ✅ | ✅ | ✅ | ✅ | debug/profile/ship 预设 |
| ABI 导出 (+注释) | ✅ | ✅ | ✅ | ✅ | extern "C" + ABI 理由注释 |

### CMake 配置声明存在（未实际验证）
| 平台 | 配置文件 | 实际验证 | 风险 |
|------|---------|---------|------|
| Linux x64 | linux-x64.cmake | ❌ 未验证 | 🟡 中 — 平台相关条件编译可能不完整 |
| macOS | macos-reference.cmake | ❌ 未验证 | 🟡 中 — 同上 |
| Android ARM64 | android-arm64.cmake | ❌ 未验证 | 🟡 中 — 交叉工具链也未经测试 |
| iOS ARM64 | ios-arm64.cmake | ❌ 未验证 | 🟡 中 — 同上 |

### 未实现
| 平台 | CoreCLR | Unity | Mono | 必要性 | 影响 |
|------|---------|-------|------|-------|------|
| WebAssembly | ❌ | ✅ | ✅ | 🔴 高 | 游戏引擎 IL2CPP 刚需（Unity WebGL） |
| 游戏引擎嵌入 | ❌ | Unity 原生 | Unity 历史 | 🔴 高 | engine-bridge 模块独立但为空壳，无实际引擎集成 |

### 可移植性成熟度评估: ~35%
**主要风险**: 全平台是"声明式"的，没有一次真实的交叉编译或交叉测试。游戏引擎集成接口存在代码但没有实际的 Unity/Unreal 钩子。

---

## 九、热更新（Chaos 核心优势）

| 组件 | CoreCLR | Unity IL2CPP | Mono | Chaos | 文件 |
|------|---------|-------------|------|-------|------|
| Method 替换 | ❌ | 第三方 (HybridCLR/xLua) | 第三方 | ✅ | method_replacement.cpp |
| Patch 加载 | ❌ | 第三方 | 第三方 | ✅ | patch_loader.cpp (23KB) |
| 热更新 Assembly 管理 | ❌ | 第三方 | 第三方 | ✅ | hot_update.cpp |
| 方法表补丁 (VTable patch) | ❌ | 第三方 | 第三方 | ✅ | hotpatch_table.cpp |

### 成熟度评估: ~90% (相对)
**评估**: 这是 Chaos IL2CPP 对 Unity IL2CPP 的**核心差异化优势**。Unity 生态的热更新完全依赖 xLua/ToLua/HybridCLR 等第三方方案，Chaos 将其内建为运行时的一等公民。

---

## 十、构建与工程基础设施

| 组件 | CoreCLR | Unity IL2CPP | Mono | Chaos | 状态 |
|------|---------|-------------|------|-------|------|
| CMake 多配置 | ✅ | 自研 | 自研 | ✅ | CMakePresets.json (debug/profile/ship) |
| 验证管线 | 自有 | 自有 | 自有 | ✅ | verification/ 完整 7-stage pipeline |
| 合约测试框架 | 自有 | 自有 | 自有 | ✅ | tests/contracts + foundation_dll tests |
| 性能测试框架 | ✅ | ✅ | ✅ | ✅ | tests/perf + stress runners |
| 代码格式化 | ✅ | ✅ | ✅ | ✅ | .clang-format |
| **CI/CD (自动回归)** | ✅ | ✅ | ✅ | ❌ **缺失** | 无 GitHub Actions / Azure Pipeline |
| 代码静态分析 | ✅ | ✅ | ✅ | ⚠️ 仅 clang-format | 无 SAST 工具集成 |
| 依赖管理 | NuGet | 自有 | 自有 | ⚠️ 手动 | third_party/ 手动管理 |

### 工程基础设施成熟度评估: ~50%
**主要风险**: 没有 CI 是最高工程风险。无法阻止回归提交。

---

## 十一、Managed / BCL 覆盖

### 参考程序集现状
| Assembly | CoreCLR | Unity | Mono | Chaos | 类型 |
|---------|---------|-------|------|-------|------|
| System.Private.CoreLib | ✅ | ✅ | ✅ | ✅ 1 个 (v8.0 + v10.0) | runtime |
| System.Console | ✅ | ✅ | ✅ | ✅ 1 个 | ref only |
| System.Collections | ✅ | ✅ | ✅ | ✅ 1 个 | ref only |
| System.Linq | ✅ | ✅ | ✅ | ✅ 1 个 | ref only |
| System.Runtime | ✅ | ✅ | ✅ | ✅ 1 个 | ref only |
| System.Runtime.Extensions | ✅ | ✅ | ✅ | ✅ 1 个 | ref only |
| mscorlib | N/A | N/A | N/A | ✅ 1 个 | ref only (compat) |
| netstandard | N/A | N/A | N/A | ✅ 1 个 | ref only |

### 缺失的重要程序集（非穷举）
| 缺失的程序集 | 影响 | 风险 |
|------------|------|------|
| System.Net.* (HttpClient, Sockets, DNS) | 网络栈 | 🔴 高 |
| System.IO.Compression | 压缩/解压 | 🟡 中 |
| System.IO.FileSystem | 文件 I/O | 🔴 高 |
| System.Security.* (Cryptography) | 加密/安全 | 🔴 高 |
| System.Text.Json / System.Xml.* | 序列化 | 🟡 中 |
| System.Threading.* (Channels, Barriers) | 高级同步 | 🟡 中 |
| System.Diagnostics.* (Tracing, Metrics) | 诊断 | 🔴 高 |
| System.Data.* | 数据库 | 🟡 中 |
| System.Runtime.InteropServices | Interop 增强 | 🔴 高 |
| System.Reflection.Emit | 动态代码生成 | 🟢 低 (AOT 不需要) |

### 运行时 Stub 覆盖
| 模块 | 文件数 | 状态 |
|------|-------|------|
| array, string, char, math, datetime, guid, hashcode, interlocked | 8 | ✅ 已实现 |
| exception, object, threading, parallel, random, reflection | 6 | ✅ 已实现 |
| collection, stream, convert, misc | 4 | ✅ 已实现 |
| 总计: runtime_stubs/.cpp+.h 配对 | 17 对 | ✅ |

### BCL 覆盖成熟度评估: ~25%
**主要风险**: 参考程序集数量严重不足。标准 .NET 库调用缺少元数据解析来源。项目声称 runtime_stubs 广泛覆盖，但缺少对应 reference assembly 意味着 codegen 缺少解析上游。

---

## 综合风险评估矩阵

| 模块 | 成熟度 | 优先级权重 | 复合风险 |
|------|-------|-----------|---------|
| GC / 内存管理 | ~65% | P0 | 🟡 受控 |
| 执行引擎 (Codegen + Interpreter) | ~45% | P0 | 🔴 高危 |
| 诊断与调试 | ~15% | P1 | 🔴 高危 |
| 线程与同步 | ~80% | P0 | 🟢 低 |
| 反射与元数据 | ~80% | P0 | 🟢 低 |
| Native Interop (P/Invoke, COM) | ~20% | P1 | 🔴 高危 |
| 异常处理 | ~85% | P0 | 🟢 低 |
| 平台可移植性 | ~35% | P1 | 🟡 中 |
| 热更新 | ~90% | P0 (优势) | 🟢 成熟 |
| 工程基础设施 (CI, 构建) | ~50% | P0 | 🟡 中 |
| BCL / Managed 覆盖 | ~25% | P0 | 🔴 高危 |

### 整体成熟度: ~50%

---

## P0 修补建议（最短路径降低最高风险）

### 1. 诊断: EventPipe 最小化实现（预估 2-3 周）
```
GC 事件已有 gc_events.h 框架 → 扩展为 EventPipe 事件发布层
→ 至少发布 GC/ThreadPool/Exception 3 类事件
→ 支持 IPC 直出 (named pipe / Unix socket)
→ dotnet-trace collect 可用
```

### 2. 全量 AOT Codegen Audit（预估 2-3 周）
```
- 遍历所有 IL opcode，确认 CodeGen 覆盖
- 缺失指令 → 归类: a) fallback 到解释器  b) 实现 codegen
- 产出 AOT coverage matrix
```

### 3. CI (预估 1 周)
```
GitHub Actions:
  - Windows: cmake --preset debug + build + run contracts
  - min 保护: 提交触发编译 + link + smoke test
```

### 4. BCL 引用补全（预估 1-2 周）
```
复制 System.Net.Primitives, System.IO.FileSystem, System.Security.Cryptography 等 ref assemblies
为 codegen 提供元数据解析来源
```

### 5. Game Engine Bridge（预估 1-2 周）
```
engine_bridge.cpp → 实现最小 API:
  - EngineRegisterTypes
  - EngineAllocateObject
  - EngineInvokeMethod
文档记录接入协议
```

---

## 附录: 独特优势（CoreCLR/Unity/Mono 没有的）

| 能力 | 说明 |
|------|------|
| **内建热更新** | Method replacement + patch loader + hotpatch table，Unity 生态无原生竞品 |
| **RDTSC Profile Scope** | 零 I/O 热点统计，已在 50+ 热点函数预埋 |
| **Trace Span 树** | 结构化会话追踪，取代 printf/perf log 散点 |
| **CRAG Region Framework** | 统一 GC/Domain/Raw 分配，A→B→C 路线图的 C 阶段架构清晰 |
| **Foundation DLL 验证管线** | 7-stage 完整验证，含 capability/contract/closure/evidence 多重检查 |
| **验证历史管理系统** | JSONL 记录 + 归档 + regression 对比能力 |

---

*本文档作为长期参考，随着 Chaos IL2CPP 模块完善应持续更新。*
