# debug-20-real-defects — STATUS

> **task_id**: debug-20-real-defects
> **task_type**: plan
> **phase**: diagnosed
> **创建日期**: 2026-09-15
> **entry_skill**: dev-il2cpp → dev-il2cpp-debug-expert
> **关键文档**: `handoff.md`

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

---

## 一句话

`system` chunk 的 `realDefect` 经 SEH 级诊断确认为 **11 个**（不是最初报告的 20/63），全部是**真正的 AOT 实现缺陷** —— 有 native 符号注册，但执行时触发访问违规或托管异常。

## 诊断方法（已实施）

### 1. SEH 级 instrumentation（关键突破）

在 `TestProject.RuntimeEntry.cpp.scriban` 的 `__except` 分支注入：

```cpp
__except(EXCEPTION_EXECUTE_HANDLER) {
    caught = true;
    const char* _sid = ...kSubjectSubjectIds[si]...;
    std::fprintf(stderr, "[SEH-FAULT] code=0x%08lx si=%d %s\n",
                 (unsigned long)GetExceptionCode(), si, _sid);
}
```

**为什么必需**：runner 的 `caught` 同时覆盖 SEH 硬件异常和 C++ 异常，只有 `caught=true` 无法区分。注入后可直接看到故障码 + subject id。

### 2. 故障码分布（353 次）

| 代码 | 含义 | 次数 |
|------|------|------|
| `0xc0000005` | STATUS_ACCESS_VIOLATION | 267 |
| `0xe0000001` | C++ EH 异常（托管抛出） | 31 |

### 3. 分类路径修正

`si` 与 metadata index **不一致**（3237/3238 条），必须用
`generatedMethodId` → metadata 行 → `methodSubjectId` 的路径。已确认
`fact_chunk.py::_annotate` 走的正是这条路径。

## 最终清单：11 个 realDefect

| # | BCL 方法 | 已有 native 符号 |
|---|---------|-----------------|
| 1 | `Activator::CreateInstance(Type)` | `ChaosReflectionCreateInstance` |
| 2 | `Array::CreateInstance(Type, Int32)` | `ChaosArrayCreateInstance` |
| 3 | `Enum::Parse(Type, String, Boolean)` | enum stubs |
| 4 | `Enum::Format(Type, Object, String)` | enum stubs |
| 5 | `UInt64::Parse(String, NumberStyles)` ×3 | `ChaosParseUInt64Styles` |
| 6 | `Math::Cos(Double)` | `ChaosMathCos` |
| 7 | `Math::Pow(Double, Double)` | `ChaosMathPow` |
| 8 | `Random::NextDouble()` | `ChaosRandomNextDouble` |
| 9 | `Type::GetField(String)` | `ChaosTypeGetFieldBindingFlags` |

## 下一步

1. 逐个定位这 11 个的故障点（已有 `[SEH-FAULT]` 输出可用）
2. 区分「native 实现缺陷」vs「参数 ABI 传递缺陷」
3. 修复

## 复现命令

```bash
# 1. 跑 pipeline（会把 [SEH-FAULT] 写入 stderr）
cd /d/agent/chaos-il2cpp/tests/e2e
CHAOS_FOUNDATION_DLL=D:/agent/chaos-il2cpp/tests/e2e/translation \
  python -m verification.chunk_pipeline --chunk system --stages build,fact --smoke

# 2. 直接跑 entry.exe 捕获 SEH 诊断
./artifacts/foundation-dll/System.Private.CoreLib/chunks/system/native/entry.exe \
  --fact-json 2>seh.log >/dev/null
grep "SEH-FAULT" seh.log
```

## 验收标准

- 11 个 realDefect 全部归因
- 修复后 `realDefect` 计数归零
