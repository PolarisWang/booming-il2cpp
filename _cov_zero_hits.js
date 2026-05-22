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

// Find ALL uncovered methods in Generator project, sorted by total lines descending
const genMethods = methods.filter(m =>
  m.file.includes('Chaos.IL2CPP.Generator') &&
  m.hitLines === 0 &&
  m.totalLines >= 2 &&
  !m.name.startsWith('<') &&
  !m.name.startsWith('get_') &&
  !m.name.startsWith('set_') &&
  !m.name.startsWith('.cctor'));

genMethods.sort((a, b) => b.totalLines - a.totalLines);
console.log('Generator uncovered methods (excluding 1-liners, auto-gen):');
let total = 0;
genMethods.slice(0, 50).forEach(m => {
  const shortFile = m.file.split('\\').pop();
  console.log(shortFile + '::' + m.name + ' (' + m.totalLines + 'L)');
  total += m.totalLines;
});
console.log('\nTotal uncovered lines in top 50: ' + total);
console.log('Total uncovered methods: ' + genMethods.length);
console.log('Total uncovered lines: ' + genMethods.reduce((s, m) => s + m.totalLines, 0));
