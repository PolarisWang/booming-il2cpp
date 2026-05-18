// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/codegen/edge/cases
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class CodegenEdgeCasesSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_0:System.Void()
    public static void Subject_0()
    {
        CodegenEdgeCasesSubjects.Subject_0();
    }

    // [1] System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_1:System.Void(System.Int32)
    public static void Subject_1()
    {
        CodegenEdgeCasesSubjects.Subject_1(42);
    }

    // [2] System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_2:System.Void()
    public static void Subject_2()
    {
        CodegenEdgeCasesSubjects.Subject_2();
    }

    // [3] System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_3:System.Int32(System.Int32)
    public static void Subject_3()
    {
        if (CodegenEdgeCasesSubjects.Subject_3(42) != CodegenEdgeCasesSubjects.Subject_3(42)) _exitCode = 1;
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Subject_0(); break;
            case 1: Subject_1(); break;
            case 2: Subject_2(); break;
            case 3: Subject_3(); break;
        }
    }

}