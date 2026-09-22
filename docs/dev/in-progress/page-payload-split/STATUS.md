# page-payload-split — page 0 大载荷段拆独立 TU（根治 C1002）

> task_id: `page-payload-split`
> 类型：CodeGen 发射架构 + 构建容量
> 创建：2026-09-21
> phase: roadmap
> 上游根因：system chunk `C1002: compiler is out of heap space in pass 2`

## 关键文档

- `roadmap-v1-01.md` — **当前权威：阶段与子任务结构**
- `design-a-payload-split.md` — 设计（brainstorm 已拍板）
- `c1002-heap-exhaustion-rootcause.md` — 根因（实测）
- `a-direction-input.md` — 调研输入（段构成 / 链接性 / 跨 TU 证据）

## 调度状态

```
task_type: roadmap
phase: roadmap
roadmap_or_plan: roadmap-v1-01.md
dispatch_model: sequential
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: pps-1-tests
最近摘要: brainstorm 完成，A 方向设计拍板（拆大载荷段 + 段内再分页；
          链接性用 extern 非 extern "C" —— 实测三情形指令序列字节级相同）。
          roadmap 已拆 4 个子任务，串行推进。
latest_stop_point: roadmap 产出，未开始实施
下一步: 启动 pps-1-tests（TDD 先写失败测试）
```

## 一、问题陈述

分页器**只切 `templateModel.Methods`**，page 0 额外挂载的 object model /
module registration / GC slot map **整体不分页**：

- page 0 = `native-aot.generated.cpp` = **71.2 MB / 1,236,192 行**
- 设计阈值 `sizeThresholdChars = 350_000`
- 超设计目标约 **200 倍** → MSVC `C1002` 堆耗尽

**与并发无关**：空闲机器单跑同样失败。

## 二、段构成（字节级实测）

| 段 | MB | % |
|---|---|---|
| Virtual method table arrays | 22.2 | 31.2% |
| Module registration | 22.0 | 31.0% |
| GC Slot Map Section | 8.7 | 12.2% |
| Hotpatch name index + dispatch table | 5.0 | 7.0% |
| 其余 16 段 | ~13 | ~19% |

## 三、边界拍板（用户已确认）

| # | 裁决 |
|---|---|
| 方向 | **A** —— 拆大载荷段 + 段内再分页（彻底） |
| 段边界 | 按段**自然边界**切 |
| 粒度 | **每段一 TU**，段内再分页 |
| 链接性 | **`extern`（C++ 链接性）**，非 `extern "C"` |

## 四、关键证据

| 结论 | 证据 |
|---|---|
| 链接性**不影响性能** | `static` / `extern` / 跨 TU 三情形反汇编**字节级相同**（`mov rax,[...]` + `ret`） |
| **不改 ABI** | runtime 的 `kMethodTable` 是另一符号 `g_method_table`；全仓无按名引用生成侧 `kMethodTable[]` |
| 跨 TU 机制**已存在** | 共享头已声明 `chaos_mt_*`(2439) / `chaos_vtable_*`(96) / `chaos_type_id_*`(2438)，page 2..N 已在用 |
| 构建系统**自动接纳** | `TestProject.CMakeLists.txt.scriban:85` glob `subjects/*.cpp` |

## 五、主要工作量

**不是挪代码，是改 234 个顶层 `static` 表的链接性** + 共享头补声明。
`static` = 内部链接，不能跨 TU —— 不解决则拆出去必 C2065/LNK2019。

## 六、单元测试规划（用户重点关注）

**可测性事实**（已实测）：
- `BuildGeneratedSources` 是 `private static` → 不直接测
- `NativeAotEmitter.GenerateFromArtifacts` 是 **`public`** → **主 seam**
- `NativeAotResult.GeneratedSources` 公开 → 可直接断言 TU 列表

**三层**：
- **L1 段分类器**：切段正确 + **内容守恒**（各段拼接 == 原文，防丢内容）
- **L2 分页预算**：每 TU 不超预算 / 段各自成 TU / 命名符合 CMake glob / page 0 瘦身
- **L3 链接性守卫**：无跨 TU `static` 引用 + 共享头声明完备

> ⚠️ **L3 最易写成假绿**（全局计数/存在性断言会被邻近合法文本满足，
> 本项目已有前科）。必须**锚定具体符号对** + **负控**。

**负控硬性要求**：每个 L2/L3 断言提交前必须证明**在旧结构下会红**
（回退 → 记录红几条 → 恢复 → PASS → 双双写进 commit message）。

## 七、风险评估摘要

| # | 风险 | 缓解 |
|---|---|---|
| R1 | `static` 漏改 → C2065/LNK2019 | Phase 1 独立验证 + L3 守卫 |
| R2 | 段间顺序依赖 → 行为变化 | 实测为纯数据；fact 全量回归 |
| R3 | 拆分丢内容 | **L1 内容守恒断言** |
| R4 | snapshot 大范围变更 | UPDATE 后**再跑 clean run** 才算验证 |
| R5 | TPG 内嵌陈旧 Generator.dll | 改后必重建 TPG |
| R6 | 性能回退 | **已实测排除** |
| R7 | 编译时间上升 | 观测项 |

## 八、三优先级权衡结论

| 优先级 | 结论 |
|---|---|
| **P1 性能最优** | ✅ 强正向（链接性零性能影响，实测） |
| **P2 架构完美** | ✅ 强正向（消除 page 0 无界缺陷） |
| **P3 热更适配** | ⚪ 中性（不改符号名/ABI） |

**无冲突。**

## 九、架构映射

- il_feature: n/a（发射结构，非 IL 翻译路径）
- translation_path: n/a
- planner: `Emission/NativeAotLoweringPlanner.Methods.cs`（段产出）
- emitter: `NativeAotEmitter.Shared.cs::BuildGeneratedSources` / `BuildSharedHeader`
- runtime_support: n/a（**runtime 零改动**）
- constraints_followed: 跨 DLL 类型身份约束（复用既有 extern 声明机制）；不改 ABI
- new_constraints: **被拆段内的表不得为 `static`**（跨 TU 必须 external linkage）
- consistency_check: ok（复用既有跨 TU 机制，非新发明）
- snapshot_tests: pending
- wiki_entry: needs-update —— 建议补 `wiki/.../01-翻译管线/` 分页设计说明
