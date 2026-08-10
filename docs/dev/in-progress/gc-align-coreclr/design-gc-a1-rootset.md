# Design — GC-A1：Young GC 全根集扫描（对齐 CoreCLR + 跨平台 + AOT/JIT）

> 版本：design-v1-01
> 创建：2026-08-10
> 上游：`docs/dev/in-progress/gc-align-coreclr/roadmap-v1-01.md`（Phase 1, GC-A1）
> 优先级：P0（正确性 — 修跨线程 UAF）
> 约束：**代码级对齐 CoreCLR** + **跨平台适配** + **AOT/JIT 双适配**

---

## 1. 问题重述（为什么是 P0）

CoreCLR 的分代收集在 STW safepoint 下收集 gen0，必须扫描 **full root set** = 全部线程栈 + 寄存器 + 全局 + TLS + GCHandle + 跨代卡。漏任何一根，存活对象会被误回收 → UAF。

CRAG 现状（`gc_young_collector.cpp:418-447` Phase 0）**只扫描触发 GC 的当前线程栈**：

```cpp
// gc_young_collector.cpp:427-447
auto* current_thread = threading::GetCurrentThread();   // ← 只有触发线程
// 只遍历 current_thread 的栈，找 nursery 指针 → promote
```

但 `GcYoungCollection` 的调用方（`gc_region.cpp:990` 等）都先 `RequestGlobalSafepoint()` 停了**所有线程**。其它被挂起线程的**栈 + 寄存器**里指向本 nursery 的引用完全没被扫描。

**后果**：线程 B 栈上持有对象 O（在线程 A 的 nursery），A 触发 young GC 时 O 不可达被回收 → B 后续访问 O = **UAF**。老年代槽有脏卡兜底，但其它线程栈/寄存器是无兜底的。

这就是 24-CRAG-GC架构参考.md 里 `GcScanAllThreadRoots` 想覆盖、却被 `g_heap_base` 过滤器挡住的场景。

---

## 2. 当前为何没实现全根集：`g_heap_base` 过滤器缺陷

`thread_state.cpp:590 GcScanAllThreadRoots` 是全线程**保守**栈扫描，但每个候选根都做了堆范围过滤：

```cpp
// thread_state.cpp:633
if (read != nullptr && reinterpret_cast<uintptr_t>(read) >= g_heap_base) {
    s_callback(slot, false, s_user_data);
}
```

而 `g_heap_base` 的赋值点是 `gc_old_gen.cpp:372`：

```cpp
heap_base_ = reinterpret_cast<uintptr_t>(mem);   // 老年代首个 page 地址
GcSetHeapBase(reinterpret_cast<void*>(heap_base_));
```

**`g_heap_base` = 老年代 base，不是"整个 GC 堆的下界"。** nursery 由 RegionManager 用独立 region 区间分配，地址可能低于老年代 base。于是：

- nursery 指针 `p < g_heap_base` → 被 `>= g_heap_base` 过滤器**丢弃**，不算根。
- 这正是 `gc_young_collector.cpp:423-426` 注释自述"nursery 低于 g_heap_base，不敢用 GcScanAllThreadRoots"的原因。

**根因 = 用错误的地址判据（老年代 base）做堆成员判断。** 这是可干净修复的，不是结构性死结。

---

## 3. 对齐 CoreCLR 的正确做法（实现细节）

### 3.1 目标架构：CoreCLR 的 `GcScanRoots` ←→ 我们的 `GcScanAllThreadRoots`

CoreCLR 的 full root scan 不是保守扫栈，而是经 `IGCToCLR::GcScanRoots` 调 JIT 的 `GcScanStack`，**用每方法的 GC 偏移表（GcSlotMap）做精确根定位**。我们把 `GcScanAllThreadRoots` 从「保守 + 堆基过滤」提升为「**逐线程逐帧精确扫描（GcSlotMap）+ 兜底保守扫描**」——这正是与 CoreCLR 同构。

### 3.2 关键前置：CRAG 已有跨 AOT/JIT 的精确根记录（这是对齐的命门）

