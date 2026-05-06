---
name: dev-foundation-dll-verify-aggregate
description: Aggregate per-family verification results (including L2, Mechanism Audit, Benchmark, HotUpdate), detect regressions, produce consolidated report with Unity IL2CPP parity tracking.
---

# Foundation DLL Verification — Aggregate & Report

## 概述

所有 family 的三闸门验证完成后，聚合结果、检测回归、生成汇总报告。

> **重要变更**：Dashboard 现在必须反映 L2（语义正确性）、M1+（Mechanism + Principle Audit）的结果，而不仅限于 L1（Codegen Success）。
> Benchmark 和 HotUpdate 结果必须参与 "completed" 状态判断。
> **closureStatus 的判断新增原则对齐要求** — 即使所有门通过，如果原则对齐状态为 VIOLATION，closureStatus 必须为 "blocked"。

**入口信号**：`dev-foundation-dll-verify-aggregate`

**前置**：一个或多个 family 已完成 Fact L1 + L2 + M1+ / Benchmark / HotUpdate 验证

## 聚合步骤

### 1. 收集各 family 验证结果

从每个 family 的验证产物目录收集结果，**包括新增的 M1+ Audit 报告（含原则对齐）**：

```bash
python -c "
from pathlib import Path
import json, sys

families = sys.argv[1:] if len(sys.argv) > 1 else []
if not families:
    base = Path('verification/foundation-dll/System.Private.CoreLib')
    families = sorted(d.name for d in base.iterdir() if d.is_dir() and (d / 'mechanism-audit-report.json').exists())

for fam in families:
    print(f'\\n=== {fam} ===')
    # M1+ Audit (primary gate — mechanism + principle)
    audit = Path(f'verification/foundation-dll/System.Private.CoreLib/{fam}/mechanism-audit-report.json')
    if audit.exists():
        data = json.loads(audit.read_bytes())
        print(f'  M1+ Audit: {\"PASS\" if data.get(\"passed\",False) else \"FAIL\"} '
              f'total={data.get(\"total_methods\",0)} false_passing={data.get(\"false_passing\",0)}')
        # Principle alignment
        pa = data.get('principle_summary', {})
        if pa:
            p1 = pa.get('performance', {})
            p2 = pa.get('architectural_perfection', {})
            p3 = pa.get('hotupdate_compatibility', {})
            print(f'  Principle: P1={p1.get(\"aligned\",0)}ALIGN/{p1.get(\"violation\",0)}VIO '
                  f'P2={p2.get(\"aligned\",0)}ALIGN/{p2.get(\"violation\",0)}VIO '
                  f'P3={p3.get(\"aligned\",0)}ALIGN/{p3.get(\"violation\",0)}VIO')
            print(f'  Overall: {\"ALIGNED\" if data.get(\"overall_alignment\")==\"ALIGNED\" else \"VIOLATION\"}')
    if audit.exists():
        data = json.loads(audit.read_bytes())
        print(f'  M1 Audit: {\"PASS\" if data.get(\"passed\",False) else \"FAIL\"} '
              f'total={data.get(\"total_methods\",0)} false_passing={data.get(\"false_passing\",0)}')
    else:
        print(f'  M1 Audit: MISSING (family cannot be marked completed without audit)')
    
    # Fact L2 results
    l2 = Path(f'verification/foundation-dll/System.Private.CoreLib/{fam}/managed_test/tests/l2-results.json')
    if l2.exists():
        data = json.loads(l2.read_bytes())
        print(f'  Fact L2: passed={data.get(\"passed\",0)}/{data.get(\"total\",0)}')
    
    # Benchmark comparison
    bm = Path(f'verification/foundation-dll/System.Private.CoreLib/{fam}/benchmark-comparison-report.json')
    if bm.exists():
        data = json.loads(bm.read_bytes())
        s = data.get('summary', {})
        print(f'  Benchmark: avg_speedup={s.get(\"averageSpeedupPercent\",\"n/a\")}% '
              f'native_faster={s.get(\"nativeFasterCount\",0)} managed_faster={s.get(\"managedFasterCount\",0)}')
    
    # HotUpdate
    hu = Path(f'verification/foundation-dll/System.Private.CoreLib/{fam}/hotupdate-verification-report.json')
    if hu.exists():
        data = json.loads(hu.read_bytes())
        s = data.get('summary', {})
        print(f'  HotUpdate: passed={s.get(\"passedMethods\",0)} failed={s.get(\"failedMethods\",0)}')
" --families
"
```

### 2. 检测回归信号

增强回归检测，包含 M1 和 L2 回归：

