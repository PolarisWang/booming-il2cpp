---
name: dev-foundation-dll-verify-data-integrity
description: Validate foundation DLL dashboard data consistency — claims, ledger, coverage evidence — before running verification gates. TRIGGER when dashboard shows incorrect numbers, claims are empty, or coverage evidence is missing.
---

# Foundation DLL Data Integrity Verification

## 概述

在运行三闸门验证之前，先确认 dashboard 底层数据是正确的。避免"测试全绿但 dashboard 显示为 0"的问题。

**入口信号**：`dev:foundation-dll-verify-data-integrity`

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
            # Also try the slug form
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

## 关联

- 父流程: `dev:foundation-dll-verification-pipeline`
- 下游: `dev:foundation-dll-family-verification`
