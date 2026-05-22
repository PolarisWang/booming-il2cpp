const fs = require('fs');
const xml = fs.readFileSync('tests/managed/Chaos.IL2CPP.CodeGen.Tests/CoverageResults/cobertura.xml', 'utf8');
const excluded = ['\\obj\\', '\\ReferenceProof\\', 'PatchData', 'NativeReferenceProofEmitter', 'RuntimeSkeleton', 'PlanBuilders', 'GeneratedPluginExports'];

// Parse methods: <method name="..." sighash="..."> <lines><line number="..." hits="0"/>...
let inMethod = false;
let currentMethod = null;
let currentFile = null;
const methods = [];

xml.split('\n').forEach(l => {
  const cf = l.match(/filename="([^"]+)"/);
  if (cf) { currentFile = cf[1]; return; }

  const mm = l.match(/name="([^"]+)".*signature="([^"]*)"/);
  if (mm && currentFile && !excluded.some(p => currentFile.includes(p))) {
    inMethod = true;
    currentMethod = {file: currentFile, name: mm[1], sig: mm[2], lines: [], hitLines: 0, totalLines: 0};
    return;
  }

  if (inMethod && l.includes('</method>')) {
    if (currentMethod) methods.push(currentMethod);
    inMethod = false;
    currentMethod = null;
    return;
  }

  const lm = l.match(/line number="(\d+)" hits="(\d+)"/);
  if (lm && inMethod && currentMethod) {
    currentMethod.totalLines++;
    if (parseInt(lm[2]) > 0) currentMethod.hitLines++;
  }
});

const uncovered = methods.filter(m => m.hitLines === 0 && m.totalLines >= 3);
uncovered.sort((a, b) => b.totalLines - a.totalLines);
console.log('Top uncovered methods (0% coverage, >=3 lines):');
console.log('File::Method | Lines | Sig');
uncovered.slice(0, 40).forEach(m => {
  const shortFile = m.file.length > 70 ? '...' + m.file.slice(-67) : m.file;
  const shortSig = m.sig.length > 40 ? m.sig.slice(0, 37) + '...' : m.sig;
  console.log(shortFile + '::' + m.name + ' | ' + m.totalLines + 'L | ' + shortSig);
});

// Sum all totally uncovered method lines
const totalUncoveredMethodLines = uncovered.reduce((s, m) => s + m.totalLines, 0);
console.log('\nTotal uncovered method lines: ' + totalUncoveredMethodLines + ' in ' + uncovered.length + ' methods');

// Also check: methods that are partially covered (1-50%) with at least 10 lines
const partial = methods.filter(m => m.hitLines > 0 && m.hitLines < m.totalLines && m.totalLines >= 10);
partial.sort((a, b) => (a.hitLines/a.totalLines) - (b.hitLines/b.totalLines));
console.log('\nTop partially covered methods (lowest % first, >=10L):');
partial.slice(0, 20).forEach(m => {
  const shortFile = m.file.length > 70 ? '...' + m.file.slice(-67) : m.file;
  const pct = (m.hitLines / m.totalLines * 100).toFixed(0);
  console.log(shortFile + '::' + m.name + ' | ' + m.hitLines + '/' + m.totalLines + ' = ' + pct + '%');
});
