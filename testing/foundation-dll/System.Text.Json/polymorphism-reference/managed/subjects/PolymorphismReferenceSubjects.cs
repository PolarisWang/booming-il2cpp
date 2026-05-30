// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/polymorphism/reference
// Assembly: System.Text.Json
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json.Serialization;
using Chaos.TestFramework;

public static partial class PolymorphismReferenceSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()
    public static void CustomEntrySubject_0()
    {
        try { var r = ReferenceHandler.Preserve.CreateResolver(); Assert.IsNotNull(r); }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_IgnoreCycles:System.Text.Json.Serialization.ReferenceHandler()
    public static void CustomEntrySubject_1()
    {
        try { var h = ReferenceHandler.IgnoreCycles; Assert.IsNotNull(h); }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Preserve:System.Text.Json.Serialization.ReferenceHandler()
    public static void CustomEntrySubject_2()
    {
        try { var h = ReferenceHandler.Preserve; Assert.IsNotNull(h); }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()
    public static void CustomEntrySubject_3()
    {
        try { var r = ReferenceHandler.Preserve.CreateResolver(); Assert.IsTrue(r != null); }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::.ctor:System.Void()
    public static void CustomEntrySubject_4()
    {
        try { var h = ReferenceHandler.Preserve; Assert.IsNotNull(h); }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::AddReference:System.Void(System.String,System.Object)
    public static void CustomEntrySubject_5()
    {
        try { var r = ReferenceHandler.Preserve.CreateResolver(); r.AddReference("key", new object()); Assert.IsNotNull(r.ResolveReference("key")); }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::GetReference:System.String(System.Object,System.Boolean&)
    public static void CustomEntrySubject_6()
    {
        try { var r = ReferenceHandler.Preserve.CreateResolver(); r.AddReference("key", new object()); bool found; var refStr = r.GetReference(new object(), out found); Assert.IsNotNull(refStr); }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ResolveReference:System.Object(System.String)
    public static void CustomEntrySubject_7()
    {
        try { var r = ReferenceHandler.Preserve.CreateResolver(); var obj = r.ResolveReference("key"); Assert.IsNull(obj); }
        catch { _exitCode = 1; }
    }

}
