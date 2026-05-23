using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Driver.Tests;

public sealed class ConvertToCppConfigTests
{
    [Fact]
    public void Parse_SingleAssembly()
    {
        var config = ConvertToCppConfig.Parse([
            "--assembly", "input.dll",
            "--output", "out_dir",
        ]);

        Assert.Single(config.AssemblyPaths);
        Assert.Equal("input.dll", config.AssemblyPaths[0]);
        Assert.Equal("out_dir", config.OutputDir);
    }

    [Fact]
    public void Parse_MultipleAssemblies()
    {
        var config = ConvertToCppConfig.Parse([
            "--assembly", "a.dll",
            "--assembly", "b.dll",
            "--assembly", "c.dll",
            "--output", "out",
        ]);

        Assert.Equal(3, config.AssemblyPaths.Count);
        Assert.Equal("a.dll", config.AssemblyPaths[0]);
        Assert.Equal("b.dll", config.AssemblyPaths[1]);
        Assert.Equal("c.dll", config.AssemblyPaths[2]);
    }

    [Fact]
    public void Parse_ShortAliases()
    {
        var config = ConvertToCppConfig.Parse([
            "-a", "input.dll",
            "-o", "out_dir",
        ]);

        Assert.Single(config.AssemblyPaths);
        Assert.Equal("input.dll", config.AssemblyPaths[0]);
        Assert.Equal("out_dir", config.OutputDir);
    }

    [Fact]
    public void Parse_WithAllOptions()
    {
        var config = ConvertToCppConfig.Parse([
            "--assembly", "input.dll",
            "--output", "out",
            "--entry-point", "MyAsm/Type::Method:System.Void()",
            "--full-closure",
            "--mode", "jit",
            "--verbose",
        ]);

        Assert.Single(config.AssemblyPaths);
        Assert.Equal("out", config.OutputDir);
        Assert.Equal("MyAsm/Type::Method:System.Void()", config.EntryPoint);
        Assert.True(config.FullClosure);
        Assert.True(config.Verbose);
        Assert.Equal(CodegenMode.Jit, config.Mode);
    }

    [Fact]
    public void Parse_DefaultOutput()
    {
        var config = ConvertToCppConfig.Parse(["--assembly", "input.dll"]);

        Assert.Single(config.AssemblyPaths);
        Assert.Equal("il2cpp_dist", config.OutputDir);
    }

    [Fact]
    public void Parse_AssemblyDirs()
    {
        var config = ConvertToCppConfig.Parse([
            "--assembly", "input.dll",
            "--output", "out",
            "--assembly-dir", "dir1",
            "--assembly-dir", "dir2",
        ]);

        Assert.Equal(2, config.AssemblyDirs.Count);
        Assert.Contains("dir1", config.AssemblyDirs);
        Assert.Contains("dir2", config.AssemblyDirs);
    }
}
