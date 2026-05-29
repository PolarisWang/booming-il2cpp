// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/generic/nullable/value
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class GenericNullableValueSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()
    public static void Subject_0()
    {
        try { _ = ((default(Nullable<int>).HasValue) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Nullable`1::get_Value:T()
    public static void Subject_1()
    {
        try { _ = ((((int?)42).Value).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()
    public static void Subject_2()
    {
        try { _ = ((default(Nullable<int>).GetValueOrDefault()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)
    // [4] System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)
    public static void Subject_4()
    {
        try { _ = ((default(Nullable<int>).Equals(null!)) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()
    public static void Subject_5()
    {
        try { _ = default(Nullable<int>).GetHashCode(); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Nullable`1::ToString:System.String()
    public static void Subject_6()
    {
        try { _ = ((default(Nullable<int>).ToString()).Length); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
    public static void Subject_7()
    {
        try { _ = ((((ValueType)42).Equals(null!)) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()
    public static void Subject_8()
    {
        try { _ = ((ValueType)42).GetHashCode(); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.ValueType::ToString:System.String()
    public static void Subject_9()
    {
        try { _ = ((((ValueType)42).ToString()).Length); }
        catch { _exitCode = 1; }
    }

}