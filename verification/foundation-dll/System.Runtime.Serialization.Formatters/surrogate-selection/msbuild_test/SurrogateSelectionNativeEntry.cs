// Auto-generated native-AOT entry point
// Family: family/System.Runtime.Serialization.Formatters/surrogate-selection
// Assembly: System.Runtime.Serialization.Formatters
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;

public static partial class SurrogateSelectionNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISerializationSurrogate::GetObjectData:System.Void(System.Object,System.Runtime.Serialization.SerializationInfo,System.Runtime.Serialization.StreamingContext)
    public static void Method0()
    {
        try { ISerializationSurrogate.GetObjectData(42, null!, null!); }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISerializationSurrogate::SetObjectData:System.Object(System.Object,System.Runtime.Serialization.SerializationInfo,System.Runtime.Serialization.StreamingContext,System.Runtime.Serialization.ISurrogateSelector)
    public static void Method1()
    {
        try { if (((ISerializationSurrogate.SetObjectData(42, null!, null!, null!)).GetHashCode()) != ((ISerializationSurrogate.SetObjectData(42, null!, null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::ChainSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector)
    public static void Method2()
    {
        try { ISurrogateSelector.ChainSelector(null!); }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::GetNextSelector:System.Runtime.Serialization.ISurrogateSelector()
    public static void Method3()
    {
        try { if ((int)(ISurrogateSelector.GetNextSelector()) != (int)(ISurrogateSelector.GetNextSelector())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::GetSurrogate:System.Runtime.Serialization.ISerializationSurrogate(System.Type,System.Runtime.Serialization.StreamingContext,System.Runtime.Serialization.ISurrogateSelector&)
    public static void Method4()
    {
    var refLocal_2 = null!;
        try { if ((int)(ISurrogateSelector.GetSurrogate(typeof(byte), null!, ref refLocal_2)) != (int)(ISurrogateSelector.GetSurrogate(typeof(byte), null!, ref refLocal_2))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::AddSurrogate:System.Void(System.Type,System.Runtime.Serialization.StreamingContext,System.Runtime.Serialization.ISerializationSurrogate)
    public static void Method5()
    {
        try { SurrogateSelector.AddSurrogate(typeof(byte), null!, null!); }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::ChainSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector)
    public static void Method6()
    {
        try { SurrogateSelector.ChainSelector(null!); }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::GetNextSelector:System.Runtime.Serialization.ISurrogateSelector()
    public static void Method7()
    {
        try { if ((int)(SurrogateSelector.GetNextSelector()) != (int)(SurrogateSelector.GetNextSelector())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::GetSurrogate:System.Runtime.Serialization.ISerializationSurrogate(System.Type,System.Runtime.Serialization.StreamingContext,System.Runtime.Serialization.ISurrogateSelector&)
    public static void Method8()
    {
    var refLocal_2 = null!;
        try { if ((int)(SurrogateSelector.GetSurrogate(typeof(byte), null!, ref refLocal_2)) != (int)(SurrogateSelector.GetSurrogate(typeof(byte), null!, ref refLocal_2))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::RemoveSurrogate:System.Void(System.Type,System.Runtime.Serialization.StreamingContext)
    public static void Method9()
    {
        try { SurrogateSelector.RemoveSurrogate(typeof(byte), null!); }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::.ctor:System.Void()
    public static void Method10()
    {
        try { new SurrogateSelector(); }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}