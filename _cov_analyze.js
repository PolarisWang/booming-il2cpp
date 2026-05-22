const fs = require('fs');
const xml = fs.readFileSync('tests/managed/Chaos.IL2CPP.CodeGen.Tests/CoverageResults/cobertura.xml', 'utf8');
const lines = xml.split('\n');
const excluded = ['\\obj\\', '\\ReferenceProof\\', 'PatchData', 'NativeReferenceProofEmitter', 'RuntimeSkeleton', 'PlanBuilders', 'GeneratedPluginExports'];
let currentFile = null, hits = 0, total = 0;
const fileStats = {};

for (let i = 0; i < lines.length; i++) {
  const l = lines[i];
  const cf = l.match(/filename="([^"]+)"/);
  if (cf) { currentFile = cf[1]; continue; }
  const lm = l.match(/line number="(\d+)" hits="(\d+)"/);
  if (lm && currentFile) {
    const isExcluded = excluded.some(p => currentFile.includes(p));
    if (!isExcluded) {
      if (!fileStats[currentFile]) fileStats[currentFile] = {hits:0, total:0};
      fileStats[currentFile].total++;
      total++;
      if (parseInt(lm[2]) > 0) { fileStats[currentFile].hits++; hits++; }
    }
  }
  if (l.includes('</class>')) currentFile = '';
}

const entries = Object.entries(fileStats).map(([f,s]) => [f, s.hits, s.total, s.total-s.hits, (s.hits/s.total*100).toFixed(1)]);
entries.sort((a,b) => b[3] - a[3]);
console.log('Top 25 files by uncovered lines:');
console.log('File | Hits/Total | Uncovered | %');
entries.slice(0,25).forEach(([f,h,t,u,p]) => {
  const short = f.length > 100 ? '...' + f.slice(-97) : f;
  console.log(short + ' | ' + h + '/' + t + ' | ' + u + ' | ' + p + '%');
});
console.log('---');
// Sum of uncovered for top 20
const top20Uncovered = entries.slice(0, 20).reduce((s, [, , , u]) => s + u, 0);
console.log('Top 20 files total uncovered: ' + top20Uncovered);
console.log('We need: 242 more lines covered');
console.log('---');
// Show files sorted by what's easiest to improve: coverage between 10-80% with high total lines
console.log('Top files by line count (easy improvement potential):');
entries.filter(([,,t,u]) => t > 50 && u > 10).sort((a,b) => b[2] - a[2]).slice(0, 15).forEach(([f,h,t,u,p]) => {
  const short = f.length > 100 ? '...' + f.slice(-97) : f;
  console.log(short + ' | ' + h + '/' + t + ' | gap=' + u + ' | ' + p + '%');
});
