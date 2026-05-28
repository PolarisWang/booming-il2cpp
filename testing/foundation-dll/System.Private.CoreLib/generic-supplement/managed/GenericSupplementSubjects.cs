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
        try { _ = GenericSupplementNativeEntry.TestSupplementalMetadataRoundtrip(); }
        catch { _exitCode = 1; }
    }

    // [1] GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestTypeLookupBySubjectId:System.Int32()
    public static void Subject_1()
    {
        try { _ = GenericSupplementNativeEntry.TestTypeLookupBySubjectId(); }
        catch { _exitCode = 1; }
    }

    // [2] GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestTypeLookupByToken:System.Int32()
    public static void Subject_2()
    {
        try { _ = GenericSupplementNativeEntry.TestTypeLookupByToken(); }
        catch { _exitCode = 1; }
    }

    // [3] GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestGenericInstantiation:System.Int32()
    public static void Subject_3()
    {
        try { _ = GenericSupplementNativeEntry.TestGenericInstantiation(); }
        catch { _exitCode = 1; }
    }

}