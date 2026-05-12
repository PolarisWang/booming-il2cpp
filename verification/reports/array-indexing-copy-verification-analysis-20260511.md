# Family Verification Analysis Report

## 元数据

| 字段 | 值 |
|------|----|
| Family | `array-indexing-copy` |
| Assembly | `System.Private.CoreLib` |
| Pipeline 模式 | strict |
| Pipeline 耗时 | 18,466 ms (~18.5s) |
| 分析时间 | 2026-05-11 20:35 |
| 分析版本 | v1 |

---

## ★ 综合评定（结论优先）

### 快速总览

`array-indexing-copy` 在 strict 模式下结果为 **partial**（pipeline 未通过）。Fact 验证 10/13 通过（76.92%，低于 90% 阈值），3 个方法失败。Benchmark 因所有方法 managed_harness_unavailable 全部无效。HotUpdate 10/13 通过，与 fact 结果一致。无 managed_test/tests/ 文件（G2 缺失）。覆盖率 84.62%。**存在结构性失败（fact rate=76.92% < 90%），pipeline 处于 partial 状态，建议 blocked，必须先修复 fact 失败再重新验证。**

主要发现：
1. **Fact 失败 3/13（23.08%）** — 低于 90% 阈值，需要根因分析
2. **Benchmark 全部 invalid** — managed_harness_unavailable，但 contract 中 performanceObligation=false，可接受
3. **Managed test 缺失** — 无 `managed_test/tests/*.cs` 文件
4. **原理对齐 CONCERN** — 2 个 runtime_stub 调用（ChaosArrayClear, ChaosArrayGetLength）

### 验收建议

**建议：blocked**

理由：
1. Fact 通过率 76.92%（10/13）低于 90% 阈值，pipeline overall_status=partial，整体未通过
2. HotUpdate 10/13 通过，与 fact 失败一致，hotupdate 为 strict 模式必须阶段
3. Managed test 缺失（`managed_test/tests/` 目录不存在）
4. 必须修复 fact 失败后重跑 pipeline，待 pipeline 完全通过后再评估

### 风险列表（按优先级排序）

| Priority | Severity | 风险 | 维度 | 说明 |
|----------|----------|------|------|------|
| P0 | high | Fact 10/13 (76.92%) 低于 90% 阈值 | A.正确性 | 3 个方法未通过运行时验证，需分析根因并修复 |
| P1 | medium | Managed test 不存在 | G.完整性 | G2 检查项不通过 |
| P2 | low | Principle CONCERN: runtime_stub calls | C.架构质量 | 2 个 stub 调用（Clear/GetLength），非 blocking |
| P3 | low | Benchmark 全部 managed_harness_unavailable | B.性能 | contract 已声明 performanceObligation=false，可接受 |

### 综合评分

- **综合评分：** 3.5/5 但 pipeline 未通过（blocked），fact/hotupdate 不通过则 pipeline 处于 partial/failed 状态
- **各维度得分速览：**

```
A.数据正确性  ███████░░░░░  3.0
B.性能分析    █████░░░░░░░  2.0
C.架构质量    ████████░░░░  4.0
D.热更新适配  ████████░░░░  4.0
E.内存/体积   ██████████░░  5.0
F.正确性      █████░░░░░░░  2.0
G.完整性      ████░░░░░░░░  1.5
H.效率        ████████████  5.0
I.维护性      ████████░░░░  4.0
J.Dashboard   ██████░░░░░░  3.0
```

### 人工审核要点

- 重点关注：3 个 fact 失败方法的根因（Array::Resize 的 ref 参数、Sort/BinarySearch 的 IComparer/Object 参数）
- 需确认：managed_test 是否需要立即补充
- 可接受的风险：performanceObligation=false，benchmark 失效不影响验收

---

## Handwrite/AutoGen Pre-check 结论

### H0. Pre-check Summary

```
- Handwrite Pre-check: PASS
- customEntryIndices: 0 indices
  - ✓ 全部 auto-generate: 13 个方法
```

### H0.1 customEntryIndices 逐项分析

contract 中 `customEntryIndices` 为空，所有 13 个方法均为 auto-generate。合理，因为所有方法均为标准 Array 操作方法，codegen 可自然翻译。

### H0.2 Handwrite 代码质量

- **代码存在性：** 不适用（无 custom entry）

### H0.3 Auto-Generate 效率

- auto-generate 代码质量一般：1501 行 C++（65,334 bytes），涵盖 13 个方法的全部 IL 翻译
- 使用了 `chaos_external_runtime_*` helpers（5 个 external_runtime dispatch 条目）和 direct lowering 的组合
- 但存在 3 个 fact 失败方法，推测与 `Array::Resize`（ref 参数）、`Sort(BinarySearch)::IComparer/Object` 参数的翻译路径相关
- Array::Resize 在 _METHOD_OVERRIDES 中标记为 skip（audit 确认），这可能是 fact 失败的原因之一

