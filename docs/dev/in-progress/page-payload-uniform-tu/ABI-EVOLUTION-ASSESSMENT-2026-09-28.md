# ABI 演进评估 — 三个超预算单元的可行性与代价

> task_id: `page-payload-uniform-tu`
> 创建：2026-09-28
> 触发：用户指令「评估 ABI 演进方案」（针对 gcslotmap）
> 性质：**评估 + 归因**，不改代码（实现另见下节）
> 前置交付：`4d783f7aa`（声明块去重）、`a6ba77075`（reflection 切分 + 分包口径）

## 一、起因：交接文档的"无解"结论未经实测

`HANDOFF-2026-09-24.md` 与更早的归因报告都写 `gcslotmap` 是
「ABI 约束，runtime 线性扫描单块，**拆段即改 ABI**，无解」。

**本评估实测推翻了「无解」，也推翻了「只需 ABI 演进」** ——
真实瓶颈既不是 ABI，也不是数据量，而是**发射手法**。

## 二、实测：gcslotmap 单元的构成（读产物）

`native-aot.payload.page-0026.cpp` = **785,725 字符**：

| 部分 | 行范围 | 字符 | 占比 |
|---|---|---:|---:|
| 固定头 | L1-43 | 1,779 | 0.2% |
| **`ChaosGcSlotMapsSectionV0` 结构体定义** | L44-12090 | **408,451** | **52.0%** |
| `kChaosGcSlotMapsSection` 初始化器 | L12091-15537 | 375,299 | 47.8% |
| `kChaosGcSlotMapsSize` | L15538 | ~60 | 0.0% |

结构体的形状（`NativeAotLoweringPlanner.GcSlotMap.cs:138-154`）：

```c
struct __attribute__((packed)) ChaosGcSlotMapsSectionV0 {
    struct { CHAOS_IL2CPP_UINT32 entry_total_size; const void* code_address;
             CHAOS_IL2CPP_UINT32 frame_size; CHAOS_IL2CPP_UINT32 num_gc_slots;
             CHAOS_IL2CPP_UINT32 slots[6];  } entry0;
    struct { ... slots[6];  } entry1;
    struct { ... slots[14]; } entry2;
    // ... 共 1720 条，每条 5 行具名嵌套 struct，约 237 字符
};
```

**1720 条 × ~237 字符 = 408,451**；`Σ slots` = 9,861（`slots[N]` 分布 3..14）。

## 三、关键事实：runtime **从不按成员名访问**

```
grep -rn "\.entry\|->entry" src/native/runtime-core/gc/gc_root_scanner.cpp   → 0 命中
grep -rn kChaosGcSlotMapsSection src/native/                                  → 仅 1 处注释
```

runtime 的唯一入口是 `bootstrap.cpp:313-324`：

```c
GcRegisterSlotMapsFromSection(code_registration->slot_map_section_begin,
                              code_registration->slot_map_section_end);
```

而 `gc_root_scanner.cpp:62-92` 按**字节偏移**扫描：

```
[entry_total_size:4][code_address:8][frame_size:4][num_gc_slots:4][slots:N*4]
#pragma pack(1)
while (ptr + 4 <= end_ptr) { memcpy(&entry_total, ptr, 4); ... ptr += entry_total; }
```

⇒ **`entryN` 这个名字从未被任何消费者使用**。具名嵌套 struct 是发射冗余，
它存在的唯一理由是"让结构体定义可读"，而代价是 408K 字符 = 该 TU 的 52%。

## 四、修正后的三单元归因

| 单元 | 字符 | 真实构成 | 可解路径 |
|---|---:|---|---|
| `gcslotmap` | 785,725 | 408K **结构体冗余** + 375K 数据 | ✅ **零 ABI**（本次实现） |
| `hotpatch` | 874,416 | `s_hotpatch_entries` 等；entries 与 methods **0/10257 同序** | ⚠️ 需 ABI（见 `design-hotpatch-chunk-all-arrays.md`）|
| `dispatch` | 555,918 | `kMethodTable` 254K + `kDefaultArgThunks` 192K，**独立平行数组** | ⚠️ 与 hotpatch **不同**问题，未调查 |

**`dispatch` 与 `hotpatch` 不是同一个问题** —— 这是 Workflow 证伪的核心结论之一，
也推翻了我此前"一次改动同时消两项"的判断。

## 五、gcslotmap：为何不需要 ABI 演进

约束只有两条（都保留）：

1. `kChaosGcSlotMapsSection` 必须是**具名类型的单个对象**，供跨 TU 取地址
   （`GcSlotMap.cs:115-121,156-159` 记录了为何不能用匿名 struct / `char[]` 替代：
   与定义同 TU 时 C2373 冲突）
2. 布局必须保持 `#pragma pack(1)` 的字节序，`kChaosGcSlotMapsSize` 必须等于
   所有条目字节数之和（`CodeRegistrationV0.slot_map_section_end` 依赖它）

**两条都与 `entryN` 具名无关。** 把每个条目从"具名嵌套 struct"改为
**匿名的聚合初始化**（编译器按初始化器推导，或复用同一匿名 struct 类型），
布局逐字节不变、`kChaosGcSlotMapsSize` 不变、runtime 一字不改。

## 六、剩余风险

| # | 风险 | 对策 |
|---|---|---|
| R1 | 改成匿名后 `sizeof(ChaosGcSlotMapsSectionV0)` 变化 | 布局由 pack(1) + 成员顺序决定，与具名无关；用 `kChaosGcSlotMapsSize` 守恒断言 + 真实编译验证 |
| R2 | 去掉具名后结构体不可读、难调试 | 保留每条一行 `/* entryN */` 注释（已有），符号名可从 `code_address` 反查 |
| R3 | 数据本身 375K 仍 >350K 预算 | **本次不解决**；瘦身后该 TU 约 375K，仍超 7%，需另议（见 §七） |

## 七、实测结论与建议

- **gcslotmap 可以零 ABI 改动砍掉约 408K（52%）**，TU 从 785,725 降到约 377K
- 但**仍略超 350K**（数据本身 375,299 就是这个下界）——
  要真正达标需要另一条路（压缩数据表达，或重新审视 350K 口径对该段的适用性）
- `hotpatch` / `dispatch` 的 ABI 演进**另行设计**；`design-hotpatch-chunk-all-arrays.md`
  已给出 entries/slots 的 4 字段方案，但**尚未实现、且尚无 dispatch 的方案**

> ⚠️ 纪律提醒（来自 `HANDOFF-2026-09-24.md`，上一轮的血泪教训）：
> **不要**用 python 字符串手术反复改生成器源码；先完整读完文件，再用 Edit 逐处改，
> 每处改完即编译。
