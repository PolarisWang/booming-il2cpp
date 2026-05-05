---
name: dev-foundation-dll-family-verification
description: Execute three-gate verification (Fact → Benchmark → HotUpdate) across one or more foundation DLL capability families, validate dashboard consistency, then produce a consolidated correctness + performance report.
---

# Foundation DLL Family Three-Gate Verification

## 概述

对选定的 capability family 依次执行三个验证门，**并在验证前后校验 dashboard 数据的一致性**：

| 门 | 验证内容 | 工具 |
|----|---------|------|
| **Data Integrity** | Dashboard claims/gates/coverage 数据一致 | `family-verification-claims.json` + `dll-matrix.json` |
| **Fact** | Managed xUnit `[Fact]` 测试全部通过 | `dotnet test` |
| **Benchmark** | Native AOT 性能不退化、计算结果正确 | native benchmark exe + managed baseline |
| **HotUpdate** | Native hotupdate patch/revert 机制正确 | native hotupdate exe |

完成后输出一份包含通过率、加速比和异常分析的汇总报告。**如果 dashboard 数据与验证结果不一致，必须先修复后再继续。**

## 前置条件

- 目标 family 已通过 `batch_native_aot_runner.py` 完整 pipeline（codegen + native compile）
- Solution 项目已生成在 `solution/System.Private.CoreLib/<family-slug>/`
- Native benchmark & hotupdate 可执行文件已构建在 `artifacts/presets/windows-x64-reference/verification/foundation-dll/System.Private.CoreLib/<family-slug>/native/Release/`
- SDK 项目 `src/reference/Chaos.TestFramework.Sdk/` 已 `dotnet build --configuration Release`

## 选择 Family 的原则

根据不同考察目的选择测试覆盖范围：

| 目的 | 推荐 family | 理由 |
|------|-------------|------|
| 基准回归 | convert-char | 简单转发，结果稳定 |
| StringId 路径 | string-char-text-core | 字符串操作密集 |
| 类型系统/stable_id | boxing-unboxing-casts, object-equality-identity | isinst/castclass/equality |
| 数值运算 | primitive-numeric-conversions | 基础 IL 翻译 |
| 反射复杂路径 | reflection-type | 43 方法，涉及外部依赖 |
| 完整覆盖 | 全部 32+ families | 综合验证 |

至少选择 **3-6 个**有代表性的 family 以保证覆盖不同维度。

## 执行步骤

### Step 0: Data Integrity — Dashboard 数据一致性校验

验证前先确认 dashboard 的底层数据是正确的，避免"测试全绿但 dashboard 显示为 0"的问题。

#### 0a. 检查 claims 是否包含 methodSubjectIds

```bash
python -c "
import json
claims = json.load(open('verification/projections/foundation-dll-audit/family-verification-claims.json'))
for asm in claims['assemblies']:
    native = [c for c in asm.get('claims',[]) if c['gateCode']=='native-proof']
    with_mids = [c for c in native if c.get('methodSubjectIds')]
    print(f'{asm[\"assemblyName\"]}: {len(native)} native-proof claims, {len(with_mids)} with methodSubjectIds')
"
```

预期：所有 native-proof claims 的 `methodSubjectIds` 不为空。如果为空，说明 `family-verification-claims.json` 是 stale 的，需要重新生成。

#### 0b. 校验 claims 与 ledger 的方法数一致

```bash
python -c "
import json
from pathlib import Path

claims = json.loads(Path('verification/projections/foundation-dll-audit/family-verification-claims.json').read_bytes())
ledger = json.loads(Path('verification/projections/foundation-dll-audit/capability-family-ledger.json').read_bytes())

# 按 assembly 对比 native-proof denominator vs ledger methodCount
ledger_by_asm = {d['assemblyName']: d for d in ledger['dlls']}
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
print('Check complete')
"
```

预期：无 MISMATCH 输出。如果有差异，说明 claims 数据源（`method-capability-contracts.json` 或 inline methodSubjectIds）与 ledger 不一致。

#### 0c. 检查 dashboard 门状态与 family-verification 一致

