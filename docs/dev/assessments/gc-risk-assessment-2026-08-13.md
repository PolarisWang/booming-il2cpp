# GC 综合风险评估报告（2026-08-13）

> 日期：2026-08-13
> 方法：只读综合分析 — 3 路并行 Explore（写屏障/根集、分代/晋升/Verify、测试/CI）+ 实读源码关键点 + 融合 `docs/dev/assessments/gc-vs-coreclr-review-2026-08-12.md` 与 `plan-v6/STATUS.md` 及近期 git log。
> 当前分支：`main`。R1-R6 重构、M3-T5 Page 加固、M2-T8 Verify 强化均已登 main。
> 声明：本报告为**只读评估**，不含任何代码改动。目标：回答"这个工程的 GC 完成情况 + 风险敞口"。

---

## TL;DR（一页结论）

CRAG GC 的**能力落地进度高且真实**：3 代+LOH+POH 模型、R1-R6 CoreCLR 对齐重构、Page 回收/FindPage O(log n)、kFull Verify 强化、NGC2 queue 均已登 main，GC 单测树大规模（56 个 CTest 目标）。**此前的 P1 架构缺口（Page 永不释放 + FindPage O(N)）已实质闭合。**

但 **工程"能干活的程度"远高于"能在并发下不出错的程度"**，且**最危险的缺陷恰好在 CI 门外**：

| 严重度 | 风险 | 状态 |
|--------|------|------|
| 🔴 **CRITICAL** | **A2b 跨代引用 UAF（store-then-barrier 竞态）** | **仍未修复**；已提交的复现测试 ~4/5 失败，且被 `-LE` 排除在所有 CI 门外 |
| 🔴 **CRITICAL** | **P0 止血不完整**：`CodegenStSFld`（jit_helpers.cpp:471）仍把非 GC `g_static_fields` 反向注册为 GC root | 与 e6ea531e7 的止血互相矛盾，三路 StSFld 不一致 |
| 🟠 **HIGH** | **`chaos_is_gc_pointer` 是地址区间近似**，nursery 盲点假设未证实 | c606783cc 守卫正确性依赖"解释器对象地址恒 < g_heap_base/全堆下界" |
| 🟠 **HIGH** | **Server GC 默认 OFF + 全工程零 CI 验证** | 默认开启即生产风险 |
| 🟠 **HIGH** | **并发 GC 无 CI 门禁**：BGC hang 历史 + 全部 stress 测试靠 `--stress-only` 手动跑 | 已知缺陷不受 CI 保护 |
| 🟡 **MED** | **Verify 深度/入口不全** | kFull 仅覆盖 WKS 显式 collect；Server/分配触发 GC/ARM64 无 |
| 🟡 **MED** | **L1 卡表 realloc 与并发 DirtyCard 的指针稳定性** | swap 释放数组，无锁读者可能悬垂 |

**一句话**：GC 的**串行/单线程能力面已相当完整**，但**并发正确性（A2b + P0 interpreter 边界 + Server GC + 卡表并发）既未修复也未受 CI 约束**——这是本工程 GC 的**最大且最集中的风险**。

---

## 一、已落地且真实（动手前先肯定）

### 1.1 分代模型与对齐重构 — 全绿 [核实]
- **3 代 + LOH + POH**：Nursery(`REGION_NURSERY`) / Gen1 独立 `REGION_GEN1` / Old(`MarkSweepOldGen` 页式) + LOH + POH。证据：`gc_young_gen.h:72-93`、`gc_region.cpp:897-904`(region-gen tag)、`gc_region.cpp:525/608/613/670`。
- **R1-R6 全部登 main**（非仅 commit 列表，源码逐一在）：
  - R1 卡表扫描=已注册段 `gc_young_collector.cpp:468-493`
  - R2 压缩重定位覆盖 static-root+GCHandle `gc_old_gen.cpp:2291-2311`
  - R3 gen1 屏障语义统一 `gc_young_collector.cpp:583-616`
  - R4 段注册/释放成对 `gc_card_table.cpp:125-227 / 292-341`
  - R6 卡粒度 512→256B + bit-per-word `gc_card_table.h:53-67`
- **region-vs-segment 混合模型**：年轻代为真 region；old-gen 为页/segment 制（非 CoreCLR 统一 region）。此二元性直接牵涉 A2b 断链（见 §2）。

