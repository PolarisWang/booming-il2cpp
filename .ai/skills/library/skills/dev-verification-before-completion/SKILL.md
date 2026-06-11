---
name: dev-verification-before-completion
description: 在即将声称工作完成、已修复或已通过时使用；在提交或创建 PR 之前使用，需要运行验证命令并先确认输出，再做任何成功声明
---

# 完成前验证

## 核心原则

先有证据，后有声明。

如果当前消息里没有运行验证命令并检查输出，就不能声称”已完成 / 已修复 / 已通过”。

### Foundation DLL 专项检查

如果当前工作涉及 foundation DLL 能力族，必须通过以下验证后才能声称完成：

1. **Codegen Pipeline** — `batch_native_aot_runner.py` 通过
2. **Data Integrity** — claims/coverage/ledger 一致
3. **Three-Gate Verification** — Fact + Benchmark + HotUpdate 全部通过
4. **Aggregate & Dashboard** — 报告已生成、dashboard 已刷新

验证命令：

```bash
python -m verification.entry_points.cli <family-slug> --assembly System.Private.CoreLib --mode strict
```

**禁止**：未跑完以上验证就归档 foundation-dll 相关任务。

## 闸门

在声明任何状态之前：

1. 识别：哪条命令能证明这个声明
2. 运行：执行完整命令
3. 阅读：检查退出码、失败数和关键输出
4. 验证：确认输出真的支持该声明
5. 只有这时，才能汇报结果

## 常见错误

- 用”应该””可能””看起来”代替验证结果
- 只跑部分验证就宣称全通过
- 只看运行日志，不刷新 formal source
- 没有 formal object 就归档 `completed`

## Pipeline 失败诊断

如果 Foundation DLL family verification 失败，按以下表格快速定位根因：

| 失败阶段 | 典型标志 | 常见根因 | 排查方向 |
|----------|----------|----------|----------|
| `preflight` | “capability-family-contract.json not found” | contract JSON 缺失或格式错误 | 检查 `family_dir/` 下 contract 文件；`json.load` 是否报错 |
| `codegen` | “Build error” / “MSBuild failed” | IL 注入失败、codegen 模板语法错误、Scriban 解析异常 | 检查 codegen/build-output/ 下的构建日志；重跑 `--verbose` |
| `jit_codegen` | “JIT codegen failed” | JIT 模式 codegen 输出路径问题，或 SDK 路径解析失败 | 检查 `entry-jit.exe` 是否生成、SDK 版本是否匹配 |
| `fact` | “Fact X failed” / “exit code non-zero” | Native 构建时断言失败、entry-aot.exe 运行时崩溃 | 用 `--native-config check` 重新构建；检查 entry-aot.exe 输出 |
| `audit` | “Mechanism violation” / “Principle violation” | Codegen 输出不符合 IL2CPP 翻译规范 | 检查具体 violation 详情；确认 codegen 翻译路径正确 |
| `asm_compare` | “asm pass rate < 100%” | AOT vs JIT 汇编差异超出阈值 | 检查 asm_compare 详情 JSON，定位差异方法 |
| `benchmark` | “OpsPerSecond too low” / “slowdown > 20%” | 性能回退，与 .NET 8 基线差距过大 | `check-net8-slowdown.sh` 查看具体比率；profile 模式采集热点 |
| `hotupdate` | “semantic_changed_count == 0” | 热更新方法语义未正确标记变更 | 检查 patch 生成是否正确；hotupdate SubjectId 映射 |
| `timeout` | “Timed out after Ns” | 阶段被 `--timeout` 强制中止，通常是卡在 native 构建或 dotnet restore | 检查日志看卡在哪一步；增加超时重试 |

如果多个 stage 同时失败，通常根因在靠前的 stage（preflight/codegen）—— 修复后重跑即可。

## Post-Pipeline 验证

Pipeline 跑完后，对通过的家庭执行以下三项验证确认数据质量：

