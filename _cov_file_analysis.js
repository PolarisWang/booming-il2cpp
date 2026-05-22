const fs = require('fs');
const xml = fs.readFileSync('tests/managed/Chaos.IL2CPP.CodeGen.Tests/CoverageResults/coverage.cobertura.xml', 'utf8');
const excluded = ['\\obj\\', '\\ReferenceProof\\', 'PatchData', 'NativeReferenceProofEmitter', 'RuntimeSkeleton', 'PlanBuilders', 'GeneratedPluginExports'];
let currentFile = '', fileStats = {};
const lines = xml.split('\n');
for (let i = 0; i < lines.length; i++) {
  const l = lines[i];
  const cf = l.match(/filename="([^"]+)"/);
  if (cf) { currentFile = cf[1]; continue; }
  const lm = l.match(/line number="(\d+)" hits="(\d+)"/);
  if (lm && currentFile) {
    const isExcluded = excluded.some(p => currentFile.includes(p));
    if (!isExcluded) {
      const fname = currentFile.split('\\').pop();
      if (!fileStats[fname]) fileStats[fname] = {h:0, t:0};
      fileStats[fname].t++;
      if (parseInt(lm[2]) > 0) fileStats[fname].h++;
    }
  }
  if (l.includes('</class>')) currentFile = '';
}
const arr = Object.entries(fileStats).map(([n,s]) => [n, s.t - s.h, (s.h/s.t*100).toFixed(1), s.t]);
arr.sort((a,b) => b[1] - a[1]);
console.log('Files with >100 uncovered lines:');
arr.filter(f => f[1] > 100).slice(0, 30).forEach(f => console.log(f[0] + ': ' + f[1] + ' uncovered / ' + f[3] + ' total = ' + f[2] + '%'));