### 1.2 之前 P1 架构缺口已实质闭合 [核实]
- **FindPage O(log n)**：按地址升序 `page_array_` 二分 + TOCTOU 快照重试 `gc_old_gen.cpp:470-544`。
- **100%-free 页回收入 pool**（非"从未释放"）：FIX-1(freelist_release_count_ 信号 `gc_old_gen.h:607`)、FIX-2(PoolPageCount accessor + pool_test 3 测)、FIX-3(provisional 高压主动 trim `gc_old_gen.cpp:3387`)。物理内存返回 OS 是**延迟**的（pool 超限 + compact STW trim `gc_old_gen.cpp:3341-3415`），但正确性安全（锁 + STW trim）；剩余风险仅"内存平台期"，非正确性。

### 1.3 Verify 强化 + 服务项 [部分]
- **kFull 引用遍历**：`GcVerifyHeap` `gc_diagnostics.cpp:67-134`；**VerifyPromotedTracked 误框已修**（gen1 语义）`gc_diagnostics.cpp:145-160`，接 `GcYoungCollection` 出口 commit `54ef748aa`。
- **NGC2 queue** 已实现 `gc_scheduler.h:343-365`（provisional→mandated gen2）。
- GC 单测树大规模（§4 详述）。

---

## 二、🔴 CRITICAL — A2b 跨代引用 UAF（唯一未竟正确性 bug）

**状态：开放。根因 100% 锁死=store-then-barrier 竞态，修复全部 revert，立项草案已建（未实现）。**

### 2.1 根因（源码+复现测试双证）
```
mutator: *old_slot[i] = nursery_ptr  →  [窗口A]  →  chaos_gc_dirty_card(slot)
                                              ↑
                                        GC safepoint 落此
```
young-GC Phase-1 扫 dirty card → 该槽卡未 dirty → skip → 对象未晋升 → 回收 → 悬垂。
- **官方自述**：`gc_region_barrier_stress_test.cpp:119-126` 注释 = 权威表述（store 与 card 是两个非原子操作）。
- **诊断收敛**：`carded=1024 registered_seg=4104 scanned_dirty=8`（1016 卡写入但扫描不可见）；卡侧三类逻辑自洽修复产生字节级相同 dangling（32-117/1024, thread-7）→ 卡表自身数学自洽，根因**在卡表之外**=并发窗口。
- **失败史**：Option B(保守 old-mark) revert → P-1 naive 全槽重扫(1023/1024，污染) revert → γ' 原子屏障段(32→95-117 收窄未根治) revert。**三次规则已触发，归档 known-hard**。
- **Charter**：`charter-hardstw-storecard-atomicity-2026-08-13.md`（**立项草案**，未实现）。两条路线：① hard-STW（真 SuspendThread，Windows APC 现状 `PalCaptureReliable==false`，跨平台改造大）② store+card 原子化（helper 临界区 + safepoint 真等，或 Phase-1 重扫兜底——后者已证 naive 方式污染）。

### 2.2 具体危害
**旧代槽→young 对象的跨代引用在 Phase-1 不可见 → 该 young 对象被回收 → 下一次读写此槽 = UAF（内存损坏/随机崩溃）**。`GcScavengeObjectKnownNursery` 只跑在"被扫 dirty 卡上的槽"（`gc_young_collector.cpp:483-484`），无法兜底未 dirty 的槽。

### 2.3 为什么这是"三重静默"
1. **复现测试被提交为已知失败**（commit `7c8087332`），但因挂 `stress` label 被 **`-LE` 排除在一切 CI 门外** → 红灯不被看见。
2. young-GC 晋升路径本身尚有 gtest `YoungCollectionEmpty`/`ConservativeSweepSelfRefs` **baseline 失败**（`test_gc_young_collector`）——晋升不稳与漏卡混在一起，detector 无法区分。
3. kFull Verify **抓不到它**：悬垂槽无 dirty 卡，无对象被"错标"，Verify 只在 post-collection 旧代页上跑。

> 这是**全工程 GC 当前最大的单一风险**：一个未修复的并发 UAF + 无 CI 护网 + 复现测试被默认可失败地提交。

---

## 三、🔴 CRITICAL — P0 止血不完整（interpreter↔GC 边界）

近两天 P0 内存安全止血（`e6ea531e7` 撤 `g_static_fields` 反向 GC-root 注册；`c606783cc` 8 处 barrier 加 `chaos_is_gc_pointer` 守卫）修对了方向，但**留下三处未闭合**：

