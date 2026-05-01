using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Auto-generated hot-update skeletons for FamilySystem.runtime.interopservicesComTypesHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.runtime.interopservicesComTypesHotUpdate
{
    // Purpose: Verify IAdviseSink.OnClose executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnClose:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IAdviseSink_OnClose_System_Void() { IAdviseSink.OnClose(); }
    // Purpose: Verify IAdviseSink.OnDataChange executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnDataChange:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IAdviseSink_OnDataChange_System_Void_P0_P1() { IAdviseSink.OnDataChange(null!, null!); }
    // Purpose: Verify IAdviseSink.OnRename executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnRename:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IAdviseSink_OnRename_System_Void_P0() { IAdviseSink.OnRename(null!); }
    // Purpose: Verify IAdviseSink.OnSave executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnSave:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IAdviseSink_OnSave_System_Void() { IAdviseSink.OnSave(); }
    // Purpose: Verify IAdviseSink.OnViewChange executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnViewChange:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IAdviseSink_OnViewChange_System_Void_P0_P1() { IAdviseSink.OnViewChange(null!, null!); }
    // Purpose: Verify IBindCtx.EnumObjectParam executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::EnumObjectParam:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_EnumObjectParam_System_Void_P0() { IBindCtx.EnumObjectParam(null!); }
    // Purpose: Verify IBindCtx.GetBindOptions executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::GetBindOptions:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_GetBindOptions_System_Void_P0() { IBindCtx.GetBindOptions(null!); }
    // Purpose: Verify IBindCtx.GetObjectParam executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::GetObjectParam:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_GetObjectParam_System_Void_P0_P1() { IBindCtx.GetObjectParam(null!, null!); }
    // Purpose: Verify IBindCtx.GetRunningObjectTable executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::GetRunningObjectTable:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_GetRunningObjectTable_System_Void_P0() { IBindCtx.GetRunningObjectTable(null!); }
    // Purpose: Verify IBindCtx.RegisterObjectBound executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::RegisterObjectBound:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_RegisterObjectBound_System_Void_P0() { IBindCtx.RegisterObjectBound(null!); }
    // Purpose: Verify IBindCtx.RegisterObjectParam executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::RegisterObjectParam:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_RegisterObjectParam_System_Void_P0_P1() { IBindCtx.RegisterObjectParam(null!, null!); }
    // Purpose: Verify IBindCtx.ReleaseBoundObjects executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::ReleaseBoundObjects:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_ReleaseBoundObjects_System_Void() { IBindCtx.ReleaseBoundObjects(); }
    // Purpose: Verify IBindCtx.RevokeObjectBound executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::RevokeObjectBound:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_RevokeObjectBound_System_Void_P0() { IBindCtx.RevokeObjectBound(null!); }
    // Purpose: Verify IBindCtx.RevokeObjectParam executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::RevokeObjectParam:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_RevokeObjectParam_System_Void_P0() { IBindCtx.RevokeObjectParam(null!); }
    // Purpose: Verify IBindCtx.SetBindOptions executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::SetBindOptions:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_SetBindOptions_System_Void_P0() { IBindCtx.SetBindOptions(null!); }
    // Purpose: Verify IConnectionPoint.Advise executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::Advise:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IConnectionPoint_Advise_System_Void_P0_P1() { IConnectionPoint.Advise(null!, null!); }
    // Purpose: Verify IConnectionPoint.EnumConnections executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::EnumConnections:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IConnectionPoint_EnumConnections_System_Void_P0() { IConnectionPoint.EnumConnections(null!); }
    // Purpose: Verify IConnectionPoint.GetConnectionInterface executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::GetConnectionInterface:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IConnectionPoint_GetConnectionInterface_System_Vo() { IConnectionPoint.GetConnectionInterface(null!); }
    // Purpose: Verify IConnectionPoint.GetConnectionPointContainer executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::GetConnectionPointContainer:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IConnectionPoint_GetConnectionPointContainer_Syst() { IConnectionPoint.GetConnectionPointContainer(null!); }
    // Purpose: Verify IConnectionPoint.Unadvise executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::Unadvise:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IConnectionPoint_Unadvise_System_Void_P0() { IConnectionPoint.Unadvise(null!); }
    // Purpose: Verify IConnectionPointContainer.EnumConnectionPoints executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPointContainer::EnumConnectionPoints:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IConnectionPointContainer_EnumConnectionPoints_Sy() { IConnectionPointContainer.EnumConnectionPoints(null!); }
    // Purpose: Verify IConnectionPointContainer.FindConnectionPoint executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPointContainer::FindConnectionPoint:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IConnectionPointContainer_FindConnectionPoint_Sys() { IConnectionPointContainer.FindConnectionPoint(null!, null!); }
    // Purpose: Verify IDataObject.DAdvise executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DAdvise:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_DAdvise_System_Void_P0_P1_P2_P3() { IDataObject.DAdvise(null!, null!, null!, null!); }
    // Purpose: Verify IDataObject.DUnadvise executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DUnadvise:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_DUnadvise_System_Void_P0() { IDataObject.DUnadvise(null!); }
    // Purpose: Verify IDataObject.EnumDAdvise executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumDAdvise:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_EnumDAdvise_System_Void_P0() { IDataObject.EnumDAdvise(null!); }
    // Purpose: Verify IDataObject.EnumFormatEtc executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumFormatEtc:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_EnumFormatEtc_System_Void_P0() { IDataObject.EnumFormatEtc(null!); }
    // Purpose: Verify IDataObject.GetCanonicalFormatEtc executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetCanonicalFormatEtc:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_GetCanonicalFormatEtc_System_Void_P0_() { IDataObject.GetCanonicalFormatEtc(null!, null!); }
    // Purpose: Verify IDataObject.GetData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_GetData_System_Void_P0_P1() { IDataObject.GetData(null!, null!); }
    // Purpose: Verify IDataObject.GetDataHere executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetDataHere:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_GetDataHere_System_Void_P0_P1() { IDataObject.GetDataHere(null!, null!); }
    // Purpose: Verify IDataObject.QueryGetData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::QueryGetData:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_QueryGetData_System_Void_P0() { IDataObject.QueryGetData(null!); }
    // Purpose: Verify IDataObject.SetData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::SetData:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_SetData_System_Void_P0_P1_P2() { IDataObject.SetData(null!, null!, null!); }
    // Purpose: Verify IEnumConnectionPoints.Clone executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnectionPoints::Clone:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnectionPoints_Clone_System_Void_P0() { IEnumConnectionPoints.Clone(null!); }
    // Purpose: Verify IEnumConnectionPoints.Next executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnectionPoints::Next:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnectionPoints_Next_System_Void_P0_P1_P2() { IEnumConnectionPoints.Next(null!, null!, null!); }
    // Purpose: Verify IEnumConnectionPoints.Reset executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnectionPoints::Reset:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnectionPoints_Reset_System_Void() { IEnumConnectionPoints.Reset(); }
    // Purpose: Verify IEnumConnectionPoints.Skip executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnectionPoints::Skip:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnectionPoints_Skip_System_Void_P0() { IEnumConnectionPoints.Skip(null!); }
    // Purpose: Verify IEnumConnections.Clone executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnections::Clone:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnections_Clone_System_Void_P0() { IEnumConnections.Clone(null!); }
    // Purpose: Verify IEnumConnections.Next executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnections::Next:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnections_Next_System_Void_P0_P1_P2() { IEnumConnections.Next(null!, null!, null!); }
    // Purpose: Verify IEnumConnections.Reset executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnections::Reset:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnections_Reset_System_Void() { IEnumConnections.Reset(); }
    // Purpose: Verify IEnumConnections.Skip executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnections::Skip:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnections_Skip_System_Void_P0() { IEnumConnections.Skip(null!); }
    // Purpose: Verify IEnumFORMATETC.Clone executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Clone:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumFORMATETC_Clone_System_Void_P0() { IEnumFORMATETC.Clone(null!); }
    // Purpose: Verify IEnumFORMATETC.Next executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Next:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumFORMATETC_Next_System_Void_P0_P1_P2() { IEnumFORMATETC.Next(null!, null!, null!); }
    // Purpose: Verify IEnumFORMATETC.Reset executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Reset:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumFORMATETC_Reset_System_Void() { IEnumFORMATETC.Reset(); }
    // Purpose: Verify IEnumFORMATETC.Skip executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Skip:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumFORMATETC_Skip_System_Void_P0() { IEnumFORMATETC.Skip(null!); }
    // Purpose: Verify IEnumMoniker.Clone executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumMoniker::Clone:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumMoniker_Clone_System_Void_P0() { IEnumMoniker.Clone(null!); }
    // Purpose: Verify IEnumMoniker.Next executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumMoniker::Next:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumMoniker_Next_System_Void_P0_P1_P2() { IEnumMoniker.Next(null!, null!, null!); }
    // Purpose: Verify IEnumMoniker.Reset executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumMoniker::Reset:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumMoniker_Reset_System_Void() { IEnumMoniker.Reset(); }
    // Purpose: Verify IEnumMoniker.Skip executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumMoniker::Skip:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumMoniker_Skip_System_Void_P0() { IEnumMoniker.Skip(null!); }
    // Purpose: Verify IEnumSTATDATA.Clone executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Clone:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumSTATDATA_Clone_System_Void_P0() { IEnumSTATDATA.Clone(null!); }
    // Purpose: Verify IEnumSTATDATA.Next executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Next:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumSTATDATA_Next_System_Void_P0_P1_P2() { IEnumSTATDATA.Next(null!, null!, null!); }
    // Purpose: Verify IEnumSTATDATA.Reset executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Reset:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumSTATDATA_Reset_System_Void() { IEnumSTATDATA.Reset(); }
    // Purpose: Verify IEnumSTATDATA.Skip executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Skip:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumSTATDATA_Skip_System_Void_P0() { IEnumSTATDATA.Skip(null!); }
    // Purpose: Verify IEnumString.Clone executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumString::Clone:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumString_Clone_System_Void_P0() { IEnumString.Clone(null!); }
    // Purpose: Verify IEnumString.Next executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumString::Next:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumString_Next_System_Void_P0_P1_P2() { IEnumString.Next(null!, null!, null!); }
    // Purpose: Verify IEnumString.Reset executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumString::Reset:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumString_Reset_System_Void() { IEnumString.Reset(); }
    // Purpose: Verify IEnumString.Skip executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumString::Skip:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumString_Skip_System_Void_P0() { IEnumString.Skip(null!); }
    // Purpose: Verify IEnumVARIANT.Clone executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumVARIANT::Clone:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumVARIANT_Clone_System_Void() { IEnumVARIANT.Clone(); }
    // Purpose: Verify IEnumVARIANT.Next executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumVARIANT::Next:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumVARIANT_Next_System_Void_P0_P1_P2() { IEnumVARIANT.Next(null!, null!, null!); }
    // Purpose: Verify IEnumVARIANT.Reset executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumVARIANT::Reset:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumVARIANT_Reset_System_Void() { IEnumVARIANT.Reset(); }
    // Purpose: Verify IEnumVARIANT.Skip executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumVARIANT::Skip:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumVARIANT_Skip_System_Void_P0() { IEnumVARIANT.Skip(null!); }
    // Purpose: Verify IMoniker.BindToObject executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::BindToObject:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_BindToObject_System_Void_P0_P1_P2_P3() { IMoniker.BindToObject(null!, null!, null!, null!); }
    // Purpose: Verify IMoniker.BindToStorage executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::BindToStorage:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_BindToStorage_System_Void_P0_P1_P2_P3() { IMoniker.BindToStorage(null!, null!, null!, null!); }
    // Purpose: Verify IMoniker.CommonPrefixWith executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::CommonPrefixWith:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_CommonPrefixWith_System_Void_P0_P1() { IMoniker.CommonPrefixWith(null!, null!); }
    // Purpose: Verify IMoniker.ComposeWith executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::ComposeWith:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_ComposeWith_System_Void_P0_P1_P2() { IMoniker.ComposeWith(null!, null!, null!); }
    // Purpose: Verify IMoniker.Enum executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Enum:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_Enum_System_Void_P0_P1() { IMoniker.Enum(null!, null!); }
    // Purpose: Verify IMoniker.GetClassID executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::GetClassID:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_GetClassID_System_Void_P0() { IMoniker.GetClassID(null!); }
    // Purpose: Verify IMoniker.GetDisplayName executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::GetDisplayName:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_GetDisplayName_System_Void_P0_P1_P2() { IMoniker.GetDisplayName(null!, null!, null!); }
    // Purpose: Verify IMoniker.GetSizeMax executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::GetSizeMax:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_GetSizeMax_System_Void_P0() { IMoniker.GetSizeMax(null!); }
    // Purpose: Verify IMoniker.GetTimeOfLastChange executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::GetTimeOfLastChange:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_GetTimeOfLastChange_System_Void_P0_P1_P2() { IMoniker.GetTimeOfLastChange(null!, null!, null!); }
    // Purpose: Verify IMoniker.Hash executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Hash:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_Hash_System_Void_P0() { IMoniker.Hash(null!); }
    // Purpose: Verify IMoniker.Inverse executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Inverse:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_Inverse_System_Void_P0() { IMoniker.Inverse(null!); }
    // Purpose: Verify IMoniker.IsDirty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::IsDirty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_IsDirty_System_Void() { IMoniker.IsDirty(); }
    // Purpose: Verify IMoniker.IsEqual executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::IsEqual:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_IsEqual_System_Void_P0() { IMoniker.IsEqual(null!); }
    // Purpose: Verify IMoniker.IsRunning executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::IsRunning:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_IsRunning_System_Void_P0_P1_P2() { IMoniker.IsRunning(null!, null!, null!); }
    // Purpose: Verify IMoniker.IsSystemMoniker executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::IsSystemMoniker:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_IsSystemMoniker_System_Void_P0() { IMoniker.IsSystemMoniker(null!); }
    // Purpose: Verify IMoniker.Load executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Load:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_Load_System_Void_P0() { IMoniker.Load(null!); }
    // Purpose: Verify IMoniker.ParseDisplayName executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::ParseDisplayName:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_ParseDisplayName_System_Void_P0_P1_P2_P3() { IMoniker.ParseDisplayName(null!, null!, null!, null!, null!); }
    // Purpose: Verify IMoniker.Reduce executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Reduce:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_Reduce_System_Void_P0_P1_P2_P3() { IMoniker.Reduce(null!, null!, null!, null!); }
    // Purpose: Verify IMoniker.RelativePathTo executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::RelativePathTo:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_RelativePathTo_System_Void_P0_P1() { IMoniker.RelativePathTo(null!, null!); }
    // Purpose: Verify IMoniker.Save executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Save:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_Save_System_Void_P0_P1() { IMoniker.Save(null!, null!); }
    // Purpose: Verify IPersistFile.GetClassID executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::GetClassID:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IPersistFile_GetClassID_System_Void_P0() { IPersistFile.GetClassID(null!); }
    // Purpose: Verify IPersistFile.GetCurFile executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::GetCurFile:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IPersistFile_GetCurFile_System_Void_P0() { IPersistFile.GetCurFile(null!); }
    // Purpose: Verify IPersistFile.IsDirty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::IsDirty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IPersistFile_IsDirty_System_Void() { IPersistFile.IsDirty(); }
    // Purpose: Verify IPersistFile.Load executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::Load:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IPersistFile_Load_System_Void_P0_P1() { IPersistFile.Load(null!, null!); }
    // Purpose: Verify IPersistFile.Save executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::Save:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IPersistFile_Save_System_Void_P0_P1() { IPersistFile.Save(null!, null!); }
    // Purpose: Verify IPersistFile.SaveCompleted executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::SaveCompleted:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IPersistFile_SaveCompleted_System_Void_P0() { IPersistFile.SaveCompleted(null!); }
    // Purpose: Verify IRunningObjectTable.EnumRunning executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::EnumRunning:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IRunningObjectTable_EnumRunning_System_Void_P0() { IRunningObjectTable.EnumRunning(null!); }
    // Purpose: Verify IRunningObjectTable.GetObject executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::GetObject:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IRunningObjectTable_GetObject_System_Void_P0_P1() { IRunningObjectTable.GetObject(null!, null!); }
    // Purpose: Verify IRunningObjectTable.GetTimeOfLastChange executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::GetTimeOfLastChange:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IRunningObjectTable_GetTimeOfLastChange_System_Vo() { IRunningObjectTable.GetTimeOfLastChange(null!, null!); }
    // Purpose: Verify IRunningObjectTable.IsRunning executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::IsRunning:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IRunningObjectTable_IsRunning_System_Void_P0() { IRunningObjectTable.IsRunning(null!); }
    // Purpose: Verify IRunningObjectTable.NoteChangeTime executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::NoteChangeTime:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IRunningObjectTable_NoteChangeTime_System_Void_P0() { IRunningObjectTable.NoteChangeTime(null!, null!); }
    // Purpose: Verify IRunningObjectTable.Register executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::Register:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IRunningObjectTable_Register_System_Void_P0_P1_P2() { IRunningObjectTable.Register(null!, null!, null!); }
    // Purpose: Verify IRunningObjectTable.Revoke executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::Revoke:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IRunningObjectTable_Revoke_System_Void_P0() { IRunningObjectTable.Revoke(null!); }
    // Purpose: Verify IStream.Clone executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Clone:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_Clone_System_Void_P0() { IStream.Clone(null!); }
    // Purpose: Verify IStream.Commit executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Commit:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_Commit_System_Void_P0() { IStream.Commit(null!); }
    // Purpose: Verify IStream.CopyTo executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::CopyTo:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_CopyTo_System_Void_P0_P1_P2_P3() { IStream.CopyTo(null!, null!, null!, null!); }
    // Purpose: Verify IStream.LockRegion executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::LockRegion:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_LockRegion_System_Void_P0_P1_P2() { IStream.LockRegion(null!, null!, null!); }
    // Purpose: Verify IStream.Read executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Read:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_Read_System_Void_P0_P1_P2() { IStream.Read(null!, null!, null!); }
    // Purpose: Verify IStream.Revert executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Revert:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_Revert_System_Void() { IStream.Revert(); }
    // Purpose: Verify IStream.Seek executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Seek:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_Seek_System_Void_P0_P1_P2() { IStream.Seek(null!, null!, null!); }
    // Purpose: Verify IStream.SetSize executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::SetSize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_SetSize_System_Void_P0() { IStream.SetSize(null!); }
    // Purpose: Verify IStream.Stat executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Stat:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_Stat_System_Void_P0_P1() { IStream.Stat(null!, null!); }
    // Purpose: Verify IStream.UnlockRegion executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::UnlockRegion:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_UnlockRegion_System_Void_P0_P1_P2() { IStream.UnlockRegion(null!, null!, null!); }
    // Purpose: Verify IStream.Write executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Write:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_Write_System_Void_P0_P1_P2() { IStream.Write(null!, null!, null!); }
    // Purpose: Verify ITypeComp.Bind executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeComp::Bind:System.Void(P0, P1, P2, P3, P4, P5)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeComp_Bind_System_Void_P0_P1_P2_P3_P4_P5() { ITypeComp.Bind(null!, null!, null!, null!, null!, null!); }
    // Purpose: Verify ITypeComp.BindType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeComp::BindType:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeComp_BindType_System_Void_P0_P1_P2_P3() { ITypeComp.BindType(null!, null!, null!, null!); }
    // Purpose: Verify ITypeInfo.AddressOfMember executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::AddressOfMember:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_AddressOfMember_System_Void_P0_P1_P2() { ITypeInfo.AddressOfMember(null!, null!, null!); }
    // Purpose: Verify ITypeInfo.CreateInstance executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::CreateInstance:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_CreateInstance_System_Void_P0_P1_P2() { ITypeInfo.CreateInstance(null!, null!, null!); }
    // Purpose: Verify ITypeInfo.GetContainingTypeLib executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetContainingTypeLib:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetContainingTypeLib_System_Void_P0_P1() { ITypeInfo.GetContainingTypeLib(null!, null!); }
    // Purpose: Verify ITypeInfo.GetDllEntry executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetDllEntry:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetDllEntry_System_Void_P0_P1_P2_P3_P4() { ITypeInfo.GetDllEntry(null!, null!, null!, null!, null!); }
    // Purpose: Verify ITypeInfo.GetDocumentation executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetDocumentation:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetDocumentation_System_Void_P0_P1_P2_P() { ITypeInfo.GetDocumentation(null!, null!, null!, null!, null!); }
    // Purpose: Verify ITypeInfo.GetFuncDesc executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetFuncDesc:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetFuncDesc_System_Void_P0_P1() { ITypeInfo.GetFuncDesc(null!, null!); }
    // Purpose: Verify ITypeInfo.GetIDsOfNames executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetIDsOfNames:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetIDsOfNames_System_Void_P0_P1_P2() { ITypeInfo.GetIDsOfNames(null!, null!, null!); }
    // Purpose: Verify ITypeInfo.GetImplTypeFlags executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetImplTypeFlags:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetImplTypeFlags_System_Void_P0_P1() { ITypeInfo.GetImplTypeFlags(null!, null!); }
    // Purpose: Verify ITypeInfo.GetMops executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetMops:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetMops_System_Void_P0_P1() { ITypeInfo.GetMops(null!, null!); }
    // Purpose: Verify ITypeInfo.GetNames executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetNames:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetNames_System_Void_P0_P1_P2_P3() { ITypeInfo.GetNames(null!, null!, null!, null!); }
    // Purpose: Verify ITypeInfo.GetRefTypeInfo executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetRefTypeInfo:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetRefTypeInfo_System_Void_P0_P1() { ITypeInfo.GetRefTypeInfo(null!, null!); }
    // Purpose: Verify ITypeInfo.GetRefTypeOfImplType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetRefTypeOfImplType:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetRefTypeOfImplType_System_Void_P0_P1() { ITypeInfo.GetRefTypeOfImplType(null!, null!); }
    // Purpose: Verify ITypeInfo.GetTypeAttr executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetTypeAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetTypeAttr_System_Void_P0() { ITypeInfo.GetTypeAttr(null!); }
    // Purpose: Verify ITypeInfo.GetTypeComp executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetTypeComp:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetTypeComp_System_Void_P0() { ITypeInfo.GetTypeComp(null!); }
    // Purpose: Verify ITypeInfo.GetVarDesc executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetVarDesc:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetVarDesc_System_Void_P0_P1() { ITypeInfo.GetVarDesc(null!, null!); }
    // Purpose: Verify ITypeInfo.Invoke executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::Invoke:System.Void(P0, P1, P2, P3, P4, P5, P6)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_Invoke_System_Void_P0_P1_P2_P3_P4_P5_P6() { ITypeInfo.Invoke(null!, null!, null!, null!, null!, null!, null!); }
    // Purpose: Verify ITypeInfo.ReleaseFuncDesc executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::ReleaseFuncDesc:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_ReleaseFuncDesc_System_Void_P0() { ITypeInfo.ReleaseFuncDesc(null!); }
    // Purpose: Verify ITypeInfo.ReleaseTypeAttr executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::ReleaseTypeAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_ReleaseTypeAttr_System_Void_P0() { ITypeInfo.ReleaseTypeAttr(null!); }
    // Purpose: Verify ITypeInfo.ReleaseVarDesc executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::ReleaseVarDesc:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_ReleaseVarDesc_System_Void_P0() { ITypeInfo.ReleaseVarDesc(null!); }
    // Purpose: Verify ITypeInfo2.AddressOfMember executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::AddressOfMember:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_AddressOfMember_System_Void_P0_P1_P2() { ITypeInfo2.AddressOfMember(null!, null!, null!); }
    // Purpose: Verify ITypeInfo2.CreateInstance executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::CreateInstance:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_CreateInstance_System_Void_P0_P1_P2() { ITypeInfo2.CreateInstance(null!, null!, null!); }
    // Purpose: Verify ITypeInfo2.GetAllCustData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllCustData:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetAllCustData_System_Void_P0() { ITypeInfo2.GetAllCustData(null!); }
    // Purpose: Verify ITypeInfo2.GetAllFuncCustData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllFuncCustData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetAllFuncCustData_System_Void_P0_P1() { ITypeInfo2.GetAllFuncCustData(null!, null!); }
    // Purpose: Verify ITypeInfo2.GetAllImplTypeCustData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllImplTypeCustData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetAllImplTypeCustData_System_Void_P0_() { ITypeInfo2.GetAllImplTypeCustData(null!, null!); }
    // Purpose: Verify ITypeInfo2.GetAllParamCustData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllParamCustData:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetAllParamCustData_System_Void_P0_P1_() { ITypeInfo2.GetAllParamCustData(null!, null!, null!); }
    // Purpose: Verify ITypeInfo2.GetAllVarCustData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllVarCustData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetAllVarCustData_System_Void_P0_P1() { ITypeInfo2.GetAllVarCustData(null!, null!); }
    // Purpose: Verify ITypeInfo2.GetContainingTypeLib executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetContainingTypeLib:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetContainingTypeLib_System_Void_P0_P1() { ITypeInfo2.GetContainingTypeLib(null!, null!); }
    // Purpose: Verify ITypeInfo2.GetCustData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetCustData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetCustData_System_Void_P0_P1() { ITypeInfo2.GetCustData(null!, null!); }
    // Purpose: Verify ITypeInfo2.GetDllEntry executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetDllEntry:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetDllEntry_System_Void_P0_P1_P2_P3_P4() { ITypeInfo2.GetDllEntry(null!, null!, null!, null!, null!); }
    // Purpose: Verify ITypeInfo2.GetDocumentation executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetDocumentation:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetDocumentation_System_Void_P0_P1_P2_() { ITypeInfo2.GetDocumentation(null!, null!, null!, null!, null!); }
    // Purpose: Verify ITypeInfo2.GetDocumentation2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetDocumentation2:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetDocumentation2_System_Void_P0_P1_P2() { ITypeInfo2.GetDocumentation2(null!, null!, null!, null!); }
    // Purpose: Verify ITypeInfo2.GetFuncCustData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetFuncCustData:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetFuncCustData_System_Void_P0_P1_P2() { ITypeInfo2.GetFuncCustData(null!, null!, null!); }
    // Purpose: Verify ITypeInfo2.GetFuncDesc executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetFuncDesc:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetFuncDesc_System_Void_P0_P1() { ITypeInfo2.GetFuncDesc(null!, null!); }
    // Purpose: Verify ITypeInfo2.GetFuncIndexOfMemId executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetFuncIndexOfMemId:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetFuncIndexOfMemId_System_Void_P0_P1_() { ITypeInfo2.GetFuncIndexOfMemId(null!, null!, null!); }
    // Purpose: Verify ITypeInfo2.GetIDsOfNames executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetIDsOfNames:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetIDsOfNames_System_Void_P0_P1_P2() { ITypeInfo2.GetIDsOfNames(null!, null!, null!); }
    // Purpose: Verify ITypeInfo2.GetImplTypeCustData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetImplTypeCustData:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetImplTypeCustData_System_Void_P0_P1_() { ITypeInfo2.GetImplTypeCustData(null!, null!, null!); }
    // Purpose: Verify ITypeInfo2.GetImplTypeFlags executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetImplTypeFlags:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetImplTypeFlags_System_Void_P0_P1() { ITypeInfo2.GetImplTypeFlags(null!, null!); }
    // Purpose: Verify ITypeInfo2.GetMops executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetMops:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetMops_System_Void_P0_P1() { ITypeInfo2.GetMops(null!, null!); }
    // Purpose: Verify ITypeInfo2.GetNames executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetNames:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetNames_System_Void_P0_P1_P2_P3() { ITypeInfo2.GetNames(null!, null!, null!, null!); }
    // Purpose: Verify ITypeInfo2.GetParamCustData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetParamCustData:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetParamCustData_System_Void_P0_P1_P2_() { ITypeInfo2.GetParamCustData(null!, null!, null!, null!); }
    // Purpose: Verify ITypeInfo2.GetRefTypeInfo executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetRefTypeInfo:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetRefTypeInfo_System_Void_P0_P1() { ITypeInfo2.GetRefTypeInfo(null!, null!); }
    // Purpose: Verify ITypeInfo2.GetRefTypeOfImplType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetRefTypeOfImplType:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetRefTypeOfImplType_System_Void_P0_P1() { ITypeInfo2.GetRefTypeOfImplType(null!, null!); }
    // Purpose: Verify ITypeInfo2.GetTypeAttr executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetTypeAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetTypeAttr_System_Void_P0() { ITypeInfo2.GetTypeAttr(null!); }
    // Purpose: Verify ITypeInfo2.GetTypeComp executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetTypeComp:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetTypeComp_System_Void_P0() { ITypeInfo2.GetTypeComp(null!); }
    // Purpose: Verify ITypeInfo2.GetTypeFlags executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetTypeFlags:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetTypeFlags_System_Void_P0() { ITypeInfo2.GetTypeFlags(null!); }
    // Purpose: Verify ITypeInfo2.GetTypeKind executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetTypeKind:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetTypeKind_System_Void_P0() { ITypeInfo2.GetTypeKind(null!); }
    // Purpose: Verify ITypeInfo2.GetVarCustData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetVarCustData:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetVarCustData_System_Void_P0_P1_P2() { ITypeInfo2.GetVarCustData(null!, null!, null!); }
    // Purpose: Verify ITypeInfo2.GetVarDesc executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetVarDesc:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetVarDesc_System_Void_P0_P1() { ITypeInfo2.GetVarDesc(null!, null!); }
    // Purpose: Verify ITypeInfo2.GetVarIndexOfMemId executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetVarIndexOfMemId:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetVarIndexOfMemId_System_Void_P0_P1() { ITypeInfo2.GetVarIndexOfMemId(null!, null!); }
    // Purpose: Verify ITypeInfo2.Invoke executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::Invoke:System.Void(P0, P1, P2, P3, P4, P5, P6)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_Invoke_System_Void_P0_P1_P2_P3_P4_P5_P() { ITypeInfo2.Invoke(null!, null!, null!, null!, null!, null!, null!); }
    // Purpose: Verify ITypeInfo2.ReleaseFuncDesc executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::ReleaseFuncDesc:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_ReleaseFuncDesc_System_Void_P0() { ITypeInfo2.ReleaseFuncDesc(null!); }
    // Purpose: Verify ITypeInfo2.ReleaseTypeAttr executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::ReleaseTypeAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_ReleaseTypeAttr_System_Void_P0() { ITypeInfo2.ReleaseTypeAttr(null!); }
    // Purpose: Verify ITypeInfo2.ReleaseVarDesc executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::ReleaseVarDesc:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_ReleaseVarDesc_System_Void_P0() { ITypeInfo2.ReleaseVarDesc(null!); }
    // Purpose: Verify ITypeLib.FindName executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::FindName:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_FindName_System_Void_P0_P1_P2_P3_P4() { ITypeLib.FindName(null!, null!, null!, null!, null!); }
    // Purpose: Verify ITypeLib.GetDocumentation executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetDocumentation:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_GetDocumentation_System_Void_P0_P1_P2_P3() { ITypeLib.GetDocumentation(null!, null!, null!, null!, null!); }
    // Purpose: Verify ITypeLib.GetLibAttr executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetLibAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_GetLibAttr_System_Void_P0() { ITypeLib.GetLibAttr(null!); }
    // Purpose: Verify ITypeLib.GetTypeComp executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeComp:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_GetTypeComp_System_Void_P0() { ITypeLib.GetTypeComp(null!); }
    // Purpose: Verify ITypeLib.GetTypeInfo executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeInfo:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_GetTypeInfo_System_Void_P0_P1() { ITypeLib.GetTypeInfo(null!, null!); }
    // Purpose: Verify ITypeLib.GetTypeInfoCount executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeInfoCount:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_GetTypeInfoCount_System_Void() { ITypeLib.GetTypeInfoCount(); }
    // Purpose: Verify ITypeLib.GetTypeInfoOfGuid executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeInfoOfGuid:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_GetTypeInfoOfGuid_System_Void_P0_P1() { ITypeLib.GetTypeInfoOfGuid(null!, null!); }
    // Purpose: Verify ITypeLib.GetTypeInfoType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeInfoType:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_GetTypeInfoType_System_Void_P0_P1() { ITypeLib.GetTypeInfoType(null!, null!); }
    // Purpose: Verify ITypeLib.IsName executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::IsName:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_IsName_System_Void_P0_P1() { ITypeLib.IsName(null!, null!); }
    // Purpose: Verify ITypeLib.ReleaseTLibAttr executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::ReleaseTLibAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_ReleaseTLibAttr_System_Void_P0() { ITypeLib.ReleaseTLibAttr(null!); }
    // Purpose: Verify ITypeLib2.FindName executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::FindName:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_FindName_System_Void_P0_P1_P2_P3_P4() { ITypeLib2.FindName(null!, null!, null!, null!, null!); }
    // Purpose: Verify ITypeLib2.GetAllCustData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetAllCustData:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetAllCustData_System_Void_P0() { ITypeLib2.GetAllCustData(null!); }
    // Purpose: Verify ITypeLib2.GetCustData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetCustData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetCustData_System_Void_P0_P1() { ITypeLib2.GetCustData(null!, null!); }
    // Purpose: Verify ITypeLib2.GetDocumentation executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetDocumentation:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetDocumentation_System_Void_P0_P1_P2_P() { ITypeLib2.GetDocumentation(null!, null!, null!, null!, null!); }
    // Purpose: Verify ITypeLib2.GetDocumentation2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetDocumentation2:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetDocumentation2_System_Void_P0_P1_P2_() { ITypeLib2.GetDocumentation2(null!, null!, null!, null!); }
    // Purpose: Verify ITypeLib2.GetLibAttr executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetLibAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetLibAttr_System_Void_P0() { ITypeLib2.GetLibAttr(null!); }
    // Purpose: Verify ITypeLib2.GetLibStatistics executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetLibStatistics:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetLibStatistics_System_Void_P0_P1() { ITypeLib2.GetLibStatistics(null!, null!); }
    // Purpose: Verify ITypeLib2.GetTypeComp executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeComp:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetTypeComp_System_Void_P0() { ITypeLib2.GetTypeComp(null!); }
    // Purpose: Verify ITypeLib2.GetTypeInfo executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeInfo:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetTypeInfo_System_Void_P0_P1() { ITypeLib2.GetTypeInfo(null!, null!); }
    // Purpose: Verify ITypeLib2.GetTypeInfoCount executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeInfoCount:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetTypeInfoCount_System_Void() { ITypeLib2.GetTypeInfoCount(); }
    // Purpose: Verify ITypeLib2.GetTypeInfoOfGuid executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeInfoOfGuid:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetTypeInfoOfGuid_System_Void_P0_P1() { ITypeLib2.GetTypeInfoOfGuid(null!, null!); }
    // Purpose: Verify ITypeLib2.GetTypeInfoType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeInfoType:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetTypeInfoType_System_Void_P0_P1() { ITypeLib2.GetTypeInfoType(null!, null!); }
    // Purpose: Verify ITypeLib2.IsName executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::IsName:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_IsName_System_Void_P0_P1() { ITypeLib2.IsName(null!, null!); }
    // Purpose: Verify ITypeLib2.ReleaseTLibAttr executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::ReleaseTLibAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_ReleaseTLibAttr_System_Void_P0() { ITypeLib2.ReleaseTLibAttr(null!); }}
