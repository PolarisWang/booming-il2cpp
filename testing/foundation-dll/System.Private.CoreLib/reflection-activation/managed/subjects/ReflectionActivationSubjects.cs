// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/activation
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Reflection;

public static partial class ReflectionActivationSubjects
{
    // [0] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)
    public static int Subject_0()
    {
        return ((Activator.CreateInstance(typeof(byte))).GetHashCode());
    }

    // [1] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])
    public static int Subject_1()
    {
        return ((Activator.CreateInstance(typeof(byte), Array.Empty<System.Object>())).GetHashCode());
    }

    // [2] System.Private.CoreLib/System.Activator::CreateInstance:T()
    public static int Subject_2()
    {
        return -1;  // cannot auto-generate call
    }

    // [3] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    public static int Subject_3()
    {
        return ((Activator.CreateInstance(typeof(byte), System.Reflection.BindingFlags.Default, default, Array.Empty<System.Object>(), default)).GetHashCode());
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static int Subject_4()
    {
        return ((Array.CreateInstance(typeof(byte), 42)).GetHashCode());
    }

    // [5] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    public static int Subject_5()
    {
        return ((Array.CreateInstance(typeof(byte), 42, 42)).GetHashCode());
    }

    // [6] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32[])
    public static int Subject_6()
    {
        return ((Array.CreateInstance(typeof(byte), Array.Empty<System.Int32>())).GetHashCode());
    }

    // [7] System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])
    public static int Subject_7()
    {
        return ((typeof(byte).GetConstructors()[0].Invoke(new object[0])).GetHashCode());
    }

}