# STATUS — void writer 方法副作用断言

```yaml
task_id: void-writer-sideeffect-assertion
task_type: plan
phase: completed
lifecycle_status: completed
current_dir: docs/dev/completed/20260920-03-void-writer-sideeffect-assertion
创建日期: 2026-09-20
entry_skill: dev-il2cpp → dev-brainstorm → dev-writing-plans
上游: json-xml-industrialization Phase 2 阻塞
关键文档:
  - problem-and-options.md
  - plan-v1-01.md
  - notes-stepA-spike.md / notes-stepA-spike2.md
  - notes-a2-implemented.md
  - notes-62-triage.md
  - notes-a-class-split.md
  - notes-b-class-and-aot-mismatch.md

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

## 目标达成情况

**目标**：让 JSON/XML writer 的 void 方法从 `unassertable` 变为可验证。

| 指标（text-json） | 起始 | 结束 |
|:------------------|:----:|:----:|
| `unassertable` | 33 | **3** |
| 断言强度 | `catch { }`（不验类型） | **typed catch**（验具体类型） |

**✅ 目标达成** —— void 方法不再"无法断言"。

## 落地的修复

| 提交 | 内容 |
|:-----|:-----|
| `ad234fa92` | shape 匹配加短名兜底（修 5 项静默失配） |
| `6257f899f` | **A2 实施**：外部 stub 异常断言改 typed catch |
| `f466633fa` | `Utf8JsonWriter` Reset/Flush 对齐 .NET `ObjectDisposedException` |

## 关键发现（按重要性）

### 1. `Assert.Throws<T>` 在 AOT 下未实现（改变了方案）

`RuntimeHelperShapeRegistry.CoreStubs.Part3.S23.cs:1523` 自承未实现。
→ 原计划的 A1 路径（复用该断言）**不通**，改用 **A2**（ATG 生成 typed catch +
C++ 层 `chaos_eh_match_type`）。**Spike 2 验证端到端可行。**

### 2. shape 静默失配（被 typed catch 暴露）

全量排查 475 个注册：**5 项失配**（callee 类型段是短名，注册用全名）。
**非系统性**。已加短名兜底修复。

### 3. 🔴 JSON writer 零写入实现

`AppendRaw`/`AppendStr` **= 0**。37 个函数全是"复刻 bare-object 异常语义"。
**JSON 写面覆盖率被高估** —— `real` 只反映异常复刻是否正确。
对比 XML writer（85 处真实写入）。**两条线成熟度不对称。**

### 4. AOT 异常类型匹配缺陷（超出本任务范围）

`Reset`/`Flush` 的 native 抛出了**正确的** `ObjectDisposedException`（插桩实测），
但生成体的 `chaos_eh_match_type` **匹配失败** → RETHROW → subject 失败。

**已记录待独立立项**（属「异常类型匹配」问题，非 void 断言）。

## 诚实标注

- **62 项 `factoryGap` 未消除** —— 归因完成（主因：重载未注册 shape），
  但未实现修复。其中 27 项属 A2（需新写 bare-object 模式函数），
  **建议暂不实现**（不增加真实写能力）
- **A 类切分的前提被推翻**：原以为"native 有实现只差注册"，
  实际 JSON writer 无任何写入逻辑
- 我在此任务中**连续两次误判**（"JSON 大部分已实现" → "JSON 未实现"），
  根因都是**没看 ATG 的 fixture 构造方式**。已沉淀教训
- B 类 native 修复**正确但未使 subject 转绿**（被下一层 AOT 缺陷阻断）

## 移交

| 新问题 | 性质 |
|:-------|:-----|
| AOT 异常类型匹配失败（`ObjectDisposedException`） | 独立缺陷，影响所有 typed catch |
| JSON writer 真实实现 | 独立大任务（输出缓冲/IBufferWriter/转义/格式化） |
| 62 项 factoryGap 的 A2 实现（27 项） | 可选，收益有限 |
