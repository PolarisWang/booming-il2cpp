using Chaos.IL2CPP.Driver;
using Xunit;

namespace Chaos.IL2CPP.Driver.Tests;

public sealed class DriverEntryTests
{
    [Fact]
    public void Name_ReturnsDriver()
    {
        Assert.Equal("Chaos.IL2CPP.Driver", new DriverEntry().Name);
    }

    [Fact]
    public void Main_NoArgs_ReturnsOne()
    {
        var result = DriverEntry.Main([]);
        Assert.Equal(1, result);
    }

    [Fact]
    public void Main_Help_ReturnsOne()
    {
        var result = DriverEntry.Main(["--help"]);
        Assert.Equal(1, result);
    }

    [Fact]
    public void Main_ConvertToCpp_NoAssembly_ReturnsOne()
    {
        var result = DriverEntry.Main(["convert-to-cpp"]);
        Assert.Equal(1, result);
    }

    [Fact]
    public void Main_LegacyConvert_TooFewArgs_ReturnsOne()
    {
        var result = DriverEntry.Main(["input.dll"]);
        Assert.Equal(1, result);
    }

    [Fact]
    public void Main_LegacyEmitNativeReference_WrongArgCount_ReturnsOne()
    {
        var result = DriverEntry.Main(["emit-native-reference", "only-one-arg"]);
        Assert.Equal(1, result);
    }

    [Fact]
    public void Main_UnknownCommand_ShowsHelpAndReturnsOne()
    {
        var result = DriverEntry.Main(["invalid-command"]);
        Assert.Equal(1, result);
    }

    [Fact]
    public void Main_Build_NoArg_ReturnsOne()
    {
        var result = DriverEntry.Main(["build"]);
        Assert.Equal(1, result);
    }

    [Fact]
    public void Main_Build_Help_ReturnsZero()
    {
        var result = DriverEntry.Main(["build", "--help"]);
        Assert.Equal(0, result);
    }

    [Fact]
    public void Main_Build_UnknownFlag_ReturnsOne()
    {
        var result = DriverEntry.Main(["build", "--invalid"]);
        Assert.Equal(1, result);
    }

    [Fact]
    public void Main_Build_NonExistentDir_ReturnsOne()
    {
        var result = DriverEntry.Main(["build", "C:\\__nonexistent_build_dir__"]);
        Assert.Equal(1, result);
    }

    [Fact]
    public void Main_Convert_NoArg_ReturnsOne()
    {
        var result = DriverEntry.Main(["convert"]);
        Assert.Equal(1, result);
    }
}
