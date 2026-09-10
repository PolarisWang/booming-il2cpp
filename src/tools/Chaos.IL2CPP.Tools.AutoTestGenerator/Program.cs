using System.Text;
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
    Console.Error.WriteLine("  --patch-mode  Generate patch subjects DLL (implies --skip-probe, returns .cs path)");
    Console.Error.WriteLine("  --generate-wrappers  Generate CombinedSubjects.cs from custom .cs files");
    Console.Error.WriteLine("    --custom-cs-files <file1;file2;...>  Semicolon-separated list of .Custom.cs paths");
    Console.Error.WriteLine("    --output <dir>      Output directory for generated files");
    Console.Error.WriteLine("    --slug <chunk-slug> Chunk slug for metadata");
    Console.Error.WriteLine("    --sdk-csproj <path> Path to Chaos.TestFramework.Sdk.csproj");
    Console.Error.WriteLine("    --tfm <netX.Y>      Target framework moniker (e.g. net10.0)");
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
bool patchMode = false;
bool generateWrappers = false;
string? customCsFiles = null;
string? wrapperSlug = null;
string? sdkCsproj = null;
string? tfm = null;
string? capabilitiesPath = null;

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
        case "--patch-mode":
            patchMode = true;
            skipProbe = true;  // patch mode implies skip probe
            break;
        case "--generate-wrappers":
            generateWrappers = true;
            break;
        case "--custom-cs-files" when i + 1 < args.Length:
            customCsFiles = args[++i];
            break;
        case "--slug" when i + 1 < args.Length:
            wrapperSlug = args[++i];
            break;
        case "--sdk-csproj" when i + 1 < args.Length:
            sdkCsproj = args[++i];
            break;
        case "--tfm" when i + 1 < args.Length:
            tfm = args[++i];
            break;
        case "--capabilities-path" when i + 1 < args.Length:
            capabilitiesPath = args[++i];
            break;
    }
}

