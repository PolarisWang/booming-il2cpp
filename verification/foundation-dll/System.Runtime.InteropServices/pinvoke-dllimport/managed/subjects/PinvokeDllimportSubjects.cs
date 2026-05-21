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
        try { _ = default(DllImportAttribute)!.Value; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::.ctor:System.Void(System.String)
    public static void Subject_1()
    {
        new DllImportAttribute("hello");
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_EntryPoint:System.String()
    public static void Subject_2()
    {
        try { _ = default(LibraryImportAttribute)!.EntryPoint; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_LibraryName:System.String()
    public static void Subject_3()
    {
        try { _ = default(LibraryImportAttribute)!.LibraryName; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_SetLastError:System.Boolean()
    public static void Subject_4()
    {
        try { _ = default(LibraryImportAttribute)!.SetLastError; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshalling:System.Runtime.InteropServices.StringMarshalling()
    public static void Subject_5()
    {
        try { _ = default(LibraryImportAttribute)!.StringMarshalling; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshallingCustomType:System.Type()
    public static void Subject_6()
    {
        try { _ = default(LibraryImportAttribute)!.StringMarshallingCustomType; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_EntryPoint:System.Void(System.String)
    public static void Subject_7()
    {
        try { _ = default(LibraryImportAttribute)!.EntryPoint = "hello"; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_SetLastError:System.Void(System.Boolean)
    public static void Subject_8()
    {
        try { _ = default(LibraryImportAttribute)!.SetLastError = true; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshalling:System.Void(System.Runtime.InteropServices.StringMarshalling)
    public static void Subject_9()
    {
        try { _ = default(LibraryImportAttribute)!.StringMarshalling = default; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshallingCustomType:System.Void(System.Type)
    public static void Subject_10()
    {
        try { default(LibraryImportAttribute)!.StringMarshallingCustomType = typeof(byte); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::.ctor:System.Void(System.String)
    public static void Subject_11()
    {
        new LibraryImportAttribute("hello");
    }

}