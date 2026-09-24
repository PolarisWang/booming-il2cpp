# design — 统一 TU 装箱器（TuPacker）与拆分可观测性

> task_id: `page-payload-uniform-tu`
> 创建：2026-09-24
> 上游：pps3-1/A/B1/B2/C（分段）、C3-1/2/3/4（分块）—— 均已交付
> 性质：**Codegen 发射架构重构**，非缺陷修复

## 一、要解决的问题（实测支撑）

当前 TU 边界的决策有**两个互不知情的入口**：

| 入口 | 依据 | 常量 |
|---|---|---|
| 方法分页 `autoPageSize` | **方法数** | 500 |
| 载荷分段 `PayloadSectionPartitioner` | **字符数** | 350_000 |

而 TU 的消费方（MSVC）只认**内存峰值**（C1002 = 编译期 OOM）。三个维度不统一：

| # | 缺陷 | 实测证据 |
|---|---|---|
| **D1** | 预算是字符数，不是内存峰值 | hotpatch 估算公式偏低 **61%**（80 字符/条 vs 实际 128）|
| **D2** | 段的"能否切/在哪切"无表达 | `PayloadSection` 只有 `Name/Content/Order/ExportedSymbols`；切分知识散在 producer |
| **D3** | 两个决策点不协同 | 各算各的预算，无"这个 TU 最终多大"的全局视图 |

**另一实测缺口**：`350_000` 常量重复 **4 处**
（`ModuleRegistration.cs:25`、`PayloadSectionPartitioner.cs:98`、
`NativeAotEmitter.Shared.cs:60` 与 `:408`），靠注释「mirror」同步。

## 二、设计与执行原则

| 原则 | 落实 |
|---|---|
| **不靠猜** | 切分点由 producer 显式声明，不估算 |
| **不靠估** | 最终以编译实测兜底 |
| **不静默** | 每次拆分决策留痕（Split Report）|

## 三、架构：三阶段

```
输入（统一 PackableUnit）
  ├── method unit          （可任意切）
  ├── atomic section       （不可切，如 gcslotmap）
  └── splittable + units   （producer 已演算的切分点）
        ▼
   TuPacker（单一决策点）
        ▼
   TU 列表 + SplitReport（每次决策留痕）
        ▼
   编译反馈（实测峰值 → 超标则细分重来）
```

## 四、Split Report —— "可 debug" 的载体

### 4.1 产物

`native-aot.split-report.json` + `native-aot.split-report.log`（人可读）

### 4.2 结构

```jsonc
{
  "packerVersion": "TuPacker.V1",
  "tuBudget": { "targetChars": 350000, "maxChars": 400000 },
  "unitsInput": [
    { "kind": "section", "id": "modulereg", "semantics": "Atomic", "size": 12345 },
    { "kind": "section", "id": "vtable",   "semantics": "ByUnits", "unitCount": 127 },
    { "kind": "method",  "id": "0x06000001", "estCost": 1842 }
  ],
  "tuList": [
    {
      "id": "payload.0",
      "units": [ { "kind": "section", "name": "modulereg", "size": 12345 },
                 { "kind": "section", "name": "vtable.0",  "size": 180123 } ],
      "accumulated": 348912,
      "flushReason": "approachingTarget",
      "estCost": 349000
    }
  ],
  "warnings": [ { "type": "oversizedAtomic", "id": "gcslotmap", "size": 4547500 } ]
}
```

### 4.3 `flushReason` —— 每个 TU「为什么在这截止」

| 值 | 含义 |
|---|---|
| `approachingTarget` | 下一个单元会超预算 |
| `unitOverflow` | 单单元本身超预算（不可切）|
| `methodLimit` | 达每 TU 方法数上限 |
| `forcedByProducer` | producer 显式要求独立 TU |
| `abiBoundary` | ABI 要求不跨此边界 |
| `lastUnit` | 输入耗尽 |

### 4.4 可复现性

拆分结果完全由「输入单元列表 + 预算 + flush 规则」决定 →
**相同输入必得相同 TU 划分**。配套 `TuPackerMatchTests` 拦住任何装箱逻辑漂移。

### 4.5 debug 用法

```
Q: 这个 TU 为什么 7.8MB？
A: cat split-report.json → 看该 TU 的 units 与 flushReason

Q: 新增一个大 section 会超吗？
A: packer dry-run → 输出预测 TU 列表 + oversized 警告

Q: C1002 又来了，是哪个 TU？
A: 编译日志的文件名 → split-report 对应 TU 的成分 → 定位单元
```

## 五、执行路径（worktree 化）

每个 Step 一个 worktree，做 → 自测 → 合回 main → 删除。
与其他 session 的 worktree（`fix-red11-link` / `hu` / eh 系列）互不冲突。

| Step | worktree | 内容 | debug 验收（能看到什么）|
|---|---|---|---|
| **1** | `tp-debug` | 观测 + SplitReport v0（**不改行为**）| `split-report.json`：每 TU 里有哪些 section |
| 2 | `tp-semantics` | `PayloadSection.Semantics` + `Units` | `unitsInput` 有 semantics/unitCount |
| 3 | `tp-packer` | `TuPacker` 合并 autoPage + Partition | 每 TU 有 `accumulated` + `flushReason` |
| 4 | `tp-cost` | `EstimatedCost` 替换纯字符 | 每单元/每 TU 有 `estCost` |
| 5 | `tp-build-feedback` | 编译实测峰值 + 超标细分 | `tu-build-metrics.json`：实测 vs 预估 |
| 6 | `tp-finalize` | 删旧路径 + 文档 | 与 Step3 基线一致 |

## 六、Step 1 范围（本次执行）

**目标**：让"拆分长什么样"先可见，**零行为改动**。

| 项 | 内容 |
|---|---|
| 改动 | `BuildGeneratedSources` 收集现有 sections → TU 映射，输出 `split-report.json` |
| 不做 | 不改分页逻辑、不改预算、不改任何生成文本 |
| 验收 | 构建后在 codegen 产物目录能读到 report，内容与实际的 TU 文件一一对应 |

**为什么先做这个**：后续每一步都要靠它验证"改动确实生效了"。
没有它，Step2-5 只能靠"编译通过"判断，而这正是历史上多次误判的根源
（见 memory `worktree-cannot-run-foundation-pipeline`）。

## 七、风险

| # | 风险 | 对策 |
|---|---|---|
| R1 | Report 与真实产物不一致（"报告说 A，产物是 B"）| 验收断言：report 的 TU 列表 == 实际生成的 TU 文件名集合 |
| R2 | Report 体积过大（138 TU × 每 TU 数百单元）| 分级：默认只记 TU 级摘要；`CHAOS_SPLIT_REPORT=verbose` 才记单元级 |
| R3 | 与并发 session 冲突 | worktree 隔离；已核对 3 个在制 worktree 均不涉及 `NativeAotEmitter` |

## 八、问题清零

```
blocking_questions: []
question_clearance: cleared
```