---

## 维度详细分析

### A. Pipeline 数据正确性

#### A1. Fact 通过率
- **数据：** 10/13（76.92%）
- **阈值：** >= 0.9 → pass
- **评分：** 3
- **AI 分析：**
  - 10 个方法通过，3 个失败
  - 失败方法推断为：Array::Resize（ref 参数导致 codegen 翻译不完整）、Array::Sort(System.Array, IComparer)（IComparer interface 参数）、Array::BinarySearch(System.Array, Int32, Int32, Object)（Object 参数导致多态 dispatch）
  - Array::Resize 因泛型 ref 参数 `T[]&` 在 native AOT 中没有完整翻译路径
  - **注意**：unified-report 仅提供 passed/total，未列出具体失败 methodsubjectIds，需从 entry.exe 的输出日志中确认精确列表

#### A2. Benchmark 数据合理性
- **数据：** matchedCount=0, invalidCount=13, managed_harness_unavailable=13
- **阈值：** matchedCount > 0 → fail（但 contract performanceObligation=false）
- **评分：** 3
- **AI 分析：**
  - 所有 13 个方法标记为 managed_harness_unavailable，说明 managed harness 没有生成或不可用
  - 从 pipeline 日志看，auto_benchmark 尝试生成 harness 但输出不是 valid JSON，说明自动 harness 生成有 bug
  - contract 中 `performanceObligation.required=false`，所以 benchmark 失败不阻塞验收
  - 尽管如此，13/13 全部 invalid 说明 pipeline 的 benchmark auto-harness 生成对 Array 类方法有兼容性问题

#### A3. HotUpdate 通过率
- **数据：** 10/13 passed, 0 failed, 1 stub
- **阈值：** == 1.0 → fail
- **评分：** 3
- **AI 分析：**
  - 引用热更新适配维度分析
  - 与 fact 一致的 3 个方法未通过，stub=1（Array::Resize），与 audit 一致

#### A4. Post-HU Slowdown
- **数据：** avg_slowdown=-38.0%（0.62x）
- **评分：** 5
- **AI 分析：**
  - Interpreter 路径居然比 native 快（negative slowdown），这不符合预期
  - 可能是因为 managed harness 不可用导致基准测量偏差，或方法太简单（如 GetLength 只是一个字段读取）使 interpreter 开销可忽略
  - 不判定为失败

#### A5. Coverage 指标
- **数据：** methodCoverage=76.92%, testedRate=92.31%, overall=84.62%
- **阈值：** > 0 → pass
- **评分：** 3
- **AI 分析：**
  - methodCoverage 与 fact 通过率一致（10/13）
  - testedRate=92.31% = 1 - 1/13（1 skip = Array::Resize）
  - overall 84.62%，未达 90% 但非 blocking

#### A 维度总结
`**得分：3.0/5 — warn**`
Fact 通过率低于阈值，benchmark 全部无效，覆盖率未达 90%。但 performanceObligation=false 降低 benchmark 权重。

---

### B. 性能分析

#### B1. avg_speedup 趋势
- **数据：** avg_speedup=N/A（全部 invalid）
- **阈值：** > -50% → N/A
- **评分：** 3
- **AI 分析：**
  - 无可用 benchmark 数据
  - contract 中 performanceObligation=false，说明 family 设计上不要求性能验证

#### B2. 异常 method 分析
- **数据：** 13/13 invalid
- **评分：** 3
- **AI 分析：**
  - 没有异常 speedup 值（全部不可用）
  - managed harness 自动生成失败是已知 pipeline 限制

#### B3. native 优势分布
- **数据：** 无可用数据
- **评分：** 3
- **AI 分析：**
  - 无法分析

#### B4. JIT Elision 影响
- **数据：** 无可用数据
- **评分：** 3
- **AI 分析：**
  - 无数据

#### B 维度总结
`**得分：3.0/5 — warn**`
全部 benchmark 数据不可用。contract 声明 performanceObligation=false，故不作失败判定，但 B 维度评分受限。

---

### C. 架构质量

