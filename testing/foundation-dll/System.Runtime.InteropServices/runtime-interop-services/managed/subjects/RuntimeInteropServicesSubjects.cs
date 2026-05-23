// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/runtime/interop/services
// Assembly: System.Runtime.InteropServices
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

public static partial class RuntimeInteropServicesSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_FrameworkDescription:System.String()
    public static void Subject_0()
    {
        try { if (((RuntimeInformation.FrameworkDescription).Length) != ((RuntimeInformation.FrameworkDescription).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_OSArchitecture:System.Runtime.InteropServices.Architecture()
    public static void Subject_1()
    {
        try { if (((RuntimeInformation.OSArchitecture).GetHashCode()) != ((RuntimeInformation.OSArchitecture).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_OSDescription:System.String()
    public static void Subject_2()
    {
        try { if (((RuntimeInformation.OSDescription).Length) != ((RuntimeInformation.OSDescription).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_ProcessArchitecture:System.Runtime.InteropServices.Architecture()
    public static void Subject_3()
    {
        try { if (((RuntimeInformation.ProcessArchitecture).GetHashCode()) != ((RuntimeInformation.ProcessArchitecture).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_RuntimeIdentifier:System.String()
    public static void Subject_4()
    {
        try { if (((RuntimeInformation.RuntimeIdentifier).Length) != ((RuntimeInformation.RuntimeIdentifier).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::IsOSPlatform:System.Boolean(System.Runtime.InteropServices.OSPlatform)
    public static void Subject_5()
    {
        try { if (((RuntimeInformation.IsOSPlatform(default)) ? 1 : 0) != ((RuntimeInformation.IsOSPlatform(default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::Create:System.Runtime.InteropServices.OSPlatform(System.String)
    public static void Subject_6()
    {
        try { if (((OSPlatform.Create("hello")).GetHashCode()) != ((OSPlatform.Create("hello")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::Equals:System.Boolean(System.Runtime.InteropServices.OSPlatform)
    public static void Subject_7()
    {
        try { if (((default(OSPlatform).Equals(default)) ? 1 : 0) != ((default(OSPlatform).Equals(default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::Equals:System.Boolean(System.Object)
    public static void Subject_8()
    {
        try { if (((default(OSPlatform).Equals(null!)) ? 1 : 0) != ((default(OSPlatform).Equals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_FreeBSD:System.Runtime.InteropServices.OSPlatform()
    public static void Subject_9()
    {
        try { if (((OSPlatform.FreeBSD).GetHashCode()) != ((OSPlatform.FreeBSD).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_Linux:System.Runtime.InteropServices.OSPlatform()
    public static void Subject_10()
    {
        try { if (((OSPlatform.Linux).GetHashCode()) != ((OSPlatform.Linux).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_OSX:System.Runtime.InteropServices.OSPlatform()
    public static void Subject_11()
    {
        try { if (((OSPlatform.OSX).GetHashCode()) != ((OSPlatform.OSX).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_Windows:System.Runtime.InteropServices.OSPlatform()
    public static void Subject_12()
    {
        try { if (((OSPlatform.Windows).GetHashCode()) != ((OSPlatform.Windows).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::GetHashCode:System.Int32()
    public static void Subject_13()
    {
        try { if (default(OSPlatform).GetHashCode() != default(OSPlatform).GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::op_Equality:System.Boolean(System.Runtime.InteropServices.OSPlatform,System.Runtime.InteropServices.OSPlatform)
    public static void Subject_14()
    {
        try { if (((default(OSPlatform) == default(OSPlatform)) ? 1 : 0) != ((default(OSPlatform) == default(OSPlatform)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::op_Inequality:System.Boolean(System.Runtime.InteropServices.OSPlatform,System.Runtime.InteropServices.OSPlatform)
    public static void Subject_15()
    {
        try { if (((default(OSPlatform) != default(OSPlatform)) ? 1 : 0) != ((default(OSPlatform) != default(OSPlatform)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::ToString:System.String()
    public static void Subject_16()
    {
        try { if (((default(OSPlatform).ToString()).Length) != ((default(OSPlatform).ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}