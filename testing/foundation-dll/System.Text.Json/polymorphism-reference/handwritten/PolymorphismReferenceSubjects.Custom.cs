// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class PolymorphismReferenceSubjects
{
    // [0] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()
    public static void CustomEntrySubject_0()
    {
        try {
            Assert.IsNotNull(default(ReferenceHandler)!.CreateResolver());
        }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()
    public static void CustomEntrySubject_3()
    {
        try {
            Assert.IsNotNull(default(ReferenceHandler<byte>)!.CreateResolver());
        }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::.ctor:System.Void()
    public static void CustomEntrySubject_4()
    {
        try {
            new ReferenceHandler<byte>();
        }
        catch { _exitCode = 1; }
    }

}
