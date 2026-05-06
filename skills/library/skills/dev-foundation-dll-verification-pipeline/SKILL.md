---
name: dev-foundation-dll-verification-pipeline
description: Master orchestrator for foundation DLL verification — codegen → data integrity → mechanism audit → three-gate → aggregation → dashboard refresh. Enforces Unity IL2CPP behavioral parity and no-skip policy.
---

# Foundation DLL Verification Pipeline

## 概述

完整的 foundation DLL 验证管线编排。从 codegen 管线开始，经过数据校验、**Mechanism Audit**、三闸门验证（含 L2 checksum 验证和 Unity IL2CPP 参考对比）、结果聚合到 dashboard 刷新。

**核心原则变更**：
1. L2 Checksum 验证不再是可选项 — 所有 family 必须通过 L2 才能 claim "native-proof passed"
2. Mechanism Audit (M1+) 确保每个 passed 结果基于真实翻译而非 stub/skip，**并在机制验证中加入三条全局原则的对齐检查**
3. No-Skip Policy — 不可通过新增特例来绕过测试失败
4. Handwrite 用例与 auto-generated 用例共享同一验证管道
5. Dashboard 必须反映 L2 + M1+ 结果，不仅限于 L1
6. **Principle Alignment — closureStatus 现在要求 overallAlignment == "ALIGNED"，即使所有闸门通过，原则违规也阻塞完成**

**入口信号**：`dev-foundation-dll-verification-pipeline`

## 前置条件

- 目标 family 的 managed 代码已实现（翻译路径已写入 codegen）
- 当前目录为仓库根目录
- Skip Registry 已初始化（`verification/projections/foundation-dll-audit/skip-registry.json`），所有例外未过期（自动生成：`skip_registry_auto.py --generate`）
- Mechanism Audit 脚本就绪（`build/toolchains/run/testing/foundation_dll/mechanism_audit.py`）
- Principle Auto-Checks 就绪（`build/toolchains/run/testing/foundation_dll/principle_auto_checks.py`）

## 管线步骤

```
Step 0: Codegen Pipeline（如未运行）
  │  运行 batch_native_aot_runner.py
  │  ├── 为每个 family 生成 entrypoint C#
  │  ├── chaos-il2cpp convert → aot-core-ir.json
  │  ├── chaos-il2cpp emit-native-aot → native-aot.generated.cpp
  │  └── native-codegen → native-reference.runtime-skeleton.coverage.json
  │
  │  ⚠ 必须确认 coverage JSON 已生成，否则 Step 1 的 integrity check 会失败
  ▼
Step 1: Data Integrity
  │  dev:foundation-dll-verify-data-integrity
  │  ├── Claims 完整性（methodSubjectIds）
  │  ├── Claims vs Ledger 一致性
  │  ├── Coverage Evidence 存在性
  │  ├── Dashboard Gate 状态一致性
  │  ├── Mechanism Integrity（新增）— 无 false-passing 信号
  │  └── Skip Registry 审计（新增）— 所有例外未过期
  │
  │  ⚠ 如果 coverage 缺失 → 回退 Step 0
  │  ⚠ 如果 claims 为空 → regenerate → 重试
  │  ⚠ 如果 Skip Registry 有过期条目 → 审查通过前不继续
  ▼
Step 2: Skip Audit（自动执行）
  │  自动审计 _METHOD_OVERRIDES：
  │  ├── `skip_registry_auto.py --validate` — 自动验证所有 skip 已在 registry 中登记
  │  ├── `skip_registry_auto.py --generate` — 自动从 _METHOD_OVERRIDES 生成 registry
  │  ├── 自动检查 review date 是否过期（对比今天日期）
  │  └── 自动检测未登记的新 skip 条目
  │
  │  输出：skip-registry.json（自动生成）
  │  通过条件：所有 skip 已登记、review date 未过期
  │  阻塞条件：存在未登记的 skip（自动检测，无需人工审查）
  ▼
Step 3: Select Families
  │  根据验证目的选择 family 列表：
  │  - 基准回归: convert-char, math-numerics
  │  - 完整验证: 所有 33 families
  │  - 定点验证: <按需指定>
  ▼
Step 4: Three-Gate Verification (per family)
  │  dev:foundation-dll-family-verification
  │  ├── Fact L1:    Codegen success (AOT lowering compiled)
  │  ├── Fact L2:    Semantic correctness (native exec checksum match)
  │  ├── M1+ Audit:  Mechanism + Principle verification (no false-passing + principle alignment)
  │  ├── M2 Parity:  Unity IL2CPP checksum parity（当参考管道可用时）
  │  ├── Benchmark:  native exe + managed baseline（含 Unity IL2CPP 基线，如可用）
  │  └── HotUpdate:  native hotupdate exe
  │
  │  ⚠ L2 必须通过，M1+ 必须通过，才能 claim "native-proof passed"
  │  ⚠ Benchmark 和 HotUpdate 必须参与 closureStatus 计算
  │  ⚠ 任何闸门失败 → 先查看 trace 定位根因，再决定继续还是停止
  │  ⚠ 禁止通过新增 skip 绕过失败
  │  ⚠ Principle VIOLATION 即使所有闸门通过也阻塞 closureStatus
  │
  │  输出：principle-alignment-report.json（每个 family）
  ▼
Step 4a: Principle Alignment Review（自动执行）
  │  自动执行 7 项自动化原则检查（principle_auto_checks.py）：
  │  ├── `principle_auto_checks.py --family <slug>` — 自动运行 7 项自动化检查
  │  ├── P1: lowering / redundant_branch / indirection / benchmark
  │  ├── P2: template_hygiene（Scriban 模板扫描）
  │  ├── P3: patch_entry / patchdata
  │  ├── `waiver_expiry_checker.py --block-expired` — 自动检查 waiver 过期
  │  └── 自动标记 overallAlignment 并写入 closureStatus 判断
  │
  │  输出：principle-alignment-report.json（自动生成）
  │  通过条件：overallAlignment == "ALIGNED"（无 VIOLATION）
  │  阻塞条件：overallAlignment == "VIOLATION" 且 waiver 已过期
  │  有条件通过：VIOLATION 但有有效 waiver — 标记 "closed-with-waiver"
  │  人工注意：以下 4 项 P2 检查不可自动阻塞，仅记录趋势供 dashboard 监控：
  │    layer_correctness, no_code_duplication, abstraction_balance, no_hardcoded_state
  ▼
Step 5: Aggregate & Report
  │  dev:foundation-dll-verify-aggregate
  │  ├── 收集各 family 结果（含 L2 + M1+ + Benchmark + HotUpdate + Principle）
  │  ├── 检测回归信号（含 L2 回归、M1 false-passing、Principle VIOLATION）
  │  ├── 生成汇总表（含 L2/M1+/Principle 列）
  │  └── 刷新 Dashboard（写入 L2/M1+/Principle 数据到 dashboard）
  │
  │  ⚠ Dashboard 必须显示 L2 和 M1+，否则 Step 5 不通过
  ▼
Step 6: Final Verification
  │  dev:verification-before-completion
  │  ├── 确认 Dashboard 数据正确反映所有验证门的结果
  │  ├── 确认 Mechanism Audit (M1+) 无 false-passing 且原则对齐
  │  ├── 确认 ledger 已更新（如适用）
  │  ├── 确认 Skip Registry 审查已完成
  │  ├── 确认 Principle Alignment 已审查（waiver 已记录或全对齐）
  │  └── 输出最终状态报告
```

