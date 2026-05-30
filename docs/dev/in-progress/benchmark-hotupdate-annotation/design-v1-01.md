# 方案 A+C 设计文档

## 1. 边界拍板

| 项目 | 决策 |
|------|------|
| patch 范围裁剪 | 只在 pipeline 验证场景传入 `--subject-only`，不改 emit-patch-data 默认行为 |
| contract 字段 | 双字段 `benchmarkMethodIndices` + `hotupdateMethodIndices`，缺省 = 全部 subject |
| 禁止修改 | codegen / IL2CPP runtime 核心（`src/native/runtime-core/`, `src/managed/Chaos.IL2CPP.Generator/` 的 lowering planner 等） |
| 允许修改 | pipeline stage（`benchmark.py`, `hotupdate.py`, `audit/`）、测试工具（`PatchDataExtractor.cs`）、contract schema（`family-schema.json`）、pre-verification audit |

## 2. 架构影响评估

### 2.1 职责边界

不影响现有模块职责边界，只在已有扩展点（contract schema / stage filter / CLI arg）上新增选项：

```
修改范围（改动层）：
  family-schema.json         ← 新增字段定义
  benchmark.py               ← 读取 benchmarkMethodIndices 过滤 benchmark 方法
  hotupdate.py               ← 读取 hotupdateMethodIndices 过滤 semantic 检测
  PatchDataExtractor.cs      ← 新增 --subject-only 过滤逻辑
  DriverEntry.cs             ← 新增 --subject-indices 参数透传
  pre_verification_audit.py  ← 新增 annotation 完整性检查
  contract/*.json            ← 补充新字段（批量迁移）
```

### 2.2 依赖方向

```
contract  →  pipeline stage (benchmark.py, hotupdate.py)
          →  emit-patch-data CLI (PatchDataExtractor.cs)
          →  pre-verification audit

emit-patch-data CLI  →  PatchDataExtractor.cs (新增 --subject-only 参数)
```

无循环依赖。emit-patch-data 的 `--subject-only` 只在 pipeline hotupdate stage 中调用。

---

## 3. 详细方案 A：Contract annotation

### 3.1 Schema 定义

`testing/foundation-dll/_contracts/family-schema.json`：

```json
{
  "benchmarkMethodIndices": {
    "type": "array",
    "items": { "type": "integer", "minimum": 0 },
    "description": "Indices into methodSubjectIds that participate in benchmark timing. Omitted or null = benchmark all subjects"
  },
  "hotupdateMethodIndices": {
    "type": "array",
    "items": { "type": "integer", "minimum": 0 },
    "description": "Indices into methodSubjectIds checked for hotupdate semantic change (semanticChangedCount). Omitted or null = check all subjects. Methods outside this list still get patched and reverted, but don't affect semanticChangedCount."
  }
}
```

### 3.2 convert-char 的 annotation 值

基于 managed_fact 结果（18 subjects）：

| Index | Method | Exception? | benchmarkMethodIndices | hotupdateMethodIndices |
|-------|--------|-----------|----------------------|----------------------|
| 0 | ToChar(Boolean) | ✓ InvalidCastException | — | — |
| 1 | ToChar(Byte) | — | ✓ | ✓ |
| 2 | ToChar(Char) | — | ✓ | ✓ |
| 3 | ToChar(DateTime) | ✓ InvalidCastException | — | — |
| 4 | ToChar(Decimal) | ✓ InvalidCastException | — | — |
| 5 | ToChar(Double) | ✓ InvalidCastException | — | — |
| 6 | ToChar(Int16) | — | ✓ | ✓ |
| 7 | ToChar(Int32) | — | ✓ | ✓ |
| 8 | ToChar(Int64) | — | ✓ | ✓ |
| 9 | ToChar(Object) | ✓ ArgumentNullException | — | — |
| 10 | ToChar(Object+IFormatProvider) | ✓ ArgumentNullException | — | — |
| 11 | ToChar(SByte) | — | ✓ | ✓ |
| 12 | ToChar(Single) | ✓ InvalidCastException | — | — |
| 13 | ToChar(String) | ✓ String too long | — | — |
| 14 | ToChar(String+IFormatProvider) | ✓ String too long | — | — |
| 15 | ToChar(UInt16) | — | ✓ | ✓ |
| 16 | ToChar(UInt32) | — | ✓ | ✓ |
| 17 | ToChar(UInt64) | — | ✓ | ✓ |

