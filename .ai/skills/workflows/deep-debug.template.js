export const meta = {
  name: 'deep-debug',
  description: '深度调试 Workflow — 并行域内调查 → 汇聚根因 → 分域修复',
  phases: [
    { title: 'Domain Investigation' },
    { title: 'Root Cause Synthesis' },
    { title: 'Domain Fix' },
  ],
}

// agents: [{ name, skillMd, task }] — 2-3 agents for parallel investigation
export async function run(agents, args) {
  phase('Domain Investigation')
  const results = await parallel(
    agents.map(a => () =>
      agent([
        '你是 ' + a.name + '。以下是你的领域知识：',
        a.skillMd,
        '---',
        '当前调试任务：' + a.task,
        '请分析根因，输出诊断结果，不要执行修改。',
      ].join('\n'), { schema: { type: 'object', properties: { diagnosis: { type: 'string' }, root_cause: { type: 'string' }, files_to_modify: { type: 'array', items: { type: 'string' } }, estimated_effort: { type: 'string' } } } })
    )
  )

  phase('Root Cause Synthesis')
  const validResults = results.filter(Boolean)
  const synthesisPrompt = [
    '你是一个调试汇聚 agent。请综合以下多域诊断结果，输出统一的根因分析和修复方案。',
    '---',
    ...validResults.map((r, i) => `=== Expert ${agents[i].name} ===\n${r.diagnosis}\n根因: ${r.root_cause}\n涉及文件: ${(r.files_to_modify || []).join(', ')}`),
    '---',
    '请输出：',
    '1. 综合根因 (root_cause)',
    '2. 修复策略 (fix_strategy)',
    '3. 回归验证范围 (regression_check)',
    '4. 各域修复顺序',
  ].join('\n')

  const synthesis = await agent(synthesisPrompt, {
    schema: {
      type: 'object',
      properties: {
        root_cause: { type: 'string' },
        fix_strategy: { type: 'string' },
        regression_check: { type: 'string' },
        fix_order: { type: 'array', items: { type: 'string' } },
      },
    },
  })
  if (!synthesis) {
    // 根因汇聚失败 — 这是严重问题，不可静默降级
    log('❌ 根因汇聚失败：合成 agent 未返回有效结果')
    log('   各域原始诊断：')
    validResults.forEach((r, i) => {
      log(`   [${agents[i].name}] ${(r.diagnosis || '').substring(0, 120)}`)
    })
    throw new Error(
      `deep-debug 根因汇聚失败：合成 agent 未返回结果。` +
      `需要人工审查 ${validResults.length} 个域的诊断后再决定修复方案。`
    )
  }

  phase('Domain Fix')
  const fixOrder = synthesis.fix_order || agents.map(a => a.name)
  for (const expertName of fixOrder) {
    const idx = agents.findIndex(a => a.name === expertName)
    if (idx === -1) continue
    const diag = validResults[idx]
    if (!diag) continue
    await agent([
      '你是 ' + agents[idx].name + '。',
      '综合根因：' + synthesis.root_cause,
      '修复策略：' + synthesis.fix_strategy,
      '你的诊断：' + diag.diagnosis,
      '需修改文件：' + (diag.files_to_modify || []).join(', '),
      '---',
      '当前任务：' + agents[idx].task,
      '请按根因分析和修复策略执行修改。完成后输出：',
      '✅ done: [已处理的子任务]',
      '⏳ remaining: [未处理的子任务 + 原因]',
    ].join('\n'))
  }
}