// ── Generate wrappers mode: generate CombinedSubjects.cs from custom .cs files ──
if (generateWrappers)
{
    if (string.IsNullOrEmpty(customCsFiles) || string.IsNullOrEmpty(outputDir) ||
        string.IsNullOrEmpty(wrapperSlug) || string.IsNullOrEmpty(sdkCsproj) || string.IsNullOrEmpty(tfm))
    {
        Console.Error.WriteLine("ERROR: --generate-wrappers requires --custom-cs-files, --output, --slug, --sdk-csproj, --tfm");
        return 1;
    }

    var csFileList = customCsFiles.Split(';', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries).ToList();
    if (csFileList.Count == 0)
    {
        Console.Error.WriteLine("ERROR: --custom-cs-files is empty");
        return 1;
    }

    // Validate that all custom .cs files exist
    var missingFiles = csFileList.Where(f => !File.Exists(f)).ToList();
    if (missingFiles.Count > 0)
    {
        Console.Error.WriteLine($"ERROR: Custom .cs files not found: {string.Join("; ", missingFiles)}");
        return 1;
    }

    if (!File.Exists(sdkCsproj))
    {
        Console.Error.WriteLine($"ERROR: SDK csproj not found: {sdkCsproj}");
        return 1;
    }

    Console.WriteLine("╔══════════════════════════════════════════════════╗");
    Console.WriteLine("║  Chaos IL2CPP AutoTestGenerator (WRAPPERS)     ║");
    Console.WriteLine("╚══════════════════════════════════════════════════╝");
    Console.WriteLine($"  Custom .cs files: {csFileList.Count}");
    Console.WriteLine($"  Output:           {outputDir}");
    Console.WriteLine($"  Slug:             {wrapperSlug}");
    Console.WriteLine($"  SDK csproj:       {sdkCsproj}");
    Console.WriteLine($"  TFM:              {tfm}");
    Console.WriteLine();

    var result = WrapperEmitter.GenerateWrappers(csFileList, outputDir, wrapperSlug, sdkCsproj, tfm);
    if (result is null)
    {
        Console.WriteLine("  No subject methods found in custom .cs files.");
        Console.WriteLine("[Output] (no files generated)");
        return 0;
    }

    Console.WriteLine($"[Output] {result.Count} methods wrapped");
    return 0;
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

// ── Patch mode: generate patch subjects DLL ──
if (patchMode)
{
    if (!allTypes)
    {
        Console.Error.WriteLine("ERROR: --patch-mode requires --all-types");
        return 1;
    }
    return RunPatchMode(dllPath, namespaceFilter, outputDir, capabilitiesPath);
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
    var typeLister = new DllScanner(capabilitiesPath);
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

    var allScanner = new DllScanner(capabilitiesPath);
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

        // Filter out methods that generate non-compilable CombinedSubjects code
        // (e.g., AddContext<JsonSerializerContext> where abstract type fails new() constraint).
        int beforeFilter = oneResult.Methods.Count;
        var filtered = oneResult.Methods.Where(m => m.Name != "AddContext").ToList();
        if (filtered.Count < beforeFilter)
        {
            Console.WriteLine($"  Filtered {beforeFilter - filtered.Count} method(s) with unresolvable constraints");
            oneResult = oneResult with { Methods = filtered };
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
            ? oneResult.Methods.Select((m, mi) => {
                var subjectId = $"{oneResult.AssemblyName}/{oneResult.TypeFullName}::{m.Name}:{m.ReturnTypeName}({string.Join(",", m.Parameters.Select(p => p.TypeName))})";
                var isVoid = m.ReturnTypeName is "System.Void" or "void";
                return new ProbeResult(
                    mi, 0, subjectId, isVoid, false, null,
                    isVoid ? null : 0L, null, m.ReturnTypeName,
                    true, null
                );
            }).ToList()
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
            oneResult.Methods, oneValueSets, oneProbeResults,
            net10OnlyMethods: oneResult.Net10OnlyMethods);

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
            int mi = 0;
            foreach (var method in scan.Methods)
            {
                var paramsStr = string.Join(",", method.Parameters.Select(p => p.TypeName));
                var subjectId = $"{scan.AssemblyName}/{method.DeclaringTypeFullName}::{method.Name}:{method.ReturnTypeName}({paramsStr})";

                // Skip methods whose generic type arguments can't satisfy new() constraints
                // at compile time (e.g., AddContext<JsonSerializerContext> where TContext is abstract).
                // These produce CS0310 errors in CombinedSubjects.cs.
                if (paramsStr.Contains("JsonSerializerContext") || method.Name is "AddContext")
                {
                    mi++;
                    continue;
                }

                string kind;
                bool isBenchmark;

                if (probeLookup.TryGetValue(subjectId, out var pr))
                {
                    if (pr.HasException)
                    {
                        // Exception-throwing: hotupdate only, no [Benchmark]
                        kind = "hotupdate";
                        isBenchmark = false;
                    }
                    else if (pr.IsDeterministic && !pr.IsVoid)
                    {
                        // Deterministic non-void: hotupdate + [Benchmark]
                        kind = "hotupdate";
                        isBenchmark = true;
                    }
                    else
                    {
                        // Deterministic void / non-deterministic: fact + [Benchmark]
                        kind = "fact";
                        isBenchmark = true;
                    }
                }
                else
                {
                    // No probe data: fact + [Benchmark]
                    kind = "fact";
                    isBenchmark = true;
                }

                // ── Generate per-value-set entries ──
                // IMPORTANT: Must use the same sanitization as TestEmitter.SanitizeIdentifier
                // to ensure generatedMethodId matches the actual C# method name in CombinedSubjects.
                // SanitizeIdentifier keeps letters/digits/_, replaces . with _, drops all other chars.
                // SanitizePath (which replaces commas/spaces with _) would produce mismatched names.
                var paramSuffix = string.Join("_", method.Parameters.Select(p =>
                    SanitizeLikeTestEmitter(CSharpSerializer.MapToCSharpType(p.TypeName))));

                int setCount;
                try
                {
                    var tempSets = allValueGenerator.Generate(method, mi);
                    setCount = tempSets.Count;
                }
                catch
                {
                    setCount = 1;
                }

                for (int si = 0; si < setCount; si++)
                {
                    var generatedMethodId = $"{SanitizePath(method.Name)}_{mi}_{paramSuffix}_{si}";

                    methodEntries.Add(new SubjectMethodEntry(globalIdx, kind, subjectId, generatedMethodId));
                    if (isBenchmark)
                        benchmarkMethodIndices.Add(globalIdx);
                    else
                    {
                        customEntryIndices.Add(globalIdx);
                        // For non-CoreLib assemblies, also mark fact subjects as
                        // benchmarks so benchmark stages have data for these methods.
                        // CoreLib is excluded because it has enough non-deterministic
                        // methods (Vector/Intrinsics/Web) that already fill the
                        // benchmark list — adding deterministic facts would double
                        // the benchmark runtime with little added value.
                        if (allAssemblyName != "System.Private.CoreLib")
                            benchmarkMethodIndices.Add(globalIdx);
                    }
                    globalIdx++;
                }

                mi++;
            }
        }

        // All fact subjects also have [HotUpdate] attribute — populate indices for TPG metadata
        hotupdateMethodIndices.AddRange(customEntryIndices);
        hotupdateMethodIndices.AddRange(benchmarkMethodIndices);

        // FactMethodCount: count of unique fact subject IDs (not value sets).
        // TotalMethods counts per-value-set entries (~500), while FactMethodCount
        // counts unique wrapper methods (~99). Pipeline uses FactMethodCount for
        // mismatch detection against kSubjectEntryCount.
        var uniqueFactSubjectIds = methodEntries
            .Where(e => !e.Kind.Contains("benchmark", StringComparison.OrdinalIgnoreCase))
            .Select(e => e.MethodSubjectId)
            .Distinct(StringComparer.Ordinal)
            .Count();

        var metadata = new SubjectsMetadata(
            SchemaVersion: 1,
            AssemblyName: allAssemblyName,
            ChunkSlug: slug,
            TotalMethods: globalIdx,
            FactMethodCount: uniqueFactSubjectIds,
            CustomEntryIndices: customEntryIndices.Count > 0 ? customEntryIndices : null,
            BenchmarkMethodIndices: benchmarkMethodIndices,
            HotupdateMethodIndices: hotupdateMethodIndices.Count > 0 ? hotupdateMethodIndices : null,
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
var scanner = new DllScanner(capabilitiesPath);
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
    scanResult.Methods, allValueSets, probeResults,
    net10OnlyMethods: scanResult.Net10OnlyMethods);

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
    return name.Replace('.', '_').Replace('<', '_').Replace('>', '_')
               .Replace('`', '_').Replace('[', '_').Replace(']', '_')
               .Replace('*', '_').Replace('&', '_')
               .Replace(',', '_').Replace(' ', '_');
}

