const fs = require('fs');
const xml = fs.readFileSync('tests/managed/Chaos.IL2CPP.CodeGen.Tests/CoverageResults/cobertura.xml', 'utf8');
const excluded = ['\\obj\\', '\\ReferenceProof\\', 'PatchData', 'NativeReferenceProofEmitter', 'RuntimeSkeleton', 'PlanBuilders', 'GeneratedPluginExports'];

// Target methods and their files:
const targets = [
  {file: 'NativeAotLoweringPlanner.cs', name: 'SanitizeCppIdentifier'},
  {file: 'NativeAotLoweringPlanner.cs', name: 'ExtractDelegateArityFromSubjectId'},
  {file: 'NativeAotLoweringPlanner.cs', name: 'ExtractSubjectIndex'},
  {file: 'NativeAotLoweringPlanner.cs', name: 'TryCreateMetadataEntityHandle'},
  {file: 'AotCoreIrLowering.cs', name: 'ClassifyValueTypeFields'},
  {file: 'AotCoreIrLowering.cs', name: 'BuildStructMarshallingDescriptor'},
  {file: 'AotCoreIrLowering.cs', name: 'CreateFieldReference'},
  {file: 'RuntimeHelperShapeRegistry.cs', name: 'TryParseGenericTypeArgsFromTypeName'},
  {file: 'RuntimeHelperShapeRegistry.cs', name: 'ExtractDelegateArity'},
  {file: 'LoaderStage.MetadataResolution.cs', name: 'TryDescribeMethodImport'},
  {file: 'LoaderStage.AssemblyLoading.cs', name: 'TryCreateAssemblyName'},
  {file: 'LoaderStage.GenericDemandGraph.cs', name: 'TrackTypeInstantiationDemand'},
];

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
    inMethod = false;
    currentMethod = null;
    return;
  }

  if (inMethod && currentMethod) {
    const lm = l.match(/line number="(\d+)" hits="(\d+)"/);
    if (lm) {
      currentMethod.totalLines++;
      if (parseInt(lm[2]) > 0) currentMethod.hitLines++;
    }
  }
});

console.log('Target method coverage:');
targets.forEach(t => {
  const matches = methods.filter(m => m.file.includes(t.file) && m.name === t.name);
  if (matches.length === 0) {
    // Try partial name match
    const partial = methods.filter(m => m.file.includes(t.file) && m.name.includes(t.name));
    if (partial.length > 0) {
      partial.forEach(m => {
        const pct = m.hitLines > 0 ? (m.hitLines/m.totalLines*100).toFixed(0) : '0';
        const short = m.file.length > 60 ? '...' + m.file.slice(-57) : m.file;
        console.log(short + '::' + m.name + ' | ' + m.hitLines + '/' + m.totalLines + ' = ' + pct + '%');
      });
    } else {
      console.log(t.file + '::' + t.name + ' - NOT FOUND');
    }
  } else {
    matches.forEach(m => {
      const pct = m.hitLines > 0 ? (m.hitLines/m.totalLines*100).toFixed(0) : '0';
      const short = m.file.length > 60 ? '...' + m.file.slice(-57) : m.file;
      console.log(short + '::' + m.name + ' | ' + m.hitLines + '/' + m.totalLines + ' = ' + pct + '%');
    });
  }
});

// Also find all uncovered methods with total lines between 5 and 50 (small testable methods)
console.log('\n--- Small testable uncovered methods (5-50 lines, 0% coverage, simple names) ---');
const simpleUncovered = methods.filter(m => m.hitLines === 0 && m.totalLines >= 5 && m.totalLines <= 50
  && !m.name.startsWith('<') && !m.name.startsWith('get_') && !m.name.startsWith('set_'));
simpleUncovered.sort((a, b) => b.totalLines - a.totalLines);
simpleUncovered.slice(0, 30).forEach(m => {
  const shortFile = m.file.length > 55 ? '...' + m.file.slice(-52) : m.file;
  console.log(shortFile + '::' + m.name + ' | ' + m.totalLines + 'L');
});
console.log('\nTotal small uncovered methods: ' + simpleUncovered.length);
const totalLines = simpleUncovered.reduce((s, m) => s + m.totalLines, 0);
console.log('Total uncovered lines: ' + totalLines);
