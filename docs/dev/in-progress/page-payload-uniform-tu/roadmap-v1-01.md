# roadmap v1-01 — page 0 大载荷彻底均匀化

> task_id: `page-payload-uniform-tu`
> 创建：2026-09-22
> 上游设计：`design-v1-01.md`（brainstorm 已拍板，问题已清零）
> 上游任务：`page-payload-split`（已交付，C1002 已消除但 page 0 仍 31 MB）

## 一、目标

让 **每个生成的 TU ≤ 8 MB**，消除 page 0 的 31 MB 单单元，并使受 ABI 约束的
大段也能按块拆分。

**当前**：page 0 = **30.96 MB / 972,873 行**（`ObjectModel` 的 VTable arrays 段占 95.4%，
未参与分段）。

> ⚠️ **深度调查修正（见 `design-v1-01.md` §1.4）**：VTable 段 927,748 行中
> **82.4% 是空槽位**，最大单个 vtable 数组仅 **11,770 行（0.4 MB）** ——
> **不存在巨型单数组**。故 **B（分段）+ C2a 后所有 TU 即已 ≤ 8MB**。
> C3 因此**无当前收益**，保留理由为**架构储备**（用户决定）。

## 二、为什么需要 roadmap（而非单一 plan）

1. **跨三层**：codegen（C#）→ contracts（ABI）→ runtime（C++），且 C3 为破坏性 ABI 变更
2. **三阶段性质不同**：B/C2a 是无 ABI 成本的补齐；C3 是 ABI 演进，必须独立验证
3. **强串行依赖**：B/C2a 先降体积 → 再评估 C3 是否仍有必要 → C3 需 contracts 先行
4. **单轮构建 ~20 分钟**，验证循环长

→ `dispatch_model: sequential`

## 三、范围边界

**做**：
- `ObjectModel` 纳入 payload 分段机制（B）
- GC Slot Map 消除类型声明文本冗余（C2a）
- `HotpatchModuleV0` / `CodeRegistrationV0` 扩展 `struct_size` + chunks（C3）
- runtime 消费端支持分块寻址，**O(1) 定位**
- 三 chunk 端到端验证

**非目标**：
- 不改符号命名
- 不改 `GcSlotMapSectionEntryHdrV0` 的字节布局
- 不做无关重构

## 四、阶段列表

### Phase 1：无 ABI 成本的体积削减（可独立验证）

- **goal**：page 0 与 GC Slot Map 降到 8 MB 以内，不触碰任何 ABI
- **exit_criteria**：
  - `ObjectModel` 被 `AddSection` 登记，page 0 ≤ 8 MB
  - GC Slot Map 类型声明去重，≤ 8 MB
  - 无跨块引用断裂（内容守恒断言）
  - system chunk 构建通过 + 单测全绿
- **deliverables**：`NativeAotLoweringPlanner.Methods.cs`、`GcSlotMap.cs`
- **dependencies**：无
- **resolved_decisions**：
  - ObjectModel 的 **127 个 vtable 数组是天然切分边界**（最大仅 11,770 行 / 0.4 MB）
  - **不采用稀疏编码**：runtime `chaos_vtable_resolve` 用裸数组索引，改它伤 P1
- **watch_items**：
  - R4：切分需保持数组内的全局 slot 语义
  - R5：某块自身可能仍超 8 MB（实测最大 0.4MB，风险低）

### Phase 2：ABI 扩展（contracts 先行）

- **goal**：`HotpatchModuleV0` / `CodeRegistrationV0` 具备分块能力与版本兼容
- **exit_criteria**：
  - contracts 新增 `struct_size` + chunks 字段
  - 生成侧初始化改为**指定初始化器**（防位置式错位）
  - contracts 单测/编译通过
- **deliverables**：`contracts/native/v0/codegen_bridge.h`
- **dependencies**：Phase 1
- **resolved_decisions**：
  - 路线 1（扩展现有结构），`struct_size` 置于**首位**
- **watch_items**：
  - R1：构造点唯一（模板），但需全仓核对

### Phase 3：runtime 消费端适配

- **goal**：runtime 支持分块寻址，**不引入渐进复杂度**
- **exit_criteria**：
  - 5 个文件 / 34 处访问改为 chunk-aware
  - **O(1) chunk 定位**（chunk_base[] 前缀和）
  - hotpatch 名字查找 / dispatch 行为不变
- **deliverables**：`hotpatch_table.cpp/h`、`chaos/hotpatch_dispatch.h`、`method_table.cpp`、`bootstrap.cpp`
- **dependencies**：Phase 2
- **resolved_decisions**：
  - 用户拍板：用 O(1) 定位**避开** P1 损失（不让位）
- **watch_items**：
  - R3：热路径退化
  - R2：全局索引空间（最危险，静默错值）

### Phase 4：codegen 分块发射与端到端验证

- **goal**：codegen 发射分块形式，三 chunk 验证通过
- **exit_criteria**：
  - 生成的分块与 runtime 消费端契约一致
  - **每 TU ≤ 8 MB**（硬阈值）
  - system / reflection / threading 构建 + fact 通过
  - 负控成立（重索引必须变红）
- **deliverables**：`ModuleRegistration.cs`、模板、STATUS
- **dependencies**：Phase 3
- **resolved_decisions**：验收口径见 `design-v1-01.md` §七
- **watch_items**：
  - R6：并发会话共用 artifacts 根 → 跑前核对时间戳

