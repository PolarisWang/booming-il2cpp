// Auto-generated native-AOT entry point
// Family: family/System.Runtime.Serialization.Formatters/binder-type-style
// Assembly: System.Runtime.Serialization.Formatters
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;

public static partial class BinderTypeStyleNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::BindToName:System.Void(System.Type,System.String&,System.String&)
    public static void Method0()
    {
    var refLocal_1 = "hello";
    var refLocal_2 = "hello";
        try { SerializationBinder.BindToName(typeof(byte), ref refLocal_1, ref refLocal_2); }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::BindToType:System.Type(System.String,System.String)
    public static void Method1()
    {
        try { if (((SerializationBinder.BindToType("hello", "hello")).GetHashCode()) != ((SerializationBinder.BindToType("hello", "hello")).GetHashCode())) _exitCode = 1; }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}