// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/generic/nullable/value
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class GenericNullableValueSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()
    public static void Subject_0()
    {
        if (((default(Nullable<int>).HasValue) ? 1 : 0) != ((default(Nullable<int>).HasValue) ? 1 : 0)) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Nullable`1::get_Value:T()
    public static void Subject_1()
    {
        // TODO: System.Private.CoreLib/System.Nullable`1::get_Value:T() could not be auto-generated
    }

    // [2] System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()
    public static void Subject_2()
    {
        if ((int)(default(Nullable<int>).GetValueOrDefault()) != (int)(default(Nullable<int>).GetValueOrDefault())) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)
    public static void Subject_3()
    {
        if ((int)(default(Nullable<int>).GetValueOrDefault(42)) != (int)(default(Nullable<int>).GetValueOrDefault(42))) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)
    public static void Subject_4()
    {
        if (((default(Nullable<int>).Equals(42)) ? 1 : 0) != ((default(Nullable<int>).Equals(42)) ? 1 : 0)) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()
    public static void Subject_5()
    {
        if (default(Nullable<int>).GetHashCode() != default(Nullable<int>).GetHashCode()) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Nullable`1::ToString:System.String()
    public static void Subject_6()
    {
        if (((default(Nullable<int>).ToString()).Length) != ((default(Nullable<int>).ToString()).Length)) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
    public static void Subject_7()
    {
        if (((((ValueType)42).Equals(42)) ? 1 : 0) != ((((ValueType)42).Equals(42)) ? 1 : 0)) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()
    public static void Subject_8()
    {
        if (((ValueType)42).GetHashCode() != ((ValueType)42).GetHashCode()) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.ValueType::ToString:System.String()
    public static void Subject_9()
    {
        if (((((ValueType)42).ToString()).Length) != ((((ValueType)42).ToString()).Length)) _exitCode = 1;
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
            case 9: Subject_9(); break;
        }
    }

}