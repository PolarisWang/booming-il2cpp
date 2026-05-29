// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/delegate/core/invocation
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class DelegateCoreInvocationSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)
    public static void Subject_0()
    {
        try { _ = ((Delegate.Combine(default, default)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)
    public static void Subject_1()
    {
        try { _ = ((Delegate.Remove(default, default)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])
    public static void Subject_2()
    {
        try { _ = ((new System.Action(() => {}).DynamicInvoke()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()
    public static void Subject_3()
    {
        try { _ = ((new System.Action(() => {}).Method).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Delegate::get_Target:System.Object()
    public static void Subject_4()
    {
        try { _ = ((new System.Action(() => {}).Target).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Object,System.String)
    // [6] System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Reflection.MethodInfo)
    // [7] System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]()
    public static void Subject_7()
    {
        try { _ = ((new System.Action(() => {}).GetInvocationList()).Length); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate)
    // [9] System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate)
}