// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/delegate/core/invocation
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Reflection;

public static partial class DelegateCoreInvocationSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)
    public static void Subject_0()
    {
        if (((Delegate.Combine(null!, null!)).GetHashCode()) != ((Delegate.Combine(null!, null!)).GetHashCode())) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)
    public static void Subject_1()
    {
        if (((Delegate.Remove(null!, null!)).GetHashCode()) != ((Delegate.Remove(null!, null!)).GetHashCode())) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])
    public static void Subject_2()
    {
        // TODO: System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[]) could not be auto-generated
    }

    // [3] System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()
    public static void Subject_3()
    {
        // TODO: System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo() could not be auto-generated
    }

    // [4] System.Private.CoreLib/System.Delegate::get_Target:System.Object()
    public static void Subject_4()
    {
        // TODO: System.Private.CoreLib/System.Delegate::get_Target:System.Object() could not be auto-generated
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
        // TODO: System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]() could not be auto-generated
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