// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/attributes/custom/metadata
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Reflection;

public static partial class AttributesCustomMetadataSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.MemberInfo,System.Type)
    public static void Subject_0()
    {
        if (((typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute))).GetHashCode()) != ((typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute))).GetHashCode())) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.Assembly,System.Type)
    public static void Subject_1()
    {
        if (((typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute))).GetHashCode()) != ((typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute))).GetHashCode())) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo,System.Type)
    public static void Subject_2()
    {
        // TODO: System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo,System.Type) could not be auto-generated
    }

    // [3] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly,System.Type)
    public static void Subject_3()
    {
        // TODO: System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly,System.Type) could not be auto-generated
    }

    // [4] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo)
    public static void Subject_4()
    {
        // TODO: System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo) could not be auto-generated
    }

    // [5] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly)
    public static void Subject_5()
    {
        // TODO: System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly) could not be auto-generated
    }

    // [6] System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.MemberInfo,System.Type)
    public static void Subject_6()
    {
        if (((typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute))) ? 1 : 0) != ((typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute))) ? 1 : 0)) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.Assembly,System.Type)
    public static void Subject_7()
    {
        if (((typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute))) ? 1 : 0) != ((typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute))) ? 1 : 0)) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Attribute::get_TypeId:System.Object()
    public static void Subject_8()
    {
        // TODO: System.Private.CoreLib/System.Attribute::get_TypeId:System.Object() could not be auto-generated
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
        }
    }

}