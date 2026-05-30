// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class SurrogateSelectionSubjects
{
    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISerializationSurrogate::GetObjectData:System.Void(System.Object,System.Runtime.Serialization.SerializationInfo,System.Runtime.Serialization.StreamingContext)
    public static void CustomEntrySubject_0()
    {        try { default(ISerializationSurrogate)!.GetObjectData(null!, null!, default); } catch { _exitCode = 1; }    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISerializationSurrogate::SetObjectData:System.Object(System.Object,System.Runtime.Serialization.SerializationInfo,System.Runtime.Serialization.StreamingContext,System.Runtime.Serialization.ISurrogateSelector)
    public static void CustomEntrySubject_1()
    {        try { _ = default(ISerializationSurrogate)!.SetObjectData(null!, null!, default, null!); } catch { _exitCode = 1; }    }

    // [2] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::ChainSelector:System.Void(System.Runtime.Serialization.ISurrogateSelector)
    public static void CustomEntrySubject_2()
    {        try { default(ISurrogateSelector)!.ChainSelector(null!); } catch { _exitCode = 1; }    }

    // [3] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::GetNextSelector:System.Runtime.Serialization.ISurrogateSelector()
    public static void CustomEntrySubject_3()
    {        try { _ = default(ISurrogateSelector)!.GetNextSelector(); } catch { _exitCode = 1; }    }

    // [4] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::GetSurrogate:System.Runtime.Serialization.ISerializationSurrogate(System.Type,System.Runtime.Serialization.StreamingContext,System.Runtime.Serialization.ISurrogateSelector&)
    public static void CustomEntrySubject_4()
    {        try { _ = default(ISurrogateSelector)!.GetSurrogate(typeof(byte), default, out null!); } catch { _exitCode = 1; }    }

}