```bash
python -c "
import json
from pathlib import Path

signals = []
base = Path('verification/foundation-dll/System.Private.CoreLib')
for fam in sorted(base.iterdir()):
    if not fam.is_dir():
        continue
    
    # M1+ Audit regression (mechanism + principle)
    audit = fam / 'mechanism-audit-report.json'
    if audit.exists():
        data = json.loads(audit.read_bytes())
        if not data.get('passed', False):
            signals.append(f'AUDIT FAILURE: {fam.name}: mechanism audit failed ({data.get(\"false_passing\",0)} false passes)')
        if data.get('audit_uncovered_methods', 0) > 0:
            signals.append(f'AUDIT GAP: {fam.name}: {data[\"audit_uncovered_methods\"]} methods not covered by audit')
        # Principle alignment regression
        alignment = data.get('overall_alignment', 'ALIGNED')
        if alignment != 'ALIGNED':
            pa = data.get('principle_summary', {})
            details = []
            for p_name, p_data in [('P1', pa.get('performance', {})), ('P2', pa.get('architectural_perfection', {})), ('P3', pa.get('hotupdate_compatibility', {}))]:
                if p_data.get('violation', 0) > 0:
                    details.append(f'{p_name}={p_data[\"violation\"]}VIO')
            signals.append(f'PRINCIPLE VIOLATION: {fam.name}: overall={alignment} {\" \".join(details)}')
    
    # L2 regression (previously passing method now fails)
    l2 = fam / 'managed_test/tests/l2-results.json'
    if l2.exists():
        data = json.loads(l2.read_bytes())
        if data.get('failed', 0) > 0:
            signals.append(f'L2 REGRESSION: {fam.name}: {data[\"failed\"]}/{data[\"total\"]} checksums mismatch')
    
    # Benchmark regression
    bm = fam / 'benchmark-comparison-report.json'
    if bm.exists():
        data = json.loads(bm.read_bytes())
        s = data.get('summary', {})
        mf = s.get('managedFasterCount', 0)
        if mf > 0:
            signals.append(f'BENCHMARK REGRESSION: {fam.name}: {mf} methods where native is slower than managed')
        avg = s.get('averageSpeedupPercent', 0)
        if isinstance(avg, (int, float)) and avg < -5:
            signals.append(f'PERF: {fam.name}: average speedup {avg}% (native slower overall)')

if signals:
    print('\\n'.join(signals))
else:
    print('No regressions detected')
"
```

**回归分类**：
- `AUDIT FAILURE` — mechanism audit 发现 false-passing，**阻塞**
- `AUDIT GAP` — 部分方法未经过 audit，需要补全
- `PRINCIPLE VIOLATION` — 修复/实现违反三条原则之一，**阻塞**
- `L2 REGRESSION` — 之前通过的 checksum 现在不匹配，**阻塞**
- `managed_faster > 0` — 部分方法 native 比 managed 慢，需排查翻译质量
- `avg_speedup < -5%` — 整体 native 比 managed 慢，可能是新增翻译路径有性能问题
- HotUpdate failed > 0 — 热更新机制有问题，需排查 stub 返回值

### 3. 生成汇总表

增强汇总表，包含 M1+ Audit 列和 Principle Alignment 列：

```
| Family | Methods | L1 Codegen | L2 Checksum | M1+ Audit | Principle | Benchmark | HotUpdate | Status |
|--------|---------|-----------|-------------|-----------|-----------|-----------|-----------|--------|
| convert-char | 18 | ✅ 18/18 | ✅ 18/18 | ✅ PASS | ✅ P1/P2 | +12.5% | 18/18 | ✅ |
| math-numerics | 18 | ✅ 18/18 | ⬜ pending | ⬜ pending | ⬜ | +8.3% | 18/18 | ⏳ |
| reflection-type | 43 | ✅ 43/43 | ⚠️ 40/43 | ❌ 2 false-pass | ❌ P2 VIO | n/a | n/a | ❌ |
```

### 4. 刷新 Dashboard

Dashboard 刷新时**必须写入** L2 和 M1 数据，不能只写入 L1：

