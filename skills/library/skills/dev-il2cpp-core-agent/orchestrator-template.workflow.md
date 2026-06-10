# dev-il2cpp-orchestrator — Workflow 编排脚本

> 用于复杂跨域 il2cpp 开发任务（涉及 3+ 域）的 Workflow 编排。
> 使用方式：通过 `dev-il2cpp-core-agent` 自动触发，或用户直接作为 Workflow 调用。

## 触发条件

以下条件满足任意两个时，Controller 应提示用户是否使用 Orchestrator：
1. 涉及 **3+ 个域**（翻译 + 运行时 + GC / CodeGen + 测试 + 热更新 等）
2. 需要修改 **10+ 文件**
3. 修改涉及 codegen + runtime **接口变更**（需要 ABI 协调）
4. 用户明确要求"并行"或"加速"

---

## 编排流程

```
export const meta = {
    name: 'dev-il2cpp-orchestrator',
    description: '编排多个 Expert Agent 并行处理复杂跨域 il2cpp 开发任务',
    phases: [
        { title: '分解' },
        { title: '执行' },
        { title: '合并' },
        { title: '验证' },
    ],
}
```

### Phase 1: 任务分解（Decompose）

主 Agent 分析用户输入的任务，将其分解为 N 个独立子任务：

```
phase('分解')
const task = args // 用户的原始任务描述

// 任务分解逻辑（根据关键词将任务拆分为域专属子任务）
const domains = identifyDomains(task) // 返回 ["translation", "runtime", "gc", ...]

log(`识别到 ${domains.length} 个域: ${domains.join(', ')}`)
log(`本次任务将由 ${domains.length} 个 Expert Agent 协作完成`)
```

**分解示例**：

| 原始任务 | 子任务 1 | 子任务 2 | 子任务 3 |
|---------|---------|---------|---------|
| "翻译新 IL calli + 改 runtime helper + 加 GC 写屏障" | 翻译专家: 完成 calli 翻译路径 | 运行时专家: 实现 helper | GC 专家: 添加写屏障 |
| "新增 COM interop + codegen 输出 + foundation-dll 测试" | 翻译专家: COM 翻译路径 | CodeGen 专家: 模板修改 | 测试专家: foundation-dll |
| "热更新 PatchLoader 报错 + interpreter 崩溃 + stress test 回归" | 热更新专家: PatchLoader fix | 调试专家: interpreter crash | GC 专家: stress test 回归 |

### Phase 2: 并行执行（Parallel Execution）

将子任务分派到对应的 Expert Agent 并行执行：

```
phase('执行')

// 定义子任务（根据实际任务动态生成）
const subtasks = [
    { domain: 'translation', prompt: '...', expert: 'dev-il2cpp-translation-expert' },
    { domain: 'runtime', prompt: '...', expert: 'dev-il2cpp-runtime-expert' },
    { domain: 'gc', prompt: '...', expert: 'dev-il2cpp-gc-expert' },
    // ... 由任务分解阶段动态生成
]

// 并行执行所有 Expert Agent
const results = await parallel(subtasks.map(st => () =>
    agent(st.prompt, {
        label: `${st.domain}: ${st.prompt.substring(0, 60)}...`,
        phase: '执行',
        schema: EXPERT_RESULT_SCHEMA,
    })
))

log(`完成 ${results.filter(Boolean).length}/${subtasks.length} 个子任务`)
```

### Phase 3: 结果合并（Merge & Conflict Detection）

收集各 Expert Agent 的输出，检测冲突：

```
phase('合并')

// 合并所有结果
const merged = mergeResults(results.filter(Boolean))

// 冲突检测
const conflicts = detectConflicts(merged)
if (conflicts.length > 0) {
    log(`检测到 ${conflicts.length} 个冲突:`)
    conflicts.forEach(c => log(`  - ${c.description}`))
    // 冲突由 Controller 提示用户决策
}
```

**冲突类型**：
- ABI 不兼容（codegen 改了接口，runtime 没跟上）
- Symbol 命名冲突（两个 Expert 生成了同名 symbol）
- 合约版本不匹配（codegen_bridge.h 状态码冲突）
- 资源竞争（两个 Expert 修改了同一文件）

