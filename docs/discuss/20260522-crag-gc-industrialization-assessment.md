# CRAG GC 工业化完成度综合评估报告

> 评估日期：2026-05-22
> 分析范围：`src/native/runtime-core/gc/` 全模块 + wiki 文档 + 压力测试数据
> 对比对象：CoreCLR WKS/SVR GC / Mono SGen GC / Unity IL2CPP Boehm GC
> 评级标准：工业化 = 可直接用于生产游戏项目，承受真实玩家负载

---

## 一、总结论

**CRAG GC 当前处于"功能完备、成熟度中等"阶段。** 核心 GC 算法（分代复制、并行标记-清扫、并发标记、压缩、精确扫描、LOH、POH、BGC）已全部实现，与 CoreCLR WKS GC 功能级差距已基本消除。但与工业化要求相比，**成熟度处于 CoreCLR 2015 年前后水平**，距离 CoreCLR 当前（2026）的工业化程度有约 **2-3 年的工程差距**。

### 综合评分

| 维度 | CRAG | CoreCLR WKS | CoreCLR SVR | Mono SGen | Unity/Boehm |
|------|------|-------------|-------------|-----------|--------------|
| **功能完整性** | 8.5/10 | 10/10 | 10/10 | 9.0/10 | 7.0/10 |
| **性能（暂停）** | 7.5/10 | 9.0/10 | 9.5/10 | 7.5/10 | 3.0/10 |
| **性能（吞吐量）** | 7.0/10 | 9.5/10 | 10/10 | 8.5/10 | 5.0/10 |
| **成熟度/稳定性** | 5.0/10 | 10/10 | 10/10 | 9.5/10 | 9.5/10 |
| **iOS 热更适配** | 9.0/10 | 5.0/10 | 5.0/10 | 6.0/10 | 7.0/10 |
| **DLL 加载/卸载** | 9.5/10 | 8.0/10 | 8.0/10 | 7.0/10 | 1.0/10 |
| **调试/诊断能力** | 4.0/10 | 10/10 | 10/10 | 8.0/10 | 6.0/10 |
| **平台覆盖** | 2.0/10 | 9.5/10 | 9.0/10 | 9.5/10 | 8.0/10 |
| **综合工业化** | **5.5/10** | **9.5/10** | **9.5/10** | **8.5/10** | **5.5/10** |

---

## 二、已完成的能力（不重复报告）

详见 `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/24-CRAG-GC架构参考.md` 的完整完成度矩阵。核心已实现：

| 组件 | 复杂度 | 对比 CoreCLR |
|------|--------|-------------|
| 3 代分代 (Nursery+Gen1+Old) | 高 | 对等 |
| Per-thread TLS nursery bump allocation | 中 | 对等 |
| Young GC Cheney BFS 复制 | 中 | 架构不同（CoreCLR 用 hybrid） |
| Old gen mark-sweep (28 size class) | 高 | 对等 |
| 并行标记 (work-stealing) | 高 | 对等 |
| BGC 并发标记 (SATB, tri-color) | 极高 | 对等 |
| 两层卡表写屏障 | 中 | 对等 |
| 精确 GcLayout 扫描 | 高 | 对等 |
| Page 内压缩 | 高 | 简化（CoreCLR 跨 page） |
| LOH (85KB+ VirtualAlloc) | 高 | 对等 |
| POH (Pinned Object Heap) | 中 | 对等 |
| GCHandle (strong/weak/pinned/dependent) | 高 | 对等 |
| Per-thread handshake safepoint | 极高 | AOT 合理替代方案 |
| 事件驱动安全点等待 | 中 | 优于 CoreCLR 的 spin-wait |
| SATB freeze 协议 | 高 | 对等 |
| APC 回退机制 | 中 | AOT 特有方案 |
| 专用 Finalizer 线程 | 中 | 对等 |
| GCCollectionMode / GCLatencyMode | 中 | 对等 |
| GCStress 模式 | 中 | 简化版 |
| NUMA 感知 | 中 | 基础版 |
| NO_GC_REGION | 低 | 对等 |
| MemoryDomain + 域卸载集成 | 极高 | **CoreCLR 不具备** |
| 静态根注册 | 中 | AOT 必要 |

---

## 三、工业化差距详细分析

### P0 级（生产阻塞 — 必须解决才能上线）

