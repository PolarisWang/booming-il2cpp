// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/com/types
// Assembly: System.Runtime.InteropServices
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.ComTypes;

public static partial class ComTypesSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnClose:System.Void()
    public static void Subject_0()
    {
        try { default(IAdviseSink)!.OnClose(); }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnDataChange:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    public static void Subject_1()
    {
        // needs-manual — OnDataChange with 2 params requires manual implementation: System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnDataChange:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnRename:System.Void(System.Runtime.InteropServices.ComTypes.IMoniker)
    public static void Subject_2()
    {
        try { default(IAdviseSink)!.OnRename(default); }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnSave:System.Void()
    public static void Subject_3()
    {
        try { default(IAdviseSink)!.OnSave(); }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnViewChange:System.Void(System.Int32,System.Int32)
    public static void Subject_4()
    {
        try { default(IAdviseSink)!.OnViewChange(42, 42); }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.ADVF,System.Runtime.InteropServices.ComTypes.IAdviseSink,System.Int32&)
    public static void Subject_5()
    {
        // needs-manual — DAdvise with 4 params requires manual implementation: System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.ADVF,System.Runtime.InteropServices.ComTypes.IAdviseSink,System.Int32&)
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DUnadvise:System.Void(System.Int32)
    public static void Subject_6()
    {
        try { default(IDataObject)!.DUnadvise(42); }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumDAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&)
    public static void Subject_7()
    {
        // needs-manual — EnumDAdvise with 1 params requires manual implementation: System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumDAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&)
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumFormatEtc:System.Runtime.InteropServices.ComTypes.IEnumFORMATETC(System.Runtime.InteropServices.ComTypes.DATADIR)
    public static void Subject_8()
    {
        try { if (((default(IDataObject)!.EnumFormatEtc(default)).GetHashCode()) != ((default(IDataObject)!.EnumFormatEtc(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetCanonicalFormatEtc:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.FORMATETC&)
    public static void Subject_9()
    {
        // needs-manual — GetCanonicalFormatEtc with 2 params requires manual implementation: System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetCanonicalFormatEtc:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.FORMATETC&)
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    public static void Subject_10()
    {
        // needs-manual — GetData with 2 params requires manual implementation: System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetDataHere:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    public static void Subject_11()
    {
        // needs-manual — GetDataHere with 2 params requires manual implementation: System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetDataHere:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::QueryGetData:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&)
    public static void Subject_12()
    {
        // needs-manual — QueryGetData with 1 params requires manual implementation: System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::QueryGetData:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&)
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::SetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&,System.Boolean)
    public static void Subject_13()
    {
        // needs-manual — SetData with 3 params requires manual implementation: System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::SetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&,System.Boolean)
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumFORMATETC&)
    public static void Subject_14()
    {
        // needs-manual — Clone with 1 params requires manual implementation: System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumFORMATETC&)
    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Next:System.Int32(System.Int32,System.Runtime.InteropServices.ComTypes.FORMATETC{},System.Int32{})
    public static void Subject_15()
    {
        try { if (default(IEnumFORMATETC)!.Next(42, default, default) != default(IEnumFORMATETC)!.Next(42, default, default)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Reset:System.Int32()
    public static void Subject_16()
    {
        try { if (default(IEnumFORMATETC)!.Reset() != default(IEnumFORMATETC)!.Reset()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Skip:System.Int32(System.Int32)
    public static void Subject_17()
    {
        try { if (default(IEnumFORMATETC)!.Skip(42) != default(IEnumFORMATETC)!.Skip(42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&)
    public static void Subject_18()
    {
        // needs-manual — Clone with 1 params requires manual implementation: System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&)
    }

    // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Next:System.Int32(System.Int32,System.Runtime.InteropServices.ComTypes.STATDATA{},System.Int32{})
    public static void Subject_19()
    {
        try { if (default(IEnumSTATDATA)!.Next(42, default, default) != default(IEnumSTATDATA)!.Next(42, default, default)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Reset:System.Int32()
    public static void Subject_20()
    {
        try { if (default(IEnumSTATDATA)!.Reset() != default(IEnumSTATDATA)!.Reset()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Skip:System.Int32(System.Int32)
    public static void Subject_21()
    {
        try { if (default(IEnumSTATDATA)!.Skip(42) != default(IEnumSTATDATA)!.Skip(42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}