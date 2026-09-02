# GC-工业化 P2 残余并发 bug 审计报告

> 日期：2026-09-02
> 审计范围：5 个 P2 已知 bug（源自 roadmap-v1-01.md exit_criteria）
> 审计方法：代码阅读 + git log 回溯

---

## 审计结论一览

| # | Bug | 状态 | 证据 |
|---|-----|:----:|------|
| 1 | `chaos_is_gc_pointer` 盲点 | ✅ 已修复 | `gc_api.h:112-141` 含 `IsNurseryPointer` + `IsPohPointer` 三路检查 |
| 2 | `CodegenStSFld` 三路不一致 | ✅ 已审计为误判撤销 | `g_static_fields` 是解释器 sandbox 非 GC 向量，补 dirty_card 反而危险 |
| 3 | L1 卡表恒定/RCU | ✅ 已修复 | `gc_card_table.cpp:39` `if (g_card_bundle != nullptr) return; // never realloc` |
| 4 | BGC root-scan 自栈遗漏 | ⚠️ 未完全修复 | 见下 |
| 5 | CrossPageCompact 自栈遗漏 | ✅ 已修复 | `gc_old_gen.cpp:2424-2438` 自栈保守扫描已实现 |

---

## Bug 4: BGC root-scan 自栈遗漏（未完全修复）

### 根因
`BgcController::PopulateRootSet()`（`gc_bgc.cpp:696-850`）在 **Phase 1c** 中调用 `threading::GcScanAllThreadRoots()` 扫描所有线程栈。但 `GcScanAllThreadRoots` **跳过当前（调用）线程的栈**——即 BGC 线程本身的栈。

如果 BGC 线程在启动 root-scan 前持有 managed 对象引用（例如通过回调、事件处理、或 pre-allocated 缓冲中的引用），这些引用不会被扫描，导致对应的 old-gen 对象被错误地回收。

### 代码证据
- `gc_bgc.cpp:781` 调用 `GcScanAllThreadRoots` → 跳过 BGC 线程自身
- `gc_old_gen.cpp:2424-2438` 有**相同的自栈遗漏问题但已修复**（full GC 的 RelocateRoots 中进行了自栈保守扫描），但 **BGC 的 PopulateRootSet 没有对应的自栈扫描**

### 风险等级
- **低**：BGC 线程在 root-scan 前通常不持有 managed 引用（它是 GC 线程，不是 mutator）。但若 BGC 线程在之前阶段处理了回调或持有工作队列中的引用，则存在 UAF 风险。
- 这是**理论 bug**，实际触发条件极苛刻（BGC 线程栈上有 live managed 引用）。

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
`git log` 显示此修复与 `3e020aa28 fix(gc): S2 — root-cause full-GC SEGFAULT: RelocateRoots self-stack underflow` 相关，已作为 S2 根因修复的一部分完成。

---

## 剩余工作

### 可操作项（简单修复）
1. **BGC PopulateRootSet 自栈扫描**（Bug 4）：复制 `gc_old_gen.cpp:2424-2438` 的自栈扫描模式到 `gc_bgc.cpp:820` 附近。约 15 行代码，低风险，完成后 BGC root-scan 与 full GC RelocateRoots 行为一致。

### 评估后的建议（不强制）
2. **单 region 分配器**：P1 最后开放项，但这是**数周的大重构**，需要：
   - CI 护网（ASAN/TSAN/stress）首绿验证
   - 架构级 brainstorm + 设计
   - 不在本审计范围内