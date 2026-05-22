// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/pinvoke/dllimport
// Assembly: System.Runtime.InteropServices
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

public static partial class PinvokeDllimportSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::get_Value:System.String()
    public static void Subject_0()
    {
        try { if (((default(DllImportAttribute)!.Value).Length) != ((default(DllImportAttribute)!.Value).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::.ctor:System.Void(System.String)
    public static void Subject_1()
    {
        try { new DllImportAttribute("hello"); }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_EntryPoint:System.String()
    public static void Subject_2()
    {
        try { if (((default(LibraryImportAttribute)!.EntryPoint).Length) != ((default(LibraryImportAttribute)!.EntryPoint).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_LibraryName:System.String()
    public static void Subject_3()
    {
        try { if (((default(LibraryImportAttribute)!.LibraryName).Length) != ((default(LibraryImportAttribute)!.LibraryName).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_SetLastError:System.Boolean()
    public static void Subject_4()
    {
        try { if (((default(LibraryImportAttribute)!.SetLastError) ? 1 : 0) != ((default(LibraryImportAttribute)!.SetLastError) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshalling:System.Runtime.InteropServices.StringMarshalling()
    public static void Subject_5()
    {
        try { if (((default(LibraryImportAttribute)!.StringMarshalling).GetHashCode()) != ((default(LibraryImportAttribute)!.StringMarshalling).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshallingCustomType:System.Type()
    public static void Subject_6()
    {
        try { if (((default(LibraryImportAttribute)!.StringMarshallingCustomType).GetHashCode()) != ((default(LibraryImportAttribute)!.StringMarshallingCustomType).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_EntryPoint:System.Void(System.String)
    public static void Subject_7()
    {
        try { default(LibraryImportAttribute)!.EntryPoint = "hello"; }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_SetLastError:System.Void(System.Boolean)
    public static void Subject_8()
    {
        try { default(LibraryImportAttribute)!.SetLastError = true; }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshalling:System.Void(System.Runtime.InteropServices.StringMarshalling)
    public static void Subject_9()
    {
        try { default(LibraryImportAttribute)!.StringMarshalling = default; }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshallingCustomType:System.Void(System.Type)
    public static void Subject_10()
    {
        try { default(LibraryImportAttribute)!.StringMarshallingCustomType = typeof(byte); }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::.ctor:System.Void(System.String)
    public static void Subject_11()
    {
        try { new LibraryImportAttribute("hello"); }
        catch { _exitCode = 1; }
    }

}