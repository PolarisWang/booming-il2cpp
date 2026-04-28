namespace Chaos.IL2CPP.CodeGen;

internal static class CodeGenRuntimeProviderCatalog
{
    public static IReadOnlyList<IRuntimeProviderPlugin> GetNativeReferenceProviders() =>
        NativeReferenceRuntimeProviderRegistry.Plugins;

    public static IReadOnlyList<IRuntimeProviderPlugin> GetNativeAotProviders() =>
        NativeAotRuntimeProviderRegistry.Plugins;
}
