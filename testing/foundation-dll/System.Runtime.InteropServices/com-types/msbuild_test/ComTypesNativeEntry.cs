// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/com-types
// Assembly: System.Runtime.InteropServices
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.ComTypes;

public static partial class ComTypesNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnClose:System.Void()
    public static void Method0()
    {
        try { IAdviseSink.OnClose(); }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnDataChange:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    public static void Method1()
    {
    var refLocal_0 = null!;
    var refLocal_1 = null!;
        try { IAdviseSink.OnDataChange(ref refLocal_0, ref refLocal_1); }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnRename:System.Void(System.Runtime.InteropServices.ComTypes.IMoniker)
    public static void Method2()
    {
        try { IAdviseSink.OnRename(null!); }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnSave:System.Void()
    public static void Method3()
    {
        try { IAdviseSink.OnSave(); }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnViewChange:System.Void(System.Int32,System.Int32)
    public static void Method4()
    {
        try { IAdviseSink.OnViewChange(42, 42); }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.ADVF,System.Runtime.InteropServices.ComTypes.IAdviseSink,System.Int32&)
    public static void Method5()
    {
    var refLocal_0 = null!;
    var refLocal_3 = 42;
        try { if (IDataObject.DAdvise(ref refLocal_0, null!, null!, ref refLocal_3) != IDataObject.DAdvise(ref refLocal_0, null!, null!, ref refLocal_3)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DUnadvise:System.Void(System.Int32)
    public static void Method6()
    {
        try { IDataObject.DUnadvise(42); }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumDAdvise:System.Int32(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&)
    public static void Method7()
    {
    var refLocal_0 = null!;
        try { if (IDataObject.EnumDAdvise(ref refLocal_0) != IDataObject.EnumDAdvise(ref refLocal_0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumFormatEtc:System.Runtime.InteropServices.ComTypes.IEnumFORMATETC(System.Runtime.InteropServices.ComTypes.DATADIR)
    public static void Method8()
    {
        try { if ((int)(IDataObject.EnumFormatEtc(null!)) != (int)(IDataObject.EnumFormatEtc(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetCanonicalFormatEtc:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.FORMATETC&)
    public static void Method9()
    {
    var refLocal_0 = null!;
    var refLocal_1 = null!;
        try { if (IDataObject.GetCanonicalFormatEtc(ref refLocal_0, ref refLocal_1) != IDataObject.GetCanonicalFormatEtc(ref refLocal_0, ref refLocal_1)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    public static void Method10()
    {
    var refLocal_0 = null!;
    var refLocal_1 = null!;
        try { IDataObject.GetData(ref refLocal_0, ref refLocal_1); }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetDataHere:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&)
    public static void Method11()
    {
    var refLocal_0 = null!;
    var refLocal_1 = null!;
        try { IDataObject.GetDataHere(ref refLocal_0, ref refLocal_1); }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::QueryGetData:System.Int32(System.Runtime.InteropServices.ComTypes.FORMATETC&)
    public static void Method12()
    {
    var refLocal_0 = null!;
        try { if (IDataObject.QueryGetData(ref refLocal_0) != IDataObject.QueryGetData(ref refLocal_0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::SetData:System.Void(System.Runtime.InteropServices.ComTypes.FORMATETC&,System.Runtime.InteropServices.ComTypes.STGMEDIUM&,System.Boolean)
    public static void Method13()
    {
    var refLocal_0 = null!;
    var refLocal_1 = null!;
        try { IDataObject.SetData(ref refLocal_0, ref refLocal_1, true); }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumFORMATETC&)
    public static void Method14()
    {
    var refLocal_0 = null!;
        try { IEnumFORMATETC.Clone(ref refLocal_0); }
        catch { _exitCode = 1; }
    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Next:System.Int32(System.Int32,System.Runtime.InteropServices.ComTypes.FORMATETC{},System.Int32{})
    public static void Method15()
    {
        try { if (IEnumFORMATETC.Next(42, null!, null!) != IEnumFORMATETC.Next(42, null!, null!)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Reset:System.Int32()
    public static void Method16()
    {
        try { if (IEnumFORMATETC.Reset() != IEnumFORMATETC.Reset()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Skip:System.Int32(System.Int32)
    public static void Method17()
    {
        try { if (IEnumFORMATETC.Skip(42) != IEnumFORMATETC.Skip(42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Clone:System.Void(System.Runtime.InteropServices.ComTypes.IEnumSTATDATA&)
    public static void Method18()
    {
    var refLocal_0 = null!;
        try { IEnumSTATDATA.Clone(ref refLocal_0); }
        catch { _exitCode = 1; }
    }

    // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Next:System.Int32(System.Int32,System.Runtime.InteropServices.ComTypes.STATDATA{},System.Int32{})
    public static void Method19()
    {
        try { if (IEnumSTATDATA.Next(42, null!, null!) != IEnumSTATDATA.Next(42, null!, null!)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Reset:System.Int32()
    public static void Method20()
    {
        try { if (IEnumSTATDATA.Reset() != IEnumSTATDATA.Reset()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Skip:System.Int32(System.Int32)
    public static void Method21()
    {
        try { if (IEnumSTATDATA.Skip(42) != IEnumSTATDATA.Skip(42)) _exitCode = 1; }
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
                case 12: Method12(); break;
                case 13: Method13(); break;
                case 14: Method14(); break;
                case 15: Method15(); break;
                case 16: Method16(); break;
                case 17: Method17(); break;
                case 18: Method18(); break;
                case 19: Method19(); break;
                case 20: Method20(); break;
                case 21: Method21(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}