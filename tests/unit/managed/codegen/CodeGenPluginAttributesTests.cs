using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

public sealed class CodeGenPluginAttributesTests
{
    // ── Enums ──────────────────────────────────────────────────────────

    [Fact]
    public void CodeGenPluginProductLine_HasExpectedValues()
    {
        Assert.Equal(0, (int)CodeGenPluginProductLine.NativeReference);
        Assert.Equal(1, (int)CodeGenPluginProductLine.NativeAot);
    }

    [Fact]
    public void RuntimeSkeletonFamilyPluginKind_HasExpectedValues()
    {
        Assert.Equal(0, (int)RuntimeSkeletonFamilyPluginKind.ManagedInvoke);
        Assert.Equal(1, (int)RuntimeSkeletonFamilyPluginKind.Kernel);
        Assert.Equal(2, (int)RuntimeSkeletonFamilyPluginKind.PlatformCapability);
        Assert.Equal(3, (int)RuntimeSkeletonFamilyPluginKind.ImportedBridge);
        Assert.Equal(4, (int)RuntimeSkeletonFamilyPluginKind.Hybrid);
    }

    // ── RuntimeSkeletonFamilyPluginAttribute ────────────────────────────

    [Fact]
    public void RuntimeSkeletonFamilyPluginAttribute_StoresValues()
    {
        var attr = new RuntimeSkeletonFamilyPluginAttribute("test-id", CodeGenPluginProductLine.NativeAot, 42, RuntimeSkeletonFamilyPluginKind.Kernel);
        Assert.Equal("test-id", attr.Id);
        Assert.Equal(CodeGenPluginProductLine.NativeAot, attr.ProductLine);
        Assert.Equal(42, attr.Order);
        Assert.Equal(RuntimeSkeletonFamilyPluginKind.Kernel, attr.Kind);
    }

    [Fact]
    public void RuntimeSkeletonFamilyPluginAttribute_DefaultValues()
    {
        var attr = new RuntimeSkeletonFamilyPluginAttribute("", CodeGenPluginProductLine.NativeReference, 0, RuntimeSkeletonFamilyPluginKind.ManagedInvoke);
        Assert.Equal("", attr.Id);
        Assert.Equal(CodeGenPluginProductLine.NativeReference, attr.ProductLine);
        Assert.Equal(0, attr.Order);
        Assert.Equal(RuntimeSkeletonFamilyPluginKind.ManagedInvoke, attr.Kind);
    }

    [Fact]
    public void RuntimeSkeletonFamilyPluginAttribute_UsageTarget()
    {
        var attrType = typeof(RuntimeSkeletonFamilyPluginAttribute);
        var usage = (AttributeUsageAttribute)attrType.GetCustomAttributes(typeof(AttributeUsageAttribute), false)[0];
        Assert.Equal(AttributeTargets.Method, usage.ValidOn);
        Assert.False(usage.AllowMultiple);
    }

    // ── LoweringPlanPluginAttribute ────────────────────────────────────

    [Fact]
    public void LoweringPlanPluginAttribute_StoresValues()
    {
        var attr = new LoweringPlanPluginAttribute("plan-id", CodeGenPluginProductLine.NativeAot, 7);
        Assert.Equal("plan-id", attr.Id);
        Assert.Equal(CodeGenPluginProductLine.NativeAot, attr.ProductLine);
        Assert.Equal(7, attr.Order);
    }

    [Fact]
    public void LoweringPlanPluginAttribute_UsageTarget()
    {
        var attrType = typeof(LoweringPlanPluginAttribute);
        var usage = (AttributeUsageAttribute)attrType.GetCustomAttributes(typeof(AttributeUsageAttribute), false)[0];
        Assert.Equal(AttributeTargets.Method, usage.ValidOn);
        Assert.False(usage.AllowMultiple);
    }

    // ── CodegenTemplateBundlePluginAttribute ────────────────────────────

    [Fact]
    public void CodegenTemplateBundlePluginAttribute_StoresValues()
    {
        var attr = new CodegenTemplateBundlePluginAttribute("bundle-id", CodeGenPluginProductLine.NativeReference, "test-domain", 99);
        Assert.Equal("bundle-id", attr.Id);
        Assert.Equal(CodeGenPluginProductLine.NativeReference, attr.ProductLine);
        Assert.Equal("test-domain", attr.Domain);
        Assert.Equal(99, attr.Order);
    }

    [Fact]
    public void CodegenTemplateBundlePluginAttribute_DefaultOrder()
    {
        var attr = new CodegenTemplateBundlePluginAttribute("b", CodeGenPluginProductLine.NativeAot, "d");
        Assert.Equal(0, attr.Order);
    }

    [Fact]
    public void CodegenTemplateBundlePluginAttribute_UsageTarget()
    {
        var attrType = typeof(CodegenTemplateBundlePluginAttribute);
        var usage = (AttributeUsageAttribute)attrType.GetCustomAttributes(typeof(AttributeUsageAttribute), false)[0];
        Assert.Equal(AttributeTargets.Class, usage.ValidOn);
        Assert.False(usage.AllowMultiple);
    }

    // ── RuntimeProviderPluginAttribute ──────────────────────────────────

    [Fact]
    public void RuntimeProviderPluginAttribute_StoresValues()
    {
        var attr = new RuntimeProviderPluginAttribute("provider-id", CodeGenPluginProductLine.NativeAot, "capability-family", 3, 5);
        Assert.Equal("provider-id", attr.Id);
        Assert.Equal(CodeGenPluginProductLine.NativeAot, attr.ProductLine);
        Assert.Equal("capability-family", attr.CapabilityFamily);
        Assert.Equal(3, attr.Version);
        Assert.Equal(5, attr.Order);
    }

    [Fact]
    public void RuntimeProviderPluginAttribute_DefaultOrder()
    {
        var attr = new RuntimeProviderPluginAttribute("p", CodeGenPluginProductLine.NativeReference, "cf", 1);
        Assert.Equal(0, attr.Order);
    }

    [Fact]
    public void RuntimeProviderPluginAttribute_UsageTarget()
    {
        var attrType = typeof(RuntimeProviderPluginAttribute);
        var usage = (AttributeUsageAttribute)attrType.GetCustomAttributes(typeof(AttributeUsageAttribute), false)[0];
        Assert.Equal(AttributeTargets.Class, usage.ValidOn);
        Assert.False(usage.AllowMultiple);
    }
}
