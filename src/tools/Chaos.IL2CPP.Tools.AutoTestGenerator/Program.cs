using System.Text.Json;
using Chaos.IL2CPP.Tools.AutoTestGenerator;

if (args.Length < 2)
{
    Console.Error.WriteLine("Usage: --dll <path> [--type <FullTypeName> | --all-types] [--output <dir>]");
    Console.Error.WriteLine("  --dll        Path to the input assembly DLL");
    Console.Error.WriteLine("  --type       Full type name to scan (e.g. System.Convert)");
    Console.Error.WriteLine("  --all-types  Scan all public types in the assembly");
    Console.Error.WriteLine("  --output     Output directory (default: ./output/{AssemblyName})");
    Console.Error.WriteLine("  --list-types List public types in the assembly");
    Console.Error.WriteLine("  --report     Aggregate coverage from output directory into SUMMARY.md");
    Console.Error.WriteLine("  --emit-metadata <path>  Emit subjects.metadata.json for all-types mode");
    Console.Error.WriteLine("  --chunk-slug <slug>     Chunk slug for metadata (default: assembly name)");
    Console.Error.WriteLine("  --namespace-filter <ns1,ns2,...>  Filter by namespace prefix (for chunk builds)");
    return args is ["--report", ..] ? 0 : 1;
}

// ── Parse CLI arguments ──
string dllPath = "";
string typeFullName = "";
string? outputDir = null;
string? emitMetadataPath = null;
string? chunkSlug = null;
string? namespaceFilter = null;
bool listTypes = false;
bool allTypes = false;
bool skipProbe = false;

for (int i = 0; i < args.Length; i++)
{
    switch (args[i])
    {
        case "--dll" when i + 1 < args.Length:
            dllPath = args[++i];
            break;
        case "--type" when i + 1 < args.Length:
            typeFullName = args[++i];
            break;
        case "--output" when i + 1 < args.Length:
            outputDir = args[++i];
            break;
        case "--emit-metadata" when i + 1 < args.Length:
            emitMetadataPath = args[++i];
            break;
        case "--chunk-slug" when i + 1 < args.Length:
            chunkSlug = args[++i];
            break;
        case "--namespace-filter" when i + 1 < args.Length:
            namespaceFilter = args[++i];
            break;
        case "--skip-probe":
            skipProbe = true;
            break;
        case "--list-types":
            listTypes = true;
            break;
        case "--all-types":
            allTypes = true;
            break;
    }
}

// ── Coverage report mode ──
var reportIdx = Array.IndexOf(args, "--report");
if (reportIdx >= 0 && reportIdx + 1 < args.Length)
{
    var reportDir = args[reportIdx + 1];
    var aggregator = new CoverageAggregator();
    var items = aggregator.Aggregate(reportDir);
    var summary = aggregator.GenerateReport(items);
    var summaryPath = Path.Combine(reportDir, "SUMMARY.md");
    File.WriteAllText(summaryPath, summary);
    Console.WriteLine(summary);
    Console.WriteLine($"[Output] {summaryPath}");
    return 0;
}

// ── Known type-to-DLL mapping for types not in System.Runtime.dll ──
var knownTypeDlls = new Dictionary<string, string>(StringComparer.Ordinal)
{
    ["System.Net.Dns"] = "System.Net.NameResolution.dll",
};

// ── Resolve known types to their DLL ──
if (string.IsNullOrEmpty(dllPath) && !string.IsNullOrEmpty(typeFullName) &&
    knownTypeDlls.TryGetValue(typeFullName, out var knownDll))
{
    // Search for the DLL across all available runtime versions, picking the latest.
    var currentDir = Path.GetDirectoryName(typeof(object).Assembly.Location);
    // currentDir = ...\shared\Microsoft.NETCore.App\8.0.11\
    // runtimeRoot = ...\shared\Microsoft.NETCore.App\  (all version dirs)
    var runtimeRoot = currentDir is not null
        ? Directory.GetParent(currentDir)?.FullName
        : null;
    if (runtimeRoot is not null && Directory.Exists(runtimeRoot))
    {
        var latestCandidate = FindLatestFile(runtimeRoot, knownDll);
        if (latestCandidate is not null)
            dllPath = latestCandidate;
    }
}

