using Xunit;

namespace Chaos.IL2CPP.Driver.Tests;

public sealed class ConvertToCppHandlerTests
{
    [Fact]
    public void Run_NoAssemblies_ReturnsOne()
    {
        var result = ConvertToCppHandler.Run(["--output", "out"]);
        Assert.Equal(1, result);
    }

    [Fact]
    public void Run_NoArgs_ReturnsOne()
    {
        var result = ConvertToCppHandler.Run([]);
        Assert.Equal(1, result);
    }

    [Fact]
    public void Run_OutputOnly_ReturnsOne()
    {
        var result = ConvertToCppHandler.Run(["--output", "out"]);
        Assert.Equal(1, result);
    }
}
