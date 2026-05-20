// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/assembly
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ReflectionAssemblyPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly:System.Reflection.Assembly()
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly:System.Reflection.Assembly()
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Private.CoreLib/System.Reflection.Assembly::get_Location:System.String()
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion:System.String()
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes:System.Type[]()
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes:System.Type[]()
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)
    public static int Subject_10()
    {
        return unchecked((int)(0xB0000000u + 10));
    }

    // [11] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String,System.Boolean)
    public static int Subject_11()
    {
        return unchecked((int)(0xB0000000u + 11));
    }

    // [12] System.Private.CoreLib/System.Reflection.Assembly::GetModule:System.Reflection.Module(System.String)
    public static int Subject_12()
    {
        return unchecked((int)(0xB0000000u + 12));
    }

    // [13] System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()
    public static int Subject_13()
    {
        return unchecked((int)(0xB0000000u + 13));
    }

    // [14] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream:System.IO.Stream(System.String)
    public static int Subject_14()
    {
        return unchecked((int)(0xB0000000u + 14));
    }

    // [15] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames:System.String[]()
    public static int Subject_15()
    {
        return unchecked((int)(0xB0000000u + 15));
    }

    // [16] System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint:System.Reflection.MethodInfo()
    public static int Subject_16()
    {
        return unchecked((int)(0xB0000000u + 16));
    }

    // [17] System.Private.CoreLib/System.Reflection.AssemblyName::get_Name:System.String()
    public static int Subject_17()
    {
        return unchecked((int)(0xB0000000u + 17));
    }

    // [18] System.Private.CoreLib/System.Reflection.AssemblyName::get_Version:System.Version()
    public static int Subject_18()
    {
        return unchecked((int)(0xB0000000u + 18));
    }

    // [19] System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo:System.Globalization.CultureInfo()
    public static int Subject_19()
    {
        return unchecked((int)(0xB0000000u + 19));
    }

    // [20] System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()
    public static int Subject_20()
    {
        return unchecked((int)(0xB0000000u + 20));
    }

    // [21] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)

    public static void Run(int entryIndex)
    {
        try
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
                case 10: Subject_10(); break;
                case 11: Subject_11(); break;
                case 12: Subject_12(); break;
                case 13: Subject_13(); break;
                case 14: Subject_14(); break;
                case 15: Subject_15(); break;
                case 16: Subject_16(); break;
                case 17: Subject_17(); break;
                case 18: Subject_18(); break;
                case 19: Subject_19(); break;
                case 20: Subject_20(); break;
                case 21: CustomEntryMethod21(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}