### Phase 4: 集成验证（Integration Verification）

```
phase('验证')

// 集成编译
const buildResult = await agent('Build and run integration tests', {
    label: '集成编译验证',
    schema: BUILD_RESULT_SCHEMA,
})

if (!buildResult.success) {
    log(`集成验证失败: ${buildResult.error}`)
    // 提示用户是否进入修复循环
} else {
    log('集成验证通过 ✓')
}

log('=== 结果摘要 ===')
results.filter(Boolean).forEach(r => {
    log(`[${r.domain}] ${r.summary}`)
})
return { results, conflicts, buildVerified: buildResult?.success }
```

---

## Expert Agent 结果 Schema

```
const EXPERT_RESULT_SCHEMA = {
    type: 'object',
    properties: {
        domain: { type: 'string' },
        summary: { type: 'string' },
        filesModified: {
            type: 'array',
            items: { type: 'string' },
        },
        constraints: {
            type: 'array',
            items: {
                type: 'object',
                properties: {
                    file: { type: 'string' },
                    description: { type: 'string' },
                },
            },
        },
        newContracts: {
            type: 'array',
            items: { type: 'string' },
        },
        abiChanges: {
            type: 'boolean',
        },
        success: { type: 'boolean' },
    },
    required: ['domain', 'summary', 'filesModified', 'success'],
}
```

---

## 使用方式

### 方式 1：通过 Controller 自动触发

`dev-il2cpp-core-agent` 检测到复杂跨域任务时，询问用户：

> 此任务涉及 3 个域（翻译 + 运行时 + GC），建议使用 Workflow Orchestrator 并行执行。
> 是否使用 Orchestrator？(yes/no)
>
> 如选 no，将走 dev-roadmap 生成多步计划依次执行。

### 方式 2：直接作为 Workflow 调用

运行时脚本为 `orchestrator.workflow.js`（同目录），通过 Workflow 工具调用：

```
请使用 Workflow 工具，args 传递任务描述：
{
    scriptPath: "skills/library/skills/dev-il2cpp-core-agent/orchestrator.workflow.js",
    args: {
        task: "翻译新 IL calli + 改 runtime helper + 加 GC 写屏障",
        domains: ["translation", "runtime", "gc"]
    }
}
```

---

## 冲突检测规则

当多个 Expert Agent 并行工作后，使用以下规则检测冲突：

| 冲突类型 | 检测方法 | 处理方式 |
|---------|---------|---------|
| **ABI 不兼容** | CodeGen Expert 改了接口签名，Runtime Expert 未同步更新 | 标记冲突 → 提示人工决策谁优先 |
| **文件竞态** | 两个 Expert 修改了同一文件（通过 `filesModified` 取交集） | 串行执行冲突文件 → 对比两次 diff |
| **Symbol 冲突** | 生成的 symbol 名称重复（`chaos_*_` prefix 相同但 body 不同） | 按"最后写入为准"合并 |
| **合约版本** | `codegen_bridge.h` 状态码在两个 Expert 中分别添加 | 重新编号 → 确保状态码唯一 |
| **命名空间冲突** | 两个 Expert 在相同 namespace 中添加了同名类型 | 人工审查 → 重命名 |
| **测试基线冲突** | CodeGen Expert 改了 snapshot 基线，Test Expert 未更新 | 以 CodeGen Expert 的基线为准，通知 Test Expert |

1. **并行有风险** — 多个 Expert 同时修改不同文件通常安全，但修改同一文件时会冲突。冲突检测在 Phase 3 处理。
2. **ABI 协调** — 如果 codegen Expert 和 runtime Expert 同时修改了同一合约接口，必须在合并阶段人工决策。
3. **兜底回退** — 如果任何 Expert Agent 返回 BLOCKED/NEEDS_CONTEXT，整个 Orchestrator 应回退到串行处理。
4. **不适用于简单任务** — 单域或双域任务用 Controller 串行路由即可，不需要 Orchestrator。
