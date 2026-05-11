---
name: dev-foundation-dll-verify-analysis
description: 对 foundation-dll verify-family pipeline 产物进行 AI 综合分析，按 11 维度标准化模板输出验收报告，供人工审核决
---

# Foundation DLL Family Verification Analysis

## 概述

Pipeline 产出 raw 数据后，由本技能执行 AI 综合分析。分析结果按固定模板输出到 `verification/reports/`。

**职责边界：** 本技能只负责分析，不修改 pipeline、contract、codegen 等任何文件。

**入口信号：** `dev-foundation-dll-verify-analysis`

## 调用方式（强制）

**必须使用 Skill 工具调用，不得跳过 Skill 入口直接分析：**

```
Skill("dev-foundation-dll-verify-analysis", "分析 <family-slug>")
```

Skill 加载后强制实施：
- 标准化的 11 维度模板约束（不得遗漏或合并维度）
- 前置 Handwrite/AutoGen Pre-check 流程
- 统一的评分规则和报告结构

不通过 Skill 入口直接分析 = 输出格式不受约束，不保证完整性。

## 使用方式

```bash
# 直接对 AI 说：请用 dev-foundation-dll-verify-analysis 技能分析 <family-slug>
#
# 技能会自动执行：
#   1. 先跑 verify-family pipeline（确保数据最新）
#   2. Handwrite/AutoGen Pre-check
#   3. 收集所有产物分析
#   4. 输出分析报告到 verification/reports/
```

## 分析流程

### 第零步：执行 Pipeline（强制）

在开始任何分析前，必须先执行 verify-family pipeline，确保数据是最新的。

```bash
# 标准模式
run foundation-dll verify-family <family-slug>

# 严格模式（含 hotupdate + post-HU benchmark）
run foundation-dll verify-family <family-slug> --mode strict
```

**规则：**
- **禁止直接使用已有数据跳过此步骤** — 无论之前 pipeline 何时跑过，本技能必须重新执行一次
- 如果 pipeline 执行失败（overall_status = "failed"），分析应基于失败的 pipeline 数据继续，但需在报告中标注 pipeline 本身失败的事实
- 如果 `--mode strict` 指定但 pipeline strict 模式失败，降级为 standard 模式的数据进行分析并标注

### 第一步：收集数据

Pipeline 执行完毕后，读取以下产物：

