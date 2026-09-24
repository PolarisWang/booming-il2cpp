# pps3 收尾 roadmap — C3 与 threading 验证

> task_id: `page-payload-uniform-tu-phase2-4`
> 创建：2026-09-23
> 上游：`page-payload-uniform-tu`（B + C2a 已交付并入 origin/main @ `f43a69fd1`）
> 性质：**跨 contracts / runtime / codegen 三层的破坏性 ABI 演进**

## 一、事实基线（本轮实测，非推断）

| 项 | 实测值 |
|---|---|
| page 0 / page-0001 / page-0071 | 7.96 / 2.81 / 7.28 MB |
| **超标 TU 数（>8MB）** | **0** |
| 本轮构建验证过的 chunk | **system**（9-23 13:29）、**reflection**（9-23，fact 两次）|
| **threading** | entry.exe 时间戳 **9-22 16:11** —— **本轮从未构建** |
| `HotpatchModuleV0` | `struct_size` = **无**，`chunks` = **无** |
| `CodeRegistrationV0` | `struct_size` = **有**（原本就有），`chunks` = **无** |
| runtime chunk-aware 访问 | `hotpatch_table.cpp` / `method_table.cpp` **0 处** |

## 二、🔴 必须先拍板的决策：C3 是否还做

`design-v1-01.md` 第 143/146 行写明了这个岔路：

> 若 Phase 1 后**所有 TU 已 ≤ 8MB**，Phase 2-4 的 C3 收益归零 → 重新评估是否仍要做
> 用户已表态倾向架构一致性

**实测结果正是岔路条件成立**（超标 0 个，最大 7.96 MB）。因此：

| 选项 | 理由 | 代价 |
|---|---|---|
| **A. 不做 C3，显式关闭** | 硬阈值已达成；C3 是**唯一触碰 ABI** 的改动，而它解决的"段过大"问题**当前不存在** | 未来 chunk 变大时需重做 |
| **B. 做 C3（架构储备）** | 用户先前拍板"B + C2a + C3 都做"；为未来更大 chunk 储备 | 破坏性 ABI 变更，跨三层，风险最高 |

**本 roadmap 假定选 B**（按 design-v1-01 的原始拍板）。若选 A，则本 roadmap 退化为
「只做 §三 的 threading 验证 + 记录 C3 关闭理由」。

## 三、Phase T：threading chunk 验证（独立、低成本、先做）

**为什么先做**：它与 C3 无依赖，且能立刻暴露 pps3 分段在**另一个 chunk** 上的问题。
system/reflection 都验过了，threading 是唯一没验的 —— 而它恰好是历史上出过
C3861 的平台相关 chunk（见 memory `system-chunk-verification-loop-discipline`）。

| 子任务 | 内容 | 判据 |
|---|---|---|
| **T1** | `threading` chunk 构建（含 JIT） | 5509 类规模 → entry.exe 通过 |
| **T2** | `threading` fact 与基线比对 | 失败集逐 subject 相同 |
| **T3** | 若失败 → 归因（是新分段引入，还是预存在） | 与 stale 基线 diff |

**注**：若 T 阶段发现分段引入的缺陷，**优先修复**再谈 C3。

## 四、Phase C3：ABI 演进（三层强串行）

依 `roadmap-v1-01.md` 的 Phase 2/3/4 结构，**顺序不可颠倒**（contracts 先行）：

### C3-1：contracts（`contracts/native/v0/codegen_bridge.h`）

| 项 | 内容 |
|---|---|
| `HotpatchModuleV0` | 首位加 `uint32_t struct_size`；尾部加 `chunks` 描述 |
| `CodeRegistrationV0` | 尾部加 `chunks` 描述（`struct_size` 已有）|
| 兼容机制 | `struct_size` 置于**首位**（拍板），老 runtime 读新结构时按 size 判断能力 |
| 初始化形式 | 生成侧改**指定初始化器**（防位置式错位）|
| 判据 | contracts 单测/编译通过；**全仓核对构造点唯一**（R1）|

### C3-2：runtime 消费端（依赖 C3-1）

| 项 | 内容 |
|---|---|
| 改动点 | `hotpatch_table.cpp/h`、`chaos/hotpatch_dispatch.h`、`method_table.cpp`、`bootstrap.cpp`（实测 5 文件）|
| 访问方式 | 改为 chunk-aware；**O(1) chunk 定位**（`chunk_base[]` 前缀和）—— 用户拍板，不让位 P1 |
| 判据 | 热路径无渐进复杂度；名字查找 / dispatch 行为**逐位不变**（R2 最危险：静默错值）|

### C3-3：codegen 分块发射（依赖 C3-2）

| 项 | 内容 |
|---|---|
| 改动点 | `ModuleRegistration.cs`、模板、STATUS |
| 判据 | 生成分块与 runtime 契约一致；**每 TU ≤ 8MB 不回归** |

### C3-4：端到端 + 负控

| 项 | 判据 |
|---|---|
| 三 chunk | system / reflection / **threading** 构建 + fact 通过 |
| **负控** | 故意打乱 chunk 索引 → **必须变红**（R2：静默错值是最危险的失败模式）|
| ABI 兼容 | 老 runtime + 新 codegen 的行为需明确（拒绝 / 降级），不得静默错读 |

## 五、风险登记

| # | 风险 | 等级 | 对策 |
|---|---|---|---|
| **R2** | 全局索引空间被破坏 → **静默错值** | 🔴 最高危 | 负控；O(1) 定位的 `chunk_base[]` 前缀和须有守卫 |
| **R3** | 热路径退化 | 中 | O(1) 定位；基准对比 |
| **R1** | 构造点不唯一 | 中 | 全仓核对 |
| **R6** | 并发 session 共用 artifacts 根 | 中 | 跑前核对时间戳（memory 已有）|
| — | C3 收益为零（当前无超标段） | — | §二 决策项 |

## 六、执行方式（按用户指定）

```
每个阶段：
  git worktree add .claude/worktrees/<name> -b <name> origin/main
    → worktree 内开发 + 自测
    → 合回 main（fast-forward 或 PR）
    → 删除 worktree
```

**T 阶段先行**（独立且能立刻暴露问题）；C3 阶段串行，contracts → runtime → codegen。

## 七、问题清零

```
blocking_questions:
  - C3 是否仍做？（§二，需用户拍板）
question_clearance: pending_user_decision
```