### 3.1 🔴 最高信号：`CodegenStSFld` 仍反向注册 root（与止血矛盾）
`jit_helpers.cpp:471-481` **未被 e6ea531e7 触及**，仍对同一 `g_static_fields[field_offset].obj` 调：
```cpp
JitSatbPreWriteBarrier(&g_static_fields[i].obj);
BgcRecordRootChange(&g_static_fields[i].obj, ...);   // 仍把非GC向量当GC root
```
而 `g_static_fields`（`interpreter_vm.cpp:37`，`CHAOS_IL2CPP_VECTOR<InterpreterValue>`）**从未** `GcRegisterStaticRootRange` 注册，`GcScanStaticRoots`(gc_static_roots.cpp:70-85) 不扫它。
→ **AOT/full-JIT 路径只要写这个解释器静态槽，就重新引入 e6ea531e7 刚修的那个 BGC re-mark 解释器堆指针的内存损坏/悬挂 hazard。**三路 StSFld 现在不一致：
- `Reg_StSFld`(ir_reg_alloc.cpp:1207) → 纯 store ✓
- `Handle_StSFld`/`interpreter_vm.cpp`(e6ea531e7) → 纯 store ✓
- `CodegenStSFld`(jit_helpers.cpp:471) → **仍 barrier+root-change ✗**

> 这是我核实过的最明确的遗留缺陷，也是 P0"止血"是否真的止住的关键断点。修复当与 e6ea531e7 对齐（若该向量不 root，就用纯 store；若需 root，需真正 `GcRegisterStaticRootRange`）。

### 3.2 🟠 `chaos_is_gc_pointer`（c606783cc 的守卫）是地址区间近似
`gc_api.h:105-116`：`addr >= g_heap_base || IsPohPointer`。注释自称"g_heap_base 是最小注册地址，覆盖 old-gen 和 nursery"。但**存在代码侧矛盾**：
- `thread_state.cpp:674-678` 明说 **nursery 指针可低于 g_heap_base**（nursery 经 RegionManager 单独分配，非总是 ≥ 旧代基址）。
- `chaos_is_gc_pointer` 快速路径**无 nursery 检查** → 若解释器/代码生成碰到的真 GC 对象恰在 below-base 的 nursery region，守卫判"非 GC"→ **跳过真 barrier → 漏 card → 跨代引用被回收 → UAF**。这是 P0 想防的失败模式的反向形态。
- 反向误判（≥ g_heap_base 的非 GC 指针发 SATB）`BgcSatbPreWriteBarrier`(gc_bgc_inline.h:42-102) **无成员校验**直接登记 → 同 class 的损坏 hazard。
- 直觉：依赖"解释器对象(MemoryDomain/TLS池)恒低于全堆下界"这一**未证实假设**。需补验证：MemoryDomain/TLS arena 地址与 g_heap_base/全堆下界的相对位置。

### 3.3 🟠 解释器对象模型边界未物化（Step 1.2 中止）
用户已决定**中止**解释器物化（memory 记录明确）。后果：解释器裸 `InterpreterObject*`/`ArrayStorage*`/`BoxedValueHeader` 仍直接进出 AOT，反物化断头（`RequireObject`/`RequireArray` `interpreter_vm.cpp:108-121` 无条件 static_cast，无判别）。Step-D `ExecutionFrame`(entry_direct.cpp:1175-1209) 未挂 frame chain，靠保守栈扫兜底——**这是已知遗留，非本轮回归**，但决定了解释器路径的 GC 安全性上限。

---

## 四、🟠 HIGH — 验证深度 / Server GC / 并发 CI 门禁

### 4.1 Server GC 默认 OFF + 零 CI 验证
- 已接线 `GcHeapManager::Initialize`(gc_heap_manager.cpp:19-48) + `GcCoordinator::ExecuteMultiHeapGc`(gc_coordinator.cpp:43-89)，但 `gc_features.h:38` `CHAOS_IL2CPP_GC_SERVER=0`、CMake `gc_server` 默认 OFF(`CMakeLists.txt:241`)。
- **全仓无任何 caller 把 `gc_server` 输入置 ON**（`ci-framework.yml` 有输入但无调用方传）。Server 路径**从未在任何 CI 运行**。默认开启即生产风险。
- kFull Verify 只在 **WKS 分支**入口/出口（`gc_region.cpp:1410-1458`），`GcCoordinator::ExecuteMultiHeapGc` 根本不调 `GcVerifyHeap` → Server 堆甚至无 verify。

