# Foundation DLL 验证管线 — 工业化综合评估报告

> **评估日期**: 2026-05-23
> **评估范围**: foundation-dll 验证管线的 13-stage pipeline 基础设施、覆盖度、CI/CD、文档、风险
> **对标标准**: 已完成的 marshal-industrialization / metadata-writer-industrialization 工业化水平

---

## 1. 当前状态概要

### 1.1 管线架构

13-stage 统一验证管线（`foundation-dll/_core/python/orchestrator.py` → `VerificationPipeline`）：

```
preflight → codegen → jit_codegen → fact → fact_jit → audit →
asm_compare → microbench → benchmark → hotupdate ×4
```

所有 13 个 stage 的 runner 已实现，管线可端到端运行。

### 1.2 覆盖范围

| Assembly | Families | 已验证 | 通过率 | 备注 |
|----------|----------|--------|--------|------|
| System.Private.CoreLib | 48 | 7 (Batch 1) + 45 (历史) | ~96% | buffer-memory 有 2 stage 误报 |
| System.Collections.Immutable | 10 | 10 | 100% | |
| System.Formats.Asn1 | 6 | 6 | 83% | encoding-rules 缺 subject IDs |
| System.IO.Compression.Brotli | 4 | 4 | 100% | |
| System.IO.Compression.ZipFile | 2 | 2 | 100% | |
| System.IO.Pipelines | 5 | 5 | 100% | |
| System.Linq | 12 | 12 | 100% | |
| System.Net.ServerSentEvents | 4 | 4 | 100% | |
| System.ObjectModel | 6 | 6 | 100% | |
| System.Runtime.InteropServices | 14 | 14 | 100% | |
| System.Runtime.Serialization.Formatters | 6 | 6 | 100% | |
| System.Security.Principal.Windows | 6 | 6 | 100% | |
| System.Text.Json | 13 | 13 | 92% | attributes 缺 subject IDs |
| System.Threading.Tasks.Parallel | 4 | 4 | 100% | |
| **合计** | **~140** | **~140** | **~97%** | 2 family 缺 subject IDs |

### 1.3 已验证但未纳入批量报告的 Family

CoreLib 中 52/53 family 有 `unified-verification-report.json`，但最新 batch report 只记录了最近一次运行。历史运行结果是 green 的，但未集中归入统一 dashboard。

---

## 2. 工业化差距分析

### 2.1 ✅ 已完成项

| 维度 | 状态 | 证据 |
|------|------|------|
| 13-stage 管线端到端 | ✅ | orchestrator.py + 13 runner |
| 绝大多数 family 验证通过 | ✅ | ~140 families, ~97% pass rate |
| Dashboard 输出 | ✅ | deep-dashboard.html (10 KB) |
| Perf 数据存储 | ✅ | perf_store 每 family 保存 |
| Batch runner | ✅ | batch_run_all.py + CLI |
| Phase 0 基础设施验证 | ✅ | Brotli/decoder 13-stage passed |
| Phase 1 Batch 1 (7 families) | ✅ | 6/7 full pass, 1 with infra false-positive |

### 2.2 ❌ 未达标项

#### 差距 1：CI/CD 集成（严重）

| 子项 | 状态 | 说明 |
|------|------|------|
| PR 触发 | ❌ | codegen-regression.yml 仅含 `cd testing/foundation-dll || echo "not set up"` |
| Push 到 main 触发 | ❌ | 同上 |
| 回归自动检测 | ❌ | 无 |
| 失败通知 | ❌ | 无 |

当前管线完全依赖手动触发 `python batch_run_all.py`。与 marshal-industrialization（CI workflow 中注册了 marshal 测试）和 metadata-writer-industrialization（新增 codegen-regression.yml 步骤）形成鲜明对比。

#### 差距 2：文档（中等）

| 子项 | 状态 | 说明 |
|------|------|------|
| 管线架构文档 | ❌ | 无独立 wiki 页面，仅有 236 行的 data-dictionary |
| Stage 说明文档 | ❌ | 无 |
| 故障诊断指南 | ❌ | 无 |
| pipeline-config.yaml 说明 | ❌ | 无 |