| # | 差距 | CRAG 现状 | CoreCLR 标准 | 影响 | 预估工作量 |
|---|------|----------|-------------|------|-----------|
| **G-1** | **Page 永不归还 OS** | 100%-free normal pages 从不释放；page_count 无界增长 | 空 page 被合并到 segment 尾部，完全空闲时 VirtualFree | 长时间运行的游戏内存 RSS 持续增长，最终 OOM | 2-3 周 |
| **G-2** | **FindPage O(N) 线性扫描** | `FindPageIndexByAddr` 使用线性扫描，page_count=1000 时 ~1μs，10000 时 ~10μs | balanced tree / radix table / 分级 page table，O(log N) 或 O(1) | Full GC 暂停时间随堆增长线性增长；10000+ pages 时暂停不可控 | 2-3 周 |
| **G-3** | **BGC-YoungGC 交互 segfault** | `gc_bgc_smoke.cpp` Test 3 预存 segfault，被隔离 | 零已知 BGC-YoungGC 交互问题 | 生产环境必现的并发崩溃 | 1-2 周 |
| **G-4** | **缺少 Server GC 模式** | 只有 WKS 模式 | WKS + SVR 双模式，SVR 使用 per-CPU heap 和专用 GC 线程 | 多核服务器吞吐量受限；无法充分利用 >8 核 | 2-3 月 |

### P1 级（严重性能/正确性 — 严重影响生产可用性）

| # | 差距 | CRAG 现状 | CoreCLR 标准 | 影响 | 预估工作量 |
|---|------|----------|-------------|------|-----------|
| **G-5** | **无硬件写屏障** | 纯软件卡表 ~6 指令 | ARM64 使用 `dvmb`/`dc cvap` 硬件屏障 | ARM 平台写屏障额外 5-10ns | 1-2 周 |
| **G-6** | **DependentHandle 仅 3 轮定点** | 3 轮 fixed-point iteration | 高效二次标记（handle table 内建标记） | ConditionalWeakTable/Ephemeron 正确性无问题但效率低 | 1-2 周 |
| **G-7** | **ETW/诊断事件缺失** | 仅 native `GcFireEvent` 回调表 | ETW 事件全覆盖（GCStart/GCEnd/GCAllocationTick 等 + perfview 集成） | 无法用标准 profiler（perfview/VTune）诊断 GC 问题；崩溃无法事后分析 | 2-4 周 |
| **G-8** | **无跨 page 压缩** | 仅 page 内 compaction | gen2 compact 跨 page 搬迁对象 | 碎片率高时堆大小无法回缩，内存膨胀 | 2-4 周 |
| **G-9** | **LOH 不释放到 OS** | 保留最多 4 个空闲 segment | 空闲 segment 立即 VirtualFree | LOH 内存只增长不缩小 | 1 周 |
| **G-10** | **BGC 并发 sweep 未实现** | Sweep 阶段 STW | BGC 并发 sweep（per-page sweep_lock） | BGC 标记完成后 STW sweep 抵消了并发收益 | 2-3 周 |
| **G-11** | **无 GC 背压/调速** | 仅 EMA 存活率 | GC 触发 + 分配速率 + 内存压力 + 临界比例综合决策 | 高分配率场景 GC 频率不够自适应 | 1-2 周 |
| **G-12** | **Finalizer run on OOM 未保证** | finalizer 在线程退出时可能没机会跑 | CoreCLR 在 AppDomain/进程退出时尽力执行 finalizer | 非托管资源泄漏风险 | 1 周 |

### P2 级（功能缺失 — 影响完整性和可观测性）

