// Auto-generated native-AOT entry point
// Family: family/System.Runtime.Serialization.Formatters/binder/type/style
// Assembly: System.Runtime.Serialization.Formatters
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;

public static partial class BinderTypeStyleSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::BindToName:System.Void(System.Type,System.String&,System.String&)
    public static void Subject_0()
    {
        try { default(SerializationBinder)!.BindToName(typeof(byte), out _, out _); }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::BindToType:System.Type(System.String,System.String)
    public static void Subject_1()
    {
        try { _ = default(SerializationBinder)!.BindToType("hello", "hello"); }
        catch { _exitCode = 1; }
    }

}