```bash
python -c "
import json
from pathlib import Path

fv = json.loads(Path('verification/projections/foundation-dll-audit/family-verification.json').read_bytes())
matrix = json.loads(Path('verification/projections/foundation-dll-audit/dll-matrix.json').read_bytes())

# 对比 gates: family-verification.json 的 verificationGates vs dll-matrix 的 gate 汇总
matrix_by_asm = {r['assemblyName']: r for r in matrix['rows']}
for asm in fv['assemblies']:
    aname = asm['assemblyName']
    matrix_row = matrix_by_asm.get(aname, {})
    # 检查所有 family 的 gate 是否都是 passed
    all_passed = all(
        g == 'passed'
        for fam in asm['families']
        for g in fam.get('verificationGates', {}).values()
        if g != 'not-required' and g != 'in-progress'
    )
    if all_passed:
        matrix_gate = matrix_row.get('completion-certification', '')
        if matrix_gate != 'in-progress':
            print(f'WARN: {aname} all gates passed but completion-certification={matrix_gate}')
    print(f'{aname}: {len(asm[\"families\"])} families, all_gates_passed={all_passed}')
"
```

#### 0d. 如果数据异常，重新生成 dashboard

```bash
python -c "
from pathlib import Path
import sys
sys.path.insert(0, 'build/toolchains/run/testing')
sys.path.insert(0, 'build/toolchains/run')
from foundation_dll_audit_generator import write_foundation_dll_audit_outputs
result = write_foundation_dll_audit_outputs(Path('.').resolve())
print(f'Regenerated: {len(result[\"artifacts\"])} artifacts -> {result[\"outputRoot\"]}')
"
```

这个命令会：
1. 从当前 `capability-family-ledger.json` 重新生成所有 claims
2. 重新评估所有 gate（native-proof, managed-proof, hotupdate-proof, benchmark, test-code）
3. 更新 `family-verification-claims.json`、`family-verification.json`、`dll-matrix.json`
4. 重新生成所有 HTML 报告页面

### Step 1: Fact — Managed xUnit 测试

```bash
# 单 family
dotnet test solution/System.Private.CoreLib/<family-slug>/tests/<ProjectBase>.Tests.csproj --configuration Release

# 多 family 并行（注意 SDK 锁竞争）
dotnet test solution/System.Private.CoreLib/convert-char/tests/ConvertChar.Tests.csproj --configuration Release &
dotnet test solution/System.Private.CoreLib/reflection-type/tests/ReflectionType.Tests.csproj --configuration Release &
wait
```

如果因 `Chaos.TestFramework.Sdk.dll` 文件锁导致 CS2012 错误，先单独 build SDK：

```bash
dotnet build src/reference/Chaos.TestFramework.Sdk/Chaos.TestFramework.Sdk.csproj --configuration Release
```

然后重跑失败的项目。

验证标准：`Failed: 0`，所有 `[Fact]` 断言通过。

### Step 2: Benchmark — Managed vs Native 性能对比

#### 2a. 运行 Managed 基线

```bash
dotnet run --project verification/foundation-dll/System.Private.CoreLib/<family-slug>/managed_test/benchmarks/ManagedBenchmarkHarness.csproj --configuration Release
```

输出 JSON 到 stdout，保存到 `benchmark/managed-baseline.json`。注意该输出**缺少开头 `{`**——需要修复：

```python
with open('managed-baseline.json') as f:
    content = f.read()
fixed = '{' + content
with open('managed-baseline.json', 'w') as f:
    f.write(fixed)
```

#### 2b. 运行 Native Benchmark

```bash
./artifacts/presets/windows-x64-reference/verification/foundation-dll/System.Private.CoreLib/<family-slug>/native/Release/chaos_benchmark_<slug>.exe \
  --entry-index <N> --iterations 100000 --subject-id "<methodSubjectId>"
```

对所有 methodSubjectId 逐个执行，输出保存为 `native/native-benchmark.json`。

#### 2c. 生成对比报告

使用 `build/toolchains/run/testing/foundation_dll/benchmark_comparator.py`：

```bash
python build/toolchains/run/testing/foundation_dll/benchmark_comparator.py \
  --managed <family-dir>/benchmark/managed-baseline.json \
  --native <family-dir>/native/native-benchmark.json \
  --output <family-dir>/benchmark-comparison-report.json
```

**注意**：如果 managed 或 native JSON 中的字段名与 comparator 预期不一致（如 managed 用 `name` 而非 `methodSubjectId`，native 用 `subjectId` 而非 `methodSubjectId`），需要手写对比脚本。直接对比逻辑：

```
speedup = ((managed_ms - native_ms) / managed_ms) * 100
native_faster = speedup > 1.0
managed_faster = speedup < -1.0
equal = -1.0 <= speedup <= 1.0
```