| # | 差距 | CRAG 现状 | CoreCLR 标准 | 影响 |
|---|------|----------|-------------|------|
| **G-13** | **GCMemoryInfo BCL 侧缺失** | native 结构体实现，托管侧无对应 `GCMemoryInfo` 类型 | `GC.GetGCMemoryInfo()` 返回完整结构 | 托管代码无法获取 GC 内存统计 |
| **G-14** | **GCNotification 托管侧未接线** | native 回调已实现 | `GC.RegisterForFullGCNotification` | .NET 标准 GC 通知 API 不可用 |
| **G-15** | **缺少 GC 基准回归测试套件** | 压力测试有 17 场景但无基准门 | dotnet/performance GC 基准 | 性能退化无法自动检测 |
| **G-16** | **缺少 SOS 调试扩展** | 无 | `sos.dll` + `GCHeap` 命令 | 生产 crash dump 无法分析 GC 堆 |
| **G-17** | **缺少 Arrhenius 升温测试** | 无长时间升温测试 | 72h+ 连续 GC + 分配测试 | 内存泄漏/碎片增长无法在短测试中发现 |
| **G-18** | **值类型嵌套引用写屏障假阳性** | 当前对所有值类型字段赋值无条件 DirtyCard | 运行时检测值类型是否在 GC 堆中 | 栈上值类型不必要的卡表标记开销 |
| **G-19** | **Promotion 年龄阈值 = 1** | 对象一次 young GC 存活即 promote | 默认 gen0→gen1 存活一次（CoreCLR 也类似但有多代缓冲） | 短期存活对象过早进入 old gen 增加碎片 |
| **G-20** | **无 LowLatency mode 真正抑制** | LowLatency 仅标记未真正抑制 BGC | LowLatency 模式下 BGC 降低频率 | 低延迟场景 pause 不可控 |

### P3 级（增强优化 — 长期工程积累）

| # | 差距 | CRAG 现状 | CoreCLR 标准 |
|---|------|----------|-------------|
| G-21 | 无大页支持 (2MB/1GB) | VirtualAlloc 默认 size | `THP`/`LargePages` |
| G-22 | 无 GC 线程 CPU 亲和性设置 | 默认系统调度 | processor affinity + idealized processor |
| G-23 | 无 GC 堆布局优化 | 固定 NUMA 感知 | per-heap per-generation 缓存优化 |
| G-24 | 无卡片标记 (card mark) 批量处理 | 逐个处理 | card bundle (1024 cards 为一组) |
| G-25 | 无 GC 并发根扫描 | STW 根扫描 | concurrent root scanning (on Linux) |
| G-26 | 无 GC 自适应模式切换 | 手动配置 LatencyMode | 根据内存压力和分配速率自动切换 |
| G-27 | 平台覆盖：仅 Windows 完整 | Linux/iOS 为 stub 或未验证 | Windows/Linux/macOS/Android/iOS 全平台 |
| G-28 | 无内存压力反馈 (AddMemoryPressure) | 已实现 basic API | 完善的外部内存压力反馈 + GC 触发 |
| G-29 | 无 GC 编译时选择 | C++ `#ifdef` 切换 | CoreCLR 编译时 FEATURE_* 宏精细控制 |
| G-30 | 测试覆盖不足（已知 4 个预存问题） | ~27 个测试 target | 千级测试 + fuzz CI |

---

## 四、按工业化阶段的分批建议

### Phase 1 — 生产最小可行（P0 阻塞修复，~2 个月）

| 优先级 | 任务 | 工作量 | 收益 |
|--------|------|--------|------|
| **P0** | Page decommission（归还 OS） | 2-3 周 | 消除 RSS 无限增长，OOM 风险 |
| **P0** | FindPage 索引化（radix tree/page table） | 2-3 周 | 大堆 Pause 可控 |
| **P0** | BGC-YoungGC 交互 segfault | 1-2 周 | 消除 BGC 并发崩溃 |
| **P1** | LOH 空闲 segment 即时 VirtualFree | 1 周 | LOH 内存回缩 |
| **P1** | Promotion 年龄阈值 1→2 | 3 天 | 降低短期对象过早 promote |
| **P1** | 值类型嵌套引用写屏障假阳性消除 | 1 周 | 减少无意义的 dirty card |
| **P2** | GCMemoryInfo BCL 侧对接 | 1 周 | 托管 API 完整 |
| **P2** | GCNotification 托管接线 | 3 天 | BCL 兼容 |

**Phase 1 完成后工业化评分：6.5/10（可上线原型验证）**

### Phase 2 — 性能对等（CoreCLR WKS 级别，~4 个月）

