// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/marshalling-attributes
// Assembly: System.Runtime.InteropServices
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

public static partial class MarshallingAttributesNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.StructLayoutAttribute::get_Value:System.Runtime.InteropServices.LayoutKind()
    public static void Method0()
    {
        try { if ((int)(StructLayoutAttribute.Value) != (int)(StructLayoutAttribute.Value)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.StructLayoutAttribute::.ctor:System.Void(System.Runtime.InteropServices.LayoutKind)
    public static void Method1()
    {
        try { new StructLayoutAttribute(null!); }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.StructLayoutAttribute::.ctor:System.Void(System.Int16)
    public static void Method2()
    {
        try { new StructLayoutAttribute((short)42); }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::get_Value:System.Runtime.InteropServices.UnmanagedType()
    public static void Method3()
    {
        try { if ((int)(MarshalAsAttribute.Value) != (int)(MarshalAsAttribute.Value)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::.ctor:System.Void(System.Runtime.InteropServices.UnmanagedType)
    public static void Method4()
    {
        try { new MarshalAsAttribute(null!); }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::.ctor:System.Void(System.Int16)
    public static void Method5()
    {
        try { new MarshalAsAttribute((short)42); }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.InAttribute::.ctor:System.Void()
    public static void Method6()
    {
        try { new InAttribute(); }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.OutAttribute::.ctor:System.Void()
    public static void Method7()
    {
        try { new OutAttribute(); }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.OptionalAttribute::.ctor:System.Void()
    public static void Method8()
    {
        try { new OptionalAttribute(); }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.PreserveSigAttribute::.ctor:System.Void()
    public static void Method9()
    {
        try { new PreserveSigAttribute(); }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}