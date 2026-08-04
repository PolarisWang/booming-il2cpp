# Chaos.IL2CPP vs Unity IL2CPP vs CoreCLR — 深入架构对比分析报告

> 日期: 2026-05-13
> 范围: 基于 `src/native/` 运行时 + `src/managed/Chaos.IL2CPP.CodeGen` codegen 全量分析

---

## 一、整体架构定位

| 维度 | Chaos.IL2CPP | Unity IL2CPP | .NET CoreCLR |
|------|------------|-------------|-------------|
| 编译策略 | AOT (IL→C++) | AOT (IL→C++) | JIT + 分层编译 + 可选 AOT |
| 元数据处理 | 自定义 AotCoreIr JSON + 模块注册表 | 全局元数据 binary blob (GlobalMetadata) | ECMA-335 完整元数据解析器 (lazy) |
| GC | 自研分代: 年轻代 Cheney BFS 复制 + 老年代 BDWGC 标记-清除 | Boehm GC (保守) / Unity 分代 (精确) | 自研分代式 (WKS/SVR, 压缩, 精确) |
| 目标场景 | Unity 游戏 / 自定义主机 | Unity 游戏 | 通用服务器/桌面/云 |
| 热更新 | **原生支持**: PatchMethod + HotpatchTable + .patchdata | ❌ 不原生支持 (需 Lua/ILRuntime) | ❌ 不支持 (ALC 需 JIT) |
| 解释器 | ✅ InterpreterVM (99 IR opcodes) | ❌ 无 (全 AOT) | ❌ 无 (全 JIT) |

---

## 二、子系统成熟度矩阵

### Tier 0 — 核心基础设施 (必须有)

| 子系统 | Chaos.IL2CPP | 差距等级 |
|--------|------------|---------|
| **类型系统 / MethodTable** | ✅ `method_table.cpp`, `type_registry.cpp` | **无差距** |
| **GC / 内存分配** | ✅ 自研分代: 年轻代复制 + 老年代标记-清除 + CardTable + RootScanner + Scheduler | **无差距** |
| **GC Roots / 栈扫描** | ✅ 精确扫描 (GcSlotMap) + 保守回退 | **无差距** |
| **方法派发 / VTable** | ✅ fast_dispatch, vtable_registry, runtime_vtable | **无差距** |
| **异常处理** | ⚠️ C++ try/catch throw 跨越 extern "C", 无 SEH | **中 — 无 SEH, throw 跨 extern C 有性能 penal** |
| **线程抽象** | ✅ ManagedThread, lock-free TLS 注册, GC 安全点 | **无差距** |
| **对象头 / SyncBlock** | ⚠️ 有对象头 (PureType/ThinLockable/Fat) 但 SyncBlock 表未实现 | **中 — Monitor 用 per-object map 而非 thin lock** |

### Tier 1 — 关键运行时服务

| 子系统 | Chaos.IL2CPP | 差距等级 |
|--------|------------|---------|
| **元数据读取** | ✅ AotCoreIr JSON 反序列化 + 模块注册表 | **无差距** |
| **同步原语 (Monitor)** | ⚠️ per-object unordered_map + recursive_mutex, 无 Pulse/Wait/超时 | **低 — 基础可用** |
| **线程池** | ⚠️ gate thread + FIFO queue, 1-32 worker, 无 hill-climbing | **中 — 无 IOCP, 无 hill-climbing, 无 timer** |
| **字符串支持** | ✅ StubStringHeader + C++ string 操作函数 | **无差距** |
| **数组支持** | ✅ codegen + runtime helpers | **无差距** |
| **P/Invoke / Interop** | ✅ 生成 P/Invoke wrapper + 结构体 marshalling descriptors + native DLL cache | **无差距** |
| **委托 (Delegate)** | ✅ codegen + runtime helpers (Combine/Remove) | **无差距** |
| **Assembly 加载** | ❌ AOT-only, 模块注册表扁平, 无 ALC | **高 — AOT 约束但缺少 ALC 隔离** |

### Tier 2 — 反射和元数据查询

| 子系统 | Chaos.IL2CPP | 差距等级 |
|--------|------------|---------|
| **Type.GetMembers/Methods/Fields** | ✅ reflection_api (~85 函数), 部分 SimpleForward | **无差距** — stub 转换进行中 |
| **Custom Attributes** | ✅ ChaosReflectionGetCustomAttribute + MemberInfo.IsDefined | **无差距** |
| **Invoke / MethodBase.Invoke** | ⚠️ SimpleForward stub, 未完全实现 | **低** |
| **MakeGenericType/MakeGenericMethod** | ✅ RuntimeInstantiationBridge + interpreter fallback | **无差距** |
| **Reflection.Emit** | ❌ 不支持 (AOT 限制) | **N/A** |
| **Activator.CreateInstance** | ✅ SimpleForward + GenericShapeDescriptor | **无差距** |

### Tier 3 — 调试与诊断

| 子系统 | Chaos.IL2CPP | 差距等级 |
|--------|------------|---------|
| **调试器 API (ICorDebug)** | ❌ 不存在 | **高** |
| **Profiling API (ICorProfiler)** | ❌ 不存在 (仅有 RDTSC micro-profiler) | **高** |
| **EventPipe / EventSource** | ❌ 不存在 | **中** |
| **栈跟踪 (Stack Trace)** | ⚠️ 基础实现 (exception_helpers) | **低** |
| **性能计数器** | ❌ 不存在 | **中** |
| **DAC / 离线分析** | ❌ 不存在 | **高** |

### Tier 4 — 全球化

