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
        if (((default(Nullable<int>).HasValue) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Nullable`1::get_Value:T()
    public static void Subject_1()
    {
        if ((int)(((int?)42).Value) != 42) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()
    public static void Subject_2()
    {
        if ((int)(default(Nullable<int>).GetValueOrDefault()) != 0) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)
    public static void Subject_3()
    {
        // TODO: System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T) could not be auto-generated
    }

    // [4] System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)
    public static void Subject_4()
    {
        if (((default(Nullable<int>).Equals(42)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()
    public static void Subject_5()
    {
        if (default(Nullable<int>).GetHashCode() != 0) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Nullable`1::ToString:System.String()
    public static void Subject_6()
    {
        if (((default(Nullable<int>).ToString()).Length) != 0) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
    public static void Subject_7()
    {
        if (((((ValueType)42).Equals(42)) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()
    public static void Subject_8()
    {
        if (((ValueType)42).GetHashCode() != 42) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.ValueType::ToString:System.String()
    public static void Subject_9()
    {
        if (((((ValueType)42).ToString()).Length) != 2) _exitCode = 1;
    }

}