const fs = require('fs');
const xml = fs.readFileSync('tests/managed/Chaos.IL2CPP.CodeGen.Tests/CoverageResults/fresh_cobertura.xml', 'utf8');
const excluded = ['\\obj\\', '\\ReferenceProof\\', 'PatchData', 'NativeReferenceProofEmitter', 'RuntimeSkeleton', 'PlanBuilders', 'GeneratedPluginExports'];
let currentFile = '', fileStats = {};
const lines = xml.split('\n');
const extraFiles = ['ManagedNaming.cs', 'ManagedClosureArtifactModels.cs', 'ManagedClosureModels.cs', 'ManagedMethodIdentityContracts.cs', 'ManagedSemanticWorldContracts.cs', 'TypedIlAndAotCoreIrContracts.cs', 'LoaderStage.cs', 'LoaderStage.AssemblyLoading.cs', 'MetadataTypeResolver.cs', 'TypeProviders.cs', 'PipelinePlan.cs'];
for (let i = 0; i < lines.length; i++) {
  const l = lines[i];
  const cf = l.match(/filename="([^"]+)"/);
  if (cf) { currentFile = cf[1]; continue; }
  const lm = l.match(/line number="(\d+)" hits="(\d+)"/);
  if (lm && currentFile) {
    const isExcluded = excluded.some(p => currentFile.includes(p));
    if (!isExcluded) {
      const fname = currentFile.split('\\').pop();
      if (extraFiles.includes(fname)) {
        if (!fileStats[fname]) fileStats[fname] = {h:0, t:0};
        fileStats[fname].t++;
        if (parseInt(lm[2]) > 0) fileStats[fname].h++;
      }
    }
  }
  if (l.includes('</class>')) currentFile = '';
}
let totH = 0, totT = 0;
Object.entries(fileStats).forEach(([n,s]) => { console.log(n + ': ' + s.h + '/' + s.t + ' = ' + (s.h/s.t*100).toFixed(1) + '%'); totH += s.h; totT += s.t; });
console.log('Combined: ' + totH + '/' + totT + ' = ' + (totH/totT*100).toFixed(1) + '%');
