# Design — GC-K2：世代感知 region→generation 写屏障（对齐 CoreCLR 原本设计）

> 版本：design-k2-v1-01
> 创建：2026-08-10
> 性质：**Phase 5 区域化重构核心（高风险）**——触碰写屏障语义 + codegen 边界，需专门架构设计后分步落地
> 对标基准：`D:\OpenSource\dotnet\runtime\src\coreclr\gc\`（已深度只读还原）
> 硬约束：**尽量忠实对齐 CoreCLR 原本设计** + **跨平台适配** + **AOT/JIT 适配**

---

## 1. CoreCLR 原本设计（权威还原，文件:行号）

### 1a. region_info：1 字节打包代信息（gcpriv.h:1578-1596）

```cpp
enum region_info : uint8_t {
    RI_GEN_0=0x0, RI_GEN_1=0x1, RI_GEN_2=0x2,  RI_GEN_MASK=0x3,   // 低2bit=当前代
    RI_SIP=0x4,                                  // bit2: sweep-in-plan
    RI_DEMOTED=0x8,                               // bit3: 降级
    RI_PLAN_GEN_SHR=0x6,                          // 高2bit偏移
    RI_PLAN_GEN_{0,1,2}=0x00/0x40/0x80, RI_PLAN_GEN_MASK=0xC0,   // 高2bit=计划代
};
```

**1 字节 = 当前代(2bit) + 计划代(2bit) + 2 个 flag**。写屏障读 `[addr>>shift] & RI_GEN_MASK`。

### 1b. map_region_to_generation_skewed 布局（card_table.cpp:527-528, gcinternal.h:2290-2302）

```cpp
map_region_to_generation = mem + layout[region_to_generation_table_element];
map_region_to_generation_skewed =
    map_region_to_generation - size_region_to_generation_table_of(0, g_gc_lowest_address);
