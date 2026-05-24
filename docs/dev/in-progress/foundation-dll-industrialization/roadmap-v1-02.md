# Foundation DLL 工业化级验证 — Roadmap v1-02

## 目标

对 14 个 assembly、~140 个 family 完成 **全量 13-stage 工业化级验证**。每个 family 跑通：preflight → codegen AOT → jit_codegen → fact AOT → fact JIT → audit → asm_compare → microbench → benchmark → hotupdate ×4，发现的问题即发现即修复，最终实现 **工业化成熟度 ≥ 4/5**（CI 集成 + 文档完备 + 回归基线 + 零阻塞性失败）。

## 范围边界

- 14 个 assembly（CoreLib、Linq、Collections.Immutable、S.R.IS、Text.Json、Formats.Asn1、Brotli、ZipFile、Pipelines、ServerSentEvents、ObjectModel、Serialization.Formatters、Security.Principal.Windows、Tasks.Parallel）
- ~140 个具备 contract.json 的 family
- 推进粒度：family 级
- 验证深度：全量 13-stage

## 非目标

- 不新增 family 或 method subject
- 不修改 contract.json 内容
- 不重构验证管线基础设施架构（仅修 bug 不重构）

## 全局优先级约束

P1 性能最优、P2 方案完美性、P3 HotUpdate 方向一致，无冲突。所有阶段默认继承此裁决。

## 阶段列表

### Phase 0: Pipeline 预检

- **Goal**: 排除已知的 pipeline 阻塞问题，确保 13-stage 管线可稳定运行
- **Exit Criteria**: 所有已知 pipeline bug 已修复，1 个代表性 family 完整跑通 13-stage 确认
- **Deliverables**:
  - pipeline bug fix 记录（pipeline-fix-log.md）
  - KNOWN_ISSUES.md（已知固有限制清单）
  - 修复验证报告（1 family 13-stage passed）
- **Dependencies**: 无
- **Resolved Decisions**:
  - Pipeline bug 优先修，不等到 Phase 2 再暴露
  - 已知固有 VIOLATION（如 p1_benchmark）只记录不修
- **Watch Items**: 微基准崩溃根因可能涉及 runtime-core，修复范围可能超出 pipeline 本身

#### 子任务列表

| # | 子任务 | 说明 |
|:-:|--------|------|
| P0.1 | 修复 microbench crash (0xC0000005) | entry.exe --microbench 访问违例根因排查与修复 |
| P0.2 | 修复 audit 计数口径不一致 | pipeline 与 audit report 的 method 计数统一 |
| P0.3 | 补齐 Asn1/encoding-rules subject IDs | 补 contract.json 的 methodSubjectIds |
| P0.4 | 补齐 Text.Json/attributes subject IDs | 补 contract.json 的 methodSubjectIds |
| P0.5 | 修复 pipeline-config.yaml 与实际脱节 | 清理或修复配置使其被实际消费 |
| P0.6 | 端到端验证 | 选 1 family 完整跑通 13-stage 确认 |

---

### Phase 1: 基线扫描

- **Goal**: 全量快速摸底，建立当前通过率基线
- **Exit Criteria**: 全部 ~140 family 跑完快速模式（codegen + fact + audit），输出基线结果
- **Deliverables**:
  - 基线扫描结果（verification-tracker.json 初始填充）
  - 每 family 的通过/失败记录
  - 初步系统性 bug 预判
- **Dependencies**: Phase 0
- **Resolved Decisions**: 快速模式不跑 benchmark/hotupdate，但 codegen+fact+audit 必须全
- **Watch Items**: 如果大量 family 在快速模式就失败，需评估是否继续推进还是先修一批

#### 子任务列表

| # | 子任务 | 说明 |
|:-:|--------|------|
| P1.1 | 运行基线扫描 | serial_full_verify.py --skip benchmark hotupdate × 140 families |
| P1.2 | 汇总基线结果 | 整理通过率、失败分类、失败根因初步分析 |

---

### Phase 1.5: 风险排序

