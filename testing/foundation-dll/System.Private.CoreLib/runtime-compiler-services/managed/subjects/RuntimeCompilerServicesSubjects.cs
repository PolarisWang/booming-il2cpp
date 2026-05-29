// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/runtime/compiler/services
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
    // [1] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::Equals:System.Boolean(System.Object,System.Object)
    // [2] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
    // [3] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::RunClassConstructor:System.Void(System.RuntimeTypeHandle)
    public static void Subject_3()
    {
        try { RuntimeHelpers.RunClassConstructor(typeof(byte).TypeHandle); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)
    // [5] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range)
    // [6] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T)
    // [7] System.Private.CoreLib/System.Runtime.CompilerServices.FormattableStringFactory::Create:System.Runtime.CompilerServices.FormattableString(System.String,System.Object[])
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