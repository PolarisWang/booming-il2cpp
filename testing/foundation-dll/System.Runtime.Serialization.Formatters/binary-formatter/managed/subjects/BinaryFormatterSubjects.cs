// Auto-generated native-AOT entry point
// Family: family/System.Runtime.Serialization.Formatters/binary/formatter
// Assembly: System.Runtime.Serialization.Formatters
// Variant: subjects

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Serialization.Formatters.Binary;

public static partial class BinaryFormatterSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::Deserialize:System.Object(System.IO.Stream)
    public static void Subject_0()
    {
        try { if (((default(BinaryFormatter)!.Deserialize(new MemoryStream())).GetHashCode()) != ((default(BinaryFormatter)!.Deserialize(new MemoryStream())).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_AssemblyFormat:System.Runtime.Serialization.Formatters.FormatterAssemblyStyle()
    public static void Subject_1()
    {
        try { if (((default(BinaryFormatter)!.AssemblyFormat).GetHashCode()) != ((default(BinaryFormatter)!.AssemblyFormat).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_Binder:System.Runtime.Serialization.SerializationBinder()
    public static void Subject_2()
    {
        try { if (((default(BinaryFormatter)!.Binder).GetHashCode()) != ((default(BinaryFormatter)!.Binder).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_Context:System.Runtime.Serialization.StreamingContext()
    public static void Subject_3()
    {
        try { if (((default(BinaryFormatter)!.Context).GetHashCode()) != ((default(BinaryFormatter)!.Context).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_FilterLevel:System.Runtime.Serialization.Formatters.TypeFilterLevel()
    public static void Subject_4()
    {
        try { if (((default(BinaryFormatter)!.FilterLevel).GetHashCode()) != ((default(BinaryFormatter)!.FilterLevel).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_SurrogateSelector:System.Runtime.Serialization.ISurrogateSelector()
    public static void Subject_5()
    {
        try { if (((default(BinaryFormatter)!.SurrogateSelector).GetHashCode()) != ((default(BinaryFormatter)!.SurrogateSelector).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_TypeFormat:System.Runtime.Serialization.Formatters.FormatterTypeStyle()
    public static void Subject_6()
    {
        try { if (((default(BinaryFormatter)!.TypeFormat).GetHashCode()) != ((default(BinaryFormatter)!.TypeFormat).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::Serialize:System.Void(System.IO.Stream,System.Object)
    public static void Subject_7()
    {
        try { default(BinaryFormatter)!.Serialize(new MemoryStream(), 42); }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_AssemblyFormat:System.Void(System.Runtime.Serialization.Formatters.FormatterAssemblyStyle)
    public static void Subject_8()
    {
        try { default(BinaryFormatter)!.AssemblyFormat = default; }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_Binder:System.Void(System.Runtime.Serialization.SerializationBinder)
    public static void Subject_9()
    {
        try { default(BinaryFormatter)!.Binder = null!; }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_Context:System.Void(System.Runtime.Serialization.StreamingContext)
    public static void Subject_10()
    {
        try { default(BinaryFormatter)!.Context = default; }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_FilterLevel:System.Void(System.Runtime.Serialization.Formatters.TypeFilterLevel)
    public static void Subject_11()
    {
        try { default(BinaryFormatter)!.FilterLevel = default; }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_SurrogateSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector)
    public static void Subject_12()
    {
        try { default(BinaryFormatter)!.SurrogateSelector = null!; }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_TypeFormat:System.Void(System.Runtime.Serialization.Formatters.FormatterTypeStyle)
    public static void Subject_13()
    {
        try { default(BinaryFormatter)!.TypeFormat = default; }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::.ctor:System.Void()
    public static void Subject_14()
    {
        try { new BinaryFormatter(); }
        catch { _exitCode = 1; }
    }

    // [15] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::.ctor:System.Void(System.Runtime.Serialization.ISurrogateSelector,System.Runtime.Serialization.StreamingContext)
    public static void Subject_15()
    {
        try { new BinaryFormatter(null!, default); }
        catch { _exitCode = 1; }
    }

}