if (string.IsNullOrEmpty(dllPath) || !File.Exists(dllPath))
{
    Console.Error.WriteLine($"ERROR: DLL not found: {dllPath}");
    return 1;
}

// ── List types mode ──
if (listTypes)
{
    var typeLister = new DllScanner();
    var types = typeLister.ListPublicTypes(dllPath);
    Console.WriteLine($"Public types in {Path.GetFileName(dllPath)}:");
    foreach (var (name, methodCount) in types)
        Console.WriteLine($"  {name} ({methodCount} methods)");
    return 0;
}

// ── All-types mode: scan every public type in the assembly ──
if (allTypes)
{
    var allAssemblyName = Path.GetFileNameWithoutExtension(dllPath);
    var baseOutput = outputDir ?? Path.GetFullPath(Path.Combine("output", allAssemblyName));

    Console.WriteLine("╔══════════════════════════════════════════════════╗");
    Console.WriteLine("║  Chaos IL2CPP AutoTestGenerator (ALL TYPES)    ║");
    Console.WriteLine("╚══════════════════════════════════════════════════╝");
    Console.WriteLine($"  DLL:  {dllPath}");
    Console.WriteLine($"  Out:  {baseOutput}");
    Console.WriteLine();

    var allScanner = new DllScanner();
    Console.WriteLine("[Phase 1/5] Scanning DLL for all public types...");
    IReadOnlyList<DllScanResult> allScanResults;
    try
    {
        allScanResults = allScanner.ScanAll(dllPath, namespaceFilter);
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"ERROR: ScanAll failed: {ex.Message}");
        return 1;
    }

    if (allScanResults.Count == 0)
    {
        Console.WriteLine("No types with public methods found. Exiting.");
        return 0;
    }

    // Shared pipeline services (reused across all types)
    var allSerializer = new CSharpSerializer();
    var allExpressionBuilder = new CSharpExpressionBuilder(allSerializer);
    var allAutoFixture = new AutoFixtureAllower(allSerializer);
    var allValueGenerator = new ValueGenerator(allSerializer, allAutoFixture);
    var allProbeEmitter = new ProbeEmitter(allSerializer, allExpressionBuilder);
    var allEmitter = new TestEmitter(allSerializer, allExpressionBuilder);
    var allWriter = new ProjectWriter();

    int totalTypes = allScanResults.Count;
    int okTypes = 0;
    var allProbeResults = new List<ProbeResult>();

    for (int ti = 0; ti < totalTypes; ti++)
    {
        var oneResult = allScanResults[ti];
        var typeOutputDir = Path.Combine(baseOutput, SanitizePath(oneResult.TypeFullName));

        Console.WriteLine($"\n[{ti + 1}/{totalTypes}] {oneResult.TypeFullName}");

        if (oneResult.Methods.Count == 0)
        {
            Console.WriteLine("  No methods to generate. Skipping.");
            continue;
        }

        try
        {
        // ── Phase 2: Generate parameter values ──
        var oneValueSets = new List<IReadOnlyList<ValueSet>>();
        int totalSetsOne = 0;
        foreach (var (method, mi) in oneResult.Methods.Select((m, i) => (m, i)))
        {
            var sets = allValueGenerator.Generate(method, mi);
            oneValueSets.Add(sets);
            totalSetsOne += sets.Count;
        }
        Console.WriteLine($"  Generated {totalSetsOne} value sets across {oneResult.Methods.Count} methods");

        // ── Phase 3: Probe (── skip-probe ──)
        var oneProbeResults = skipProbe
            ? System.Array.Empty<ProbeResult>()
            : allProbeEmitter.Probe(
                typeOutputDir, oneResult.AssemblyName, oneResult.TypeFullName,
                oneResult.Methods, oneValueSets, dllPath, oneResult.TargetFramework);

        if (oneProbeResults.Count > 0)
        {
            int deterministic = oneProbeResults.Count(r => r.IsDeterministic);
            int nonDeterministic = oneProbeResults.Count(r => !r.IsDeterministic);
            int exceptions = oneProbeResults.Count(r => r.HasException);
            Console.WriteLine($"  {deterministic} deterministic, {nonDeterministic} non-deterministic, {exceptions} exception-throwing");
            allProbeResults.AddRange(oneProbeResults);
        }

        // ── Phase 4: Emit test source ──
        var oneSource = allEmitter.Emit(
            oneResult.AssemblyName, oneResult.TypeFullName, oneResult.TypeNamespace,
            oneResult.Methods, oneValueSets, oneProbeResults);

        var oneReport = allEmitter.GenerateReport(
            oneResult.AssemblyName, oneResult.TypeFullName,
            oneResult.Methods, oneValueSets, oneProbeResults,
            oneResult.SkippedMethods);

        // ── Phase 5: Write project ──
        allWriter.WriteAll(typeOutputDir, oneResult.AssemblyName, oneResult.TypeFullName,
            oneSource, oneProbeResults, oneReport, oneResult.TargetFramework);

        Console.WriteLine($"Coverage: {oneReport.AutoGeneratedMethods} auto / {oneReport.BenchmarkOnlyMethods} bench-only ({oneReport.TotalSubjects} subjects)");
        okTypes++;
        }
        catch (Exception ex_)
        {
            Console.WriteLine($"  [SKIP] Type processing failed: {ex_.Message}");
        }    }

    Console.WriteLine($"\nDone. {okTypes}/{totalTypes} types processed successfully.");

    // ── Emit subjects.metadata.json ──
    if (emitMetadataPath is not null)
    {
        var slug = chunkSlug ?? allAssemblyName;
        var methodEntries = new List<SubjectMethodEntry>();
        var customEntryIndices = new List<int>();
        var benchmarkMethodIndices = new List<int>();
        var hotupdateMethodIndices = new List<int>();

        // Build a lookup: SubjectId → ProbeResult
        var probeLookup = new Dictionary<string, ProbeResult>(StringComparer.Ordinal);
        foreach (var pr in allProbeResults)
            probeLookup.TryAdd(pr.SubjectId, pr);

        int globalIdx = 0;
        foreach (var scan in allScanResults)
        {
            foreach (var method in scan.Methods)
            {
                var paramsStr = string.Join(",", method.Parameters.Select(p => p.TypeName));
                var subjectId = $"{scan.AssemblyName}/{method.DeclaringTypeFullName}::{method.Name}:{method.ReturnTypeName}({paramsStr})";

                string kind;
                bool isBenchmark;

                if (probeLookup.TryGetValue(subjectId, out var pr))
                {
                    if (pr.HasException)
                    {
                        kind = "fact";
                        isBenchmark = false;
                    }
                    else if (pr.IsDeterministic && !pr.IsVoid)
                    {
                        kind = "fact";
                        isBenchmark = false;
                    }
                    else
                    {
                        kind = "benchmark";
                        isBenchmark = true;
                    }
                }
                else
                {
                    // No probe result (probe build failed) — default to benchmark
                    kind = "benchmark";
                    isBenchmark = true;
                }

                methodEntries.Add(new SubjectMethodEntry(globalIdx, kind, subjectId));
                if (isBenchmark)
                    benchmarkMethodIndices.Add(globalIdx);
                else
                    customEntryIndices.Add(globalIdx);
                globalIdx++;
            }
        }

        var metadata = new SubjectsMetadata(
            SchemaVersion: 1,
            AssemblyName: allAssemblyName,
            ChunkSlug: slug,
            TotalMethods: globalIdx,
            CustomEntryIndices: customEntryIndices.Count > 0 ? customEntryIndices : null,
            BenchmarkMethodIndices: benchmarkMethodIndices.Count > 0 ? benchmarkMethodIndices : null,
            HotupdateMethodIndices: null,
            Methods: methodEntries);

        var metaJson = JsonSerializer.Serialize(metadata, new JsonSerializerOptions
        {
            WriteIndented = true,
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        });
        var metaDir = Path.GetDirectoryName(emitMetadataPath)!;
        if (!string.IsNullOrEmpty(metaDir))
            Directory.CreateDirectory(metaDir);
        File.WriteAllText(emitMetadataPath, metaJson);
        Console.WriteLine($"[Metadata] {emitMetadataPath}  ({globalIdx} subjects)");
    }

    return 0;
}

