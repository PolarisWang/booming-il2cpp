// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/delegate/core/invocation
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Reflection;

public static partial class DelegateCoreInvocationSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)
    public static void Subject_0()
    {
        try { Delegate.Combine(null!, null!); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)
    public static void Subject_1()
    {
        try { Delegate.Remove(null!, null!); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])
    public static void Subject_2()
    {
        try { new System.Action(() => {}).DynamicInvoke(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()
    public static void Subject_3()
    {
        if (((new System.Action(() => {}).Method).GetHashCode()) != 407344002) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Delegate::get_Target:System.Object()
    public static void Subject_4()
    {
        if (((new System.Action(() => {}).Target).GetHashCode()) != 35342034) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Object,System.String)
    public static void Subject_5()
    {
        // TODO: System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Object,System.String) could not be auto-generated
    }

    // [6] System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Reflection.MethodInfo)
    public static void Subject_6()
    {
        // TODO: System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Reflection.MethodInfo) could not be auto-generated
    }

    // [7] System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]()
    public static void Subject_7()
    {
        if (((new System.Action(() => {}).GetInvocationList()).Length) != 1) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate)
    public static void Subject_8()
    {
        // TODO: System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate) could not be auto-generated
    }

    // [9] System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate)
    public static void Subject_9()
    {
        // TODO: System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate) could not be auto-generated
    }

}