`GcSlotMapV0` 是 **codegen_bridge 的统一契约**，AOT 和 JIT 都产出它：

| 帧来源 | GcSlotMap 产出 | 注册时机 |
|--------|---------------|---------|
| **AOT**（静态编译） | `NativeAotLoweringPlanner.GcSlotMap.cs` 生成 `.gc_slot_maps` 段 | 模块加载 `GcRegisterSlotMapsFromSection` |
| **JIT**（运行时编译） | `jit_engine.cpp` / `jit_method.h` 运行时生成 | `GcRegisterSlotMap` |

所以**两种帧都讲同一种 GcSlotMap 语言** —— 这使"精确全根集扫描"同时覆盖 AOT 和 JIT，无需两套扫描器。这是 CRAG 相比 CoreCLR（单一 JIT）的最大优势也是对齐的基础。

### 3.3 根扫描器当前的三种模式（gc_root_scanner.h 已具备）

```cpp
GcScanPreciseFrame(...)     // 按 GcSlotMap 精确扫一帧
GcScanConservativeFrame(...)// 未知帧保守扫
GcScanFrameHybrid(...)      // 精确优先，未知槽回退保守  ← CoreCLR 同款
```

`GcScanAllThreadRoots` 只需改用 **`GcScanFrameHybrid` 逐帧扫**，替代现在的"整栈保守 + 堆基过滤"。这与 CoreCLR `GcScanStack` 的 hybrid（精确优先、fallback 保守）完全同构。

---

## 4. 实现步骤（代码级细节）

### 步骤 1：修正地址判据 —— 用真实堆成员测试替代 `>= g_heap_base`

`thread_state.cpp:633` 的 `>= g_heap_base` 改为正确判据：**指针是否指向 GC 管理的任何 region（nursery/old/LOH）**。两个候选：

- **(a) RegionManager 成员测试**：已有 `IsInNursery()` / old-gen / LOH 区间检查可组合成 `IsInGCHeap(ptr)`。**推荐** —— 精确、O(1)、跨平台（纯指针范围比较，无平台调用）。
- **(b) 交给 mark 阶段**：过滤直接移除，让 null pointer 之外都由 Mark 阶段据 `IsInOldGen`/`IsInNursery` 判定。保留预过滤仅作性能优化。

> 关键：无论 a/b，**过滤的目的从"什么是根"变成"什么值得当候选"**，正确性下放到 mark 阶段的可达性判定（与 CoreCLR 一致——根扫描报候选，标记阶段判可达）。

### 步骤 2：young GC Phase 0 改为扫全线程精确根

`gc_young_collector.cpp:418-447` 的 `current_thread` 单栈扫 → 改为**遍历全部已注册 ManagedThread**：

```cpp
// gc_young_collector.cpp Phase 0 改造
EnumerateThreads([&](ManagedThread* t) {
    if (t == current_thread) 当前逻辑（已有 GcLayout 精确扫 nursery）
    else 用 GcScanFrameHybrid 扫 t 的栈帧（GcSlotMap 精确优先）
});
```

- 每个挂起线程在 safepoint 下栈一致（`thread_state.cpp` 已保证）。
- 遍历用现有 `EnumerateThreads`（已含每个线程的 stack_limit/stack_base）。
- **寄存器根**：CoreCLR 精确扫描含 `GcScanRegisters`；JIT 侧已有 `SpillGcRefCachedRegs()` 在 safepoint 强制 spill 寄存器根到栈。AOT 侧经核查**不做寄存器缓存**（根天然在 C++ 栈局部），无 spill 需求（见步骤 5）。

### 步骤 3：扫描器逐帧化

`GcScanAllThreadRoots` 需要从"整栈一个 for 循环"重构为"遍历 ManagedFrameInfo → GcScanFrameHybrid"。借助既有 `GcScanInterpFramesFn` 跨模块回调 + AOT/JIT 帧链遍历。

- 栈帧链遍历：`FastFrame`（AOT/JIT 统一 frame 结构）—— 已有 `fast_dispatch`/`entry_direct` 支持。
- 未知帧（无 GcSlotMap）：回退保守（`GcScanConservativeFrame`），保留堆范围预过滤作为**保守兜底的性能优化**（此时过滤用 `IsInGCHeap` 而非 `g_heap_base`）。