/// <summary>
/// Sanitize like TestEmitter.SanitizeIdentifier: keep letters/digits/_,
/// replace . with _, drop all other chars (commas, spaces, angle brackets, etc.).
/// This ensures generatedMethodId matches the actual C# method name in CombinedSubjects.
/// </summary>
static string SanitizeLikeTestEmitter(string name)
{
    var sb = new System.Text.StringBuilder();
    foreach (var c in name)
    {
        if (char.IsLetterOrDigit(c) || c == '_')
            sb.Append(c);
        else if (c == '.')
            sb.Append('_');
    }
    if (sb.Length > 0 && char.IsDigit(sb[0]))
        sb.Insert(0, '_');
    return sb.ToString();
}

static string EscapeCSharpKeyword(string name)
{
    // @-prefix C# reserved keywords so they're valid as type names in declarations
    return name switch
    {
        "abstract" or "as" or "base" or "break" or "case" or "catch" or "checked" or "class" or "const"
        or "continue" or "default" or "delegate" or "do" or "else" or "enum" or "event" or "explicit"
        or "extern" or "false" or "finally" or "fixed" or "for" or "foreach" or "goto" or "if" or "implicit"
        or "in" or "interface" or "internal" or "is" or "isnot" or "lock" or "namespace" or "new" or "null"
        or "operator" or "out" or "override" or "params" or "private" or "protected" or "public"
        or "readonly" or "record" or "ref" or "return" or "sealed" or "sizeof" or "stackalloc" or "static"
        or "struct" or "switch" or "this" or "throw" or "true" or "try" or "typeof" or "unchecked"
        or "unsafe" or "using" or "virtual" or "volatile" or "while" => "@" + name,
        _ => name,
    };
}