if (string.IsNullOrEmpty(typeFullName))
{
    Console.Error.WriteLine("ERROR: --type is required (or use --list-types or --all-types)");
    return 1;
}

var assemblyName = Path.GetFileNameWithoutExtension(dllPath);
outputDir ??= Path.GetFullPath(Path.Combine("output", assemblyName, SanitizePath(typeFullName)));

Console.WriteLine("╔══════════════════════════════════════════════════╗");
Console.WriteLine("║  Chaos IL2CPP AutoTestGenerator                 ║");
Console.WriteLine("╚══════════════════════════════════════════════════╝");
Console.WriteLine($"  DLL:  {dllPath}");
Console.WriteLine($"  Type: {typeFullName}");
Console.WriteLine($"  Out:  {outputDir}");
Console.WriteLine();

// ── Phase 1: Scan ──
Console.WriteLine("[Phase 1/5] Scanning DLL for methods...");
var scanner = new DllScanner();
DllScanResult scanResult;
try
{
    scanResult = scanner.Scan(dllPath, typeFullName);
}
catch (Exception ex)
{
    Console.Error.WriteLine($"ERROR: Scan failed: {ex.Message}");
    return 1;
}

Console.WriteLine($"  Found {scanResult.Methods.Count} public methods" +
    (scanResult.SkippedMethods.Count > 0
        ? $" ({scanResult.SkippedMethods.Count} skipped: {string.Join(", ", scanResult.SkippedMethods)})"
        : ""));
