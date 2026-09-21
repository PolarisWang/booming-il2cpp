# JSON / XML 工业化 — Roadmap v2.01

```yaml
task_id: json-xml-industrialization
dispatch_model: hybrid          # 单终端 + Workflow 内部 fan-out
worktree_policy: 隔离高风险实验（非并行开发）
merge_granularity: 阶段边界
上游基线: docs/dev/review/json-xml-industrialization-gap-2026-09-20.md（本日实测）
前身: docs/dev/in-progress/json-xml-production-readiness/roadmap-v1-01.md
```

## 1. 为什么需要 v2

v1（9/13）的 Phase 划分**结构是对的**，但有两个必须修正的前提：

1. **估算是盲估** —— v1 写「Phase 2 = 4wk / Phase 3 = 10wk」，但当时
   `api-coverage-matrix.json` **100% unclassified**，没有可信分母。
   本日实测拿到真实基线（`realVerified=14.4%`、`stubGap=51.2%`），可以重估。
2. **调度是 sequential** —— 用户要求 workflow + worktree。
   但**单终端下二者的语义与多终端不同**（见 §5），必须显式重定义，否则会
   重现本仓库已记录的并发事故。

## 2. 范围边界

**覆盖**：`System.Text.Json`(486) / `System.Private.Xml`(849) / `System.Xml.ReaderWriter`(442)
三 assembly 的 public method 面。

**不覆盖**：`System.Text.Encodings.Web`、`System.Private.Xml.Linq`、
XML 序列化引擎（沿用 v1 边界）。

## 3. 非目标

- ❌ 不追求 100% real 覆盖（`not-supported` / `not-applicable` 是合法终态）
- ❌ 不重构 `ExternalRuntimeFallback` 架构
- ❌ 不为凑覆盖率加无断言 smoke
- ❌ **不并行跑同一个 chunk**（见 §5.2 红线）

## 4. 阶段列表

### Phase 0 — 度量基建（🔴 起点，无依赖）

- **Goal**：建立**可信的进度标尺**。当前矩阵 100% unclassified，
  没有它所有进度估算都是盲猜。
- **Exit Criteria**：
  - `api-coverage-matrix.json` 的 `unclassified` 占比 **< 10%**
  - 分母 == Σ 各 `dll-method-manifest.json`（可复算）
  - 每项有档位：`real` / `stub` / `not-supported` / `not-applicable` / `fake-green`
- **Deliverables**：分类器 + 矩阵 v2 + 覆盖率报告
- **估算**：**1-2 会话**
- **Resolved Decisions**：
  - 分母权威 = `_dll/dll-method-manifest.json`（沿用 v1）
  - 分类方式 = **从实际 fact 数据反查**（每个 chunk 的 `fact-results.json`
    含 `methodSubjectId` + `resultKind`），join 到 manifest。**这是本日实测
    验证可行的路径** —— 比扩 `ApiSurfaceScanner` 更直接。

### Phase 1 — 假绿清零

- **Goal**：消除「未实现却计 passed」的假象，让数字可信。
- **Exit Criteria**：
  - 报告中 `passed` 字段不再包含 `stubGap`（或明确标注）
  - 三个口径陷阱（本日报告 §1.2）全部修复或文档化
- **Deliverables**：口径修复 + 文档
- **估算**：**1 会话**
- **⚠️ Watch**：修复后真实覆盖率会**先下降**（诚实化的正常代价），**不得回退**。

### Phase 2 — P0 核心 API 补齐（收益最大）

- **Goal**：`Utf8JsonReader` / `JsonDocument` / `JsonElement` / `Utf8JsonWriter` /
  `XmlReader` 全族 / `XmlDocument` 核心 —— 决定「能不能用」。
- **Exit Criteria**：核心类型族 `realVerified` 占比 **> 60%**
- **Deliverables**：分族实现 + 单测
- **估算**：**4-6 会话**
- **Dependencies**：Phase 0（需要标尺验证进度）
- **⚠️ Watch**：M1/M3 涉及 SIMD/span，可能与既有 vector operand 缺陷交互

### Phase 3 — stubGap 收敛

- **Goal**：756 个 stubGap 逐项收敛（实现 or 显式 not-supported）
- **Exit Criteria**：`stubGap` 占比 **< 10%**
- **Deliverables**：分项修复
- **估算**：**6-8 会话**（部分随 Phase 2 消解）
- **Dependencies**：Phase 2

### Phase 4 — failed 对账 + 未覆盖 chunk

- **Goal**：105 个 `failed` 逐项定位；补齐 3 个从未跑过的 chunk
- **Exit Criteria**：每个 failed 有归因；所有配置 chunk 至少跑过一次
- **Deliverables**：归因报告 + 新增 chunk 基线
- **估算**：**3-4 会话**
- **⚠️ 本阶段不可跳过** —— `failed` 里可能藏真缺陷（本日已修两个）

### Phase 5 — 收口

- **Goal**：性能基线 + 语义对齐 + 归档
- **Exit Criteria**：v1 review report 全部风险项关闭
- **估算**：**2-3 会话**

### 总计：**17–24 会话**（含返工余量）

---

## 5. 调度设计（本 roadmap 最关键的部分）

### 5.1 单终端下 workflow + worktree 的真实语义

用户选择了「1 个终端 + Workflow」。必须澄清：

| 机制 | 多终端下 | **单终端下（本计划）** |
|:-----|:---------|:----------------------|
| **Workflow** | 各终端跑各自 workflow | ✅ **agent 级并行**（同一工作树内） |
| **worktree** | 各终端在各自 worktree 并行开发 | ⚠️ **仅用于隔离高风险实验**，非并行开发 |

