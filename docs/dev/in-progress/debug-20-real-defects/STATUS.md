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

## 最终清单：9 个 realDefect

| si | BCL 方法 | 故障码 | 状态 |
|----|---------|--------|------|
| 7 | `Activator::CreateInstance(Type)` | 待查 | 未修 |
| 151 | `Array::CreateInstance(Type, Int32)` | `0xe0000001` | 未修 |
| 1357 | `Enum::Parse(Type, String, Boolean)` | 待查 | 未修 |
| 1371 | `Enum::Format(Type, Object, String)` | `0xe0000001` | 未修 |
| 1720 | `UInt64::Parse(String, NumberStyles)` | 待查 | 未修 |
| 1871 | `Math::Cos(Double)` | `0xc0000005` | 未修 |
| 1877 | `Math::Pow(Double, Double)` | `0xc0000005` | 未修 |
| 2899 | `Type::GetField(String)` | 待查 | 未修 |
| — | `Random::NextDouble()` | `0xc0000005` | 未修 |

## 已修复（本轮）

### Int32/Int64 Parse 多参重载（commit `697b8550b`）

**症状**：`0xc0000005` 访问违规。

**根因**：只有 1 参 `Parse(String)` 注册了 shape；ATG 注入有效字面量后调用
`Parse("1234567", NumberStyles)`（2 参），而 SimpleForward 发射器转发**全部**
参数 → `ChaosParseInt32(2参)` 打到 1 参 native → 栈上读到垃圾 → AV。

**修复**：
- native 侧加 `ChaosParseInt32Styles/Provider/StylesProvider` +
  `ChaosParseInt64Styles/Provider/StylesProvider`
- 注册侧补齐 6 个 shape，指向对应 arity 的符号

**验证**：realDefect 从 20 降到 9（去重后独立方法）。

## 两类剩余故障

| 故障码 | 含义 | 数量 |
|--------|------|------|
| `0xe0000001` | C++ EH 异常（托管抛出） | 3 |
| `0xc0000005` | STATUS_ACCESS_VIOLATION | 3 |
| 待查 | — | 3 |

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