## 前置准备：Codegen Pipeline

如果目标 family 尚未运行 codegen 管线，先补跑：

```bash
cd <REPO_ROOT>
python build/toolchains/run/testing/foundation_dll/batch_native_aot_runner.py \
  --assembly-name System.Private.CoreLib \
  --families convert-char array-indexing-copy ... \
  --trace
```

然后确认 coverage JSON 已生成：

```bash
ls verification/foundation-dll/System.Private.CoreLib/<family>/il2cpp_dist/native-reference.runtime-skeleton.coverage.json
```

## 参数

| 参数 | 说明 | 默认值 |
|------|------|--------|
| `assembly` | 目标 assembly | `System.Private.CoreLib` |
| `families` | family 列表（逗号分隔） | 全部（自动发现） |
| `skip_codegen` | 跳过 codegen 步骤 | `false` |
| `skip_integrity` | 跳过 Step 1 | `false` |
| `skip_skip_audit` | 跳过 Step 2（仅限紧急情况） | `false` |
| `skip_aggregate` | 跳过 Step 5-6 | `false` |
| `enforce_m1` | M1+ Audit 失败是否阻塞 | `true` |
| `enforce_unity_parity` | M2 Unity 校验是否启用 | `false`（待基础设施就绪） |
| `enforce_principle_alignment` | Principle VIOLATION 是否阻塞 closureStatus | `true` |
| `principle_waiver_path` | principle waiver 文件路径 | `verification/projections/foundation-dll-audit/principle-waivers.json` |

## 选择 Family 原则

| 目的 | 推荐组合 | 数量 |
|------|---------|------|
| 快速回归检查 | `convert-char, math-numerics, string-char-text-core` | 3 |
| 完整验证 | 所有 33 families | 33 |
| 定点调试 | 按需指定 | 1-N |

## 失败处理

