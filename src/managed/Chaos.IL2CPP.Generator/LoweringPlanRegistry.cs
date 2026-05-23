namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Registry-based dispatch for lowering plan selection. Replaces sequential
/// TryCreate* chains with capability-matched plugin dispatch.
///
/// Phase 2 (G12): Supports ProductLine + capability family matching to select
/// the correct lowering plan builder for a given method. Falls through to
/// the built-in shape detectors (constructorThenInstanceCall, staticCallCtorGetter)
/// when no registered plugin matches.
///
/// Future phases: Will integrate with auto-generated PluginMetadata from
/// CodeGenPluginRegistryGenerator for fully dynamic dispatch.
/// </summary>
internal static class LoweringPlanRegistry
{
    /// <summary>
    /// Try to find a lowering plan for the given method by matching against
    /// registered capability families. Returns the plan kind if matched.
    /// </summary>
    public static bool TryMatchCapabilityFamily(
        IReadOnlyList<string> methodCapabilities,
        IReadOnlyList<string> targetFamilies,
        out string? matchedFamily)
    {
        matchedFamily = null;

        if (targetFamilies.Count == 0 || methodCapabilities.Count == 0)
            return false;

        // Check if any method capability intersects with target families
        foreach (var capability in methodCapabilities)
        {
            foreach (var targetFamily in targetFamilies)
            {
                if (string.Equals(capability, targetFamily, StringComparison.Ordinal))
                {
                    matchedFamily = targetFamily;
                    return true;
                }
            }
        }

        return false;
    }

    /// <summary>
    /// Get lowering plan plugins registered for the given product line,
    /// ordered by their declared priority. NativeAot lowering plan registry
    /// is not yet auto-generated (G12 Phase 2 partial — NativeReference only).
    /// </summary>
    public static IReadOnlyList<GeneratedLoweringPlanPluginMetadata> GetPlugins(
        CodeGenPluginProductLine productLine)
    {
        return productLine switch
        {
            CodeGenPluginProductLine.NativeReference => NativeReferenceLoweringPlanRegistry.Plugins,
            _ => [],
        };
    }
}