Console.WriteLine();

if (scanResult.Methods.Count == 0)
{
    Console.WriteLine("No methods to generate. Exiting.");
    return 0;
}

// ── Phase 2: Generate parameter values ──
Console.WriteLine("[Phase 2/5] Generating parameter values (AutoFixture + boundary)...");
var serializer = new CSharpSerializer();
var expressionBuilder = new CSharpExpressionBuilder(serializer);
var autoFixture = new AutoFixtureAllower(serializer);
var valueGenerator = new ValueGenerator(serializer, autoFixture);

var allValueSets = new List<IReadOnlyList<ValueSet>>();
int totalSets = 0;
foreach (var (method, mi) in scanResult.Methods.Select((m, i) => (m, i)))
{
    var sets = valueGenerator.Generate(method, mi);
    allValueSets.Add(sets);
    totalSets += sets.Count;
}
Console.WriteLine($"  Generated {totalSets} value sets across {scanResult.Methods.Count} methods");
Console.WriteLine();

// ── Phase 3: Probe ──
Console.WriteLine("[Phase 3/5] Running probe (2-pass) to capture expected values...");
var probeEmitter = new ProbeEmitter(serializer, expressionBuilder);
var probeResults = probeEmitter.Probe(
    outputDir, scanResult.AssemblyName, scanResult.TypeFullName,
    scanResult.Methods, allValueSets, dllPath, scanResult.TargetFramework);

