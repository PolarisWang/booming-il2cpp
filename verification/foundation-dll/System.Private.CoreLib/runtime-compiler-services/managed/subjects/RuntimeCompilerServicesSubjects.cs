// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/runtime/compiler/services
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Runtime.CompilerServices;

public static partial class RuntimeCompilerServicesSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode:System.Int32(System.Object)
    public static void Subject_0()
    {
        if (RuntimeHelpers.GetHashCode(42) != RuntimeHelpers.GetHashCode(42)) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::Equals:System.Boolean(System.Object,System.Object)
    public static void Subject_1()
    {
        if (((RuntimeHelpers.Equals(42, 42)) ? 1 : 0) != ((RuntimeHelpers.Equals(42, 42)) ? 1 : 0)) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
    public static void Subject_2()
    {
        if (((RuntimeHelpers.GetObjectValue(42)).GetHashCode()) != ((RuntimeHelpers.GetObjectValue(42)).GetHashCode())) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::RunClassConstructor:System.Void(System.RuntimeTypeHandle)
    public static void Subject_3()
    {
        // TODO: System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::RunClassConstructor:System.Void(System.RuntimeTypeHandle) could not be auto-generated
    }

    // [4] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)
    public static void Subject_4()
    {
        // TODO: System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle) could not be auto-generated
    }

    // [5] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range)
    public static void Subject_5()
    {
        // TODO: System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range) could not be auto-generated
    }

    // [6] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T)
    public static void Subject_6()
    {
        // TODO: System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T) could not be auto-generated
    }

    // [7] System.Private.CoreLib/System.Runtime.CompilerServices.FormattableStringFactory::Create:System.Runtime.CompilerServices.FormattableString(System.String,System.Object[])
    public static void Subject_7()
    {
        if (((FormattableStringFactory.Create("hello", Array.Empty<System.Object>())).GetHashCode()) != ((FormattableStringFactory.Create("hello", Array.Empty<System.Object>())).GetHashCode())) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::.ctor:System.Void(System.Object)
    public static void Subject_8()
    {
        new RuntimeWrappedException(42);
    }

    // [9] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::get_WrappedException:System.Object()
    public static void Subject_9()
    {
        if (((new RuntimeWrappedException(42).WrappedException).GetHashCode()) != ((new RuntimeWrappedException(42).WrappedException).GetHashCode())) _exitCode = 1;
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Subject_0(); break;
            case 1: Subject_1(); break;
            case 2: Subject_2(); break;
            case 3: Subject_3(); break;
            case 4: Subject_4(); break;
            case 5: Subject_5(); break;
            case 6: Subject_6(); break;
            case 7: Subject_7(); break;
            case 8: Subject_8(); break;
            case 9: Subject_9(); break;
        }
    }

}