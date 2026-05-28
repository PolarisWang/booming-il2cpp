// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/runtime/compiler-services
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading;

public static partial class RuntimeCompilerServicesSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode:System.Int32(System.Object)
    public static void Subject_0()
    {
        try { _ = RuntimeHelpers.GetHashCode(null!); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::Equals:System.Boolean(System.Object,System.Object)
    public static void Subject_1()
    {
        try { _ = ((RuntimeHelpers.Equals(null!, null!)) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
    public static void Subject_2()
    {
        try { _ = ((RuntimeHelpers.GetObjectValue(null!)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::RunClassConstructor:System.Void(System.RuntimeTypeHandle)
    public static void Subject_3()
    {
        try { RuntimeHelpers.RunClassConstructor(typeof(byte).TypeHandle); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)
    public static void Subject_4()
    {
        // needs-manual — InitializeArray with 2 params requires manual implementation: System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)
    }

    // [5] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range)
    public static void Subject_5()
    {
        // needs-manual — GetSubArray with 2 params requires manual implementation: System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range)
    }

    // [6] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T)
    public static void Subject_6()
    {
        // non-callable: System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T)
    }

    // [7] System.Private.CoreLib/System.Runtime.CompilerServices.FormattableStringFactory::Create:System.Runtime.CompilerServices.FormattableString(System.String,System.Object[])
    public static void Subject_7()
    {
        try { _ = ((FormattableStringFactory.Create("hello", Array.Empty<System.Object>())).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::.ctor:System.Void(System.Object)
    public static void Subject_8()
    {
        try { new RuntimeWrappedException(null!); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::get_WrappedException:System.Object()
    public static void Subject_9()
    {
        try { _ = ((new RuntimeWrappedException(42).WrappedException).GetHashCode()); }
        catch { _exitCode = 1; }
    }

}