### 步骤 4：确认对 old-gen / LOH 全 GC 无回归

`OldGen::Collect` 的 `ScanThreadRoots` 也用 `GcScanAllThreadRoots`。改造后 old-gen/LOH 的 full GC 根集合从「保守 + 堆基过滤」变为「精确 + 保守 hybrid」。

- 正确性：**变严**（漏假根减少），但必须回归验证 LOH/old-gen 压缩时 relocation 仍正确 fix-up（不能误移动。
- 性能：精确扫描比保守全栈扫快（不逐槽判堆）。

### 步骤 5：AOT 寄存器根 spill 核查结论（2026-08-10 实跑核实 = 非缺口）

**初判**曾把 AOT 寄存器根当作最高风险（"JIT 有 `SpillGcRefCachedRegs`，AOT 疑似无 → AOT 下 UAF"）。深度核查后**推翻初判**：

| 检查 | 结论 | 证据 |
|------|------|------|
| JIT 寄存器根 spill | 有 | `jit_engine.cpp:1232 SpillGcRefCachedRegs()` + `EmitSafepointPoll` 在 safepoint 前 spill 彩色寄存器 ObjectRef → `RecordGcPoint` |
| **AOT 是否做寄存器缓存** | **否** | Generator 无 `register_caching`/`gpr_color`/`colored`/`SpillGc` 任何引用。AOT 的 ObjectRef 是 **C++ 栈局部**（`const auto chaos_x...`），非手管寄存器 |
| AOT safepoint 是否真停 | 是 | AOT 发 `chaos_safepoint_poll()`（`StructuredIR.Emit.cs:701`）→ `SafepointPoll()` 真查 `suspend_seq` |

**关键推论**：寄存器缓存（colored vreg）是 **JIT 特有优化**；AOT 不做它，所以 ObjectRef 恒在 C++ 栈局部（GcSlotMap 的 `localsBase+i*PtrSize`、`evalBase+j*PtrSize` 覆盖）。即使 C++ 编译器把某局部放寄存器跨 `chaos_safepoint_poll()`，机器级也由 ABI 保护 callee-saved 寄存器，且该局部的**逻辑 GC 根 = 栈 home 槽**仍被扫描。

**结论**：AOT 寄存器根 **非缺口，无需改动**。步骤 5 从"最高风险、需补 spill"降级为"**架构说明 + AOT/JIT 差异记录**"。这是 AOT（C++ 栈局部，天然安全）vs JIT（寄存器缓存，需显式 spill）的**结构性差异，正是 CoreCLR（单一 JIT）无需面对、本项目必须认知的 AOT/JIT 对齐点**。

---

## 5. 跨平台适配细节

CoreCLR 根扫描依赖平台 unwind（`unwindinfo`/`.pdata`）/栈帧约定。我们**刻意不依赖**这些，全用 **GcSlotMap 契约 + FastFrame 链**，它们是**纯指针 + 偏移，无平台调用**：

| 平台关注点 | CoreCLR 做法 | 我们的做法（跨平台安全） |
|-----------|-------------|-------------------------|
| 栈帧遍历 | `.pdata`/unwind info | FastFrame 链（统一结构，AOT/JIT/解释器共享） |
| 帧根定位 | JIT GcInfo | GcSlotMap（codegen_bridge 契约） |
| 保守兜底 | JIT emit 的 conservative | GcScanConservativeFrame（纯指针对齐扫描） |
| 寄存器根 | JIT GcInfo regs | SpillGcRefRegs（JIT）+ AOT 需补齐（步骤 5） |

- **无 asm trampoline 依赖**：根扫描是纯 C++，`gc_suspend_trampoline_*.S`（x64/arm64 已有）只用在 safepoint hijack，不参与根扫描。
- **地址判据跨平台**：`IsInGCHeap` 是纯指针范围比较，无 Windows/Linux/iOS/Android 差异。
- **iOS/Android TLS/栈**：读 `thread->stack_limit/base` 是平台无关字段；不新增平台特定代码。