```bash
python -c "
from pathlib import Path
import sys
sys.path.insert(0, 'build/toolchains/run/testing')
sys.path.insert(0, 'build/toolchains/run')
from foundation_dll_audit_generator import write_foundation_dll_audit_outputs
result = write_foundation_dll_audit_outputs(Path('.').resolve())
print(f'Dashboard refreshed: {len(result[\"artifacts\"])} artifacts')

# 验证 dashboard 包含 L2 + M1 数据
import json
fv_path = Path('verification/projections/foundation-dll-audit/family-verification.json')
if fv_path.exists():
    fv = json.loads(fv_path.read_bytes())
    l2_families = sum(1 for asm in fv.get('assemblies',[]) for fam in asm.get('families',[]) if fam.get('l2ChecksumPassRate'))
    m1_families = sum(1 for asm in fv.get('assemblies',[]) for fam in asm.get('families',[]) if fam.get('mechanismAuditStatus'))
    print(f'Dashboard data: L2 present in {l2_families} families, M1 present in {m1_families} families')
    if l2_families == 0:
        print('WARNING: L2 data missing from dashboard — verify integration')
    if m1_families == 0:
        print('WARNING: M1 data missing from dashboard — verify integration')
"
```

### 5. 验证 Dashboard 数据

打开每个已验证 family 的 fact/benchmark/hotupdate 页面，确认数据正确反映验证结果。

```bash
# 验证 L2 通过率与测试结果一致
python -c "
import json
with open('docs/verification/foundation-dll-audit/dlls/System.Private.CoreLib/families/{family}-fact.html') as f:
    c = f.read()
import re; m = re.search(r'L2 Pass Rate\s*(\d+)/(\d+)', c)  # 注意: 现在是 L2 Pass Rate 而非 Fact Pass Rate
if m: print(f'L2: {m.group(1)}/{m.group(2)}')
"
```

## Dashboard 数据映射（新增）

| 验证门 | Dashboard 字段 | 来源文件 |
|--------|---------------|---------|
| L1 Codegen | `codegenSuccessRate` | `batch-native-aot-pipeline-results.json` |
| L2 Checksum | `l2ChecksumPassRate` | `l2-results.json` |
| M1+ Audit | `mechanismAuditStatus` + `principleAlignment` | `mechanism-audit-report.json` |
| Benchmark | `benchmarkAvgSpeedup` | `benchmark-comparison-report.json` |
| HotUpdate | `hotupdatePassRate` | `hotupdate-verification-report.json` |
| Overall | `closureStatus` | 以上全部聚合 + 原则对齐 |

## 输出

| 产物 | 路径 | 说明 |
|------|------|------|
| 汇总报告 | `verification/foundation-dll/System.Private.CoreLib/reports/aggregate-report.json` | 机器可读的 JSON，包含 L2 + M1+ 数据 |
| 刷新后 dashboard | `docs/verification/foundation-dll-audit/` | HTML 报告（必须显示 L2 和 M1+） |
| 回归信号列表 | 输出到 stdout | 自动检测（无人工审查） |

### aggregate-report.json 格式

```json
{
  "generatedAt": "2026-05-06T...",
  "assembly": "System.Private.CoreLib",
  "families": [
    {
      "slug": "convert-char",
      "methods": 18,
      "gates": {
        "l1Codegen": {"status": "passed", "passed": 18, "total": 18},
        "l2Checksum": {"status": "passed", "passed": 18, "total": 18},
        "m1PlusAudit": {"status": "passed", "falsePassing": 0, "audited": 18},
        "benchmark": {"status": "passed", "avgSpeedupPercent": 12.5},
        "hotupdate": {"status": "passed", "passed": 18, "total": 18}
      },
      "principleAlignment": {
        "performance": {"status": "ALIGNED", "aligned": 18, "violation": 0},
        "architecturalPerfection": {"status": "ALIGNED", "aligned": 18, "violation": 0},
        "hotupdateCompatibility": {"status": "NOT_APPLICABLE"},
        "overall": "ALIGNED"
      },
      "closureStatus": "closed"
    }
  ],
  "regressions": []
}
```

## 异常处理

- 如果某个 family 缺少 M1+ Audit 报告，标记为 `audit:pending`，不参与 completed 状态计算
- 如果某个 family 缺少 benchmark/hotupdate 结果，以 WARN 标记但继续（仅当该闸门标记为 `not-required` 时才允许通过）
- 如果 L2 数据未集成到 dashboard（dashboard 中 L2 字段为空），**停止并提示修复 dashboard 集成**
- 回归信号自动检测（`waiver_expiry_checker.py`），无需人工确认
- **如果 Mechanism Audit 发现任何 false-passing，自动阻塞该 family 的 completed 状态**
- **如果 principle_auto_checks.py 返回 VIOLATION，自动阻塞 closureStatus**

## 关联

- 父流程: `dev-foundation-dll-verification-pipeline`
- 上游: `dev-foundation-dll-family-verification`
- Dashboard Schema: `verification/projections/foundation-dll-audit/schema/family-verification.schema.json`
