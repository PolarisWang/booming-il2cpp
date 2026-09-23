# pps3-C 设计 — GC Slot Map 段拆分

> task_id: `page-payload-uniform-tu` / 子任务 `pps3-C`
> 创建：2026-09-23
> 上游：`design-pps3-B2.md`、roadmap-v1-01
> 目标：page-0071 从 11.53 MB 压到 ≤ 8 MB

## 一、为什么 B1/B2 的方法**不适用**

B1/B2 的做法是「把一个段拆成多个段，各占一个 TU」。pps3-C **不能这样做**，
因为 runtime 的消费方式是**单次线性扫描一段连续内存**：

```cpp
// bootstrap.cpp:322
GcRegisterSlotMapsFromSection(
    code_registration->slot_map_section_begin,   // = &kChaosGcSlotMapsSection
    code_registration->slot_map_section_end);
```

```cpp
// gc_root_scanner.cpp:62 —— 按 entry_total_size 步进，直到 end_ptr
while (ptr + sizeof(uint32_t) <= end_ptr) {
    memcpy(&entry_total, ptr, 4);
    ...
    ptr += entry_total;
}
```

🔴 **拆成多个 C++ 对象 = 改 ABI + 改 runtime** → 超出本任务范围（roadmap 已标注 C3 才动 ABI）。

## 二、实测构成（关键发现）

page-0071 的 slot map 段 **8.69 MB** 里：

| 类别 | 大小 | 说明 |
|---|---|---|
| **注释** | **6.03 MB** | 每 entry 一条 `/* ── Entry N: <长符号名> ── */` |
| 声明 | 2.22 MB | `CHAOS_IL2CPP_UINT32 entry_total_size;` 等，**每个 entry 重复 5 行** |
| 数据 | 0.45 MB | `147u,` / `reinterpret_cast<...>` |

**注释占 69%**，声明占 26%，真正的数据只占 5%。

原因：每个 entry 都重复写完整的 5 行 struct 成员声明 + 一条含完整 native 符号名的注释。
10,251 个 entry × (~590 字节注释 + ~217 字节声明) ≈ 8.2 MB。

## 三、方案：不拆段，**压缩段的文本**

保持「单个连续对象」这一 ABI 契约不变，只减少**生成文本**：

### C1 — 注释瘦身
`/* ── Entry N: <nativeSymbol> ── */`（~590 B/条）→ 去掉符号名或整条删除。
**节省 ~6 MB。**

- 保留 `Entry N` 序号（便于与 runtime 日志对照）
- 符号名信息**不丢失**：它在 code_address 指向的函数名里，且 debug 时可用序号反查

### C2 — 声明去重
5 行成员声明 → 提取为一个具名的 entry 结构体，数组式承载：

```cpp
struct ChaosGcSlotMapEntryV1 {           // 只定义一次
    CHAOS_IL2CPP_UINT32 entry_total_size;
    const void*         code_address;
    CHAOS_IL2CPP_UINT32 frame_size;
    CHAOS_IL2CPP_UINT32 num_gc_slots;
    CHAOS_IL2CPP_UINT32 slots[1];        // 柔性尾巴；实际按 num_gc_slots 走
};
```
**节省 ~2.2 MB。**

### C3 — 数值写法压缩
`0u, 4u, 8u` → 省略 `u` 后缀（C++ 中 int 字面量在 uint32_t 初始化里合法）。

## 四、🔴 风险评估

| # | 风险 | 对策 |
|---|---|---|
| **C-R1** | 改结构体 → **改 ABI**，runtime 扫描按固定布局 | 🔴 **不采用 C2 的结构体改写**（见下 §五）|
| **C-R2** | 删注释导致符号名丢失、排障困难 | 序号保留；且 `code_address` 指向的函数名可反查 |
| **C-R3** | 文本变了但**语义变了**（漏 entry / 顺序变） | 守卫断言 entry 数与 `entry_total_size` 之和不变 |
| **C-R4** | 仍超 8 MB | 见 §五 预算 |

## 五、范围修正（实测预算）

| 方案 | 节省 | page-0071 预计 |
|---|---|---|
| 只做 C1（删注释） | 6.03 MB | 11.53 → **5.50 MB** ✅ |
| C1 + C2 | +2.2 MB | 3.3 MB（但 C2 触及布局，**不做**）|

**结论：只做 C1 就足够达标**（5.50 MB ≪ 8 MB），且**完全不碰 ABI/布局**。
C2/C3 收益不足以抵消改动 ABI 的风险 —— 按 CLAUDE.md 的
「低优先级让位于高优先级」+ 最小改动原则，**本子任务只做 C1**。

## 六、验收

| 项 | 判据 |
|---|---|
| page-0071 ≤ 8 MB | 实测 |
| **entry 数不变** | 产物中 `entryN` 声明数 == 10251 |
| **entry_total_size 之和不变** | 从产物文本求和，与基线比对 |
| 构建通过 | system chunk → entry.exe |
| **fact 无回归** | 与基线逐 subject 比对 |
| runtime 扫描不变 | `GcRegisterSlotMapsFromSection` 未改动（零 diff）|

## 七、问题清零

```
blocking_questions: []
question_clearance: cleared
```
