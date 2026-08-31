# T-B3 — LEAF 写屏障设计（mode switch 绑定版）

> 标题：LEAF 写屏障设计（修正方向：编译期原子片段契约 + mode switch 绑定）
> 作者：Chaos IL2CPP 基础设施
> 日期：2026-08-31
> 上位文档：`roadmap-v1-01.md` T-B3 / `a3-safepoint-design.md` §5-6 / `a3-allocator-design.md`

---

## 0. 核心认知修正（来自 T-B1 研究）

**「LEAF barrier = 总线级原子化 store+barrier」是误解。**

T-B1 研究（§5）澄清：**CoreCLR 的 LEAF/GCsafe 是注解系统，约束函数体在 safepoint 竞态一致性上的义务**，不是把 store+barrier 做成原子指令。真正的 atomicity 由 **mode switch 保证**：

> "WriteBarrier 触发只在 cooperative mode；一旦线程被判定 preemptive，它不再执行 barrier。store+barrier 的一致性由模式切换保证，而非由 barrier 本身 LEAF/原子保证。"

因此，CRAG 的 LEAF barrier 设计目标**不是**「实现一个不可中断的 store+barrier 指令对」，而是：

1. **LEAF 化保证「无跨 suspend 点」**——barrier 函数体极短、不含可被挂起的调用（不触发 GC、不分配、不等待事件）
2. **绑定 mode switch**——barrier 只在 cooperative mode 触发；一旦线程 preemptive 就不再执行
3. **与全局 trap 标志配合**——GC 启动时 cooperative 线程被驱赶到安全点，preemptive 线程不做 barrier

---

## 1. 现状

### 1.1 当前 barrier 形态

- `DirtyCard(const void* obj)` — header 内联 C++ 函数，已完成 Nursery 快速跳过 + L1/L2 双层卡表 + 原子 Or + card bundle
- `chaos_gc_dirty_card(obj)` — codegen 调用的 extern "C" 入口
- `chaos_gc_dirty_card_dst_ref(dst, ref)` — 世代感知版本（dst+ref）
- `CardBundleSet` — 2MB bundle 位

### 1.2 当前问题

| 问题 | 现状 |
|:---|:---|
| **非 LEAF** | `DirtyCard` 是 C++ 内联，可能被编译器插入调用（`CardBundleSet` 是独立函数？） |
| **无 mode switch 绑定** | barrier 不检查线程是否 cooperative；preemptive 线程也可能执行 |
| **store+barrier 一致性依赖** | 依赖 C++ 语义而非显式 mode 保证 |
| **跨平台** | `_InterlockedOr` / `__atomic_fetch_or` 已跨平台，但 assembly LEAF 尚无 |

---

## 2. 设计目标

1. **LEAF 化**barrier（编译期可证明的原子片段，无跨 suspend 点）
2. **mode switch 绑定**（barrier 仅在 cooperative 触发）
3. **消除 store+barrier 中间窗口**（cooperative 线程被 trap 保证在安全点）
4. **跨平台**（x64 + ARM64）
5. **保持当前性能**（~6 指令预算）

---

## 3. 架构设计

### 3.1 两层设计：Inline LEAF + Out-of-line 兜底

```
┌─────────────────────────────────────────────────────┐
│ Inline LEAF fast path（codegen 热路径）             │
│                                                     │
│   - 汇编/内联 intrinsic，无函数调用                  │
│   - LEAF 保证：不触发 GC、不分配、不等待             │
│   - mode switch 前置检查（当前线程 cooperative）     │
│   - 只有 < 12 native instruction                    │
└──────────────────────┬──────────────────────────────┘
                       │ miss
┌──────────────────────┴──────────────────────────────┐
│ Out-of-line 兜底（慢路径，含完整检查）              │
│                                                     │
│   - 调用 chaos_gc_dirty_card_dst_ref                │
│   - 完整 generation-aware 逻辑                      │
└──────────────────────────────────────────────────────┘
```

### 3.2 LEAF 保证（编译期契约）

对 barrier 函数体，强制以下不变量：

| 不变量 | 实现 |
|:---|:---|
| 不调用 BGC/GC 触发 | `GC_NOTRIGGER` 语义（CoreCLR `forbid_suspend` 对齐） |
| 不分配 | barrier 内部不调用 `G_OldGen().Allocate` 等 |
| 不等待事件 | 不 `PalEventWait` |
| 不含跨 suspend 点 | 无函数调用（或仅叶调用） |
| 无异常 | `noexcept` + 不抛 |

**实现方式**：把 `DirtyCard` 重构为纯 leaf 内联（`__forceinline` + 无 `CardBundleSet` 分离调用），或在关键路径用汇编。

### 3.3 mode switch 绑定

```
barrier 入口:
  if !tls_current_mode_is_cooperative: return   // preemptive: 不做 barrier
  // 进入 cooperative 时已完成 rendezvous 检测（A3）
  // 因此此处若有 trap 进行中，线程已在安全点，barrier 安全
  执行 DirtyCard(...)
```

关键：**barrier 不在 preemptive mode 触发**。一旦线程切换到 cooperative 进入 barrier，由于 A3 的 trap 标志保证「cooperative 线程已在安全点」，barrier 执行的 store 不会落入「store 已做、barrier 未做」的窗口。

### 3.4 store+barrier 窗口消除（终极）

A3 的全局 trap 标志保证了**核心不变量**：

> **当 GC 启动时，尚无线程处于「已发布 store 但未完成 barrier」的状态。**

证明：
1. GC 置 trap 标志
2. cooperative 线程被驱赶到安全点（rendezvous），此时它已完成上一个 store+barrier，未开始新的
3. 新进入 cooperative 的线程在 `RareDisablePreemptiveGC` 撞到 trap → 撤回 preemptive 排队
4. 因此 GC 扫描时，所有线程要么 preemptive（不做 barrier），要么在 rendezvous（已完成 barrier）
5. **不存在「store 落子、barrier 未发」的中间线程**