| 优先级 | 任务 | 工作量 | 收益 |
|--------|------|--------|------|
| P1 | DependentHandle 高效二次标记 | 1-2 周 | 提升 ConditionalWeakTable 性能 |
| P1 | BGC 并发 sweep | 2-3 周 | 消除 sweep STW，充分发挥 BGC 收益 |
| P1 | GC 自适应调速（分配速率 + 内存压力 + 综合决策） | 1-2 周 | 更智能的 GC 触发频率 |
| P1 | ETW/诊断事件 | 2-4 周 | 性能问题可定位 |
| P2 | LowLatency 真正抑制 | 1 周 | 延迟敏感场景可预测 |
| P2 | GC 基准回归测试 | 2 周 | 性能退化自动拦截 |
| P1 | 跨 page 压缩 | 2-4 周 | 堆大小可回缩，碎片可控 |
| P3 | 卡片标记批量处理 (card bundle) | 1-2 周 | 减少每个 dirty card 的扫描开销 |
| P3 | Finalizer OOM 保证 | 1 周 | 资源泄漏兜底 |

**Phase 2 完成后工业化评分：8.0/10（可与 CoreCLR WKS 竞争多数场景）**

### Phase 3 — 工业化收口（线上可运维，~6 个月）

| 优先级 | 任务 | 工作量 | 收益 |
|--------|------|--------|------|
| P1 | Server GC 模式 | 2-3 月 | 多核服务器吞吐量最大化 |
| P3 | iOS/Android 平台验证 + 硬件写屏障 | 2-4 周 | 移动端可用 |
| P2 | SOS 调试扩展 | 2-4 周 | Crash dump 可分析 |
| P2 | Arrhenius 72h+ 升温测试 | 2 周 | 长时间稳定性验证 |
| P3 | 大页支持 (THP/LargePages) | 2 周 | 减少 TLB miss |
| P3 | GC 线程 CPU 亲和性 | 1 周 | 减少缓存抖动 |
| P3 | 并发根扫描 | 2-4 周 | 减少 STW pause |
| P2 | 修复 4 个预存测试问题 | 1-2 周 | 全测试 green |

**Phase 3 完成后工业化评分：9.0/10（接近 CoreCLR WKS 工业化程度）**

---

## 五、各维度与竞品深层次对比

### 5.1 暂停时间对比（预期值，100MB 堆，并发负载）

| GC 类型 | CRAG | CoreCLR WKS | CoreCLR SVR | Mono SGen | Unity/Boehm |
|---------|------|-------------|-------------|-----------|--------------|
| Young GC (avg) | ~350µs | ~50-200µs | ~100-300µs | ~300-800µs | N/A (STW full) |
| Young GC (p99) | ~600µs | ~500µs | ~800µs | ~2ms | N/A |
| BGC (background) | ~2-5ms (concurrent) | ~1-3ms (concurrent) | ~1ms (per-heap concurrent) | N/A (no BGC) | N/A |
| Full GC (STW, no BGC) | ~20-130ms | ~10-50ms | ~5-30ms | ~50-200ms | ~500ms+ |
| Full GC (compact) | ~80-180ms | ~20-100ms | ~10-60ms | ~50-150ms | N/A |
| LOH sweep | ~1ms | ~500µs | ~300µs | ~1ms | N/A |

**CRAG Young GC pause 较 CoreCLR 高的主要原因**：Cheney BFS worklist malloc/free 固定开销 + Debug 构建。Profile 构建可改善。

### 5.2 吞吐量对比（估算，纯分配场景）

| 指标 | CRAG | CoreCLR | Mono | Unity/Boehm |
|------|------|---------|------|--------------|
| 分配速度 (bump) | ~10 指令 | ~8 指令 | ~10 指令 | ~30 指令 (free-list) |
| 写屏障开销 | ~6 指令 | ~8 指令 | ~6 指令 | 无 |
| GC CPU 占用 (100MB) | ~5-8% | ~3-5% | ~5-10% | ~10-20% |
| 内存开销（相对） | ~1.5x | 1.0x | ~1.2x | ~0.8x（保守，实为碎片更多） |

### 5.3 CRAG 独特优势 — 热更新领域的决定性领先

| 能力 | CRAG | 竞品 |
|------|------|------|
| DLL 卸载回收所有内存 | O(1) region 批量释放 | CoreCLR 需 full GC + weak ref；Unity 不支持 |
| 域隔离扫描 | card table 跨域脏卡扫描 | CoreCLR 无 isolation；所有 ALC 共享堆 |
| 卸载无 pin/unpin 复杂性 | region 隔离可绕过 | CoreCLR pin 对象阻止 segment 回收 |
| 无堆碎片（域内） | 不存储跨域指针 | CoreCLR ALC 内部碎片无法避免 |

