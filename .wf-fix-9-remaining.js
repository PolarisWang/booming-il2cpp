export const meta = {
  name: "fix-9-remaining-real-defects",
  description: "Parallel investigation and fix of 9 remaining AOT realDefects in system chunk",
  phases: [
    { title: "Investigate", detail: "3 parallel clusters — Math, Enum/Array, Activator/Type/UInt64" },
    { title: "Fix", detail: "Apply per-cluster fixes" },
    { title: "Verify", detail: "Build and run system chunk, check realDefect count" },
  ],
};

const W = "D:/agent/chaos-il2cpp";

phase("Investigate");
log("Investigating 9 realDefects across 3 clusters in parallel...");

const investigations = await parallel([
  // Cluster B — Math.Cos / Math.Pow (0xc0000005, si=1871/1877)
  () => agent([
    "Investigate Math::Cos and Math::Pow AOT failures (STATUS_ACCESS_VIOLATION).",
    "LOCATION: " + W,
    "",
    "Both have SimpleForward shapes:",
    "  System.Math::Cos(Double) -> ChaosMathCos (math_stubs.cpp:std::cos)",
    "  System.Math::Pow(Double,Double) -> ChaosMathPow (math_stubs.cpp:std::pow)",
    "",
    "GENERATED C# TEST:",
    "  var result = Math.Cos(default(double));",
    "  Assert.AreEqual(1d, result);          // Cos(0)=1",
    "  return BitConverter.DoubleToInt64Bits(result);",
    "",
    "GENERATED AOT C++:",
    "  const auto chaos_result = ChaosMathCos(ChaosLoadFloat64(chaos_arg_0));",
    "",
    "TASK:",
    "1. Read " + W + "/src/native/runtime-core/runtime_stubs/math_stubs.cpp for ChaosMathCos/Pow implementations",
    "2. Read " + W + "/artifacts/foundation-dll/System.Private.CoreLib/chunks/system/native/subjects/native-aot.generated.cpp around 'MathTests_Cos_8_double_0' for the full generated body",
    "3. The SEH says 0xc0000005 (access violation). ChaosLoadFloat64 and ChaosMathCos take double arguments. Read " + W + "/src/native/runtime-core/core/value_type_kernels.cpp for ChaosLoadFloat64 definition",
    "4. Check if the problem is in the ABI argument passing: the double value is passed via CHAOS_IL2CPP_INTPTR (pointer-size register) but loaded as double. Could be the CHAOS_IL2CPP_INTPTR slot holding a POINTER to the double rather than the double value itself",
    "5. Read " + W + "/artifacts/foundation-dll/System.Private.CoreLib/chunks/system/managed/combined/CombinedSubjects.cs for Cos_8_double_0 and Cos_8_double_1 bodies",
    "",
    "Report exact root cause (file + line) and fix steps.",
  ].join("\n"), { label: "investigate-math", phase: "Investigate" }),

  // Cluster A — Array.CreateInstance (0xe0000001) + Enum.Format (0xe0000001)
  () => agent([
    "Investigate Array::CreateInstance and Enum::Format managed-exception failures.",
    "LOCATION: " + W,
    "",
    "FACTS:",
    "  si=151 Array::CreateInstance(Type,Int32) -> 0xe0000001 (C++ EH exception)",
    "  si=1371 Enum::Format(Type,Object,String) -> 0xe0000001 (C++ EH exception)",
    "  Both have codegen shapes registered.",
    "",
    "Generated test for CreateInstance(Type,Int32):",
    "  var result = Array.CreateInstance(typeof(int), default(int));",
    "  return (object)(result) != null ? 1L : 0L;",
    "",
    "TASK:",
    "1. Read " + W + "/artifacts/foundation-dll/System.Private.CoreLib/chunks/system/managed/combined/CombinedSubjects.cs for 'CreateInstance_3_System_Type_int_0'",
    "2. Read " + W + "/src/native/runtime-core/runtime_stubs/array_stubs.cpp for ChaosArrayCreateInstance — does it handle null type or negative size?",
    "3. Read " + W + "/src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.CoreStubs.Part2.S7.cs for Array.CreateInstance registrations",
    "4. For Enum::Format: read " + W + "/src/native/runtime-core/enum_stubs.cpp or runtime_stubs/enum_stubs.cpp for ChaosEnumFormat or equivalent",
    "5. Check " + W + "/src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/ for Enum.Format shape registration",
    "",
    "Report: which native functions are called, what arguments they receive, and what goes wrong.",
  ].join("\n"), { label: "investigate-clusterA", phase: "Investigate" }),

  // Cluster C — Activator.CreateInstance + UInt64.Parse + Type.GetField
  () => agent([
    "Investigate Activator::CreateInstance, UInt64::Parse, and Type::GetField failures.",
    "LOCATION: " + W,
    "",
    "FACTS:",
    "  si=7   Activator::CreateInstance(Type) — has ChaosReflectionCreateInstance native",
    "  si=1720 UInt64::Parse(String,NumberStyles) — 2-arg overload, same pattern as the fixed Int32 Parse bug",
    "  si=2899 Type::GetField(String) — has ChaosTypeGetFieldBindingFlags native",
    "",
    "GENERATED TESTS:",
    "  // Activator.CreateInstance(typeof(int)) — ATG injects typeof(int) via ValueGenerator",
    "  var result = Activator.CreateInstance(typeof(int))",
    "  return (object)(result) != null ? 1L : 0L;",
    "",
    "  // UInt64.Parse(\"1234567890123456789\", NumberStyles)",
    "  var result = UInt64.Parse(\"1234567890123456789\", default(NumberStyles));",
    "  Assert.AreEqual(1234567890123456789, result);",
    "  return (long)(result);",
    "",
    "  // Type.GetField(\"SampleField\")",
    "  var result = typeof(ReflectionSubjectSample).GetField(\"SampleField\");",
    "  return (object)(result) != null ? 1L : 0L;",
    "",
    "TASK:",
    "1. For Activator: read " + W + "/src/native/runtime-core/reflection/invoke.cpp for ChaosReflectionCreateInstance — does it handle typeof(int) (a value type with no .ctor)?",
    "2. Check " + W + "/artifacts/foundation-dll/System.Private.CoreLib/chunks/system/native/subjects/native-aot.generated.cpp for 'ActivatorTests_CreateInstance_3_System_Type_0' to see emitted arg",
    "3. For UInt64: check " + W + "/src/native/runtime-core/parse_convert.cpp for ChaosParseUInt64Styles — does it exist? (should have been added in fix",
    "4. For Type.GetField: read " + W + "/src/native/runtime-core/reflection/type_stubs.cpp for ChaosTypeGetFieldBindingFlags — the test feeds a real member name ('SampleField'), not null",
    "5. Check " + W + "/src/native/runtime-core/runtime_stubs/convert_stubs.h for ChaosParseUInt64Styles declaration",
    "",
    "Report: which are already fixed (need pipeline rebuild) vs still broken.",
  ].join("\n"), { label: "investigate-clusterC", phase: "Investigate" }),
]);