// JIT 侧：skewed_index = addr >> shift  （原生地址位移，无需减 low-watermark）
// shift = min_segment_size_shr = index_of_highest_set_bit(region_size);  4MB→22
```

**skewed 技巧**：把"地址 0 到 lowest"那段索引偏移量**预先减进指针**，JIT/写屏障只需一次 `addr>>shift` 查表。

### 1c. region 写屏障 fast-path（JitHelpers_FastWriteBarriers.asm Region64，默认 bit 版）

```
dst_gen = table[dst >> shift]          // shr rcx,16h; mov rcx,[rcx+rax]
if dst_gen == GEN0:    ret             // gen0 整区被扫 → 无需卡
if !(heap_low<=ref<heap_high): ret     // ref 越界(非托管/只读)
ref_gen = table[ref >> shift]
if ref_gen >= dst_gen: ret             // 非"老→幼" → 无需卡
// 老→幼 且 dst 非 gen0：
atomic set card[dst] + 附带 set bundle[dst]
```

**核心语义**：`ref.gen < dst.gen`（源比目标更年轻=老代写入年轻代，或 gen0 写入 old 的反向也成立因为 ref.gen=0<old）时设卡；`dst.gen==0` 一律跳过；同代/新→老跳过。→ **gen0→gen0 不设卡**（CoreCLR 的关键节省）。

### 1d. card bundle（2MB 粗卡，扫描侧跳过）（gcinternal.h:2015-2033, card_table.cpp:1619-1684）

- 层级：card(256B/位, shr 0x0B) ⊂ card_bundle(2MB, shr 0x0A)。
- **写屏障设卡时顺带着色 bundle**；**扫描端 `find_card_dword` 先用 BitScanForward 跳大段未脏 bundle**，再进对应 card 区。
- **bundle 不是判定一部分**，只加速扫描。

### 1e. mark 扫脏卡 region-gen 过滤（mark_phase.cpp:1388-1404, gcinternal.h:155-174）

```
if condemned_gen != max_gen && get_region_gen_num(o) > condemned_gen:  skip
```
**只有 gen ≤ condemned 的 region 的卡才被扫**。与 CRAG 的 handle 剪枝（GC-F1）语义一致。

### 1f. StompWriteBarrier 运行时切换（gc.cpp:557-640, writebarriermanager.cpp:383-427）

GC 运行时**把写屏障函数体整体 stomp 到共享 `JIT_WriteBarrier_Loc` 槽**（不是跳转表），切换 region/byte/server 形态。入参 `rcx=dst, rdx=ref`。

---

## 2. CRAG 现状对比（差距）

| 维度 | CRAG 现状 | CoreCLR | K2 差距 |
|------|----------|---------|---------|
| 卡表 | 两级 L1[seg]->cards[card]，无条件对 dst 设卡（仅 nursery fast-skip） | region→gen 映射 + ref/dst 相对比较 | 🔴 无条件设卡，无世代短路 |
| 代信息 | 无 region→gen 表；用 `IsNurseryPointer`(region 查询) | 1 字节 region_info 表 | 🔴 缺 region→gen 映射 |
| 跳过条件 | 仅"dst 在 nursery 范围" | `dst.gen==0` + `ref.gen>=dst.gen` | 🔴 缺世代相对判定 |
| card bundle | 无 | 2MB 粗卡扫描加速 | 🟠 缺 |
| 写屏障切换 | 无（固定 chaos_gc_dirty_card） | StompWriteBarrier 换函数体 | 🟠 缺 |
| codegen 触点 | `chaos_gc_dirty_card(obj)` 独立发射 | JIT 跳 `JIT_WriteBarrier_Loc` | 🔴 AOT codegen 需适配 |

---

## 3. 对齐 CoreCLR 的设计（跨平台 + AOT/JIT 适配）

### 3a. 新增 region→gen 映射表（忠实 CoreCLR 1b）

新增 `REGION_TO_GEN` 表 = 每 region **1 字节**（`region_info` 同构：当前代低 2bit + 计划代高 2bit + flag），`skewed` 基址预减 offset：

```cpp
// gc_region.h — 挂在 RegionManager
static constexpr CHAOS_IL2CPP_SIZE kRegionGenShift = 22;   // 4MB region (K1 SelectRegionSize 最大类)
extern uint8_t* g_region_to_gen;             // = base - (lowest>>shift)，skewed
inline uint8_t GetRegionGen(uintptr_t addr) {
    return g_region_to_gen[addr >> kRegionGenShift] & 0x3;  // RI_GEN_MASK
}
```

**忠实对齐 CoreCLR**：1 字节/region + 低 2 位代 / 高 2 位计划代 + skewed 查表。**跨平台**：纯指针位移 + 位运算，无平台调用。

### 3b. 世代感知写屏障（忠实 CoreCLR 1c，3 条件短路）

将 `chaos_gc_dirty_card(obj)`（现无条件设卡）升级为 **CoreCLR region 语义**。但要处理**一个本质差异**：CoreCLR 的 JIT barrier 同时拿 `dst` 和 `ref` 两个操作数；CRAG 的 `chaos_gc_dirty_card(obj)` 只传 **dst**。**这是对齐 + AOT/JIT 的核心张力**。

方案（忠实 CoreCLR + 兼容 CRAG 单参签名）：
- **runtime 双参写屏障** `chaos_gc_dirty_card_dst_ref(dst, ref)`：完整复刻 CoreCLR 3 条件短路（dst.gen==0 跳过 / ref 越界跳过 / ref.gen>=dst.gen 跳过）。**忠实对齐**。
- CRTAG codegen 从 `chaos_gc_dirty_card(dst)` 升级为传 **dst+ref**（在 stfld/stobj/stelem.ref 发射点拿得到 ref）→ 让 AOT/JIT 都能做世代短路。**这是 codegen 边界改动，需列 BOUNDARY 确认**。

### 3c. region 代号维护（用分代重绑，对齐 CoreCLR plan-gen）

- `set_region_gen(region, gen)`：GC 后按 region 所属代重写表（Young→gen0，survivor→gen1，old→gen2）。
- **K3 分代重绑**依赖此表；K2 先建表 + 维护机制。

### 3d. card bundle（2MB 粗卡，扫描加速）—— 忠实 CoreCLR 1d

追加 bundle 位图（2MB/位），`chaos_gc_dirty_card` 设卡时顺带着色；`ScanDirtyCards` 用 bundle 跳大段未脏区。

### 3e. mark/扫脏卡过滤（忠实 CoreCLR 1e）

`ScanDirtyCards` / young-GC Phase 1 用 `GetRegionGen` 过滤：只扫 `gen ≤ condemned` 的 region 的卡（对齐 GC-F1 handle 剪枝语义）。

### 3f. 写屏障形态切换（对齐 CoreCLR 1f，简化）

暂不做 StompWriteBarrier 全函数体切换（需 copy JIT 代码体，跨平台复杂）。用**函数指针** `g_write_barrier_fn` 指向不同形态（region/legacy），运行时切换指针（比 stomp 简单且跨平台安全）。**差异记录**：CoreCLR 用 stomp 是 JIT 单一模式优化；CRAG AOT/JIT 双模式只需指针切换。

---

## 4. AOT / JIT 适配

| 场景 | 适配 |
|------|------|
| **AOT（静态 codegen）** | codegen 在 stfld/stobj/stelem.ref 发射 `chaos_gc_dirty_card_dst_ref(dst, ref)`（升级 `chaos_gc_dirty_card`）→ 世代短路。AOT 帧 C++ 环境，双参屏障天然可调。 |
| **JIT** | JIT 的 objref 写入同样调 runtime 双参屏障（或生成 inline 短路）。JIT 可进一步 inline 3 条件短路（读 g_region_to_gen 表）。 |
| **保持兼容** | 保留 `chaos_gc_dirty_card(dst)` 单参兼容入口（ref 不可用时报 dst，退化为"非 gen0 即设卡"保守路径），避免破坏既有调用点/热更新。 |

## 5. 跨平台适配

| 平台关注点 | CoreCLR | CRAG K2 |
|-----------|---------|---------|
| 卡表/region 表 | amd64 asm barrier + 运行时 stomp | **纯 C++**：`addr>>shift` + 位运算，无 asm/平台调用，Win/Linux/iOS/Android 一致 |
| 原子设卡 | `lock or`（bit 版） | `std::atomic<uint8>::fetch_or`（跨平台） |
| 写屏障切换 | stomp JIT_WriteBarrier_Loc 代码体 | 函数指针切换（无 stomp 代码拷贝，跨平台安全） |
| 大页 | — | 保留 K1 大页 fallback |

**刻意不做 stomp**（对齐差异）：CoreCLR 的 stomp 依赖"所有 JIT 生成代码跳同一 JIT_WriteBarrier_Loc"，CRAG AOT/JIT 双模式不适合；用函数指针等价达成"运行时切换屏障形态"。

---

## 6. 分步落地（风险评估）

| 步骤 | 内容 | 风险 | 依赖 |
|------|------|------|------|
| K2a | 建 region→gen 表 + `GetRegionGen` + 代号维护（GC 后重写） | 低 | K1 |
| K2b | `chaos_gc_dirty_card_dst_ref` 双参屏障（3 条件短路） | 🟠中 | K2a |
| K2c | codegen 升级 stfld/stobj/stelem.ref 发射双参屏障 | 🔴高（codegen 边界） | K2b |
| K2d | card bundle 位图 + ScanDirtyCards 过滤 | 中 | K2a |
| K2e | mark 扫脏卡 gen 过滤 | 中 | K2a |

**每步可独立验证**（编译 + 对应 GC 单测 + 内存正确性），每步提交。K2c 触碰 codegen 需列 BOUNDARY_OVERRIDE。

## 7. 验收（exit criteria，最终）

- [ ] region→gen 表 + `GetRegionGen` + 代号维护（K2a）
- [ ] `chaos_gc_dirty_card_dst_ref` 双参屏障实现 3 条件短路（K2b）
- [ ] AOT/JIT codegen 发射双参屏障，`gen0→gen0` 不设卡（K2c）
- [ ] card bundle + ScanDirtyCards gen 过滤（K2d/e）
- [ ] 跨平台：纯 C++ 无平台调用；Function-pointer 切换而非 stomp
- [ ] 全部 GC 单测 0 回归 + 热更新域卸载不破坏

---

## 8. 边界声明

- 本设计是 **K2 架构文档**（忠实 CoreCLR 语义 + 跨平台/AOT-JIT 适配方案），非本次全部实现。
- **下一步建议**：从 **K2a（region→gen 表 + 代号维护，低风险）** 起步，逐步骤实现并验证。
- 忠实差异记录：stomp→函数指针切换（跨平台）、双参屏障适配 codegen、card bundle 扫描加速可选。