- **Goal**: 基于基线结果生成量化风险排序，创建完整跟踪系统
- **Exit Criteria**: verification-tracker.json 已填充所有 family 的风险等级和系统性 bug 标记
- **Deliverables**:
  - verification-tracker.json（完整跟踪数据）
  - SYSTEMIC_ISSUES.md（系统性 bug 清单）
  - FAMILY_ORDER.md（按风险排序的执行顺序）
  - VERIFICATION_CHECKLIST.template.md 已就位
- **Dependencies**: Phase 1
- **Resolved Decisions**:
  - 排序算法：Risk_Score = Failure_Severity × Complexity_Multiplier
  - Complexity_Multiplier 基于 contract.json 方法签名的静态分析
- **Watch Items**: 如果系统性 bug 过多（>10 个），需评估是否回到 Phase 0

#### 子任务列表

| # | 子任务 | 说明 |
|:-:|--------|------|
| P1.5.1 | 计算风险评分 | 对每个 family 计算 Failure_Severity + Complexity_Multiplier |
| P1.5.2 | 识别系统性 bug | 扫描失败 family 的公共根因，创建 SYS-N |
| P1.5.3 | 排序并填充 tracker | 按 P0→P3 排序写入 verification-tracker.json |

---

### Phase 2: 逐 family 攻坚

- **Goal**: 按风险优先级逐个 family 跑全量 13-stage，发现 bug 即修
- **Exit Criteria**: 全部 ~140 family 的 13-stage 完成，最终状态为 ✅ PASSED 或 ⚠️ FAILED (accepted)
- **Deliverables**:
  - 每 family 的 VERIFICATION_CHECKLIST.md
  - verification-tracker.json（全程持续更新）
  - SYSTEMIC_ISSUES.md（系统性 bug 解决记录）
  - 全部修复累积记录（含 benchmark 前后对比）
- **Dependencies**: Phase 1.5
- **Resolved Decisions**:
  - 执行模式：逐 family 串行，风险优先（P0→P1→P2→P3）
  - 失败处理：即停即修，每失败走完整 triage 协议
  - 系统性 bug：标记 affected，修根因后只重跑受影响 family
  - Benchmark 追踪：每次 fix 记录前后对比
  - 回归验证：每个 resolved SYS 选 1-2 个已 passed family 验证
- **Watch Items**:
  - 如果 P0 阶段大量 family 失败，说明 il2cpp/runtime 存在深层问题
  - 如果系统性 bug 修复影响面判断不准，需手动确认

#### 子任务列表（按风险 Tier 分组）

| # | Tier | 说明 |
|:-:|:----:|------|
| P2-T0 | P0 | 基线失败 + 复杂 family（先攻坚最深的问题） |
| P2-T1 | P1 | 基线失败 + 简单 family |
| P2-T2 | P2 | 基线通过 + 复杂 family（潜在问题） |
| P2-T3 | P3 | 基线通过 + 简单 family（快速通过） |

每个 Tier 内按 Phase 1 耗时降序排列（耗时越长说明方法越多，放在该 Tier 前面）。

---

### Phase 3: 工业化收口

- **Goal**: 补齐工业化要素，达到成熟度 ≥ 4/5
- **Exit Criteria**: CI 集成完成、wiki 文档就绪、全量 dashboard 已生成、回归阈值已配置
- **Deliverables**:
  - codegen-regression.yml foundation-dll smoke 步骤
  - wiki 管线架构文档 + stage 说明 + 故障诊断指南
  - 全量统一 dashboard（含所有 family 汇总）
  - 回归阈值配置（基于 Phase 2 benchmark 数据）
  - 全量最终重跑确认报告
- **Dependencies**: Phase 2
- **Resolved Decisions**:
  - CI 选 3-5 个代表性 family 做快速模式（非全量 13-stage）
  - 全部 ~140 family 的全量重跑只做最后一次确认
- **Watch Items**: 全量重跑如果出现 regression，需要回到 Phase 2 修复

#### 子任务列表

| # | 子任务 | 说明 |
|:-:|--------|------|
| P3.1 | CI 集成 | codegen-regression.yml 增加 foundation-dll smoke |
| P3.2 | Wiki 文档 | 管线架构 + stage 说明 + 故障诊断 |
| P3.3 | Dashboard | 全量汇总 + 回归阈值 |
| P3.4 | 全量最终重跑 | 全部 family 完整 13-stage 确认 |