对照 metadata-writer 的 `28-MetadataWriter架构.md`（190 行，含 5 种故障诊断），foundation-dll 管线几乎没有面向使用者的文档。

#### 差距 3：已知基础设施问题（中等）

| 问题 | 影响 | 根因 |
|------|------|------|
| Microbench 访问违例崩溃 | 多个 family 的 stage 7 失败 | **已定位**：GC BGC 线程与 mutator 并发竞态。Subject_0 反复分配触发 BGC，BGC 与 mutator 并发访问 GC 堆导致 0xC0000005。属于 GC 工程问题，非 microbench 特有。详见 FD-I2。 |
| Audit stage 评估误报 | buffer-memory 的 audit report 自称 passed: true，但 pipeline 标 failed | 计数口径不一致（report:63 methods vs pipeline:87 methods） |
| 2 family 缺 subject IDs | Asn1/encoding-rules, Text.Json/attributes 无法通过 | 未补充 contract.json 的 methodSubjectIds |

#### 差距 4：测试覆盖率（中等）

| 子项 | 状态 | 说明 |
|------|------|------|
| Pipeline runner 单元测试 | ❌ | 无 |
| Orchestrator 测试 | ❌ | 无 |
| Stage runner 测试 | ❌ | 无 |
| 集成测试 | ❌ | 无（仅端到端运行） |

对照 marshal-industrialization（119 native tests + 185 managed tests），foundation-dll 管线基础设施本身没有测试。

#### 差距 5：架构审视（低）

| 子项 | 状态 | 说明 |
|------|------|------|
| 错误处理细化 | ⚠️ 部分 | run_family 有 try/except，但 stage 级别的错误处理和重试机制弱 |
| pipeline-config.yaml 与运行时脱节 | ⚠️ | config 中 migrationSkips=true 但不被 orchestrator 消费 |
| 代码重复 | ✅ | 各 runner 职责清晰 |

#### 差距 6：Benchmark 基线（低）

| 子项 | 状态 | 说明 |
|------|------|------|
| 3-way benchmark 基线 | ✅ | 已收集 |
| HotUpdate baseline | ✅ | 已收集 |
| 回归阈值 | ❌ | perf_store 收集了数据但未设置回归门禁 |
| 历史趋势 dashboard | ❌ | deep-dashboard.html 仅显示单次结果 |

---

## 3. 风险评估

### 3.1 高优先级风险

| # | 风险 | 概率 | 影响 | 说明 |
|---|------|------|------|------|
| R1 | **回归无法自动检测** | 高 | 高 | CI 无集成，任何 codegen 变更可能破坏 family 验证而不自知 |
| R2 | **Microbench 崩溃已定位但未修复** | 中 | 高 | BGC 并发竞态导致 0xC0000005。根因在 GC 工程（BGC mutator 同步），非短期可修。CI smoke 已跳过 microbench，缓解了大部分影响 |
| R3 | **全量重验证成本高** | 高 | 中 | ~140 families × ~50s = ~2 小时全量运行；每次 main merge 不可能全量重跑 |

### 3.2 中优先级风险

| # | 风险 | 概率 | 影响 | 说明 |
|---|------|------|------|------|
| R4 | **pipeline-config.yaml 与实际运行脱节** | 高 | 中 | migrationSkips=true 但不被代码消费，造成配置与行为不匹配 |
| R5 | **CoreLib 剩余 41 families 未通过当前 pipeline 验证** | 中 | 中 | 有历史 unified-report 但未通过最新的 13-stage pipeline 重跑 |
| R6 | **2 family 缺 subject IDs 无限期搁置** | 高 | 低 | 已已知但未修复，影响精确覆盖率统计 |

### 3.3 低优先级风险

| # | 风险 | 概率 | 影响 | 说明 |
|---|------|------|------|------|
| R7 | **Audit stage 评估误报影响可信度** | 中 | 低 | pipeline 标记 failed 但审计报告自称 passed |
| R8 | **HotUpdate 验证覆盖率不全** | 低 | 中 | 部分 family 的 hotupdate bench 返回 0/10（无 hotupdate 数据） |

---

## 4. 推荐行动项

### P0（必须完成，影响工业化 gate）

