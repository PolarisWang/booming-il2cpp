// Auto-generated native-AOT entry point
// Family: family/System.Runtime.Serialization.Formatters/formatter-base
// Assembly: System.Runtime.Serialization.Formatters
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;

public static partial class FormatterBaseNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Deserialize:System.Object(System.IO.Stream)
    public static void Method0()
    {
        try { if (((Formatter.Deserialize(null!)).GetHashCode()) != ((Formatter.Deserialize(null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Binder:System.Runtime.Serialization.SerializationBinder()
    public static void Method1()
    {
        try { if ((int)(Formatter.Binder) != (int)(Formatter.Binder)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Context:System.Runtime.Serialization.StreamingContext()
    public static void Method2()
    {
        try { if ((int)(Formatter.Context) != (int)(Formatter.Context)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_SurrogateSelector:System.Runtime.Serialization.ISurrogateSelector()
    public static void Method3()
    {
        try { if ((int)(Formatter.SurrogateSelector) != (int)(Formatter.SurrogateSelector)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Serialize:System.Void(System.IO.Stream,System.Object)
    public static void Method4()
    {
        try { Formatter.Serialize(null!, 42); }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Binder:System.Void(System.Runtime.Serialization.SerializationBinder)
    public static void Method5()
    {
        try { Formatter.Binder = null!; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Context:System.Void(System.Runtime.Serialization.StreamingContext)
    public static void Method6()
    {
        try { Formatter.Context = null!; }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_SurrogateSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector)
    public static void Method7()
    {
        try { Formatter.SurrogateSelector = null!; }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Deserialize:System.Object(System.IO.Stream)
    public static void Method8()
    {
        try { if (((IFormatter.Deserialize(null!)).GetHashCode()) != ((IFormatter.Deserialize(null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Binder:System.Runtime.Serialization.SerializationBinder()
    public static void Method9()
    {
        try { if ((int)(IFormatter.Binder) != (int)(IFormatter.Binder)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Context:System.Runtime.Serialization.StreamingContext()
    public static void Method10()
    {
        try { if ((int)(IFormatter.Context) != (int)(IFormatter.Context)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_SurrogateSelector:System.Runtime.Serialization.ISurrogateSelector()
    public static void Method11()
    {
        try { if ((int)(IFormatter.SurrogateSelector) != (int)(IFormatter.SurrogateSelector)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Serialize:System.Void(System.IO.Stream,System.Object)
    public static void Method12()
    {
        try { IFormatter.Serialize(null!, 42); }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Binder:System.Void(System.Runtime.Serialization.SerializationBinder)
    public static void Method13()
    {
        try { IFormatter.Binder = null!; }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Context:System.Void(System.Runtime.Serialization.StreamingContext)
    public static void Method14()
    {
        try { IFormatter.Context = null!; }
        catch { _exitCode = 1; }
    }

    // [15] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_SurrogateSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector)
    public static void Method15()
    {
        try { IFormatter.SurrogateSelector = null!; }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
                case 7: Method7(); break;
                case 8: Method8(); break;
                case 9: Method9(); break;
                case 10: Method10(); break;
                case 11: Method11(); break;
                case 12: Method12(); break;
                case 13: Method13(); break;
                case 14: Method14(); break;
                case 15: Method15(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}