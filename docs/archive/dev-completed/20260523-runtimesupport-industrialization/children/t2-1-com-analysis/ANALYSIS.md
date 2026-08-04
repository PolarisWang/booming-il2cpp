# COM Interop ANALYSIS.md

> **task_id**: t2-1-com-analysis
> **生成日期**: 2026-05-23
> **task_type**: analysis
> **parent_task_id**: 20260523-runtimesupport-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap

## Scope

分析 `src/native/runtime-core/` 下全部 COM interop 代码的质量、结构、职责、风险和测试覆盖缺口。

## 文件清单

| 文件 | 行数 | 职责 |
|------|------|------|
| `com_abi.h` | 20 | IUnknownVtbl POD 定义 |
| `com_platform.h` | 57 | 平台抽象层声明 |
| `com_platform.cpp` | 175 | IUnknown vtable 调用、CoCreateInstance、BSTR helper |
| `com_ccw.h` | 147 | CCW 数据结构 + API 声明 |
| `com_ccw.cpp` | 332 | CCW 生命周期、IUnknown 实现、聚合、接口注册、DispatchMethod |
| `com_rcw.h` | 73 | RCW 数据结构 + API 声明 |
| `com_rcw.cpp` | 126 | RCW 全局表、缓存、生命周期 |
| `com_connection_point.h` | 88 | 连接点数据结构 + API 声明 |
| `com_connection_point.cpp` | 213 | Advise/Unadvise/FindConnectionPoints/CreateEventSinkCcw |

**总计**: ~1,231 行

## 职责边界

```
com_abi.h          → 纯 ABI 类型（IUnknownVtbl），零逻辑
com_platform.{h.cpp} → 平台抽象层，隔离 Win32 API 依赖
com_ccw.{h.cpp}    → COM Callable Wrapper — managed → COM
com_rcw.{h.cpp}    → COM Runtime Callable Wrapper — COM → managed
com_connection_point.{h.cpp} → COM 事件连接点（Advise/Unadvise）
```

## 质量评级

### com_platform.cpp — ⭐⭐⭐⭐

- Win32 路径走真实 IUnknown vtable 调用，非 Win32 返回 E_NOTIMPL
- CoCreateInstance 正确传递 CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER
- Dynamic resolution of oleaut32 (no link-time dependency)
- BSTR helpers resolve dynamically and cache function pointers

**问题**: 无显著问题

### com_ccw.cpp — ⭐⭐⭐⭐（含 caveat）

- 正确的 IUnknown 三方法实现（QI/AddRef/Release）
- COM 聚合支持完整（identity rule、delegation、outer_unknown lifecycle）
- 接口注册使用 SmallVector（4 个 inline slot，超限动态分配）
- GCHandle 管理 managed object lifecycle
- CcwFromInterface 使用 ccw_ptr back-pointer

**问题**:
1. **CcwDispatchMethod 不支持多参数方法** — 函数签名 `void (*)(void*)` 只传 obj 无参，接口方法调用无参数传递机制。实际 COM 接口方法会有多个参数。当前只适用于 0-参数通知回调。影响：需要设计参数编组机制。
2. **无 COM  apartment 支持** — 没有 STA/MTA 同步。如果 COM 在 STA 线程调用，managed object 访问可能并发。
3. **IDispatch stub** — GetTypeInfoCount/GetTypeInfo 返回 E_NOTIMPL。非 blocking（V1 不做 IDispatch）。
4. **ResolveCcw 冗余分支** — 当 ccw_ptr != nullptr 时才用 entry->ccw_ptr 回退，但实际上所有 RegisterCcwInterface 都设置了 ccw_ptr。保留它是安全的防御性代码。

### com_rcw.cpp — ⭐⭐⭐⭐

- 全局 RCW 表（unordered_dense 表）mutex 保护
- identity_unknown-based 查找，正确的 COM identity 语义
- 固定 8-slot interface cache（per-RCW，无锁——写一次读多次）
- wrapper_refcount 控制生命周期

**问题**:
1. **Interface cache 无锁读有 risk** — `cache_count` 可能被并发写（Advise 触发 QI 新接口）。但实际场景中 cache 只有 grow 操作（cache_count 递增），reader 看到过时 `cache_count` 只会 miss 新条目，不会崩溃。评为 low risk。
2. **RCW 销毁在锁内执行** — `ReleaseRcw` 在持锁期间调用 `PlatformRelease`（触发 COM Release），可能进入 COM 回调（重入锁）。当前 COM 回调会进 QI（RCW 查找），但全局表已经 erase 该 entry，find 会 miss — **不会死锁**，但 RCW 销毁质量可优化。
3. **没有 wrapper 泄漏检测** — 如果 managed wrapper GC 忘记调用 ReleaseRcw，RCW 泄漏。这由 managed side 负责，native 层不做 GC 扫描。

### com_connection_point.cpp — ⭐⭐⭐⭐

- Advise/Unadvise 使用链表 + cookie
- 延迟分配 ConnectionPointContainer（首次 Advise 时）
- sink AddRef/Release 正确管理引用计数
- 每个 IID 一个 ConnectionPointList，支持 8 个 source interface

**问题**:
1. **ComFindConnectionPoints 扫描 CCW 接口表而非 ConnectionPointList** — 语义上是"CCW 是否支持这个 source interface"，但真正的 FindConnectionPoints 应返回 IID → ConnectionPoint 映射。当前实现检查 CCW 接口注册（QI 路径），不是 ConnectionPointList。正确但非标准的解读。
2. **ComCreateEventSinkCcw 使用 GetCurrentRuntimeState()** — 依赖 thread-local runtime state，多线程场景可能为 nullptr。
3. **Unadvise 找不到 cookie 返回 CONNECT_E_NOCONNECTION** — 正确。但如果 cookie 已经被 unadvise 过，当前实现返回 NOCONNECTION（幂等），这符合 COM 规范。

