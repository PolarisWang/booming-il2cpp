export const meta = {
  name: 'code-review-fixer',
  description: 'Route code review findings to domain experts and fix in parallel',
  phases: [
    { title: 'Classify', detail: 'Route findings to expert agents' },
    { title: 'Fix', detail: 'Experts fix in parallel' },
    { title: 'Verify', detail: 'Layer boundary + compile checks' },
  ],
}

// args = { findings: [{ severity, file, line, issue, description }] }
// findings come from code review tool output

phase('Classify')

const RULES = [
  { pattern: /Chaos\.IL2CPP\.Generator.*\.cs$/, expert: 'dev-il2cpp-codegen-expert' },
  { pattern: /Chaos\.IL2CPP\.Tools\.(TestProjectGenerator|AutoTestGenerator).*\.cs$/, expert: 'dev-il2cpp-build-fixer' },
  { pattern: /testing\/foundation-dll\/verification\/stages\/.*\.py$/, expert: 'dev-il2cpp-verification-pipeline' },
  { pattern: /testing\/.*\.py$/, expert: 'dev-il2cpp-build-fixer' },
  { pattern: /build_presets\.py$/, expert: 'dev-il2cpp-platform-expert' },
  { pattern: /runtime-core\/.*\.(cpp|h)$/, expert: 'dev-il2cpp-runtime-expert' },
  { pattern: /AsyncCoroutineEmitter.*\.cs$/, expert: 'dev-il2cpp-codegen-expert' },
  { pattern: /scripts\/.*\.py$/, expert: 'dev-il2cpp-platform-expert' },
  { pattern: /src\/managed\/.*\.cs$/, expert: 'dev-il2cpp-codegen-expert' },
  { pattern: /\.scriban$/, expert: 'dev-il2cpp-build-fixer' },
  { pattern: /NativeAotLoweringPlanner/, expert: 'dev-il2cpp-codegen-expert' },
  // CMakeLists.txt / TPG-generated native artifacts → build-fixer
  { pattern: /CMakeLists\.txt/, expert: 'dev-il2cpp-build-fixer' },
  { pattern: /object_stubs\.cpp/, expert: 'dev-il2cpp-runtime-expert' },
  { pattern: /\.claude\//, expert: 'dev-il2cpp-build-fixer' },
  { pattern: /\.dll$/, expert: 'dev-il2cpp-build-fixer' },
  { pattern: /CMakePresets\.json/, expert: 'dev-il2cpp-build-fixer' },
]

const expertGroups = {}
for (const finding of args.findings) {
  let assigned = false
  for (const rule of RULES) {
    if (rule.pattern.test(finding.file)) {
      expertGroups[rule.expert] = expertGroups[rule.expert] || []
      expertGroups[rule.expert].push(finding)
      assigned = true
      break
    }
  }
  if (!assigned) {
    log(`[code-review] Unrouted: ${finding.severity} ${finding.file}:${finding.line}`)
  }
}

log(`[code-review] Routed to ${Object.keys(expertGroups).length} experts: ${Object.keys(expertGroups).join(', ')}`)
for (const [expert, issues] of Object.entries(expertGroups)) {
  log(`[code-review]   ${expert}: ${issues.length} issues`)
}

phase('Fix')

// Domain scope guard: each expert may only modify files matching their domain
const DOMAIN_SCOPES = {
  'dev-il2cpp-codegen-expert': [/src\/managed\/.*\.cs$/, /NativeAotLoweringPlanner/],
  'dev-il2cpp-build-fixer': [/src\/tools\//, /CMakeLists/, /\.claude\//, /\.dll$/, /\.scriban$/],
  'dev-il2cpp-verification-pipeline': [/testing\/foundation-dll\/verification\/.*\.py$/, /scripts\/.*\.py$/],
  'dev-il2cpp-platform-expert': [/build_presets/, /scripts\/.*\.py$/, /CMakeLists/, /CMakePresets/],
  'dev-il2cpp-runtime-expert': [/runtime-core\/.*\.(cpp|h)$/, /object_stubs\.cpp/],
}

const fixResults = await parallel(Object.entries(expertGroups).map(([expert, issues]) => {
  // Warn about out-of-scope findings
  const scopes = DOMAIN_SCOPES[expert] || []
  for (const finding of issues) {
    const inScope = scopes.some(s => s.test(finding.file))
    if (!inScope) {
      log(`[code-review] ⚠️ ${expert} received out-of-scope finding: ${finding.file} (${finding.issue})`)
    }
  }
  return () =>
    agent(`You are a code review fixer in the '${expert}' domain. First load the expert SKILL.md with 'cat .ai/skills/library/skills/${expert}/SKILL.md' to inject domain knowledge, then fix these findings.

CRITICAL: Only modify files within your domain scope. Do NOT modify auto-generated test files
(testing/foundation-dll/*/chunks/*/managed/), and never wrap exceptions in try-catch to silence them.

${issues.map(i => `[${i.severity}] ${i.file}:${i.line} — ${i.issue || i.description}`).join('\n')}

For each finding:
1. Read the affected file at the reported line
2. Determine if the issue is real (check current file state — it may already be fixed)
3. If real, apply the fix
4. Verify the fix compiles (dotnet build or python syntax check as appropriate)
5. Report what was changed and why`,
    {
      label: expert,
      phase: 'Fix',
    })
}))

phase('Verify')

const verificationTasks = Object.keys(expertGroups).map(expert => () =>
  agent(`Verify all fixes in ${expert} domain compile correctly. Run dotnet build or python syntax check as appropriate.`, {
    label: `verify:${expert}`,
    phase: 'Verify',
  })
)

// Also run layer boundary check
verificationTasks.push(() =>
  agent(`Run the layer boundary preflight check (python testing/foundation-dll/verification/preflight/check_layer_boundaries.py --ci) and report any remaining violations.`, {
    label: 'layer-boundary',
    phase: 'Verify',
  })
)

const verifyResults = await parallel(verificationTasks)

// ── Commit & Push ──
const allPassed = verifyResults.every(Boolean) !== false
if (allPassed) {
  log('[code-review] All checks passed, committing and pushing...')
  const commitMsg = args.commitMessage || `fix: code review — ${args.findings.map(f => f.issue).filter(Boolean).join(', ').slice(0, 120)}`
  await agent(`Git operations:
1. git add -A
2. git commit -m "${commitMsg}" (include Co-Authored-By: Claude Fable 5 <noreply@anthropic.com>)
3. git push

Handle trivially: if nothing to commit, log "nothing to commit" and skip push.`, {
    label: 'git-commit-push',
    phase: 'Verify',
  })
} else {
  log('[code-review] Some checks failed — skipping commit. Manual review required.')
}

return {
  findingsCount: args.findings.length,
  expertsUsed: Object.keys(expertGroups),
  fixResults: fixResults.filter(Boolean),
  verifyResults: verifyResults.filter(Boolean),
  committed: allPassed,
}
