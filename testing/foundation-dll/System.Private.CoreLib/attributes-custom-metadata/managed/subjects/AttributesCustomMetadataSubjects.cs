// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/attributes/custom/metadata
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

public static partial class AttributesCustomMetadataSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.MemberInfo,System.Type)
    public static void Subject_0()
    {
        try { _ = ((typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute))).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.Assembly,System.Type)
    public static void Subject_1()
    {
        try { _ = ((typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute))).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo,System.Type)
    public static void Subject_2()
    {
        try { _ = (new System.Collections.Generic.List<System.Attribute>(typeof(byte).Assembly.GetCustomAttributes(typeof(AssemblyDescriptionAttribute)).Cast<System.Attribute>()).Count); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly,System.Type)
    public static void Subject_3()
    {
        try { _ = (new System.Collections.Generic.List<System.Attribute>(typeof(byte).Assembly.GetCustomAttributes(typeof(AssemblyDescriptionAttribute)).Cast<System.Attribute>()).Count); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo)
    public static void Subject_4()
    {
        try { _ = (new System.Collections.Generic.List<System.Attribute>(typeof(byte).Assembly.GetCustomAttributes(false).Cast<System.Attribute>()).Count); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly)
    public static void Subject_5()
    {
        try { _ = (new System.Collections.Generic.List<System.Attribute>(typeof(byte).Assembly.GetCustomAttributes(false).Cast<System.Attribute>()).Count); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.MemberInfo,System.Type)
    public static void Subject_6()
    {
        try { _ = ((typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute))) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.Assembly,System.Type)
    public static void Subject_7()
    {
        try { _ = ((typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute))) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Attribute::get_TypeId:System.Object()
    public static void Subject_8()
    {
        // needs-manual — get_TypeId with 0 params requires manual implementation: System.Private.CoreLib/System.Attribute::get_TypeId:System.Object()
    }

}