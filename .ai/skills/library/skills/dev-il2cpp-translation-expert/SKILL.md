---
name: dev-il2cpp-translation-expert
description: dev-il2cpp-translation-expert — il2cpp 翻译专家
---

> ⚠️ **本文件通过 Skill 工具加载，作用是注入领域知识到当前对话上下文。**
> 本文件**不是可执行的 agent**。当前 Agent 需阅读下方"执行流程"作为实现参考。
>
> **当前 Agent 请做**：阅读知识域和约束 → 自行实现代码 → 自行验证
> **当前 Agent 不要做**：加载后等待"Expert 自动执行"——它不会，Skill 只加载文本。
>

# dev-il2cpp-translation-expert — il2cpp 翻译专家

## 领域边界

### 我负责的
- 新 IL 指令的翻译路径设计
- 修改现有翻译行为（Planning / Emission / RuntimeSupport 文件）
- 新增 Emission 或 Planning 策略
- 修改 codegen 桥接合约（`codegen_bridge.h`、`runtime_abi.h` 中与翻译相关的部分）
- 翻译路径一致性验证

### 我不负责的（超出以下范围 → 标记 remaining，回 Dispatcher 重新分发）

- **运行时实现**（修改 interpreter / runtime-core 的执行逻辑）→ 超出范围，标记 remaining，原因：需要运行时域知识
- **C# codegen 工具链修改**（T4 模板、NativeAot lowering 管线）→ 超出范围，标记 remaining，原因：需要 CodeGen 域知识
- **GC/内存分配相关** → 超出范围，标记 remaining，原因：需要 GC 域知识
- **测试治理**（subject/manifest/runner）→ 超出范围，标记 remaining，原因：需要测试治理域知识
- **编译失败 / codegen stub**（LNK 错误、C++ 编译错、CMake 错误、dotnet build 失败）→ 超出范围，标记 remaining，原因：需要构建修复域知识

## 输出格式（Dispatcher 回读用）

每个 Expert 处理完任务后，必须在当前上下文中输出：

```
✅ done: [已处理的子任务 ID 列表]
⏳ remaining: [未处理的子任务 ID 列表 + 原因]
```

---

## 触发条件

- 从 `dev-il2cpp-core-agent` 路由到本 expert
- 用户直接输入 `/dev-il2cpp-translation-expert`
- 用户输入包含：新 IL 指令、翻译路径、Emission、Planner、Lowering、翻译策略、codegen_bridge

---

---REF---

## 核心流程

### 🚨 第一步：输出分类声明（强制阻断）

**在使用任何工具之前，必须先输出：**

```
classification: domains=[翻译] mode=knowledge-inject expert=dev-il2cpp-translation-expert
```

**不输出分类声明，不得进行任何文件操作。**

---

本 expert 的核心是**封装 `dev-architecture-first-development` 的三阶段流程**，并在此基础上增加翻译领域的专业知识。

### Step 0：前置检查

1. 确认 `dev-architecture-first-development` 的三条禁止规则未被违反：
   - ❌ 跳过架构查询直接实现
   - ❌ 未确认架构路径一致性前修改代码
   - ❌ 新翻译路径不记录到 wiki/STATUS.md

2. 如果有进行中的设计讨论在 `docs/discuss/`，先了解当前共识

### Step 1：架构查询（Architecture Query）

执行 `dev-architecture-first-development` 的 Phase 1：

1. **优先级约束检查**：
   - 是否会引入性能降级？如果有，是否有合理理由？
   - 是否有替代方案能在同等性能下获得更好的架构完整性？
   - 如果涉及 hotupdate，是否需要额外支持？

2. **读取翻译路径参考**：
   - 加载 [`wiki/03-功能模块/06-il2cpp核心架构/02-翻译路径参考/INDEX.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/02-翻译路径参考/INDEX.md)
   - 找到目标 IL 特性在哪个翻译表中（12 个翻译表之一）
   - 记录翻译策略名称、Planning 文件路径、Emission 文件路径、已知约束

3. **新路径处理**：
   - 如果目标 IL 在表中不存在 → 标记为"新路径"
   - 读取 [`03-架构约束/01-翻译层硬性约束.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/03-架构约束/01-翻译层硬性约束.md) 确认不违反已知约束
   - 在 `docs/discuss/` 创建设计讨论文档
   - **等待用户确认后再进入 Step 2**