### 验证项

| 检查 | 脚本 | 说明 |
|------|------|------|
| 基准计时 | `run_family_validations()` | 确认所有 benchmark 的 `elapsedMilliseconds > 0` |
| .NET 8 性能偏差 | `run_family_validations()` | 确认 AOT/JIT 对比 .NET 8 的 slowdown ≤ 20% |
| HotUpdate 完整性 | `run_family_validations()` | 确认 `semantic_changed_count > 0` 且 overhead ≤ 100% |

### 触发方式

**自动（batch 模式）**：加 `--validate` 参数，跑完每个 family 后自动执行对应检查：

```bash
python -m verification.entry_points.batch --validate
```

**手动（单 family）**：验证函数可直接调用：

```python
from verification.entry_points.batch import run_family_validations
failures = run_family_validations("convert-char")
assert not failures, failures
```

## 项目绑定

- 项目级测试知识以 [`wiki/06-测试验证/INDEX.md`](../../../wiki/06-测试验证/INDEX.md) 为正式入口
- selector / registry 信息优先读 `artifacts/tests/registry/current/index.json` 或执行 `run test registry list --json`
- 如果 registry 给出 `canonicalCommand`，优先直接执行
- 如果用户明确要求“新的验证通过记录 / 新的数据”，或任务命中 `dev-project-test-governance` verification 主线，则 regression 通过后还必须刷新 formal source；默认执行 `run test inventory --json`
- `run test inventory` 不是 public verification entry
- `benchmark --record` 不等于 formal refresh；需要新的 benchmark archive / projection / merged data 时，仍要执行 `run test inventory --json`
- 只有 `summaryPath` / `eventsPath` / `consolePath`，但没有 `verification/archive/{latest,master,reports}` 或 `verification/evidence/owners/*/codegen-stubs/*` 更新时，不足以声称“新测试流程已打通”
- `docs/dev/**` 只是任务过程记录，不是长期 authority

## Foundation DLL family verification 补充约束

- 如果本轮改动触及 foundation-dll family verification authority、projection、dashboard 或 detail 消费链路，完成前必须判断是否应该刷新：
  - `docs/verification/foundation-dll-audit/family-verification-claims.json`
  - `docs/verification/foundation-dll-audit/family-verification.json`
- 典型命中点包括：
  - `testing/foundation-dll/verification/tooling/derive.py`
  - `build/toolchains/run/testing/foundation_dll_audit_generator.py`
  - `Native Proof Detail`、family progress、tooltip detail
- 命中时，默认需要执行 `run test inventory --json`，而不是只改 HTML 或局部 JSON
- 完成前至少核对：
  - `docs/verification/foundation-dll-audit/family-verification-claims.json`
  - `docs/verification/foundation-dll-audit/family-verification.json`
  - `docs/verification/foundation-dll-audit/dlls/*.json`

## CodeGen 快照测试专项检查

如果本轮改动涉及 codegen (emitter / planner / RuntimeSupport / Scriban 模板)，完成前必须运行快照测试并确认以下事项：

- 运行 `dotnet test tests/snapshots/Chaos.IL2CPP.Generator.SnapshotTests/` 是否通过
- 如果基线有变更，是否已通过 `git diff` 审查
- 新增 IL 模式时是否补了对应夹具和基线
- 基线更新是否使用了 `SNAPSHOT_UPDATE=1` 模式

快照测试未通过不得声称 codegen 改动完成。

详见 [`wiki/06-测试验证/CodeGen快照测试规范.md`](../../../wiki/06-测试验证/CodeGen快照测试规范.md)。

## 结构告警与架构审视

在声称”可以 completed”之前，除了 formal verification 与受影响测试，还必须完成最新一轮结构审视（检查项同 `dev-executing-plans` 中的定义）。未解决的 `blocker` 会阻止完成声明。

## Obligation-driven 任务