这比「原子化 store+barrier」更简单，也更正确——它绕开了 OS 寄存器不可靠问题。

---

## 4. 汇编 LEAF（x64 + ARM64）

### 4.1 为什么需要汇编

`DirtyCard` 当前是 C++ 内联。LEAF 化需要**数学上无函数调用**的证明。编译器 `__forceinline` 通常能做到，但在 O2 下可能引入 `CardBundleSet` 分离调用。

**务实策略**：先用 `__forceinline` + 代码审查保证 LEAF（零汇编，P1 时验证），若编译器破坏 LEAF 再降级为手写汇编。**不做强制汇编**——这是为了保持代码可维护性。

### 4.2 若需汇编（参考设计）

```
x64 DirtyCard LEAF:
  ; RDI = obj
  cmp RDI, [g_heap_base+rip]      ; below heap?
  jb  .ret
  cmp RDI, [g_nursery_begin+rip]  ; nursery skip
  jae .ret_after_nursery_begin     ; (careful: check both bounds)
  ; L1/L2 card table access
  shr RDI, kCardShift
  ...
  lock or [card_word], bitmask     ; atomic
  ret
```

### 4.3 跨平台

- x64: SysV AMD64 / MSVC x64 ABI
- ARM64: AAPCS64
- 卡表访问逻辑相同，仅寄存器约定不同

---

## 5. 与既有 barrier 的关系

| 现有 | 设计后 | 变更 |
|:---|:---|:---|
| `DirtyCard` 内联 | `DirtyCardLeaf`（LEAF 保证） | 重构 |
| `CardBundleSet` 分离 | 并入 LEAF 或可选 | 内联或删除分离 |
| `chaos_gc_dirty_card` | 保留（兜底入口） | 无 |
| `chaos_gc_dirty_card_dst_ref` | 保留（世代感知兜底） | 无 |
| codegen 调用 | 改为 LEAF 内联 + 兜底 | codegen 更新 |

---

## 6. 验证方式

1. **单元测试**：barrier 单元测试（card-table 已有）验证 LEAF 化后行为不变
2. **ASAN**：LEAF 化后无回归（P1 时 ASAN 护网）
3. **TSAN**：mode switch 绑定后，并发测试无数据竞争（T-A3 已建 Linux TSAN CI）
4. **性能**：LEAF 化后 barrier 热路径开销 ≤ 现状（~6 指令）

---

## 7. 已拍板决策

- [x] LEAF = 编译期原子片段契约 + mode switch 绑定，**非**总线原子化 store+barrier
- [x] store+barrier 一致性由 mode switch + trap 标志保证，非原子指令对
- [x] `DirtyCard` 重构为 LEAF 内联（`__forceinline` + 代码审查），不强制汇编
- [x] barrier 仅在 cooperative mode 触发
- [x] 卡表逻辑与世代感知逻辑不变（仅变换形）

---

## 8. Watch Items

- [ ] `__forceinline` 在 Release O2 下是否保持 LEAF（需 P1 验证，若破坏则降级汇编）
- [ ] mode switch 的 `tls_current_mode_is_cooperative` 读取成本（每次 barrier 一个 TLS 读，能否合并）
- [ ] x64/ARM64 LEAF 汇编的维护成本（若需）

---

## 9. 结论

LEAF barrier **不是**「原子化 store+barrier」的魔法，而是三件事的组合：
1. **LEAF 化**——barrier 函数体可证明无跨 suspend 点
2. **mode switch 绑定**——barrier 仅 cooperative 触发
3. **trap 标志保证**——cooperative 线程已完成 barrier

这三者共同**结构性消除**了 A2b 的 store+barrier 窗口，且绕开 CoreCLR 证伪的「挂起态扫描」路径。跨平台（x64/ARM64）可行性确立。

---

## 10. 交接约束：store 与 barrier 的 mode 绑定（接续者必读）

「mode switch 保证一致性」**有隐含前提**——store 与 barrier 必须执行于同一 mode 上下文，且 mode 切换点与 barrier 调用之间的可见性有明确定义。缺失此处会引入竞态：

1. **同一 mode 上下文**：存储目标引用的 `store` 与随之调用的 `barrier` 必须都由**同一条 codegen 指令序列**在同一 cooperative 段内发出（即插入 barrier 时，该线程已在 cooperative 模式）。禁止 store 与 barrier 被 `EnablePreemptiveGC()/DisablePreemptiveGC()` 切换点隔开——若线程在 store 与 barrier 之间切到 preemptive，mode switch 前提即被破坏，A2b 窗口复活。
2. **mode 读取顺序**：barrier 开头读取 `tls_current_mode_is_cooperative`；仅在 cooperative 时执行 card/世代处理，否则跳过（preemptive 线程由 rendezvous 兜底扫描）。该判断必须发生在**任何 store 副作用之前**（acquire 顺序），确保「cooperative 读到」⇒「该 store 会被 barrier 覆盖」。
3. **可见性语义**：cooperative 线程的每次 store 后**必须**跟一次 barrier（sink 处，非 source）。codegen 契约落地时（见 T-B4 契约），对每个写引用 `stfld/stind.ref/stelem.ref` 在**同一基本块末尾**（仍 cooperative）插入 barrier 调用，保证 mode 未切换。
4. **交接检查**：P1 实现 codegen 插入逻辑时，验证每个 barrier 调用的生成位置与其对应 store 的最高 mode 值一致；用 `gc_region_barrier_stress_test` + TSAN（T-A3）覆盖「store 后立即切换 mode」的并发用例，捕获边界违例。
