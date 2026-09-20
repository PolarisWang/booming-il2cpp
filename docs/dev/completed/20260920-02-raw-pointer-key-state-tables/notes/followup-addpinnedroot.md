# 后续排查：AddPinnedRoot「零调用者」——**结论：非缺陷（我此前判断有误）**

> 日期：2026-09-20
> 起因：修 `GcSetHandleTarget`（零调用者）时顺带发现 `AddPinnedRoot` 在生产路径
> 也无调用者，当时记录为「GC 的 pin 判定机制未接线」。

## 纠正

**此前判断错误。** 原因是我当时的 `grep` **只搜了 `src/`，漏了 `tests/`**，
据此得出「零调用者」。完整搜索后：

| 路径 | `AddPinnedRoot` 调用 |
|:-----|:--------------------|
| `src/`（生产） | **0** |
| `tests/` | **多处**（`delegate_stress_test.cpp`、`gc_finalizer_integration_test.cpp`、`gc_stress_test.cpp` 等） |

## 真实结论：这是设计，不是漏接线

生产路径的「pin」有**两条独立且都已接线**的机制，都不经 `AddPinnedRoot`：

| 机制 | 语义 | 生产使用点 |
|:-----|:-----|:-----------|
| `GcCreatePinnedHandle` / `GcCreateAsyncPinnedHandle` | pin **已存在**的单个对象 | `abi_reflection.cpp:72`、`gc_api.cpp:253` |
| `PohAllocate` / `GcAllocatePinned` | 分配即在 POH（天然不搬移） | `gc_old_gen.cpp:1009`、`gc_region.cpp:1455`、`enum_stubs.cpp:100` |

而 `AddPinnedRoot(void* addr, size_t size)` 注册的是**地址范围**
（`gc_old_gen.h:295-296`：「object that must never be moved/collected」），
消费路径完整：`pinned_compact_skip_ = pinned_roots_`（`gc_old_gen.cpp:3146/3774`）
→ 压缩时跳过这些页。

**它是留给测试的范围级工具** —— 压力测试用它手动固定一批对象，模拟
「GC 不搬移」的场景。生产代码没有这个需求，故不调用。

## 与 `GcSetHandleTarget` 的区别（为什么那个是真缺陷）

| | `GcSetHandleTarget` | `AddPinnedRoot` |
|:--|:--------------------|:----------------|
| 生产调用者 | 0 | 0 |
| 设计意图 | **应被 GC 搬移路径调用**（它有 pin_set 同步 + SATB 维护逻辑） | 测试工具 |
| 证据 | 同逻辑已在 `GcRelocateHandles` 里补上（本任务修复），说明是遗漏 | 生产 pin 走 handle 路径，功能完整 |

判据：**「零调用者」本身不是缺陷信号**；要看该函数是否**替代了某条应有的路径**，
以及**同类功能是否有其他已接线的实现**。

## 教训

`grep` 的搜索范围决定了结论 —— 只搜 `src/` 会漏掉测试，从而把「仅测试使用」
误判为「完全没人用」。**判定调用者时必须覆盖全仓库。**

## 相关
[[gc-fastpath-missing-relocate-and-raw-key-tables]]（本次主线修复）