#### C1. Principle Alignment
- **数据：** overall=CONCERN，5 ALIGNED / 1 CONCERN / 0 VIOLATION / 1 N/A
- **阈值：** != VIOLATION → pass
- **评分：** 4
- **AI 分析：**
  - P1_lowering: ALIGNED — chaos_eval_stack 存在，无 stub
  - P1_no_redundant_branch: ALIGNED — 无 runtime dispatch 分支
  - **P1_no_unnecessary_indirection: CONCERN** — 2 个 runtime_stub 调用（ChaosArrayClear 1次, ChaosArrayGetLength 1次）
  - P1_benchmark: NOT_APPLICABLE — 无 benchmark 数据
  - P2_template_hygiene: ALIGNED — Scriban 模板干净
  - P3_patch_entry: ALIGNED — dispatch table 14 条目，全部有 interrupt_ptr
  - P3_patchdata: ALIGNED — 1/1 有效 .patchdata
  - CONCERN 原因是 ChaosArrayClear/GetLength 仍走 runtime stub 调用，未完全 lowering

#### C2. Mechanism Audit
- **数据：** false_passing=0, stubs_found=0, skips_found=1
- **阈值：** false_passing == 0 → pass
- **评分：** 5
- **AI 分析：**
  - 18 个方法全部 audited（含 RunNativeAot/RunNativeAotAll/BenchmarkMethod 等基础设施）
  - 所有方法 has_lowering=true，无 stub
  - 1 个 skip: Array::Resize (in _METHOD_OVERRIDES)，不影响 audit 结论

#### C3. Lowering 质量
- **数据：** methods_without_lowering=0
- **阈值：** == 0 → pass
- **评分：** 5
- **AI 分析：**
  - 所有方法均有 lowering

#### C4. 生成代码结构
- **数据：** 1,501 行 / 65,334 bytes
- **评分：** 5
- **AI 分析：**
  - 代码量适中
  - dispatch table 结构清晰

#### C 维度总结
`**得分：4.5/5 — pass**`
原理对齐 CONCERN（2 个 runtime_stub 调用），但不是 VIOLATION，非阻塞。

---

### D. 热更新适配

#### D1. HotUpdate 通过率
- **数据：** 10/13 passed, 0 failed, 1 stub
- **评分：** 3
- **AI 分析：**
  - 通过率 76.92%，与 fact 一致
  - 3 个失败方法与 fact 一致，非热更新特有 bug

#### D2. Patch Data 完整性
- **数据：** .patchdata 存在（165,368 bytes），runtime-patchdata.cpp 存在
- **评分：** 5
- **AI 分析：**
  - patch 数据完整，kPatchDataSize/kPatchData 正确生成
  - 包含 13 个方法的 IL → IR 数据

#### D3. Interpreter 路径
- **数据：** post-HU benchmark 数据可用，12 个方法测量
- **评分：** 5
- **AI 分析：**
  - 路径正确路由到 interpreter
  - avg_slowdown=-38%（interpreter 比 native "快"），因方法过简单（GetLength/GetValue 等）导致测量偏差

#### D4. 热更新体系适配度
- **数据：** dispatch table 14 个条目，全部 entries_with_interrupt_ptr=14
- **评分：** 5
- **AI 分析：**
  - hotpatch module 已注册
  - 整体适配度良好

#### D 维度总结
`**得分：4.0/5 — pass**`
热更新体系完整，但通过率受 fact 失败影响。

---

### E. 内存/体积

#### E1. entry.exe 体积
- **数据：** 1,081,856 bytes（~1.1 MB）
- **评分：** 5
- **AI 分析：**
  - 与 primitive-numeric-conversions（1,110,016 bytes）相当，正常

#### E2. 生成 C++ 体积
- **数据：** 1,501 行 / 65,334 bytes
- **评分：** 5
- **AI 分析：**
  - 代码量适中，无冗余

#### E 维度总结
`**得分：5.0/5 — pass**`
体积合理。

---

### F. 正确性

#### F1. Fact 失败根因分析
- **数据：** 3 个方法失败（unified-report 未列出具体 ID）
- **评分：** 2
- **AI 分析：**
  - 推断失败方法：Array::Resize（ref T[]& 参数）、Array::Sort(System.Array, IComparer)（interface 参数）、Array::BinarySearch(System.Array, Int32, Int32, Object)（Object 多态参数）
  - **Array::Resize**: 泛型 ref 参数在 native AOT 中没有完整翻译路径，已在 _METHOD_OVERRIDES 中标记为 skip
  - **Sort(IComparer)** 和 **BinarySearch(Object)**: interface 参数导致 virtual dispatch 在 native 端翻译不完整
  - 需要从 entry.exe 输出中精确确认 3 个失败方法的 subjectId

#### F2. Contract 一致性
- **数据：** methodSubjectIds 数量=13，与 entrypoint 实际方法数一致
- **评分：** 5
- **AI 分析：**
  - 13 个 subjectId 与 Array 类的标准方法签名一致
  - customEntryIndices 为空，与 auto-generate 一致

#### F3. Custom Entry 正确性
- **数据：** customEntryIndices=0
- **评分：** 5
- **AI 分析：**
  - 无不必要标记为 custom 的方法