4. **加载相关管线文档**（根据目标 IL 指令类型选择性加载）：
   - 调用/字段指令 → [`01-翻译管线/03-求值栈与ABI调用模型.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/03-求值栈与ABI调用模型.md)
   - 异常相关 → [`01-翻译管线/05-异常处理架构.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/05-异常处理架构.md)
   - 泛型相关 → [`01-翻译管线/08-泛型执行模型.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/08-泛型执行模型.md)
   - 委托相关 → [`02-翻译路径参考/05-委托与运行时Helper翻译表.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/02-翻译路径参考/05-委托与运行时Helper翻译表.md)
   - 字符串相关 → [`01-翻译管线/06-字符串ID系统.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/06-字符串ID系统.md)
   - 结构化控制流 → [`02-翻译路径参考/09-结构化控制流恢复翻译表.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/02-翻译路径参考/09-结构化控制流恢复翻译表.md)

### Step 2：路径匹配与实现（Path Matching）

1. 确认使用的 Planning / Emission / RuntimeSupport 文件
2. 确认所有已知约束（核对 5 类架构约束）
3. 如果涉及新的 C++ 运行时功能，同步检查：
   - `contracts/native/v0/codegen_bridge.h` 和 `runtime_abi.h` 合约边界
   - `build/architecture.yaml` 依赖约束
4. 按既有模式实现代码

### Step 3：一致性验证（Consistency Verification）

### snapshot 基线变更说明

如果 snapshot 测试基线发生变化，必须在 STATUS.md 中说明变化原因：

```
## Snapshot 基线变更说明
- 变更的夹具: [N 个]
- 变化类型: 结构变化 / 新增指令 / 优化降级
- 变化原因: [简要说明]
- 是否预期: 是 / 否（需要追踪）
```


实现完成后逐项检查：

```
1. files_affected — 是否修改了非声明文件？
2. constraints — 是否违反了任何架构约束？
3. patterns — 是否引入了禁止模式？
4. new_constraints — 是否有新约束需要记录？
5. snapshot_tests — 如果改 emitter/planner，跑快照测试
6. wiki_entry — wiki 是否需要同步更新？
```

---

## 执行前 Checklist

```
□ 目标 IL 特性是否在 02-翻译路径参考/ 表中？
□ 如果否，是否已在 docs/discuss/ 创建了设计讨论？
□ 是否加载了所有相关架构约束（03-架构约束/）？
□ 是否确认了 Planning / Emission / RuntimeSupport 文件路径？
□ 是否检查过 codegen_bridge.h 和 runtime_abi.h 的合约边界？
```

## 执行后 Checklist

```
□ STATUS.md 是否包含 ## 架构映射 小节？
□ files_affected — 是否修改了非声明文件？
□ constraints — 是否违反了任何架构约束？
□ patterns — 是否引入了禁止模式？
□ new_constraints — 是否有新约束需要记录？
□ snapshot_tests — 如果改 emitter/planner，跑快照测试
□ wiki_entry — wiki 是否需要同步更新？
```

---

## 输出约束

1. **STATUS.md 必须包含 `## 架构映射` 小节**，格式如下：
   ```markdown
   ## 架构映射
   - il_feature: [IL 特性名]
   - translation_path: [翻译策略名]
   - planner: [Planning 文件路径::方法名]
   - emitter: [Emission 文件路径::方法名]
   - runtime_support: [RuntimeSupport 文件路径 / n/a]
   - constraints_followed: [已确认的约束列表]
   - new_constraints: [新发现的约束 / 无]
   - consistency_check: [ok / issues:...]
   - snapshot_tests: [passed / baseline_updated / n/a]
   - wiki_entry: [ok / updated:<path> / needs-update:<path>]
   - design_discussion: [链接 / n/a]
   ```

2. **如果涉及 codegen 桥接合约变更**，必须通知下游（当前为人工通知，CodeGen Expert 创建后自动路由）

3. **Wiki 一致性**：新路径稳定后，必须触发 `dev-project-wiki-maintenance` 更新 `02-翻译路径参考/` 对应文档

4. **commit message 要求** — 修改完成后必须包含三段式根因：
   - `root_cause` — 一句话根因
   - `fix_strategy` — 修复策略
   - `regression_check` — 验证范围

---

## 集成点

| 上游 | 本 skill | 下游 |
|------|----------|------|
| `dev-il2cpp-core-agent` → 翻译路由 | **dev-il2cpp-translation-expert** | `dev-project-wiki-maintenance` |
| 用户直接输入 | | `dev-architecture-first-development`（底层流程） |
| | | `dev-project-test-governance`（涉及测试时） |
