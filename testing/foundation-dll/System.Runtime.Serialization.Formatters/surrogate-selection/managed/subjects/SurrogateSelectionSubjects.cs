// Auto-generated native-AOT entry point
// Family: family/System.Runtime.Serialization.Formatters/surrogate/selection
// Assembly: System.Runtime.Serialization.Formatters
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;

public static partial class SurrogateSelectionSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISerializationSurrogate::GetObjectData:System.Void(System.Object,System.Runtime.Serialization.SerializationInfo,System.Runtime.Serialization.StreamingContext)
    public static void Subject_0()
    {
        try { default(ISerializationSurrogate)!.GetObjectData(42, null!, default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISerializationSurrogate::SetObjectData:System.Object(System.Object,System.Runtime.Serialization.SerializationInfo,System.Runtime.Serialization.StreamingContext,System.Runtime.Serialization.ISurrogateSelector)
    public static void Subject_1()
    {
        try { default(ISerializationSurrogate)!.SetObjectData(42, null!, default, null!); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::ChainSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector)
    public static void Subject_2()
    {
        try { default(ISurrogateSelector)!.ChainSelector(null!); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::GetNextSelector:System.Runtime.Serialization.ISurrogateSelector()
    public static void Subject_3()
    {
        try { default(ISurrogateSelector)!.GetNextSelector(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::GetSurrogate:System.Runtime.Serialization.ISerializationSurrogate(System.Type,System.Runtime.Serialization.StreamingContext,System.Runtime.Serialization.ISurrogateSelector&)
    public static void Subject_4()
    {
        try { default(ISurrogateSelector)!.GetSurrogate(typeof(byte), default, out _); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::AddSurrogate:System.Void(System.Type,System.Runtime.Serialization.StreamingContext,System.Runtime.Serialization.ISerializationSurrogate)
    public static void Subject_5()
    {
        try { default(SurrogateSelector)!.AddSurrogate(typeof(byte), default, default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [6] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::ChainSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector)
    public static void Subject_6()
    {
        try { default(SurrogateSelector)!.ChainSelector(null!); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [7] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::GetNextSelector:System.Runtime.Serialization.ISurrogateSelector()
    public static void Subject_7()
    {
        try { default(SurrogateSelector)!.GetNextSelector(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [8] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::GetSurrogate:System.Runtime.Serialization.ISerializationSurrogate(System.Type,System.Runtime.Serialization.StreamingContext,System.Runtime.Serialization.ISurrogateSelector&)
    public static void Subject_8()
    {
        try { default(SurrogateSelector)!.GetSurrogate(typeof(byte), default, out _); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [9] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::RemoveSurrogate:System.Void(System.Type,System.Runtime.Serialization.StreamingContext)
    public static void Subject_9()
    {
        try { default(SurrogateSelector)!.RemoveSurrogate(typeof(byte), default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [10] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::.ctor:System.Void()
    public static void Subject_10()
    {
        new SurrogateSelector();
    }

}