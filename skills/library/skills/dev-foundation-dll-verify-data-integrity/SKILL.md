---
name: dev-foundation-dll-verify-data-integrity
description: Validate foundation DLL dashboard data consistency — claims, ledger, coverage evidence, and mechanism integrity — before or after running verify_family() verification.
---

# Foundation DLL Data Integrity Verification

## 概述

在运行 `verify_family()` 验证之前（或之后），确认 dashboard 底层数据是正确的。避免"测试全绿但 dashboard 显示为 0"的问题。本技能定义**检查策略**而非手动执行步骤——检查可由 `run foundation-dll verify-consistency` 自动完成。

**入口信号**：`dev-foundation-dll-verify-data-integrity`

## 检查项

### 1. Claims 完整性

所有 assembly 的 native-proof claims 必须有 `methodSubjectIds`。

```bash
run foundation-dll verify-consistency --mode full
```

或手动检查：
```bash
python -c "
import json
claims = json.load(open('verification/projections/foundation-dll-audit/family-verification-claims.json'))
for asm in claims['assemblies']:
    native = [c for c in asm.get('claims',[]) if c['gateCode']=='native-proof']
    with_mids = [c for c in native if c.get('methodSubjectIds')]
    if len(native) != len(with_mids):
        print(f'ISSUE: {asm[\"assemblyName\"]}: {len(native)} claims, {len(with_mids)} have MIDs')
"
```

### 2. Claims vs Ledger 一致性

Claims 中的 denominator 必须与 ledger 中的 methodCount 一致。

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

### 3. Coverage Evidence 存在性

有 methodCount 的 family 都应该有 coverage JSON。

### 4. Dashboard Gate 状态一致性

Dashboard 上的 gate 状态与 unified-verification-report.json 一致。

### 5. Mechanism Integrity（新增）

验证 claims 中的 "passed" 状态不是通过 skip/stub 伪造的假通过：
- 没有 claimed=passed 但实际在 skip registry 中的方法
- 没有 100% pass rate 但 mechanism audit 发现 false-passing 的 family

### 6. Skip Registry 审计

验证 skip registry 中的条目没有过期：

```bash
python -c "
import json
from pathlib import Path
from datetime import date
registry = json.loads(Path('verification/projections/foundation-dll-audit/skip-registry.json').read_bytes())
today = date.today()
expired = []
for entry in registry.get('exceptions', []):
    review_date = entry.get('reviewDate')
    if review_date:
        rd = date.fromisoformat(review_date)
        if rd < today:
            expired.append(f'{entry.get(\"methodSubjectId\",\"?\")}: review date {review_date} expired')
if expired:
    print('EXPIRED SKIP EXCEPTIONS:')
    for e in expired: print(f'  {e}')
else:
    print('Skip registry: all exceptions current')
"
```

## 修复命令

如果上面任何检查失败，一键刷新：

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
| Coverage 存在 | 有 methodCount 的 family 有 coverage JSON | run pipeline_native_aot_runner |
| Dashboard 门状态 | 无异常 gate 状态 | re-run audit generator |
| Mechanism Integrity | 无 false-passing 信号 | 定位 stub/skip → 修复 → 重新验证 |
| Skip Registry | 所有例外未过期 | 审查过期条目 → 更新或消除 skip |

## 关联

- CLI 入口: `run foundation-dll verify-consistency`
- 修复命令: `run foundation-dll refresh`
- 下游: `dev-foundation-dll-verification-pipeline`
- Skip Registry: `wiki/06-测试验证/foundation-dll-skip-registry.md`
