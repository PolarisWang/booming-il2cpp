using Chaos.IL2CPP.Generator.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

/// <summary>
/// Tests for <see cref="AssemblyFullClosureAuditEmitter"/> — plan validation,
/// page-count math, and generated-file emission.
/// </summary>
public sealed class AssemblyFullClosureAuditEmitterTests
{
    private const string DefaultAssembly = "TestAssembly";
    private const string DefaultPlanKind = "assembly-full-closure-audit";

    [Fact]
    public void ValidatePlan_ValidInput_Passes()
    {
        AssemblyFullClosureAuditEmitter.ValidatePlan(
            DefaultPlanKind,
            DefaultAssembly,
            translationUnitMode: "audit-only",
            translationUnitMethodSubjectIds: new[] { "Mod.Type::Method" },
            translationUnitPageSize: 100,
            translationUnitPageCount: 1,
            translationUnitPages: new[]
            {
                new AuditTranslationUnitPageArtifact
                {
                    PageNumber = 1,
                    MethodCount = 1,
                    Path = "pages/page1.json",
                },
            },
            expectedPageSize: 100);
    }

    [Fact]
    public void ValidatePlan_WrongPlanKind_Throws()
    {
        var ex = Assert.Throws<InvalidOperationException>(() =>
            AssemblyFullClosureAuditEmitter.ValidatePlan(
                "full-assembly",
                DefaultAssembly,
                translationUnitMode: "audit-only",
                translationUnitMethodSubjectIds: new[] { "Mod.Type::Method" },
                translationUnitPageSize: 100,
                translationUnitPageCount: 1,
                translationUnitPages: new[]
                {
                    new AuditTranslationUnitPageArtifact
                    {
                        PageNumber = 1,
                        MethodCount = 1,
                        Path = "pages/page1.json",
                    },
                },
                expectedPageSize: 100));
        Assert.Contains("expected plan kind", ex.Message);
    }

    [Fact]
    public void ValidatePlan_NullAssemblyName_Throws()
    {
        var ex = Assert.Throws<InvalidOperationException>(() =>
            AssemblyFullClosureAuditEmitter.ValidatePlan(
                DefaultPlanKind,
                null!,
                translationUnitMode: "audit-only",
                translationUnitMethodSubjectIds: new[] { "Mod.Type::Method" },
                translationUnitPageSize: 100,
                translationUnitPageCount: 1,
                translationUnitPages: new[]
                {
                    new AuditTranslationUnitPageArtifact
                    {
                        PageNumber = 1,
                        MethodCount = 1,
                        Path = "pages/page1.json",
                    },
                },
                expectedPageSize: 100));
        Assert.Contains("requires", ex.Message);
    }

    [Fact]
    public void ValidatePlan_WrongMode_Throws()
    {
        var ex = Assert.Throws<InvalidOperationException>(() =>
            AssemblyFullClosureAuditEmitter.ValidatePlan(
                DefaultPlanKind,
                DefaultAssembly,
                translationUnitMode: "codegen",
                translationUnitMethodSubjectIds: new[] { "Mod.Type::Method" },
                translationUnitPageSize: 100,
                translationUnitPageCount: 1,
                translationUnitPages: new[]
                {
                    new AuditTranslationUnitPageArtifact
                    {
                        PageNumber = 1,
                        MethodCount = 1,
                        Path = "pages/page1.json",
                    },
                },
                expectedPageSize: 100));
        Assert.Contains("translation-unit mode", ex.Message);
    }

    [Fact]
    public void ValidatePlan_NullMethodSubjectIds_Throws()
    {
        var ex = Assert.Throws<InvalidOperationException>(() =>
            AssemblyFullClosureAuditEmitter.ValidatePlan(
                DefaultPlanKind,
                DefaultAssembly,
                translationUnitMode: "audit-only",
                translationUnitMethodSubjectIds: null,
                translationUnitPageSize: 100,
                translationUnitPageCount: 1,
                translationUnitPages: new[]
                {
                    new AuditTranslationUnitPageArtifact
                    {
                        PageNumber = 1,
                        MethodCount = 1,
                        Path = "pages/page1.json",
                    },
                },
                expectedPageSize: 100));
        Assert.Contains("requires at least one", ex.Message);
    }

    [Fact]
    public void ValidatePlan_EmptyMethodSubjectIds_Throws()
    {
        var ex = Assert.Throws<InvalidOperationException>(() =>
            AssemblyFullClosureAuditEmitter.ValidatePlan(
                DefaultPlanKind,
                DefaultAssembly,
                translationUnitMode: "audit-only",
                translationUnitMethodSubjectIds: Array.Empty<string>(),
                translationUnitPageSize: 100,
                translationUnitPageCount: 1,
                translationUnitPages: new[]
                {
                    new AuditTranslationUnitPageArtifact
                    {
                        PageNumber = 1,
                        MethodCount = 1,
                        Path = "pages/page1.json",
                    },
                },
                expectedPageSize: 100));
        Assert.Contains("requires at least one", ex.Message);
    }