```json
{
  "benchmarkMethodIndices": [1, 2, 6, 7, 8, 11, 15, 16, 17],
  "hotupdateMethodIndices": [1, 2, 6, 7, 8, 11, 15, 16, 17]
}
```

基准测试从 18 个方法减到 8 个（减少 55% pipeline 时间占比）。

### 3.3 benchmark.py 修改

```python
def _load_benchmark_indices(ctx: FamilyContext) -> list[int]:
    """Load benchmarkMethodIndices from contract. Fall back to all subjects."""
    contract_path = ctx.contract_path
    if not contract_path.exists():
        return []
    try:
        c = json.loads(contract_path.read_text(encoding="utf-8"))
        indices = c.get("benchmarkMethodIndices")
        if indices is not None and len(indices) > 0:
            return sorted(indices)
        # Fallback: all subjects
        mids = c.get("methodSubjectIds", [])
        return list(range(len(mids)))
    except Exception:
        return []
```

`_run_all_benchmarks()` 使用该 indices：

```python
method_indices = _load_benchmark_indices(ctx)
if not method_indices:
    return {"status": "skipped", "summary": "no benchmark methods"}
total_methods = len(method_indices)

# R5 值门控：只对 benchmark indices 做 pre-verify
# --fact-json 对所有 subject 执行，但只有 benchmark 索引内的计入 value_gate
```

### 3.4 hotupdate.py 修改

```python
def _load_hotupdate_indices(ctx: FamilyContext) -> list[int]:
    """Load hotupdateMethodIndices from contract. Fall back to all subjects."""
    # 逻辑同 _load_benchmark_indices，但读取 hotupdateMethodIndices
```

变更点：

1. **`run_hotupdate()`**: fact 验证仍对所有 subject 执行（验证 revert），但 semanticChangedCount 只统计 `hotupdateMethodIndices` 内的方法
2. **`run_hotupdate_aot_bench()`**: benchmark 只对 `hotupdateMethodIndices` 内的方法执行
3. **`run_hotupdate_jit_fact()`**: semanticChangedCount 只统计 `hotupdateMethodIndices` 内的方法 → JIT 不再报告 0/18

### 3.5 pre-verification audit 补充检查

新增 `p4_annotation_completeness` check：

```python
# 检查 benchmarkMethodIndices 是否覆盖所有"非异常"方法
# 1. 运行 managed_fact 获取异常路径方法
# 2. 检查所有正常返回的方法是否都在 benchmarkMethodIndices 中
# 3. 检查 benchmarkMethodIndices 中没有异常路径方法
# 4. 对 hotupdateMethodIndices 做同样的检查
# 5. 检查 hotupdateMethodIndices 中的方法是否有非 void 返回类型
```

---

## 4. 详细方案 C：Patch scope 裁剪

### 4.1 emit-patch-data CLI 新增参数

```
dotnet run -- emit-patch-data <dll> <output> 
    [--aot-core-ir <path>] 
    [--direction <forward|backward>]
    [--subject-only] 
    [--subject-indices "0,1,2,6,7,8"]
```

- `--subject-only`: 开关，启用 subject-only 模式
- `--subject-indices`: subject index 列表（对应 contract 中的 methodSubjectIds 索引），与 `--subject-only` 配合使用
- 两个参数缺省时，行为与当前一致（patch 所有方法）

### 4.2 PatchDataExtractor.cs 修改

