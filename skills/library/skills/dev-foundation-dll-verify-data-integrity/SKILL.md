---
name: dev-foundation-dll-verify-data-integrity
description: Validate foundation DLL dashboard data consistency — claims, ledger, coverage evidence, and mechanism integrity — before running verification gates.
---

# Foundation DLL Data Integrity Verification

## 概述

在运行三闸门验证之前，先确认 dashboard 底层数据是正确的。避免"测试全绿但 dashboard 显示为 0"的问题。

**新增检查维度**：Mechanism Integrity — 验证 claims 中的通过率是否基于真实机制，而非 skip 或 stub 的假通过。

**入口信号**：`dev-foundation-dll-verify-data-integrity`

## 检查项

### 1. Claims 完整性

```bash
python -c "
import json
claims = json.load(open('verification/projections/foundation-dll-audit/family-verification-claims.json'))
for asm in claims['assemblies']:
    native = [c for c in asm.get('claims',[]) if c['gateCode']=='native-proof']
    with_mids = [c for c in native if c.get('methodSubjectIds')]
    if len(native) != len(with_mids):
        print(f'ISSUE: {asm[\"assemblyName\"]}: {len(native)} native-proof claims, {len(with_mids)} have methodSubjectIds')
        print(f'  -> Run: python -c \"from foundation_dll_audit_generator import write_foundation_dll_audit_outputs; write_foundation_dll_audit_outputs(Path(chr(46).resolve()))\"')
"
```

**通过标准**：所有 assembly 的 native-proof claims 都有 `methodSubjectIds`。

### 2. Claims vs Ledger 一致性

```bash
python -c "
import json
from pathlib import Path
claims = json.loads(Path('verification/projections/foundation-dll-audit/family-verification-claims.json').read_bytes())
ledger = json.loads(Path('verification/projections/foundation-dll-audit/capability-family-ledger.json').read_bytes())
ledger_by_asm = {d['assemblyName']: d for d in ledger['dlls']}
mismatches = 0
for asm in claims['assemblies']:
    aname = asm['assemblyName']
    ledger_asm = ledger_by_asm.get(aname, {})
    ledger_fams = {f['familyId']: f.get('methodCount', 0) for f in ledger_asm.get('families', [])}
    for c in asm.get('claims', []):
        if c['gateCode'] == 'native-proof':
            fid = c['familyId']
            ld = ledger_fams.get(fid, -1)
            if ld >= 0 and c['denominator'] != ld:
                print(f'MISMATCH {aname}/{fid}: claims denom={c[\"denominator\"]} vs ledger={ld}')
                mismatches += 1
if mismatches == 0:
    print('All claims match ledger')
"
```

**通过标准**：无 MISMATCH 输出。

### 3. Coverage Evidence 存在性

```bash
python -c "
import json
from pathlib import Path
ledger = json.loads(Path('verification/projections/foundation-dll-audit/capability-family-ledger.json').read_bytes())
missing = 0
for dll in ledger['dlls']:
    for fam in dll.get('families', []):
        cov_path = Path('verification/foundation-dll') / dll['assemblyName'] / fam['familyId'].split('/')[-1] / 'il2cpp_dist' / 'native-reference.runtime-skeleton.coverage.json'
        if not cov_path.exists():
            slug = '-'.join(fam['familyId'].split('/')[2:])
            cov_path2 = Path('verification/foundation-dll') / dll['assemblyName'] / slug / 'il2cpp_dist' / 'native-reference.runtime-skeleton.coverage.json'
            if not cov_path2.exists() and fam.get('methodCount', 0) > 0:
                print(f'MISSING: {dll[\"assemblyName\"]}/{fam.get(\"familyId\",\"?\")} ({fam.get(\"methodCount\",0)} methods, no coverage)')
                missing += 1
if missing == 0:
    print('All families have coverage JSON or 0 methods')
"
```

**通过标准**：有 methodCount 的 family 都应该有 coverage JSON。

### 4. Dashboard Gate 状态一致性

```bash
python -c "
import json
from pathlib import Path
fv = json.loads(Path('verification/projections/foundation-dll-audit/family-verification.json').read_bytes())
matrix = json.loads(Path('verification/projections/foundation-dll-audit/dll-matrix.json').read_bytes())
matrix_by_asm = {r['assemblyName']: r for r in matrix['rows']}
for asm in fv['assemblies']:
    aname = asm['assemblyName']
    all_passed = all(
        g == 'passed'
        for fam in asm['families']
        for g in fam.get('verificationGates', {}).values()
        if g not in ('not-required', 'in-progress', '')
    )
    print(f'{aname}: {len(asm[\"families\"])} families, all_gates_passed={all_passed}')
"
```

