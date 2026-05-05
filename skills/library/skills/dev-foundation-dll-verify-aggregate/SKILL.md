---
name: dev-foundation-dll-verify-aggregate
description: Aggregate per-family verification results, detect regressions, produce consolidated report. TRIGGER after all families complete verification, or when asked to refresh verification dashboard or generate summary report.
---

# Foundation DLL Verification — Aggregate & Report

## 概述

所有 family 的三闸门验证完成后，聚合结果、检测回归、生成汇总报告。

**入口信号**：`dev:foundation-dll-verify-aggregate`

**前置**：一个或多个 family 已完成 Fact/Benchmark/HotUpdate 验证

## 聚合步骤

### 1. 收集各 family 验证结果

从每个 family 的验证产物目录收集结果：

```bash
python -c "
from pathlib import Path
import json, sys

families = sys.argv[1:] if len(sys.argv) > 1 else []
if not families:
    # Auto-discover families with verification results
    base = Path('verification/foundation-dll/System.Private.CoreLib')
    families = sorted(d.name for d in base.iterdir() if d.is_dir() and (d / 'benchmark-comparison-report.json').exists())

for fam in families:
    print(f'\\n=== {fam} ===')
    # Fact results: check test TRX or summary
    test_summary = Path(f'verification/foundation-dll/System.Private.CoreLib/{fam}/managed_test/tests')
    if test_summary.exists():
        print(f'  Fact: tests dir exists')
    
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

```bash
python -c "
import json
from pathlib import Path

signals = []
base = Path('verification/foundation-dll/System.Private.CoreLib')
for fam in sorted(base.iterdir()):
    if not fam.is_dir():
        continue
    bm = fam / 'benchmark-comparison-report.json'
    if bm.exists():
        data = json.loads(bm.read_bytes())
        s = data.get('summary', {})
        mf = s.get('managedFasterCount', 0)
        if mf > 0:
            signals.append(f'REGRESSION: {fam.name}: {mf} methods where native is slower than managed')
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
- `managed_faster > 0` — 部分方法 native 比 managed 慢，需排查翻译质量
- `avg_speedup < -5%` — 整体 native 比 managed 慢，可能是新增翻译路径有性能问题
- HotUpdate failed > 0 — 热更新机制有问题，需排查 stub 返回值

### 3. 生成汇总表

```
| Family | Methods | Fact | Benchmark Avg Speedup | HotUpdate | Status |
|--------|---------|------|---------------------|-----------|--------|
| convert-char | 18 | 18/18 | +12.5% | 18/18 | ✅ PASS |
| math-numerics | 18 | 18/18 | +8.3% | 18/18 | ✅ PASS |
| ... | | | | | |
```

### 4. 刷新 Dashboard

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

### 5. 验证 Dashboard 数据

打开每个已验证 family 的 fact/benchmark/hotupdate 页面，确认数据正确反映验证结果。

```bash
# 验证 fact 通过率与测试结果一致
python -c "
import re
with open('docs/verification/foundation-dll-audit/dlls/System.Private.CoreLib/families/{family}-fact.html') as f:
    c = f.read()
import re; m = re.search(r'Fact Pass Rate\s*(\d+)/(\d+)', c)
if m: print(f'Fact: {m.group(1)}/{m.group(2)}')
"
```

## 输出

| 产物 | 路径 | 说明 |
|------|------|------|
| 汇总报告 | `verification/foundation-dll/System.Private.CoreLib/reports/aggregate-report.json` | 机器可读的 JSON |
| 刷新后 dashboard | `docs/verification/foundation-dll-audit/` | HTML 报告 |
| 回归信号列表 | 输出到 stdout | 人工审查 |

## 异常处理

- 如果某个 family 缺少 benchmark/hotupdate 结果，以 WARN 标记但继续
- 如果 dashboard 刷新后数据与验证结果不一致，停止并提示修复
- 回归信号需要人工确认后再标注为已知问题

## 关联

- 父流程: `dev:foundation-dll-verification-pipeline`
- 上游: `dev:foundation-dll-family-verification`
