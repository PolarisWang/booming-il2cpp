# Hotupdate Return-Value 全链路验证设计 v1

## 背景

Foundation-dll chunk pipeline 的 hotupdate 阶段当前是空转——`entry.exe --hotupdate` 跑的是零长度 patch data，baseline capture 后直接跑回 baseline（无 patch 应用、无语义变化检测、无实际 revert 验证）。

## 目标

实现从 AutoTestGenerator → PatchDataExtractor → 原生 ApplyPatchFromMemory 的完整 hotupdate 验证链路，用 return-value 差异证明 patch 机制工作正常。

## 架构

```
Build（不变）:
  ATG (baseline) → subjects.dll + subjects.metadata.json
  TPG → entry.exe (hasPatchData=false)

Hotupdate（新增）:
  ATG (--patch-mode) → subjects-patch.dll + patch-indices.json
  csc → subjects-patch.dll
  PatchDataExtractor (subjectOnly) → patch.patchdata
  entry.exe --hotupdate --patch-data patch.patchdata → hotupdate.json
```

## 核心设计

### Subject_N 统一 return-value 模式

所有 Subject_N 方法统一返回 `long`，同时标记 `[Fact]` 和 `[HotUpdate]`：

```csharp
[Fact]
[HotUpdate]
public long Subject_42() {
    var result = Math.Abs(-5);
    Assert.Equal(5, result);
    return ResultToLong(result);
}
```

一个方法体同时服务于两种场景：
- `--fact-json` 运行时：作为 `[Fact]` 验证 assertion 正确性
- `--hotupdate` 运行时：作为 `[HotUpdate]` 被 patch 替换实现，return value 被原生代码捕获做 baseline vs patched 比较

Patch DLL 中只改变 return value，方法签名不变（NameIndex 匹配依据）。

### ResultToLong + Patch 变换

| 原始类型 | Baseline | Patch |
|---------|----------|-------|
| 整数系 (int/long/short/byte 等) | `(long)result` | `^ 0xFF` |
| bool | `result ? 1L : 0L` | 取反 |
| char | `(long)result` | `^ 0x80` |
| float/double | `(long)result` | `^ 0xFF` |
| enum | `(long)(int)result` | `^ 0xFF` |
| string/ref non-null | `1L` | `0L` |
| 可空 ref | `result != null ? 1L : 0L` | 取反 |
| void | `42L` | `142L` |

Assert.Equal 在 baseline 和 patch 中保持一致——patch 验证的是"返回值变了"而不是"断言崩了"。

### 原生语义检测

`RunHotupdateMode()` 的 `semantic_changed_count` 通过 return value 比较检测 patch 是否生效：

```
baseline: Subject_42 返回 5, 状态 pass
patched:  Subject_42 返回 5 ^ 0xFF = 250, 状态 pass
→ semantic_changed_count++（返回值不同）
→ allSemantic = true ✓

revert: Subject_42 返回 5, 状态 pass
→ allRevert = true ✓
```

### TPG 改动

`RuntimeEntry.cpp.scriban` 新增：
- `--patch-data <file>` CLI 参数
- `ApplyHotpatchFromFile()` 函数：读取外部 .patchdata 文件并调用 `ApplyPatchFromMemory`

### 管线路入

Hotupdate stage 默认包含在 pipeline 中，与 `build,fact,coverage-audit` 同级：
- `--stages build,fact,hotupdate,coverage-audit`
- 若 ATG --patch-mode 失败 → stage 标记为 skipped，不阻塞 pipeline

### 关键决策记录

- **Scope**: 全部方法都生成 HotUpdate 变体
- **Void**: Sentinel 值（42L/142L）
- **ATG 调用**: 两次调用（baseline + --patch-mode）
- **第二次 TPG**: 不需要（PatchDataExtractor subjectOnly 用哨兵 AOT IR）
- **Stage 触发**: 默认执行
