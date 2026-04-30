// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/member-complete
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Reflection;

public static class SystemPrivateCoreLib_ReflectionMemberCompleteNativeEntry
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
            case 14: return Method14();
            case 15: return Method15();
            case 16: return Method16();
            case 17: return Method17();
            case 18: return Method18();
            case 19: return Method19();
            case 20: return Method20();
            case 21: return Method21();
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

    // [3] System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()
    static int Method3()
    {
        return typeof(string).GetMembers()[0].ReflectedType.GetHashCode();
    }

    // [4] System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()
    static int Method4()
    {
        return typeof(string).GetMembers()[0].MetadataToken;
    }

    // [5] System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()
    static int Method5()
    {
        return typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.ReturnType.GetHashCode();
    }

    // [6] System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()
    static int Method6()
    {
        return typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.GetBaseDefinition().GetHashCode();
    }

    // [7] System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()
    static int Method7()
    {
        return typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.MethodHandle.GetHashCode();
    }

    // [8] System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()
    static int Method8()
    {
        return typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.CallingConvention.GetHashCode();
    }

    // [9] System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()
    static int Method9()
    {
        return (typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.IsPublic ? 1 : 0);
    }

    // [10] System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()
    static int Method10()
    {
        return (typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.IsStatic ? 1 : 0);
    }

    // [11] System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()
    static int Method11()
    {
        return (typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.IsVirtual ? 1 : 0);
    }

    // [12] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()
    static int Method12()
    {
        return typeof(string).GetField("Empty")!.FieldType.GetHashCode();
    }

    // [13] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()
    static int Method13()
    {
        return (typeof(string).GetField("Empty")!.IsStatic ? 1 : 0);
    }

    // [14] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()
    static int Method14()
    {
        return (typeof(string).GetField("Empty")!.IsInitOnly ? 1 : 0);
    }

    // [15] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()
    static int Method15()
    {
        return (typeof(string).GetField("Empty")!.IsLiteral ? 1 : 0);
    }

    // [16] System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()
    static int Method16()
    {
        return typeof(string).GetProperty("Length")!.PropertyType.GetHashCode();
    }

    // [17] System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()
    static int Method17()
    {
        return (typeof(string).GetProperty("Length")!.CanRead ? 1 : 0);
    }

    // [18] System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()
    static int Method18()
    {
        return (typeof(string).GetProperty("Length")!.CanWrite ? 1 : 0);
    }

    // [19] System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()
    static int Method19()
    {
        return typeof(string).GetProperty("Length")!.GetIndexParameters().GetHashCode();
    }

    // [20] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()
    static int Method20()
    {
        return typeof(string).GetField("Empty")!.FieldHandle.GetHashCode();
    }

    // [21] System.Private.CoreLib/System.Reflection.ConstructorInfo::get_CallingConvention:System.Reflection.CallingConventions()
    static int Method21()
    {
        return typeof(object).GetConstructor(System.Type.EmptyTypes)!.CallingConvention.GetHashCode();
    }

}