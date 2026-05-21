// Auto-generated native-AOT entry point
// Family: family/System.Runtime.Serialization.Formatters/formatter/base
// Assembly: System.Runtime.Serialization.Formatters
// Variant: subjects

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;

public static partial class FormatterBaseSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Deserialize:System.Object(System.IO.Stream)
    public static void Subject_0()
    {
        try { if (((default(Formatter)!.Deserialize(new MemoryStream())).GetHashCode()) != ((default(Formatter)!.Deserialize(new MemoryStream())).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Binder:System.Runtime.Serialization.SerializationBinder()
    public static void Subject_1()
    {
        try { if (((default(Formatter)!.Binder).GetHashCode()) != ((default(Formatter)!.Binder).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Context:System.Runtime.Serialization.StreamingContext()
    public static void Subject_2()
    {
        try { if (((default(Formatter)!.Context).GetHashCode()) != ((default(Formatter)!.Context).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_SurrogateSelector:System.Runtime.Serialization.ISurrogateSelector()
    public static void Subject_3()
    {
        try { if (((default(Formatter)!.SurrogateSelector).GetHashCode()) != ((default(Formatter)!.SurrogateSelector).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Serialize:System.Void(System.IO.Stream,System.Object)
    public static void Subject_4()
    {
        try { default(Formatter)!.Serialize(new MemoryStream(), 42); }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Binder:System.Void(System.Runtime.Serialization.SerializationBinder)
    public static void Subject_5()
    {
        try { default(Formatter)!.Binder = null!; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Context:System.Void(System.Runtime.Serialization.StreamingContext)
    public static void Subject_6()
    {
        try { default(Formatter)!.Context = default; }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_SurrogateSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector)
    public static void Subject_7()
    {
        try { default(Formatter)!.SurrogateSelector = null!; }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Deserialize:System.Object(System.IO.Stream)
    public static void Subject_8()
    {
        try { if (((default(IFormatter)!.Deserialize(new MemoryStream())).GetHashCode()) != ((default(IFormatter)!.Deserialize(new MemoryStream())).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Binder:System.Runtime.Serialization.SerializationBinder()
    public static void Subject_9()
    {
        try { if (((default(IFormatter)!.Binder).GetHashCode()) != ((default(IFormatter)!.Binder).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Context:System.Runtime.Serialization.StreamingContext()
    public static void Subject_10()
    {
        try { if (((default(IFormatter)!.Context).GetHashCode()) != ((default(IFormatter)!.Context).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_SurrogateSelector:System.Runtime.Serialization.ISurrogateSelector()
    public static void Subject_11()
    {
        try { if (((default(IFormatter)!.SurrogateSelector).GetHashCode()) != ((default(IFormatter)!.SurrogateSelector).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Serialize:System.Void(System.IO.Stream,System.Object)
    public static void Subject_12()
    {
        try { default(IFormatter)!.Serialize(new MemoryStream(), 42); }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Binder:System.Void(System.Runtime.Serialization.SerializationBinder)
    public static void Subject_13()
    {
        try { default(IFormatter)!.Binder = null!; }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Context:System.Void(System.Runtime.Serialization.StreamingContext)
    public static void Subject_14()
    {
        try { default(IFormatter)!.Context = default; }
        catch { _exitCode = 1; }
    }

    // [15] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_SurrogateSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector)
    public static void Subject_15()
    {
        try { default(IFormatter)!.SurrogateSelector = null!; }
        catch { _exitCode = 1; }
    }

}