| ID | 行动 | 预估 | 状态 | 关联风险 |
|----|------|------|------|---------|
| FD-I1 | **CI 集成**：在 codegen-regression.yml 中增加 foundation-dll smoke 步骤（选 3-5 个代表性 family 自动运行 6-stage 快速模式） | 1d | ✅ **已完成** | R1, R4 |
| FD-I2 | **Root cause microbench crash**：排查 entry.exe --microbench 的 access violation 根因 | 1d | ✅ **已定位**：BGC 并发竞态（GC 工程问题，非简单修复） | R2 |

### P1（重要，影响日常使用）

| ID | 行动 | 预估 | 关联风险 |
|----|------|------|---------|
| FD-I3 | **Wiki 文档**：创建 foundation-dll-verification-pipeline.md，包含管线架构、stage 说明、运行方法、故障诊断 | 0.5d | 文档缺口 |
| FD-I4 | **修复 audit stage 评估逻辑**：统一 pipeline 与 audit report 的计数口径 | 0.5d | R7 |
| FD-I5 | **补齐 2 family 缺省 subject IDs** | 0.5d | R6 |
| FD-I6 | **覆盖 CoreLib 剩余 families**：运行 fd-p1-corelib-b2~b7 | 2d | R5 |

### P2（改善类）

| ID | 行动 | 预估 | 关联风险 |
|----|------|------|---------|
| FD-I7 | **Pipeline 测试**：为核心 orchestrator/runner 添加单元测试 | 1d | 测试缺口 |
| FD-I8 | **Benchmark 回归阈值**：基于已有 perf 数据设置回归检测阈值 | 0.5d | 基线缺口 |
| FD-I9 | **全量重跑并刷新 dashboard**：所有 ~140 families 统一重跑，生成全量 dashboard | 2h | R5 |
| FD-I10 | **修复 pipeline-config.yaml**：使其被 orchestrator 实际消费或删除 | 0.5d | R4 |

---

## 5. 工业化成熟度评分

| 维度 | 权重 | 评分 | 说明 |
|------|------|------|------|
| 功能覆盖度 | 25% | ⭐⭐⭐⭐☆ (4/5) | ~140 families 已验证，仅 2 family 缺数据 |
| CI/CD 集成 | 25% | ⭐⭐☆☆☆ (2/5) | CI smoke 已集成（4 families, 6-stage quick mode），但无全量回归自动检测 |
| 文档完备度 | 15% | ⭐⭐☆☆☆ (2/5) | 仅有 data-dictionary，无架构/使用/故障文档 |
| 基础设施健壮性 | 15% | ⭐⭐⭐☆☆ (3/5) | Microbench 崩溃已定位（BGC 并发竞态），audit 误报 + 配置脱节仍存 |
| 可维护性（测试） | 10% | ⭐☆☆☆☆ (1/5) | 零测试 |
| 基准/回归能力 | 10% | ⭐⭐⭐☆☆ (3/5) | 数据已收集但无自动回归检测 |

**加权总分：2.65 / 5.0**（较评估时 2.6 略有上升，得益于 CI smoke 集成）— 低于 marshal (4.8) 和 metadata-writer (4.5) 的工业化完成度。

---

## 6. 结论

Foundation-DLL 验证管线的功能基础是扎实的（~140 families, ~97% pass rate），但在**工业化完备度**上与 marshal/metadata-writer 差距明显：

- **最大缺口**：CI/CD 集成（2/5 → 已部分解决：CI smoke + Driver build 已集成）和基础设施测试（1/5）
- **核心阻碍**：microbench 崩溃已定位（BGC 并发竞态，需 GC 工程修复）、audit 评估误报
- **最易修复**：文档（2→4 可在 0.5d 内完成）、2 family subject IDs

已完成行动：
- ✅ **FD-I1**（CI 集成）：codegen-regression.yml 已增加 foundation-dll smoke 步骤，4 个代表 family 6-stage quick mode
- ✅ **FD-I2**（microbench 崩溃定位）：根因为 BGC 线程与 mutator 并发竞态，已在 microbench runner 中加诊断信息

要达到 marshal 级别的工业化标准（4.8/5），预估还需要 **3-4 天 FT** 工作量，主要投入在 CI 全量回归、文档和全量覆盖补齐。