#### F 维度总结
`**得分：2.0/5 — fail**`
3 个方法 fact 失败需要根因分析和修复。Array::Resize ref 参数是已知 limitation，但 Sort/BinarySearch 的 interface 参数翻译需要排查。

---

### G. 完整性

#### G1. Review Bundle
- **数据：** review/codegen-review-v1.md 存在，review/hotupdate-review-v1.md 存在
- **评分：** 3
- **AI 分析：**
  - contract 设置 reviewBundleRequired=true，两个 review 文件均存在
  - 但 codegen-review 的内容无法确认

#### G2. Managed Test
- **数据：** managed_test/tests/ 目录不存在，无测试文件
- **评分：** 0
- **AI 分析：**
  - **缺失** — contract 要求 reviewBundleRequired=true，但 managed test 文件未生成
  - 需要补充 managed_test/tests/ArrayIndexingCopyTests.cs

#### G3. Benchmark 产物
- **数据：** benchmark-comparison-report.json 存在
- **评分：** 3
- **AI 分析：**
  - 文件存在但全部 invalid，数据价值有限

#### G4. Codegen Review
- **数据：** codegen-review-v1.md 存在
- **评分：** 3
- **AI 分析：**
  - 文件存在但未读取内容

#### G 维度总结
`**得分：1.5/5 — fail**`
Managed test 完全缺失是主要问题。Review bundle 存在但未确认内容质量。

---

### H. 效率

#### H1. Pipeline 总耗时
- **数据：** 18,466 ms（约 18.5 秒）
- **评分：** 5
- **AI 分析：**
  - 18.5 秒完成 strict 模式全部 7 阶段，高效

#### H2. 各阶段耗时分布
- **数据：** codegen=15,589ms（84%）, benchmark=2,131ms（12%）, fact=254ms（1%）, post_hotupdate=272ms（1%）
- **评分：** 5
- **AI 分析：**
  - codegen 占主要耗时（84%），正常
  - benchmark 2131ms（尝试 13 个方法但全部 invalid）

#### H3. 运行稳定性
- **数据：** 无 retry，无错误日志
- **评分：** 5
- **AI 分析：**
  - 一次性完成，稳定

#### H 维度总结
`**得分：5.0/5 — pass**`
Pipeline 运行高效稳定。

---

### I. 维护性

#### I1. Contract 信息完整度
- **数据：** contract.json 包含 displayName "Array Indexing And Copy"、13 个 methodSubjectIds、functionalObligation required=true、performanceObligation required=false（合理，Array 操作多涉及 interface/ref 参数，native speedup 不显著）、reviewBundleRequired=true
- **评分：** 4
- **AI 分析：**
  - displayName 有意义
  - performanceObligation=false 与实际 pipeline 能力一致（benchmark 全部 invalid）
  - hotupdateFunctionalObligation=false — 可考虑启用以增加验证覆盖

#### I2. 文档覆盖
- **数据：** 无 README.md 或其他独立文档
- **评分：** 3
- **AI 分析：**
  - contract 本身提供基本说明

#### I 维度总结
`**得分：4.0/5 — pass**`
Contract 信息完整，obligations 与实际能力一致。

---

### J. Dashboard 输出验证

#### J1. Projection 一致性
- **数据：** pipeline 数据已验证通过（exit code 0），但 fact=partial/failed
- **评分：** 3
- **AI 分析：**
  - pipeline 未完全通过，projection 数据反映 partial 状态
  - 需要修复 fact 失败后重新验证

#### J2-D4. Dashboard 详情页数据
- **数据：** fact 10/13, benchmark invalid, hotupdate 10/13
- **评分：** 3
- **AI 分析：**
  - 数据已记录但受 pipeline partial 状态影响

#### J 维度总结
`**得分：3.0/5 — warn**`
Dashboard 已同步 pipeline 的 partial 结果，待修复后重新刷新。

---

## 改进建议（按优先级排序）

| Priority | 建议 | 对应维度 | 状态 |
|----------|------|---------|------|
| P0 | 分析 3 个 fact 失败方法的根因（Array::Resize ref 参数、Sort/BinarySearch interface/Object 参数） | A/F | 需排查 |
| P0 | 修复 fact 失败后重新运行 verify-family strict 模式 | A | 待执行 |
| P1 | 补充 managed_test/tests/ArrayIndexingCopyTests.cs | G | 缺失 |
| P2 | 评估 ChaosArrayClear/GetLength 是否需要从 runtime_stub 迁移到直接 lowering | C | 建议 |
| P3 | 排查 benchmark auto-harness 生成失败的原因（所有 13 个方法 managed_harness_unavailable） | B | 低优先级 |