## 五、子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---|---|---|---|---|---|---|---|---|---|---|---|
| `pps3-1-objmodel` | 1 | ready | main | ObjectModel 纳入分段 | — | b1 | page 0 ≤ 8MB + 内容守恒 | `NativeAotLoweringPlanner.Methods.cs` | page 0 ≤ 8MB 且构建通过 | `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.Methods*.cs` | L |
| `pps3-2-gcsm` | 1 | planned | main | GC Slot Map 去类型重复 | pps3-1 | b1 | ≤8MB，字节布局不变 | `GcSlotMap.cs` | ≤8MB 且 runtime 扫描行为不变 | `.../NativeAotLoweringPlanner.GcSlotMap.cs` | M |
| `pps3-3-contracts` | 2 | planned | main | ABI 加 struct_size + chunks | pps3-2 | b2 | contracts 编译通过 | `contracts/native/v0/codegen_bridge.h` | 新字段就位，旧字段偏移不变 | `contracts/native/v0/**` | M |
| `pps3-4-runtime` | 3 | planned | main | runtime 分块寻址 O(1) | pps3-3 | b3 | 34 处访问改写 | `hotpatch_table.*`、`hotpatch_dispatch.h`、`method_table.cpp`、`bootstrap.cpp` | 行为不变 + O(1) 定位 | `src/native/runtime-core/hotpatch_*`、`src/native/bootstrap/**` | L |
| `pps3-5-codegen` | 4 | planned | main | 分块发射 + 模板改造 | pps3-4 | b4 | 分块产物与契约一致 | `ModuleRegistration.cs`、`Templates/NativeAot.HotpatchTable.cpp.scriban` | 产物 ≤8MB/单元 | `.../Emission/NativeAotLoweringPlanner.ModuleRegistration*.cs`、`.../Templates/NativeAot.HotpatchTable*` | L |
| `pps3-6-verify` | 4 | planned | main | 三 chunk 端到端验证 | pps3-5 | b4 | 构建 + fact + 负控 | `docs/dev/in-progress/page-payload-uniform-tu/**` | 三 chunk 通过、无劣化 | 同上 | M |

## 六、每阶段完成定义

见 §四各阶段 `exit_criteria`。

## 七、依赖

```
pps3-1-objmodel ──> pps3-2-gcsm ──> pps3-3-contracts ──> pps3-4-runtime ──> pps3-5-codegen ──> pps3-6-verify
    (无ABI)            (无ABI)          (ABI定义)          (runtime适配)       (发射)          (验证)
```

## 八、风险

| # | 风险 | 触发 | 后果 | 缓解 |
|---|---|---|---|---|
| **R1** | `struct_size` 首位破坏构造点 | 位置式初始化 | 编译失败/静默错值 | 改模板为指定初始化器；全仓核对（已确认构造点唯一）|
| **R2** | 全局索引空间被破坏 | 分块时重索引 | **静默解析到错误方法** | 保留全局索引空间 + **负控** |
| **R3** | hotpatch 热路径退化 | 分块引入间接层 | P1 回退 | **O(1) 定位**（用户拍板）|
| **R4** | ObjectModel 拆函数破坏引用 | 同函数内拆分 | 编译失败/行为变化 | 先测依赖图；内容守恒断言 |
| **R5** | 8 MB 仍不达标 | 某块自身过大 | 验收失败 | 分块前先测分布 |
| **R6** | 并发会话共用 artifacts | 两会话跑同 chunk | 假数字 | 跑前核对产物时间戳 |

## 九、备选路径

| 方案 | 何时启用 |
|---|---|
| 只做 Phase 1（B+C2a） | 若 Phase 1 后**所有 TU 已 ≤ 8MB**，Phase 2-4 的 C3 收益归零 → 重新评估是否仍要做 |
| 调阈值 | 若某块受 ABI 约束确实无法达标，显式记录并调整阈值 |

> ⚠️ **Phase 1 结束时必须重新评估 C3 的必要性**：若 B+C2a 已让全部 TU ≤ 8MB，
> 则 C3 是在为「零超标」付出 ABI 变更代价。用户已表态倾向架构一致性，
> 但届时需以**实测数据**再确认一次。

## 十、当前建议推进顺序

`pps3-1-objmodel` → `pps3-2-gcsm` → 重新评估 → `pps3-3-contracts` → `pps3-4-runtime` → `pps3-5-codegen` → `pps3-6-verify`

## 十一、子任务执行策略

```yaml
task_type: roadmap
phase: roadmap
dispatch_model: sequential
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: pps3-1-objmodel
```

## 十二、三优先级权衡

| 优先级 | Phase 1 (B+C2a) | Phase 2-4 (C3) |
|---|---|---|
| **P1 性能最优** | ✅ 运行期零影响 | ⚠️ 有风险 → O(1) 定位抵消 |
| **P2 架构完美** | ✅ 补齐遗漏 + 去冗余 | ✅ 段边界=ABI边界，全段可切 |
| **P3 热更适配** | ✅ 不碰 | ❌ 改 hotpatch 核心结构 |

**P1/P2 与 P3 冲突裁决**（用户已确认）：接受 P3 让位，用手法把 P1 损失降到零。
