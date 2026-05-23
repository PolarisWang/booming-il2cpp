// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/generic/nullable/value
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;

public static partial class GenericNullableValueSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()
    public static void Subject_0()
    {
        try { if (((default(Nullable<int>).HasValue) ? 1 : 0) != ((default(Nullable<int>).HasValue) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Nullable`1::get_Value:T()
    public static void Subject_1()
    {
        try { if (((((int?)42).Value).GetHashCode()) != ((((int?)42).Value).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()
    public static void Subject_2()
    {
        try { if (((default(Nullable<int>).GetValueOrDefault()).GetHashCode()) != ((default(Nullable<int>).GetValueOrDefault()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)
    public static void Subject_3()
    {
        // non-callable: System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)
    }

    // [4] System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)
    public static void Subject_4()
    {
        try { if (((default(Nullable<int>).Equals(null!)) ? 1 : 0) != ((default(Nullable<int>).Equals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()
    public static void Subject_5()
    {
        try { if (default(Nullable<int>).GetHashCode() != default(Nullable<int>).GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Nullable`1::ToString:System.String()
    public static void Subject_6()
    {
        try { if (((default(Nullable<int>).ToString()).Length) != ((default(Nullable<int>).ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
    public static void Subject_7()
    {
        try { if (((((ValueType)42).Equals(null!)) ? 1 : 0) != ((((ValueType)42).Equals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()
    public static void Subject_8()
    {
        try { if (((ValueType)42).GetHashCode() != ((ValueType)42).GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.ValueType::ToString:System.String()
    public static void Subject_9()
    {
        try { if (((((ValueType)42).ToString()).Length) != ((((ValueType)42).ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}