// Handwritten custom entry for patch variant — Type.GetType(string) at index 21.
// The auto-generator cannot produce this because GetType(string) returns a
// System.Type via reflection resolution, which is not a simple static call.
using System;
public static partial class ReflectionAssemblyPatchEntry
{
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    public static void CustomEntryMethod21()
    {
        var t = Type.GetType("System.String, System.Private.CoreLib");
        if (t == null) _exitCode = 1;
    }
}
