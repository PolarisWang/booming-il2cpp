// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class ReflectionMemberBasicsSubjects
{
    // [0] System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()
    public static void CustomEntrySubject_0()
    {        try { _ = default(MemberInfo)!.Name; } catch { _exitCode = 1; }    }

    // [1] System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()
    public static void CustomEntrySubject_1()
    {        try { _ = default(MemberInfo)!.MemberType; } catch { _exitCode = 1; }    }

    // [2] System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()
    public static void CustomEntrySubject_2()
    {        try { _ = default(MemberInfo)!.DeclaringType; } catch { _exitCode = 1; }    }

    // [3] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
    public static void CustomEntrySubject_3()
    {        try { _ = default(MethodInfo)!.Invoke(null!, Array.Empty<System.Object>()); } catch { _exitCode = 1; }    }

    // [4] System.Private.CoreLib/System.Reflection.MethodInfo::GetParameters:System.Reflection.ParameterInfo[]()
    public static void CustomEntrySubject_4()
    {        try { _ = default(MethodInfo)!.GetParameters(); } catch { _exitCode = 1; }    }

    // [5] System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()
    public static void CustomEntrySubject_5()
    {        try { _ = default(MethodInfo)!.ReturnType; } catch { _exitCode = 1; }    }

    // [9] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()
    public static void CustomEntrySubject_9()
    {        try { _ = 0; } catch { _exitCode = 1; }    }

    // [13] System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()
    public static void CustomEntrySubject_13()
    {        try { _ = 0; } catch { _exitCode = 1; }    }

}
