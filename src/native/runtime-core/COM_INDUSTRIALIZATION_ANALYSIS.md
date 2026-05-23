# COM Interop 工业化评估报告

## 概述

评估 COM Interop 模块（CCW、RCW、ConnectionPoint）的当前完成度、工业化差距、风险、并给出推进计划。

## 评估范围

| 文件 | 行数 | 职责 |
|------|------|------|
| `src/native/runtime-core/com_abi.h` | 22 | IUnknownVtbl struct 定义 |
| `src/native/runtime-core/com_ccw.h` | 135 | CCW 头文件：ComCcw struct、接口声明 |
| `src/native/runtime-core/com_ccw.cpp` | 299 | CCW 实现：QI/AddRef/Release/聚合/CcwDispatchMethod |
| `src/native/runtime-core/com_rcw.h` | 73 | RCW 头文件：ComRcwNative struct |
| `src/native/runtime-core/com_rcw.cpp` | 144 | RCW 实现：全局表/缓存/QI |
| `src/native/runtime-core/com_connection_point.h` | 88 | ConnectionPoint 头文件 |
| `src/native/runtime-core/com_connection_point.cpp` | 214 | ConnectionPoint 实现：Advise/Unadvise |

测试文件（6 个）：runtime-core 3 个（基本空安全）+ hot-update 3 个（mock 全面测试）。

---

## 1. 完成度审查

### 1.1 CCW

| 功能 | 状态 | 备注 |
|------|------|------|
| CreateCcw | 完成 | malloc 分配 + GCHandle 生命周期 |
| CcwQueryInterface | 完成 | IUnknown 匹配 + 非聚合/聚合路径 |
| CcwAddRef / CcwRelease | 完成 | 原子操作，自销毁 |
| RegisterCcwInterface | 完成 | 最多 kMaxCcwInterfaces(4) |
| CreateCcwAggregated | 完成 | 聚合模式 + outer 引用管理 |
| DestroyCcw | 完成 | 仅聚合 CCW |
| CcwFromInterface | 完成 | inline 函数，还原 CCW 指针 |
| CcwDispatchMethod | 完成 | 走 TypeInfoWarm::iface_map 分派 |
| IDispatch: GetTypeInfoCount | 桩 | 返回 0 (无类型信息) |
| IDispatch: GetTypeInfo | 桩 | 返回 E_NOTIMPL |

### 1.2 RCW

| 功能 | 状态 | 备注 |
|------|------|------|
| FindOrCreateRcw | 完成 | 全局 mutex 保护表 |
| ReleaseRcw | 完成 | refcount 管理 + 缓存接口释放 |
| QueryInterfaceCached | 完成 | 线性扫描缓存 → 回退真实 QI |
| IsComRcwHandle | 完成 | Magic 校验 |
| 全局 RCW 表 | 完成 | unordered_dense_map 指针 key |

### 1.3 ConnectionPoint

| 功能 | 状态 | 备注 |
|------|------|------|
| ComFindConnectionPoints | 完成 | 扫描 CCW 已注册接口 |
| ComAdvise | 完成 | 懒分配容器 + 链表 + AddRef sink |
| ComUnadvise | 完成 | 链表删除 + Release sink |
| ComCreateEventSinkCcw | 完成 | 创建 CCW 包装 delegate |

### 1.4 测试覆盖

**runtime-core 测试**（使用 MarshalTestFixture，基本空安全）：
- com_ccw_test: 16 测试 — null QI/AddRef/Release + RegisterInterface + CreateCcw 空值
- com_rcw_test: 9 测试 — IsComRcwHandle + FindOrCreateRcw 空值 + Release 空值
- com_connection_point_test: 8 测试 — 全部是空参数检查

**hot-update 测试**（使用 Mock IUnknown/RuntimeState，全面场景）：
- com_ccw_test: 12 测试 — 完整 CCW lifecycle + QI/AddRef/Release + RegisterInterface + Aggregated
- com_rcw_test: 6 测试 — 完整 RCW lifecycle + 重复查找 + 引用计数
- com_connection_point_test: 7 测试 — Advise/Unadvise + 多 sink + 不同 IID

---

## 2. 工业化差距分析

### 2.1 必须修复 (P0)

| # | 问题 | 位置 | 影响 |
|---|------|------|------|
| G1 | `std::malloc`/`std::free` 直接使用 | com_ccw.cpp:136, com_rcw.cpp:38 | 违反统一内存分配约束。应替换为 `CHAOS_IL2CPP_MALLOC`/`CHAOS_IL2CPP_FREE` |
| G2 | LOG_WARN 缺失 | com_ccw.cpp:261-296 (CcwDispatchMethod), com_ccw.cpp:112-121 (IDispatch stubs) | 所有 stub 路径必须打 LOG_WARN |
| G3 | CcwDispatchMethod 无测试 | com_ccw.cpp:261-296 | 关键功能零覆盖 |
| G4 | RCW 测试 Win32 跳过 | com_rcw_test.cpp:68-86 (runtime-core), com_rcw_test.cpp:84-101 (hot-update) | 核心测试在 Windows 上无法自动运行 |

