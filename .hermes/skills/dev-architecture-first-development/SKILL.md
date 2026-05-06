---
name: dev-architecture-first-development
description: 开发 il2cpp 功能前必须先查询架构路径，避免方案反复与前后不自洽。三步流程：架构查询 → 路径匹配 → 一致性验证。
---

# 架构优先开发

## 触发条件

本技能在以下情况必须调用：
- 开发一个新的 IL 特性翻译路径
- 修改现有翻译行为（Planning / Emission / RuntimeSupport 中的任何文件）
- 新增一种 Emission 或 Planning 策略
- 修改 c++ 运行时中与代码生成对应的行为

## 禁止

- 跳过步骤 1（架构查询）直接进入实现
- 在未确认架构路径一致性前修改代码
- 将新翻译路径直接写入代码而不记录到 wiki 或 STATUS.md

## 三阶段流程

### 步骤 1：架构查询

1. **优先级约束检查**：开始查询前，先确认当前任务的三条全局优先级对齐情况：
   - 是否会引入性能降级？如果有，是否有合理理由？
   - 是否有替代方案能在同等性能下获得更好的架构完整性或 hotupdate 支持？
   - 如果涉及 hotupdate，是否需要额外的插件/补丁支持？
2. 读取 `wiki/03-功能模块/il2cpp-核心架构/il2cpp-核心翻译路径参考.md`
2. 识别目标 IL 特性（call / callvirt / newobj / ldstr / stsfld / ldsfld / throw / rethrow / box / unbox / delegate / 间接调用 / ldelem / stelem 等）
3. 在路径参考表中找到对应条目，记录以下信息：
   - 翻译策略名称
   - Planning 文件路径
   - Emission 文件路径
   - 已知的所有约束
4. 如果目标 IL 特性在表中不存在：
   - 标记为"新路径"
   - 读取 `wiki/03-功能模块/il2cpp-核心架构/il2cpp-架构约束与边界.md`，确认不会违反已知约束
   - 在 `docs/discuss/` 创建设计讨论文档，记录翻译方案
   - 等待设计讨论达成一致后再进入步骤 2

### 步骤 2：路径匹配与实现

1. 确认使用的 Planning / Emission / RuntimeSupport 文件
2. 确认遵循了所有已知约束（核对 `il2cpp-架构约束与边界.md`）
3. 如果翻译路径涉及新的 C++ 运行时功能，同步确认：
   - `contracts/native/v0/codegen_bridge.h` 和 `runtime_abi.h` 中的合约边界
   - `build/architecture.yaml` 中的依赖约束
4. 按既有模式实现代码

### 步骤 3：一致性验证

实现完成后逐项检查：

1. **files_affected** — 是否修改了翻译路径声明外的文件？（如有，说明理由）
2. **constraints** — 是否违反了 `il2cpp-架构约束与边界.md` 中任何已知约束？
3. **patterns** — 是否引入了架构约束中禁止的模式（如 `IsNullOrWhiteSpace` 用于 SubjectId、`ContainsValue` O(n) 扫描等）？
4. **new_constraints** — 是否产生了新的约束或边界？如有：
   - 记录到 `docs/discuss/` 对应的设计讨论（或新建讨论文档）
   - 安排后续更新 `il2cpp-架构约束与边界.md` 和 `il2cpp-核心翻译路径参考.md`

验证结果写入 STATUS.md。

### Wiki 一致性检查（必须）

架构查询完成后，必须判断并记录：

1. **本次翻译路径在 wiki 中是否存在对应条目？**
   - 如果存在但描述不准确 → STATUS.md 中记录待修复项
   - 如果不存在（新路径） → 实现完成后更新 `wiki/03-功能模块/06-il2cpp核心架构/02-翻译路径参考/` 对应翻译表
2. **约束/架构文档是否需要同步更新？**
   - 如果新增约束 → 更新 `03-架构约束/` 下对应文档
   - 如果新增决策 → 更新 `04-历史决策/` 下对应文档
3. **运行自动化检查**：执行 `python build/check_wiki.py --ci` 验证 opcode 覆盖一致性
4. 在 STATUS.md 的 `## 架构映射` 中用 `wiki_entry: [ok / updated:<path> / needs-update:<path>]` 标记状态

## STATUS.md 记录格式

每个涉及 il2cpp 翻译逻辑的任务，STATUS.md 必须包含 `## 架构映射` 小节：

```markdown
## 架构映射
- il_feature: [IL 特性名，如 callvirt]
- translation_path: [翻译策略名，如 vtable_dispatch]
- planner: [Planning 文件路径::方法名，如 InvocationPlanning.cs::TryResolveVirtualDispatch]
- emitter: [Emission 文件路径::方法名，如 MethodEmission.cs::EmitCallInstruction]
- runtime_support: [RuntimeSupport 文件路径 / n/a]
- constraints_followed: [已确认的约束列表]
- new_constraints: [新发现的约束 / 无]
- consistency_check: [ok / issues:...]
- wiki_entry: [ok / updated:<path> / needs-update:<path>]
- design_discussion: [链接到 docs/discuss/ 的讨论文档 / n/a]
```

## 检查清单速查

步骤 1 完成后确认以下问题：

- [ ] 目标 IL 特性是否在翻译路径参考表中？
- [ ] 如果否，是否已在 docs/discuss/ 创建设计讨论？
- [ ] 是否读取了架构约束与边界文档？
- [ ] 是否已知所有相关的 Planning / Emission 文件？

步骤 3 完成后确认以下问题：

- [ ] 是否修改了预期外的文件？
- [ ] 是否违反了任何已知约束？
- [ ] 是否引入了禁止模式？
- [ ] 是否产生了需要记录的新约束？

## 集成

- `dev-executing-plans` — 在实现前应先调用本技能完成架构查询
- `dev-subagent-driven-development` — 控制器在派发子 Agent 前应调用本技能
- `dev-project-wiki-maintenance` — 新路径稳定后，将知识同步到 wiki
- `dev-brainstorm` — 当需要设计新翻译路径时