```csharp
public void Extract(
    string dllPath, 
    string outputPath, 
    string? aotCoreIrPath = null, 
    string? genuineIrPath = null,
    string? subjectIndices = null)  // 新增: "0,1,2,6,7,8" 格式
{
    // ...
    // 在 BuildAotCoreIrSection() 后，如果 subjectIndices 不为 null:
    //   1. 解析 subjectIndices 为 HashSet<int>
    //   2. 遍历所有 patch method entry
    //   3. 只保留 subject 方法的 entry
    //   4. 过滤非 subject 方法的 patch data
    // PatchDataExtractor 内部方法通过 patch DLL method name 匹配 Subject_N
}
```

关键技术点：
- subject method 的特征：方法名包含 `Subject_` 或 `CustomEntrySubject_` 或 `CustomEntryMethod`（与 `NativeAotLoweringPlanner.ExtractSubjectIndex` 一致的模式）
- 非 subject 的 patch entry 直接从 patchdata 二进制中移除
- Header/meta section 中的 method_count 更新为只包含 subject 方法

### 4.3 DriverEntry.cs 修改

```csharp
// 新增参数透传
else if (args[i] == "--subject-indices" && i + 1 < args.Length)
    subjectIndices = args[++i];
```

### 4.4 hotupdate.py _ensure_patch_data() 修改

```python
def _ensure_patch_data(ctx, direction="forward"):
    # ...
    hotupdate_indices = _load_hotupdate_indices(ctx)
    subject_only = len(hotupdate_indices) > 0 and len(hotupdate_indices) < total_subjects
    
    cmd = [dotnet, "run", "--", "emit-patch-data", dll_path, output_path,
           "--aot-core-ir", ir_path, "--direction", direction]
    
    if subject_only:
        indices_str = ",".join(str(i) for i in hotupdate_indices)
        cmd += ["--subject-only", "--subject-indices", indices_str]
    # ...
```

---

## 5. 合同迁移策略

### 5.1 自动检测

`pre_verification_audit.py` 输出 `verdict=MISSING_ANNOTATION` 当：
- `benchmarkMethodIndices` 或 `hotupdateMethodIndices` 不存在
- 且存在非异常方法（需要 benchmark）

### 5.2 半自动补全

```bash
python -m verification.stages.pre_verification_audit <slug> \
    --assembly System.Private.CoreLib \
    --fix-annotations
```

`--fix-annotations` 自动：
1. 运行 managed_fact 检测异常路径
2. 非异常方法 → 加入 `benchmarkMethodIndices`
3. 非 void 返回 + 非异常方法 → 加入 `hotupdateMethodIndices`
4. 备份原 contract → 写新 contract

### 5.3 手动调整（按需）

自动补全后，人工审查：
- 是否有条件异常方法误判？（如 Object 重载传入 null 抛异常但传入有效对象正常工作）
- 是否需要从 hotupdateMethodIndices 中移除某些方法？（如果它们虽然是 int 但 patch 不会改变返回值）

---

## 6. 风险评估

| 风险 | 严重度 | 概率 | 缓解措施 |
|------|--------|------|---------|
| **A: 条件异常方法误分类** — Object 重载传入 null 抛异常但传入有效对象正常工作 | 中 | 中 | `--fix-annotations` 只做建议，人工确认；audit 阶段标记 CONCERN |
| **A: 缺省行为不一致** — 新 contract 有字段为空 = 不 benchmark 任何方法，旧 contract 无字段 = 全部 benchmark | 中 | 低 | Python 端读取时区分 "字段不存在" 和 "字段存在但为空数组" |
| **C: subjectIndices 解析错误** — 索引字符串格式不对导致 PatchDataExtractor 静默忽略所有方法 | 高 | 低 | patchdata 输出后校验 method_count > 0 |
| **C: Subject_ N 命名冲突** — 非 subject 方法名也包含 Subject_ 前缀 | 中 | 低 | PatchDataExtractor 使用与 NativeAotLoweringPlanner 一致的 regex |
| **C: patchdata method_count=0** — 所有方法被过滤掉导致空 patch | 高 | 低 | hotupdate.py 验证 method_count > 0（已有逻辑） |
| **迁移: 遗漏更新** — 新增 subject 时忘记更新 annotation 字段 | 低 | 中 | pre-verification audit 自动检测并报 CONCERN |