    [Fact]
    public void ValidatePlan_WrongPageSize_Throws()
    {
        var ex = Assert.Throws<InvalidOperationException>(() =>
            AssemblyFullClosureAuditEmitter.ValidatePlan(
                DefaultPlanKind,
                DefaultAssembly,
                translationUnitMode: "audit-only",
                translationUnitMethodSubjectIds: new[] { "Mod.Type::Method" },
                translationUnitPageSize: 50,
                translationUnitPageCount: 1,
                translationUnitPages: new[]
                {
                    new AuditTranslationUnitPageArtifact
                    {
                        PageNumber = 1,
                        MethodCount = 1,
                        Path = "pages/page1.json",
                    },
                },
                expectedPageSize: 100));
        Assert.Contains("page size", ex.Message);
    }

    [Fact]
    public void ValidatePlan_WrongPageCount_Throws()
    {
        var ex = Assert.Throws<InvalidOperationException>(() =>
            AssemblyFullClosureAuditEmitter.ValidatePlan(
                DefaultPlanKind,
                DefaultAssembly,
                translationUnitMode: "audit-only",
                translationUnitMethodSubjectIds: new[] { "Mod.Type::Method" },
                translationUnitPageSize: 100,
                translationUnitPageCount: 5,
                translationUnitPages: new[]
                {
                    new AuditTranslationUnitPageArtifact
                    {
                        PageNumber = 1,
                        MethodCount = 1,
                        Path = "pages/page1.json",
                    },
                },
                expectedPageSize: 100));
        Assert.Contains("page count", ex.Message);
    }

    [Fact]
    public void ValidatePlan_NullPages_Throws()
    {
        var ex = Assert.Throws<InvalidOperationException>(() =>
            AssemblyFullClosureAuditEmitter.ValidatePlan(
                DefaultPlanKind,
                DefaultAssembly,
                translationUnitMode: "audit-only",
                translationUnitMethodSubjectIds: new[] { "Mod.Type::Method" },
                translationUnitPageSize: 100,
                translationUnitPageCount: 1,
                translationUnitPages: null,
                expectedPageSize: 100));
        Assert.Contains("page metadata", ex.Message);
    }

    [Fact]
    public void GetPageCount_ZeroMethods_ReturnsZero()
    {
        Assert.Equal(0, AssemblyFullClosureAuditEmitter.GetPageCount(0, 100));
    }

    [Fact]
    public void GetPageCount_ExactPageSize_ReturnsOne()
    {
        Assert.Equal(1, AssemblyFullClosureAuditEmitter.GetPageCount(100, 100));
    }

    [Fact]
    public void GetPageCount_PartialPage_ReturnsOne()
    {
        Assert.Equal(1, AssemblyFullClosureAuditEmitter.GetPageCount(1, 100));
    }

    [Fact]
    public void GetPageCount_MultiplePages_RoundsUp()
    {
        Assert.Equal(3, AssemblyFullClosureAuditEmitter.GetPageCount(250, 100));
    }

    [Fact]
    public void BuildGeneratedFiles_WithPages_ReturnsFiles()
    {
        var files = AssemblyFullClosureAuditEmitter.BuildGeneratedFiles(
            DefaultAssembly,
            DefaultPlanKind,
            translationUnitMethodCount: 5,
            translationUnitPageSize: 10,
            translationUnitPages: new[]
            {
                new AuditTranslationUnitPageArtifact
                {
                    PageNumber = 1,
                    MethodCount = 3,
                    Path = "audit/page001.json",
                },
                new AuditTranslationUnitPageArtifact
                {
                    PageNumber = 2,
                    MethodCount = 2,
                    Path = "audit/page002.json",
                },
            },
            methodSubjectIds: new[]
            {
                "Mod.Type::Method1",
                "Mod.Type::Method2",
                "Mod.Type::Method3",
                "Mod.Type::Method4",
                "Mod.Type::Method5",
            },
            summaryRelativePath: "audit/summary.cpp");

        Assert.NotEmpty(files);
        Assert.Contains(files, f => f.RelativePath == "audit/summary.cpp");
        Assert.Contains(files, f => f.RelativePath == "audit/page001.json");
        Assert.Contains(files, f => f.RelativePath == "audit/page002.json");
        Assert.All(files, f => Assert.NotEmpty(f.Contents));
    }

    [Fact]
    public void BuildGeneratedFiles_NoPages_ReturnsOnlySummary()
    {
        var files = AssemblyFullClosureAuditEmitter.BuildGeneratedFiles(
            DefaultAssembly,
            DefaultPlanKind,
            translationUnitMethodCount: 0,
            translationUnitPageSize: 10,
            translationUnitPages: Array.Empty<AuditTranslationUnitPageArtifact>(),
            methodSubjectIds: Array.Empty<string>(),
            summaryRelativePath: "audit/summary.cpp");

        Assert.Single(files);
        Assert.Equal("audit/summary.cpp", files[0].RelativePath);
        Assert.NotEmpty(files[0].Contents);
    }
}