Native 使用 100000 iterations，managed 使用 100000 iterations 以获得稳定结果。checksum 验证计算结果一致。

### Step 3: HotUpdate — Native 热更验证

```bash
./artifacts/presets/windows-x64-reference/verification/foundation-dll/System.Private.CoreLib/<family-slug>/native/Release/chaos_hotupdate_<slug>.exe 2>/dev/null
```

输出 JSON 到 stdout（`verificationKind: "hotupdate-proof"`）。验证字段：

- `passedMethods` = `totalMethods`（或说明失败的合理性）
- 每个 method: `status == "passed"`, `revertVerified == true`
- `patchedReturnValue == expectedPatchedValue`

如果 Release 版 exe 不存在，可尝试 Debug 版（路径中 `Debug` 替换 `Release`）。

### Step 4: 汇总分析

对每个 family 收集以下数据：

```
| Family | Fact(tested/total) | Benchmark(matched/faster) | HotUpdate(passed/total) | RevertVerified | AvgSpeedup% |
```

需要关注的异常信号：
- Benchmark 出现 `managed_faster > 0`：代表 native 比 managed 慢，可能为回归
- HotUpdate 出现 `failedMethods > 0`：检查 `originalReturnValue` 是否与其他方法不一致
- HotUpdate 出现 `WARN: both host and semantic-patch returned 0 (stub?)`：方法为 skeleton stub，尚未真实翻译

#### reflection-type 已知问题

该 family 的 method index 21 存在 pre-existing stub 返回 `CHAOS_BRIDGE_STATUS_UNIMPLEMENTED`（值 0x01）而非 `CHAOS_BRIDGE_STATUS_OK`（值 0x00），导致 hotupdate 验证标记为 `failed`。Patch 逻辑和 revert 均正确，待真实 IL 翻译后自动修复。

### Step 5: Dashboard 刷新与验证

三扇门全部跑完后，需刷新 dashboard 数据并验证 dashboard 显示与实际结果一致。

#### 5a. 重新生成 dashboard

```bash
python -c "
from pathlib import Path
import sys
sys.path.insert(0, 'build/toolchains/run/testing')
sys.path.insert(0, 'build/toolchains/run')
from foundation_dll_audit_generator import write_foundation_dll_audit_outputs
result = write_foundation_dll_audit_outputs(Path('.').resolve())
print(f'OK: {len(result[\"artifacts\"])} artifacts')
"
```

#### 5b. 验证 Fact 通过率

打开 `docs/verification/foundation-dll-audit/dlls/<assembly>/families/<slug>-fact.html` 确认：

- `Fact Pass Rate` = 实际通过的 method 数 / total（不再为 0/13）
- `Closure Status` = `in-progress` 或 `closed`（取决于是否所有 gate 通过）
- 每行 method 的 covered 状态正确

#### 5c. 验证 Benchmark/HotUpdate 数据

- `docs/verification/foundation-dll-audit/dlls/<assembly>/families/<slug>-benchmark.html`
  - 每个 method 的 ops/sec 或对比数据正确
- `docs/verification/foundation-dll-audit/dlls/<assembly>/families/<slug>-hotupdate.html`
  - `passedMethods` = `totalMethods`

#### 5d. 验证主页面

`docs/verification/foundation-dll-audit/dlls/<assembly>.html` 确认：

- native-proof 列显示正确的通过比例（不再是 0/N）
- managed-proof 列状态正确
- 展开 tooltip 能看到方法级详情

如果 5a–5d 中任何数据与验证结果不一致，**必须先修复数据源（claims/ledger），重新生成，确认一致后才能继续**。

## 正确性评估标准

| 等级 | 条件 | 行动 |
|------|------|------|
| ✅ PASS | 所有三扇门通过，零异常 | 报告通过 |
| ⚠️ WARN | 个别 family 有 pre-existing 失败（如 stub 返回值） | 说明原因，标记为 pre-existing 非回归 |
| ❌ FAIL | 新出现的 test 失败 / benchmark 退化 / hotupdate 不匹配 | 立即停止，分析根因 |

## 输出格式

报告应包含以下章节：

1. **验证范围** — 选中的 family 列表与选取理由
2. **Fact 结果** — 每 family 的通过/总数表格
3. **Benchmark 结果** — 每 family 的 native-faster/managed-faster/平均加速比表格
4. **HotUpdate 结果** — 每 family 的通过/失败/revert-verified 表格，异常方法分析
5. **总体评估** — 按正确性评估标准给出结论