// ── Patch mode: generate patch subjects .cs file ──
// Scans DLL, generates values, and emits a single .cs file with [HotUpdate] subject
// methods using GetPatchReturnExpression for return values.
// The pipeline compiles this into PatchSubjects.dll for PatchDataExtractor.
static int RunPatchMode(string dllPath, string? namespaceFilter, string? outputDir, string? capabilitiesPath)
{
    var assemblyName = Path.GetFileNameWithoutExtension(dllPath);
    var baseOutput = outputDir ?? Path.GetFullPath(Path.Combine("output", assemblyName));

    Console.WriteLine("╔══════════════════════════════════════════════════╗");
    Console.WriteLine("║  Chaos IL2CPP AutoTestGenerator (PATCH MODE)   ║");
    Console.WriteLine("╚══════════════════════════════════════════════════╝");
    Console.WriteLine($"  DLL:  {dllPath}");
    Console.WriteLine($"  Out:  {baseOutput}");
    Console.WriteLine();

    var scanner = new DllScanner(capabilitiesPath);
    Console.WriteLine("[Phase 1/3] Scanning DLL...");
    IReadOnlyList<DllScanResult> scanResults;
    try
    {
        scanResults = scanner.ScanAll(dllPath, namespaceFilter);
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"ERROR: Scan failed: {ex.Message}");
        return 1;
    }

    if (scanResults.Count == 0)
    {
        Console.WriteLine("No types found. Nothing to generate.");
        return 0;
    }

    var serializer = new CSharpSerializer();
    var expressionBuilder = new CSharpExpressionBuilder(serializer);
    var autoFixture = new AutoFixtureAllower(serializer);
    var valueGenerator = new ValueGenerator(serializer, autoFixture);

    Console.WriteLine("[Phase 2/3] Generating values and emitting patch source...");
    var sb = new StringBuilder();
    sb.AppendLine("// Auto-generated patch subjects for hotupdate verification");
    sb.AppendLine("// Each method matches baseline [HotUpdate] subject signature,");
    sb.AppendLine("// but returns a transformed value for semantic change detection.");
    sb.AppendLine("using System;");
    sb.AppendLine("using System.Buffers;");
    sb.AppendLine("using System.Collections;");
    sb.AppendLine("using System.Collections.Concurrent;");
    sb.AppendLine("using System.Collections.Generic;");
    sb.AppendLine("using System.ComponentModel;");
    sb.AppendLine("using System.ComponentModel.Design;");
    sb.AppendLine("using System.Data;");
    sb.AppendLine("using System.Data.Common;");
    sb.AppendLine("using System.Diagnostics;");
    sb.AppendLine("using System.Diagnostics.Metrics;");
    sb.AppendLine("using System.Dynamic;");
    sb.AppendLine("using System.Globalization;");
    sb.AppendLine("using System.IO;");
    sb.AppendLine("using System.Linq;");
    sb.AppendLine("using System.Linq.Expressions;");
    sb.AppendLine("using System.Net;");
    sb.AppendLine("using System.Net.Sockets;");
    sb.AppendLine("using System.Reflection;");
    sb.AppendLine("using System.Reflection.Emit;");
    sb.AppendLine("using System.Runtime.CompilerServices;");
    sb.AppendLine("using System.Runtime.InteropServices;");
    sb.AppendLine("using System.Runtime.Serialization;");
    sb.AppendLine("using System.Security.Claims;");
    sb.AppendLine("using System.Text;");
    sb.AppendLine("using System.Text.Json;");
    sb.AppendLine("using System.Text.Json.Serialization.Metadata;");
    sb.AppendLine("using Chaos.TestFramework;");

    // Assembly-specific using directives (resolves types not in standard .NET)
    if (string.Equals(assemblyName, "System.Net.ServerSentEvents", StringComparison.Ordinal))
        sb.AppendLine("using System.Net.ServerSentEvents;");
    if (string.Equals(assemblyName, "System.Text.Json", StringComparison.Ordinal))
    {
        sb.AppendLine("using System.Text.Json.Nodes;");
        sb.AppendLine("using System.Text.Json.Schema;");
    }
    if (string.Equals(assemblyName, "System.Private.Xml", StringComparison.Ordinal))
    {
        sb.AppendLine("using System.Xml;");
        sb.AppendLine("using System.Xml.Schema;");
        sb.AppendLine("using System.Xml.Serialization;");
        sb.AppendLine("using System.Xml.Xsl;");
        sb.AppendLine("using System.Xml.XPath;");
        sb.AppendLine("using System.Xml.Linq;");
    }
    if (string.Equals(assemblyName, "System.Security.Cryptography", StringComparison.Ordinal))
        sb.AppendLine("using System.Security.Cryptography;");
    sb.AppendLine();

    var patchClassName = SanitizePath(assemblyName) + "PatchSubjects";
    sb.AppendLine($"namespace AutoGenerated.Patch");
    sb.AppendLine("{");
    sb.AppendLine($"    public sealed unsafe class {patchClassName}");
    sb.AppendLine("    {");

    int subjectIndex = 0;
    int patchedCount = 0;

    foreach (var scan in scanResults)
    {
        var typeValueSets = new List<IReadOnlyList<ValueSet>>();
        foreach (var (method, mi) in scan.Methods.Select((m, i) => (m, i)))
        {
            var sets = valueGenerator.Generate(method, mi);
            typeValueSets.Add(sets);
        }

        var typeProbeResults = System.Array.Empty<ProbeResult>(); // skip probe in patch mode

        for (int mi = 0; mi < scan.Methods.Count; mi++)
        {
            var method = scan.Methods[mi];
            var sets = typeValueSets[mi];

            foreach (var set in sets)
            {
                var paramSuffix = string.Join("_", method.Parameters.Select(p =>
                    SanitizePath(CSharpSerializer.MapToCSharpType(p.TypeName))));
                var methodSuffix = $"{SanitizePath(method.Name)}_{mi}_{paramSuffix}_{set.SetIndex}";

                // Build arg list with variable declarations for out/ref
                var prelude = new List<string>();
                var finalArgs = new List<string>();
                int refVarCounter = 0;
                var refParamTypes = new List<string>(); // track types for XOR encoding

                for (int pi = 0; pi < method.Parameters.Count; pi++)
                {
                    var param = method.Parameters[pi];
                    var argExpr = set.ArgumentExpressions[pi];

                    if (param.IsOut || param.IsRef)
                    {
                        var baseTypeName = param.TypeName.EndsWith('&')
                            ? param.TypeName[..^1].Trim()
                            : param.TypeName;
                        var csType = CSharpSerializer.MapToCSharpType(baseTypeName);
                        refParamTypes.Add(baseTypeName);

                        // argExpr contains the fully-qualified type name (e.g. "default(System.Numerics.Matrix3x2)!")
                        // Extract it to use for the variable declaration, since csType may lack namespace
                        // when MLC can't resolve the type (CS0246).
                        var declType = csType;
                        if (!csType.Contains('.') && !csType.Contains('<') &&
                            csType is not ("bool" or "byte" or "sbyte" or "short" or "ushort"
                                or "int" or "uint" or "long" or "ulong" or "float" or "double"
                                or "decimal" or "char" or "string" or "void" or "object"))
                        {
                            // Try to extract fully-qualified type from argExpr like "default(TypeName)!"
                            var m = System.Text.RegularExpressions.Regex.Match(argExpr, @"default\(([^)]+)\)");
                            if (m.Success && m.Groups[1].Value.Contains('.'))
                                declType = m.Groups[1].Value;
                        }

                        var varName = $"__ref_{mi}_{set.SetIndex}_{refVarCounter++}";
                        prelude.Add($"            {EscapeCSharpKeyword(declType)} {varName} = default;");
                        finalArgs.Add(param.IsOut ? $"out {varName}" : $"ref {varName}");
                    }
                    else
                    {
                        finalArgs.Add(DisambiguateArg(param.TypeName, argExpr, serializer));
                    }
                }

                var preludeStr = prelude.Count > 0
                    ? string.Join("\n", prelude) + "\n"
                    : "";

                var argsStr = string.Join(", ", finalArgs);
                var genericSuffix = method.GenericTypeArgs is { Count: > 0 }
                    ? $"<{string.Join(", ", method.GenericTypeArgs.Select(CSharpSerializer.MapToCSharpType))}>"
                    : "";

                var instanceExpr = expressionBuilder.GetInstanceExpression(scan.TypeFullName, method.IsStatic);
                var callExpr = $"{instanceExpr}.{method.Name}{genericSuffix}({argsStr})";

                var rt = method.ReturnTypeName;
                var isPlainTask = rt is "System.Threading.Tasks.Task" or "System.Threading.Tasks.ValueTask";
                if (isPlainTask || rt.StartsWith("System.Threading.Tasks.Task<") ||
                    rt.StartsWith("System.Threading.Tasks.ValueTask<"))
                    callExpr += ".GetAwaiter().GetResult()";

                var hasRefParam = method.HasRefParam;
                var isVoid = method.IsVoid;

                // For pure void methods (no ref params), skip — they have no [HotUpdate]
                if (isVoid && !hasRefParam)
                    continue;

                // Emit patch subject method — same call+assert as baseline, different return
                sb.AppendLine();
                sb.AppendLine($"        public long Subject_{subjectIndex}_{methodSuffix}()");
                sb.AppendLine("        {");

                if (isVoid || isPlainTask)
                {
                    sb.AppendLine($"{preludeStr}            {callExpr};");
                    if (hasRefParam)
                    {
                        // Encode ref/out values into the return so hotupdate can detect
                        // semantic changes in ref/out parameters.  XOR each ref/out value
                        // (cast to long) into the return sentinel.  If any ref/out value
                        // differs between baseline and patch, the combined XOR changes.
                        var refParts = new List<string>();
                        for (int rvi = 0; rvi < refVarCounter; rvi++)
                        {
                            var vn = $"__ref_{mi}_{set.SetIndex}_{rvi}";
                            var paramType = rvi < refParamTypes.Count ? refParamTypes[rvi] : "object";
                            // Only integer-compatible types can be cast to long directly.
                            // For reference types, use null check as a safe sentinel.
                            // For pointer types (byte*, void*, etc.), use null comparison
                            // since (object)(byte*) is illegal (CS0030).
                            if (IsIntegerCompatibleType(paramType))
                                refParts.Add($"((long)({vn}) ^ 0xFF)");
                            else if (paramType.EndsWith("*"))
                                refParts.Add($"({vn} != null ? 1L : 0L)");
                            else
                                refParts.Add($"((object)({vn}) != null ? 1L : 0L)");
                        }
                        sb.AppendLine("            return " + string.Join(" ^ ", refParts) + ";");
                    }
                    else
                    {
                        sb.AppendLine("            return 142L;");
                    }
                }
                else
                {
                    sb.AppendLine($"{preludeStr}            var result = {callExpr};");
                    // For non-void methods with ref/out params, XOR ref/out values into sentinel
                    if (refVarCounter > 0)
                    {
                        var refParts = new List<string>
                        {
                            $"({ValueGenerator.GetPatchReturnExpression(method.ReturnTypeName, "result")})"
                        };
                        for (int rvi = 0; rvi < refVarCounter; rvi++)
                        {
                            var vn = $"__ref_{mi}_{set.SetIndex}_{rvi}";
                            var paramType = rvi < refParamTypes.Count ? refParamTypes[rvi] : "object";
                            if (IsIntegerCompatibleType(paramType))
                                refParts.Add($"((long)({vn}) ^ 0xFF)");
                            else if (paramType.EndsWith("*"))
                                refParts.Add($"({vn} != null ? 1L : 0L)");
                            else
                                refParts.Add($"((object)({vn}) != null ? 1L : 0L)");
                        }
                        sb.AppendLine("            return " + string.Join(" ^ ", refParts) + ";");
                    }
                    else
                    {
                        sb.AppendLine($"            return {ValueGenerator.GetPatchReturnExpression(method.ReturnTypeName, "result")};");
                    }
                }

                sb.AppendLine("        }");
                patchedCount++;
                subjectIndex++;
            }
        }
    }

    sb.AppendLine("    }");
    sb.AppendLine("}");

    // Write output
    var patchDir = Path.Combine(baseOutput, "patch");
    Directory.CreateDirectory(patchDir);
    var csPath = Path.Combine(patchDir, $"{patchClassName}.cs");
    File.WriteAllText(csPath, sb.ToString());
    Console.WriteLine($"[Patch] {csPath}  ({patchedCount} patch subjects)");
    Console.WriteLine($"[Output] {csPath}");

    return 0;
}

