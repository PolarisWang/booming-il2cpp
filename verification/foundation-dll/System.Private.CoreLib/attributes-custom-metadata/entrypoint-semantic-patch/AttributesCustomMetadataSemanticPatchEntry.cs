// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/attributes/custom/metadata
// Assembly: System.Private.CoreLib
// Variant: semantic-patch

using System;
using System.Reflection;

public static class AttributesCustomMetadataSemanticPatchEntry
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
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.MemberInfo,System.Type)
    static int Method0()
    {
        return typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute)).GetHashCode();
    }

    // [1] System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.Assembly,System.Type)
    static int Method1()
    {
        return typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute)).GetHashCode();
    }

    // [2] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo,System.Type)
    static int Method2()
    {
        return typeof(byte).Assembly.GetCustomAttributes(typeof(AssemblyDescriptionAttribute)).GetHashCode();
    }

    // [3] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly,System.Type)
    static int Method3()
    {
        return typeof(byte).Assembly.GetCustomAttributes(typeof(AssemblyDescriptionAttribute)).GetHashCode();
    }

    // [4] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo)
    static int Method4()
    {
        return typeof(byte).Assembly.GetCustomAttributes().GetHashCode();
    }

    // [5] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly)
    static int Method5()
    {
        return typeof(byte).Assembly.GetCustomAttributes().GetHashCode();
    }

    // [6] System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.MemberInfo,System.Type)
    static int Method6()
    {
        return (typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute)) ? 1 : 0);
    }

    // [7] System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.Assembly,System.Type)
    static int Method7()
    {
        return (typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute)) ? 1 : 0);
    }

    // [8] System.Private.CoreLib/System.Attribute::get_TypeId:System.Object()
    static int Method8()
    {
        return (new ObsoleteAttribute().TypeId?.GetHashCode() ?? 0);
    }

}