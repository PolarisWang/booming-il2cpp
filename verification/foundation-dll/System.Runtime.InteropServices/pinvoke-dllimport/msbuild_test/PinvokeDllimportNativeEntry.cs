// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/pinvoke-dllimport
// Assembly: System.Runtime.InteropServices
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

public static partial class PinvokeDllimportNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::get_Value:System.String()
    public static void Method0()
    {
        try { if (((DllImportAttribute.Value).Length) != ((DllImportAttribute.Value).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::.ctor:System.Void(System.String)
    public static void Method1()
    {
        try { new DllImportAttribute("hello"); }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_EntryPoint:System.String()
    public static void Method2()
    {
        try { if (((LibraryImportAttribute.EntryPoint).Length) != ((LibraryImportAttribute.EntryPoint).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_LibraryName:System.String()
    public static void Method3()
    {
        try { if (((LibraryImportAttribute.LibraryName).Length) != ((LibraryImportAttribute.LibraryName).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_SetLastError:System.Boolean()
    public static void Method4()
    {
        try { if (((LibraryImportAttribute.SetLastError) ? 1 : 0) != ((LibraryImportAttribute.SetLastError) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshalling:System.Runtime.InteropServices.StringMarshalling()
    public static void Method5()
    {
        try { if ((int)(LibraryImportAttribute.StringMarshalling) != (int)(LibraryImportAttribute.StringMarshalling)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshallingCustomType:System.Type()
    public static void Method6()
    {
        try { if (((LibraryImportAttribute.StringMarshallingCustomType).GetHashCode()) != ((LibraryImportAttribute.StringMarshallingCustomType).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_EntryPoint:System.Void(System.String)
    public static void Method7()
    {
        try { LibraryImportAttribute.EntryPoint = "hello"; }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_SetLastError:System.Void(System.Boolean)
    public static void Method8()
    {
        try { LibraryImportAttribute.SetLastError = true; }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshalling:System.Void(System.Runtime.InteropServices.StringMarshalling)
    public static void Method9()
    {
        try { LibraryImportAttribute.StringMarshalling = null!; }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshallingCustomType:System.Void(System.Type)
    public static void Method10()
    {
        try { LibraryImportAttribute.StringMarshallingCustomType = typeof(byte); }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::.ctor:System.Void(System.String)
    public static void Method11()
    {
        try { new LibraryImportAttribute("hello"); }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}