// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/runtime/interop/services
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class RuntimeInteropServicesPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_FrameworkDescription:System.String()
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_OSArchitecture:System.Runtime.InteropServices.Architecture()
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_OSDescription:System.String()
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_ProcessArchitecture:System.Runtime.InteropServices.Architecture()
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_RuntimeIdentifier:System.String()
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::IsOSPlatform:System.Boolean(System.Runtime.InteropServices.OSPlatform)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::Create:System.Runtime.InteropServices.OSPlatform(System.String)
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::Equals:System.Boolean(System.Runtime.InteropServices.OSPlatform)
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::Equals:System.Boolean(System.Object)
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_FreeBSD:System.Runtime.InteropServices.OSPlatform()
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_Linux:System.Runtime.InteropServices.OSPlatform()
    public static int Subject_10()
    {
        return unchecked((int)(0xB0000000u + 10));
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_OSX:System.Runtime.InteropServices.OSPlatform()
    public static int Subject_11()
    {
        return unchecked((int)(0xB0000000u + 11));
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_Windows:System.Runtime.InteropServices.OSPlatform()
    public static int Subject_12()
    {
        return unchecked((int)(0xB0000000u + 12));
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::GetHashCode:System.Int32()
    public static int Subject_13()
    {
        return unchecked((int)(0xB0000000u + 13));
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::op_Equality:System.Boolean(System.Runtime.InteropServices.OSPlatform,System.Runtime.InteropServices.OSPlatform)
    public static int Subject_14()
    {
        return unchecked((int)(0xB0000000u + 14));
    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::op_Inequality:System.Boolean(System.Runtime.InteropServices.OSPlatform,System.Runtime.InteropServices.OSPlatform)
    public static int Subject_15()
    {
        return unchecked((int)(0xB0000000u + 15));
    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::ToString:System.String()
    public static int Subject_16()
    {
        return unchecked((int)(0xB0000000u + 16));
    }

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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}