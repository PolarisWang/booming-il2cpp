// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/member/basics
// Assembly: System.Private.CoreLib
// Variant: semantic-patch

using System;
using System.Reflection;

public static class ReflectionMemberBasicsSemanticPatchEntry
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
            case 10: return Method10();
            case 11: return Method11();
            case 12: return Method12();
            case 13: return Method13();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()
    static int Method0()
    {
        return (typeof(string).GetMembers()[0].Name?.GetHashCode() ?? 0);
    }

    // [1] System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()
    static int Method1()
    {
        return typeof(string).GetMembers()[0].MemberType.GetHashCode();
    }

    // [2] System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()
    static int Method2()
    {
        return typeof(string).GetMembers()[0].DeclaringType.GetHashCode();
    }

    // [3] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
    static int Method3()
    {
        return (typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.Invoke("hello", null)?.GetHashCode() ?? 0);
    }

    // [4] System.Private.CoreLib/System.Reflection.MethodInfo::GetParameters:System.Reflection.ParameterInfo[]()
    static int Method4()
    {
        return typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.GetParameters().GetHashCode();
    }

    // [5] System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()
    static int Method5()
    {
        return typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.ReturnType.GetHashCode();
    }

    // [6] System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])
    static int Method6()
    {
        return (typeof(object).GetConstructor(System.Type.EmptyTypes)!.Invoke(null)?.GetHashCode() ?? 0);
    }

    // [7] System.Private.CoreLib/System.Reflection.FieldInfo::GetValue:System.Object(System.Object)
    static int Method7()
    {
        return (typeof(string).GetField("Empty")!.GetValue(null)?.GetHashCode() ?? 0);
    }

    // [8] System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object)
    static int Method8()
    {
        typeof(string).GetField("Empty")!.SetValue(null, "");
        return (int)0;
    }

    // [9] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()
    static int Method9()
    {
        return typeof(string).GetField("Empty")!.FieldType.GetHashCode();
    }

    // [10] System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object)
    static int Method10()
    {
        return (typeof(string).GetProperty("Length")!.GetValue("hello")?.GetHashCode() ?? 0);
    }

    // [11] System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object,System.Object[])
    static int Method11()
    {
        return (typeof(string).GetProperty("Length")!.GetValue("hello", null)?.GetHashCode() ?? 0);
    }

    // [12] System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue:System.Void(System.Object,System.Object)
    static int Method12()
    {
        typeof(string).GetProperty("Length")!.SetValue("hello", 0);
        return (int)0;
    }

    // [13] System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()
    static int Method13()
    {
        return typeof(string).GetProperty("Length")!.PropertyType.GetHashCode();
    }

}