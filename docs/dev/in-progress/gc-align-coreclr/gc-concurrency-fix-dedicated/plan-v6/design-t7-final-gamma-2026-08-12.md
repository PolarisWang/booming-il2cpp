# T7 最终方案 — 对齐 CoreCLR 叶子 write-barrier 的 γ' 根治 + 实现细节

> 日期：2026-08-12
> 依据：实读 CoreCLR `JitHelpers_FastWriteBarriers.asm`（JIT_WriteBarrier_Byte_Region64 / _SVR64）+ Chaos 现状 + M1 诊断
> 定位：**从根本上修 store-then-barrier 竞态**，非 detector；**零假阳性**压力验证

---

## 0. CoreCLR 方案的精髓（已实读验证）

CoreCLR 的写屏障是 **LEAF assembly 函数**（`LEAF_ENTRY ... LEAF_END_MARKED`），内部只有 `mov/cmp/shr/ret`，**无任何 `call`、无 safepoint poll**：
```
LEAF JIT_WriteBarrier_Byte_Region64:
    mov [rcx], rdx        ; ① STORE
    ...  gen/region 判定（纯 mov/cmp/shr）
    mov byte [r8+rax], 0xFF   ; ② CARD（单条原子 store，TSO）
    ret
```
**安全不变量**：store(①) 与 card(②) 在**同一个无挂起点的叶子**内。CoreCLR 协作式挂起只在 safe point（JIT 插的 poll 点）生效 → 线程要么**完整跑完 store+card**（poll 在下一条），要么**没开始**。**绝不可能被挂在 ① 与 ② 之间。**

> **对齐结论：根治 = 让「store + card」成为 GC 不可插队的原子段（无可挂起点）。** 不是改顺序（α 无效）、不是必须硬 STW（因为 CoreCLR 也没在任意指令处掐线程，是靠叶子内无 poll）。

---

## 1. Chaos 问题本质（与 CoreCLR 的差距，已实读）

Chaos 发射：
```
codegen:  [store] slot = ref                        ; 内联指令
        [call]  chaos_gc_dirty_card_dst_ref(dst,ref)  ; 真 C++ 函数 = call 边界
```
- `chaos_gc_dirty_card_dst_ref` 是 `extern "C"`（gc_card_table.h:185）→ **store 与 card 之间有 call 边界**。
- Chaos 协作式 safepoint（thread_state.cpp:274）对 fordid-suspend 线程 **ack-and-continue（不 wait）** → GC 可在 mutator 处于 write-barrier 中间态时进 Phase-1。
- **⇒ store 与 card 之间可被挂起 = 竞态窗口。**

---

## 2. γ' 根治设计（对齐 CoreCLR 不变量，三层落地）

### 核心目标
> **Phase-1 开始扫描的时刻，不变量「已 store 的槽 ⇒ 其 card 已 dirty」恒成立。**
> 等价于 CoreCLR：**任何线程都不处于「正在执行 store+card、但尚未完成」的中间态。**

### 实现：让 write-barrier 成为「GC 必须等待其完成的临界区」

对每条跨代引用写，把 store+card 包进**临界区**，且**修正 safepoint 让它等临界区退出再进 Phase-1**（而非 ack-and-continue 直进）：

```cpp
// 新运行时原语（chaos/gc_card_table.h 或 thread_state.h）
// 语义：进入后本线程的 store+card 是不可打断的原子段；GC 挂起此线程必须在它退出后。
class ScopedBarrierAtomic {
public:
    ScopedBarrierAtomic() noexcept { ++tls_forbid_suspend_depth; }
    ~ScopedBarrierAtomic() noexcept { --tls_forbid_suspend_depth; /* 若==0 且 safepoint 激活→本线程将正确地在其后 poll 等待 */ }
};
```
关键不是这个 RAII（它已有），而是**修正 thread_state.cpp:274**：`tls_forbid_suspend_depth>0` 时不再简单 ack 直进，而是**标记这个线程"未到安全点"并让协调器等待**（见第 3 节）。

---

## 3. 实现细节（按层）

### 3.1 运行时：修正 safepoint 的 ack-and-continue（核心）

`thread_state.cpp:274` 当前：`depth>0 → return`（ack 但 GC 直进）。

**改为「深度归零才 ack」**：
- 协调器 `RequestGlobalSafepoint`（thread_state.cpp:411）collect 阶段：**不只等 suspend_ack，还要等所有线程 `tls_forbid_suspend_depth` 归 0**（即所有在途 write-barrier 已退出）。
- 实现：safepoint 请求时，若某线程 `depth>0`，该线程在 `ScopedBarrierAtomic` 析构（depth 回到 0）后的下一次 poll 才真正 ack；协调器在超时前等待。
- **防死锁**：write-barrier 内不得再请求 safepoint/分配（纯 store+card 无 call 以外动作）；协调器等待用 bounded + `PalPreemptRequest` 兜底（已有机制）。

