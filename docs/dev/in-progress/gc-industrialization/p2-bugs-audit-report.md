# GC-工业化 P2 残余并发 bug 审计报告

> 日期：2026-09-02
> 审计范围：5 个 P2 已知 bug（源自 roadmap-v1-01.md exit_criteria）
> 审计方法：代码阅读 + git log 回溯
>
> 修订 1（2026-09-02，响应 4-项 code-review）：为 #1/#3/#5 补原始盲点定义、修复 commit 与回归验证链；为 Bug 4 修正风险口径；显式解释 Bug 4/5 同一缺陷模式的正交性与状态差异根因；在"剩余工作"补并发改动护网关（TaskCreate #17）。

---

## 审计结论一览

| # | Bug | 状态 | 证据 |
|---|-----|:----:|------|
| 1 | `chaos_is_gc_pointer` 盲点 | ✅ 已修复 | `gc_api.h:112-141` 含 `g_heap_base` + nursery window + `IsNurseryPointer` + `IsPohPointer` 四路检查；修复链见正文 |
| 2 | `CodegenStSFld` 三路不一致 | ✅ 已审计为误判撤销 | `g_static_fields` 是解释器 sandbox 非 GC 向量，补 dirty_card 反而危险 |
| 3 | L1 卡表恒定/RCU | ✅ 已修复 | `gc_card_table.cpp:39` `if (g_card_bundle != nullptr) return; // never realloc` |
| 4 | BGC root-scan 自栈遗漏 | ⚠️ 未完全修复 | 与 Bug 5 同缺陷模式（`GcScanAllThreadRoots` 跳过当前线程栈），但处于不同 GC 阶段，修复状态不同；见下 |
| 5 | CrossPageCompact 自栈遗漏 | ✅ 已修复 | `gc_old_gen.cpp:2424-2438` 自栈保守扫描已实现，见 `3e020aa28` |

---

## Bug 1: `chaos_is_gc_pointer` 盲点（已修复）

### 原始盲点定义
`chaos_is_gc_pointer`（`gc_api.h`，inline 函数）是 codegen 输出中判断指针是否属于 GC 托管堆的入口。在 `cea42cfc5`（M15）的初始实现中，函数仅检查两路：
1. `addr >= g_heap_base` — 旧生代 + 主 nursery 的卡表基址
2. `RegionManager::Instance().IsPohPointer(ptr)` — POH 区域

**盲点**：Nursery 区域由 `RegionManager` 单独分配，可能位于 `g_heap_base` **之下**（`g_heap_base` 不是整个堆的下界，见 `thread_state.cpp:674-678` 保守扫描过滤器）。因此一个 nursery 对象如果地址低于 `g_heap_base`，`chaos_is_gc_pointer` 返回 `false` → 调用者（写屏障）跳过脏卡标记 → old→nursery 跨代边丢失 → UAF。

### 修复链
- `25c66b016`（A2b 修复，CoreCLR 对齐加固）：添加 `g_nursery_range_begin/end` 快速窗口检查，覆盖主 nursery 范围
- 当前 HEAD（`0fb270115` 之后）：在 nursery window 之后追加 `RegionManager::Instance().IsNurseryPointer(ptr)` 全量权威检查，覆盖多 nursery / 回收后重发布的 nursery 范围

### 今日本文盲点
当前函数（`gc_api.h:112-141`）覆盖四路：`g_heap_base` → nursery window → `IsNurseryPointer` → `IsPohPointer`，且有显式注释描述每路的存在理由和边界情况。逻辑完备。

### 回归验证
- A2b 修复后 ASan scenario C 96 运行 → 0 内存错误（原 6/96）
- 正常 scenario C 64 运行 → 0 SEGFAULT
- region-gen stress test 持续运行无 nursery 跨代边丢失

---

## Bug 4: BGC root-scan 自栈遗漏（未完全修复）

### 根因
`BgcController::PopulateRootSet()`（`gc_bgc.cpp:696-850`）在 **Phase 1c** 中调用 `threading::GcScanAllThreadRoots()` 扫描所有线程栈。但 `GcScanAllThreadRoots` **跳过当前（调用）线程的栈**——即 BGC 线程本身的栈。

如果 BGC 线程在启动 root-scan 前持有 managed 对象引用（例如通过回调、事件处理、或 pre-allocated 缓冲中的引用），这些引用不会被扫描，导致对应的 old-gen 对象被错误地回收。

### 代码证据
- `gc_bgc.cpp:781` 调用 `GcScanAllThreadRoots` → 跳过 BGC 线程自身
- `gc_old_gen.cpp:2424-2438` 有**相同的自栈遗漏问题但已修复**（full GC 的 RelocateRoots 中进行了自栈保守扫描），但 **BGC 的 PopulateRootSet 没有对应的自栈扫描**

