const fs = require('fs');

function getFileLines(xmlPath) {
  const xml = fs.readFileSync(xmlPath, 'utf8');
  const excluded = ['\\obj\\', '\\ReferenceProof\\', 'PatchData', 'NativeReferenceProofEmitter', 'RuntimeSkeleton', 'PlanBuilders', 'GeneratedPluginExports'];
  let currentFile = '', fileLines = {};
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
        if (!fileLines[fname]) fileLines[fname] = {h:0, t:0};
        fileLines[fname].t++;
        if (parseInt(lm[2]) > 0) fileLines[fname].h++;
      }
    }
    if (l.includes('</class>')) currentFile = '';
  }
  return fileLines;
}

const fresh = getFileLines('tests/managed/Chaos.IL2CPP.CodeGen.Tests/CoverageResults/fresh_cobertura.xml');
const current = getFileLines('tests/managed/Chaos.IL2CPP.CodeGen.Tests/CoverageResults/coverage.cobertura.xml');

const allFiles = new Set([...Object.keys(fresh), ...Object.keys(current)]);
console.log('Files in fresh but NOT in current:');
allFiles.forEach(f => { if (fresh[f] && !current[f]) console.log('  ' + f + ' (' + fresh[f].t + ' lines)'); });
console.log('\nFiles in current but NOT in fresh:');
allFiles.forEach(f => { if (!fresh[f] && current[f]) console.log('  ' + f + ' (' + current[f].t + ' lines)'); });
console.log('\nFiles with >100 line count difference:');
allFiles.forEach(f => {
  if (fresh[f] && current[f]) {
    const diff = Math.abs(fresh[f].t - current[f].t);
    if (diff > 100) console.log('  ' + f + ': fresh=' + fresh[f].t + ' current=' + current[f].t + ' diff=' + (fresh[f].t - current[f].t));
  }
});