| # | 数据源 | 路径模式 | 用途 |
|----|--------|---------|------|
| 1 | unified-verification-report.json | `verification/foundation-dll/<Assembly>/<family>/` | stages, coverage, duration |
| 2 | benchmark-comparison-report.json | 同上 | methodResults, summary |
| 3 | mechanism-audit-report.json | 同上 | false_passing, stubs, skips |
| 4 | principle-alignment-report.json | 同上 | 7 项原则检查结果 |
| 5 | hotupdate-verification-report.json | 同上 | hotupdate 通过情况 |
| 6 | post-hotupdate-benchmark-report.json | 同上 | interpreter 路径性能 |
| 7 | capability-family-contract.json | family 根目录 | obligations, customIndices, methodSubjectIds |
| 8 | review/*.md | family/review/ | codegen review 文档 |
| 9 | handwritten/*.Custom.cs | family/handwritten/ | 手写 entry 代码 |
| 10 | managed_test/tests/*.cs | family/managed_test/tests/ | managed 单元测试 |
| 11 | family-verification.json | `verification/projections/foundation-dll-audit/` | dashboard projection |
| 12 | dashboard 详情页 HTML | `projections/.../dlls/<Assembly>/families/<family>-(fact\|benchmark\|hotupdate).html` | dashboard 展示数据 |
| 13 | native-aot.generated.cpp | `il2cpp_dist/<Class>/generated/` 或 `il2cpp_dist/genuine/<Class>/generated/` | 生成 C++ 代码体积 |
| 14 | console.log | `artifacts/logs/run/<session>/` | pipeline 运行日志 |

**注意：**
- native-aot.generated.cpp 可能位于 `il2cpp_dist/<ClassName>/generated/` 或 `il2cpp_dist/genuine/<ClassName>/generated/`，具体取决于 codegen 输出结构，需要使用 glob 定位
- benchmark-comparison-report.json 由 pipeline 的 benchmark 阶段生成，如该阶段被跳过则不存在

### 第二步：Handwrite/AutoGen Pre-check（前置审查）

在进入 11 维度分析前，必须执行此前置审查。**如果此步骤发现 blocking 问题，应停止分析并建议修复。**

#### H0.1 检查 customEntryIndices

读取 contract 的 `customEntryIndices` 字段，逐项检查：

| 条件 | 判定 | 后续动作 |
|------|------|---------|
| 该方法是 throwing 方法（在 `_always_throws` 集合中） | ✅ 合理，无需手写 | Auto-generate 的 external runtime helper 已正确生成 throw。建议从 customEntryIndices 中移除或添加注释说明"throwing, auto-generated"。**注意：此情况不影响评分，但应记录在报告的建议中** |
| 需要真实逻辑且 **handwrite 代码存在** | ✅ 正常 | 进入 H0.2 检查代码质量 |
| 需要真实逻辑但 **handwrite 代码不存在** | ❌ blocking | **建议先调用** `dev-foundation-dll-ai-test-generation` 生成手写代码，完成后重跑 pipeline，再回到本分析流程 |

#### H0.2 Handwrite 代码质量检查

如果 handwrite 代码存在，检查：
- 签名是否与 auto-generated 桩一致（`public static void CustomEntryMethodN()`）
- 是否使用 `_exitCode = 1` on failure + `try/catch` for expected exceptions
- 代码风格（是否包含硬编码路径、magic number、无注释的复杂逻辑）

#### H0.3 Auto-Generate 效率检查

对于不在 `customEntryIndices` 中的方法（auto-generate），检查是否有方法**本应标记为 custom**：
- auto-generate 生成的 throwing stub 是否正确（对于 throwing 方法）
- 是否有方法因 auto-generate 的代码质量过低而需要降级为 custom + handwrite
- 生成代码是否有明显的冗余或错误模式

#### H0.4 Pre-check 结论

输出 pre-check 结论，作为报告的一部分：

```
- Handwrite Pre-check: [PASS / BLOCKING / WARN]
- customEntryIndices: [N] indices
  - ✓ throwing, 无需手写: [N] 个 (indices: [...])
  - ✓ handwrite 存在且质量合格: [N] 个 (indices: [...])
  - ✗ handwrite 缺失: [N] 个 (indices: [...]) ← 如有，这是 blocking
  - ⚠ autogen 效率问题: [N] 个 (简述)
```

### 第三步：逐维度分析

前置审查通过后，按下方模板的 11 个维度逐项分析。每项必须包含：
- **定量计算**（数值、阈值对比）
- **定性判断**（根因分析、合理性评估）
- **评分**（0-5 分，见评分规则）

### 第四步：综合评定

汇总各维度评分，给出 final score、关键风险列表、改进建议和验收建议。

## 评分规则

每项检查三个等级，对应 0-5 分：

| 等级 | 分值 | 含义 |
|------|------|------|
| pass | 5 | 完全符合预期 |
| warn | 3 | 有问题但可接受/已知限制 |
| fail | 0 | 需要修复 |

维度得分 = 该维度所有项得分的平均值（保留 1 位小数）。  
综合评分 = 所有维度得分的平均值（保留 1 位小数）。

**验收建议规则：**

| 条件 | 建议 |
|------|------|
| 所有维度得分 >= 4，且无 blocking 问题 | **approved** |
| 有维度得分 < 4，但无结构性失败（fact rate < 0.9 / false_passing > 0 / contract 无效） | **review-needed** |
| 存在结构性失败 | **blocked** |

---

## 输出报告模板

以下是每次分析**必须严格遵循**的输出模板。AI 按此模板填空，不得改变 section 结构、不得遗漏任何维度。

```markdown
# Family Verification Analysis Report

## 元数据

| 字段 | 值 |
|------|----|
| Family | `<family-slug>` |
| Assembly | `<assembly-name>` |
| Pipeline 模式 | standard / strict |
| Pipeline 耗时 | `<duration_ms>ms` |
| 分析时间 | `<YYYY-MM-DD HH:MM>` |
| 分析版本 | v1 |

---

## ★ 综合评定（结论优先）

### 快速总览

3-5 句话概括本次 family 验证的核心结论。包含关键数据（fact 通过率、性能指标）、最重要发现、以及总体验收判断。

### 验收建议

**建议：approved / review-needed / blocked**

理由：
1. ...
2. ...
3. ...

### 风险列表（按优先级排序）

| Priority | Severity | 风险 | 维度 | 说明 |
|----------|----------|------|------|------|
| P0 | high | ... | ... | ... |
| P1 | medium | ... | ... | ... |
| P2 | low | ... | ... | ... |

### 综合评分

- **综合评分：** x.x/5
- **各维度得分速览：**

```
A.数据正确性  ████████░░  x.x
B.性能分析    ████████░░  x.x
C.架构质量    ████████░░  x.x
D.热更新适配  ████████░░  x.x
...
```

### 人工审核要点

- 重点关注：...
- 需确认：...
- 可接受的风险：...

---

## Handwrite/AutoGen Pre-check 结论

### H0. Pre-check Summary

```
- Handwrite Pre-check: [PASS / BLOCKING / WARN]
- customEntryIndices: [N] indices
  - ✓ throwing, 无需手写: [N] 个
  - ✓ handwrite 存在且合格: [N] 个
  - ✗ handwrite 缺失: [N] 个
  - ⚠ autogen 效率问题: [N] 个
```

### H0.1 customEntryIndices 逐项分析

| Index | 方法 | 类型 | 状态 | 说明 |
|-------|------|------|------|------|
| 0 | ... | throwing | ✓ 无需手写 | ... |
| 1 | ... | complex | ✗ 缺失 | ... |

### H0.2 Handwrite 代码质量

- **代码存在性：** [存在 / 不存在]
- **签名对齐：** [OK / FAIL]
- **错误处理：** [完整 / 部分缺失 / 无]
- **总体评价：** [良好 / 需改进 / 不合格]

### H0.3 Auto-Generate 效率

- auto-generate 代码整体质量评估
- 发现的问题 / 优化建议

---

## 维度详细分析

以下是各维度的详细检查数据，作为综合评定的支撑证据。

### A. Pipeline 数据正确性

#### A1. Fact 通过率
- **数据：** N/M（x.xx%）
- **阈值：** >= 0.9 → pass
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 通过/失败方法明细
  - 失败方法根因分析（codegen bug / known limitation / pre-existing）
  - 总体可信度判断

#### A2. Benchmark 数据合理性
- **数据：** matchedCount=N, nativeFaster=N, managedFaster=N, avg_speedup=x.xx%
- **阈值：** matchedCount > 0 → pass
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - matched/jit_elided/invalid 分布是否合理（invalid = throwing 方法 = 正确）
  - managedFaster 是否 > 0，如是则分析根因：
    - BenchmarkMethod 方案已消除 hotpatch dispatch 开销，managed-faster 不应出现
    - 如仍存在 managed-faster，需判断是否为微基准噪声或真实翻译质量问题
  - avg_speedup 是否在合理范围
  - 对比数据可信度（native 端使用 BenchmarkMethod 直接调用 vs managed 端 JIT 编译）

#### HotUpdate 通过率
- **数据：** N/M passed
- **阈值：** == 1.0 → pass（详见热更新适配维度分析）
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 引用热更新适配维度的分析结果
  - 仅做数据正确性确认，不重复展开

#### Post-HU Slowdown
- **数据：** avg_slowdown=x.xx%（x.x`x）
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 引用热更新适配维度中 Interpreter 路径的分析结果
  - 仅做数据正确性确认，不重复展开

#### A5. Coverage 指标
- **数据：** methodCoverage=x.xx%, testedRate=x.xx%, overall=x.xx%
- **阈值：** > 0 为 pass
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 覆盖率与 fact 通过率的匹配性

#### A 维度总结
`**得分：x.x/5 — pass/warn/fail**`
文字总结。

---

### B. 性能分析

#### B1. avg_speedup 趋势
- **数据：** avg_speedup=x.xx%
- **阈值：** > -50% → pass
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - native vs managed 整体性能趋势
  - 是否符合预期（对于简单 cast 操作）

#### B2. 异常 method 分析
- **数据：** 列出 speedup 极端值方法
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 每个极端值的根因（JIT elision / throwing path / 数据类型特征）
  - 是否是 pipeline 的数据问题

#### B3. native 优势分布
- **数据：** 最快/最慢方法列表
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 性能分布与 type 特征的关联
  - native 是否在所有非异常方法上都 >= managed

#### B4. JIT Elision 影响
- **数据：** jit_elided 方法列表
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 哪些方法被 JIT 优化掉
  - 缺失的数据点是否影响 benchmark 结论有效性

#### B 维度总结
`**得分：x.x/5 — pass/warn/fail**`
文字总结。

---

### C. 架构质量

#### C1. Principle Alignment
- **数据：** overall=ALIGNED/CONCERN/VIOLATION，各子项详情
- **阈值：** != VIOLATION → pass
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 各原则子项逐项评述
  - VIOLATION 或 CONCERN 的原因分析
  - 是否可以 waived

#### C2. Mechanism Audit
- **数据：** false_passing=N, stubs_found=N, skips_found=N
- **阈值：** false_passing == 0 → pass
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 审计结果的完整性
  - 任何 false_passing 的根因

#### C3. Lowering 质量
- **数据：** methods_without_lowering=N
- **阈值：** == 0 → pass
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 是否需要 codegen 修复

#### C4. 生成代码结构
- **数据：** native-aot.generated.cpp 行数/字节数
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 代码是否有明显冗余或不合理模式
  - 与同类 family 的对比

#### C 维度总结
`**得分：x.x/5 — pass/warn/fail**`
文字总结。

---

### D. 热更新适配

#### HotUpdate 通过率
- **数据：** N/M passed
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 失败方法分析（如有）
  - 是 Pipeline 数据正确性维度此检查项的数据来源

#### D2. Patch Data 完整性
- **数据：** .patchdata 文件存在，runtime-patchdata.cpp 存在
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - patch 数据是否完整
  - kPatchDataSize/kPatchData 是否正确

#### D3. Interpreter 路径
- **数据：** post-HU benchmark 可用性
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 路径是否正确路由到 interpreter
  - 结果是否可信

#### D4. 热更新体系适配度
- **数据：** dispatch 是否走 HotpatchLookupBySlot
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - dispatch table 结构
  - interrupt_ptr 支持
  - 整体适配度评估

#### D 维度总结
`**得分：x.x/5 — pass/warn/fail**`
文字总结。

---

### E. 内存/体积

#### entry.exe 体积
- **数据：** N bytes
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 与同类 family 对比

#### 生成 C++ 体积
- **数据：** N 行 / N bytes
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 代码密度和合理性

#### E 维度总结
`**得分：x.x/5 — pass/warn/fail**`
文字总结。

---

### F. 正确性

#### F1. Fact 失败根因分析
- **数据：** 失败的 methodSubjectIds 列表
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 逐一分析每个失败方法
  - 判断是 codegen bug / known limitation / 环境问题
  - 修复优先级

#### F2. Contract 一致性
- **数据：** methodSubjectIds 数量 vs entrypoint 实际
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 签名一致性
  - customEntryIndices 正确性

#### F3. Custom Entry 正确性
- **数据：** customEntryIndices 对应的实现
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - handwrite 实现是否正确
  - 是否可改为 auto-generate

#### F 维度总结
`**得分：x.x/5 — pass/warn/fail**`
文字总结。

---

### G. 完整性

#### G1. Review Bundle
- **数据：** review/*.md 数量及内容摘要
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 是否存在
  - 内容质量评估
  - codegen-review 是否已执行

#### G2. Managed Test
- **数据：** managed_test/tests/*.cs 存在性
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 是否存在
  - 覆盖度评估

#### G3. Benchmark 产物
- **数据：** benchmark 原始数据存在性
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 数据完整性

#### G4. Codegen Review
- **数据：** codegen-review-v*.md 存在
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - review 是否已完成
  - review 结论

#### G 维度总结
`**得分：x.x/5 — pass/warn/fail**`
文字总结。

---

### H. 效率

#### H1. Pipeline 总耗时
- **数据：** N ms（约 N 秒）
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 是否在合理范围

#### H2. 各阶段耗时分布
- **数据：** 各阶段 duration_ms
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 瓶颈阶段识别
  - 耗时是否正常

#### H3. 运行稳定性
- **数据：** 是否有 retry / 错误日志
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 系统稳定性判断

#### H 维度总结
`**得分：x.x/5 — pass/warn/fail**`
文字总结。

---

### I. 维护性

#### I1. Contract 信息完整度
- **数据：** contract.json 各字段
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - displayName
  - obligation 设置
  - reviewBundleRequired 等

#### I2. 文档覆盖
- **数据：** README.md 及其他文档
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 是否充分

#### I 维度总结
`**得分：x.x/5 — pass/warn/fail**`
文字总结。

---

### J. Dashboard 输出验证

#### Projection 一致性
- **数据：** projection 中该 family 条目的 verificationGates 和 closureStatus
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - closureStatus 与最新 pipeline 结果是否匹配
  - verificationGates 是否反映了各阶段实际状态

#### Dashboard 详情页数据正确性
- **数据：** fact/benchmark/hotupdate 三个 HTML 详情页内容
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 各详情页显示的数值与对应 pipeline report（unified-report / benchmark-comparison-report / hotupdate-report）是否一致
  - method 列表是否完整
  - 一次性检查所有三个详情页

#### 证据链接完整性
- **数据：** HTML 中的 evidence 引用
- **评分：** 0 / 3 / 5
- **AI 分析：**
  - 链接文件是否存在
  - 引用是否有效
  - 是否存在跨 family 污染（evidence 列表中混入其他 family 的路径）

#### J 维度总结
`**得分：x.x/5 — pass/warn/fail**`
文字总结。

---

## 改进建议（按优先级排序）

| Priority | 建议 | 对应维度 | 预期效果 |
|----------|------|---------|---------|
| P0 | ... | ... | ... |
| P1 | ... | ... | ... |
| P2 | ... | ... | ... |
```

---

## 模板使用约束

1. **报告结构必须按以下顺序：** 元数据 → ★ 综合评定（结论优先）→ Handwrite/AutoGen Pre-check → 维度详细分析（A-J）→ 改进建议
2. **必须保留所有子项**的评分 + AI 分析
3. **不得删除或合并维度**
4. 如果某子项数据不可用，标注 `数据不可用` 并给 warn 分（3），不得跳过
5. 如果某维度对所有子项均无法获取数据，整维度给 0 分并说明原因
6. **综合评定必须放在维度详细分析之前**，确保读者第一时间看到结论和风险

## 输出位置

| 文件 | 路径 |
|------|------|
| 完整分析报告 | `verification/reports/<family>-verification-analysis-<YYYYMMDD>.md` |
| 摘要索引 | `verification/reports/analysis-index.md`（每次追加一行） |

analysis-index.md 格式：
```markdown
| <YYYY-MM-DD> | <family> | x.x/5 | approved/review-needed/blocked | 一句话摘要 |
```

## 与 Pipeline 技能的关系

- **执行管线：** 本技能在分析前**自动执行** `dev-foundation-dll-verification-pipeline` 的 `verify-family`，确保数据最新。不依赖用户预先运行 pipeline。
- **联动技能：** `dev-foundation-dll-ai-test-generation` — 当 Handwrite Pre-check 发现手写代码缺失时，建议先调用此技能生成手写代码，然后重跑 pipeline、再回到本分析流程
- **下游：** 人工审核分析报告 → 决定 closureStatus

Pipeline 完成后执行：
```bash
run foundation-dll verify-family <family>         # Step 1: pipeline
# Step 2: AI 分析（通过 Skill 工具调用）
```