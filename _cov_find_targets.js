const fs = require('fs');
const xml = fs.readFileSync('tests/managed/Chaos.IL2CPP.CodeGen.Tests/CoverageResults/cobertura.xml', 'utf8');
const lines = xml.split('\n');
const excluded = ['\\obj\\', '\\ReferenceProof\\', 'PatchData', 'NativeReferenceProofEmitter', 'RuntimeSkeleton', 'PlanBuilders', 'GeneratedPluginExports'];

// Collect files with 50-500 uncovered lines (smaller, more testable files)
let currentFile = null;
const fileLines = {}; // filename -> {hits, total, uncoveredLines: [{number, hits}]}
let classLines = [];

for (let i = 0; i < lines.length; i++) {
  const l = lines[i];
  const cf = l.match(/filename="([^"]+)"/);
  if (cf) {
    currentFile = cf[1];
    classLines = [];
    continue;
  }

  if (l.includes('</class>')) {
    if (currentFile && !excluded.some(p => currentFile.includes(p))) {
      if (!fileLines[currentFile]) {
        fileLines[currentFile] = {hits: 0, total: 0, uncoveredLines: []};
      }
      classLines.forEach(cl => {
        fileLines[currentFile].total++;
        if (cl.hits > 0) fileLines[currentFile].hits++;
        else fileLines[currentFile].uncoveredLines.push(cl);
      });
    }
    currentFile = null;
    classLines = [];
    continue;
  }

  const lm = l.match(/line number="(\d+)" hits="(\d+)"/);
  if (lm && currentFile) {
    classLines.push({number: parseInt(lm[1]), hits: parseInt(lm[2])});
  }
}

// Focus on smaller files: 50-500 total lines, at least 10 uncovered
// Also look at higher coverage files (above 70%) that are just a few lines from full coverage
const candidates = Object.entries(fileLines)
  .filter(([, s]) => s.total >= 30 && s.total <= 800 && s.uncoveredLines.length >= 5)
  .sort((a, b) => {
    // Prefer higher coverage (easier wins) and smaller files
    const aRatio = a[1].hits / a[1].total;
    const bRatio = b[1].hits / b[1].total;
    return bRatio - aRatio;
  });

console.log('Best candidates for quick coverage wins (high %, small gap):');
candidates.slice(0, 25).forEach(([f, s]) => {
  const short = f.length > 90 ? '...' + f.slice(-87) : f;
  const pct = (s.hits / s.total * 100).toFixed(1);
  console.log(short + ' | ' + s.hits + '/' + s.total + ' | gap=' + s.uncoveredLines.length + ' | ' + pct + '%');
});

// Show the specific uncovered lines for the top 5 candidates
console.log('\n\n--- Uncovered lines detail for top 5 ---');
candidates.slice(0, 5).forEach(([f, s]) => {
  console.log('\n' + f + ' (' + s.uncoveredLines.length + ' uncovered lines):');
  const groups = [];
  let currentGroup = [];
  let prevLine = -10;
  s.uncoveredLines.sort((a, b) => a.number - b.number).forEach(l => {
    if (l.number - prevLine > 3) {
      if (currentGroup.length > 0) groups.push(currentGroup);
      currentGroup = [];
    }
    currentGroup.push(l.number);
    prevLine = l.number;
  });
  if (currentGroup.length > 0) groups.push(currentGroup);
  groups.forEach(g => {
    if (g.length === 1) console.log('  Line ' + g[0]);
    else console.log('  Lines ' + g[0] + '-' + g[g.length-1] + ' (' + g.length + ' lines)');
  });
});