### 4.2 并发 GC 无 CI 门禁
- 全部 14+ stress/soak 测试（gc_stress, satb_stress, bgc_stress, general_stress, soak …）一律 `-LE` 排除在默认门禁外，仅 `--stress-only` 手动跑。
- BGC hang 是**已记录的实物**：`gc_bgc.cpp:167-172`("the 'BGC hang'")、`gc-j1-bgc-sweep/STATUS.md`("曾并行后因 BGC hang 回退")、`chaos_gc_bgc_root_scan_test` KNOWN-FAIL 且 ~392s 极慢。BgcSweep 被迫串行化管理。
- **最该被 CI 保护的两类缺陷（A2b + P0/解释器边界）恰好全在 CI 门外。**

### 4.3 L1 卡表 realloc 与并发 DirtyCard 指针稳定性 [code-level 隐患]
`GcRegisterHeapRange` 向下 rebase 时 `g_card_l1.swap(new_table)`(gc_card_table.cpp:168/193) 释放旧数组；而 `DirtyCard`(gc_card_table.h:166) / `chaos_is_gc_pointer` 用 relaxed 加载**不带旧代 mutex_**读这个数组。并发 DirtyCard 跨线程读写 swap 释放的数组 = **L1 表悬垂 UAF**。`g_card_bundle` 因"预分配一次永不 realloc"规避了此 hazard(gc_card_table.cpp:37)，`g_card_l1` 缺同样保护。且 rebase 只平移 L1 条目，**node 冻结的 `seg_idx` 不重索引**(gc_card_table.cpp:221, 255 + 140-172)——正是 A2b 断链侧，也是独立并发隐患。

### 4.4 Verify 入口不全
kFull 仅：WKS 显式 `chaos_gc_collect` 入口/出口；**分配触发的 young GC**(TLAB 耗尽 `gc_region.cpp:250/260`) 不跑；**Server 不跑**；`CHAOS_GC_HeapVerify=2` 靠 env 手动开，无 workflow 设置。Bitmap 采样仅 4096 对象 + 首字/region-gen，**无对象图可达遍历**。

---

## 五、🟡 MED/LOW — 其他已知缺口（相对 CoreCLR，多数非本次回归）

- 写屏障/卡表：世代 `_dst_ref` 三层接线已做但缺压力验证（K2b 断言只验位置非存活=假阴性）；card bundle 指针稳定；增量/并发三色 GC 非缺口（CoreCLR 亦 SATB-based）。
- 句柄：~5/13 型（REFCOUNTED/WEAK_INTERIOR 已补，边缘型缺）；HandleTable 代剪枝 iterator 被收敛移除（回退）。
- 诊断：ETW ~8/40 事件（缺 BGC 阶段族）；`gc_bgc.cpp:321` Gen1 ETW payload 硬编码 0。
- 动态调优：24/~75 旋钮；dynamic_tuning/BGC servo 未闭环；Dynamic Heap Count 无。
- vxsort SIMD 排序、HardLimit 分代、no_gc region 完整、Frozen/RO segment 等边缘缺失。

> 这些在 8-12 评估已详列，非本次新发现。核心架构骨架 ~80%+，功能完整度 ~55-60%（8-12 结论仍成立）。

---

## 六、测试/CI 现状（诚实盘点）

- **单测入口 `--layer unit`**：纯 codegen/snapshot（`tests/unit/managed/`），**零 GC 正确性**——GC 只在 native 层。
- **Native GC CTest 树 `tests/unit/runtime-native/runtime-core/gc/`**：56 目标，单元级覆盖广（scheduler/handle/loh/poh/young/old/base/gen1/safepoint/parallel_mark/mark_bitmap/sanity/domain_unload/heap_limit/root_change…）。这是 GC 单测的主资产。
- **旧 contracts 树 `tests/contracts/native`**：`chaos_gc_*` 命名 + glob 自动发现。⚠️ **gc-ci.yml 的 `-R "chaos_gc_"` 只匹配此旧树，匹配不到新的 `test_gc_*` 56 目标** → 双树过滤不匹配，新树 GC 测试依赖 integration 层覆盖，易漏。
- **CI 矩阵缺口**：gc-ci 仅 Windows+Debug；ARM64 只有 JIT 无 GC；无 Release/shipping GC job；Server ON 无 caller。
- **当前门禁**：unit 层 OVERALL: OK（2249/2277，28 known-fail 全 codegen）；native/integration 层需真构建验证，baseline 已知含 `chaos_gc_bgc_root_scan_test`。

