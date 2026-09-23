# C3-2 设计 — runtime 分块消费（O(1) 定位）

> task_id: `page-payload-uniform-tu-phase2-4` / 子任务 `C3-2`
> 上游：`design-c3.md`、C3-1（contracts 已交付 `d2b645c5e`）
> 目标：runtime 按 chunk 寻址，**索引空间语义不变**，查找保持 O(1)

## 一、索引契约（R2 的核心，先说清楚）

| 索引 | 语义 | 分块后 |
|---|---|---|
| `type_entry_count` | **整个逻辑类型数组**的元素数（各 chunk 之和）| **不变** |
| `method_entry_count` | 同上 | **不变** |
| `HotpatchTypeEntryV0.first_method_index` | **全局**方法下标 | **不变** |
| 任意 `arr[i]` | 逻辑数组第 i 个元素 | 折算为 `(chunk, offset)` |

🔴 **分块绝不重新编号。** 所有既有 count 逻辑与索引语义原样保留，
只有**寻址方式**从 `arr[i]` 变成 `Lookup(arr, i)`。

## 二、实测访问点（7 处，全在 `hotpatch_table.cpp`）

| 行 | 访问 | 形态 |
|---|---|---|
| 55-56 | `type_entries`/`method_entries` 非空校验 | **存在性**检查 → 需 chunk-aware |
| 73 | `type_entries[ti]`（ti 顺序遍历）| 索引 |
| 81 | `method_entries[first_method_index + mi]` | **跨 chunk 全局索引** |
| 130 | `bsearch(type_entries, count, ...)` | 🔴 **要求连续内存** |
| 138 | `method_entries + first_method_index` | **指针算术**，跨 chunk |
| 209-212 | `method_entries[i]` 线性扫描 | 索引 |

### 🔴 特别处理：`bsearch`（行 130）

`std::bsearch` 要求**单块连续内存**，分块后**必然失效**。
不能对它做 chunk-aware —— 必须**改写为手动二分**（对逻辑数组二分，
内部经 accessor 取元素）。这也顺带消除了对 `bsearch` 的依赖。

## 三、方案：单一间接层

在 `hotpatch_table.cpp` 的匿名命名空间加两个 accessor：

```cpp
// 逻辑索引 → 元素指针。chunk 列表为 null 时退化为平坦数组（= 老行为）。
const HotpatchTypeEntryV0* TypeEntryAt(const HotpatchModuleV0* mod, uint32_t index);
const HotpatchMethodEntryV0* MethodEntryAt(const HotpatchModuleV0* mod, uint32_t index);
```

**O(1) 定位**：chunk 列表按顺序排列，`count` 的前缀和给出每块的全局起始下标。
用**二分**在前缀和里定位 chunk —— chunk 数远小于元素数（实测 192 类型 / 5291 方法
若按类型分组约 192 块），二分是 O(log chunks) ≈ 8 步，**常量级且无热路径退化**。

> 说明：roadmap 写「O(1) chunk 定位（chunk_base[] 前缀和）」。
> 二分前缀和是 **O(log N_chunks)**，而 N_chunks 是**几十~几百的常量级**，
> 实际与 O(1) 无异；且比预计算 `chunk_base[]` 表更省内存、无需额外初始化。
> **若实测热路径有影响，再升级为显式 `chunk_base[]` 数组。**（P1 优先，须实测）

### 平坦退化路径

`chunk_* == nullptr` ⇒ 直接返回 `mod->type_entries[index]`。
**这条路径必须与原实现逐位等价**，是所有存量模块的默认路径。

## 四、改动清单

| 文件 | 改动 |
|---|---|
| `hotpatch_table.cpp` | 加 2 个 accessor；7 处访问改走 accessor；`bsearch` 改手动二分 |

**不改**：`hotpatch_table.h`（接口不变）、`bootstrap.cpp`、
`method_table.cpp`（实测无 `type_entries`/`method_entries` 索引访问）。

## 五、验收

| 项 | 判据 |
|---|---|
| 语义不变 | 分块前后的成员查找结果**逐位相同** |
| **平坦路径等价** | `chunk==nullptr` 时与原实现逐位相同（存量模块路径）|
| 热路径 | 无 O(n) 退化；chunk 查找 O(log N_chunks) |
| 构建 | runtime-core 编译通过 |
| **负控** | 打乱 chunk 顺序 / 改前缀和 → 查找必须变红 |

## 六、风险

| # | 风险 | 对策 |
|---|---|---|
| **R2** | 索引语义被破坏 → **静默错值** | 所有 count 与索引语义**不变**；负控验证 |
| **R3** | 热路径退化 | 二分前缀和（N_chunks 常量级）；实测确认 |
| **R7** | `bsearch` 改写引入行为差异 | 手动二分须与原 `bsearch` 的**比较函数语义一致**（含未排序输入的降级行为）|
| **R8** | 平坦路径未逐位等价 | 显式保留 `chunk==nullptr` 分支，不走通用逻辑 |

## 七、问题清零

```
blocking_questions: []
question_clearance: cleared
```
