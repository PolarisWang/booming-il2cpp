const fs = require('fs');
const xml = fs.readFileSync('tests/managed/Chaos.IL2CPP.CodeGen.Tests/CoverageResults/cobertura.xml', 'utf8');
const m = xml.match(/lines-covered="(\d+)"\s+lines-valid="(\d+)"/);
console.log('Raw: ' + m[1] + '/' + m[2] + ' = ' + (parseInt(m[1])/parseInt(m[2])*100).toFixed(2) + '%');

const lines = xml.split('\n');
const excluded = ['\\obj\\', '\\ReferenceProof\\', 'PatchData', 'NativeReferenceProofEmitter', 'RuntimeSkeleton', 'PlanBuilders', 'GeneratedPluginExports'];
let inHits = 0, inTotal = 0, currentFile = '';
let fileStats = {};

for (let i = 0; i < lines.length; i++) {
  const l = lines[i];
  const cf = l.match(/filename="([^"]+)"/);
  if (cf) { currentFile = cf[1]; continue; }
  const lm = l.match(/line number="(\d+)" hits="(\d+)"/);
  if (lm && currentFile) {
    const isExcluded = excluded.some(p => currentFile.includes(p));
    if (!isExcluded) {
      if (!fileStats[currentFile]) fileStats[currentFile] = {h:0, t:0};
      fileStats[currentFile].t++;
      inTotal++;
      if (parseInt(lm[2]) > 0) { fileStats[currentFile].h++; inHits++; }
    }
  }
  if (l.includes('</class>')) currentFile = '';
}
console.log('Filtered: ' + inHits + '/' + inTotal + ' = ' + (inHits/inTotal*100).toFixed(2) + '%');
console.log('Need: ' + Math.ceil(inTotal * 0.6 - inHits) + ' more lines');

// Per-file detail for specific files
const targets = ['RuntimeHelperShapeRegistry.cs', 'NativeAotLoweringPlanner.cs', 'AotCoreIrLowering.cs', 'TypeProviders.cs'];
targets.forEach(t => {
  const entry = Object.entries(fileStats).find(([name]) => name.includes(t));
  if (entry) console.log(entry[0].split('\\').pop() + ': ' + entry[1].h + '/' + entry[1].t + ' = ' + (entry[1].h/entry[1].t*100).toFixed(2) + '%');
});
