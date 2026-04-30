// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/runtime/compiler/services
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Runtime.CompilerServices;

public static class RuntimeCompilerServicesNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            case 3: return Method3();
            case 4: return Method4();
            case 5: return Method5();
            case 6: return Method6();
            case 7: return Method7();
            case 8: return Method8();
            case 9: return Method9();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode:System.Int32(System.Object)
    static int Method0()
    {
        return RuntimeHelpers.GetHashCode(42);
    }

    // [1] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::Equals:System.Boolean(System.Object,System.Object)
    static int Method1()
    {
        return (RuntimeHelpers.Equals(42, 42) ? 1 : 0);
    }

    // [2] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
    static int Method2()
    {
        return (RuntimeHelpers.GetObjectValue(42)?.GetHashCode() ?? 0);
    }

    // [3] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::RunClassConstructor:System.Void(System.RuntimeTypeHandle)
    static int Method3()
    {
        // TODO: System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::RunClassConstructor:System.Void(System.RuntimeTypeHandle) could not be auto-generated
        return 0;
    }

    // [4] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)
    static int Method4()
    {
        // TODO: System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle) could not be auto-generated
        return 0;
    }

    // [5] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range)
    static int Method5()
    {
        // TODO: System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range) could not be auto-generated
        return 0;
    }

    // [6] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T)
    static int Method6()
    {
        // TODO: System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T) could not be auto-generated
        return 0;
    }

    // [7] System.Private.CoreLib/System.Runtime.CompilerServices.FormattableStringFactory::Create:System.Runtime.CompilerServices.FormattableString(System.String,System.Object[])
    static int Method7()
    {
        return FormattableStringFactory.Create("hello", Array.Empty<System.Object>()).GetHashCode();
    }

    // [8] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::.ctor:System.Void(System.Object)
    static int Method8()
    {
        new RuntimeWrappedException(42);
        return (int)0;
    }

    // [9] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::get_WrappedException:System.Object()
    static int Method9()
    {
        return (new RuntimeWrappedException(42).WrappedException?.GetHashCode() ?? 0);
    }

}