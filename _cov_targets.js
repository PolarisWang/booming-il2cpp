const fs = require('fs');
const xml = fs.readFileSync('tests/managed/Chaos.IL2CPP.CodeGen.Tests/CoverageResults/coverage.cobertura.xml', 'utf8');
const excluded = ['\\obj\\', '\\ReferenceProof\\', 'PatchData', 'NativeReferenceProofEmitter', 'RuntimeSkeleton', 'PlanBuilders', 'GeneratedPluginExports'];
let currentFile = null, inMethod = false, currentMethod = null, methods = [];

xml.split('\n').forEach(l => {
  const cf = l.match(/filename="([^"]+)"/);
  if (cf) { currentFile = cf[1]; return; }
  const mm = l.match(/name="([^"]+)".*signature="([^"]*)"/);
  if (mm) {
    if (currentFile && !excluded.some(p => currentFile.includes(p))) {
      inMethod = true;
      currentMethod = {file: currentFile, name: mm[1], sig: mm[2], hitLines: 0, totalLines: 0};
    }
    return;
  }
  if (inMethod && l.includes('</method>')) {
    if (currentMethod) methods.push(currentMethod);
    inMethod = false; currentMethod = null;
    return;
  }
  if (inMethod && currentMethod) {
    const lm = l.match(/line number="(\d+)" hits="(\d+)"/);
    if (lm) { currentMethod.totalLines++; if (parseInt(lm[2]) > 0) currentMethod.hitLines++; }
  }
});

// Find uncovered in specific small target files
const targets = ['ManagedClosureArtifactModels.cs', 'ManagedClosureModels.cs', 'ManagedSemanticWorldContracts.cs',
  'TypedIlAndAotCoreIrContracts.cs', 'LoaderModels.cs', 'ManagedMethodIdentityContracts.cs',
  'ManagedNaming.cs', 'TypeProviders.cs', 'LoaderStage.cs', 'MetadataTypeResolver.cs',
  'LoaderStage.CrossAssemblyInstantiation.cs', 'LoaderStage.AssemblyLoading.cs',
  'LoaderStage.GenericDemandGraph.cs', 'LoaderStage.MetadataResolution.cs',
  'EnumMetadataExtractor.cs', 'LinkerStage.Reachability.cs', 'CodeGenStage.cs',
  'NativeAotTemplateCatalog.cs', 'ManagedClosureContracts.cs', 'ContractSubject.cs',
  'ManagedClosureModels.cs'];

targets.forEach(sf => {
  const ms = methods.filter(m => m.file.includes(sf));
  if (ms.length === 0) { console.log(sf + ': no methods found'); return; }
  const hit = ms.reduce((s, m) => s + m.hitLines, 0);
  const tot = ms.reduce((s, m) => s + m.totalLines, 0);
  const pct = tot > 0 ? (hit/tot*100).toFixed(1) : 'N/A';
  console.log(sf + ': ' + hit + '/' + tot + ' = ' + pct + '%');
  const uncovered = ms.filter(m => m.hitLines === 0);
  uncovered.slice(0, 5).forEach(m => console.log('  UNCOVERED: ' + m.name + ' (' + m.totalLines + 'L)'));
  const partial = ms.filter(m => m.hitLines > 0 && m.hitLines < m.totalLines);
  partial.slice(0, 3).forEach(m => console.log('  PARTIAL: ' + m.name + ' (' + m.hitLines + '/' + m.totalLines + ')'));
});
