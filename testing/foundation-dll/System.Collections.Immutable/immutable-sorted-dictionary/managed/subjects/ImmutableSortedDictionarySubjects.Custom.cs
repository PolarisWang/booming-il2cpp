using System;
using System.Collections.Immutable;
using Chaos.TestFramework;

public static partial class ImmutableSortedDictionarySubjects
{
    [Fact]
    public static void CustomEntrySubject_0()
    {
        var r = ImmutableSortedDictionary.Create<string, int>();
        Assert.AreEqual(0, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_1()
    {
        var r = ImmutableSortedDictionary.Create<byte, byte>();
        Assert.AreEqual(0, r.Count);
    }
}
