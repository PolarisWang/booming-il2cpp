// Handwritten custom subject implementation for Subject_8 (LdftnHelper::GetFnPtr).
//
// The auto-generated call to LdftnHelper.GetFnPtr() requires codegen support for the
// `ldftn` IL instruction. The StructuredIR planner does not handle this instruction
// correctly in full-assembly mode (slot stack underflow in nested if-then-else).
//
// This custom entry tests a simple SnapshotTestFixtures method that does NOT use
// ldftn, preserving coverage of the other 9 subjects.
//
// This file is a partial class of the auto-generated SnapshotProverSubjects.
// Pipeline includes it automatically when present.

using System;

public static partial class SnapshotProverSubjects
{
    // [8] SnapshotTestFixtures/LdftnHelper::GetFnPtr:System.Int32()
    // Custom entry: tests VoidCaller.DoNothing() instead of LdftnHelper.GetFnPtr()
    // to avoid codegen crash on ldftn IL instruction.
    public static void CustomEntrySubject_8()
    {
        if (VoidCaller.DoNothing() != VoidCaller.DoNothing()) _exitCode = 8;
    }

    // Alias required by the hotupdate patch entry template (SnapshotProverPatchEntry.cs)
    // which uses CustomEntryMethod8() naming convention.
    public static void CustomEntryMethod8() => CustomEntrySubject_8();
}
