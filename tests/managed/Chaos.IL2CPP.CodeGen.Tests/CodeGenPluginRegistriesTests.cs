using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests;

public sealed class CodeGenPluginRegistriesTests
{
    [Fact]
    public void GeneratedRuntimeSkeletonFamilyPluginMetadata_StoresValues()
    {
        var meta = new GeneratedRuntimeSkeletonFamilyPluginMetadata(
            "test-id",
            CodeGenPluginProductLine.NativeReference,
            42,
            RuntimeSkeletonFamilyPluginKind.ManagedInvoke,
            "TestMethod");
        Assert.Equal("test-id", meta.Id);
        Assert.Equal(CodeGenPluginProductLine.NativeReference, meta.ProductLine);
        Assert.Equal(42, meta.Order);
        Assert.Equal(RuntimeSkeletonFamilyPluginKind.ManagedInvoke, meta.Kind);
        Assert.Equal("TestMethod", meta.MethodName);
    }

    [Fact]
    public void GeneratedRuntimeSkeletonFamilyPluginMetadata_NativeAotProductLine()
    {
        var meta = new GeneratedRuntimeSkeletonFamilyPluginMetadata(
            "aot-id",
            CodeGenPluginProductLine.NativeAot,
            0,
            RuntimeSkeletonFamilyPluginKind.Kernel,
            "AotMethod");
        Assert.Equal("aot-id", meta.Id);
        Assert.Equal(CodeGenPluginProductLine.NativeAot, meta.ProductLine);
        Assert.Equal(RuntimeSkeletonFamilyPluginKind.Kernel, meta.Kind);
    }

    [Fact]
    public void GeneratedLoweringPlanPluginMetadata_StoresValues()
    {
        var meta = new GeneratedLoweringPlanPluginMetadata(
            "plan-id",
            CodeGenPluginProductLine.NativeReference,
            7,
            "ExecutePlan");
        Assert.Equal("plan-id", meta.Id);
        Assert.Equal(CodeGenPluginProductLine.NativeReference, meta.ProductLine);
        Assert.Equal(7, meta.Order);
        Assert.Equal("ExecutePlan", meta.MethodName);
    }

    [Fact]
    public void GeneratedLoweringPlanPluginMetadata_NativeAotProductLine()
    {
        var meta = new GeneratedLoweringPlanPluginMetadata(
            "aot-plan",
            CodeGenPluginProductLine.NativeAot,
            1,
            "BuildAotPlan");
        Assert.Equal(CodeGenPluginProductLine.NativeAot, meta.ProductLine);
        Assert.Equal("BuildAotPlan", meta.MethodName);
    }

    [Fact]
    public void GeneratedTemplateBundlePluginMetadata_StoresValues()
    {
        var meta = new GeneratedTemplateBundlePluginMetadata(
            "bundle-1",
            CodeGenPluginProductLine.NativeReference,
            "runtime",
            3,
            "RuntimeBundle");
        Assert.Equal("bundle-1", meta.Id);
        Assert.Equal(CodeGenPluginProductLine.NativeReference, meta.ProductLine);
        Assert.Equal("runtime", meta.Domain);
        Assert.Equal(3, meta.Order);
        Assert.Equal("RuntimeBundle", meta.TypeName);
    }

    [Fact]
    public void GeneratedTemplateBundlePluginMetadata_DefaultOrder()
    {
        var meta = new GeneratedTemplateBundlePluginMetadata(
            "bundle-2",
            CodeGenPluginProductLine.NativeAot,
            "templates",
            0,
            "AotBundle");
        Assert.Equal(0, meta.Order);
        Assert.Equal("AotBundle", meta.TypeName);
    }

    [Fact]
    public void GeneratedRuntimeProviderPluginMetadata_StoresValues()
    {
        var meta = new GeneratedRuntimeProviderPluginMetadata(
            "provider-1",
            CodeGenPluginProductLine.NativeReference,
            "GC",
            2,
            5,
            "GcProvider");
        Assert.Equal("provider-1", meta.Id);
        Assert.Equal(CodeGenPluginProductLine.NativeReference, meta.ProductLine);
        Assert.Equal("GC", meta.CapabilityFamily);
        Assert.Equal(2, meta.Version);
        Assert.Equal(5, meta.Order);
        Assert.Equal("GcProvider", meta.TypeName);
    }

    [Fact]
    public void GeneratedRuntimeProviderPluginMetadata_NativeAotProvider()
    {
        var meta = new GeneratedRuntimeProviderPluginMetadata(
            "aot-provider",
            CodeGenPluginProductLine.NativeAot,
            "Threading",
            1,
            0,
            "ThreadingProvider");
        Assert.Equal(CodeGenPluginProductLine.NativeAot, meta.ProductLine);
        Assert.Equal("Threading", meta.CapabilityFamily);
        Assert.Equal("ThreadingProvider", meta.TypeName);
    }
}

public sealed class GlobalGenericSharingDirectiveTests
{
    [Fact]
    public void Default_StoresValues()
    {
        var dict = new Dictionary<string, string>
        {
            ["A.G.M`1[[System.Int32]]::Method()"] = "A.G.M`1[[System.Object]]::Method()",
        };
        var directive = new GlobalGenericSharingDirective
        {
            CanonicalMapEntries = dict,
            Version = "v1",
        };
        Assert.Same(dict, directive.CanonicalMapEntries);
        Assert.Equal("v1", directive.Version);
    }

    [Fact]
    public void CurrentVersion_IsV1()
    {
        Assert.Equal("v1", GlobalGenericSharingDirective.CurrentVersion);
    }

    [Fact]
    public void EmptyCanonicalMap_Allowed()
    {
        var directive = new GlobalGenericSharingDirective
        {
            CanonicalMapEntries = new Dictionary<string, string>(),
            Version = "v1",
        };
        Assert.Empty(directive.CanonicalMapEntries);
    }
}

public sealed class CodeGenRuntimeProviderCatalogTests
{
    [Fact]
    public void GetNativeReferenceProviders_ReturnsList()
    {
        var providers = CodeGenRuntimeProviderCatalog.GetNativeReferenceProviders();
        Assert.NotNull(providers);
    }

    [Fact]
    public void GetNativeAotProviders_ReturnsList()
    {
        var providers = CodeGenRuntimeProviderCatalog.GetNativeAotProviders();
        Assert.NotNull(providers);
    }
}
