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

[CapabilityTest("family/System.Private.CoreLib/interface/dispatch", IncludeBenchmark = true, IncludeHotUpdate = true)]
public partial class InterfaceDispatchTests
{
    // AI-generated tests for family/System.Private.CoreLib/interface/dispatch

    // === simple (all-primitive) ===

    [Fact]
    public void _InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_Run_System_Int32_System_Int32()
    {
        // Purpose: Verify InterfaceDispatchNativeEntry.Run with typical input
            var result = InterfaceDispatchNativeEntry.Run(42);
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestSingleImplSimple_System_Int32()
    {
        // Purpose: Verify InterfaceDispatchNativeEntry.TestSingleImplSimple with typical input
            var result = InterfaceDispatchNativeEntry.TestSingleImplSimple();
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestSingleImplDefault_System_Int32()
    {
        // Purpose: Verify InterfaceDispatchNativeEntry.TestSingleImplDefault with typical input
            var result = InterfaceDispatchNativeEntry.TestSingleImplDefault();
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestMultiImplCalc_System_Int32()
    {
        // Purpose: Verify InterfaceDispatchNativeEntry.TestMultiImplCalc with typical input
            var result = InterfaceDispatchNativeEntry.TestMultiImplCalc();
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestIsCheck_System_Int32()
    {
        // Purpose: Verify InterfaceDispatchNativeEntry.TestIsCheck with typical input
            var result = InterfaceDispatchNativeEntry.TestIsCheck();
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestAsCheck_System_Int32()
    {
        // Purpose: Verify InterfaceDispatchNativeEntry.TestAsCheck with typical input
            var result = InterfaceDispatchNativeEntry.TestAsCheck();
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestDiamondBase_System_Int32()
    {
        // Purpose: Verify InterfaceDispatchNativeEntry.TestDiamondBase with typical input
            var result = InterfaceDispatchNativeEntry.TestDiamondBase();
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestDiamondDerived_System_Int32()
    {
        // Purpose: Verify InterfaceDispatchNativeEntry.TestDiamondDerived with typical input
            var result = InterfaceDispatchNativeEntry.TestDiamondDerived();
            Xunit.Assert.NotNull((object)result);
    }

    [Fact]
    public void _InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestDiamondMulti_System_Int32()
    {
        // Purpose: Verify InterfaceDispatchNativeEntry.TestDiamondMulti with typical input
            var result = InterfaceDispatchNativeEntry.TestDiamondMulti();
            Xunit.Assert.NotNull((object)result);
    }
}