**这是 CRAG 相比 CoreCLR 的唯一本质优势**。但对于不需要热更新的游戏项目，CoreCLR 综合优于 CRAG。

---

## 六、当前已知测试问题清单

| # | 问题 | 涉及测试 | 严重性 | 说明 | 
|---|------|---------|--------|------|
| 1 | BGC-YoungGC segfault | `gc_bgc_smoke:TestBgcWithYoungGc` | **P0** | 被隔离到末尾运行，不影响其他测试 |
| 2 | POH 不在 old-gen page array | `gc_poh_test:TestPohFullGc` | P2 | 直接 g_old_gen.Collect() 挂起，workaround 通过 |
| 3 | pinned 标记内容校验不一致 | `gc_handle_test` | P2 | 某些 pinned 对象标记后内容验证不一致 |
| 4 | TLS nursery 初始化假设 | `gc_region_test` | P2 | 测试对初始化顺序有假设依赖性 |

---

## 七、推荐路线图

```
Phase 1 (2 个月) — 生产最小可行
├── [P0] Page decommission ───────────────── 第 1-3 周
├── [P0] FindPage radix tree ──────────────── 第 1-3 周
├── [P0] BGC-YoungGC segfault ─────────────── 第 3-4 周
├── [P1] LOH segment VirtualFree + promotion 2 ─ 第 4-5 周
├── [P2] GCMemoryInfo + GCNotification BCL ── 第 5-6 周
├── [P1] 假阳性写屏障修复 ────────────────── 第 6-7 周
└── [Phase 1 验收] 24h 升温测试 + 回归 ──── 第 7-8 周
                        ↓
Phase 2 (4 个月) — CoreCLR WKS 对等
├── [P1] BGC 并发 sweep ─────────────────── 第 1-3 周
├── [P1] ETW 诊断事件 ────────────────────── 第 3-6 周
├── [P1] 高效 DependentHandle + GC 调速 ──── 第 6-8 周
├── [P2] LowLatency 真正抑制 + 基准测试 ──── 第 8-10 周
├── [P1] 跨 page 压缩 ───────────────────── 第 10-13 周
├── [P3] card bundle 批量处理 ───────────── 第 13-15 周
└── [Phase 2 验收] perf benchmark 对比 ──── 第 15-16 周
                        ↓
Phase 3 (6 个月) — 工业化收口
├── [P1] Server GC 模式 ─────────────────── 第 1-12 周
├── [P3] iOS/Android 全平台验证 ──────────── 第 3-6 周
├── [P2] SOS 调试扩展 ───────────────────── 第 6-10 周
├── [P2] Arrhenius 72h 升温 ─────────────── 第 10-12 周
├── [P3] 大页 + CPU 亲和性 ──────────────── 第 12-15 周
├── [P3] 并发根扫描 ────────────────────── 第 15-20 周
├── [P2] 修复全部已知测试问题 ───────────── 第 20-22 周
└── [Phase 3 验收] 全平台 72h soak ──────── 第 22-24 周
```

---

## 八、结论

**CRAG GC 已具备所有核心 GC 算法级能力，架构设计合理。** 与 CoreCLR 的差距已经从"有没有"转为"好不好"：

1. **功能完整性**：CRAG ≈ CoreCLR WKS 的 85% — 所有核心 GC 机制均实现
2. **成熟度**：CRAG ≈ CoreCLR 2015 水平 — 需要 2-3 年工程积累
3. **性能**：CRAG ≈ CoreCLR WKS 的 70-80% — 主要差在细节优化而非架构
4. **调试/诊断**：CRAG ≈ CoreCLR 的 40% — 这是最大短板，生产运维关键
5. **平台覆盖**：CRAG ≈ CoreCLR 的 20% — 目前仅 Windows 验证
6. **热更新**：**CRAG 全球领先** — CoreCLR/Unity/Mono 均无对等能力

### 上线门槛判断

- **不能上线的原因**：P0 级 4 个阻塞项（page decommission, FindPage O(N), BGC-YoungGC segfault, 无 Server GC）会在生产环境引发 OOM、pause 不可控、随机崩溃
- **最小上线条件**：Phase 1 完成后（~2 个月），可达"原型验证可上线"级别
- **工业化标准上线**：Phase 3 完成后（~12 个月），可达"接近 CoreCLR WKS"级别
