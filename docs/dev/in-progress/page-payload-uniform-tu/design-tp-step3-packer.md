# design — TP-Step3 TuPacker 统一装箱器

> task_id: `page-payload-uniform-tu`
> 创建：2026-09-24
> 上游：TP-Step1（观测）、TP-Step2（声明式协议）—— 均已合入 origin/main
> 性质：**重构**（合并两个决策点），非新功能

## 一、Step 3 的真实范围（先纠正认知）

原本设想是"发明一个统一装箱器"。实读代码后发现**两个决策点各自已经是一个贪心装箱器**，
只是载体不同：

| 决策点 | 载体 | 预算 | 单元 |
|---|---|---|---|
| **方法分页**（`BuildGeneratedSources` 内联循环）| 显式循环 | `sizeThresholdChars` + `page0Overhead` | 方法（含 `perMethodOverhead`）|
| **载荷分段**（`PayloadSectionPartitioner.Partition`）| 独立静态类 | `budgetChars` | section / piece |

两者**算法同构**（累积 + 超预算即 flush + 永不空 TU），但：

1. **常量重复 4 处**（`PayloadSectioningBudgetChars` / `sizeThresholdChars` /
   `DefaultBudgetChars` / `HotpatchChunkBudgetChars`），靠注释「mirror」
2. **page0Overhead 与 perPageOverhead 只存在于方法侧** —— 载荷侧不知道 page 0
   还要背 object model 的开销
3. **无统一视图**：没人回答"这个 TU 最终多大"

**所以 Step 3 = 把这两段同构逻辑抽成一个装箱器，并让预算成为单一份。**

## 二、设计

### 2.1 抽象

```csharp
public sealed record PackableUnit(
    string Id,            // 报告用：可反查到具体段/方法
    long SizeChars,
    string Kind);         // "method" | "section"

public static class TuPacker
{
    public static IReadOnlyList<PackedTu> Pack(
        IReadOnlyList<PackableUnit> units,
        TuBudget budget);
}

public sealed record TuBudget(long TargetChars, long FirstTuOverheadChars);
public sealed record PackedTu(IReadOnlyList<PackableUnit> Units, long AccumulatedChars, string FlushReason);
```

### 2.2 关键不变量（与现状等价）

| # | 不变量 | 理由 |
|---|---|---|
| I1 | 累积 ≤ `TargetChars`，否则 flush | 与两处现逻辑一致 |
| I2 | **绝不产出空 TU**（单单元超预算时独占一个）| 现状的 `current.Count > 0` / `methodIdx > pageStart` guard |
| I3 | 单元顺序 = 输入顺序 | 方法与段的顺序都有语义 |
| I4 | 第一个 TU 带 `FirstTuOverheadChars` | 正是 page0Overhead 的语义 |
| I5 | 确定性 | 同输入同输出（Step1 已有此测试）|

### 2.3 范围边界（**本步不做**）

- **不改**方法分页的**结果**：Step 3 是重构，TU 划分应与重构前**逐字节一致**
- **不改**预算数值（仍 350_000）
- **不接入** ByUnits 的 producer 迁移（vtable/hotpatch 仍是既有切法）——
  那属于 Step 3 之后的清理，因为迁移会**改变 TU 划分**，需独立验证

### 2.4 验收方式（本步的核心）

**新旧对拍**：同一模型下，重构后的 TU 列表必须与重构前**逐 TU、逐方法/段**一致。

这是重构的唯一可信验收。为此：

```
1. 重构前：跑 system chunk，记录每个 TU 的文件名 + 方法数 + 字符数  → 基线
2. 重构后：重跑，比对
3. 差异 = 0 才算通过
```

## 三、风险

| # | 风险 | 对策 |
|---|---|---|
| R1 | 重构悄悄改变 TU 划分 | **对拍验收**（§2.4）；不一致即回退 |
| R2 | 两处 overhead 语义被抹平 | 显式建模 `FirstTuOverheadChars`，不合并 |
| R3 | 方法侧的 `methodIdx > pageStart` guard 丢失 | 不变量 I2 显式测试（单方法超预算）|
| R4 | 并发 session 干扰 | worktree 隔离 + A/B 对照（本会话已验证此手法）|

## 四、执行

worktree `tp-packer`（本步新建，与 Step1/2 的 `tp-debug` 分离 —— 那棵已合入 main，
新步从最新 origin/main 起）。

## 五、问题清零

```
blocking_questions: []
question_clearance: cleared
```
