// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/marshalling/attributes
// Assembly: System.Runtime.InteropServices
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

public static partial class MarshallingAttributesSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.StructLayoutAttribute::get_Value:System.Runtime.InteropServices.LayoutKind()
    public static void Subject_0()
    {
        try { _ = default(StructLayoutAttribute)!.Value; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.StructLayoutAttribute::.ctor:System.Void(System.Runtime.InteropServices.LayoutKind)
    public static void Subject_1()
    {
        new StructLayoutAttribute(LayoutKind.Auto);
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.StructLayoutAttribute::.ctor:System.Void(System.Int16)
    public static void Subject_2()
    {
        new StructLayoutAttribute(LayoutKind.Auto);
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::get_Value:System.Runtime.InteropServices.UnmanagedType()
    public static void Subject_3()
    {
        try { _ = default(MarshalAsAttribute)!.Value; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::.ctor:System.Void(System.Runtime.InteropServices.UnmanagedType)
    public static void Subject_4()
    {
        new MarshalAsAttribute(UnmanagedType.Bool);
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::.ctor:System.Void(System.Int16)
    public static void Subject_5()
    {
        new MarshalAsAttribute(UnmanagedType.Bool);
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.InAttribute::.ctor:System.Void()
    public static void Subject_6()
    {
        new InAttribute();
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.OutAttribute::.ctor:System.Void()
    public static void Subject_7()
    {
        new OutAttribute();
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.OptionalAttribute::.ctor:System.Void()
    public static void Subject_8()
    {
        new OptionalAttribute();
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.PreserveSigAttribute::.ctor:System.Void()
    public static void Subject_9()
    {
        new PreserveSigAttribute();
    }

}