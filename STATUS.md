# Chaos IL2CPP — 项目状态

> 最后更新: 2026-06-11

---

## 架构映射

### 项目模块边界

| 层级 | 职责 | 关键路径 |
|------|------|----------|
| Codegen (C#) | IL→C++ 翻译、Planner、Emission | `src/managed/Chaos.IL2CPP.Generator/` |
| TPG (C#) | 测试项目生成、entry 模板 | `src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/` |
| Python 管线 | 构建、验证、部署编排 | `Testing/foundation-dll/verification/` |
| Native C++ | 运行时核心、解释器、GC、JIT | `src/native/runtime-core/`, `src/native/interpreter/` |

### 翻译路径

| IL 特性 | 翻译策略 | Constraints | wiki 条目 |
|---------|----------|-------------|-----------|
| call (direct) | direct_call | ABI 签名匹配 | ok |
| callvirt | vtable_dispatch | TCVC + IOC 缓存 | ok |
| newobj | constructor_dispatch | GC 分配路径 | ok |
| box | heap_box | GC 分配 | ok |
| unbox | unbox_stub | 值类型语义 | ok |
| throw | exception_throw | SEH + CHAOS_EH_TRY | ok |
| rethrow | exception_rethrow | SEH 传播 | ok |
| ldfld/ldflda | field_access | 偏移计算、GC 引用 | ok |
| stfld | field_store | GC 写屏障 | ok |
| ldelem/stelem | array_element | GC 感知 | ok |
| ldstr | string_literal | string table 查找 | ok |
| delegate | delegate_invoke | thunk 表 + InterpreterEntryDirect | ok |
| calli | indirect_call | ABI 签名匹配 | ok |
| conv_ovf | overflow_check | JIT 内联 | ok |
| boxable<T> | generic_box | 泛型特化 | ok |
| initobj | zero_init | 值类型零初始化 | ok |
| cpblk/initblk | memcpy_memset | Unsafe.CopyBlock/InitBlock stubs | ok |

### GC 子系统

| 组件 | 状态 | 备注 |
|------|------|------|
| Young Gen (Nursery+TLAB) | ✅ 完成 | 3 代分代: Nursery → Survivor → Old |
| Old Gen (SATB) | ✅ 完成 | 并发标记 + 清除 |
| BGC (Background GC) | ✅ 完成 | 并发工作线程 |
| LOH (Large Object Heap) | ✅ 完成 | >85K 对象 |
| Full GC | ✅ 完成 | 无 STW 问题 |
| GC Stress | ✅ 完成 | `CHAOS_GC_STRESS=1` |
| Server GC | ✅ 完成 | Phase 3: Thread hijacking + coordinated pause |
| GC ETW | ✅ 完成 | Windows 事件 |

### JIT 子系统

| 组件 | 状态 | 备注 |
|------|------|------|
| Quick JIT (T1) | ✅ 完成 | precode 系统 |
| Full JIT (T3/T4) | ✅ 完成 | register allocator + SEH |
| Tier promotion | ✅ 完成 | call_count-based, 0→8 状态机 |
| OSR (On-Stack Replacement) | ✅ 完成 | T4 deopt → interpreter |
| ARM64 | 🔄 基础支持 | QEMU 通过，性能未优化 |

### 解释器

| 组件 | 状态 | 备注 |
|------|------|------|
| FastExecute (Layer C) | ✅ 完成 | FastFrame + SEH |
| RegisterExecute (Layer R) | ✅ 完成 | Register-allocated IL |
| InterpreterVM (Layer D) | ✅ 完成 | 完整 ExecutionFrame |
| InterpreterEntryDirect | ✅ 完成 | 5 级 dispatch (Step1→A0→B→C→D) |
| Crypto interpreter routing | ✅ IL data emission fixed | x509: 88.8% → 预期接近100%；Core crypto: 需P/Invoke路由方案 |

### 热更新

| 组件 | 状态 | 备注 |
|------|------|------|
| HotpatchNameRegistry | ✅ 完成 | name→token→slot 三级查找 |
| SetPatchedBySlot | ✅ 完成 | Atomic flags + version bump |
| PatchMethod tier state | ✅ 完成 | CAS 0→8 tier machine |
| LCAC (Layered Cross-Assembly) | ✅ 完成 | Phase 1-6 替代 BridgeAOT |
| Delegate hotpatch | ✅ 完成 | DelegateHotpatchCheckpoint |
| Non-CoreLib build | ✅ 完成 | 3 template/pipeline fixes |

## 验证管线状态

### foundation-dll verification pipeline

| Assembly | Chunks | Status |
|----------|--------|--------|
| System.Private.CoreLib | 25 | ✅ Passed |
| System.Collections | 2 | ✅ Passed |
| System.Collections.Immutable | 6 | ✅ Passed |
| System.ComponentModel.TypeConverter | 3 | ✅ Passed |
| System.Data.Common | 4 | ✅ Passed |
| System.Diagnostics.DiagnosticSource | 2 | ✅ Passed |
| System.Formats.Asn1 | 1 | ✅ Passed |
| System.IO.Compression.Brotli | 1 | ✅ Passed |
| System.IO.Pipelines | 1 | ✅ Passed |
| System.Linq | 15 | ✅ Passed |
| System.Linq.Expressions | 1 | ✅ Passed |
| System.Net.Http | 2 | ✅ Passed |
| System.Net.ServerSentEvents | 5 | ✅ Passed |
| System.Net.Sockets | 1 | ✅ Passed |
| System.ObjectModel | 2 | ✅ Passed |
| System.Private.Xml | 1 | ✅ Passed |
| System.Reflection.Metadata | 3 | ✅ Passed |
| System.Runtime.InteropServices | 1 | ✅ Passed |
| System.Runtime.Intrinsics | 2 | ✅ Passed |
| System.Runtime.Serialization.Formatters | 1 | ✅ Passed |
| System.Security.Claims | 1 | ✅ Passed |
| System.Security.Cryptography | 4 | ✅ 88% (1077/1222) AOT/JIT aligned |
| System.Security.Principal.Windows | 1 | ✅ Passed |
| System.Text.Json | 1 | ✅ Passed |
| System.Threading.Tasks.Parallel | 1 | ✅ Passed |
| System.Xml.ReaderWriter | 1 | ✅ Passed |

### CodeGen 测试

- **总数**: 878 tests, 0 skipped
- **Coverlet 覆盖率**: 38.5% (ReferenceProof excluded)
- **状态**: ✅ 全部通过

## 已知问题

### P0（必须修复）

| # | 问题 | 状态 | 根因 |
|---|------|------|------|
	| 1 | G-1: Page 归还 OS | ✅ 已实现 (2026-06-11) | page_pool_ + PalVirtualDecommit + kMaxPoolSize=16 trim, 超限页 PalVirtualFree |
	| 2 | G-2: FindPage 索引化 | ✅ 已实现 (2026-06-11) | PageArray 排序数组 + 二分查找 O(log n) |
	| 3 | G-3: BGC-YoungGC 交互 segfault | ✅ 已修复 (2026-06-11) | Phase 3 coordinated pause 协议：atomic flag 握手 + nursery drain + Phase 3d re-rooting |
	| 4 | Server GC crash | ✅ DrainMarkStackParallel fix + chaos_jit MSVC build fix | Worker pool re-entrancy + GCC atomics on MSVC |
	| 5 | Cryptography 360 failures | ✅ 88% (1077/1222) passed | BCrypt P/Invoke stubs + routing chain complete. AOT/JIT behavior aligned |
	| 6 | SIMD V256_Mul_I32 overload rename | ✅ 已修复 (2026-06-11) | CRLF→LF + .gitattributes 强制 LF |
	| 7 | RunFactMode SEH longjmp gap | ✅ 已修复 (2026-06-10) | g_chaos_fail_hook longjmp to uninitialized jmp_buf |

### P1（高优先级）

| # | 问题 | 状态 | 根因 |
|---|------|------|------|
| 1 | emit-patch-data AOT Core IR 复用 | ⚠️ 已知限制 | Subject_N 合成方法省略 |
| 2 | Benchmark p1_VIOLATION: enum/guid | ⚠️ 架构限制 | GC alloc vs JIT stack allocation |
| 3 | Cross-assembly null check limit | ⚠️ 架构限制 | 16 failures |
| 4 | G-9: LOH 空闲 segment 即时 VirtualFree | ✅ 已实现 | Sweep() 保留 ≤4 空闲 segment，超出立即 PalVirtualFree + 注销卡表 |
| 5 | G-11: Promotion 年龄阈值 1→2 | ✅ 已实现 | promotion_age_threshold_ 默认=2，scheduler 动态调整 (1~12)，基于 EMA 存活率 |
| 6 | G-18: 值类型嵌套引用写屏障假阳性 | ✅ 已修复 (2026-06-11) | CodegenStFld / CodegenStObj 的 SATB pre-write barrier 加 chaos_is_gc_pointer 守卫 |

### P2（增强）

| # | 问题 | 状态 |
|---|------|------|
| 1 | GC Phase 3 (Thread hijacking + Server GC) | ✅ 完成 (2026-06-11) |
| 2 | ARM64 JIT 全量验证 | 🔄 基础支持 |

## 关键术语

| 术语 | 含义 |
|------|------|
| AOT | Ahead-of-Time compilation (native C++ codegen) |
| BGC | Background GC (concurrent mark/sweep thread) |
| CRAG | Chaos Region-based Adaptive GC |
| EH | Exception Handling |
| IOC | Interface Offset Cache |
| JIT | Just-In-Time compilation |
| LCAC | Layered Cross-Assembly Compilation |
| LOH | Large Object Heap |
| OSR | On-Stack Replacement |
| SATB | Snapshot-At-The-Beginning (GC marking) |
| TCVC | Thread-local VTable Cache |
| TLAB | Thread-Local Allocation Buffer |
| TPG | Test Project Generator |