## P0/P1 问题判定

### P0（阻断）: 0 个

### P1（严重）: 1 个

**CcwDispatchMethod 不支持多参数方法**（com_ccw.cpp:294-329）
- 描述: 函数签名 `void fn(void*)` 硬编码了 0-参数调用。COM 接口方法通常带有多个输入/输出参数。
- 影响: 事件通知仅支持无参回调。任何有参 COM 事件通知（如 PropertyChanged(sender, args)）无法工作。
- 修复方案: 在 ComCcw 中存储 method call thunk 或使用可变参数模板 dispatch。或由 codegen 为每个 COM 接口生成 thunk。
- 优先级: P1（因为当前集成场景用不到有参 COM 事件）

### P2（次要）: 3 个

1. **RCW 销毁在持锁期间触发 COM Release**（com_rcw.cpp:64-93）
   - 潜在重入问题，但当前不会死锁
   - 修复: ReleaseRcw 先从全局表 erase，release 操作移到锁外

2. **ComCreateEventSinkCcw 缺少 event_iid 注册**（com_connection_point.cpp:186-210）
   - 创建了 CCW 但未使用 event_iid 注册对应接口
   - 调用者需额外 RegisterCcwInterface

3. **无测试覆盖** — 当前 3 个 COM 文件零测试

## 结构告警

1. **HRESULT 常量重复定义** — com_ccw.h 和 com_connection_point.h 各自定义了 kE_POINTER 等 HRESULT 常量。com_ccw.h 使用 `CHAOS_IL2CPP_INT32`，com_connection_point.h 使用 `uint32_t`。两种 signedness 共存。

2. **com_platform.cpp 中 ResolveOleAut32Proc 函数模板使用 GetModuleHandleW + LoadLibraryW** — 模块句柄泄漏（s_mod 存储在全局静态变量中，但 LoadLibrary 永远不会 FreeLibrary）。不过在当前架构中这是有意为之——DLL 需要在程序生命周期内保持加载。不算 bug。

3. **CcwDispatchMethod 的 iface_map walk 使用 O(n) 扫描** — `warm->iface_map[]` 线性扫描。建议: 接口数量通常 < 10，O(n) 可接受。

## 文件与职责

当前结构合理：
- `com_platform` 层隔离 OS 依赖
- `com_ccw` / `com_rcw` / `com_connection_point` 各司其职
- 没有职责混入或重复抽象

无需文件拆分或合并。

## 三优先级权衡

| 优先级 | 当前评估 |
|--------|---------|
| 性能最优 | CCW/RCW 缓存设计到位（SmallVector、8-slot RCW cache）；DispatchMethod 使用原始 vtable slot 而非反射查找 |
| 架构完美 | 结构清晰，职责分离，平台抽象到位。HRESULT signedness 问题是唯一架构瑕疵 |
| 热更适配 | COM CCW 通过 type_info.iface_map 查找接口方法，与热更兼容（热更后 type_info 指针更新） |

## 测试策略

测试分为三个层次：

1. **单元测试 — com_platform**
   - 只测试非 Win32 stub（Win32 路径依赖真实 COM 运行时，隔离测试不可行）
   - 验证空指针返回默认值
   - 验证 BSTR helpers 在非 Win32 下返回 nullptr/0

2. **单元测试 — com_ccw**
   - 创建 CCW（用 mock GCHandle 函数）
   - QI 测试：IUnknown 返回 self、已注册接口返回 vtable 地址、未注册接口返回 E_NOINTERFACE
   - AddRef/Release 测试：refcount 正确增减，Release → 0 触发 cleanup
   - 聚合测试：aggregated CCW 的 QI/AddRef/Release 委托到 outer
   - RegisterCcwInterface：正常注册、超限、重复注册
   - CreateCcw + DestroyCcw 生命周期
   - 注：GCHandle 使用 stub（CHAOS_GC_HANDLE_INVALID 表示无 GC handle）

3. **单元测试 — com_rcw**
   - FindOrCreateRcw：新建 + 复用
   - ReleaseRcw：refcount 递减 + 0 时释放 + double release
   - QueryInterfaceCached：缓存命中走缓存，未命中 fallthrough
   - 注：PlatformQueryInterface 使用 stub（非 Win32 返回 E_NOTIMPL）

4. **单元测试 — com_connection_point**
   - ComFindConnectionPoints：正确扫描 CCW 接口表
   - ComAdvise/ComUnadvise：正常 Advise/Unadvise、重复 cookie 查找、空 ccw/sink
   - ComCreateEventSinkCcw：正常创建、null delegate

## 测试依赖

| 依赖 | 说明 |
|------|------|
| GCHandle stub | com_ccw 测试需要 `abi->gc_handle_new` / `gc_handle_free` / `gc_handle_get` 可调用 |
| RuntimeState | CreateCcw 需要 runtime_state 参数，测试中传 nullptr |
| memory_domain | DomainCurrentAllocateTagged 需要 MemoryDomain |
| GetCurrentRuntimeState | ComCreateEventSinkCcw 依赖 thread-local runtime state |

## 测试文件组织

建议新建 `testing/src/native/runtime-core/com_interop_test.cpp`（单一文件），避免为每个小模块创建多个测试目标。

## P0/P1 修复计划

**P1: CcwDispatchMethod 多参数支持** — 不在本次测试阶段修复。这是产品功能升级需求，需要 codegen 侧配合。

在 t2-3-com-fixes 中修复：
1. HRESULT signedness 统一
2. RCW 销毁锁范围优化（如有需要）
