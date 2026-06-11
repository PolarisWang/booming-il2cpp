// dev-il2cpp-orchestrator — Workflow 编排脚本
// 用于多域复杂 il2cpp 开发任务的并行 Agent 委托执行。
// 使用方式：通过 dev-il2cpp-core-agent 自动触发（≥3域时）。
//
// args 格式：
// {
//   task: "用户原始任务描述",
//   domains: ["runtime", "codegen", "gc"],  // 已分类的域
// }

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

// ── 域→Expert 映射 ──────────────────────────────────────────────
const DOMAIN_EXPERT = {
  translation: 'dev-il2cpp-translation-expert',
  runtime:     'dev-il2cpp-runtime-expert',
  debug:       'dev-il2cpp-debug-expert',
  gc:          'dev-il2cpp-gc-expert',
  codegen:     'dev-il2cpp-codegen-expert',
  hotupdate:   'dev-il2cpp-hotupdate-expert',
  optimizer:   'dev-il2cpp-foundation-dll-optimizer',
  build:       'dev-il2cpp-build-fixer',
  test:        'dev-project-test-governance',
}

// Expert 结果 Schema（各子 Agent 必须按此格式返回）
const EXPERT_RESULT_SCHEMA = {
  type: 'object',
  properties: {
    domain:       { type: 'string' },
    summary:      { type: 'string' },
    filesModified:{ type: 'array', items: { type: 'string' } },
    success:      { type: 'boolean' },
    abiChanges:   { type: 'boolean' },
  },
  required: ['domain', 'summary', 'filesModified', 'success'],
}

// ── Phase 1: 任务分解 ───────────────────────────────────────────
phase('分解')
const { task, domains } = args
log(`原始任务: ${task}`)
log(`识别到 ${domains.length} 个域: ${domains.join(', ')}`)

// 为每个域生成子任务 prompt
function buildSubtaskPrompt(domain, originalTask) {
  switch (domain) {
    case 'translation':
      return `作为 il2cpp 翻译专家，处理以下任务中的翻译部分：\n${originalTask}\n\n要求：\n1. 先执行 dev-architecture-first-development 三阶段流程\n2. 确认翻译路径与既有架构一致\n3. 记录新路径到 STATUS.md`
    case 'runtime':
      return `作为 il2cpp 运行时专家，处理以下任务中的运行时部分：\n${originalTask}\n\n要求：\n1. 遵循 chaos::il2cpp::runtime_core 命名空间规范\n2. 热点路径注意 LOG_LEVEL 设置\n3. 合约变更同步更新 codegen_bridge.h`
    case 'debug':
      return `作为 il2cpp 调试专家，处理以下任务中的调试部分：\n${originalTask}\n\n要求：\n1. 先调用 dev-systematic-debugging 完成四阶段调试\n2. 确认根因、修复策略、回归检查\n3. 记录到 STATUS.md`
    case 'gc':
      return `作为 il2cpp GC 专家，处理以下任务中的 GC 部分：\n${originalTask}\n\n要求：\n1. 确认 CRAG GC 架构约束\n2. 涉及分代时检查 three-generational-gc-analysis\n3. stress test 验证`
    case 'codegen':
      return `作为 il2cpp CodeGen 专家，处理以下任务中的 codegen 部分：\n${originalTask}\n\n要求：\n1. 确认 NativeAotLoweringPlanner 文件布局\n2. 修改 T4 模板后运行 snapshot 测试\n3. CHAOS_IL2CPP_ 宏约束`
    case 'hotupdate':
      return `作为 il2cpp 热更新专家，处理以下任务中的热更新部分：\n${originalTask}\n\n要求：\n1. 确认 HotpatchDispatch 架构决策\n2. PatchLoader 修改需通过 hotupdate verification\n3. 注意 kHotpatchKeepNative/kHotpatchActive flags`
    default:
      return `处理以下任务中关于 ${domain} 的部分：\n${originalTask}`
  }
}

const subtasks = domains
  .filter(d => DOMAIN_EXPERT[d])
  .map(d => ({
    domain: d,
    expert: DOMAIN_EXPERT[d],
    prompt: buildSubtaskPrompt(d, task),
  }))

log(`生成 ${subtasks.length} 个子任务`)
subtasks.forEach(st => log(`  [${st.domain}] → ${st.expert}`))

// ── Phase 2: 并行执行 ───────────────────────────────────────────
phase('执行')

const results = await parallel(subtasks.map(st => () =>
  agent(st.prompt, {
    label: `${st.domain}: ${task.substring(0, 50)}...`,
    phase: '执行',
    schema: EXPERT_RESULT_SCHEMA,
  })
))

const succeeded = results.filter(Boolean)
log(`完成 ${succeeded.length}/${subtasks.length} 个子任务`)

// ── Phase 3: 结果合并 + 冲突检测 ───────────────────────────────
phase('合并')

// 文件冲突检测：找出被多个 Expert 修改的相同文件
const fileMap = {}
succeeded.forEach(r => {
  ;(r.filesModified || []).forEach(f => {
    if (!fileMap[f]) fileMap[f] = []
    fileMap[f].push(r.domain)
  })
})

const conflicts = Object.entries(fileMap)
  .filter(([, domains]) => domains.length > 1)
  .map(([file, domains]) => ({
    file,
    domains,
    description: `${file} 被 ${domains.join(', ')} 同时修改，需检查兼容性`,
  }))

if (conflicts.length > 0) {
  log(`检测到 ${conflicts.length} 个文件冲突:`)
  conflicts.forEach(c => log(`  - ${c.description}`))
} else {
  log('无文件冲突 ✓')
}

// ABI 变更检查
const abiChanges = succeeded.filter(r => r.abiChanges)
if (abiChanges.length > 0) {
  log(`⚠️ ABI 变更域: ${abiChanges.map(r => r.domain).join(', ')}`)
  log('需要集成验证确保 codegen ↔ runtime 合约一致')
}

// ── Phase 4: 集成验证 ───────────────────────────────────────────
phase('验证')

const buildResult = await agent(`集成验证：\n${task}\n\n修改涉及以下文件：\n${
  succeeded.flatMap(r => (r.filesModified || [])).join('\n')
}\n\n请执行：\n1. cmake build 确认编译通过\n2. 运行相关测试\n3. 返回成功/失败状态`, {
  label: '集成编译验证',
  schema: {
    type: 'object',
    properties: {
      success: { type: 'boolean' },
      buildErrors: { type: 'array', items: { type: 'string' } },
      testResults: { type: 'string' },
    },
    required: ['success'],
  },
})

if (buildResult?.success) {
  log('集成验证通过 ✓')
} else {
  log(`集成验证失败: ${(buildResult?.buildErrors || ['unknown']).join(', ')}`)
}

// ── 结果摘要 ────────────────────────────────────────────────────
log('=== 结果摘要 ===')
succeeded.forEach(r => log(`[${r.domain}] ${r.summary}`))
if (conflicts.length > 0) {
  log(`⚠️ ${conflicts.length} 个文件冲突需人工审查`)
}

return {
  subtaskCount: subtasks.length,
  succeeded: succeeded.length,
  results: succeeded,
  conflicts,
  buildVerified: buildResult?.success ?? false,
}
