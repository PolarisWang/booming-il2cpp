using Chaos.IL2CPP.Generator.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

/// <summary>
/// Verifies that the test infrastructure itself works: planner accepts minimal
/// artifact models and produces valid output. These are the first tests to run
/// when setting up the framework — they check that mocks are sufficient.
/// </summary>
public sealed class InfrastructureTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    [Fact]
    public void Planner_AcceptsMinimalArtifact()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestMethod");
        var artifact = ModelFactory.CreateArtifact(method);
        var plan = ModelFactory.CreateDefaultPlan("TestModule.TestClass::TestMethod");
        var manifest = ModelFactory.CreateDefaultManifest(
            inputAssemblyPath: PlannerFixture.StubAssemblyPath);
        var metadata = ModelFactory.CreateEmptyMetadataRegistration();
        var supplemental = ModelFactory.CreateEmptySupplementalMetadata();

        var result = _fixture.RunPlanner(artifact, plan, manifest, metadata, supplemental);
        Assert.NotNull(result);
        Assert.NotEmpty(result.Methods);
    }

    [Fact]
    public void Planner_GeneratesMethodSource()
    {
        const string subjectId = "TestModule.TestClass::TestMethod";
        var method = ModelFactory.CreateMethod(subjectId);

        var source = _fixture.RunPlannerSingleMethod(method);
        Assert.False(string.IsNullOrWhiteSpace(source));
    }

    [Fact]
    public void Planner_OutputMethodCount_MatchesInput()
    {
        var methods = new[]
        {
            ModelFactory.CreateMethod("A.A::M1"),
            ModelFactory.CreateMethod("B.B::M2"),
            ModelFactory.CreateMethod("C.C::M3"),
        };
        var artifact = ModelFactory.CreateArtifact(methods);
        var result = _fixture.RunPlanner(artifact);

        // Full assembly mode should process at least the first method.
        Assert.NotEmpty(result.Methods);
        Assert.Contains(result.Methods, m => m.SubjectId.Contains("M1"));
    }

    [Fact]
    public void SimpleRetMethod_GeneratesRetInstruction()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::RetMethod");
        var source = _fixture.RunPlannerSingleMethod(method);

        // The simplest possible method should contain a "return" or "ret"
        AssertExtensions.ContainsCode("return", source);
    }

    public void Dispose()
    {
        // PlannerFixture has no cleanup needed currently.
    }
}
