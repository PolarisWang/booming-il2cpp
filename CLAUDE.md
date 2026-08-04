# Chaos IL2CPP 开发规则

> **第〇条规则（🔴 不可跳过）**：
> 进入新域/新任务时，回复第一行必须是分类声明，第二行输出 `Skill("dev-il2cpp")` 调用。
> **dev-il2cpp 是固定首加载**，不可替换为其他 skill。
>
> 格式：`本轮任务涉及 {域1(编号)} + {域2(编号)} ，{action} 操作，第 N 轮 → 加载 dev-il2cpp → dev-xxx-expert`
> 已在该域中的简单回复（"继续"/"ok"/"A"）不需要重复输出。
>
> **执行协议（🔴 每一步必须执行）**：
> 0. 输出分类声明 → `echo "本轮任务涉及..." > .claude/.classified`；`echo 'loaded_expert:dev-il2cpp→{expert}' >> .claude/.classified`
> 1. **强制**：在分类声明后、任何其他工具使用前，调用 `Skill("dev-il2cpp")` 加载入口技能
>    — 这不是可选项。跳过此步骤会被 hook 拦截。
> 2. **🔴 强制阻断门**：按 dev-il2cpp 的路由协议加载对应知识
>    — 单域：读取对应 Expert 的 SKILL.md 注入领域知识
>    — 多域（≥2）：读取 `dev-il2cpp-core-agent` 的 SKILL.md 执行调度循环
>    — 验证方式：`.claude/.classified` 文件必须含 `loaded_expert:dev-il2cpp→{expert}` 行

## 全局优先级（强制）

P1（性能最优）> P2（方案完美性）> P3（HotUpdate 支持）。低优先级让位于高优先级，但高优先级方案不应故意破坏低优先级。

## Workflow 路由

- **单域** → 当前 Agent 自行实现
- **≥2 域** → 默认 Workflow 委托
- **连续 2 轮无进展** → 终止，需人工介入
- **深度调试触发**（以下任一走 Workflow，不得手动迭代）：
  - 同一 chunk fact 失败超过 5 轮
  - 根因横跨 ≥2 域（如 CodeGen + 运行时 + 构建）
  - 当前 Agent 连续 3 次尝试未取得进展

详见 `skills/discovery/routing-rules.md`。

## 架构优先开发

开发新翻译路径前，必须执行 `dev-architecture-first-development` 完成架构查询，确认与既有架构一致。禁止未查表直接实现。

## 完成前自测

1. **查 wiki 要求** → 2. **无要求走 TDD**（先写失败测试）→ 3. **验证管线**（跑 foundation-dll pipeline）
2. **统一测试入口**：改码后先跑 `python tests/runner/test_driver.py --layer unit` 自测。
   出现**非 known 的 `[FAIL]`**（`OVERALL: FAILED`）即为真实回归，须修复后再推。
   报告见 `tests/runner/test-report.json`；CI 走 `scripts/ci_test.py --preset <x>`。
   详见 [`wiki/06-测试验证/统一测试入口.md`](wiki/06-测试验证/统一测试入口.md)（含 no-skip 规则）。

## AI Agent Bug 修复三规则

1. **系统性调试**：根因调查 → 模式分析 → 假设验证 → 实现修复（禁止跳过第一步）
2. **根因记录**：commit message 必须含 `root_cause` / `fix_strategy` / `regression_check`
3. **三次修复规则**：同 bug 第三次仍未解决，禁止再尝试。先调用 `dev-brainstorm` 或 `dev-architecture-first-development`

## Trace 优先调试

遇到失败/编译错/崩溃，先查 trace 系统：`run trace` → `trace-analyze`。信息不足时才加 print/log。

## 知识记录

- 新翻译路径 → `wiki/03-功能模块/il2cpp-核心架构/il2cpp-核心翻译路径参考.md`
- 设计讨论 → `docs/discuss/`
- 每个功能的 STATUS.md 必须含 `## 架构映射` 小节

## 技能调用

技能源码在 `.ai/skills/library/skills/`（git 跟踪）。发现流程：
1. 读 `.ai/skills/discovery/skill-index.md`（预加载）
2. 匹配领域 → 读对应 `registries/<domain>.md`
3. 找到 skill → 读 `library/skills/<name>/SKILL.md`

## 多域 Workflow 委托

≥3 域或深度调试条件满足时，启动 Workflow：
```
Phase 1 — 并行域内调查（每个 Expert 独立分析）
Phase 2 — 汇聚根因（synthesize agent）
Phase 3 — 按方案逐域修复
```

## 禁止 git stash（强制）

严禁 `git stash`。Stash 隐藏未提交变更，`git stash drop` 造成不可逆数据损失。切换上下文用 `git worktree` 或显式提交。

## 统一内存分配约束（强制）

1. **禁止全局 `operator new/delete` 重载** — 污染引擎分配行为
2. **codegen 输出必须用 `CHAOS_IL2CPP_` 宏**（`CHAOS_IL2CPP_NEW_GC` / `CHAOS_IL2CPP_MALLOC`）
3. **禁止跨域分配/free 不匹配** — GC 域→GC 回收，Domain 域→heap->Destroy()，Raw 域→free
4. 新分配策略通过 `GcAllocate`/`GcAllocateAtomic` 内部切换，对 codegen 透明

详见 `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/17-统一内存分配体系.md`。

## 四层架构红线（强制）

| 层 | 路径 | 允许产出 | 红线 |
|----|------|---------|------|
| **ATG** | `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/` | `.cs`, `.csproj`, `.json` | 不得生成 C++ |
| **Codegen** | `src/managed/Chaos.IL2CPP.Generator/` | `.generated.cpp`, `.generated.h`, `.json` | 产出必须自包含 |
| **TPG** | `src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/` | `.cpp`, `.h` (via Scriban), `.cmake` | 不得修改 `.generated.*` |
| **Python** | `testing/foundation-dll/verification/` | `.py`, `.json`, `.yaml` | 不得 `write_text` 写入 `.cpp`/`.h` |

跨层写入必须加 `# BOUNDARY_OVERRIDE: issues/NNN` + `Reason:` + `Expires:`。参见 `testing/foundation-dll/verification/preflight/check_layer_boundaries.py`。

## 编码规范（wiki 链接）

以下规范已移入 wiki，写对应文件前先查阅：

| 规范 | wiki 路径 |
|------|----------|
| Native 调试/LOG/PROFILE_SCOPE | `wiki/调试与性能分析规范.md` |
| 命名空间/Include/extern C | `wiki/C++编码规范.md` |
| `unordered_map` 选型 | `wiki/unordered_map选型规则.md` |
| SDK/TPG 输出边界 | `wiki/SDK-TPG边界约束.md` |
