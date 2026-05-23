namespace Chaos.IL2CPP.Generator;

internal enum CodeGenPluginProductLine
{
    NativeReference,
    NativeAot,
}

internal enum RuntimeSkeletonFamilyPluginKind
{
    ManagedInvoke,
    Kernel,
    PlatformCapability,
    ImportedBridge,
    Hybrid,
}

[AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
internal sealed class RuntimeSkeletonFamilyPluginAttribute : Attribute
{
    public RuntimeSkeletonFamilyPluginAttribute(
        string id,
        CodeGenPluginProductLine productLine,
        int order,
        RuntimeSkeletonFamilyPluginKind kind)
    {
        Id = id;
        ProductLine = productLine;
        Order = order;
        Kind = kind;
    }

    public string Id { get; }

    public CodeGenPluginProductLine ProductLine { get; }

    public int Order { get; }

    public RuntimeSkeletonFamilyPluginKind Kind { get; }
}

[AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
internal sealed class LoweringPlanPluginAttribute : Attribute
{
    public LoweringPlanPluginAttribute(
        string id,
        CodeGenPluginProductLine productLine,
        int order,
        string[]? targetCapabilityFamilies = null)
    {
        Id = id;
        ProductLine = productLine;
        Order = order;
        TargetCapabilityFamilies = targetCapabilityFamilies ?? [];
    }

    public string Id { get; }

    public CodeGenPluginProductLine ProductLine { get; }

    public int Order { get; }

    /// <summary>
    /// Optional capability family filter. When non-empty, this plugin only
    /// activates for methods whose capability set intersects with these families.
    /// When empty, the plugin is always considered (current default).
    /// </summary>
    public IReadOnlyList<string> TargetCapabilityFamilies { get; }
}

[AttributeUsage(AttributeTargets.Class, AllowMultiple = false)]
internal sealed class CodegenTemplateBundlePluginAttribute : Attribute
{
    public CodegenTemplateBundlePluginAttribute(
        string id,
        CodeGenPluginProductLine productLine,
        string domain,
        int order = 0)
    {
        Id = id;
        ProductLine = productLine;
        Domain = domain;
        Order = order;
    }

    public string Id { get; }

    public CodeGenPluginProductLine ProductLine { get; }

    public string Domain { get; }

    public int Order { get; }
}

[AttributeUsage(AttributeTargets.Class, AllowMultiple = false)]
internal sealed class RuntimeProviderPluginAttribute : Attribute
{
    public RuntimeProviderPluginAttribute(
        string id,
        CodeGenPluginProductLine productLine,
        string capabilityFamily,
        int version,
        int order = 0)
    {
        Id = id;
        ProductLine = productLine;
        CapabilityFamily = capabilityFamily;
        Version = version;
        Order = order;
    }

    public string Id { get; }

    public CodeGenPluginProductLine ProductLine { get; }

    public string CapabilityFamily { get; }

    public int Version { get; }

    public int Order { get; }
}