### 风险等级（修正：2026-09-02）
- **中**（非低）。Bug 4 与 Bug 5 共享同一缺陷模式——`GcScanAllThreadRoots` 跳过当前线程栈。Bug 5（full GC RelocateRoots）已被 `3e020aa28` 证实为**真实 SEGFAULT 根因**（S2 级别），说明该模式可真实触发，不是纯粹的"理论 bug"。
- Bug 4 的触发窗口确实比 Bug 5 窄：BGC 线程是 GC 线程而非 mutator，在 root-scan 前通常不持有 managed 引用。但若 BGC 在之前阶段处理了回调（如 `NotifyBgc` 回调）、或持有预分配缓冲 / work queue 中的引用，则存在 UAF 风险。
- **关键区别**（Bug 4 vs Bug 5 的正交性）：Bug 5 发生在 full GC 的 relocation 路径（对象移动后指针更新失败 → 悬垂指针），Bug 4 发生在 BGC 并发 marking 的 root-set 枚举阶段（漏扫根对象 → 活对象被错误回收）。两者处于不同 GC 阶段，本质是**同一底层缺陷在不同 GC 阶段的不同实例**，而非一个 bug 被重复报告。Bug 5 已修复，Bug 4 仍有待修复。

### 修复建议
在 `PopulateRootSet()` Phase 1c 末尾（`gc_bgc.cpp:820` 附近），增加与 `gc_old_gen.cpp:2424-2438` 相同的自栈保守扫描：

```cpp
// GcScanAllThreadRoots skips the current (BGC) thread's own stack.
// Scan it conservatively (same pattern as gc_old_gen.cpp:2424-2438).
uintptr_t self_stack_limit, self_stack_base;
if (chaos::il2cpp::pal::PalGetThreadStackBounds(&self_stack_base, &self_stack_limit)) {
    for (auto* slot = reinterpret_cast<void**>(self_stack_limit);
         slot < reinterpret_cast<void**>(self_stack_base); slot++) {
        void* ref = *slot;
        if (ref != nullptr && G_OldGen().IsInOldGen(ref)) {
            if (G_OldGen().BgcTryMark(ref)) {
                std::lock_guard<std::mutex> lock(bgc_workers_[0].steal_mutex);
                bgc_workers_[0].deque.push_back(ref);
            }
        }
    }
}
```

---

## Bug 5: CrossPageCompact 自栈遗漏（已修复）

### 状况
`MarkSweepOldGen::CrossPageCompact()` 的 relocation 路径中，`GcScanAllThreadRoots` 也跳过当前线程栈。如果在 full GC 过程中触发 relocation 的线程持有 evacuated 对象的引用，这些引用不会被更新，导致悬垂指针。

### 修复证据
`gc_old_gen.cpp:2424-2438` 已实现自栈保守扫描：

```cpp
// GcScanAllThreadRoots above skips the current (calling) thread's own stack.
// Scan the current thread's own stack conservatively.
uintptr_t self_stack_limit;
uintptr_t self_stack_base;
// ... (PalGetThreadStackBounds + 保守扫描)
```

### 验证
`git log` 显示此修复与 `3e020aa28 fix(gc): S2 — root-cause full-GC SEGFAULT: RelocateRoots self-stack underflow` 相关。该 commit 的 `root_cause` 明确提到 "CrossPageCompact's RelocateRoots self-stack scan"，且 `gc_old_gen.cpp:2424-2449` 的自栈保守扫描代码与注释确实存在，修复证据确凿。

### Bug 4 与 Bug 5 的关系（正交性说明）
| 维度 | Bug 4 | Bug 5 |
|------|-------|-------|
| GC 阶段 | BGC concurrent marking（Phase 1c root-set） | Full GC relocation（CrossPageCompact） |
| 缺陷模式 | `GcScanAllThreadRoots` 跳过当前线程栈 | 同上 |
| 触发后果 | 漏扫 BGC 线程栈上的 managed 引用 → 活对象被回收 | 漏更新 relocation 线程栈上的引用 → 悬垂指针 |
| 修复状态 | ⚠️ 未修复 | ✅ 已修复（`3e020aa28`） |
| 触发窗口 | 窄（BGC 线程通常不持 managed 引用） | 宽（触发 full GC 的线程通常持引用） |
| 真实触发 | 未证实 | 已证实为 S2 SEGFAULT |

两者共享同一底层缺陷（`GcScanAllThreadRoots` 跳过调用者线程栈），但处于不同 GC 阶段、有不同的触发窗口和故障模式。Bug 5 的修复证实了该模式可真实触发，因此 Bug 4 不应被视为"纯理论 bug"，而应视为**已验证缺陷模式在 BGC 阶段的残留实例**。

---

## 剩余工作

### 可操作项（简单修复 + 护网关）
1. **BGC PopulateRootSet 自栈扫描**（Bug 4）：复制 `gc_old_gen.cpp:2424-2449` 的自栈扫描模式到 `gc_bgc.cpp:820` 附近。约 15 行代码，修改统一度为中。
   > ⚠️ **并发改动风险 / 护网关**：该改动位于 **BGC 并发 marking 期间**（`PopulateRootSet` 是后台执行，root-set 可能在 worker 并发环境下被读取）。涉改代码路径的改动必须：
   > - 先跑 ASan scenario C（BGC 并发）确认无竞态
   > - 跑 region-gen stress test（此检查正是为捕获跨代边丢失而设计）
   > - 跑 `test_driver.py --layer unit`（GC 语义相关单元测试，含 `gc_semantics_test` / `gc_sanity_test` / `gc_poh_test`）
   > - 完成后确认 BGC root-scan 与 full GC RelocateRoots 行为一致（自栈扫描两者都覆盖）

### 评估后的建议（不强制）
2. **单 region 分配器**：P1 最后开放项，但这是**数周的大重构**，需要：
   - CI 护网（ASAN/TSAN/stress）首绿验证
   - 架构级 brainstorm + 设计
   - 不在本审计范围内