> **为何这样根治**：Phase-1 开始时协调器已保证所有线程 depth==0 = 没有任何线程正处于"已 store 未 card"中间态。窗口消失。

### 3.2 codegen（managed .cs，6 站点）

把 store+card 包进 `ScopedBarrierAtomic`（或复用现有 `ForbidSuspendScope`，关键在于 3.1 的 wait 语义）。站点（已实读）：
| 文件 | 行 | 形态 |
|------|----|----|
| LinearEmission.cs | 84-89 | StoreIndirect |
| LinearEmission.cs | 267-273 | ArrayStore |
| LinearEmission.Advanced.cs | 140-144 | StoreObjectValue（单参 card） |
| ExceptionEmission.EmitInstruction.cs | 1218 | 值类型 owner |
| ExceptionEmission.EmitInstruction.cs | 1234 | 对象字段 |

改法：`{ ScopedBarrierAtomic barrier; store; chaos_gc_dirty_card_dst_ref(...); }`（把 store 从 codegen 内联移到 barrier 作用域内）。需 regen `.generated.cpp` + 更新 snapshot。

### 3.3 JIT（jit_helpers.cpp CodegenSt*）

JIT 已把 GC-ref 写统一路由到共享 helper（jit_helpers.cpp `CodegenStFld`/`StFldNoBarrier`/`StElem`/`StObj`/`StSFld`，6-8 个）。**只需在这些 helper 函数体开头包 `ScopedBarrierAtomic barrier;`** —— 一处改动覆盖 JIT 全部发射，无需改 emit。

### 3.4 解释器（Handle_StFld/StElem）

解释器 `fast_dispatch_*.inc`/`interpreter_vm.cpp` 内联站点（~6-8 处）：同样包 `ScopedBarrierAtomic`。

> **为什么 JIT/解释器改动小**：它们已走共享 helper / 集中站点；codegen 是唯一需要逐站点 + regen 的。

---

## 4. 零假阳性的压力验证（对齐 CoreCLR 语义）

γ' 落地后，store+card 原子 → 屏障不漏 → **存活级检测零误报**。验证：
1. **`gc_region_multigen_stress_test`**：四代交叉 + static-root 持根 + 每轮 GC 后存活级断言（IsMarked/IsNurseryPointer∧region_gen/IsInGen1 + gen1_bump）。
2. **对照组 self-check**：一跳过 barrier 的场景须 FAIL（证明 detector 有效），独立 heap 隔离。
3. **20x 连跑**：γ' 后 dangling 应归零且稳定。
4. **A2b 隔离复测**：若仍偶发（A2b 独立于屏障），双根对照归因，不当作漏卡。

---

## 5. 验证与回滚

| 步骤 | 命令/判据 |
|------|----------|
| 编译 | `cmake --build ... --target chaos_gc_region_barrier_stress_test` |
| 单元 | `ctest -R gc_ -L unit` 无回归（尤其 thread_state/young_collector） |
| 压力 | `gc_region_barrier_stress_test` 20x 全 0 |
| codegen | snapshot regen + `test_driver.py --layer unit` 无 new FAIL |
| 风险 | 3.1 的 safepoint wait 改动最险（死锁）→ 先跑 stress 防挂，bounded wait + APC 兜底 |

---

## 6. 分阶段落地（降低风险）

- **阶段 1（最险，先做）**：修正 3.1 safepoint wait——让 Phase-1 等所有 barrier 临界区退出。单独提交，跑全部 GC + stress 验无死锁/回归。
- **阶段 2**：3.2/3.3/3.4 三套发射包 `ScopedBarrierAtomic`。阶段 1 已在运行时保证"无在途 barrier"，此阶段只是让更多路径符合（即使某处漏包，阶段 1 的 wait 也兜住——因为漏包 = 没进临界区 = 那线程可能被挂起于中间态，需阶段 1 的 wait 处理，故**阶段 1 必须已处理不在临界区的线程**）。
- **阶段 3**：压力测试 + A2b 复测。

> **诚实边界**：阶段 1 的「wait 所有 depth==0」若无法可靠枚举"在途 barrier"，需退化为**硬挂起兜底**（协调器对不归零线程用真挂起/APC 冻结，此时 store+card 中间态被硬冻结——需配合 γ'，与 β' 会合）。这点会在阶段 1 的针尖实验确认。
