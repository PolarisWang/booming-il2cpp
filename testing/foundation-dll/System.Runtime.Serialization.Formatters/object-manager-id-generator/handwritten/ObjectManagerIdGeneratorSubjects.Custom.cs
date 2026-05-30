// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class ObjectManagerIdGeneratorSubjects
{
    // [7] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordArrayElementFixup:System.Void(System.Int64,System.Int32,System.Int64)
    [Fact]
    public static void CustomEntrySubject_7()
    {
        try {
            default(ObjectManager)!.RecordArrayElementFixup(42L, 42, 42L);
        }
        catch { }
    }

    // [8] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordArrayElementFixup:System.Void(System.Int64,System.Int32{},System.Int64)
    [Fact]
    public static void CustomEntrySubject_8()
    {
        try {
            default(ObjectManager)!.RecordArrayElementFixup(42L, default, 42L);
        }
        catch { }
    }

}
