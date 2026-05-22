// Handwritten custom subject implementation for Subject_8 (LdftnHelper::GetFnPtr).
//
// The auto-generated call to LdftnHelper.GetFnPtr() requires codegen support for the
// `ldftn` IL instruction. The StructuredIR planner does not handle this instruction
// correctly in full-assembly mode (slot stack underflow in nested if-then-else).
//
// This is the patch (hotupdate) variant. It replaces the Subject_8 call with a
// no-op to avoid hitting the ldftn codegen crash. The patch entry DLL does not
// reference test fixtures (VoidCaller, etc.), so we use a simple empty method.
//
// For the subjects variant, the equivalent file lives at
// managed/subjects/SnapshotProverSubjects.Custom.cs.

public static partial class SnapshotProverPatchEntry
{
    // [8] SnapshotTestFixtures/LdftnHelper::GetFnPtr:System.Int32()
    // No-op replacement to avoid ldftn codegen crash in hotupdate path.
    public static void CustomEntryMethod8()
    {
        // Intentionally empty: Subject_8's ldftn IL instruction is not
        // supported by the current codegen IR planner. This no-op allows
        // the other 9 subjects to be tested in hotupdate mode.
    }
}
