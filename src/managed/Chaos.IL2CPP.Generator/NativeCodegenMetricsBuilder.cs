using System.Diagnostics;
using System.Text;
using System.Text.RegularExpressions;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal static partial class NativeCodegenMetricsBuilder
{
    private static readonly Regex ExternCSymbolRegex = ExternCSymbolPattern();

    public static NativeCodegenMetricsArtifact Build(
        string codegenKind,
        string planKind,
        IEnumerable<(string RelativePath, string Contents)> generatedSources,
        int structuredMethodCount = 0,
        int structuredExceptionBodyCount = 0,
        int totalMethodCount = 0,
        int aotReachableMethodCount = 0,
        int aotUnreachableMethodCount = 0,
        int hotpatchEntryCount = 0,
        int hotpatchEligibleMethodCount = 0)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(codegenKind);
        ArgumentException.ThrowIfNullOrWhiteSpace(planKind);
        ArgumentNullException.ThrowIfNull(generatedSources);

        var generatedSourceItems = generatedSources.ToArray();
        var generatedSourcePaths = generatedSourceItems
            .Select(item => item.RelativePath)
            .Where(path => !string.IsNullOrWhiteSpace(path))
            .ToArray();
        var generatedSourceByteCounts = generatedSourceItems
            .Select(item => (long)Encoding.UTF8.GetByteCount(item.Contents))
            .ToArray();
        var emittedSymbols = new HashSet<string>(StringComparer.Ordinal);

        foreach (var generatedSource in generatedSourceItems)
        {
            foreach (Match match in ExternCSymbolRegex.Matches(generatedSource.Contents))
            {
                if (match.Groups["symbol"].Success)
                {
                    emittedSymbols.Add(match.Groups["symbol"].Value);
                }
            }
        }

        return new NativeCodegenMetricsArtifact
        {
            CodegenKind = codegenKind,
            PlanKind = planKind,
            GeneratedSourcePaths = generatedSourcePaths,
            GeneratedCppFileCount = generatedSourceItems.Length,
            GeneratedCppTotalBytes = generatedSourceByteCounts.Sum(),
            LargestGeneratedCppBytes = generatedSourceByteCounts.Length == 0 ? 0 : generatedSourceByteCounts.Max(),
            GeneratedSymbolCount = emittedSymbols.Count,
            PeakWorkingSetBytes = Process.GetCurrentProcess().PeakWorkingSet64,
            StructuredMethodCount = structuredMethodCount,
            StructuredExceptionBodyCount = structuredExceptionBodyCount,
            TotalMethodCount = totalMethodCount,
            StructuredRecoveryRate = totalMethodCount > 0
                ? (double)(structuredMethodCount + structuredExceptionBodyCount) / totalMethodCount
                : 1.0,
            AotReachableMethodCount = aotReachableMethodCount,
            AotUnreachableMethodCount = aotUnreachableMethodCount,
            HotpatchEntryCount = hotpatchEntryCount,
            HotpatchEligibleMethodCount = hotpatchEligibleMethodCount,
            HotpatchDispatchCoverage = hotpatchEligibleMethodCount > 0
                ? (double)hotpatchEntryCount / hotpatchEligibleMethodCount
                : 1.0,
        };
    }

    [GeneratedRegex("extern\\s+\"C\"\\s+[^\\(\\r\\n]+\\s+(?<symbol>[A-Za-z_][A-Za-z0-9_]*)\\s*\\(")]
    private static partial Regex ExternCSymbolPattern();
}