static bool IsIntegerCompatibleType(string typeName)
{
    return typeName is "System.Byte" or "System.SByte"
        or "System.Int16" or "System.UInt16"
        or "System.Int32" or "System.UInt32"
        or "System.Int64" or "System.UInt64"
        or "System.IntPtr" or "System.UIntPtr"
        or "System.Char" or "System.Single" or "System.Double"
        or "System.Decimal" or "System.Half";
}

// Helper for disambiguating arg casts (reused from TestEmitter pattern)
static string DisambiguateArg(string paramType, string argExpr, CSharpSerializer serializer)
{
    if (argExpr == "null!")
        return $"({CSharpSerializer.MapToCSharpType(paramType)})null!";

    var castNeeded = new HashSet<string>(StringComparer.Ordinal)
    {
        "System.Boolean", "System.Byte", "System.SByte", "System.Int16", "System.UInt16",
        "System.Int32", "System.UInt32", "System.Int64", "System.UInt64", "System.Single",
        "System.Double", "System.Decimal", "System.Char",
    };

    if (!castNeeded.Contains(paramType)) return argExpr;
    if (argExpr.Length == 0) return argExpr;
    var first = argExpr[0];
    if (!char.IsDigit(first) && first != '-' && first != '\"') return argExpr;
    if (argExpr.StartsWith('(')) return argExpr;
    var csType = CSharpSerializer.MapToCSharpType(paramType);
    return $"({csType}){argExpr}";
}

// ── Subjects metadata records (mirrors TPG format for pipeline interop) ──
internal sealed record SubjectsMetadata(
    int SchemaVersion,
    string AssemblyName,
    string ChunkSlug,
    int TotalMethods,
    int FactMethodCount,
    IReadOnlyList<int>? CustomEntryIndices,
    IReadOnlyList<int>? BenchmarkMethodIndices,
    IReadOnlyList<int>? HotupdateMethodIndices,
    IReadOnlyList<SubjectMethodEntry> Methods);

internal sealed record SubjectMethodEntry(
    int Index,
    string Kind,
    string MethodSubjectId,
    string? GeneratedMethodId = null);