如果任务命中以下任一字段或明显属于 AOT / IL2CPP / test governance 主线，则按 obligation-driven 模式验证（obligation 字段列表与确认优先级同 `dev-executing-plans` 中的定义）：

## `dotnet` 编译崩溃闸门

只要 `dotnet build` / `dotnet test` / `msbuild` 崩溃：

- 当前验证立即视为失败
- **先重试一次**（部分崩溃是 OOM 或磁盘抖动导致）
- 重试仍崩溃时：
  - 必须保留 `stderr`、`binlog`、堆栈或 dump
  - 必须查明并修复根因后重跑
  - 根因未修复前，不得宣称通过

## 结果汇报

汇报验证结果时，至少给出：

- `canonicalCommand`
- `summaryPath`
- `eventsPath`
- `consolePath`

如果本轮要求新的 verification 数据，还要给出：

- `refreshCommand`
- `verificationOutputRoot`
- `latestResultPath`
- `masterResultPath`
- `reportSummaryPath`
- `codegenStubPath`

建议使用最小证据卡：

```markdown
## 完成证据
- arch_review: [ok / blocker-fixed / n/a]
- authority_review: [ok / n/a]
- verification:
  - canonicalCommand: [...]
  - formalObject: [...]
  - summaryPath: [...]
  - eventsPath: [...]
  - consolePath: [...]
- verification_data:
  - refreshCommand: [... / n/a]
  - verificationOutputRoot: [... / n/a]
  - latestResultPath: [... / n/a]
  - masterResultPath: [... / n/a]
  - reportSummaryPath: [... / n/a]
  - codegenStubPath: [... / n/a]
- test_result: [passed / failed]
- wiki: [updated:<path> / n/a]
```

如果当前不涉及 verification data 刷新，可以使用简化版：

```markdown
## 完成证据（简化）
- arch_review: [ok / blocker-fixed / n/a]
- verification: [命令已执行 / 输出已确认]
- test_result: [passed / failed]
- summaryPath: <path>
```

## 底线

运行命令。阅读输出。然后再声明结果。

## 进化系统健康检查（新增）

如果本轮改动触及 skills/ 下的技能内容或进化系统，完成前运行以下检查：

```
## 进化系统验证清单
[ ] python skills/tooling/learning/health_engine.py report --window 30
     — 确认无异常指标（LOW-TOOL、HIGH-FALLBACK 等）
[ ] python skills/tooling/learning/evolve.py propose --dry-run
     — 确认无意外进化提案
[ ] python skills/tooling/verification/verify-skill-pipeline.ps1
     — 确认 junction、manifest、catalog、evolution 目录一致
```

### 每日/每周维护

```powershell
# 计算健康指标
python skills/tooling/learning/health_engine.py compute --all --window 30

# 生成报告
python skills/tooling/learning/health_engine.py report --window 30

# 预览进化提案
python skills/tooling/learning/evolve.py propose --dry-run

# 执行进化（需要 review 后 promote）
python skills/tooling/learning/evolve.py auto-evolve
```

## Dashboard 数据约束检查（新增）

如果本轮改动触及以下任一文件，完成前必须运行 dashboard 数据约束检查：

- `build/toolchains/run/testing/foundation_dll_audit_generator.py`
- `testing/foundation-dll/verification/tooling/derive.py`

命中时执行以下检查：

```
## Dashboard 验证清单
[ ] 运行 foundation_dll_audit_generator.py — 重新生成输出工件
[ ] 运行 validate-output-schemas.py — 所有输出符合 JSON Schema
[ ] 运行 verify-dashboard-field-coverage.py — 字段覆盖率 >= 95%
[ ] 注册表 diff 检查 — provenance-registry.md 与 committed baseline 差异审查
[ ] 新增字段已在数据字典中登记（wiki 文档）
[ ] 新增 enum 值已在枚举值字典中登记
[ ] tooltip Data Provenance 信息准确（如涉及 UI 变更）
```
