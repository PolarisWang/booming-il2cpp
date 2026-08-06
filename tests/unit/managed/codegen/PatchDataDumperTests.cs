using System.Reflection;
using System.Text;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

/// <summary>
/// Tests for PatchDataDumper static helper methods.
/// Tests ExtractMethodName and ReadString via reflection.
/// </summary>
public sealed class PatchDataDumperTests
{
    private static readonly Type s_type = typeof(Chaos.IL2CPP.Generator.PatchDataDumper);
    private const BindingFlags s_flags = BindingFlags.NonPublic | BindingFlags.Static;

    private static T? Invoke<T>(string name, Type[] paramTypes, object[] args)
    {
        var method = s_type.GetMethod(name, s_flags, paramTypes)
            ?? throw new InvalidOperationException($"Method '{name}' with specified params not found");
        return (T?)method.Invoke(null, args);
    }

    // ── ExtractMethodName ──────────────────────────────────────────────

    [Theory]
    [InlineData(@"""subjectId"":""Test/Module::Method:Return()""}", "Test/Module::Method:Return()")]
    [InlineData(@"""subjectId"":""Simple""}", "Simple")]
    [InlineData(@"{""op"":""ret""}", @"{""op"":""ret""}")] // no match → returns raw snippet
    [InlineData("", "")]
    public void ExtractMethodName_VariousInputs(string json, string expected)
    {
        var result = Invoke<string>("ExtractMethodName", new[] { typeof(string) }, new object[] { json });
        Assert.Equal(expected, result);
    }

    [Fact]
    public void ExtractMethodName_LongJsonSnippet_TruncatesTo80Chars()
    {
        var input = new string('x', 200);
        var result = Invoke<string>("ExtractMethodName", new[] { typeof(string) }, new object[] { input });
        Assert.Equal(83, result!.Length); // 80 chars + "..."
        Assert.EndsWith("...", result);
    }

    [Fact]
    public void ExtractMethodName_SubjectIdAtEndOfSnippet_ExtractsCorrectly()
    {
        var snippet = @"{""op"":""add"",""subjectId"":""My/Type::Method:Ret()""}";
        Assert.Equal("My/Type::Method:Ret()", Invoke<string>("ExtractMethodName", new[] { typeof(string) }, new object[] { snippet }));
    }

    // ── ReadString ─────────────────────────────────────────────────────

    [Fact]
    public void ReadString_ValidOffset_ReturnsString()
    {
        // strOffset=0 returns empty (null check). Non-zero offsets work.
        var bytes = Encoding.UTF8.GetBytes("\0hello\0world\0");
        Assert.Equal("hello", InvokeString("ReadString", bytes, 0u, 1u));
        Assert.Equal("world", InvokeString("ReadString", bytes, 0u, 7u));
    }

    [Fact]
    public void ReadString_ZeroOffset_ReturnsEmpty()
    {
        Assert.Equal("", InvokeString("ReadString", Encoding.UTF8.GetBytes("hello\0"), 0u, 0u));
    }

    [Fact]
    public void ReadString_OffsetBeyondLength_ReturnsEmpty()
    {
        Assert.Equal("", InvokeString("ReadString", Encoding.UTF8.GetBytes("hello"), 0u, 99u));
    }

    [Fact]
    public void ReadString_WithHeapOffset_UsesCorrectBase()
    {
        // strOffset=0 is treated as null by ReadString (returns "").
        // With heapOffset=5 and strOffset=1, reads at pos 6.
        var bytes = Encoding.UTF8.GetBytes("XXXXX\0hello\0");
        Assert.Equal("hello", InvokeString("ReadString", bytes, 5u, 1u));
    }

    [Fact]
    public void ReadString_WithBothOffsets()
    {
        // strOffset is relative to heapOffset
        var bytes = Encoding.UTF8.GetBytes("prefix\0target\0");
        // "target" starts at heapOffset(0) + strOffset(7) = 7
        Assert.Equal("target", InvokeString("ReadString", bytes, 0u, 7u));
    }

    private static string InvokeString(string methodName, byte[] bytes, uint heapOffset, uint strOffset)
    {
        var paramTypes = new[] { typeof(byte[]), typeof(uint), typeof(uint) };
        return (string)Invoke<string>(methodName, paramTypes, new object[] { bytes, heapOffset, strOffset })!;
    }
}
