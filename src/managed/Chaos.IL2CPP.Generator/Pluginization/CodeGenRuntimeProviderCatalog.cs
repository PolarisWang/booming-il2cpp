namespace Chaos.IL2CPP.Generator;

internal static class CodeGenRuntimeProviderCatalog
{
    public static IReadOnlyList<IRuntimeProviderPlugin> GetNativeReferenceProviders() =>
        NativeReferenceRuntimeProviderRegistry.Plugins;

    public static IReadOnlyList<IRuntimeProviderPlugin> GetNativeAotProviders() =>
        NativeAotRuntimeProviderRegistry.Plugins;
}
