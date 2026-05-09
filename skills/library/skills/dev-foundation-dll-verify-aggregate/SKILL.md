---
name: dev-foundation-dll-verify-aggregate
description: Define cross-family aggregation strategy and regression detection logic using unified-verification-report.json. Provide guidelines for dashboard refresh.
---

# Foundation DLL Verification — Aggregate & Report

## 概述

所有 family 的验证完成后，聚合结果、检测回归、生成汇总报告。本技能定义聚合策略和回归检测逻辑——实际数据来源于 `unified-verification-report.json`（由 `verify_family()` 自动生成）。

**核心要求**：
- closureStatus 必须考虑所有 required stage 的结果
- Principle VIOLATION 即使所有闸门通过也阻塞 closureStatus
- Dashboard 必须反映 Audit（含 Principle Alignment）结果

## 数据来源

每个 family 的 `unified-verification-report.json` 是聚合主数据源。补充文件：

| 文件 | 用途 |
|------|------|
| `unified-verification-report.json` | 主数据源（stage 状态 + coverage + regression） |
| `mechanism-audit-report.json` | 机制审计详情 |
| `principle-alignment-report.json` | 原则对齐详情 |
| `benchmark-comparison-report.json` | benchmark 详情 |
| `hotupdate-verification-report.json` | hotupdate 详情 |

## 聚合策略

### 1. 跨 family coverage 汇总

从 `unified-verification-report.json` 提取各 family 的 coverage 字段：

```json
{
  "methodCoverage": 1.0,
  "skipRate": 1.0,
  "overall": 1.0
}
```

### 2. 回归检测

每条回归信号记录为：
- **checksum regression** — 之前通过的 checksum 现在不匹配
- **benchmark regression** — managed_faster > 0 或 avg_speedup 显著下降
- **audit regression** — 新增 false-passing 或 principle VIOLATION

回归检测由 `_detect_regression()` 自动执行，调用 `baseline_manager.compare_checksum_baseline()` 和 `compare_benchmark_baseline()`。

### 3. 汇总表

跨 family 汇总表应包含：

```
| Family | Methods | Preflight | Codegen | Fact | Audit | Principle | Benchmark | HotUpdate | Status |
|--------|---------|-----------|---------|------|-------|-----------|-----------|-----------|--------|
| convert-char    | 18 | ✅ | ✅ | 18/18 | ✅ PASS | ✅ P1/P2 | +12.5% | 18/18 | ✅ |
| math-numerics   | 18 | ✅ | ✅ | 18/18 | ✅ PASS | ✅ P1/P2 | +8.3%  | 18/18 | ✅ |
| reflection-type | 43 | ✅ | ✅ | 40/43 | ❌ 2 false-pass | ❌ P2 VIO | n/a | n/a | ❌ |
```

### 4. Dashboard 刷新

```bash
python -c "
from pathlib import Path
import sys
sys.path.insert(0, 'build/toolchains/run/testing')
sys.path.insert(0, 'build/toolchains/run')
from foundation_dll_audit_generator import write_foundation_dll_audit_outputs
result = write_foundation_dll_audit_outputs(Path('.').resolve())
print(f'Dashboard refreshed: {len(result[\"artifacts\"])} artifacts')
"
```

**验证 dashboard 包含 audit 数据**：
```bash
python -c "
import json
from pathlib import Path
fv_path = Path('verification/projections/foundation-dll-audit/family-verification.json')
if fv_path.exists():
    fv = json.loads(fv_path.read_bytes())
    audit_families = sum(1 for asm in fv.get('assemblies',[]) for fam in asm.get('families',[]) if fam.get('mechanismAuditStatus'))
    print(f'Dashboard: Audit present in {audit_families} families')
    if audit_families == 0:
        print('WARNING: Audit data missing from dashboard')
"
```

## 异常处理

- 缺少 unified-verification-report.json → 该 family 标记为 `verify:pending`，先跑 `verify_family()`
- 缺少 benchmark/hotupdate 报告 → 以 WARN 标记但继续（仅 advisory 阶段）
- Principle VIOLATION → **阻塞 closureStatus**（除非有有效 waiver）
- False-passing > 0 → **自动阻塞 closureStatus**

## 产出

| 产物 | 路径 | 说明 |
|------|------|------|
| 汇总报告 | `verification/foundation-dll/<Assembly>/reports/aggregate-report.json` | 跨 family 机器可读 JSON |
| 刷新后 dashboard | `docs/verification/foundation-dll-audit/` | HTML 报告 |
| 回归信号列表 | stdout | 自动检测 |

### aggregate-report.json 格式

```json
{
  "generatedAt": "2026-05-06T...",
  "assembly": "System.Private.CoreLib",
  "families": [
    {
      "slug": "convert-char",
      "methods": 18,
      "unifiedReport": { "overall_status": "passed", "coverage": {...} },
      "gates": {
        "fact":       {"status": "passed", "passed": 18, "total": 18},
        "audit":      {"status": "passed", "falsePassing": 0, "principle": "ALIGNED"},
        "benchmark":  {"status": "passed", "avgSpeedupPercent": 12.5},
        "hotupdate":  {"status": "passed", "passed": 18, "total": 18}
      },
      "closureStatus": "closed"
    }
  ],
  "regressions": []
}
```

## 关联

- 主数据源: `unified-verification-report.json`（来自 `verify_family()`）
- 上游: `dev-foundation-dll-verification-pipeline`
- Dashboard Schema: `verification/projections/foundation-dll-audit/schema/family-verification.schema.json`
- 审计详情: `mechanism-audit-report.json`, `principle-alignment-report.json`