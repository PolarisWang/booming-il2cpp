// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/attributes/custom/metadata
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Reflection;

public static partial class AttributesCustomMetadataSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.MemberInfo,System.Type)
    public static void Subject_0()
    {
        try { if (((typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute))).GetHashCode()) != ((typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.Assembly,System.Type)
    public static void Subject_1()
    {
        try { if (((typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute))).GetHashCode()) != ((typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo,System.Type)
    public static void Subject_2()
    {
        try { if (((new System.Collections.Generic.List<System.Attribute>(typeof(byte).Assembly.GetCustomAttributes(typeof(AssemblyDescriptionAttribute))).Count).Length) != ((new System.Collections.Generic.List<System.Attribute>(typeof(byte).Assembly.GetCustomAttributes(typeof(AssemblyDescriptionAttribute))).Count).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly,System.Type)
    public static void Subject_3()
    {
        try { if (((new System.Collections.Generic.List<System.Attribute>(typeof(byte).Assembly.GetCustomAttributes(typeof(AssemblyDescriptionAttribute))).Count).Length) != ((new System.Collections.Generic.List<System.Attribute>(typeof(byte).Assembly.GetCustomAttributes(typeof(AssemblyDescriptionAttribute))).Count).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo)
    public static void Subject_4()
    {
        try { if (((new System.Collections.Generic.List<System.Attribute>(typeof(byte).Assembly.GetCustomAttributes(false)).Count).Length) != ((new System.Collections.Generic.List<System.Attribute>(typeof(byte).Assembly.GetCustomAttributes(false)).Count).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly)
    public static void Subject_5()
    {
        try { if (((new System.Collections.Generic.List<System.Attribute>(typeof(byte).Assembly.GetCustomAttributes(false)).Count).Length) != ((new System.Collections.Generic.List<System.Attribute>(typeof(byte).Assembly.GetCustomAttributes(false)).Count).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.MemberInfo,System.Type)
    public static void Subject_6()
    {
        try { if (((typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute))) ? 1 : 0) != ((typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.Assembly,System.Type)
    public static void Subject_7()
    {
        try { if (((typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute))) ? 1 : 0) != ((typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Attribute::get_TypeId:System.Object()
    public static void Subject_8()
    {
        // TODO: System.Private.CoreLib/System.Attribute::get_TypeId:System.Object() could not be auto-generated
    }

}