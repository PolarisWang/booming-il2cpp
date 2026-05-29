// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/hotupdate
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class ReflectionHotupdateSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()
    public static void Subject_0()
    {
        // non-callable: System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()
    }

    // [1] System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()
    public static void Subject_1()
    {
        // non-callable: System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()
    }

    // [2] System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)
    public static void Subject_2()
    {
        try { _ = ((Type.GetTypeFromHandle(default(System.RuntimeTypeHandle))).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)
    public static void Subject_3()
    {
        // non-callable: System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)
    }

}