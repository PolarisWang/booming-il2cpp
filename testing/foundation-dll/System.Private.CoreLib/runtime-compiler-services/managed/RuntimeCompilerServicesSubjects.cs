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
    // [0] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode:System.Int32(System.Object)
    [Fact]
    public static void Subject_0()
    {
        try { var __val = RuntimeHelpers.GetHashCode(null!); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::Equals:System.Boolean(System.Object,System.Object)
    [Fact]
    public static void Subject_1()
    {
        try { var __val = ((RuntimeHelpers.Equals(null!, null!)) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
    [Fact]
    public static void Subject_2()
    {
        try { var __val = ((RuntimeHelpers.GetObjectValue(null!)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::RunClassConstructor:System.Void(System.RuntimeTypeHandle)
    [Fact]
    public static void Subject_3()
    {
        try { RuntimeHelpers.RunClassConstructor(typeof(byte).TypeHandle); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)
    // [5] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range)
    // [6] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T)
    [Fact]
    public static void Subject_6()
    {
        // non-callable: System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T)
    }

    // [7] System.Private.CoreLib/System.Runtime.CompilerServices.FormattableStringFactory::Create:System.Runtime.CompilerServices.FormattableString(System.String,System.Object[])
    // [8] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::.ctor:System.Void(System.Object)
    [Fact]
    public static void Subject_8()
    {
        try { new RuntimeWrappedException(null!); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::get_WrappedException:System.Object()
    [Fact]
    public static void Subject_9()
    {
        try { var __val = ((new RuntimeWrappedException(42).WrappedException).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}