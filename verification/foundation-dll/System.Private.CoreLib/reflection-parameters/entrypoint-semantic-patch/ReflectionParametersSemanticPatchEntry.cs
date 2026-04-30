// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/parameters
// Assembly: System.Private.CoreLib
// Variant: semantic-patch

using System;
using System.Reflection;

public static class ReflectionParametersSemanticPatchEntry
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

    // [0] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String()
    static int Method0()
    {
        return (typeof(int).GetMethod("CompareTo", new[] { typeof(int) })!.GetParameters()[0].Name?.GetHashCode() ?? 0);
    }

    // [1] System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType:System.Type()
    static int Method1()
    {
        return typeof(int).GetMethod("CompareTo", new[] { typeof(int) })!.GetParameters()[0].ParameterType.GetHashCode();
    }

    // [2] System.Private.CoreLib/System.Reflection.ParameterInfo::get_DefaultValue:System.Object()
    static int Method2()
    {
        return (typeof(int).GetMethod("CompareTo", new[] { typeof(int) })!.GetParameters()[0].DefaultValue?.GetHashCode() ?? 0);
    }

    // [3] System.Private.CoreLib/System.Reflection.ParameterInfo::get_RawDefaultValue:System.Object()
    static int Method3()
    {
        return (typeof(int).GetMethod("CompareTo", new[] { typeof(int) })!.GetParameters()[0].RawDefaultValue?.GetHashCode() ?? 0);
    }

    // [4] System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue:System.Boolean()
    static int Method4()
    {
        return (typeof(int).GetMethod("CompareTo", new[] { typeof(int) })!.GetParameters()[0].HasDefaultValue ? 1 : 0);
    }

    // [5] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Attributes:System.Reflection.ParameterAttributes()
    static int Method5()
    {
        return typeof(int).GetMethod("CompareTo", new[] { typeof(int) })!.GetParameters()[0].Attributes.GetHashCode();
    }

    // [6] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Position:System.Int32()
    static int Method6()
    {
        return typeof(int).GetMethod("CompareTo", new[] { typeof(int) })!.GetParameters()[0].Position;
    }

    // [7] System.Private.CoreLib/System.Reflection.ParameterInfo::GetRequiredCustomModifiers:System.Type[]()
    static int Method7()
    {
        return typeof(int).GetMethod("CompareTo", new[] { typeof(int) })!.GetParameters()[0].GetRequiredCustomModifiers().GetHashCode();
    }

    // [8] System.Private.CoreLib/System.Reflection.ParameterInfo::GetOptionalCustomModifiers:System.Type[]()
    static int Method8()
    {
        return typeof(int).GetMethod("CompareTo", new[] { typeof(int) })!.GetParameters()[0].GetOptionalCustomModifiers().GetHashCode();
    }

    // [9] System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]()
    static int Method9()
    {
        return typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.GetParameters().GetHashCode();
    }

}