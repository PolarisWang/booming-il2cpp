using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests;

public sealed class NativeCodegenMetricsBuilderTests
{
    [Fact]
    public void Build_BasicMetrics_ReturnsCorrectValues()
    {
        var result = NativeCodegenMetricsBuilder.Build(
            codegenKind: "native-aot",
            planKind: "full-assembly-entry",
            generatedSources: new[] { ("file1.cpp", "extern \"C\" void Foo();") },
            structuredMethodCount: 10,
            structuredExceptionBodyCount: 2,
            flatFallbackCount: 1,
            totalMethodCount: 13,
            aotReachableMethodCount: 12,
            aotUnreachableMethodCount: 1);

        Assert.Equal("native-aot", result.CodegenKind);
        Assert.Equal("full-assembly-entry", result.PlanKind);
        Assert.Equal(1, result.GeneratedCppFileCount);
        Assert.Equal(10, result.StructuredMethodCount);
        Assert.Equal(2, result.StructuredExceptionBodyCount);
        Assert.Equal(1, result.FlatFallbackCount);
        Assert.Equal(13, result.TotalMethodCount);
        Assert.Equal(12, result.AotReachableMethodCount);
        Assert.Equal(1, result.AotUnreachableMethodCount);
    }

    [Fact]
    public void Build_WithExternSymbols_CountsThem()
    {
        var sources = new[]
        {
            ("a.cpp", "extern \"C\" void Foo();\nextern \"C\" int Bar();"),
            ("b.cpp", "extern \"C\" void Baz();"),
        };

        var result = NativeCodegenMetricsBuilder.Build(
            codegenKind: "native-aot",
            planKind: "full-assembly",
            generatedSources: sources);

        Assert.Equal(3, result.GeneratedSymbolCount);
    }

    [Fact]
    public void Build_DuplicateExternSymbol_Deduplicates()
    {
        var sources = new[]
        {
            ("a.cpp", "extern \"C\" void Foo();"),
            ("b.cpp", "extern \"C\" void Foo();"),
        };

        var result = NativeCodegenMetricsBuilder.Build(
            codegenKind: "native-aot",
            planKind: "full-assembly",
            generatedSources: sources);

        Assert.Equal(1, result.GeneratedSymbolCount);
    }

    [Fact]
    public void Build_EmptySources_ReturnsZeroCounts()
    {
        var result = NativeCodegenMetricsBuilder.Build(
            codegenKind: "native-aot",
            planKind: "full-assembly",
            generatedSources: Array.Empty<(string, string)>());

        Assert.Equal(0, result.GeneratedCppFileCount);
        Assert.Equal(0, result.GeneratedCppTotalBytes);
        Assert.Equal(0, result.LargestGeneratedCppBytes);
        Assert.Equal(0, result.GeneratedSymbolCount);
    }

    [Fact]
    public void Build_WithByteCount_SumsCorrectly()
    {
        var sources = new[] { ("f.cpp", "Hello"), ("g.cpp", "World!") };

        var result = NativeCodegenMetricsBuilder.Build(
            codegenKind: "native-aot",
            planKind: "full-assembly",
            generatedSources: sources);

        // "Hello" = 5 bytes, "World!" = 6 bytes
        Assert.Equal(11, result.GeneratedCppTotalBytes);
        Assert.Equal(6, result.LargestGeneratedCppBytes);
        Assert.Equal(2, result.GeneratedCppFileCount);
    }

    [Fact]
    public void Build_NullCodegenKind_Throws()
    {
        // .NET 8's ThrowIfNullOrWhiteSpace throws ArgumentNullException for null input
        Assert.Throws<ArgumentNullException>(() => NativeCodegenMetricsBuilder.Build(
            codegenKind: null!,
            planKind: "full-assembly",
            generatedSources: Array.Empty<(string, string)>()));
    }

    [Fact]
    public void Build_EmptyPlanKind_Throws()
    {
        // .NET 8's ThrowIfNullOrWhiteSpace throws ArgumentException for empty string
        Assert.Throws<ArgumentException>(() => NativeCodegenMetricsBuilder.Build(
            codegenKind: "native-aot",
            planKind: "",
            generatedSources: Array.Empty<(string, string)>()));
    }

    [Fact]
    public void Build_StructuredRecoveryRate_CalculatedCorrectly()
    {
        var result = NativeCodegenMetricsBuilder.Build(
            codegenKind: "native-aot",
            planKind: "full-assembly",
            generatedSources: Array.Empty<(string, string)>(),
            structuredMethodCount: 90,
            structuredExceptionBodyCount: 5,
            totalMethodCount: 100);

        Assert.Equal(0.95, result.StructuredRecoveryRate);
    }

    [Fact]
    public void Build_ZeroTotalMethodCount_RecoveryRateIsOne()
    {
        var result = NativeCodegenMetricsBuilder.Build(
            codegenKind: "native-aot",
            planKind: "full-assembly",
            generatedSources: Array.Empty<(string, string)>());

        Assert.Equal(1.0, result.StructuredRecoveryRate);
    }
}
