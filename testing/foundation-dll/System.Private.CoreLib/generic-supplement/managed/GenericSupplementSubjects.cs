using System;

/// <summary>
/// Auto-generated subjects class for GenericSupplement test family.
/// Each subject method calls the corresponding NativeEntry test method twice
/// and checks that both invocations return the same result.
/// </summary>
internal static partial class GenericSupplementSubjects
{
    // Inlined exit code — avoids SDK method resolution in codegen for direct field access.
    // volatile prevents the compiler from eliding the double-invoke pattern.
    private static volatile int _exitCode = 0;

    // Subject 0: roundtrip test
    internal static void Subject_0()
    {
        try
        {
            var a = GenericSupplementNativeEntry.TestSupplementalMetadataRoundtrip();
            var b = GenericSupplementNativeEntry.TestSupplementalMetadataRoundtrip();
            if (a != b) _exitCode = 1;
        }
        catch { _exitCode = 1; }
    }

    // Subject 1: type lookup by subject ID
    internal static void Subject_1()
    {
        try
        {
            var a = GenericSupplementNativeEntry.TestTypeLookupBySubjectId();
            var b = GenericSupplementNativeEntry.TestTypeLookupBySubjectId();
            if (a != b) _exitCode = 1;
        }
        catch { _exitCode = 1; }
    }

    // Subject 2: type lookup by token
    internal static void Subject_2()
    {
        try
        {
            var a = GenericSupplementNativeEntry.TestTypeLookupByToken();
            var b = GenericSupplementNativeEntry.TestTypeLookupByToken();
            if (a != b) _exitCode = 1;
        }
        catch { _exitCode = 1; }
    }

    // Subject 3: generic instantiation tracking
    internal static void Subject_3()
    {
        try
        {
            var a = GenericSupplementNativeEntry.TestGenericInstantiation();
            var b = GenericSupplementNativeEntry.TestGenericInstantiation();
            if (a != b) _exitCode = 1;
        }
        catch { _exitCode = 1; }
    }
}
