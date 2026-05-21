// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/runtime-interop-services
// Assembly: System.Runtime.InteropServices
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

public static partial class RuntimeInteropServicesNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_FrameworkDescription:System.String()
    public static void Method0()
    {
        try { if (((RuntimeInformation.FrameworkDescription).Length) != ((RuntimeInformation.FrameworkDescription).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_OSArchitecture:System.Runtime.InteropServices.Architecture()
    public static void Method1()
    {
        try { if ((int)(RuntimeInformation.OSArchitecture) != (int)(RuntimeInformation.OSArchitecture)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_OSDescription:System.String()
    public static void Method2()
    {
        try { if (((RuntimeInformation.OSDescription).Length) != ((RuntimeInformation.OSDescription).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_ProcessArchitecture:System.Runtime.InteropServices.Architecture()
    public static void Method3()
    {
        try { if ((int)(RuntimeInformation.ProcessArchitecture) != (int)(RuntimeInformation.ProcessArchitecture)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_RuntimeIdentifier:System.String()
    public static void Method4()
    {
        try { if (((RuntimeInformation.RuntimeIdentifier).Length) != ((RuntimeInformation.RuntimeIdentifier).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::IsOSPlatform:System.Boolean(System.Runtime.InteropServices.OSPlatform)
    public static void Method5()
    {
        try { if (((RuntimeInformation.IsOSPlatform(null!)) ? 1 : 0) != ((RuntimeInformation.IsOSPlatform(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::Create:System.Runtime.InteropServices.OSPlatform(System.String)
    public static void Method6()
    {
        try { if ((int)(OSPlatform.Create("hello")) != (int)(OSPlatform.Create("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::Equals:System.Boolean(System.Runtime.InteropServices.OSPlatform)
    public static void Method7()
    {
        try { if (((OSPlatform.Equals(null!)) ? 1 : 0) != ((OSPlatform.Equals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::Equals:System.Boolean(System.Object)
    public static void Method8()
    {
        try { if (((OSPlatform.Equals(42)) ? 1 : 0) != ((OSPlatform.Equals(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_FreeBSD:System.Runtime.InteropServices.OSPlatform()
    public static void Method9()
    {
        try { if ((int)(OSPlatform.FreeBSD) != (int)(OSPlatform.FreeBSD)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_Linux:System.Runtime.InteropServices.OSPlatform()
    public static void Method10()
    {
        try { if ((int)(OSPlatform.Linux) != (int)(OSPlatform.Linux)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_OSX:System.Runtime.InteropServices.OSPlatform()
    public static void Method11()
    {
        try { if ((int)(OSPlatform.OSX) != (int)(OSPlatform.OSX)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_Windows:System.Runtime.InteropServices.OSPlatform()
    public static void Method12()
    {
        try { if ((int)(OSPlatform.Windows) != (int)(OSPlatform.Windows)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::GetHashCode:System.Int32()
    public static void Method13()
    {
        try { if (OSPlatform.GetHashCode() != OSPlatform.GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::op_Equality:System.Boolean(System.Runtime.InteropServices.OSPlatform,System.Runtime.InteropServices.OSPlatform)
    public static void Method14()
    {
        try { if (((OSPlatform.op_Equality(null!, null!)) ? 1 : 0) != ((OSPlatform.op_Equality(null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::op_Inequality:System.Boolean(System.Runtime.InteropServices.OSPlatform,System.Runtime.InteropServices.OSPlatform)
    public static void Method15()
    {
        try { if (((OSPlatform.op_Inequality(null!, null!)) ? 1 : 0) != ((OSPlatform.op_Inequality(null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::ToString:System.String()
    public static void Method16()
    {
        try { if (((OSPlatform.ToString()).Length) != ((OSPlatform.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
                case 7: Method7(); break;
                case 8: Method8(); break;
                case 9: Method9(); break;
                case 10: Method10(); break;
                case 11: Method11(); break;
                case 12: Method12(); break;
                case 13: Method13(); break;
                case 14: Method14(); break;
                case 15: Method15(); break;
                case 16: Method16(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}