---

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|------------|----------|--------------|--------------|---------------|----------------|-----------------|
| fd-p0-precheck | 0 | completed | main | 修复已知 pipeline 阻塞 bug，确保管线可稳定运行 | — | — | 修复 microbench crash、audit 误报、补齐 subject IDs、修复 pipeline-config、1 family 13-stage 端到端验证 | pipeline-fix-log.md, KNOWN_ISSUES.md, verification-tracker.json | 1 family 完整跑通 13-stage | build/toolchains/run/testing/foundation_dll/, testing/foundation-dll/ | 1d |
| fd-p1-baseline | 1 | completed | main | 全量快速摸底，建立通过率基线 | fd-p0-precheck | — | serial_full_verify.py 快速模式 × 140 families | verification-tracker.json（初始填充） | 全部 family 完成快速模式 | testing/foundation-dll/ | 0.5d |
| fd-p15-ranking | 1.5 | completed | main | 基于基线生成风险排序，创建完整跟踪系统 | fd-p1-baseline | — | 计算 Risk_Score、识别系统性 bug、填充 tracker | verification-tracker.json（完整）, SYSTEMIC_ISSUES.md, FAMILY_ORDER.md | tracker 包含全部 family 的 tier+系统性标记 | docs/dev/in-progress/foundation-dll-industrialization/ | 0.5d |
| fd-p2-storm | 2 | planned | main | 按风险优先级逐 family 全量 13-stage，发现即修 | fd-p15-ranking | — | 逐 family 运行 → triage → fix → re-run，含系统性 bug 批量修复 | 每 family 的 VERIFICATION_CHECKLIST.md, 更新的 tracker.json | 140 family 终态为 PASSED 或 FAILED(accepted)，阻塞性失败=0 | testing/foundation-dll/, src/native/, src/managed/ | 3-5d |
| fd-p3-industrialization | 3 | planned | main | CI 集成 + 文档 + dashboard + 全量重跑确认 | fd-p2-storm | — | CI smoke、wiki 文档、dashboard、回归阈值、全量重跑 | codegen-regression.yml patch, wiki 页面, dashboard | CI smoke 运行通过，全量重跑 140/140 通过 | .github/workflows/, wiki/ | 1d |

## 依赖

```
fd-p0-precheck → fd-p1-baseline → fd-p15-ranking → fd-p2-storm → fd-p3-industrialization
```

全部串行，无并行路径。

## 风险

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| Pipeline bug 修复耗时超预期 | 中 | 中 | 先修阻塞性 bug，非阻塞性标记 known issue 进入 Phase 2 |
| 基线扫描大量 family 失败 | 中 | 高 | 优先识别系统性根因，评估是否回到 Phase 0 先修一批 |
| 系统性 bug 影响面判断遗漏 | 中 | 中 | 回归验证机制 + 重复检查合同扫描结果 |
| Phase 2 修复引入 regression | 低 | 高 | 每次系统性修复后做回归验证（选已 passed family） |
| 全量 13-stage 运行时间过长 | 高 | 中 | 串行约 7h（纯运行），可中断恢复（tracker 记录 current_family） |
| 单个 family 修复时间不可控 | 中 | 中 | 复杂 bug 标记留待后续，不阻塞整体推进 |

## 备选路径

- 如果 Phase 0 耗时超 2d：只修 microbench crash 和 subject IDs，audit 误报和 pipeline-config 留到 Phase 3
- 如果 Phase 1 基线扫描 > 50% 失败：暂停 Phase 2，评估是否需要大规模运行时修复
- 如果 Phase 2 某个 tier 修复周期过长：标记已知限制进入 accepted，不阻塞后续 tier
- 如果 Phase 3 全量重跑出现 10+ regression：回到 Phase 2 的修复循环

## 当前建议推进顺序

```
fd-p0-precheck → fd-p1-baseline → fd-p15-ranking → fd-p2-storm → fd-p3-industrialization
```

## 子任务执行策略

- child_execution_mode: auto
- auto_continue: true
- auto_stop_policy: blocking-only
- dispatch_model: sequential
- recommended_next_child: fd-p0-precheck
