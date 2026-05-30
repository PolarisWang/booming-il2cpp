// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class ComTypesSubjects
{
    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnClose:System.Void()
    public static void CustomEntrySubject_0()
    {        try { default(IAdviseSink)!.OnClose(); } catch { _exitCode = 1; }    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnDataChange:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    public static void CustomEntrySubject_1()
    {        try { default(IAdviseSink)!.OnDataChange(out default, out default); } catch { _exitCode = 1; }    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnRename:System.Void(System.Runtime.InteropServices.ComTypes.IMoniker)
    public static void CustomEntrySubject_2()
    {        try { default(IAdviseSink)!.OnRename(default); } catch { _exitCode = 1; }    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnSave:System.Void()
    public static void CustomEntrySubject_3()
    {        try { default(IAdviseSink)!.OnSave(); } catch { _exitCode = 1; }    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnViewChange:System.Void(System.Int32,System.Int32)
    public static void CustomEntrySubject_4()
    {        try { default(IAdviseSink)!.OnViewChange(42, 42); } catch { _exitCode = 1; }    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.ADVF,System.Runtime.InteropServices.ComTypes.IAdviseSink,System.Int32&)
    public static void CustomEntrySubject_5()
    {        try { _ = default(IDataObject)!.DAdvise(out default, default, default, out 42); } catch { _exitCode = 1; }    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DUnadvise:System.Void(System.Int32)
    public static void CustomEntrySubject_6()
    {        try { default(IDataObject)!.DUnadvise(42); } catch { _exitCode = 1; }    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumDAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&)
    public static void CustomEntrySubject_7()
    {        try { _ = default(IDataObject)!.EnumDAdvise(out default); } catch { _exitCode = 1; }    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumFormatEtc:System.Runtime.InteropServices.ComTypes.IEnumFORMATETC(System.Runtime.InteropServices.ComTypes.DATADIR)
    public static void CustomEntrySubject_8()
    {        try { _ = default(IDataObject)!.EnumFormatEtc(default); } catch { _exitCode = 1; }    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetCanonicalFormatEtc:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.FORMATETC&)
    public static void CustomEntrySubject_9()
    {        try { _ = default(IDataObject)!.GetCanonicalFormatEtc(out default, out default); } catch { _exitCode = 1; }    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    public static void CustomEntrySubject_10()
    {        try { default(IDataObject)!.GetData(out default, out default); } catch { _exitCode = 1; }    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetDataHere:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    public static void CustomEntrySubject_11()
    {        try { default(IDataObject)!.GetDataHere(out default, out default); } catch { _exitCode = 1; }    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::QueryGetData:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&)
    public static void CustomEntrySubject_12()
    {        try { _ = default(IDataObject)!.QueryGetData(out default); } catch { _exitCode = 1; }    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::SetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&,System.Boolean)
    public static void CustomEntrySubject_13()
    {        try { default(IDataObject)!.SetData(out default, out default, true); } catch { _exitCode = 1; }    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumFORMATETC&)
    public static void CustomEntrySubject_14()
    {        try { default(IEnumFORMATETC)!.Clone(out default); } catch { _exitCode = 1; }    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Next:System.Int32(System.Int32,System.Runtime.InteropServices.ComTypes.FORMATETC{},System.Int32{})
    public static void CustomEntrySubject_15()
    {        try { _ = default(IEnumFORMATETC)!.Next(42, default, default); } catch { _exitCode = 1; }    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Reset:System.Int32()
    public static void CustomEntrySubject_16()
    {        try { _ = default(IEnumFORMATETC)!.Reset(); } catch { _exitCode = 1; }    }

    // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Skip:System.Int32(System.Int32)
    public static void CustomEntrySubject_17()
    {        try { _ = default(IEnumFORMATETC)!.Skip(42); } catch { _exitCode = 1; }    }

    // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&)
    public static void CustomEntrySubject_18()
    {        try { default(IEnumSTATDATA)!.Clone(out default); } catch { _exitCode = 1; }    }

    // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Next:System.Int32(System.Int32,System.Runtime.InteropServices.ComTypes.STATDATA{},System.Int32{})
    public static void CustomEntrySubject_19()
    {        try { _ = default(IEnumSTATDATA)!.Next(42, default, default); } catch { _exitCode = 1; }    }

    // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Reset:System.Int32()
    public static void CustomEntrySubject_20()
    {        try { _ = default(IEnumSTATDATA)!.Reset(); } catch { _exitCode = 1; }    }

    // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Skip:System.Int32(System.Int32)
    public static void CustomEntrySubject_21()
    {        try { _ = default(IEnumSTATDATA)!.Skip(42); } catch { _exitCode = 1; }    }

}
