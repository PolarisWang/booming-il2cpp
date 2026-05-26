// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/generic/supplement
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class GenericSupplementSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestSupplementalMetadataRoundtrip:System.Int32()
    public static void Subject_0()
    {
        try { if (GenericSupplementNativeEntry.TestSupplementalMetadataRoundtrip() != GenericSupplementNativeEntry.TestSupplementalMetadataRoundtrip()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestTypeLookupBySubjectId:System.Int32()
    public static void Subject_1()
    {
        try { if (GenericSupplementNativeEntry.TestTypeLookupBySubjectId() != GenericSupplementNativeEntry.TestTypeLookupBySubjectId()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestTypeLookupByToken:System.Int32()
    public static void Subject_2()
    {
        try { if (GenericSupplementNativeEntry.TestTypeLookupByToken() != GenericSupplementNativeEntry.TestTypeLookupByToken()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestGenericInstantiation:System.Int32()
    public static void Subject_3()
    {
        try { if (GenericSupplementNativeEntry.TestGenericInstantiation() != GenericSupplementNativeEntry.TestGenericInstantiation()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}