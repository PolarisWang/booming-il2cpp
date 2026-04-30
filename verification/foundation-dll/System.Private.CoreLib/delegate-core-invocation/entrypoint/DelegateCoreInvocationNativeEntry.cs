// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/delegate/core/invocation
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Reflection;

public static class DelegateCoreInvocationNativeEntry
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

    // [0] System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)
    static int Method0()
    {
        return Delegate.Combine(new Action(() => {}), new Action(() => {})).GetHashCode();
    }

    // [1] System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)
    static int Method1()
    {
        return Delegate.Remove(new Action(() => {}), new Action(() => {})).GetHashCode();
    }

    // [2] System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])
    static int Method2()
    {
        return (new Action(() => {}).DynamicInvoke()?.GetHashCode() ?? 0);
    }

    // [3] System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()
    static int Method3()
    {
        return new Action(() => {}).Method.GetHashCode();
    }

    // [4] System.Private.CoreLib/System.Delegate::get_Target:System.Object()
    static int Method4()
    {
        return (new Action(() => {}).Target?.GetHashCode() ?? 0);
    }

    // [5] System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Object,System.String)
    static int Method5()
    {
        return Delegate.CreateDelegate(typeof(Action), new object(), typeof(byte).GetMethod("ToString", System.Type.EmptyTypes)!).GetHashCode();
    }

    // [6] System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Reflection.MethodInfo)
    static int Method6()
    {
        return Delegate.CreateDelegate(typeof(Action), typeof(byte).GetMethod("ToString", System.Type.EmptyTypes)!).GetHashCode();
    }

    // [7] System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]()
    static int Method7()
    {
        return new Action(() => {}).GetInvocationList().GetHashCode();
    }

    // [8] System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate)
    static int Method8()
    {
        // TODO: System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate) could not be auto-generated
        return 0;
    }

    // [9] System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate)
    static int Method9()
    {
        // TODO: System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate) could not be auto-generated
        return 0;
    }

}