| 子系统 | Chaos.IL2CPP | 差距等级 |
|--------|------------|---------|
| **ICU / 区域设置** | ❌ Culture stubs 全部返回 dummy | **高 — 非英语场景不可靠** |
| **字符分类 (非 ASCII)** | ⚠️ ASCII-only (Char::IsDigit/IsLetter/IsWhiteSpace) | **中** |
| **日历 / 时区** | ⚠️ DateTime parse/format 硬编码固定格式 | **低** |

### Tier 5 — 专用/互操作

| 子系统 | Chaos.IL2CPP | 差距等级 |
|--------|------------|---------|
| **COM Interop** | ❌ com_abi.h 仅 IUnknown vtable 声明 | **低 — 游戏通常不需要** |
| **WinRT 互操作** | ❌ 不存在 | **低** |
| **结构体封送** | ✅ descriptor-tree driven | **无差距** |

---

## 三、六大缺失大模块

### 1. 调试器基础设施 — 高优先级

**当前状态**: 完全无调试器支持。运行时无法响应断点、单步、变量查看。

**需要建设的核心组件**:
- `contracts/native/v0/debugger.h` — 调试器合同接口
- 断点注入机制 (在 fast_dispatch / interpreter 插入检查点)
- 栈帧遍历 API (frame walking)
- 局部/参数变量解析
- SOS 风格的 DAC (minidump 分析) — 长期

### 2. Profiling API — 高优先级

**当前状态**: 仅有 RDTSC micro-profiler (单线程 benchmark-only)。

**需要建设的核心组件**:
- `contracts/native/v0/profiler.h` — profiler 接口
- 关键事件埋点: GC、安全点、线程、方法入口/出口
- 性能数据收集和导出

> 对游戏引擎场景, profiler 优先级可适度降低, 因为游戏引擎有自己的 profiler 体系。

### 3. 全球化 / ICU 支持 — 高优先级

**当前状态**: 所有 Culture stub 返回 dummy 对象。CompareInfo、TextInfo、CultureInfo 全部空。

**实际影响**:
- 非英语字符串比较/排序不可靠
- `String.StartsWith("...", StringComparison.CurrentCulture)` 行为错误
- 数字/日期格式化无法感知区域设置

**建议路径**: CultureInfo invariant (短期) → UTF-16 字符分类表 (中期) → ICU4C/ICU4N 集成 (长期)

### 4. 线程池增强 — 中优先级

**核心差距**: 无 IOCP 集成、无 hill-climbing 算法、无 timer 支持。

**文件和代码行**: `src/native/runtime-core/thread_pool.cpp` (205 行), gate thread + FIFO queue。

### 5. Monitor / SyncBlock — 中优先级

**核心差距**: 无 thin lock, 无 `Monitor.Wait/Pulse`, 每次锁操作进行 map 查找。

**文件**: `src/native/common/chaos/monitor.h` (单函数 `require_monitor_runtime_entry`)。

### 6. 异常处理增强 — 中优先级

**核心差距**: C++ throw 跨 extern "C" 边界, 要求 MSVC `/EHs`。

**建议**: 短期维持现状, 长期实现 personality routine + OS SEH / dwarf unwinding。

---

## 四、差异化核心优势 (vs Unity IL2CPP)

| 功能 | Chaos.IL2CPP | Unity IL2CPP | 备注 |
|------|------------|-------------|------|
| **热更新** | ✅ 原生支持 | ❌ 需第三方 | **核心优势** |
| **解释器** | ✅ InterpreterVM | ❌ 纯 AOT | 热更新 + 调试器基础 |
| **GC 精确扫描** | ✅ GcSlotMap 精确 | ❌ Boehm 保守 | 更低碎片 |
| **泛型共享** | ✅ interpreter fallback | ❌ 全 specialization | 更小二进制 |
| **结构体封送** | ✅ descriptor-driven | ✅ C++ 生成 | 水平相当 |

---

## 五、建设优先级路线图

### Phase A — 核心完善 (1-2 个月)
1. 完成 `reflection_stubs.cpp` 全部剩余函数
2. 完成 RuntimeHelperShapeRegistry 全部 stub → SimpleForward (~80% 已完成)
3. 完善 Monitor: 实现 Pulse/Wait/TryEnter timeout
4. 实现非 ASCII 字符分类表

### Phase B — 全球化基础 (2-3 个月)
5. CultureInfo 返回 invariant culture (降级)
6. CompareInfo/TextInfo ordinal 比较
7. DateTime 文化感知格式化

### Phase C — 诊断/调试 (3-6 个月)
8. GC 事件回调系统
9. 方法入口/出口 hook 支持
10. 栈帧遍历 API
11. 断点注入机制 (interpreter)

### Phase D — 高级服务 (6-12 个月)
12. ThreadPool hill-climbing + IOCP
13. SyncBlock thin lock 优化
14. SEH 异常处理 (personality routine)
15. AssemblyLoadContext (如需)

---

## 六、架构成熟度评分

| Tier | 领域 | 完成度 |
|------|------|--------|
| Tier 0 | 核心基础设施 | **~90%** |
| Tier 1 | 运行时服务 | **~70%** |
| Tier 2 | 反射/元数据 | **~60%** |
| Tier 3 | 诊断/调试 | **~5%** |
| Tier 4 | 全球化 | **~5%** |

**总体结论**: Chaos.IL2CPP 在 Tier 0/1 已经达到或超过 Unity IL2CPP 水平, 核心架构优于 Unity IL2CPP (热更新、解释器、精确 GC、泛型共享)。主要差距在 Tier 3 (诊断/调试) 和 Tier 4 (全球化), 而这些对游戏引擎场景的优先级较低。