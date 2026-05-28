// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/customattr
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class ReflectionCustomattrSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.MemberInfo::IsDefined:System.Boolean(System.Type,System.Boolean)
    public static void Subject_0()
    {
        try { _ = ((default(MemberInfo)!.IsDefined(typeof(byte), true)) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Reflection.MemberInfo::GetCustomAttribute:System.Object(System.Type,System.Boolean)
    public static void Subject_1()
    {
        try { _ = ((default(MemberInfo)!.GetCustomAttribute(typeof(byte), true)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Reflection.MemberInfo::GetCustomAttributes:System.Object[](System.Boolean)
    public static void Subject_2()
    {
        try { _ = ((default(MemberInfo)!.GetCustomAttributes(true)).Length); }
        catch { _exitCode = 1; }
    }

}