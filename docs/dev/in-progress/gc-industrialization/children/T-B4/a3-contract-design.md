# T-B4 — codegen/JIT/interpreter 写屏障契约

> 标题：写屏障跨模块调用契约（codegen/JIT/interpreter 统一）
> 作者：Chaos IL2CPP 基础设施
> 日期：2026-08-31
> 上位文档：`roadmap-v1-01.md` T-B4 / `a3-safepoint-design.md` §6 / `a3-leaf-barrier-design.md`

---

## 0. 问题背景

**`CodegenStSFld 三路不一致` 是已知并发正确性缺陷**（工业化报告 Phase 2 项）。存储引用到 GC 独占字段/数组/静态字段的道路有三条，各自调用的写屏障形式不一致：

| 路径 | 调用 | 一致性 |
|:---|:---|:---:|
| **Codegen Reg/Handle**（`LinearEmission.cs:84/89`） | `BgcSatbPreWriteBarrier` + `chaos_gc_dirty_card_dst_ref(chaos_address, chaos_value)` | ✅ 完整（SATB + 世代感知 dirty） |
| **Codegen Static**（`LinearEmission.cs:1179/1180`） | `BgcSatbPreWriteBarrier` + `BgcRecordRootChange` | ⚠️ 用 `BgcRecordRootChange`（BGC root scan），非 dirty_card |
| **JIT**（`jit_helpers.cpp:506`） | `JitSatbPreWriteBarrier(slot)` + 直接 store（无 dirty_card） | ❌ 只 SATB，无双屏障 |
| **Interpreter**（`fast_dispatch_object.inc` 等） | `BgcSatbPreWriteBarrier` + （部分缺 dirty） | ⚠️ 参差 |

**风险（T-B1 后认知）**：safepoint 的 store+barrier 一致性由 `mode switch` 保证，但**前提是 barrier 三路都正确触发**。若某一路只做 SATB 不做 card dirty（JIT stsfld），Young GC 扫描 card 时会漏 old-gen 引用 → 跨代 UAF。这正是「三路不一致」危在 A2b 窗口之外的真实危害。

---

## 1. 契约目标

定义**唯一权威的 barrier 调用序列**，所有三路（codegen/JIT/interpreter）遵循同一序列，消除不一致。

## 2. 核心契约（SBD 序列）

**对任意「向 GC 托管字段存储引用 / 任意 store+barrier」的操作，按此固定顺序：**

```
S — SATB 预写屏障（BGC concurrent mark 的读屏障）
    BgcSatbPreWriteBarrier(&slot)
B — 世代感知写屏障（Young GC 的 card 标记）
    chaos_gc_dirty_card_leaf(&slot, ref)   // 见 T-B3 LEAF 化，mode switch 绑定
D — Death/静态根辅助（仅命中 BGC root scan 的静态字段）
    BgcRecordRootChange(&slot, ref)        // 仅在 stsfld 命中的一条补充
```

**三路约定**：
| 场景 | S (SATB) | B (dirty card) | D (root change) |
|:---|:---:|:---:|:---:|
| 实例字段 stfld（Reg/Handle） | ✅ | ✅ | — |
| 数组 stelem.ref | ✅ | ✅ | — |
| **静态字段 stsfld** | ✅ | ✅ | ✅（BGC root scan 需） |
| 变量/装箱 stloc | ✅ | ✅（若指向 old-gen） | — |

**关键裁决**：`jit_helpers.cpp:506` 的静态 store **缺 B（dirty card）**，这是必须补的。Codegen Static 用 `BgcRecordRootChange` 替代 dirty card 是**错误**——两者职责不同（root change 是 BGC 并发根扫描；dirty card 是 Young GC card 标记），**不能互替**。

## 3. 统一接口

```
// — 唯一权威 barrier 序列（codegen 内联，leaf 化） —
inline void chaos_gc_store_ref(void** slot, void* ref, bool is_static_root) noexcept {
    chaos_gc_barrier_satb(slot);              // S: SATB 预写
    chaos_gc_dirty_card_leaf(slot, ref);      // B: 世代感知 card（mode-switch 绑定）
    if (is_static_root) chaos_gc_barrier_root_change(slot, ref);  // D: 静态根辅助
    *slot = ref;                              // 实际 store（在 barrier 之后）
}
```

> ⚠️ 顺序注意：SATB 预写屏障必须在 store **之前**（Yuasa-deletion 语义）；dirty card 在 store 之后或之前均可（CoreCLR 在其后），但 CRAG 统一放 pre-write 以匹配 `mode switch` 保证——线程在 cooperative 触发，store 完成时已在安全点屏障内。

## 4. 三路调用点修正清单

| 文件 | 当前 | 修正 |
|:---|:---|:---|
| `chaos_gen/LinearEmission.cs:84/89` | `SatbPreWriteBarrier` + `dirty_card_dst_ref` | 改为 `chaos_gc_store_ref`（或保持 sequence，已验证） |
| `LinearEmission.cs:1179/1180` (static) | `SatbPreWriteBarrier` + `RecordRootChange` | **补 dirty_card**：完整 S+B+D |
| `jit_helpers.cpp:506` (stsfld) | 仅 `JitSatbPreWriteBarrier` | **补 dirty_card**：S+B |
| `jit_helpers.cpp` 其他 | 已含 dirty_card | 统一为 `chaos_gc_store_ref` |
| `interpreter/*.inc` | 仅 `SatbPreWriteBarrier` | 补 dirty_card（if old-gen） |
| `gc_bgc_inline.h` / `gc_card_table.h` | 现有函数 | 新增 `chaos_gc_store_ref` 权威序列 |

## 5. 与 T-B3 LEAF barrier 的关系

- `chaos_gc_dirty_card_leaf` 是 T-B3 的 LEAF 化版本（mode switch 绑定）
- `chaos_gc_store_ref` 是 T-B3 的 full sequence 封装（S+B+[D]）
- T-B3 的「mode switch 保证」在此契约中落地：`chaos_gc_store_ref` 假设调用方已在 cooperative mode（由 A3 保证）

## 6. 验证

1. **静态字段测试**：`jit_helpers.cpp` 静态 store 补 barrier 后，stsfld → Young GC 不漏 old-gen 引用（`chaos_gc_region_barrier_stress_test` 扩容）
2. **三路一致性**：codegen/JIT/interpreter 对同一操作产生相同 barrier 序列
3. **ASAN/TSAN**（T-A2/T-A3 已就绪）：补 barrier 后无 UAF/数据竞争

## 7. 已拍板决策

- [x] 三路统一为 `S+B+[D]` 序列
- [x] `jit_helpers.cpp:506` 静态 store 补 dirty_card（当前缺失）
- [x] Codegen static 的 `BgcRecordRootChange` **不能替代** dirty_card（职责不同）
- [x] 唯一权威入口 `chaos_gc_store_ref(slot, ref, is_static)`
- [x] mode switch 保证在此契约中落地

## 8. 归属

本契约设计为 P1/P2 实现输入。**实际修正 `jit_helpers.cpp` + codegen emitter + interpreter 是 P2 的「CodegenStSFld 统一」清单项**，在 ASAN/TSAN 护网下执行。