**结论**：GC **单测资产量大且真实**；GC **正确性/并发/stress/平台覆盖"作者了但 CI 不 enforce"**——最危险缺陷恰是 CI 门外的那些。

---

## 七、风险矩阵（按影响 × 概率 × 是否受护）

| # | 风险 | 影响 | 概率 | 被 CI 护? | 综合 | 建议动作 |
|---|------|------|------|-----------|------|---------|
| 1 | A2b 跨代 UAF | **内存损坏/随崩溃** | 中（复现 4/5） | ❌ | **CRITICAL** | 优先立项修复（hard-STW 或 store+card 原子化）；修复前把 `gc_region_barrier_stress_test` 提为 gate 内"已知失败但必跑"以建立可见性 |
| 2 | CodegenStSFld 反向 root | **内存损坏/悬挂** | 高（路径可达） | ⚠️(单测未必覆盖) | **CRITICAL** | 与 e6ea531e7 对齐：撤 barrier 或真正 `GcRegisterStaticRootRange`；补 triple-path StSFld 一致性测试 |
| 3 | chaos_is_gc_pointer nursery 盲点 | 漏卡→UAF / 误 SATB | 中（依赖未证实假设） | ❌ | **HIGH** | 验证 MemoryDomain/TLS arena 与全堆下界相对位置；改用 IsValidManagedObject/FindPage 精确判定；补断言护栏(IsOldGen\|IsNursery else abort) |
| 4 | Server GC 默认 OFF + 零 CI | 默认开启即生产故障 | 高(一旦开) | ❌ | **HIGH** | 补 Server ON 的 CI 矩阵 + 至少一个 Server 冒烟；修 Verify 不覆盖 Server 的缺口 |
| 5 | 并发 GC(BGC/stress) 无门禁 | 回归难现 | — | ❌ | **HIGH** | stress/soak 至少 nightly 自动化 + 报告门禁；处理 bgc_root_scan 慢测 |
| 6 | L1 卡表 swap + 并发读 | 卡表 UAF | 低(窗口小但存在) | ❌ | **MED** | swap 前栅栏/引用计数/读写锁；重索引冻结 seg_idx |
| 7 | Verify 深度/入口不全 | 簿记漂移不易现 | — | ⚠️ | **MED** | kFull 覆盖分配触发 GC + Server；CI 默认开 verify 档 |
| 8 | 双测试树过滤不匹配 | 新 GC 测试漏检 | 高 | ⚠️ | **MED** | gc-ci 正则扩到 `test_gc_` + `chaos_gc_`，统一用 native 层驱动 |
| 9 | 解释器边界未物化(Step1.2 中止) | 边界路径 GC 安全上限 | — | ⚠️ | **MED**(已知遗留) | 尊重用户中止决定；作为 future 专项，不回归 |
| 10 | 边缘能力(handle/ETW/dynamic/hardlimit) | 功能完整度 | — | — | **LOW** | 8-12 已列，backlog |

---

## 八、一句话给决策者

GC 的**能力落地（串行/单线程/结构）令人满意且真实**，核心骨架对齐 CoreCLR 约 80%+；但**并发正确性链（A2b UAF + P0 解释器边界不闭合 + Server GC + 卡表并发 + CI 门禁外置）是当前工程 GC 的最大集中风险**——它既是 max 影响的 bug，又是 min CI 保护的缺陷。**"能跑"与"并发下不错"之间的差距，是本报告的核心结论。**

---

## 九、可信度边界

1. 本报告为只读分析，未做全量重编译/重跑 gate；native/integration 层"当前是否全绿"依据 baseline 文件 + 参考构建 CTest 枚举推断，未真跑。
2. A2b dangling 计数（32-117/thread-7）来自复现测试/文档自述，非本会话复跑。
3. `chaos_is_gc_pointer` nursery 盲点是**代码级推断**（两处注释矛盾），需地址布局实测确证。
4. "2380s BGC hang" 数字在仓内定位不到字面出处，但 BGC hang 作为已记录实物成立。
5. 本文档覆盖 GC 域；解释器/CodeGen/JIT 的 GC 交互（§3/§4.3）属跨域交叉风险，摘入以示相互影响，详评见各自 assessment。