### 5. Mechanism Integrity（新增）

验证 claims 中的 "passed" 状态不是通过 skip/stub 伪造的假通过：

```bash
python -c "
import json
from pathlib import Path

claims = json.loads(Path('verification/projections/foundation-dll-audit/family-verification-claims.json').read_bytes())
skip_registry_path = Path('verification/projections/foundation-dll-audit/skip-registry.json')
skip_registry = json.loads(skip_registry_path.read_bytes()) if skip_registry_path.exists() else {}

issues = []
for asm in claims['assemblies']:
    for c in asm.get('claims', []):
        if c.get('gateCode') != 'native-proof':
            continue
        method_ids = c.get('methodSubjectIds', [])
        for mid in method_ids:
            if mid in skip_registry:
                issues.append(f'{asm[\"assemblyName\"]}/{c[\"familyId\"]}: {mid} is in skip registry but claim is passed')
        
        # Check for discrepancy between claim status and actual mechanism audit
        if c.get('numerator', 0) == c.get('denominator', 0) and c.get('denominator', 0) > 0:
            # 100% pass rate — verify this isn't because everything was skipped
            audit_path = Path('verification') / 'foundation-dll' / asm['assemblyName'] / \
                c['familyId'].split('/')[-1] / 'mechanism-audit-report.json'
            if audit_path.exists():
                audit = json.loads(audit_path.read_bytes())
                if audit.get('false_passing', 0) > 0:
                    issues.append(f'{asm[\"assemblyName\"]}/{c[\"familyId\"]}: claim=100% but audit found {audit[\"false_passing\"]} false passes')

if issues:
    print('MECHANISM INTEGRITY ISSUES:')
    for i in issues:
        print(f'  {i}')
else:
    print('Mechanism integrity check passed — no false-passing signals detected')
"
```

**通过标准**：
- 没有 claimed=passed 但实际在 skip registry 中的方法
- 没有 100% pass rate 但 mechanism audit 发现 false-passing 的 family
- 所有 claims 的通过率反映的是真实 C++ lowering，不是跳过测试的结果

### 6. Skip Registry 审计（新增）

验证 skip registry 中的条目没有过期：

```bash
python -c "
import json
from pathlib import Path
from datetime import datetime, date

registry_path = Path('verification/projections/foundation-dll-audit/skip-registry.json')
if not registry_path.exists():
    print('NO SKIP REGISTRY — all methods must be tested, no skips allowed without registration')
    exit(1)

registry = json.loads(registry_path.read_bytes())
today = date.today()
expired = []
for entry in registry.get('exceptions', []):
    review_date = entry.get('reviewDate')
    if review_date:
        try:
            rd = date.fromisoformat(review_date)
            if rd < today:
                expired.append(f'{entry.get(\"methodSubjectId\",\"?\")}: review date {review_date} is past due')
        except ValueError:
            expired.append(f'{entry.get(\"methodSubjectId\",\"?\")}: invalid reviewDate format {review_date}')

if expired:
    print('EXPIRED SKIP EXCEPTIONS:')
    for e in expired:
        print(f'  {e}')
    print(f'Total expired: {len(expired)} — must be reviewed before verification gates can proceed')
else:
    print('Skip registry: all exceptions current')
"
```

## 修复命令

如果上面任何检查失败，一键修复：

```bash
python -c "
from pathlib import Path
import sys
sys.path.insert(0, 'build/toolchains/run/testing')
sys.path.insert(0, 'build/toolchains/run')
from foundation_dll_audit_generator import write_foundation_dll_audit_outputs
result = write_foundation_dll_audit_outputs(Path('.').resolve())
print(f'Regenerated: {len(result[\"artifacts\"])} artifacts')
"
```

## 验证标准

| 检查 | 通过条件 | 失败处理 |
|------|---------|----------|
| Claims 完整性 | 所有 native-proof claims 有 methodSubjectIds | re-run audit generator |
| Claims vs Ledger | 无 denominator 不一致 | 检查 ledger 数据源 |
| Coverage 存在 | 有 methodCount 的 family 有 coverage JSON | run batch_native_aot_runner |
| Dashboard 门状态 | 无异常 gate 状态 | re-run audit generator |
| **Mechanism Integrity** | **无 false-passing 信号** | **定位 stub/skip 问题 → 修复 → 重新验证** |
| **Skip Registry** | **所有例外未过期** | **审查过期条目 → 更新或消除 skip** |

## 关联

- 父流程: `dev-foundation-dll-verification-pipeline`
- 下游: `dev-foundation-dll-family-verification`
- Skip Registry: `wiki/06-测试验证/foundation-dll-skip-registry.md`