| 阶段 | 失败模式 | 处理方式 |
|------|---------|----------|
| Step 0 | codegen 失败 | 先 `run trace --exception` 查看失败原因，修复后重跑 |
| Step 1 | claims 为空 | `write_foundation_dll_audit_outputs()` regenerate → 重试 |
| Step 1 | coverage 缺失 | Step 0 未完成 → 回退 codegen |
| Step 1 | Mechanism Integrity 失败 | 定位 false-passing → 修复 stub/skip → 重新验证 |
| Step 1 | Skip Registry 过期 | 审查所有过期条目 → 更新或消除 → 重试 |
| Step 2 | Skip Audit 失败 | `skip_registry_auto.py --validate` 检测到未登记或过期 skip → `--generate` 更新 |
| Step 4 | Fact L1 失败（无 AOT lowering） | 标记为 "not-translated"，该 family AOT 翻译未完成 |
| Step 4 | **Fact L2 失败（checksum 不匹配）** | **阻塞** → **查看 trace** → `dev-systematic-debugging` → 确认翻译语义错误 |
| Step 4 | **M1+ Audit 失败（false-passing）** | **阻塞** → 定位 stub/skip → 实现真实机制 → 重新验证 |
| Step 4 | **Principle VIOLATION** | **阻塞 closureStatus** → 审查违规原因 → 修复或登记 waiver |
| Step 4 | Benchmark 退化 | **查看 trace** → 排查翻译质量 → 标记 regression |
| Step 4 | HotUpdate 失败 | **查看 trace** → 检查 codegen 输出 → 标记失败 |
| Step 4a | **Principle Alignment 审查未通过** | **阻塞** → waiver 缺失或违规无法合理解释 → 修复对齐或补充 waiver |
| Step 5 | Dashboard 不一致 | `write_foundation_dll_audit_outputs()` 重新生成 |
| Step 5 | **Dashboard 缺少 L2/M1+ 数据** | **修复 dashboard 集成 → 重新生成** |

### 闸门失败标准流程

任何闸门失败时：

```bash
# 1. 先查 trace
python -m build.toolchains.run.commands.trace_viewer --exception

# 2. 分析根因（使用 dev-systematic-debugging 方法论）
# 3. 实现真实机制修复（非新增 skip/特例）
# 4. 修复后重跑
# 5. 确认修复后再继续下一 family
```

### Mechanism Audit (M1+) 失败的特殊处理

当 M1+ Audit 在 Step 4 中失败时，按以下流程处理：

```
1. 立即停止该 family 的 Step 4
2. 标记该 family 为 audit:FAILED
3. 定位 false-passing 根因：
   - stub_detector 误判 → 完善检测模式
   - 测试实际通过了 stub 的偶然正确 → 补充 assert intrinsic 确保真实比较
   - skip 隐藏了未实现的方法 → 移除 skip 并实现真实翻译
4. 修复后重新从 Step 4 开始完整验证
5. 仅当 M1+ 通过后才允许继续到 Step 4a
```

### Principle Violation 的特殊处理

当 `principle_auto_checks.py` （Step 4a）发现 VIOLATION 时：

```
1. 自动记录具体违规项（P1/P2/P3 及违规详情）
2. 自动阻断 closureStatus
3. 修复或登记 waiver：
   a. 可修复 → 回到 Step 4 修复后重跑
   b. 不可修复 → 登记 principle_waiver（人工决策，不阻塞流程）
4. `waiver_expiry_checker.py` 自动检查 waiver 过期
5. 有有效 waiver 的 family → "closed-with-waiver"
6. 无 waiver 或 waiver 过期的 family → closureStatus = "blocked"
```

## 职责边界

| 技能 | 负责 | 不负责 |
|------|------|--------|
| `codegen pipeline` | 运行 batch_native_aot_runner | 验证、数据完整性 |
| `verify-data-integrity` | 验证前数据检查（含 Mechanism Integrity 和 Skip Registry） | 修复数据（仅提供修复命令） |
| `family-verification` | 单 family 三闸门 + M1+ Audit（含 Principle Alignment）+ M2 Parity | 数据完整性、跨 family 聚合 |
| `verify-aggregate` | 结果汇总（含 L2/M1+/Principle）、回归检测、dashboard 刷新 | 单 family 验证执行 |
| `verification-pipeline` | 编排、参数传递、流程控制、Skip Audit、Principle Alignment 审查 | 具体验证执行逻辑 |

## 关联

- 前置: 翻译路径实现完成
- Step 0: `batch_native_aot_runner.py` + native codegen
- Step 1: `dev-foundation-dll-verify-data-integrity`
- Step 4: `dev-foundation-dll-family-verification`
- Step 4a: Principle Alignment Review（本技能内执行）
- Step 5: `dev-foundation-dll-verify-aggregate`
- Step 6: `dev-verification-before-completion`
- Skip Registry: `wiki/06-测试验证/foundation-dll-skip-registry.md`
- Handwrite 规范: `wiki/06-测试验证/handwrite-test-guidelines.md`
- Mechanism Audit: `build/toolchains/run/testing/foundation_dll/mechanism_audit.py`
- Principle Waiver: `verification/projections/foundation-dll-audit/principle-waivers.json`
