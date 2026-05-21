using Chaos.IL2CPP.CodeGen;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests;

public sealed class ScribanTemplateRendererTests
{
    [Fact]
    public void Tab_ReturnsTabCharacter()
    {
        Assert.Equal("\t", ScribanTemplateRenderer.Tab);
    }

    [Fact]
    public void Indentation_ZeroDepth_ReturnsEmpty()
    {
        Assert.Equal("", ScribanTemplateRenderer.Indentation(0));
    }

    [Fact]
    public void Indentation_OneDepth_ReturnsOneTab()
    {
        Assert.Equal("\t", ScribanTemplateRenderer.Indentation(1));
    }

    [Fact]
    public void Indentation_ThreeDepth_ReturnsThreeTabs()
    {
        Assert.Equal("\t\t\t", ScribanTemplateRenderer.Indentation(3));
    }

    [Fact]
    public void NormalizeIndentation_NoSpaces_ReturnsSame()
    {
        Assert.Equal("hello", ScribanTemplateRenderer.NormalizeIndentation("hello"));
    }

    [Fact]
    public void NormalizeIndentation_FourSpaces_ReplacesWithTab()
    {
        Assert.Equal("\thello", ScribanTemplateRenderer.NormalizeIndentation("    hello"));
    }

    [Fact]
    public void NormalizeIndentation_EightSpaces_ReplacesWithTwoTabs()
    {
        Assert.Equal("\t\thello", ScribanTemplateRenderer.NormalizeIndentation("        hello"));
    }

    [Fact]
    public void NormalizeIndentation_TabAlready_RemainsTab()
    {
        Assert.Equal("\thello", ScribanTemplateRenderer.NormalizeIndentation("\thello"));
    }

    [Fact]
    public void NormalizeIndentation_ThreeSpacesOnly_NotEnoughForOneTab()
    {
        Assert.Equal("   hello", ScribanTemplateRenderer.NormalizeIndentation("   hello"));
    }
}
