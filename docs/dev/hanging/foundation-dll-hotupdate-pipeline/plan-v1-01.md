# Hotupdate 验证管线修复 实现计划

> **面向执行 Agent：** 使用 `dev-executing-plans` 执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 修复 hotupdate 验证管线的 6 个验证正确性问题 + 5 个报告正确性问题。核心改动：PatchDataExtractor IL 重写（返回 sentinel）、Scriban 模板 SEH 包裹、Python 层修复。

**架构：** 四层修改（PatchDataExtractor C# → Scriban C++ 模板 → Python 编排 → Python 聚合），各层职责独立，互不依赖。

**技术栈：** C# (PatchDataExtractor), Scriban (C++ 模板), Python (编排/聚合)

**架构审核模式：** critical（涉及 codegen 层 PatchDataExtractor + IL 操作）

**结构告警重点：**
- PatchDataExtractor 的 IL 重写逻辑必须与现有的 PE 元数据读取逻辑分离（职责清晰）
- Scriban 模板改动需要保持向后兼容（不破坏已有的无-patch-data 路径）

**权责图审核主题：**
- PatchDataExtractor（C#）只负责 PE 元数据提取 + IL body 序列化 → 新增 IL 重写逻辑放在独立方法 `RewriteSubjectBodies()`
- Scriban 模板（C++）负责生成 entry.cpp → SEH 包裹只改动 dispatch 循环体
- Python 层只负责编排和报告 → 不引入新工具

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: hotupdate-verification
- capabilityItem: hotupdate-fix-pipeline
- ownerSubjectId: n/a
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: Proof
- formalVerificationObjects: [PatchDataExtractor IL rewriting, runtime-entry.cpp SEH, hotupdate_chunk.py pass criterion]
- requiredGates: build -> hotupdate

**设计文档：** `docs/dev/in-progress/foundation-dll-hotupdate-pipeline/design-v1-01.md`

**问题清零来源：** brainstorm-approved user confirmation

**计划来源：** direct-plan

**预期知识沉淀：** n/a（修复不产生长期知识）

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 → 测试通过 → 归档 completed → 合并&提交"固定链路。

---

## 修改清单

| # | 文件 | 改动 | 解决的问题 |
|---|------|------|-----------|
| 1 | `src/managed/Chaos.IL2CPP.Generator/PatchDataExtractor.cs` | IL 重写：Subject_N body → ldc.i4 sentinel; ret | A1 (语义验证) |
| 2 | `src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/Templates/TestProject.RuntimeEntry.cpp.scriban` | SEH 包裹 RunHotupdateMode 体 | A5 (AV 崩溃) |
| 3 | `testing/foundation-dll/verification/stages/hotupdate_chunk.py` | B1/B3/B4/A6 四项修复 | JSON 恢复/状态语义/通过标准/降级告警 |
| 4 | `testing/foundation-dll/verification/stages/aggregate.py` | 增加 hotupdate 字段传播 | B2 |

不需要改：ATG（Program.cs / ValueGenerator / TestEmitter）

---

## Task 1：PatchDataExtractor IL 重写

**文件**: `src/managed/Chaos.IL2CPP.Generator/PatchDataExtractor.cs`

在 `Extract()` 方法的 `ReadBodyData()` 调用之后，增加独立重写方法。

```
原始 Subject_N wrapper IL: call real_method + 转换返回值 + ret
重写后 IL (Tiny 格式, 7 字节): ldc.i4 <0xBEEF0000 | index> + ret
```

关键点：
- 识别 Subject_N 方法：MethodDefEntry.Name 以 "Subject_" 开头（从 StringHeap 读取）
- Sentinel = `0xBEEF0000 | (subjectIndex & 0xFFFF)`
- 7 字节 Tiny body 必然 <= 任何真实 IL body → 原地覆盖安全
- 加 `Debug.Assert(newBody.Length <= md.BodySize)`

方法签名：
```csharp
private void RewriteSubjectBodies(MethodDefEntry[] methodDefs, byte[] bodyData)
private static byte[] BuildSentinelBody(int sentinel)
```

---

## Task 2：Scriban 模板 SEH 包裹

**文件**: `src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/Templates/TestProject.RuntimeEntry.cpp.scriban`

在 `RunHotupdateMode` 函数体内外层包裹 `__try/__except`：

```cpp
__try {
    // 整个 RunHotupdateMode 现有逻辑
} __except (EXCEPTION_EXECUTE_HANDLER) {
    fprintf(stderr, "[hotupdate] CRASH: exception code=0x%X\n", GetExceptionCode());
    // 输出已收集的 partial JSON
}
```

需要读取模板文件确认 RunHotupdateMode 的精确位置。

---

## Task 3：hotupdate_chunk.py 修复

**文件**: `testing/foundation-dll/verification/stages/hotupdate_chunk.py`

### B1: JSON 截断完整性验证
- 恢复后：比较 `totalMethods` vs `len(baselineFact)`
- 不匹配时设置 `_truncated: true`

### B3: 区分 skipped 状态
- `skipped_no_subjects` vs `skipped_all_failed`

### B4: 统一 pass criterion
- 去掉 `all_semantic` 硬性要求
- 统一为 `failed == 0 && passed > 0 && all_revert`

### A6: 补丁生成降级告警
- `_patch_generation_attempted` 标记
- `result_data["patchFailed"] = True`
- 打印 WARNING

---

## Task 4：aggregate.py 字段传播

**文件**: `testing/foundation-dll/verification/stages/aggregate.py`

增加字段：`exitCode`, `patchDataUsed`, `patchFailed`, `truncated`, `assertFailed`, `semanticChangedCount`

---

## Task 5：验证

1. **PatchDataExtractor 验证**: 用 system chunk 重建，检查 patch.patchdata
   ```bash
   python -m verification.chunk_pipeline --chunk system --stages build
   ```

2. **Hotupdate 验证**: system chunk hotupdate 应全通过
   ```bash
   python -m verification.chunk_pipeline --chunk system --stages hotupdate
   ```

3. **崩溃恢复验证**: system-5 应不再 kill entry.exe
   ```bash
   python -m verification.chunk_pipeline --chunk system-5 --stages hotupdate
   ```

4. **全量验证**:
   ```bash
   python -m verification.chunk_pipeline --all-chunks --stages build,hotupdate
   ```

---

## 执行顺序

```
Task 1 (PatchDataExtractor) ──┐
Task 2 (Scriban 模板) ────────┤
Task 3 (hotupdate_chunk.py) ──┤──> 并行修改
Task 4 (aggregate.py) ───────┤
                               │
                               v
                         Task 5 (验证)
```
