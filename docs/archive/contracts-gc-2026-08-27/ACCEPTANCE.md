# CRAG GC — 可追审验收清单 & .NET 语义兼容矩阵（方案 C 脚手架）

> 目的：把"self-claim"升级为"可独立验收、可追审"，补齐文档审计短板（评估发现：
> 仅单篇自评 wiki、无 STATUS/sign-off、无独立验收产物）。
> 状态：**脚手架** —— 清单条目需在执行环境可用后由真实测试逐一勾选验证；
> `null`/`未验` 一律不得宣称"已通过"（no-skip / 不注水原则）。

## 1. 验收维度（每条对应可执行验收器 + 判定口径 + 通过基线）

| ID | 维度 | 验收器（建议目标） | 判定口径 | 当前状态 |
|----|------|-------------------|---------|---------|
| GC-A1 | 分代正确性 | `test_gc_baseline_*`（A2） | Young→Gen1→Old 晋升年龄符合 kPromotionAgeThreshold=2 | 未验 |
| GC-A2 | 跨代引用不漏扫 | 现有 `gc_holder` 双卡表 + 语义 oracle | old→young store 后不触发 GC 亦不 UAF | 未验 |
| GC-A3 | 并发期内引用变化（SATB） | `test_gc_bgc_*` 真交错（B2 已恢复） | 并发标记期间 mutator 覆盖根不丢对象 | 未验 |
| GC-A4 | 弱引用/复活语义 | `test_gc_semantics_weakref`（C） | 目标仅弱引用可达 → 回收后 null；有复活链 → 正确处理 | 未验 |
| GC-A5 | DependentHandle/Ephemeron | `test_gc_semantics_eph`（C） | 键弱值强，键被回收则值可回收 | 未验 |
| GC-A6 | GCHandle pin 稳定性 | `test_gc_semantics_pin`（C）＋现有 gc_handle | pin 后地址稳定、不随 compact 移动 | 未验 |
| GC-A7 | 域卸载回收 | 现有 domain_unload + `test_gc_domain_*` | 卸载后域内对象全部回收、无跨域泄漏 | 未验 |
| GC-A8 | OOM 兜底 | `chaos_gc_*_oom`（D 可观测） | HandleOomCondition 走 full GC→重试→emergency reserve 链 | 未验 |
| GC-A9 | 低内存响应 | `test_gc_lowmem`（D） | 低内存通知触发 rate-limited GC | 未验 |
| GC-A10 | 堆硬/软上限 | `test_gc_heaplimit`（D） | 超上限分配失败→OOM 链；soft 触发 full GC | 未验 |

## 2. .NET 语义兼容矩阵（对拍参照，逐项判定"设计差异 or 缺陷"）

| 语义 | .NET/.NET GC 参照行为 | CRAG 当前行为 | 判定 |
|------|---------------------|--------------|------|
| WeakReference 目标回收 | 目标仅弱可达时于某次 GC 被收集并置 null | （待录制） | 待判定 |
| Ephemeron 键值关联 | 键不可达且值仅键引用→整体回收 | （待录制） | 待判定 |
| GCHandle pin | pinned 对象地址在 compact 中保持 | （待录制） | 待判定 |
| Finalizer 顺序 | 终结队列按注册顺序；复活后可再次终结机制 | （待录制） | 待判定 |
| GC.GetGCMemoryInfo | 托管 GCMemoryInfo 类型传递完整快照 | 缺失托管类型；native 多字段 always 0 | **缺陷（文档已自认）** |

> 录制/判定需在可执行环境完成（`--preset native-gc` + 语义 oracle）。禁止未验宣称兼容。

## 3. 落地（工具恢复后）
- C 语义 gtest：`tests/unit/runtime-native/runtime-core/gc/semantics/` 下 `test_gc_semantics_weakref/eph/pin`，gtest，`LABELS "unit;gc"`。
- 可执行验收器：`tests/contracts/native/gc/`（现空占位）下 `gc_acceptance_*`（`WITHOUT_GTEST` 自带 main）。
- 兼容判定落 `docs/gc-semantic-compat.md`；每条验收器勾选后更新本清单状态 ="✅ 已验(日期/hash)"。
