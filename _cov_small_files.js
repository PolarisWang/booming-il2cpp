const fs = require('fs');
const xml = fs.readFileSync('tests/managed/Chaos.IL2CPP.CodeGen.Tests/CoverageResults/cobertura.xml', 'utf8');
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

// Find uncovered methods in ManagedNaming.cs
console.log('=== ManagedNaming.cs uncovered methods ===');
methods.filter(m => m.file.includes('ManagedNaming.cs') && m.hitLines === 0)
  .forEach(m => console.log('  ' + m.name + ' | ' + m.totalLines + 'L'));

// Find uncovered in NativeAotEmitter.cs
console.log('=== NativeAotEmitter.cs uncovered methods ===');
methods.filter(m => m.file.includes('NativeAotEmitter.cs') && m.hitLines === 0)
  .forEach(m => console.log('  ' + m.name + ' | ' + m.totalLines + 'L'));

// Find uncovered in TypeProviders.cs
console.log('=== TypeProviders.cs uncovered methods ===');
methods.filter(m => m.file.includes('TypeProviders.cs') && m.hitLines === 0)
  .forEach(m => console.log('  ' + m.name + ' | ' + m.totalLines + 'L'));

// Summarize: total uncovered lines in files with .cs extension that are NOT huge pipeline files
const smallerFiles = ['ManagedNaming.cs', 'NativeAotEmitter.cs', 'TypeProviders.cs', 'CodeGenStage.cs',
  'LoaderStage.cs', 'LoaderStage.AssemblyLoading.cs', 'MetadataTypeResolver.cs',
  'ManagedClosureModels.cs', 'ManagedClosureArtifactModels.cs', 'ManagedMethodIdentityContracts.cs',
  'ManagedSemanticWorldContracts.cs', 'TypedIlAndAotCoreIrContracts.cs',
  'ObjectModelUtilities.cs', 'StringIdEmission.cs'];

console.log('\n=== Small file uncovered totals ===');
let total = 0;
smallerFiles.forEach(sf => {
  const ms = methods.filter(m => m.file.includes(sf));
  const hit = ms.reduce((s, m) => s + m.hitLines, 0);
  const tot = ms.reduce((s, m) => s + m.totalLines, 0);
  const pct = tot > 0 ? (hit/tot*100).toFixed(1) : 'N/A';
  console.log('  ' + sf + ' | ' + hit + '/' + tot + ' = ' + pct + '%');
  total += (tot - hit);
});
console.log('Total uncovered in small files: ' + total);
