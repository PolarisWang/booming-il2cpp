# 三代分代 GC 调研与 CRAG 性能优化预估

> 日期: 2026-05-20
> 上下文: CRAG (Chaos Region-Aware GC) 当前为 2 代设计 (Young + Old), 调研是否值得加入中间代 (Gen1)。

---

## 1. CRAG 当前分代模型

### 1.1 物理代

| 代 | Region 类型 | 大小 | 分配方式 | 回收方式 |
|---|------------|------|---------|---------|
| Young (Gen0) | REGION_NURSERY | 16 MB (固定) | TLAB bump-pointer | Cheney 复制 (STW) |
| Old (Gen1) | REGION_TENURED | 动态 (按页增长) | MarkSweepOldGen::Allocate | Mark-Sweep (STW 或 BGC) |

### 1.2 Survivor 区 (伪 Gen1)

新生代 16 MB 被等分为两块:
- **Young half** (8 MB): 新对象分配区
- **Survivor half** (8 MB): 一次 Young GC 存活对象的暂存区

晋升路径:
```
分配 → Young half → (存活第1次 Young GC) → Survivor half → (存活第2次 Young GC) → Old gen
```

关键限制:
- `kPromotionAgeThreshold = 1` — 在 Survivor 区存活 1 次 Young GC 即晋升 Old
- Survivor 区大小**固定为 8 MB**，不能独立增长
- Survivor 区**不能独立回收** — 它总是随 Young GC 一起处理 (Phase 2b 扫描 + Phase 4 重置)
- Survivor 区满时直接溢出到 Old gen，无中间回退

### 1.3 回收类型

| 类型 | 触发条件 | 暂停 |
|-----|---------|------|
| YOUNG (Gen0 only) | Nursery 满 / 调度器触发 | STW (年轻代复制) |
| FULL (Gen0+Gen1) | 页增长≥32 / 分配量>4×堆大小 / 显式请求 | STW (全堆标记+可选的压缩) |
| FULL_BGC | 同上，非紧急时 | 并发标记 + STW re-mark |

**没有 Gen1-only 的回收。** Survivor 区始终与 Young GC 绑定，不能被单独回收。

---

## 2. CoreCLR 3 代模型对比

### 2.1 物理代

| 代 | 典型预算 | 回收频率 | 暂停时间 | 回收范围 |
|---|---------|---------|---------|---------|
| Gen0 | ~256 KB | 数秒一次 | <1 ms | Gen0 区 |
| Gen1 | ~2 MB | Gen0 每收集 N 次触发一次 | ~5 ms | Gen0 + Gen1 区 |
| Gen2 | 可达 GB | 每日数次 (实际受内存压力) | 50ms+ | 全堆 |

### 2.2 核心差异

| 维度 | CoreCLR 3 代 | CRAG 当前 2+Survivor |
|------|-------------|-------------------|
| Gen1 大小 | 自适应 (根据存活率动态调整) | 固定 8 MB (Survivor half) |
| Gen1 可独立收集 | 是 (Gen1 collection 不碰 Gen2) | 否 (随 Young GC 绑定) |
| 晋升年龄 | 可超过 1 (对象可在 Gen1 多轮存活) | 固定 1 (进 Survivor 后下次 GC 即升 Old) |
| 写屏障开销 | 高 (所有跨代引用需记录) | 低 (仅 old→young 跨代引用) |
| 自适应调优 | 完整 (预算动态调优) | 基础 (EMA 存活率 + 固定 nursery 大小) |

### 2.3 Gen1 在 CoreCLR 中起什么作用

Gen1 本质上是一个**缓冲代**:

1. **过滤短命对象**: 很多对象能存活一次 Gen0 收集 (跨越函数调用的局部变量)，但在 Gen1 的下一次收集前就死亡。如果没有 Gen1，这些对象会直接进入 Gen2，迫使 Gen2 收集更频繁。
2. **吸收分配峰值**: 瞬时高分配场景下，Gen0 可能迅速填满。Gen1 提供额外的缓冲空间防止对象溢出到 Gen2。
3. **减少碎片**: Gen2 的压缩成本很高，Gen1 的复制收集天然是整理压缩的。

---

## 3. 三代模型的优势 (针对 CRAG)

### 3.1 如果 CRAG 增加真正 Gen1

设计变化:

1. **新增 REGION_GEN1 Region 类型**: 在 Nursery 和 Tenured 之间插入动态大小的 Gen1 区域
2. **Gen1 可独立收集**: 当 Gen1 存活率低时，可以独立回收而不触发 Full GC
3. **多轮年龄**: 取消固定 `kPromotionAgeThreshold=1`，支持对象在 Gen1 存活多轮

### 3.2 收益评估

| 场景 | 当前 2 代 | 加 Gen1 后 |
|------|---------|-----------|
| 帧尾死亡对象 (存活 2-3 Young GC) | 升到 Old gen → 碎片积累 | 在 Gen1 停留 → 独立回收 |
| 高分配峰值 ((Instantiate)批量生成) | Survivor 满 → 涌入 Old gen | Gen1 吸收 → 逐轮回收 |
| Full GC 频率 | 偏高 (Old gen 增长快) | 降低 (Gen1 过滤大部分) |

### 3.3 代价

| 代价项 | 说明 | 严重程度 |
|-------|------|---------|
| 写屏障复杂度 | 需跟踪 Gen1→Nursery 跨代引用 (新增 L1 卡表段) | 中 |
| Gen1 元数据 | Region 管理、Age 位、跨代引用集 | 低-中 |
| Gen1 扫描 Phase | Young GC 需新增 Phase 扫描 Gen1→Nursery 引用 | 中 |
| 内存开销 | Gen1 区域额外占用 (典型 16-64 MB) | 低 (游戏场景可接受) |
| 工程复杂度 | 新增 Gen1 回收路径、调度决策、自适应调优 | 高 |

