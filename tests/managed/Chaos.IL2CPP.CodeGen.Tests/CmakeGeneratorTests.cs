using Chaos.IL2CPP.Generator;
using Chaos.IL2CPP.Generator.BuildSystem;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

public sealed class CmakeGeneratorTests
{
    private static NativeAotResult MakeResult(string assemblyName, string[] sourcePaths)
    {
        return new NativeAotResult
        {
            OutputRootPath = "/tmp/out",
            LoweringPlan = new NativeAotLoweringPlanArtifact
            {
                PlanKind = "generic-managed-entry",
                AssemblyName = assemblyName,
                EntrySubjectId = "Test/Prog::Main:System.Int32()",
                NativeEntryFunctionName = "RunNativeAot",
                EntrySymbol = "Test_Prog_Main",
                EntryMethodToken = "0u",
                WorkloadAbi = "int()",
            },
            Manifest = new NativeAotManifestArtifact
            {
                AssemblyName = assemblyName,
                EntrySubjectId = "Test/Prog::Main:System.Int32()",
                ManagedClosureRootPath = "/tmp/out",
                PlanArtifactPath = "native-aot-plan.json",
                GeneratedArtifacts = [],
            },
            CodegenMetrics = new NativeCodegenMetricsArtifact
            {
                CodegenKind = "native-aot",
                PlanKind = "generic-managed-entry",
                GeneratedSourcePaths = sourcePaths,
            },
            GeneratedSources = sourcePaths.Select(p => new NativeAotGeneratedSource
            {
                RelativePath = p,
                Contents = "// test",
            }).ToArray(),
        };
    }

    [Fact]
    public void Generate_EmptyAssemblyList_ProducesCmakeOutput()
    {
        var gen = new CmakeGenerator("/repo");
        var result = gen.Generate(
            Array.Empty<NativeAotResult>(),
            nativeLibDir: "/native-lib",
            buildConfig: "Debug");
        Assert.NotEmpty(result);
        Assert.Contains("cmake_minimum_required", result);
    }

    [Fact]
    public void Generate_SingleAssemblyWithCppSource_CreatesStaticLibraryTarget()
    {
        var gen = new CmakeGenerator("/repo");
        var results = new[]
        {
            MakeResult("TestAssembly", new[] { "generated/test.cpp" }),
        };
        var result = gen.Generate(results, "/native-lib");

        Assert.Contains("chaos_gen_TestAssembly", result);
        Assert.Contains("STATIC", result);
        Assert.Contains("generated/test.cpp", result);
    }

    [Fact]
    public void Generate_MultipleAssemblies_CreatesMultipleTargets()
    {
        var gen = new CmakeGenerator("/repo");
        var results = new[]
        {
            MakeResult("Asm1", new[] { "asm1/a.cpp" }),
            MakeResult("Asm2", new[] { "asm2/b.cpp" }),
        };
        var result = gen.Generate(results, "/native-lib");

        Assert.Contains("chaos_gen_Asm1", result);
        Assert.Contains("chaos_gen_Asm2", result);
        Assert.Contains("asm1/a.cpp", result);
        Assert.Contains("asm2/b.cpp", result);
    }

    [Fact]
    public void Generate_AssemblyWithNonCppSources_FiltersToCppOnly()
    {
        var gen = new CmakeGenerator("/repo");
        var results = new[]
        {
            MakeResult("FilterTest", new[] { "a.cpp", "b.h", "c.hpp", "d.txt" }),
        };
        var result = gen.Generate(results, "/native-lib");

        Assert.Contains("a.cpp", result);
        Assert.DoesNotContain("b.h", result);
        Assert.DoesNotContain("c.hpp", result);
        Assert.DoesNotContain("d.txt", result);
    }

    [Fact]
    public void Generate_AssemblyNameWithDots_SanitizesTargetName()
    {
        var gen = new CmakeGenerator("/repo");
        var results = new[]
        {
            MakeResult("My.Library.Core", new[] { "gen/test.cpp" }),
        };
        var result = gen.Generate(results, "/native-lib");

        Assert.Contains("chaos_gen_My_Library_Core", result);
    }

    [Fact]
    public void Generate_WithExtraSources_IncludesThem()
    {
        var gen = new CmakeGenerator("/repo");
        var results = new[]
        {
            MakeResult("Test", new[] { "a.cpp" }),
        };
        var result = gen.Generate(
            results,
            "/native-lib",
            extraSources: new[] { "shared/helper.cpp", "shared/utils.cpp" });

        Assert.Contains("shared/helper.cpp", result);
        Assert.Contains("shared/utils.cpp", result);
    }

    [Fact]
    public void Generate_WithCustomBuildConfig_UsesIt()
    {
        var gen = new CmakeGenerator("/repo");
        var results = new[]
        {
            MakeResult("Test", new[] { "a.cpp" }),
        };
        var result = gen.Generate(results, "/native-lib", buildConfig: "Release");

        Assert.Contains("Release", result);
    }

    [Fact]
    public void Generate_WithCustomTargetName_UsesIt()
    {
        var gen = new CmakeGenerator("/repo");
        var results = Array.Empty<NativeAotResult>();
        var result = gen.Generate(results, "/native-lib", targetName: "my_custom_exe");

        Assert.Contains("my_custom_exe", result);
    }

    [Fact]
    public void Generate_NoAssemblySources_SkipsTarget()
    {
        var gen = new CmakeGenerator("/repo");
        var results = new[]
        {
            MakeResult("EmptyAsm", Array.Empty<string>()),
        };
        var result = gen.Generate(results, "/native-lib");

        Assert.DoesNotContain("chaos_gen_EmptyAsm", result);
    }

    [Fact]
    public void SanitizeTargetName_WithSpecialChars_ReplacesThem()
    {
        var gen = new CmakeGenerator("/repo");
        var results = new[]
        {
            MakeResult("My-Assembly(v2)", new[] { "a.cpp" }),
        };
        var result = gen.Generate(results, "/native-lib");

        Assert.Contains("chaos_gen_My_Assembly_v2_", result);
    }

    [Fact]
    public void Generate_BackslashInRepoRoot_NormalizesToForwardSlash()
    {
        var gen = new CmakeGenerator("C:\\repo\\path");
        var result = gen.Generate(
            Array.Empty<NativeAotResult>(),
            nativeLibDir: "D:\\native\\lib");

        Assert.Contains("C:/repo/path", result);
        Assert.Contains("D:/native/lib", result);
    }
}
