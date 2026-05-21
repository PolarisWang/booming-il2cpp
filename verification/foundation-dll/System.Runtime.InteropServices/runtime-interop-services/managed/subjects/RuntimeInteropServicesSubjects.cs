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
        if (((RuntimeInformation.FrameworkDescription).Length) != 11) _exitCode = 1;
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_OSArchitecture:System.Runtime.InteropServices.Architecture()
    public static void Subject_1()
    {
        if (((RuntimeInformation.OSArchitecture).GetHashCode()) != 1) _exitCode = 1;
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_OSDescription:System.String()
    public static void Subject_2()
    {
        if (((RuntimeInformation.OSDescription).Length) != 28) _exitCode = 1;
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_ProcessArchitecture:System.Runtime.InteropServices.Architecture()
    public static void Subject_3()
    {
        if (((RuntimeInformation.ProcessArchitecture).GetHashCode()) != 1) _exitCode = 1;
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::get_RuntimeIdentifier:System.String()
    public static void Subject_4()
    {
        if (((RuntimeInformation.RuntimeIdentifier).Length) != 7) _exitCode = 1;
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.RuntimeInformation::IsOSPlatform:System.Boolean(System.Runtime.InteropServices.OSPlatform)
    public static void Subject_5()
    {
        try { RuntimeInformation.IsOSPlatform(default); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::Create:System.Runtime.InteropServices.OSPlatform(System.String)
    public static void Subject_6()
    {
        if (((OSPlatform.Create("hello")).GetHashCode()) != -1619787047) _exitCode = 1;
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::Equals:System.Boolean(System.Runtime.InteropServices.OSPlatform)
    public static void Subject_7()
    {
        if (((default(OSPlatform).Equals(default)) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::Equals:System.Boolean(System.Object)
    public static void Subject_8()
    {
        if (((default(OSPlatform).Equals(42)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_FreeBSD:System.Runtime.InteropServices.OSPlatform()
    public static void Subject_9()
    {
        if (((OSPlatform.FreeBSD).GetHashCode()) != 1386618053) _exitCode = 1;
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_Linux:System.Runtime.InteropServices.OSPlatform()
    public static void Subject_10()
    {
        if (((OSPlatform.Linux).GetHashCode()) != 1163975557) _exitCode = 1;
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_OSX:System.Runtime.InteropServices.OSPlatform()
    public static void Subject_11()
    {
        if (((OSPlatform.OSX).GetHashCode()) != 1995954951) _exitCode = 1;
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::get_Windows:System.Runtime.InteropServices.OSPlatform()
    public static void Subject_12()
    {
        if (((OSPlatform.Windows).GetHashCode()) != 1143014564) _exitCode = 1;
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::GetHashCode:System.Int32()
    public static void Subject_13()
    {
        if (default(OSPlatform).GetHashCode() != 0) _exitCode = 1;
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::op_Equality:System.Boolean(System.Runtime.InteropServices.OSPlatform,System.Runtime.InteropServices.OSPlatform)
    public static void Subject_14()
    {
        if (((default(OSPlatform) == default(OSPlatform)) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::op_Inequality:System.Boolean(System.Runtime.InteropServices.OSPlatform,System.Runtime.InteropServices.OSPlatform)
    public static void Subject_15()
    {
        if (((default(OSPlatform) != default(OSPlatform)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.OSPlatform::ToString:System.String()
    public static void Subject_16()
    {
        if (((default(OSPlatform).ToString()).Length) != 0) _exitCode = 1;
    }

}