using System;
using System.Collections.Immutable;
using Chaos.TestFramework;

public static partial class ImmutableQueueStackSubjects
{
    // ImmutableQueue<T> uses IsEmpty (not Count)
    [Fact]
    public static void CustomEntrySubject_0()
    {
        var r = ImmutableQueue.Create<byte>();
        Assert.IsTrue(r.IsEmpty);
    }

    [Fact]
    public static void CustomEntrySubject_1()
    {
        var r = ImmutableQueue.Create<byte>((byte)42);
        Assert.IsFalse(r.IsEmpty);
    }

    [Fact]
    public static void CustomEntrySubject_2()
    {
        var r = ImmutableQueue.Create<byte>((byte)42, (byte)43);
        Assert.IsFalse(r.IsEmpty);
    }

    [Fact]
    public static void CustomEntrySubject_3()
    {
        var r = ImmutableQueue.Create<int>();
        Assert.IsTrue(r.IsEmpty);
    }

    [Fact]
    public static void CustomEntrySubject_4()
    {
        var r = ImmutableQueue.Create<int>(42);
        Assert.IsFalse(r.IsEmpty);
    }

    // ImmutableStack<T> uses IsEmpty (not Count)
    [Fact]
    public static void CustomEntrySubject_5()
    {
        var r = ImmutableStack.Create<byte>();
        Assert.IsTrue(r.IsEmpty);
    }

    [Fact]
    public static void CustomEntrySubject_6()
    {
        var r = ImmutableStack.Create<byte>((byte)42);
        Assert.IsFalse(r.IsEmpty);
    }

    [Fact]
    public static void CustomEntrySubject_7()
    {
        var r = ImmutableStack.Create<byte>((byte)42, (byte)43);
        Assert.IsFalse(r.IsEmpty);
    }

    [Fact]
    public static void CustomEntrySubject_8()
    {
        var r = ImmutableStack.Create<int>();
        Assert.IsTrue(r.IsEmpty);
    }

    [Fact]
    public static void CustomEntrySubject_9()
    {
        var r = ImmutableStack.Create<int>(42);
        Assert.IsFalse(r.IsEmpty);
    }
}