### 3.4 制约因素分析

CRAG 的 Survivor 区已经承担了 Gen1 的一部分工作。当前设计的核心瓶颈不在于"有没有 Gen1"，而在于:

1. **Survivor 区固定 8 MB**: 无法根据分配压力动态调整
2. **无独立回收**: Survivor 区里的"死亡"对象不能独立清理
3. **晋升太快**: `kPromotionAgeThreshold=1` 导致对象太快进入 Old gen

---

## 4. 性能优化预估

### 4.1 基于 CoreCLR 经验数据的推算

CoreCLR 的典型存活率模式:
- Gen0 存活率: ~10-30% (90% 以上的对象在 Gen0 死亡)
- Gen1 存活率: ~50-70% (Gen1 本质上是"筛选器")
- Gen2 不收集时的对象永不死亡

### 4.2 CRAG 场景 (Unity/IL2CPP 游戏)

游戏 workload 的分代特征:
- **帧分配**: 大量小对象 (Vector3, Transform, Renderer 相关) → 每一帧死亡
- **场景加载**: 中等生命周期对象 (GameObject, Component) → 存活数秒到数分钟
- **持久对象**: Manager/System 单例, Asset 缓存 → 永久存活

当前 2 代模型的问题:
- 场景加载产生的"中生命周期"对象经过 2 次 Young GC 后全部进入 Old gen
- 这些对象在场景切换时(数分钟后)才需要回收 → 触发 Full GC
- 如果场景切换频繁 (开放世界游戏), Full GC 频率会成为性能瓶颈

### 4.3 加入 Gen1 后的预估改善

| 指标 | 改善幅度 | 说明 |
|------|---------|------|
| Full GC 频率 | 降低 30-50% | Gen1 过滤中生命周期对象 |
| Young GC 暂停 | 增加 5-15% | Gen1 引用扫描额外开销 |
| 写屏障开销 | 增加 ~3-8% | Gen1→Nursery 卡表维护 |
| Old gen 碎片 | 降低 (qualitative) | 更少对象进入 Old gen |
| P95 暂停时间 | 降低 20-40% | Full GC 减少, 最坏情况变短 |

注意: 这些估算基于 CoreCLR 公开数据和游戏 workload 的类比，具体数值需通过 benchmark 验证。

### 4.4 不加入 Gen1, 优化当前 2 代模型的方案

相比加入 Gen1 的高工程成本，以下低投入方案可能提供更好的性价比:

1. **动态 Survivor 大小**: 将固定 8 MB Survivor 改为动态 (如 `min(8 MB, 当前老年代空闲页的 10%)`)
2. **提高晋升阈值**: `kPromotionAgeThreshold = 2` 或 `3`，让对象在 Survivor 区存活更多轮，延缓进入 Old gen
3. **Survivor 独立收集**: 在 Survivor 区满时触发"微 Young GC" (仅收集 Survivor 区中的死亡对象，不进行 Cheney 复制全量 Young GC)
4. **自适应 Survivor 溢出**: Survivor 满时不直接溢出到 Old gen，而是触发一次"紧急 Young GC" 清理 Survivor 区的死亡对象后再晋升

### 4.5 优先推荐方案

**快速见效方案 (2-3 周工程投入):**

| 步骤 | 改动 | 预期收益 | 风险 |
|------|-----|---------|------|
| 1 | `kPromotionAgeThreshold = 2` (当前 1 → 2) | Full GC -10~20% | 低 (仅改常量) |
| 2 | Survivor 动态大小 | Full GC -15~25% | 中 (需改 `InitYoungGeneration` + GC 后调整) |
| 3 | Survivor 区独立回收 | Full GC -20~30% | 高 (需要新增 GC 路径) |

**完整三代方案 (2-3 月工程投入):**

| 改动 | 工程量 | 收益 | 建议 |
|------|-------|------|-----|
| 新增 REGION_GEN1 | 大 | Full GC -30~50% | 待 V0 稳定后评估 |
| Gen1 独立收集 | 大 | 同左 | 同上 |
| 自适应 Gen1 预算 | 中-大 | 边际收益 | 同一 |

---

## 5. 结论

### 不建议当前阶段加入完整 3 代模型

理由:
1. CRAG 的 Survivor 区已在一定程度上承担 Gen1 的角色
2. 工程成本高 (2-3 月)，与当前 V0 阶段的重心不匹配
3. 性能收益主要通过简单改动 (晋升阈值、动态 Survivor) 即可实现大部分

### 建议的短期优化路线

1. **立即**: `kPromotionAgeThreshold = 1 → 2` — 改动 1 行常量，预期减少 Full GC 10-20%
2. **短期**: 动态 Survivor 大小 — 跟随实际分配压力调整，预期减少 Full GC 15-25%
3. **中期评估**: Survivor 区独立回收 — 新增微 GC 路径，预期减少 Full GC 20-30%
4. **长期 (V1)**: 在独立 Survivor 收集验证通过后，将其演进为完整 Gen1

### 待验证数据需求

在决定投入完整 3 代方案前，需要采集以下数据:
1. 当前 Full GC 频率和暂停时间分布 (已有 `g_gc_pause_histogram` 和 `gc_event_ring`)
2. 对象在 Survivor 区的实际死亡率 (需新增统计点: `survivor_alloc_bytes` vs `survivor_survive_bytes`)
3. Old gen 中"中生命周期"对象占比 (通过 BGC 标记前后的存活率差估算)
