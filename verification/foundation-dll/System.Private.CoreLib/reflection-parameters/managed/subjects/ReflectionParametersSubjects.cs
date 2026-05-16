// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/parameters
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Reflection;

public static partial class ReflectionParametersSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String()
    public static void Subject_0()
    {
        if (((default(ParameterInfo)!.Name).Length) != ((default(ParameterInfo)!.Name).Length)) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType:System.Type()
    public static void Subject_1()
    {
        if (((default(ParameterInfo)!.ParameterType).GetHashCode()) != ((default(ParameterInfo)!.ParameterType).GetHashCode())) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Reflection.ParameterInfo::get_DefaultValue:System.Object()
    public static void Subject_2()
    {
        if (((default(ParameterInfo)!.DefaultValue).GetHashCode()) != ((default(ParameterInfo)!.DefaultValue).GetHashCode())) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Reflection.ParameterInfo::get_RawDefaultValue:System.Object()
    public static void Subject_3()
    {
        if (((default(ParameterInfo)!.RawDefaultValue).GetHashCode()) != ((default(ParameterInfo)!.RawDefaultValue).GetHashCode())) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue:System.Boolean()
    public static void Subject_4()
    {
        if (((default(ParameterInfo)!.HasDefaultValue) ? 1 : 0) != ((default(ParameterInfo)!.HasDefaultValue) ? 1 : 0)) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Attributes:System.Reflection.ParameterAttributes()
    public static void Subject_5()
    {
        if ((int)(default(ParameterInfo)!.Attributes) != (int)(default(ParameterInfo)!.Attributes)) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Position:System.Int32()
    public static void Subject_6()
    {
        if (default(ParameterInfo)!.Position != default(ParameterInfo)!.Position) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Reflection.ParameterInfo::GetRequiredCustomModifiers:System.Type[]()
    public static void Subject_7()
    {
        if (((default(ParameterInfo)!.GetRequiredCustomModifiers()).Length) != ((default(ParameterInfo)!.GetRequiredCustomModifiers()).Length)) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Reflection.ParameterInfo::GetOptionalCustomModifiers:System.Type[]()
    public static void Subject_8()
    {
        if (((default(ParameterInfo)!.GetOptionalCustomModifiers()).Length) != ((default(ParameterInfo)!.GetOptionalCustomModifiers()).Length)) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]()
    public static void Subject_9()
    {
        if (((default(MethodInfo)!.GetParameters()).Length) != ((default(MethodInfo)!.GetParameters()).Length)) _exitCode = 1;
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