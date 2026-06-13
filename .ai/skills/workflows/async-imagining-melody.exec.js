const meta = {
  name: 'async-imagining-melody',
  description: 'Execute all 5 phases of Chaos IL2CPP architecture refactor',
  phases: [
    { title: 'Phase 1', detail: 'Eliminate post-TPG patches' },
    { title: 'Phase 2', detail: 'ABI function table' },
    { title: 'Phase 3', detail: 'TPG simplification' },
    { title: 'Phase 4', detail: 'capabilities feedback' },
    { title: 'Phase 5', detail: 'Expert agent updates' },
  ],
}

async function run() {
  phase('Phase 1')
  log('P1a: Implement ChaosAbiExportCollector')
  const p1a = await agent(
    'Read src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ModuleRegistration.cs; Read src/managed/Chaos.IL2CPP.Generator/NativeAotEmitter.cs lines 300-450 and 830-874. Context: plan requires ChaosAbiExportCollector that scans generated C++ for chaos_external_runtime_* refs and emits complete extern declarations. Called from BuildTypeDeclarationsCode(). Implement using Write tool on a new partial class file NativeAotLoweringPlanner.ExportCollector.cs in the Emission directory.',
    { label: 'P1a', phase: 'Phase 1', isolation: 'worktree' }
  )

  const p1b = await agent(
    'Read testing/foundation-dll/verification/stages/build.py lines 600-720. Context: plan requires removing two post-TPG sections: (1) codegen copy workaround lines 609-620, (2) chaos_static_* auto-repair lines 629-682. These become unnecessary when codegen output is self-contained. Output JSON analysis of sections_to_remove and risks.',
    { label: 'P1b', phase: 'Phase 1', schema: { type: 'object', properties: { sections_to_remove: { type: 'array', items: { type: 'string' } }, risks: { type: 'array', items: { type: 'string' } } } } }
  )

  phase('Phase 2')
  const p2 = await agent(
    'Read contracts/native/v0/runtime_abi.h; Read src/native/runtime-core/core/abi_export.cpp. Context: plan requires RuntimeAbiV0->V1 upgrade. 9 new function pointers already added in commit 1ba478ce9. Remaining: rename to V1, add remaining ~30 pointers, switch codegen templates from direct includes to ABI table calls. Output JSON: completed_items, remaining_items, effort.',
    { label: 'P2', phase: 'Phase 2', schema: { type: 'object', properties: { completed_items: { type: 'array', items: { type: 'string' } }, remaining_items: { type: 'array', items: { type: 'string' } }, estimated_effort: { type: 'string' } } } }
  )

  phase('Phase 3')
  const p3 = await agent(
    'Read src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/Emission/CppProjectEmitter.cs lines 390-460. Context: plan requires simplifying BuildProject() by removing Ninja detection, VS env check, cmake retry. Output JSON.',
    { label: 'P3', phase: 'Phase 3', schema: { type: 'object', properties: { sections_to_simplify: { type: 'array', items: { type: 'object', properties: { location: { type: 'string' }, change: { type: 'string' }, risk: { type: 'string' } } } }, estimated_effort: { type: 'string' } } } }
  )

  phase('Phase 4')
  const p4 = await agent(
    'Grep DllScanner.cs for UnprobableMethods or capabilities; Read ConvertToCpp/ConvertToCppHandler.cs for capabilities.json generation. Context: ~70% done, needs DllScanner to actually READ capabilities.json. Output JSON.',
    { label: 'P4', phase: 'Phase 4', schema: { type: 'object', properties: { completed: { type: 'array', items: { type: 'string' } }, remaining: { type: 'array', items: { type: 'object', properties: { task: { type: 'string' }, file: { type: 'string' }, effort: { type: 'string' } } } } } } }
  )

  phase('Phase 5')
  const p5 = await agent(
    'Read .ai/skills/discovery/expert-registry.json. There is routing id=10 dev-il2cpp-codegen-capabilities without a physical library/skills/ dir. Create SKILL.md + skill.manifest.json for it. Read an existing expert like dev-il2cpp-simd-expert for format reference. Output what was created.',
    { label: 'P5', phase: 'Phase 5', isolation: 'worktree' }
  )

  log('P1a ' + (p1a ? 'DONE' : 'FAIL'))
  log('P1b analysis: ' + JSON.stringify(p1b))
  log('P2: ' + JSON.stringify(p2))
  log('P3: ' + JSON.stringify(p3))
  log('P4: ' + JSON.stringify(p4))
  log('P5: ' + (p5 ? 'Created' : 'FAIL'))

  return { p1b, p2, p3, p4, p5 }
}

module.exports = { meta, run }
