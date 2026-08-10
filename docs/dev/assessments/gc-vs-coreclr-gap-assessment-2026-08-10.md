# CRAG GC vs CoreCLR GC — 综合差距评估报告

> 日期：2026-08-10
> 对标基准：`D:\OpenSource\dotnet\runtime\src\coreclr\gc\`（权威源码还原）
> 现状基线：本会话 Phases 1-5（13 个子任务 + 补测）落地的 CRAG GC
> 方法：逐能力对照 CoreCLR 实际实现，标已对齐/部分/未做，附差距说明

---

## 一、已对齐 CoreCLR 的能力（本会话落地）

### ✅ 完全对齐（架构一致）
| 能力 | CRAG 现状 | CoreCLR 原文 |
|------|----------|-------------|
| **分代精确扫描**（GcLayout/slotmap） | GcSlotMap 精确扫描 + 保守兜底 | gcslotmap 精确 |
| **并行标记（work-stealing）** | 声明式终止（K2 简化） | mark_phase.cpp |
| **声明式 mark 终止**（GC-C1） | 收敛复查 + 重扫至稳定 | mark_phase.cpp:3107 |
| **世代感知写屏障 3 短路**（K2b） | `chaos_gc_dirty_card_dst_ref`（dst.gen0/ref null/ref.gen>=dst.gen） | JitHelpers Region64 |
| **card bundle 粗卡**（K2d） | 1bit/2MB 扫侧跳段 | card_table.cpp:1619 |
| **三档 OOM 归因 + provisional force-blocking + 压力 decommit**（D1） | 真/瞬态归因 + 压力禁 BGC defer | allocation.cpp handle_oom |
| **配置旋钮 env + AOT/JIT API**（E1） | `CHAOS_GC_<Key>` + `chaos_gc_config_*` | gcconfig.h |
| **Dependent 运行期收敛**（G1） | 去 3 轮 cap do-while | objecthandle.cpp:1203 |
| **handle 代剪枝 + 触发原因追踪**（F1/H1） | per-handle nursery bit + GcTriggerReason | handletable / gcrecord.h |

### 🟢 部分对齐（语义达成，未 100% 结构同构）
| 能力 | CRAG | CoreCLR | 差距 |
|------|------|---------|------|
| **BGC 并发** | 并发 mark + 并发 sweep（J1）+ SATB | 并发 mark+sweep+分相 | CRAG 无 1st/2nd 快照分相 |
| **region→gen 映射**（K2a） | 1 字节 skewed 表 | region_info | CRAG 无 plan-gen 高位 |
| **OOM 半量预算** | kOomReportHalfBudget 上报 | oom_budget 缩放 | CRAG 只改上报不缩放预算 |
| **写屏障切换** | 函数指针（计划） | StompWriteBarrier 换函数体 | CRAG 简化为指针 |

---

## 二、剩余差距（按严重度）

### 🔴 高优先级（影响正确性/生产）

**1. K2c 世代写屏障未完成管线 regen 验证**
- codegen 4 个 emit 点已改 `_dst_ref`，但 **foundation-dll 管线 regen 未跑**（`tests/e2e/translation/` 生成代码未见 `_dst_ref`）。
- 风险：生成的 AOT 代码仍走单参 `chaos_gc_dirty_card`，世代短路未实际生效；且 `_dst_ref` 的运行时符号是否在生成代码 linking 未知。
- **需做**：跑完整 foundation-dll 管线 regen + 生成 native 编译验证。

**2. 世代写屏障的真 SQL 正确性未压力验证**
- K2b 短路逻辑已单测，但**高并发/大堆下 gen0→gen0 短路是否漏跨代引用**无压力场景。
- CoreCLR 的 mark 扫脏卡 `gen>condemned→skip` 过滤（mark_phase.cpp:1388）CRAG 未做（K2e 判定为既有 nursery fast-skip 覆盖）。需确认不漏。
- **需做**：多线程 ref-store 到各自 nursery + 触发 young GC 的压力场景。

### 🟠 中优先级（结构/能力差距）

**3. Server GC 多堆（dynamic heap count）完全未对齐**
- CoreCLR `dynamic_heap_count.cpp`：运行期增减 server 堆数。CRAG 只有 `gc_coordinator`/`gc_heap_manager`（K-phase 之外未动），**多堆协调未验证**（GC_SERVER 默认 ON 无集成测试）。
- **需做**：dynamic heap count + 集成测试。

**4. provisional 完整形态（Cerbase：gen1 全 compact + NGC2 排队）未做**
- 我只实现 force-blocking。CoreCLR `collect.cpp:1638` 的"高记忆+高碎片→gen1 强制 compact + NGC2 而非全 GC"未对齐。
- **需做**：完整 provisional。

**5. 分代方案：无 gen2/max-generation 概念、无 LOH 世代细分**
- CRAG 是 2 代（young/old）+ LOH 独立。CoreCLR 是 gen0/1/2 + LOH/region，代间提升语义更细。
- CRAG 已用 region-gen 表（0/2），中途代 gen1 仅 young 侧。

**6. R2 值类型嵌套写屏障假阳性**
- 文档自标"可加 runtime GC-heap-pointer 检测函数避免不必要 barrier"（24 号:478），未做。

### 🟡 低-选做（非核心，可延）

**7. BGC 分相状态机（1st/2nd 快照 + concurrent plan/sweep）** — J1 已并发 sweep，分相细化未做。
**8. CDAC/SOS 调试器支持** — 无（对标注释为非目标）。
**9. GCProvModeStress 压测旋钮** — 无。
**10. SoftwareWriteWatch** — 非目标。

---

## 三、深度分析（几个关键判断）

1. **架构对齐已达"抽象级"**：region 写屏障、OOM 阶梯、mark 终止、handle 剪枝在**语义上忠实对齐 CoreCLR**，但**多数是"源码级"而非"完整管线+压力验证级"**。最大未验证点是 K2c 世代写屏障的完整 regen。

2. **最该补的是"生成代码实际吃到世代短路"**：K2b/K2d 的省卡价值，只有 foundation-dll regen 后生成代码真用 `_dst_ref` + 压力场景验证才兑现。这是当前"实现 vs 生产"最大落差。

3. **Server GC 多堆是最大的未对齐能力**：CoreCLR 的动态堆数在 `dynamic_heap_count.cpp` 是闭环自适应，CRAG 连多堆集成测试都没有（GC_SERVER 默认 ON 无验证）。这块工作量不亚于一个 phase。

4. **provisional 只做了一半**：force-blocking 是皮毛，CoreCLR 的"gen1 强制 compact + NGC2"才是降延迟关键。

---

## 四、对标量化（打分）

| 维度 | CoreCLR | CRAG | 对齐度 |
|------|---------|------|--------|
| 架构/核心（精确扫描、并发 mark、mark 终止） | 10 | ~9 | 高 |
| 世代写屏障（region→gen + 短路 + bundle） | 10 | ~8（源码级，未 regen 验证） | 中高 |
| OOM 降级链 | 10 | ~7（半个 provisional，无半量预算缩放） | 中 |
| BGC 并发 | 10 | ~7（并发 mark/sweep，无分相） | 中 |
| Server GC 多堆 + 动态堆数 | 10 | ~3（协调器在但未验证） | 低 |
| 配置/可观测性（config/events/reason） | 10 | ~7 | 中 |
| 成熟度/压力验证 | 10 | ~5（单元充分，压力/回归不足） | 低-中 |

---

## 五、结论与建议顺序

**现状**：CRAG GC 在**核心 GC 语义上已高度对齐 CoreCLR**（精确扫描、并发、世代写屏障、OOM、配置），但存在 3 个"实现 vs 生产"落差：
- 🔴 **K2c 世代写屏障未管线 regen 验证**（最优先）
- 🟠 **Server GC 多堆未验证**
- 🟠 **provisional 只做一半**

**修顺序建议**：
1. **K2c regen**（让世代写屏障真正进生成代码）：foundation-dll 管线全量 regen + 生成 native 编译。
2. **世代写屏障压力场景**（多线程 ref-store + young GC，验证不漏跨代引用）。
3. **Server GC 多堆集成测试**（GC_SERVER=ON 实际跑）+ 后续 dynamic heap count。
4. **provisional 完整形态**（gen1 compact + NGC2）。

> 说明：本报告为纯代码+对照评估，未跑 full foundation-dll pipeline。K2c 的"未 regen 验证"是基于 `tests/e2e/translation/` 无 `_dst_ref` 的静态证据。
