// Handwritten partial class for reflection-assembly custom entries.
// Source: handwritten/ — READ ONLY, pipeline never overwrites.
// Pipeline copies this file to subjects dir for compilation.

using System;

public static partial class ReflectionAssemblySubjects
{
    /// Custom entry for Subject_21 (Type.GetType with non-existent type).
    /// Type.GetType("NonExistentType") returns null in both managed and native AOT.
    /// We dereference with !.GetHashCode() to trigger codegen's RaiseNullReferenceException.
    public static void CustomEntrySubject_21()
    {
        try { Type.GetType("NonExistentType")!.GetHashCode(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }
}