---

## 7. 三优先级权衡

| 优先级 | 方案 A | 方案 C |
|--------|--------|--------|
| **P1 性能最优** | 无影响（benchmark 只是测量，不改变 codegen） | 无影响（patch scope 裁剪只影响验证流程，不改变 runtime） |
| **P2 架构完美** | 正向贡献—per-method annotation 比全量 benchmark 更精确 | 正向贡献—只 patch 需要验证的方法更合理 |
| **P3 热更适配** | 无影响 | 不影响生产发布（--subject-only 仅在 pipeline 场景使用） |

---

## 8. 文件清单

| 文件 | 改动类型 | 改动量 |
|------|---------|--------|
| `testing/foundation-dll/_contracts/family-schema.json` | 新增字段 | ~20 行 |
| `testing/foundation-dll/verification/stages/benchmark.py` | 修改 `_run_all_benchmarks()` + 新增 `_load_benchmark_indices()` | ~50 行 |
| `testing/foundation-dll/verification/stages/hotupdate.py` | 修改 `_ensure_patch_data()` + `run_hotupdate()` + `run_hotupdate_aot_bench()` + `run_hotupdate_jit_fact()` + 新增 `_load_hotupdate_indices()` | ~80 行 |
| `testing/foundation-dll/verification/stages/pre_verification_audit.py` | 新增 `p4_annotation_completeness` 检查 | ~50 行 |
| `src/managed/Chaos.IL2CPP.Generator/PatchDataExtractor.cs` | 新增 `--subject-only/--subject-indices` 过滤逻辑 | ~100 行 |
| `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs` | 新增参数透传 | ~10 行 |
| `testing/foundation-dll/System.Private.CoreLib/*/capability-family-contract.json` | 批量补充新字段 | ~20 files × 5 行 |
| **总计** | | **~330 行** |

---

## 9. 执行计划

### Phase 1：Contract schema + Pipeline 修改（~2 小时）
1. `family-schema.json` 加 benchmarkMethodIndices / hotupdateMethodIndices
2. `benchmark.py` 加 `_load_benchmark_indices()` + 过滤逻辑
3. `hotupdate.py` 加 `_load_hotupdate_indices()` + 过滤逻辑
4. 跑 convert-char pipeline 验证

### Phase 2：Patch scope 裁剪（~2 小时）
1. `PatchDataExtractor.cs` 加 `--subject-only/--subject-indices`
2. `DriverEntry.cs` 加参数透传
3. `hotupdate.py` 的 `_ensure_patch_data()` 传入 subject indices
4. 跑 convert-char hotupdate 验证

### Phase 3：Audit 补充 + 合同迁移（~1 小时）
1. `pre_verification_audit.py` 加 annotation 完整性检查
2. 自动补全脚本 `--fix-annotations`
3. 批量迁移现有 contract（~20 files）

### Phase 4：回归验证（~30 分钟）
1. 跑 2-3 个典型 family 的全量 pipeline
2. 验证向后兼容性（旧 contract 无字段时 = 默认全部 subject）

---

## 10. 回滚方案

| 层次 | 回滚操作 | 影响 |
|------|---------|------|
| Contract | 删除 `benchmarkMethodIndices`/`hotupdateMethodIndices` 字段 | 回退到全部 subject benchmark，零风险 |
| Pipeline | `git revert` benchmark.py + hotupdate.py 修改 | benchmark 行为恢复原状 |
| PatchDataExtractor | `git revert` PatchDataExtractor.cs + DriverEntry.cs | 恢复全量 patch |
| Audit | `git revert` pre_verification_audit.py | audit 恢复只检查现有的 checks |
