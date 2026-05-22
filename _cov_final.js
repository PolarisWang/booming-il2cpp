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

// Find simple-named uncovered methods with 3-30 lines (easy targets)
const small = methods.filter(m => m.hitLines === 0 && m.totalLines >= 3 && m.totalLines <= 40
  && !m.name.startsWith('<') && !m.name.startsWith('get_') && !m.name.startsWith('set_'));
small.sort((a, b) => b.totalLines - a.totalLines);

console.log('Still-uncovered simple methods (3-40 lines):');
small.slice(0, 30).forEach(m => {
  const shortFile = m.file.length > 50 ? '...' + m.file.slice(-47) : m.file;
  console.log(shortFile + '::' + m.name + ' | ' + m.totalLines + 'L');
});

const sumLines = small.reduce((s, m) => s + m.totalLines, 0);
console.log('\nTotal small uncovered methods: ' + small.length + ', total lines: ' + sumLines);
console.log('Need only 62 more lines to reach 60%');
