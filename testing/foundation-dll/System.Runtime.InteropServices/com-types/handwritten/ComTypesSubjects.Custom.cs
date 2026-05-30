// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class ComTypesSubjects
{
    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnClose:System.Void()
    [Fact]
    public static void CustomEntrySubject_0()
    {        try { default(IAdviseSink)!.OnClose(); } catch { }    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnDataChange:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    [Fact]
    public static void CustomEntrySubject_1()
    {        try { default(IAdviseSink)!.OnDataChange(out default, out default); } catch { }    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnRename:System.Void(System.Runtime.InteropServices.ComTypes.IMoniker)
    [Fact]
    public static void CustomEntrySubject_2()
    {        try { default(IAdviseSink)!.OnRename(default); } catch { }    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnSave:System.Void()
    [Fact]
    public static void CustomEntrySubject_3()
    {        try { default(IAdviseSink)!.OnSave(); } catch { }    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnViewChange:System.Void(System.Int32,System.Int32)
    [Fact]
    public static void CustomEntrySubject_4()
    {        try { default(IAdviseSink)!.OnViewChange(42, 42); } catch { }    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.ADVF,System.Runtime.InteropServices.ComTypes.IAdviseSink,System.Int32&)
    [Fact]
    public static void CustomEntrySubject_5()
    {        try { _ = default(IDataObject)!.DAdvise(out default, default, default, out 42); } catch { }    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DUnadvise:System.Void(System.Int32)
    [Fact]
    public static void CustomEntrySubject_6()
    {        try { default(IDataObject)!.DUnadvise(42); } catch { }    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumDAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&)
    [Fact]
    public static void CustomEntrySubject_7()
    {        try { _ = default(IDataObject)!.EnumDAdvise(out default); } catch { }    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumFormatEtc:System.Runtime.InteropServices.ComTypes.IEnumFORMATETC(System.Runtime.InteropServices.ComTypes.DATADIR)
    [Fact]
    public static void CustomEntrySubject_8()
    {        try { _ = default(IDataObject)!.EnumFormatEtc(default); } catch { }    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetCanonicalFormatEtc:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.FORMATETC&)
    [Fact]
    public static void CustomEntrySubject_9()
    {        try { _ = default(IDataObject)!.GetCanonicalFormatEtc(out default, out default); } catch { }    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    [Fact]
    public static void CustomEntrySubject_10()
    {        try { default(IDataObject)!.GetData(out default, out default); } catch { }    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetDataHere:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    [Fact]
    public static void CustomEntrySubject_11()
    {        try { default(IDataObject)!.GetDataHere(out default, out default); } catch { }    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::QueryGetData:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&)
    [Fact]
    public static void CustomEntrySubject_12()
    {        try { _ = default(IDataObject)!.QueryGetData(out default); } catch { }    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::SetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&,System.Boolean)
    [Fact]
    public static void CustomEntrySubject_13()
    {        try { default(IDataObject)!.SetData(out default, out default, true); } catch { }    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumFORMATETC&)
    [Fact]
    public static void CustomEntrySubject_14()
    {        try { default(IEnumFORMATETC)!.Clone(out default); } catch { }    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Next:System.Int32(System.Int32,System.Runtime.InteropServices.ComTypes.FORMATETC{},System.Int32{})
    [Fact]
    public static void CustomEntrySubject_15()
    {        try { _ = default(IEnumFORMATETC)!.Next(42, default, default); } catch { }    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Reset:System.Int32()
    [Fact]
    public static void CustomEntrySubject_16()
    {        try { _ = default(IEnumFORMATETC)!.Reset(); } catch { }    }

    // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Skip:System.Int32(System.Int32)
    [Fact]
    public static void CustomEntrySubject_17()
    {        try { _ = default(IEnumFORMATETC)!.Skip(42); } catch { }    }

    // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&)
    [Fact]
    public static void CustomEntrySubject_18()
    {        try { default(IEnumSTATDATA)!.Clone(out default); } catch { }    }

    // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Next:System.Int32(System.Int32,System.Runtime.InteropServices.ComTypes.STATDATA{},System.Int32{})
    [Fact]
    public static void CustomEntrySubject_19()
    {        try { _ = default(IEnumSTATDATA)!.Next(42, default, default); } catch { }    }

    // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Reset:System.Int32()
    [Fact]
    public static void CustomEntrySubject_20()
    {        try { _ = default(IEnumSTATDATA)!.Reset(); } catch { }    }

    // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Skip:System.Int32(System.Int32)
    [Fact]
    public static void CustomEntrySubject_21()
    {        try { _ = default(IEnumSTATDATA)!.Skip(42); } catch { }    }

}