const ok = investigations.filter(Boolean);
log(ok.length + "/3 investigations complete.");

phase("Fix");

const fixPlan = await agent([
  "You have 3 investigation reports for 9 AOT realDefects in the CoreLib system chunk.",
  "",
  "=== Cluster B (Math) ===",
  ok[0] || "(no result)",
  "",
  "=== Cluster A (Array/Enum) ===",
  ok[1] || "(no result)",
  "",
  "=== Cluster C (Activator/UInt64/Type) ===",
  ok[2] || "(no result)",
  "",
  "Synthesize ONE unified fix plan. For each cluster list:",
  "- Root cause (one sentence)",
  "- Exact file changes (file path + line number + what to change)",
  "- Whether a pipeline rebuild is required or just a source change",
  "- Estimated complexity (S/M/L)",
  "",
  "If a defect is already fixed (just needs rebuilding), say so explicitly.",
  "",
  "Be concrete. Working tree: " + W,
].join("\n"), { label: "synthesize-plan", phase: "Fix" });

// Phase 3: Verify — apply fixes and run pipeline
phase("Verify");

log("Fix plan: " + (fixPlan ? fixPlan.slice(0, 1000) : "(empty)"));

// Apply changes and rebuild
const buildResult = await agent([
  "Apply the proposed fixes to the 9 remaining realDefects.",
  "",
  "LOCATION: " + W,
  "",
  "For each file change:",
  "1. Read the current file content",
  "2. Apply the fix",
  "3. Verify it compiles",
  "",
  "Work from the synthesis plan. After all fixes are applied:",
  "- dotnet build " + W + "/src/managed/Chaos.IL2CPP.Generator/Chaos.IL2CPP.Generator.csproj",
  "- python " + W + "/tests/e2e/translation/artifacts/build_presets.py --preset windows-x64-reference --force",
  "- Copy lib + rebuild entry.exe",
  "",
  "Report what was changed and whether each change compiled/passed.",
].join("\n"), { label: "apply-fixes", phase: "Verify" });

const pipelineResult = await agent([
  "Run the system chunk pipeline and check realDefect count.",
  "",
  "LOCATION: " + W,
  "",
  "cd " + W + "/tests/e2e",
  "CHAOS_FOUNDATION_DLL=D:/agent/chaos-il2cpp/tests/e2e/translation",
  "python -m verification.chunk_pipeline --chunk system --stages build,fact --smoke",
  "",
  "After completion run:",
  "python -c \"",
  "import json",
  "from collections import Counter",
  "d = json.load(open('artifacts/foundation-dll/System.Private.CoreLib/chunks/system/results/fact-results.json'))",
  "c = Counter(x.get('resultKind') for x in d.get('aot', []))",
  "print('resultKinds:', dict(c))",
  "for x in d.get('aot', []):",
  "    if x.get('resultKind') == 'realDefect':",
  "        print('  realDefect:', x.get('methodSubjectId'))",
  "\"",
  "",
  "Report the final realDefect count and any remaining failures.",
].join("\n"), { label: "verify-pipeline", phase: "Verify" });

return { investigations: ok.length, fixPlan, buildResult, pipelineResult };