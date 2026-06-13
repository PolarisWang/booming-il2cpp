export const meta = {
  name: 'dual-expert',
  description: '双 Expert 并行调查 + 串行修复',
  phases: [
    { title: 'Parallel Investigation' },
    { title: 'Sequential Fix' },
  ],
}

// agents: [{ name, skillMd, task }]
export async function run(agents, args) {
  phase('Parallel Investigation')
  const results = await parallel(
    agents.map(a => () =>
      agent([
        '你是 ' + a.name + '。以下是你的领域知识：',
        a.skillMd,
        '---',
        '当前任务：' + a.task,
        '请分析问题并输出诊断结果，不要执行修改。',
      ].join('\n'), { schema: { type: 'object', properties: { diagnosis: { type: 'string' }, files_to_modify: { type: 'array', items: { type: 'string' } }, estimated_effort: { type: 'string' } } } })
    )
  )

  phase('Sequential Fix')
  for (let i = 0; i < agents.length; i++) {
    const diag = results[i]
    if (!diag) continue
    await agent([
      '你是 ' + agents[i].name + '。',
      '诊断结果：' + diag.diagnosis,
      '需修改文件：' + (diag.files_to_modify || []).join(', '),
      '---',
      '当前任务：' + agents[i].task,
      '请执行修改。完成后输出：',
      '✅ done: [已处理的子任务]',
      '⏳ remaining: [未处理的子任务 + 原因]',
    ].join('\n'))
  }
}