**Workflow 的 agent 共享工作树** —— 所以：

### 5.2 🔴 并行红线（源自本仓库已记录的事故）

**可以并行**（只读 / 无共享写入）：

- 多 agent 并行**调查**不同的 chunk / API 族
- 多 agent 并行**分析** fact 数据、生成报告
- 多 agent 并行**设计**方案（不改文件）

**必须串行**（有共享写入）：

- ❌ **两个 agent 同时跑同一个 chunk 的 pipeline**
  → 共享 `artifacts/foundation-dll/<asm>/chunks/<chunk>/`，产物互相覆盖，
    fact 静默消费陈旧 exe → **假数字**（真实事故，曾据此误报不存在的阻断）
- ❌ 两个 agent 同时改同一批源文件

**判据（任一命中即作废本次数字）**：

1. 数字在修复提交后仍逐项不变
2. `entry.exe` mtime 早于同 chunk 的 codegen mtime
3. 编译错误自行消失

### 5.3 worktree 策略

**用途**：隔离**高风险单点实验**（如改 GC 核心、改 codegen 发射点），
避免污染主检出。

**不用途**：并行开发（单终端下无意义）。

**使用约束**（源自 `worktree-build-isolation-broken`，已修但有前提）：

```bash
# 必须用根工程配置，且数据根指向主检出
cmake -S . -B <worktree>/artifacts/presets/windows-x64-reference
CHAOS_FOUNDATION_DLL=<主检出>/testing/foundation-dll
```

### 5.4 每阶段的执行模板

```
Phase N 开始
  │
  ├─ 1. 主检出建阶段分支（如 json-xml/p2-core-api）
  ├─ 2. 用 Workflow 并行做「调查/设计」（只读）
  ├─ 3. 串行执行实现（单 agent，避免文件冲突）
  │     └─ 高风险改动 → 开 worktree 隔离验证
  ├─ 4. 阶段自验（跑相关 chunk，串行）
  └─ 5. 阶段边界合回 main + 删 worktree → 下一阶段
```

---

## 6. 子任务映射

| batch | phase | 子任务 | 模式 | 估算 |
|:-----:|:-----:|:-------|:-----|:----:|
| B0 | 0 | P0-01 manifest 聚合分类器 | Workflow 并行调查 + 串行实现 | 1-2 会话 |
| B0 | 0 | P0-02 覆盖率报告 v2 | 串行 | |
| B1 | 1 | P1-01 口径修复（passed/stubGap/gateTotal） | 串行 | 1 会话 |
| B2 | 2 | P2-01 Utf8JsonReader + Utf8JsonWriter | Workflow 并行调查 → 串行实现 | 4-6 会话 |
| B2 | 2 | P2-02 JsonDocument + JsonElement | 同上 | |
| B2 | 2 | P2-03 XmlReader 全族 | 同上 | |
| B2 | 2 | P2-04 XmlDocument 核心 | 同上 | |
| B3 | 3 | P3-01..04 stubGap 分组收敛 | 按族拆，串行 | 6-8 会话 |
| B4 | 4 | P4-01 failed 对账 | **Workflow 并行调查**（理想场景） | 3-4 会话 |
| B4 | 4 | P4-02 未跑 chunk 补齐 | 串行（避免 artifacts 冲突） | |
| B5 | 5 | P5-01 性能 + 语义 + 归档 | 串行 | 2-3 会话 |

**Workflow 的最佳落点**：Phase 0（并行扫 chunk 数据）、Phase 4（并行对账 failed）。
这两处是**纯调查**，无共享写入，天然适合 fan-out。

**必须串行**：Phase 2/3 的实现（改共享源文件）、任何 pipeline 运行。

---

## 7. 与 v1 roadmap 的关系

**v1 保留不删**，作为历史结构参考。v2 的差异：

| | v1 | v2 |
|:--|:---|:---|
| 基线 | 9/13（矩阵 100% unclassified） | 9/20 实测（realVerified 14.4%） |
| dispatch | sequential | hybrid（单终端 + Workflow） |
| Phase 1 | 假绿分类器接入 | 假绿清零 + **三个口径陷阱修复** |
| 新增 | — | **Phase 0 度量基建作为起点** |
| 估算 | 盲估（4wk + 10wk） | 17-24 会话（基于实测） |

**v1 的 B0/B1 子任务（P0-C-02/03、P1-01/02）合并进 v2 的 Phase 0/1。**

---

## 8. 风险与诚实标注

| 风险 | 影响 | 缓解 |
|:-----|:-----|:-----|
| **artifacts 并发污染** | 假数字 | §5.2 红线 + 三条判据 |
| **估算误差** | 计划失真 | Phase 0 完成后用真标尺重估 |
| **stubGap 里藏真缺陷** | 低估工作量 | Phase 4 强制对账 |
| **单终端吞吐** | 比多终端慢 | 接受；用 Workflow 补并行调查 |
| `global-ns` 跑失败（未归因） | 1 个 chunk 无数据 | Phase 4 处理 |

**诚实标注**：

- 本 roadmap 的估算是**基于 5 个 chunk 的实测**外推到 8 个 chunk + 3 个 assembly，
  **不是一个精确的度量**
- Phase 0 完成后应**重新评估**后续阶段估算
- 未纳入 `System.Xml.ReaderWriter`（442 methods）的实测数据 —— 可能显著改变总量

---

## 9. 下一步入口

**Phase 0 需要用户确认后启动**（用户已选「先建度量」）。

启动方式：`dev-executing-plans` + Workflow（并行扫 chunk 数据）