if (probeResults.Count == 0)
{
    Console.Error.WriteLine("WARNING: Probe returned no results. Generating benchmark-only output.");
}
else
{
    int deterministic = probeResults.Count(r => r.IsDeterministic);
    int nonDeterministic = probeResults.Count(r => !r.IsDeterministic);
    int exceptions = probeResults.Count(r => r.HasException);
    Console.WriteLine($"  {deterministic} deterministic, {nonDeterministic} non-deterministic, {exceptions} exception-throwing");
}
Console.WriteLine();

// ── Phase 4: Emit test source ──
Console.WriteLine("[Phase 4/5] Emitting test source code...");
var emitter = new TestEmitter(serializer, expressionBuilder);
var autoGeneratedSource = emitter.Emit(
    scanResult.AssemblyName, scanResult.TypeFullName, scanResult.TypeNamespace,
    scanResult.Methods, allValueSets, probeResults);

var report = emitter.GenerateReport(
    scanResult.AssemblyName, scanResult.TypeFullName,
    scanResult.Methods, allValueSets, probeResults,
    scanResult.SkippedMethods);
Console.WriteLine();

// ── Phase 5: Write project ──
Console.WriteLine("[Phase 5/5] Writing project files...");
var writer = new ProjectWriter();
writer.WriteAll(outputDir, scanResult.AssemblyName, scanResult.TypeFullName,
    autoGeneratedSource, probeResults, report, scanResult.TargetFramework);

Console.WriteLine();
Console.WriteLine("╔══════════════════════════════════════════════════╗");
Console.WriteLine("║  Generation complete!                           ║");
Console.WriteLine("╚══════════════════════════════════════════════════╝");
Console.WriteLine();
Console.WriteLine("Coverage:");
Console.WriteLine($"  Total methods:        {report.TotalMethods}");
Console.WriteLine($"  Auto-generated:       {report.AutoGeneratedMethods}");
Console.WriteLine($"  Benchmark-only:       {report.BenchmarkOnlyMethods}");
Console.WriteLine($"  Skipped:              {report.SkippedMethods}");
Console.WriteLine($"  Total test subjects:  {report.TotalSubjects}");
Console.WriteLine();
Console.WriteLine($"Output: {outputDir}");
Console.WriteLine();
Console.WriteLine("To build with asserts:");
Console.WriteLine($"  dotnet build \"{Path.Combine(outputDir, SanitizePath(typeFullName) + ".csproj")}\" -p:DefineConstants=VERIFY");
Console.WriteLine();
Console.WriteLine("To build for benchmarks (asserts eliminated):");
Console.WriteLine($"  dotnet build \"{Path.Combine(outputDir, SanitizePath(typeFullName) + ".csproj")}\"");

return 0;

static string? FindLatestFile(string searchRoot, string fileName)
{
    string? latest = null;
    Version? best = null;
    foreach (var dir in Directory.EnumerateDirectories(searchRoot))
    {
        var versionName = Path.GetFileName(dir);
        if (!Version.TryParse(versionName, out var version)) continue;
        var candidate = Path.Combine(dir, fileName);
        if (!File.Exists(candidate)) continue;
        if (best is null || version > best)
        {
            best = version;
            latest = candidate;
        }
    }
    return latest;
}

static string SanitizePath(string name)
{
    return name.Replace('.', '_').Replace('<', '_').Replace('>', '_').Replace('`', '_');
}

// ── Subjects metadata records (mirrors TPG format for pipeline interop) ──
internal sealed record SubjectsMetadata(
    int SchemaVersion,
    string AssemblyName,
    string ChunkSlug,
    int TotalMethods,
    IReadOnlyList<int>? CustomEntryIndices,
    IReadOnlyList<int>? BenchmarkMethodIndices,
    IReadOnlyList<int>? HotupdateMethodIndices,
    IReadOnlyList<SubjectMethodEntry> Methods);

internal sealed record SubjectMethodEntry(
    int Index,
    string Kind,
    string MethodSubjectId);
