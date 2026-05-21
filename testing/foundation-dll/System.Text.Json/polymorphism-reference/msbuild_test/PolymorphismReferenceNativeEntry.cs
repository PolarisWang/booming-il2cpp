// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/polymorphism-reference
// Assembly: System.Text.Json
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json.Serialization;

public static partial class PolymorphismReferenceNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()
    public static void Method0()
    {
        try { if ((int)(ReferenceHandler.CreateResolver()) != (int)(ReferenceHandler.CreateResolver())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_IgnoreCycles:System.Text.Json.Serialization.ReferenceHandler()
    public static void Method1()
    {
        try { if ((int)(ReferenceHandler.IgnoreCycles) != (int)(ReferenceHandler.IgnoreCycles)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Preserve:System.Text.Json.Serialization.ReferenceHandler()
    public static void Method2()
    {
        try { if ((int)(ReferenceHandler.Preserve) != (int)(ReferenceHandler.Preserve)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()
    public static void Method3()
    {
        try { if ((int)(ReferenceHandler.CreateResolver()) != (int)(ReferenceHandler.CreateResolver())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::.ctor:System.Void()
    public static void Method4()
    {
        try { new ReferenceHandler(); }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::AddReference:System.Void(System.String,System.Object)
    public static void Method5()
    {
        try { ReferenceResolver.AddReference("hello", 42); }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::GetReference:System.String(System.Object,System.Boolean&)
    public static void Method6()
    {
    var refLocal_1 = true;
        try { if (((ReferenceResolver.GetReference(42, ref refLocal_1)).Length) != ((ReferenceResolver.GetReference(42, ref refLocal_1)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ResolveReference:System.Object(System.String)
    public static void Method7()
    {
        try { if (((ReferenceResolver.ResolveReference("hello")).GetHashCode()) != ((ReferenceResolver.ResolveReference("hello")).GetHashCode())) _exitCode = 1; }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}