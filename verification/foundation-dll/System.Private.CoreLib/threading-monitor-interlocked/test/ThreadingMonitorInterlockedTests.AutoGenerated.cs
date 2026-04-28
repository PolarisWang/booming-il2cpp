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
using Xunit;

// Auto-generated skeleton for Monitor And Interlocked (family/System.Private.CoreLib/threading/monitor-interlocked).
public partial class ThreadingMonitorInterlockedTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Enter with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Monitor_Enter_System_Void_System_Object()
    {
    // TODO: needs-manual — Enter with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Exit with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Monitor_Exit_System_Void_System_Object()
    {
    // TODO: needs-manual — Exit with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object()
    {
    var result = Monitor.TryEnter(42);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object_System_Int32()
    {
    var result = Monitor.TryEnter(42, 42);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Pulse with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Monitor_Pulse_System_Void_System_Object()
    {
    // TODO: needs-manual — Pulse with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — PulseAll with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Monitor_PulseAll_System_Void_System_Object()
    {
    // TODO: needs-manual — PulseAll with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Wait with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Monitor_Wait_System_Boolean_System_Object()
    {
    // TODO: needs-manual — Wait with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Increment with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Interlocked_Increment_System_Int32_System_Int32()
    {
    // TODO: needs-manual — Increment with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Decrement with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Interlocked_Decrement_System_Int32_System_Int32()
    {
    // TODO: needs-manual — Decrement with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Exchange with 2 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Interlocked_Exchange_System_Int32_System_Int32_System_Int32()
    {
    // TODO: needs-manual — Exchange with 2 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — CompareExchange with 3 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Interlocked_CompareExchange_System_Int32_System_Int32_System_Int32_System_Int32()
    {
    // TODO: needs-manual — CompareExchange with 3 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Add with 2 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Threading_Interlocked_Add_System_Int32_System_Int32_System_Int32()
    {
    // TODO: needs-manual — Add with 2 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Threading_Interlocked_MemoryBarrier_System_Void()
    {
    Interlocked.MemoryBarrier();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Threading.Volatile::Read:System.Int32(System.Int32&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ThreadingMonitorInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ref/pointer parameter requires unsafe context")]
    public void Method_System_Private_CoreLib_System_Threading_Volatile_Read_System_Int32_System_Int32()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }}
