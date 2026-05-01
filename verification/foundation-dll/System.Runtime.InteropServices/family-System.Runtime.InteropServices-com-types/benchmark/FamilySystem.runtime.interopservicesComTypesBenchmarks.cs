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

// Auto-generated benchmark skeletons for FamilySystem.runtime.interopservicesComTypesBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.runtime.interopservicesComTypesBenchmarks
{
    // Purpose: Benchmark native-runtime performance of IAdviseSink.OnClose with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnClose:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IAdviseSink_OnClose_System_Void() { IAdviseSink.OnClose(); }
    // Purpose: Benchmark native-runtime performance of IAdviseSink.OnDataChange with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnDataChange:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IAdviseSink_OnDataChange_System_Void_P0_P1() { IAdviseSink.OnDataChange(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IAdviseSink.OnRename with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnRename:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IAdviseSink_OnRename_System_Void_P0() { IAdviseSink.OnRename(null!); }
    // Purpose: Benchmark native-runtime performance of IAdviseSink.OnSave with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnSave:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IAdviseSink_OnSave_System_Void() { IAdviseSink.OnSave(); }
    // Purpose: Benchmark native-runtime performance of IAdviseSink.OnViewChange with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnViewChange:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IAdviseSink_OnViewChange_System_Void_P0_P1() { IAdviseSink.OnViewChange(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IBindCtx.EnumObjectParam with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::EnumObjectParam:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_EnumObjectParam_System_Void_P0() { IBindCtx.EnumObjectParam(null!); }
    // Purpose: Benchmark native-runtime performance of IBindCtx.GetBindOptions with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::GetBindOptions:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_GetBindOptions_System_Void_P0() { IBindCtx.GetBindOptions(null!); }
    // Purpose: Benchmark native-runtime performance of IBindCtx.GetObjectParam with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::GetObjectParam:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_GetObjectParam_System_Void_P0_P1() { IBindCtx.GetObjectParam(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IBindCtx.GetRunningObjectTable with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::GetRunningObjectTable:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_GetRunningObjectTable_System_Void_P0() { IBindCtx.GetRunningObjectTable(null!); }
    // Purpose: Benchmark native-runtime performance of IBindCtx.RegisterObjectBound with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::RegisterObjectBound:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_RegisterObjectBound_System_Void_P0() { IBindCtx.RegisterObjectBound(null!); }
    // Purpose: Benchmark native-runtime performance of IBindCtx.RegisterObjectParam with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::RegisterObjectParam:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_RegisterObjectParam_System_Void_P0_P1() { IBindCtx.RegisterObjectParam(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IBindCtx.ReleaseBoundObjects with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::ReleaseBoundObjects:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_ReleaseBoundObjects_System_Void() { IBindCtx.ReleaseBoundObjects(); }
    // Purpose: Benchmark native-runtime performance of IBindCtx.RevokeObjectBound with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::RevokeObjectBound:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_RevokeObjectBound_System_Void_P0() { IBindCtx.RevokeObjectBound(null!); }
    // Purpose: Benchmark native-runtime performance of IBindCtx.RevokeObjectParam with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::RevokeObjectParam:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_RevokeObjectParam_System_Void_P0() { IBindCtx.RevokeObjectParam(null!); }
    // Purpose: Benchmark native-runtime performance of IBindCtx.SetBindOptions with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::SetBindOptions:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IBindCtx_SetBindOptions_System_Void_P0() { IBindCtx.SetBindOptions(null!); }
    // Purpose: Benchmark native-runtime performance of IConnectionPoint.Advise with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::Advise:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IConnectionPoint_Advise_System_Void_P0_P1() { IConnectionPoint.Advise(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IConnectionPoint.EnumConnections with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::EnumConnections:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IConnectionPoint_EnumConnections_System_Void_P0() { IConnectionPoint.EnumConnections(null!); }
    // Purpose: Benchmark native-runtime performance of IConnectionPoint.GetConnectionInterface with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::GetConnectionInterface:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IConnectionPoint_GetConnectionInterface_System_Vo() { IConnectionPoint.GetConnectionInterface(null!); }
    // Purpose: Benchmark native-runtime performance of IConnectionPoint.GetConnectionPointContainer with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::GetConnectionPointContainer:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IConnectionPoint_GetConnectionPointContainer_Syst() { IConnectionPoint.GetConnectionPointContainer(null!); }
    // Purpose: Benchmark native-runtime performance of IConnectionPoint.Unadvise with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::Unadvise:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IConnectionPoint_Unadvise_System_Void_P0() { IConnectionPoint.Unadvise(null!); }
    // Purpose: Benchmark native-runtime performance of IConnectionPointContainer.EnumConnectionPoints with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPointContainer::EnumConnectionPoints:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IConnectionPointContainer_EnumConnectionPoints_Sy() { IConnectionPointContainer.EnumConnectionPoints(null!); }
    // Purpose: Benchmark native-runtime performance of IConnectionPointContainer.FindConnectionPoint with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPointContainer::FindConnectionPoint:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IConnectionPointContainer_FindConnectionPoint_Sys() { IConnectionPointContainer.FindConnectionPoint(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IDataObject.DAdvise with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DAdvise:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_DAdvise_System_Void_P0_P1_P2_P3() { IDataObject.DAdvise(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IDataObject.DUnadvise with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DUnadvise:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_DUnadvise_System_Void_P0() { IDataObject.DUnadvise(null!); }
    // Purpose: Benchmark native-runtime performance of IDataObject.EnumDAdvise with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumDAdvise:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_EnumDAdvise_System_Void_P0() { IDataObject.EnumDAdvise(null!); }
    // Purpose: Benchmark native-runtime performance of IDataObject.EnumFormatEtc with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumFormatEtc:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_EnumFormatEtc_System_Void_P0() { IDataObject.EnumFormatEtc(null!); }
    // Purpose: Benchmark native-runtime performance of IDataObject.GetCanonicalFormatEtc with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetCanonicalFormatEtc:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_GetCanonicalFormatEtc_System_Void_P0_() { IDataObject.GetCanonicalFormatEtc(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IDataObject.GetData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_GetData_System_Void_P0_P1() { IDataObject.GetData(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IDataObject.GetDataHere with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetDataHere:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_GetDataHere_System_Void_P0_P1() { IDataObject.GetDataHere(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IDataObject.QueryGetData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::QueryGetData:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_QueryGetData_System_Void_P0() { IDataObject.QueryGetData(null!); }
    // Purpose: Benchmark native-runtime performance of IDataObject.SetData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::SetData:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IDataObject_SetData_System_Void_P0_P1_P2() { IDataObject.SetData(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IEnumConnectionPoints.Clone with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnectionPoints::Clone:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnectionPoints_Clone_System_Void_P0() { IEnumConnectionPoints.Clone(null!); }
    // Purpose: Benchmark native-runtime performance of IEnumConnectionPoints.Next with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnectionPoints::Next:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnectionPoints_Next_System_Void_P0_P1_P2() { IEnumConnectionPoints.Next(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IEnumConnectionPoints.Reset with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnectionPoints::Reset:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnectionPoints_Reset_System_Void() { IEnumConnectionPoints.Reset(); }
    // Purpose: Benchmark native-runtime performance of IEnumConnectionPoints.Skip with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnectionPoints::Skip:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnectionPoints_Skip_System_Void_P0() { IEnumConnectionPoints.Skip(null!); }
    // Purpose: Benchmark native-runtime performance of IEnumConnections.Clone with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnections::Clone:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnections_Clone_System_Void_P0() { IEnumConnections.Clone(null!); }
    // Purpose: Benchmark native-runtime performance of IEnumConnections.Next with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnections::Next:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnections_Next_System_Void_P0_P1_P2() { IEnumConnections.Next(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IEnumConnections.Reset with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnections::Reset:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnections_Reset_System_Void() { IEnumConnections.Reset(); }
    // Purpose: Benchmark native-runtime performance of IEnumConnections.Skip with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnections::Skip:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumConnections_Skip_System_Void_P0() { IEnumConnections.Skip(null!); }
    // Purpose: Benchmark native-runtime performance of IEnumFORMATETC.Clone with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Clone:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumFORMATETC_Clone_System_Void_P0() { IEnumFORMATETC.Clone(null!); }
    // Purpose: Benchmark native-runtime performance of IEnumFORMATETC.Next with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Next:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumFORMATETC_Next_System_Void_P0_P1_P2() { IEnumFORMATETC.Next(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IEnumFORMATETC.Reset with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Reset:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumFORMATETC_Reset_System_Void() { IEnumFORMATETC.Reset(); }
    // Purpose: Benchmark native-runtime performance of IEnumFORMATETC.Skip with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Skip:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumFORMATETC_Skip_System_Void_P0() { IEnumFORMATETC.Skip(null!); }
    // Purpose: Benchmark native-runtime performance of IEnumMoniker.Clone with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumMoniker::Clone:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumMoniker_Clone_System_Void_P0() { IEnumMoniker.Clone(null!); }
    // Purpose: Benchmark native-runtime performance of IEnumMoniker.Next with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumMoniker::Next:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumMoniker_Next_System_Void_P0_P1_P2() { IEnumMoniker.Next(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IEnumMoniker.Reset with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumMoniker::Reset:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumMoniker_Reset_System_Void() { IEnumMoniker.Reset(); }
    // Purpose: Benchmark native-runtime performance of IEnumMoniker.Skip with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumMoniker::Skip:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumMoniker_Skip_System_Void_P0() { IEnumMoniker.Skip(null!); }
    // Purpose: Benchmark native-runtime performance of IEnumSTATDATA.Clone with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Clone:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumSTATDATA_Clone_System_Void_P0() { IEnumSTATDATA.Clone(null!); }
    // Purpose: Benchmark native-runtime performance of IEnumSTATDATA.Next with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Next:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumSTATDATA_Next_System_Void_P0_P1_P2() { IEnumSTATDATA.Next(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IEnumSTATDATA.Reset with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Reset:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumSTATDATA_Reset_System_Void() { IEnumSTATDATA.Reset(); }
    // Purpose: Benchmark native-runtime performance of IEnumSTATDATA.Skip with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Skip:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumSTATDATA_Skip_System_Void_P0() { IEnumSTATDATA.Skip(null!); }
    // Purpose: Benchmark native-runtime performance of IEnumString.Clone with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumString::Clone:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumString_Clone_System_Void_P0() { IEnumString.Clone(null!); }
    // Purpose: Benchmark native-runtime performance of IEnumString.Next with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumString::Next:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumString_Next_System_Void_P0_P1_P2() { IEnumString.Next(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IEnumString.Reset with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumString::Reset:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumString_Reset_System_Void() { IEnumString.Reset(); }
    // Purpose: Benchmark native-runtime performance of IEnumString.Skip with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumString::Skip:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumString_Skip_System_Void_P0() { IEnumString.Skip(null!); }
    // Purpose: Benchmark native-runtime performance of IEnumVARIANT.Clone with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumVARIANT::Clone:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumVARIANT_Clone_System_Void() { IEnumVARIANT.Clone(); }
    // Purpose: Benchmark native-runtime performance of IEnumVARIANT.Next with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumVARIANT::Next:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumVARIANT_Next_System_Void_P0_P1_P2() { IEnumVARIANT.Next(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IEnumVARIANT.Reset with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumVARIANT::Reset:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumVARIANT_Reset_System_Void() { IEnumVARIANT.Reset(); }
    // Purpose: Benchmark native-runtime performance of IEnumVARIANT.Skip with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumVARIANT::Skip:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IEnumVARIANT_Skip_System_Void_P0() { IEnumVARIANT.Skip(null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.BindToObject with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::BindToObject:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_BindToObject_System_Void_P0_P1_P2_P3() { IMoniker.BindToObject(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.BindToStorage with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::BindToStorage:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_BindToStorage_System_Void_P0_P1_P2_P3() { IMoniker.BindToStorage(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.CommonPrefixWith with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::CommonPrefixWith:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_CommonPrefixWith_System_Void_P0_P1() { IMoniker.CommonPrefixWith(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.ComposeWith with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::ComposeWith:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_ComposeWith_System_Void_P0_P1_P2() { IMoniker.ComposeWith(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.Enum with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Enum:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_Enum_System_Void_P0_P1() { IMoniker.Enum(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.GetClassID with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::GetClassID:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_GetClassID_System_Void_P0() { IMoniker.GetClassID(null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.GetDisplayName with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::GetDisplayName:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_GetDisplayName_System_Void_P0_P1_P2() { IMoniker.GetDisplayName(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.GetSizeMax with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::GetSizeMax:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_GetSizeMax_System_Void_P0() { IMoniker.GetSizeMax(null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.GetTimeOfLastChange with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::GetTimeOfLastChange:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_GetTimeOfLastChange_System_Void_P0_P1_P2() { IMoniker.GetTimeOfLastChange(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.Hash with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Hash:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_Hash_System_Void_P0() { IMoniker.Hash(null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.Inverse with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Inverse:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_Inverse_System_Void_P0() { IMoniker.Inverse(null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.IsDirty with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::IsDirty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_IsDirty_System_Void() { IMoniker.IsDirty(); }
    // Purpose: Benchmark native-runtime performance of IMoniker.IsEqual with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::IsEqual:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_IsEqual_System_Void_P0() { IMoniker.IsEqual(null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.IsRunning with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::IsRunning:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_IsRunning_System_Void_P0_P1_P2() { IMoniker.IsRunning(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.IsSystemMoniker with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::IsSystemMoniker:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_IsSystemMoniker_System_Void_P0() { IMoniker.IsSystemMoniker(null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.Load with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Load:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_Load_System_Void_P0() { IMoniker.Load(null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.ParseDisplayName with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::ParseDisplayName:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_ParseDisplayName_System_Void_P0_P1_P2_P3() { IMoniker.ParseDisplayName(null!, null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.Reduce with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Reduce:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_Reduce_System_Void_P0_P1_P2_P3() { IMoniker.Reduce(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.RelativePathTo with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::RelativePathTo:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_RelativePathTo_System_Void_P0_P1() { IMoniker.RelativePathTo(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IMoniker.Save with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Save:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IMoniker_Save_System_Void_P0_P1() { IMoniker.Save(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IPersistFile.GetClassID with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::GetClassID:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IPersistFile_GetClassID_System_Void_P0() { IPersistFile.GetClassID(null!); }
    // Purpose: Benchmark native-runtime performance of IPersistFile.GetCurFile with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::GetCurFile:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IPersistFile_GetCurFile_System_Void_P0() { IPersistFile.GetCurFile(null!); }
    // Purpose: Benchmark native-runtime performance of IPersistFile.IsDirty with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::IsDirty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IPersistFile_IsDirty_System_Void() { IPersistFile.IsDirty(); }
    // Purpose: Benchmark native-runtime performance of IPersistFile.Load with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::Load:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IPersistFile_Load_System_Void_P0_P1() { IPersistFile.Load(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IPersistFile.Save with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::Save:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IPersistFile_Save_System_Void_P0_P1() { IPersistFile.Save(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IPersistFile.SaveCompleted with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::SaveCompleted:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IPersistFile_SaveCompleted_System_Void_P0() { IPersistFile.SaveCompleted(null!); }
    // Purpose: Benchmark native-runtime performance of IRunningObjectTable.EnumRunning with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::EnumRunning:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IRunningObjectTable_EnumRunning_System_Void_P0() { IRunningObjectTable.EnumRunning(null!); }
    // Purpose: Benchmark native-runtime performance of IRunningObjectTable.GetObject with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::GetObject:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IRunningObjectTable_GetObject_System_Void_P0_P1() { IRunningObjectTable.GetObject(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IRunningObjectTable.GetTimeOfLastChange with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::GetTimeOfLastChange:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IRunningObjectTable_GetTimeOfLastChange_System_Vo() { IRunningObjectTable.GetTimeOfLastChange(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IRunningObjectTable.IsRunning with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::IsRunning:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IRunningObjectTable_IsRunning_System_Void_P0() { IRunningObjectTable.IsRunning(null!); }
    // Purpose: Benchmark native-runtime performance of IRunningObjectTable.NoteChangeTime with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::NoteChangeTime:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IRunningObjectTable_NoteChangeTime_System_Void_P0() { IRunningObjectTable.NoteChangeTime(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IRunningObjectTable.Register with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::Register:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IRunningObjectTable_Register_System_Void_P0_P1_P2() { IRunningObjectTable.Register(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IRunningObjectTable.Revoke with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::Revoke:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IRunningObjectTable_Revoke_System_Void_P0() { IRunningObjectTable.Revoke(null!); }
    // Purpose: Benchmark native-runtime performance of IStream.Clone with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Clone:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_Clone_System_Void_P0() { IStream.Clone(null!); }
    // Purpose: Benchmark native-runtime performance of IStream.Commit with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Commit:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_Commit_System_Void_P0() { IStream.Commit(null!); }
    // Purpose: Benchmark native-runtime performance of IStream.CopyTo with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::CopyTo:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_CopyTo_System_Void_P0_P1_P2_P3() { IStream.CopyTo(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IStream.LockRegion with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::LockRegion:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_LockRegion_System_Void_P0_P1_P2() { IStream.LockRegion(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IStream.Read with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Read:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_Read_System_Void_P0_P1_P2() { IStream.Read(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IStream.Revert with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Revert:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_Revert_System_Void() { IStream.Revert(); }
    // Purpose: Benchmark native-runtime performance of IStream.Seek with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Seek:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_Seek_System_Void_P0_P1_P2() { IStream.Seek(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IStream.SetSize with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::SetSize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_SetSize_System_Void_P0() { IStream.SetSize(null!); }
    // Purpose: Benchmark native-runtime performance of IStream.Stat with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Stat:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_Stat_System_Void_P0_P1() { IStream.Stat(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IStream.UnlockRegion with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::UnlockRegion:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_UnlockRegion_System_Void_P0_P1_P2() { IStream.UnlockRegion(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IStream.Write with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Write:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_IStream_Write_System_Void_P0_P1_P2() { IStream.Write(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeComp.Bind with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeComp::Bind:System.Void(P0, P1, P2, P3, P4, P5)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeComp_Bind_System_Void_P0_P1_P2_P3_P4_P5() { ITypeComp.Bind(null!, null!, null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeComp.BindType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeComp::BindType:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeComp_BindType_System_Void_P0_P1_P2_P3() { ITypeComp.BindType(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.AddressOfMember with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::AddressOfMember:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_AddressOfMember_System_Void_P0_P1_P2() { ITypeInfo.AddressOfMember(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.CreateInstance with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::CreateInstance:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_CreateInstance_System_Void_P0_P1_P2() { ITypeInfo.CreateInstance(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.GetContainingTypeLib with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetContainingTypeLib:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetContainingTypeLib_System_Void_P0_P1() { ITypeInfo.GetContainingTypeLib(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.GetDllEntry with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetDllEntry:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetDllEntry_System_Void_P0_P1_P2_P3_P4() { ITypeInfo.GetDllEntry(null!, null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.GetDocumentation with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetDocumentation:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetDocumentation_System_Void_P0_P1_P2_P() { ITypeInfo.GetDocumentation(null!, null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.GetFuncDesc with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetFuncDesc:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetFuncDesc_System_Void_P0_P1() { ITypeInfo.GetFuncDesc(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.GetIDsOfNames with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetIDsOfNames:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetIDsOfNames_System_Void_P0_P1_P2() { ITypeInfo.GetIDsOfNames(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.GetImplTypeFlags with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetImplTypeFlags:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetImplTypeFlags_System_Void_P0_P1() { ITypeInfo.GetImplTypeFlags(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.GetMops with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetMops:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetMops_System_Void_P0_P1() { ITypeInfo.GetMops(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.GetNames with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetNames:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetNames_System_Void_P0_P1_P2_P3() { ITypeInfo.GetNames(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.GetRefTypeInfo with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetRefTypeInfo:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetRefTypeInfo_System_Void_P0_P1() { ITypeInfo.GetRefTypeInfo(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.GetRefTypeOfImplType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetRefTypeOfImplType:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetRefTypeOfImplType_System_Void_P0_P1() { ITypeInfo.GetRefTypeOfImplType(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.GetTypeAttr with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetTypeAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetTypeAttr_System_Void_P0() { ITypeInfo.GetTypeAttr(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.GetTypeComp with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetTypeComp:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetTypeComp_System_Void_P0() { ITypeInfo.GetTypeComp(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.GetVarDesc with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetVarDesc:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_GetVarDesc_System_Void_P0_P1() { ITypeInfo.GetVarDesc(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.Invoke with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::Invoke:System.Void(P0, P1, P2, P3, P4, P5, P6)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_Invoke_System_Void_P0_P1_P2_P3_P4_P5_P6() { ITypeInfo.Invoke(null!, null!, null!, null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.ReleaseFuncDesc with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::ReleaseFuncDesc:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_ReleaseFuncDesc_System_Void_P0() { ITypeInfo.ReleaseFuncDesc(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.ReleaseTypeAttr with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::ReleaseTypeAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_ReleaseTypeAttr_System_Void_P0() { ITypeInfo.ReleaseTypeAttr(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo.ReleaseVarDesc with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::ReleaseVarDesc:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo_ReleaseVarDesc_System_Void_P0() { ITypeInfo.ReleaseVarDesc(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.AddressOfMember with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::AddressOfMember:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_AddressOfMember_System_Void_P0_P1_P2() { ITypeInfo2.AddressOfMember(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.CreateInstance with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::CreateInstance:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_CreateInstance_System_Void_P0_P1_P2() { ITypeInfo2.CreateInstance(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetAllCustData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllCustData:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetAllCustData_System_Void_P0() { ITypeInfo2.GetAllCustData(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetAllFuncCustData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllFuncCustData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetAllFuncCustData_System_Void_P0_P1() { ITypeInfo2.GetAllFuncCustData(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetAllImplTypeCustData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllImplTypeCustData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetAllImplTypeCustData_System_Void_P0_() { ITypeInfo2.GetAllImplTypeCustData(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetAllParamCustData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllParamCustData:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetAllParamCustData_System_Void_P0_P1_() { ITypeInfo2.GetAllParamCustData(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetAllVarCustData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllVarCustData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetAllVarCustData_System_Void_P0_P1() { ITypeInfo2.GetAllVarCustData(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetContainingTypeLib with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetContainingTypeLib:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetContainingTypeLib_System_Void_P0_P1() { ITypeInfo2.GetContainingTypeLib(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetCustData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetCustData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetCustData_System_Void_P0_P1() { ITypeInfo2.GetCustData(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetDllEntry with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetDllEntry:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetDllEntry_System_Void_P0_P1_P2_P3_P4() { ITypeInfo2.GetDllEntry(null!, null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetDocumentation with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetDocumentation:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetDocumentation_System_Void_P0_P1_P2_() { ITypeInfo2.GetDocumentation(null!, null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetDocumentation2 with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetDocumentation2:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetDocumentation2_System_Void_P0_P1_P2() { ITypeInfo2.GetDocumentation2(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetFuncCustData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetFuncCustData:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetFuncCustData_System_Void_P0_P1_P2() { ITypeInfo2.GetFuncCustData(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetFuncDesc with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetFuncDesc:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetFuncDesc_System_Void_P0_P1() { ITypeInfo2.GetFuncDesc(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetFuncIndexOfMemId with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetFuncIndexOfMemId:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetFuncIndexOfMemId_System_Void_P0_P1_() { ITypeInfo2.GetFuncIndexOfMemId(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetIDsOfNames with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetIDsOfNames:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetIDsOfNames_System_Void_P0_P1_P2() { ITypeInfo2.GetIDsOfNames(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetImplTypeCustData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetImplTypeCustData:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetImplTypeCustData_System_Void_P0_P1_() { ITypeInfo2.GetImplTypeCustData(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetImplTypeFlags with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetImplTypeFlags:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetImplTypeFlags_System_Void_P0_P1() { ITypeInfo2.GetImplTypeFlags(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetMops with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetMops:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetMops_System_Void_P0_P1() { ITypeInfo2.GetMops(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetNames with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetNames:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetNames_System_Void_P0_P1_P2_P3() { ITypeInfo2.GetNames(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetParamCustData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetParamCustData:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetParamCustData_System_Void_P0_P1_P2_() { ITypeInfo2.GetParamCustData(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetRefTypeInfo with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetRefTypeInfo:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetRefTypeInfo_System_Void_P0_P1() { ITypeInfo2.GetRefTypeInfo(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetRefTypeOfImplType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetRefTypeOfImplType:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetRefTypeOfImplType_System_Void_P0_P1() { ITypeInfo2.GetRefTypeOfImplType(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetTypeAttr with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetTypeAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetTypeAttr_System_Void_P0() { ITypeInfo2.GetTypeAttr(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetTypeComp with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetTypeComp:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetTypeComp_System_Void_P0() { ITypeInfo2.GetTypeComp(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetTypeFlags with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetTypeFlags:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetTypeFlags_System_Void_P0() { ITypeInfo2.GetTypeFlags(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetTypeKind with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetTypeKind:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetTypeKind_System_Void_P0() { ITypeInfo2.GetTypeKind(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetVarCustData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetVarCustData:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetVarCustData_System_Void_P0_P1_P2() { ITypeInfo2.GetVarCustData(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetVarDesc with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetVarDesc:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetVarDesc_System_Void_P0_P1() { ITypeInfo2.GetVarDesc(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.GetVarIndexOfMemId with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetVarIndexOfMemId:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_GetVarIndexOfMemId_System_Void_P0_P1() { ITypeInfo2.GetVarIndexOfMemId(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.Invoke with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::Invoke:System.Void(P0, P1, P2, P3, P4, P5, P6)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_Invoke_System_Void_P0_P1_P2_P3_P4_P5_P() { ITypeInfo2.Invoke(null!, null!, null!, null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.ReleaseFuncDesc with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::ReleaseFuncDesc:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_ReleaseFuncDesc_System_Void_P0() { ITypeInfo2.ReleaseFuncDesc(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.ReleaseTypeAttr with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::ReleaseTypeAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_ReleaseTypeAttr_System_Void_P0() { ITypeInfo2.ReleaseTypeAttr(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeInfo2.ReleaseVarDesc with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::ReleaseVarDesc:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeInfo2_ReleaseVarDesc_System_Void_P0() { ITypeInfo2.ReleaseVarDesc(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib.FindName with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::FindName:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_FindName_System_Void_P0_P1_P2_P3_P4() { ITypeLib.FindName(null!, null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib.GetDocumentation with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetDocumentation:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_GetDocumentation_System_Void_P0_P1_P2_P3() { ITypeLib.GetDocumentation(null!, null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib.GetLibAttr with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetLibAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_GetLibAttr_System_Void_P0() { ITypeLib.GetLibAttr(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib.GetTypeComp with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeComp:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_GetTypeComp_System_Void_P0() { ITypeLib.GetTypeComp(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib.GetTypeInfo with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeInfo:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_GetTypeInfo_System_Void_P0_P1() { ITypeLib.GetTypeInfo(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib.GetTypeInfoCount with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeInfoCount:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_GetTypeInfoCount_System_Void() { ITypeLib.GetTypeInfoCount(); }
    // Purpose: Benchmark native-runtime performance of ITypeLib.GetTypeInfoOfGuid with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeInfoOfGuid:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_GetTypeInfoOfGuid_System_Void_P0_P1() { ITypeLib.GetTypeInfoOfGuid(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib.GetTypeInfoType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeInfoType:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_GetTypeInfoType_System_Void_P0_P1() { ITypeLib.GetTypeInfoType(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib.IsName with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::IsName:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_IsName_System_Void_P0_P1() { ITypeLib.IsName(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib.ReleaseTLibAttr with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::ReleaseTLibAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib_ReleaseTLibAttr_System_Void_P0() { ITypeLib.ReleaseTLibAttr(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib2.FindName with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::FindName:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_FindName_System_Void_P0_P1_P2_P3_P4() { ITypeLib2.FindName(null!, null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib2.GetAllCustData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetAllCustData:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetAllCustData_System_Void_P0() { ITypeLib2.GetAllCustData(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib2.GetCustData with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetCustData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetCustData_System_Void_P0_P1() { ITypeLib2.GetCustData(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib2.GetDocumentation with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetDocumentation:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetDocumentation_System_Void_P0_P1_P2_P() { ITypeLib2.GetDocumentation(null!, null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib2.GetDocumentation2 with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetDocumentation2:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetDocumentation2_System_Void_P0_P1_P2_() { ITypeLib2.GetDocumentation2(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib2.GetLibAttr with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetLibAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetLibAttr_System_Void_P0() { ITypeLib2.GetLibAttr(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib2.GetLibStatistics with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetLibStatistics:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetLibStatistics_System_Void_P0_P1() { ITypeLib2.GetLibStatistics(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib2.GetTypeComp with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeComp:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetTypeComp_System_Void_P0() { ITypeLib2.GetTypeComp(null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib2.GetTypeInfo with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeInfo:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetTypeInfo_System_Void_P0_P1() { ITypeLib2.GetTypeInfo(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib2.GetTypeInfoCount with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeInfoCount:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetTypeInfoCount_System_Void() { ITypeLib2.GetTypeInfoCount(); }
    // Purpose: Benchmark native-runtime performance of ITypeLib2.GetTypeInfoOfGuid with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeInfoOfGuid:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetTypeInfoOfGuid_System_Void_P0_P1() { ITypeLib2.GetTypeInfoOfGuid(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib2.GetTypeInfoType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeInfoType:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_GetTypeInfoType_System_Void_P0_P1() { ITypeLib2.GetTypeInfoType(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib2.IsName with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::IsName:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_IsName_System_Void_P0_P1() { ITypeLib2.IsName(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ITypeLib2.ReleaseTLibAttr with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::ReleaseTLibAttr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComTypes_ITypeLib2_ReleaseTLibAttr_System_Void_P0() { ITypeLib2.ReleaseTLibAttr(null!); }}