### 2.2 建议改进 (P1)

| # | 问题 | 位置 | 影响 |
|---|------|------|------|
| I1 | CCW 无 edge case 测试 | com_ccw_test.cpp (runtime-core) | 缺少满接口表 QI、聚合路径、Release 自销毁等边界 |
| I2 | ConnectionPoint 无 edge case 测试 | com_connection_point_test.cpp (runtime-core) | 缺少 Advise 满、Unadvise 不存在的 cookie、重复 Unadvise |
| I3 | CHAOS_IL2CPP_MEMCMP 宏替换不完整 | com_ccw.cpp:57 已用 | 已基本完整，但应确认所有 memcmp 都已替换 |

### 2.3 可延迟 (P2)

| # | 问题 | 位置 | 影响 |
|---|------|------|------|
| D1 | IDispatch 完整实现 | com_ccw.h:74-75 | 需要 typeinfo 集成，延迟到 Phase 4+ |
| D2 | CcwDispatchMethod 异常处理 | com_ccw.cpp:261 | 缺少 SEH 包装，延迟到异常工业化 |
| D3 | RCW 缓存淘汰策略 | com_rcw.cpp:128-133 | kMaxInterfaceCache=8 满后不缓存新结果 |

---

## 3. 风险评估

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|----------|
| std::malloc 导致分配域污染 | 中 | 高 | G1 修复：替换为 CHAOS_IL2CPP_MALLOC |
| CcwDispatchMethod 在未测试路径崩溃 | 低 | 高 | G3 修复：添加 iface_map 测试 |
| RCW Win32 QI 路径不可测 | 中 | 中 | G4 修复：提供 mock IUnknown 测试设施 |
| ConnectionPoint cp_container 空指针 | 低 | 中 | ComUnadvise 已有 null 检查 |
| CHAOS_IL2CPP_MEMCMP 宏在非 MSVC 平台 | 低 | 低 | 已在 main 分支使用，跨平台已验证 |

---

## 4. 建议推进顺序

基于 gap 严重性和依赖关系：

```
Phase 2-1: Memory allocation fix (G1)          [独立，高优先级]
Phase 2-2: LOG_WARN stubs (G2)                [独立，高优先级]
Phase 2-3: Comprehensive CCW/RCW/CP tests     [需要 G1 基础]
Phase 2-4: Edge case + benchmark tests        [依赖 2-3]
Phase 2-5: Documentation                       [最后]
```

### 阶段详情

#### Phase 2-1: 统一内存分配修复
- `com_ccw.cpp`: `std::malloc` → `CHAOS_IL2CPP_MALLOC`, `std::free` → `CHAOS_IL2CPP_FREE`
- `com_rcw.cpp`: 同上
- `com_connection_point.cpp`: `CHAOS_IL2CPP_MALLOC`/`CHAOS_IL2CPP_FREE` 已用，无需修改

#### Phase 2-2: LOG_WARN 补充
- `CcwGetTypeInfoCount` / `CcwGetTypeInfo`: 添加 CHAOS_IL2CPP_LOG_WARN
- `CcwDispatchMethod`: 添加 CHAOS_IL2CPP_LOG_WARN (full stub path)

#### Phase 2-3: 测试增强
- CCW: 满接口表 QI + 聚合 QI/AddRef/Release + Release 自销毁
- RCW: mock IUnknown 模式下边界的 QueryInterfaceCached 压力测试
- ConnectionPoint: Advise 满容器、重复 Unadvise、跨 IID Unadvise

#### Phase 2-4: Documentation
- 更新 wiki/ 中 COM 互操作文档（如果存在）
- 接口契约注释完善

---

## 5. 三优先级权衡

| 维度 | 评估 |
|------|------|
| 性能最优 (P1) | COM 互操作非常用路径，性能不敏感。当前实现使用 malloc/free 而非 GC 分配正确。但 std::malloc 违反分配约束，必须替换 |
| 方案完美性 (P2) | 架构清晰，职责分离（CCW/RCW/ConnectionPoint 三模块）。CcwDispatchMethod 走 iface_map 是正确的设计方向 |
| HotUpdate 支持 (P3) | COM 接口与 hotupdate 无直接冲突。CCW 通过 GCHandle 保持 managed object 存活，hotupdate 不影响 |

**结论**：无需三优先级重新裁决。直接推进工业化修复。

---

## 6. 当前测试计数

| 测试文件 | 当前数量 | 目标数量 |
|----------|----------|----------|
| runtime-core/com_ccw_test | 16 | 25+ |
| runtime-core/com_rcw_test | 9 | 15+ |
| runtime-core/com_connection_point_test | 8 | 15+ |
| hot-update/com_ccw_test | 12 | 12 (已完整) |
| hot-update/com_rcw_test | 6 | 6 (已完整) |
| hot-update/com_connection_point_test | 7 | 7 (已完整) |
| **总计** | **58** | **80+** |
