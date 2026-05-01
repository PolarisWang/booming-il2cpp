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

// Auto-generated benchmark skeletons for FamilySystem.runtime.interopservicesObjectiveCInteropBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.runtime.interopservicesObjectiveCInteropBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ObjectiveCMarshal.CreateReferenceTrackingHandle with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::CreateReferenceTrackingHandle:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_CreateReferenceTrackingHandle() { ObjectiveCMarshal.CreateReferenceTrackingHandle(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObjectiveCMarshal.Initialize with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::Initialize:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_Initialize_System_Void_P0_P1_() { ObjectiveCMarshal.Initialize(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObjectiveCMarshal.SetMessageSendCallback with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendCallback:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_SetMessageSendCallback_System() { ObjectiveCMarshal.SetMessageSendCallback(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObjectiveCMarshal.SetMessageSendPendingException with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendPendingException:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_SetMessageSendPendingExceptio() { ObjectiveCMarshal.SetMessageSendPendingException(null!); }
    // Purpose: Benchmark native-runtime performance of ObjectiveCMarshal+UnhandledExceptionPropagationHandler.ObjectiveCMarshal+UnhandledExceptionPropagationHandler: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::ObjectiveCMarshal+UnhandledExceptionPropagationHandler::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_UnhandledExceptionPropagation() { _ = ObjectiveCMarshal+UnhandledExceptionPropagationHandler.ObjectiveCMarshal+UnhandledExceptionPropagationHandler:(42, 42); }
    // Purpose: Benchmark native-runtime performance of ObjectiveCMarshal+UnhandledExceptionPropagationHandler.BeginInvoke with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::BeginInvoke:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_UnhandledExceptionPropagation() { ObjectiveCMarshal+UnhandledExceptionPropagationHandler.BeginInvoke(null!, null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObjectiveCMarshal+UnhandledExceptionPropagationHandler.EndInvoke with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::EndInvoke:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_UnhandledExceptionPropagation() { ObjectiveCMarshal+UnhandledExceptionPropagationHandler.EndInvoke(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObjectiveCMarshal+UnhandledExceptionPropagationHandler.Invoke with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::Invoke:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCMarshal_UnhandledExceptionPropagation() { ObjectiveCMarshal+UnhandledExceptionPropagationHandler.Invoke(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObjectiveCTrackedTypeAttribute.ObjectiveCTrackedTypeAttribute: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCTrackedTypeAttribute::ObjectiveCTrackedTypeAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ObjectiveC_ObjectiveCTrackedTypeAttribute_ObjectiveCTracke() { _ = ObjectiveCTrackedTypeAttribute.ObjectiveCTrackedTypeAttribute:(); }}
