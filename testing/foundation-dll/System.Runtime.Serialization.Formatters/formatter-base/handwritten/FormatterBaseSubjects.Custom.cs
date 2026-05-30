// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class FormatterBaseSubjects
{
    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Deserialize:System.Object(System.IO.Stream)
    [Fact]
    public static void CustomEntrySubject_0()
    {        try { _ = default(Formatter)!.Deserialize(new MemoryStream()); } catch { }    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Binder:System.Runtime.Serialization.SerializationBinder()
    [Fact]
    public static void CustomEntrySubject_1()
    {        try { _ = default(Formatter)!.Binder; } catch { }    }

    // [2] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Context:System.Runtime.Serialization.StreamingContext()
    [Fact]
    public static void CustomEntrySubject_2()
    {        try { _ = default(Formatter)!.Context; } catch { }    }

    // [3] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_SurrogateSelector:System.Runtime.Serialization.ISurrogateSelector()
    [Fact]
    public static void CustomEntrySubject_3()
    {        try { _ = default(Formatter)!.SurrogateSelector; } catch { }    }

    // [4] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Serialize:System.Void(System.IO.Stream,System.Object)
    [Fact]
    public static void CustomEntrySubject_4()
    {        try { default(Formatter)!.Serialize(new MemoryStream(), null!); } catch { }    }

    // [5] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Binder:System.Void(System.Runtime.Serialization.SerializationBinder)
    [Fact]
    public static void CustomEntrySubject_5()
    {        try { default(Formatter)!.Binder = null!; } catch { }    }

    // [6] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Context:System.Void(System.Runtime.Serialization.StreamingContext)
    [Fact]
    public static void CustomEntrySubject_6()
    {        try { default(Formatter)!.Context = default; } catch { }    }

    // [7] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_SurrogateSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector)
    [Fact]
    public static void CustomEntrySubject_7()
    {        try { default(Formatter)!.SurrogateSelector = null!; } catch { }    }

    // [8] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Deserialize:System.Object(System.IO.Stream)
    [Fact]
    public static void CustomEntrySubject_8()
    {        try { _ = default(IFormatter)!.Deserialize(new MemoryStream()); } catch { }    }

    // [9] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Binder:System.Runtime.Serialization.SerializationBinder()
    [Fact]
    public static void CustomEntrySubject_9()
    {        try { _ = default(IFormatter)!.Binder; } catch { }    }

    // [10] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Context:System.Runtime.Serialization.StreamingContext()
    [Fact]
    public static void CustomEntrySubject_10()
    {        try { _ = default(IFormatter)!.Context; } catch { }    }

    // [11] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_SurrogateSelector:System.Runtime.Serialization.ISurrogateSelector()
    [Fact]
    public static void CustomEntrySubject_11()
    {        try { _ = default(IFormatter)!.SurrogateSelector; } catch { }    }

    // [12] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Serialize:System.Void(System.IO.Stream,System.Object)
    [Fact]
    public static void CustomEntrySubject_12()
    {        try { default(IFormatter)!.Serialize(new MemoryStream(), null!); } catch { }    }

    // [13] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Binder:System.Void(System.Runtime.Serialization.SerializationBinder)
    [Fact]
    public static void CustomEntrySubject_13()
    {        try { default(IFormatter)!.Binder = null!; } catch { }    }

    // [14] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Context:System.Void(System.Runtime.Serialization.StreamingContext)
    [Fact]
    public static void CustomEntrySubject_14()
    {        try { default(IFormatter)!.Context = default; } catch { }    }

    // [15] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_SurrogateSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector)
    [Fact]
    public static void CustomEntrySubject_15()
    {        try { default(IFormatter)!.SurrogateSelector = null!; } catch { }    }

}
