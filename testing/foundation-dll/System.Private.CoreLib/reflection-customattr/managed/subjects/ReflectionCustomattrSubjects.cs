// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/customattr
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

public static partial class ReflectionCustomattrSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.MemberInfo::IsDefined:System.Boolean(System.Type,System.Boolean)
    public static void Subject_0()
    {
        try { if (((default(MemberInfo)!.IsDefined(typeof(byte), true)) ? 1 : 0) != ((default(MemberInfo)!.IsDefined(typeof(byte), true)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Reflection.MemberInfo::GetCustomAttribute:System.Object(System.Type,System.Boolean)
    public static void Subject_1()
    {
        try { if (((default(MemberInfo)!.GetCustomAttribute(typeof(byte), true)).GetHashCode()) != ((default(MemberInfo)!.GetCustomAttribute(typeof(byte), true)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Reflection.MemberInfo::GetCustomAttributes:System.Object[](System.Boolean)
    public static void Subject_2()
    {
        try { if (((default(MemberInfo)!.GetCustomAttributes(true)).Length) != ((default(MemberInfo)!.GetCustomAttributes(true)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}