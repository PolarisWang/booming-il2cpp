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

// Easy testable targets: Generator static non-public methods, small-to-medium (5-40 lines)
// That look like utility functions (not emitter methods)
const easy = methods.filter(m =>
  m.file.includes('Chaos.IL2CPP.Generator') &&
  m.hitLines === 0 &&
  m.totalLines >= 5 && m.totalLines <= 40 &&
  !m.name.startsWith('<') &&
  !m.name.startsWith('get_') &&
  !m.name.startsWith('set_') &&
  !m.name.startsWith('.cctor') &&
  !m.name.startsWith('.ctor') &&
  // Skip methods that require complex pipeline state
  !m.name.includes('Emit') &&
  !m.name.includes('Build') &&
  !m.name.includes('Plan') &&
  !m.name.includes('Collect') &&
  !m.name.includes('Create') &&
  !m.name.includes('Register') &&
  !m.name.includes('Process') &&
  !m.name.includes('Generate') &&
  !m.name.includes('Materialize') &&
  !m.name.includes('Initialize') &&
  !m.name.includes('Resolve') &&
  !m.name.includes('Encode') &&
  !m.name.includes('Allocate') &&
  !m.name.includes('Setup') &&
  !m.name.includes('Enrich') &&
  !m.name.includes('Layout') &&
  !m.name.includes('Dispatch') &&
  !m.name.includes('Invoke') &&
  !m.name.includes('Format') &&
  !m.name.includes('Convert') &&
  !m.name.includes('Enter'));

console.log('Easy testable uncovered Generator methods (utility-style, 5-40L):');
let totalLines = 0;
easy.forEach(m => {
  const shortFile = m.file.split('\\').pop();
  console.log(shortFile + '::' + m.name + ' (' + m.totalLines + 'L) [' + m.sig + ']');
  totalLines += m.totalLines;
});
console.log('\nTotal: ' + easy.length + ' methods, ' + totalLines + ' lines');

// Also show the partially covered methods with >0 hit lines but < total lines
const partial = methods.filter(m =>
  m.file.includes('Chaos.IL2CPP.Generator') &&
  m.hitLines > 0 && m.hitLines < m.totalLines &&
  m.totalLines >= 5 && m.totalLines <= 40 &&
  !m.name.startsWith('<') &&
  !m.name.startsWith('get_') &&
  !m.name.startsWith('set_'));
partial.sort((a, b) => (b.totalLines - b.hitLines) - (a.totalLines - a.hitLines));
console.log('\nPartially covered (most uncovered lines first):');
partial.slice(0, 30).forEach(m => {
  const shortFile = m.file.split('\\').pop();
  const uncovered = m.totalLines - m.hitLines;
  console.log(shortFile + '::' + m.name + ' (' + m.hitLines + '/' + m.totalLines + ' = ' + uncovered + ' uncovered)');
});
