namespace Chaos.IL2CPP.Generator;

internal sealed record GeneratedRuntimeSkeletonFamilyPluginMetadata(
    string Id,
    CodeGenPluginProductLine ProductLine,
    int Order,
    RuntimeSkeletonFamilyPluginKind Kind,
    string MethodName);

internal sealed record GeneratedLoweringPlanPluginMetadata(
    string Id,
    CodeGenPluginProductLine ProductLine,
    int Order,
    string MethodName);

internal interface ICodegenTemplateBundlePlugin
{
}

internal sealed record GeneratedTemplateBundlePluginMetadata(
    string Id,
    CodeGenPluginProductLine ProductLine,
    string Domain,
    int Order,
    string TypeName);

internal interface IRuntimeProviderPlugin
{
}

internal sealed record GeneratedRuntimeProviderPluginMetadata(
    string Id,
    CodeGenPluginProductLine ProductLine,
    string CapabilityFamily,
    int Version,
    int Order,
    string TypeName);
