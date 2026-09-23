# page-payload-uniform-tu — page 0 大载荷彻底均匀化

> task_id: `page-payload-uniform-tu`
> 类型：CodeGen 发射结构 + ABI 演进 + 构建容量
> 创建：2026-09-22
> phase: roadmap
> 上游任务：`page-payload-split`（已交付，C1002 已消除但 page 0 仍 31 MB）

## 关键文档

- `roadmap-v1-01.md` — **当前权威：阶段与子任务结构**
- `design-v1-01.md` — **当前权威设计**（brainstorm 已拍板，问题已清零）

## 调度状态

```
task_type: roadmap
phase: roadmap
roadmap_or_plan: roadmap-v1-01.md
dispatch_model: sequential
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: (无 — roadmap 全部子任务已交付)
最近摘要: **roadmap 收口**。pps3-1 / A / B1 / B2 / C 全部交付，
          **每个 TU ≤ 8 MB 的硬阈值达成**（超标 0 个，最大 7.96 MB）：
            page 0:      30.96 MB -> 7.96 MB
            page-0001:   22.53 MB -> 2.81 MB
            page-0071:   11.53 MB -> 7.28 MB
          system chunk 5509 subjects -> entry.exe 通过；
          reflection fact 失败集与基线逐 subject 相同（新增 0 / 消失 0）。
latest_stop_point: roadmap 完成
下一步: 无（如需继续：观察 CI 上 C1002 是否复现）
```

## 交付清单

| 子任务 | 手法 | 结果 | commit |
|---|---|---|---|
| **pps3-1** | vtable 数据经 `VTableDataCode` 拆为 payload 段 | page 0 30.96→14.12 MB | `774fead66` |
| **pps3-A** | 两个 page-0 巨型反射 dispatch 拆 `_partN` | page 0 14.08→7.96 MB | `11c42b90f` |
| **pps3-B1** | 两个 `_b3` 反射巨型函数拆 `_partN` | page-0001 22.53→8.91 MB | `479f322ca` |
| **pps3-B2** | `kRefl_*` 成员表按声明类型分组 | page-0001 8.91→2.81 MB | `65349bf45` |
| **pps3-C** | GC Slot Map **文本瘦身**（非拆段） | page-0071 11.53→7.28 MB | `a04fddd58` |

**方法论**：前四项都是「拆段 → 各占一 TU」；**pps3-C 不是** —— runtime 把 slot map
当单块连续内存线性扫描，拆段即改 ABI，故改为压缩生成文本（注释占原段 69%）。

## 守卫（均含负控核对）

- `PagePayloadSplitTests`：9 项（vtable 归属 / page-0 无巨型函数 / 反射分发守恒与
  case 原子性 / kRefl 同段约束 / slot map 连续性与 entry 尺寸自洽）
- `ReflectionDispatchSplitTests`：11 项（顺序保持 / 去重保留 / 守恒 / case 不可分）


> ⚠️ **测试环境注意**：本 checkout 为多 session 共享，其他 session 正在改
> `RuntimeHelperShapeRegistry.CoreStubs.Part3*.cs`。全量测试失败数会在 0↔59 之间
> 摆动（噪声）。隔离验证方式见 memory `shared-tree-test-churn-from-concurrent-sessions`。
> 我的 8 个文件在**隔离 worktree 下全量 2287/2287 通过**（含一次完整绿）。


## 设计摘要（本任务权威输入）

**边界拍板**：
- 方案 = **B + C2a + C3 都做**（彻底均匀）
- 验收 = **每个 TU ≤ 8 MB**（硬阈值）
- C3 路线 = **路线 1**（扩展现有结构，加 `struct_size` + chunks）
- P1/P2 冲突 = **用 O(1) chunk 定位避开性能损失**（不让位）

**根因结论**：
- `ObjectModel` 未登记为 payload section（VTable arrays 段占 page 0 的 95.4%）
- C1002 = 编译期内存峰值，非文件大小阈值
- **VTable 段 82.4% 是空槽位；最大单数组仅 0.4MB** → 无巨型单数组
- runtime `chaos_vtable_resolve` 用裸数组索引 → **稀疏编码会伤 P1**，不采用

**当前结论**：
- page 0 = 30.96 MB；GC Slot Map = 10.42 MB；Hotpatch = 6.68 MB
- 按 8MB 阈值：page 0 超标 3.9×，GC Slot Map 超标 1.3×，Hotpatch **未超标**

**下一步入口**：`roadmap-v1-01.md` → `pps3-1-objmodel`

**风险评估摘要**：R2（全局索引空间被破坏 → 静默错值）最高危，须负控；
R3（热路径退化）用 O(1) 定位缓解；R5（仍不达标）需先测分布。

**三优先级权衡结论**：Phase 1 三项全正向；Phase 2-4 触及 P3（hotpatch 核心结构），
用户已确认接受让位，并以 O(1) 定位保住 P1。

## 架构映射

- il_feature: n/a（发射结构，非 IL 翻译路径）
- translation_path: n/a
- planner: `NativeAotLoweringPlanner.Methods.cs`
- emitter: `NativeAotEmitter.Shared.cs`
- runtime_support: `hotpatch_table.*` / `bootstrap.cpp`（C3 触及）
- contracts: `contracts/native/v0/codegen_bridge.h`（C3 触及）
- constraints_followed: 段边界 = ABI 边界
- new_constraints: `HotpatchModuleV0` 引入 `struct_size` 兼容机制
- consistency_check: C3 需同步 contracts + runtime + codegen 三侧

## pps3-1 实现决策：模型字段拆分（`VTableDataCode`）

**问题**：vtable 数据从 `ObjectModelCode` 中拆出后，谁还能看到它？

- 页面发射（page 0）**必须看不到** —— 否则与 payload TU 重复定义（C2086）
- 其余消费者（`AsyncIteratorMemberSurfaceTests`、`BuildGeneratedModuleHeader/Source`、
  单 TU 路径）**必须看得到** —— 它们把对象模型当作完整模型读

**决策**：对象模型保持**完整**（vtable 数据追加在其尾部），page 0 按**长度**切割。

| 载体 | 处理 |
|---|---|
| `ObjectModelCode`（string，小模型 <200K） | 含 vtable；`VTableDataCode` 为空，切割为 no-op |
| `ObjectModelCodeBuilder`（StringBuilder，大模型） | 含 vtable；发射器按 `keep = Length - VTableDataCode.Length` 截断 |

**为什么按长度而非查找**：vtable 数据**永远是模型尾部**，无需在多 MB 缓冲里搜索；
且避免引入任何额外分配。

**为什么追加必须在 dedup 之前**：vtable 数组引用 `chaos_mt_*` 符号，
而 dedup 只认 `inline constexpr chaos_type_id_*` / `chaos_mt_*` 字面量。
若 vtable 数据在 dedup 之后才追加，其引用到的重复 `chaos_mt_*` 就逃过去重 → C2374。

**为什么三个站点放在一起**：`objectModelBuilder.Append(vtableDataCode)` 原被包在
`try/catch (OutOfMemoryException)` 中并附「这是 codegen 侧内存压力」的说明 ——
但 `Append` 本身可达 GB 级；该 try/catch 在最终版本中被移除（拼接沿用已有 builder，
失败会以裸 OOM 浮出，不再被误报为 MSVC 问题）。⚠️ 此处曾因重复 Edit 造成
三段副本并最终误删整段阈值逻辑，靠 `grep -n` 逐行核界才修复 —— 教训：**大段重构用
行号核界 + 断言，不要连续 Edit 同一区域**。

