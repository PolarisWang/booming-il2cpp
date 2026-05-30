using System;
using System.Collections.Immutable;
using Chaos.TestFramework;

public static partial class ImmutableInterlockedSubjects
{
    [Fact]
    public static void CustomEntrySubject_0()
    {
        string value = "hello";
        bool result = ImmutableInterlocked.Update<string>(ref value, (s) => s + " world");
        Assert.IsTrue(result);
    }
}
