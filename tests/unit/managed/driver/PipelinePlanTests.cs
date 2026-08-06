using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Pipeline;
using Xunit;

namespace Chaos.IL2CPP.Driver.Tests;

public sealed class PipelinePlanTests
{
    [Fact]
    public void Stages_ReturnsFiveStages()
    {
        var plan = new PipelinePlan();
        Assert.Equal(5, plan.Stages.Count);
        Assert.Contains("Loader", plan.Stages);
        Assert.Contains("SemanticWorld", plan.Stages);
        Assert.Contains("Linker", plan.Stages);
        Assert.Contains("MetadataWriter", plan.Stages);
        Assert.Contains("CodeGen", plan.Stages);
    }

    [Fact]
    public void Stages_OrderIsCorrect()
    {
        var plan = new PipelinePlan();
        Assert.Equal(["Loader", "SemanticWorld", "Linker", "MetadataWriter", "CodeGen"], plan.Stages);
    }

    [Fact]
    public void Stages_ReturnsSameInstance()
    {
        var plan = new PipelinePlan();
        Assert.Same(plan.Stages, plan.Stages);
    }

    [Fact]
    public void CanCreateMultipleInstances()
    {
        var plan1 = new PipelinePlan();
        var plan2 = new PipelinePlan();
        Assert.NotSame(plan1, plan2);
        Assert.Equal(plan1.Stages.Count, plan2.Stages.Count);
    }
}
