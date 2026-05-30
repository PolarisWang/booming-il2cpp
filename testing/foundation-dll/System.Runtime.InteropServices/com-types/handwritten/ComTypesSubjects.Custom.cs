// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class ComTypesSubjects
{
    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnDataChange:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    public static void CustomEntrySubject_1()
    {
        try {
            default(IAdviseSink)!.OnDataChange(out default, out default);
        }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.ADVF,System.Runtime.InteropServices.ComTypes.IAdviseSink,System.Int32&)
    public static void CustomEntrySubject_5()
    {
        try {
            Assert.IsNotNull(default(IDataObject)!.DAdvise(out default, default, default, out 42));
        }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumDAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&)
    public static void CustomEntrySubject_7()
    {
        try {
            Assert.IsNotNull(default(IDataObject)!.EnumDAdvise(out default));
        }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetCanonicalFormatEtc:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.FORMATETC&)
    public static void CustomEntrySubject_9()
    {
        try {
            Assert.IsNotNull(default(IDataObject)!.GetCanonicalFormatEtc(out default, out default));
        }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    public static void CustomEntrySubject_10()
    {
        try {
            default(IDataObject)!.GetData(out default, out default);
        }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetDataHere:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    public static void CustomEntrySubject_11()
    {
        try {
            default(IDataObject)!.GetDataHere(out default, out default);
        }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::QueryGetData:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&)
    public static void CustomEntrySubject_12()
    {
        try {
            Assert.IsNotNull(default(IDataObject)!.QueryGetData(out default));
        }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::SetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&,System.Boolean)
    public static void CustomEntrySubject_13()
    {
        try {
            default(IDataObject)!.SetData(out default, out default, true);
        }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumFORMATETC&)
    public static void CustomEntrySubject_14()
    {
        try {
            default(IEnumFORMATETC)!.Clone(out default);
        }
        catch { _exitCode = 1; }
    }

    // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&)
    public static void CustomEntrySubject_18()
    {
        try {
            default(IEnumSTATDATA)!.Clone(out default);
        }
        catch { _exitCode = 1; }
    }

}