---

## 6. AOT/JIT 适配细节

| 场景 | 根记录 | 扫描 | 说明 |
|------|--------|------|------|
| **AOT（静态编译）** | `.gc_slot_maps` 段（`GcRegisterSlotMapsFromSection`） | GcScanFrameHybrid 精确 | 需补 AOT 寄存器 spill（步骤 5） |
| **JIT（运行时编译）** | `GcRegisterSlotMap`（`jit_engine.cpp`） | 同上精确 | 已有 `SpillGcRefRegs` |
| **解释器（热更）** | `GcScanInterpFramesFn` 回调 | GcScanFrameHybrid | 已注册，跨域热更场景 |

三者共用同一 `GcScanAllThreadRoots` 改造后的入口 → **一套扫描器覆盖 AOT/JIT/解释器**，这正是"代码级对齐 CoreCLR 又不破坏热更新域隔离"的实现保证。

---

## 7. 关键风险与护栏

| 风险 | 等级 | 护栏 |
|------|------|------|
| AOT 寄存器根未 spill → AOT 下 UAF | 🟢 **无** | **已核实非缺口**：AOT 不做寄存器缓存，根恒在 C++ 栈局部（步骤 5） |
| 全线程精确扫引入暂停开销 | 🟠 中 | 精确扫描比保守全栈快；用 hybrid 兜底未知帧；可加只扫"含 nursery 引用可能性"的过滤 |
| LOH/old-gen 压缩 relocation 误判 | 🟠 中 | 步骤 4 回归 LOH/old-gen 压缩测试 |
| 堆基过滤改为 IsInGCHeap 后漏候选 | 🟢 低 | mark 阶段做最终可达性判定，保守兜底保留 |

---

## 8. 验收标准（exit criteria）

- [ ] young GC 下，线程 B 栈持有线程 A nursery 对象 O → O 存活，无 UAF（针对性测试）
- [ ] AOT、JIT、解释器三种帧下的跨线程引用均正确存活
- [ ] `chaos_gc_region_test` / `chaos_gc_atomic_alloc_test` 等分配路径测试 0 回归
- [ ] LOH/old-gen full GC + 压缩 relocation 无回归
- [ ] 跨平台：Windows x64 验证通过（Linux/iOS/Android 为纯指针逻辑，无平台差异，待 CI 覆盖）
- [ ] 不破坏热更新域卸载（域卸载仍 O(region)）

---

## 9. 架构优先确认点（动码前必读）

1. `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/24-CRAG-GC架构参考.md` — young GC 根集设计 + GcScanAllThreadRoots 契约
2. `thread_state.cpp:590` GcScanAllThreadRoots 现有实现（保守 + 堆基过滤）
3. `gc_root_scanner.h` — GcScanPreciseFrame/GcScanConservativeFrame/GcScanFrameHybrid
4. `codegen_bridge.h` GcSlotMapV0 契约 + `NativeAotLoweringPlanner.GcSlotMap.cs`（AOT 侧）
5. JIT 侧 `SpillGcRefRegs` 与 AOT 侧 `RecordGcPoint` 寄存器根 spill 对照
6. 步骤 5 的 AOT 寄存器根 gap 是否存在的核对结果

---

## 10. 结论偏好

**主推荐**：按步骤 1-4 全量实施 —— 把 `GcScanAllThreadRoots` 提升为「精确优先 hybrid + IsInGCHeap 过滤」，young GC Phase 0 扫全线程精确根。步骤 5 已核实无需改动（AOT 非缺口）。这是与 CoreCLR 同构的完整解法，同时靠 GcSlotMap 统一契约保住 AOT/JIT/解释器三态与跨平台。

**备选降级**（若精确 hybrid 暂不可行）：至少先修 `g_heap_base` → `IsInGCHeap` 过滤，让 `GcScanAllThreadRoots`（保守）能在 young GC 用上 —— 虽非精确，但消除 UAF（保守扫描不会漏根，只有误报，误报由 mark 阶段筛掉）。作为正确性兜底。
